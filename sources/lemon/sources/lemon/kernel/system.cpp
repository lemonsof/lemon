#include <cassert>
#include <lemon/kernel/system.hpp>

namespace lemon{namespace impl{

	lemon_system::lemon_system(size_t maxcoros,size_t maxchannels)
		:_seq(1)
		,_exit(false)
		,_maxcoros(maxcoros?maxcoros:LEMON_MAX_COROS)
		,_maxchannels(maxchannels?maxchannels:LEMON_MAX_CHANNELS)
	{
		_timewheel.start(this);

		_channelSystem.start(this);

		size_t Qs = std::thread::hardware_concurrency();

		for(size_t i = 0; i < Qs; ++ i)
		{
			_runqs.push_back(new lemon_runq(this));
		}
	}

	lemon_system::~lemon_system()
	{
		//TODO: add exit function

		for(auto q : _runqs)
		{
			delete q;
		}
	}

	void lemon_system::join()
	{
		for(auto q : _runqs)
		{
			q->join();
		}

		_extensionSystem.stop();

		_timewheel.join();
	}

	void lemon_system::stop() 
	{
		{
			std::unique_lock<std::mutex> lock(_activeActorsMutex);

			_exit = true;

			_condition.notify_all();

		}

		_channelSystem.stop();

		_timewheel.stop();
	}

	lemon_actor* lemon_system::dispatch()
	{
		lemon_actor* actor = nullptr;

		std::unique_lock<std::mutex> lock(_activeActorsMutex);

		while(!_exit)
		{
			if(_activeActors.empty())
			{
				_condition.wait(lock);

				continue;
			}

			//pop the actor;

			actor = _activeActors.front();

			_activeActors.pop();

			return actor;
		}

		return actor;
	}

	void lemon_system::kill_dispatch(basic_lemon_runq * runq)
	{
		for(auto a : _actors)
		{
			auto actor = a.second;

			actor->Q = runq;

			actor->Exit |= (int)exit_status::killed;

			if(actor->Mutex.mutex) actor->Mutex.lock(actor->Mutex.mutex);

			lemon_context_jump(runq->context(),actor->Context,(intptr_t)actor);

			assert(actor->Exit & (int)exit_status::exited);

			_actorFactory.close(actor);
		}

		_actors.clear();
	}

	lemon_t lemon_system::go(lemon_state S,lemon_f f, void * userdata,size_t stacksize)
	{
		lemon_actor *actor = nullptr;

		{
			std::unique_lock<std::mutex> lock(_actorMutex);

			for(;;)
			{
				lemon_t id = _seq ++ ;

				if(id != LEMON_MAIN_ACTOR_ID && id != LEMON_INVALID_HANDLE && _actors.find(id) == _actors.end())
				{
					actor = _actorFactory.create(S,id,f,userdata,stacksize);

					_actors[id] = actor;

					break;
				}
			}
		}

		
		assert(actor);

		std::unique_lock<std::mutex> lock(_activeActorsMutex);

		_activeActors.push(actor);

		_condition.notify_one();

		return actor->Id;
	}


	void lemon_system::wait_or_kill(lemon_actor * actor)
	{
		if(actor->Exit & (int)exit_status::exited)
		{
			std::unique_lock<std::mutex> lock(_actorMutex);

			_actors.erase(actor->Id);

			_actorFactory.close(actor);

			return;
		}

		if(actor->Exit & (int)exit_status::killed)
		{
			std::unique_lock<std::mutex> lock(_activeActorsMutex);

			_activeActors.push(actor);

			_condition.notify_one();

			return;
		}

		{
			std::unique_lock<std::mutex> lock(_waitingActorsMutex);

			assert(_waitingActors.find(actor->Id) == _waitingActors.end());

			_waitingActors[actor->Id] = actor;

			if(actor->Mutex.mutex) 
			{
				actor->Mutex.unlock(actor->Mutex.mutex);

				lemon_log_debug((lemon_state)actor,"unlock ...");
			}
		}

		try
		{
			if(actor->Timeout != LEMON_INFINITE)
			{
				_timewheel.create_timer(actor->Id,actor->Timeout);
			}

		}
		catch(const lemon_errno_info&)
		{
			{
				std::unique_lock<std::mutex> lock(_waitingActorsMutex);

				_waitingActors.erase(actor->Id);
			}

			{
				std::unique_lock<std::mutex> lock(_activeActorsMutex);

				_activeActors.push(actor);

				_condition.notify_one();
			}

			lemon_raise_trace((lemon_state)actor);
		}
	}

	bool lemon_system::notify(lemon_t target, const lemon_event_t * waitlist, size_t len)
	{
		lemon_actor * actor = nullptr;

		{
			std::unique_lock<std::mutex> lock(_waitingActorsMutex);

			auto iter = _waitingActors.find(target);

			if(iter == _waitingActors.end()) return false;

			actor = iter->second;

			assert(actor->WaitResult == LEMON_INVALID_HANDLE);

			for(size_t i = 0; i < len; ++ i)
			{
				for(size_t j = 0; j < actor->Waits; j ++ )
				{
					if(waitlist[i] == actor->WaitList[j])
					{
						actor->WaitResult = waitlist[i];

						_waitingActors.erase(iter);

						break;
					}
				}
			}

		}
		

		if(actor->WaitResult  != LEMON_INVALID_HANDLE)
		{
			std::unique_lock<std::mutex> lock(_activeActorsMutex);

			_activeActors.push(actor);

			_condition.notify_one();

			return true;
		}

		return false;
	}

	bool lemon_system::notify_timeout(lemon_t target)
	{
		lemon_actor * actor = nullptr;

		{
			std::unique_lock<std::mutex> lock(_waitingActorsMutex);

			auto iter = _waitingActors.find(target);

			if(iter == _waitingActors.end()) return false;

			if(iter->second->Timeout == LEMON_INFINITE) return false;

			actor = iter->second;

			actor->WaitResult = LEMON_INVALID_HANDLE;

			_waitingActors.erase(iter);
		}

		std::unique_lock<std::mutex> lock(_activeActorsMutex);

		_activeActors.push(actor);

		_condition.notify_one();

		return true;
	}
}}


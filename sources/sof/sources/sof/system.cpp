#include <cassert>
#include <sof/system.hpp>

namespace sof{namespace impl{

	sof_system::sof_system(size_t maxcoros,size_t maxchannels)
		:_seq(1)
		,_exit(false)
		,_maxcoros(maxcoros?maxcoros:SOF_MAX_COROS)
		,_maxchannels(maxchannels?maxchannels:SOF_MAX_CHANNELS)
	{
		size_t Qs = std::thread::hardware_concurrency();

		for(size_t i = 0; i < Qs; ++ i)
		{
			_runqs.push_back(new sof_runq(this));
		}
	}

	sof_system::~sof_system()
	{
		//TODO: add exit function

		for(auto q : _runqs)
		{
			delete q;
		}
	}

	void sof_system::join()
	{
		for(auto q : _runqs)
		{
			q->join();
		}

		_extensionSystem.stop();
	}

	void sof_system::stop() 
	{
		std::unique_lock<std::mutex> lock(_activeActorsMutex);

		_exit = true;

		_condition.notify_all();
	}

	sof_actor* sof_system::dispatch()
	{
		sof_actor* actor = nullptr;

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

	sof_t sof_system::go(sof_state S,sof_f f, void * userdata,size_t stacksize)
	{
		sof_actor *actor = nullptr;

		{
			std::unique_lock<std::mutex> lock(_actorMutex);

			for(;;)
			{
				sof_t id = _seq ++ ;

				if(id != 0 && id != SOF_INVALID_HANDLE && _actors.find(id) == _actors.end())
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


	void sof_system::wait_or_kill(sof_actor * actor)
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

			return;
		}

		std::unique_lock<std::mutex> lock(_waitingActorsMutex);

		if(actor->Mutex.mutex) actor->Mutex.unlock(actor->Mutex.mutex);

		assert(_waitingActors.find(actor->Id) == _waitingActors.end());

		_waitingActors[actor->Id] = actor;
	}

	bool sof_system::notify(sof_t target, const sof_event_t * waitlist, size_t len)
	{
		sof_actor * actor = nullptr;

		{
			std::unique_lock<std::mutex> lock(_waitingActorsMutex);

			auto iter = _waitingActors.find(target);

			if(iter == _waitingActors.end()) return false;

			actor = iter->second;

			assert(actor->WaitResult == SOF_INVALID_HANDLE);

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
		

		if(actor->WaitResult  != SOF_INVALID_HANDLE)
		{
			std::unique_lock<std::mutex> lock(_activeActorsMutex);

			_activeActors.push(actor);

			return true;
		}

		return false;
	}
}}


#include <thread>
#include <lemon/kernel/system.hpp>

namespace lemon{namespace kernel{

	lemon_system::lemon_system(size_t maxcoros,size_t maxchannels)
		:_maxcoros(maxcoros)
		,_maxchannels(maxchannels)
		,_exit(false)
		,_seq(0)
	{
		_mainActor.set_system(this);

		_timewheel.start(this);

		_actorSystem.start(this);

		_extensionSystem.start(this);

		_channelSystem.start(this);

		size_t Qs = std::thread::hardware_concurrency();

		for(size_t i = 0; i < Qs; ++ i)
		{
			_runqs.push_back(new lemon_runq(this));
		}
	}

	lemon_system::~lemon_system()
	{
		stop();

		join();

		for(auto q : _runqs)
		{
			delete q;
		}

		lemon_context_t context;

		lemon_log_debug(_traceSystem,_mainActor,"lemon exit dispatch ...");

		for(auto id : _actors)
		{
			auto actor = lemon_actor::from(id);

			actor->kill();

			actor->parent_reset(&context);

			lemon_context_jump(&context,*actor,(intptr_t)actor);

			assert(actor->exited());

			_actorSystem.close(actor);
		}

		lemon_log_debug(_traceSystem,_mainActor,"lemon exit dispatch -- success");
	}

	void lemon_system::join()
	{
		for(auto q : _runqs)
		{
			q->join();
		}

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

		_extensionSystem.stop();

		_timewheel.stop();
	}

	lemon_t lemon_system::go(lemon_t source,lemon_f f, void * userdata,size_t stacksize)
	{
		lemon_actor *actor = nullptr;

		{
			std::unique_lock<std::mutex> lock(_actorsMutex);

			actor = _actorSystem.create(source,f,userdata,stacksize);

			_actors.insert(*actor);
		}


		assert(actor);

		std::unique_lock<std::mutex> lock(_activeActorsMutex);

		_activeActors.push_back(actor);

		_condition.notify_one();

		return *actor;
	}

	lemon_actor* lemon_system::dispatch_one()
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

			_activeActors.pop_front();

			return actor;
		}

		return actor;
	}

	void lemon_system::wait_or_kill(lemon_actor * actor)
	{
		if(actor->exited())
		{
			std::unique_lock<std::mutex> lock(_actorsMutex);

			_actors.erase(*actor);

			_actorSystem.close(actor);

			return;
		}

		if(actor->killed())
		{
			std::unique_lock<std::mutex> lock(_activeActorsMutex);

			_activeActors.push_back(actor);

			_condition.notify_one();

			return;
		}

		
		
		try
		{
			std::unique_lock<std::mutex> lock(_waitingActorsMutex);

			if(actor->timeout() != lemon_infinite)
			{
				_timewheel.create_timer(*actor,actor->timeout());
			}

			//no throw promise
			assert(_waitingActors.count(*actor) == 0);

			_waitingActors.insert(*actor);

			actor->unlock();

		}
		catch(const lemon_errno_info& e)
		{
			lemon_log_error(_traceSystem,*actor,"create waiting timer failed !!!");

			actor->lasterror_reset(e);

			std::unique_lock<std::mutex> lock(_activeActorsMutex);

			_activeActors.push_back(actor);

			_condition.notify_one();
		}
	}

	bool lemon_system::notify_timeout(lemon_t target)
	{
		lemon_actor * actor = lemon_actor::from(target);

		{
			std::unique_lock<std::mutex> lock(_waitingActorsMutex);

			if(_waitingActors.count(target) == 0) return false;

			actor->notified_reset();

			_waitingActors.erase(target);
		}


		std::unique_lock<std::mutex> lock(_activeActorsMutex);

		_activeActors.push_back(actor);

		_condition.notify_one();

		return true;
	}

	bool lemon_system::notify(lemon_t target,const const_buff<lemon_event_t> & events)
	{

		lemon_actor * actor = lemon_actor::from(target);

		{
			std::unique_lock<std::mutex> lock(_waitingActorsMutex);

			if(_waitingActors.count(target) == 0) return false;

			assert(actor->notified() == LEMON_INVALID_HANDLE(lemon_event_t));


			for(size_t i = 0; i < events.length; ++ i)
			{
				for(size_t j = 0; j < actor->events().length; j ++ )
				{
					if(events.buff[i] == actor->events().buff[j])
					{
						actor->notified_reset(j);

						_waitingActors.erase(target);

						break;
					}
				}
			}
		}

		if(actor->notified() != LEMON_INVALID_HANDLE(lemon_event_t))
		{
			std::unique_lock<std::mutex> lock(_activeActorsMutex);

			_activeActors.push_back(actor);

			_condition.notify_one();

			return true;
		}

		return false;
	}

}}
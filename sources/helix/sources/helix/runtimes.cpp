#include <cassert>
#include <helix/assembly.h>
#include <helix/runtimes.hpp>

namespace helix{ namespace impl{

	runtimes::runtimes(helix_alloc_t * alloc)
		:_status(true),_seq(1),_alloc(alloc)
	{
		_timewheel.start(this);

		_main_runq.start(this);

		_actors[_main_actor.handle()] = &_main_actor;

		size_t cpus = std::thread::hardware_concurrency();

		for(size_t i = 0; i < cpus; ++ i)
		{
			_runqs.push_back(new(alloc) runq(this));
		}
	}

	void runtimes::sleep(basic_actor_t * actor)
	{
		std::unique_lock<std::mutex> lock(_mutex);

		if(actor->exited())
		{
			_actors.erase(actor->handle());

			release(actor,_alloc);
			return;
		}

		if(_status == false || actor->status() == actor_status::stopped)
		{
			actor->status(actor_status::stopped);

			actor->Q()->dispatch_one(actor);

			return;
		}

		actor->status(actor_status::sleeping);

		if(actor->poll_first_event())
		{
			actor->Q()->dispatch_one(actor);

			return;
		}

		
	}

	uintptr_t runtimes::create_go(void(*f)(helix_t, void*), void* userdata,size_t stacksize)
	{
		std::unique_lock<std::mutex> lock(_mutex);

		if(_status == false)
		{
			helix_declare_errcode(errorCode);

			helix_user_errno(errorCode,HELIX_EXIT_ERROR);

			throw errorCode;
		}

		for(;;)
		{
			uintptr_t handle = (uintptr_t)_seq ++;

			if(_actors.count(handle) == 0 && handle != HELIX_INVALID_HANDLE)
			{
				basic_actor_t * actor = new(_alloc) actor_t(this,f,userdata,stacksize,handle);

				_actors[handle] = actor;

				balance_dispatch(actor);

				return handle;
			}
		}
	}

	void runtimes::balance_dispatch(basic_actor_t * actor)
	{
		size_t id = actor->handle() % (_runqs.size() + 1);

		if(id == 0)
		{
			_main_runq.dispatch_one(actor);
		}
		else
		{
			_runqs[-- id]->dispatch_one(actor);
		}
	}

	bool runtimes::__notify(basic_actor_t * actor,uintptr_t eventid)
	{
		if (actor->notify(eventid))
		{
			if (actor->status() == actor_status::sleeping)
			{
				if (!actor->poll_first_event())
				{
					assert(false && "not here");
				}

				actor->Q()->dispatch_one(actor);
			}

			return true;
		}

		return false;
	}

	void runtimes::notify(uintptr_t target,uintptr_t eventid)
	{
		std::unique_lock<std::mutex> lock(_mutex);

		auto iter = _actors.find(target);

		if(iter != _actors.end())
		{
			__notify(iter->second,eventid);
		}
	}

	void runtimes::notify_all(uintptr_t eventid)
	{
		std::unique_lock<std::mutex> lock(_mutex);

		auto range = _eventwaiters.equal_range(eventid);

		std::for_each(range.first,range.second,[=](event_waiters::value_type & val){
			__notify(val.second,eventid);
		});
	}
        
        void runtimes::notify_one(uintptr_t eventid)
	{
		std::unique_lock<std::mutex> lock(_mutex);

		auto range = _eventwaiters.equal_range(eventid);
                
                for(auto iter = range.first; iter != range.second; ++ iter){
                    if(__notify(iter->second,eventid)){
                        break;
                    }
                }
	}
} }
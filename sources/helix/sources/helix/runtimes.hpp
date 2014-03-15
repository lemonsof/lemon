/**
* 
* @file     runtimes
* @brief    Copyright (C) 2014  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2014/02/08
*/
#ifndef HELIX_RUNTIMES_HPP
#define HELIX_RUNTIMES_HPP
#include <vector>
#include <algorithm>
#include <helix/abi.h>
#include <helix/actor.hpp>
#include <helix/object.hpp>
#include <helix/runq.hpp>
#include <unordered_map>
#include <helix/timewheel.hpp>

namespace helix{ namespace impl{

	class runtimes : public object
	{
	public:

		typedef std::unordered_multimap<uintptr_t,basic_actor_t*> event_waiters;
		
		runtimes(helix_alloc_t * alloc);

		main_actor_t * main_actor() { return &_main_actor; }

		helix_alloc_t* alloc(){ return _alloc; }

		void sleep(basic_actor_t * actor);

		uintptr_t create_go(void(*f)(helix_t, void*), void* userdata,size_t stacksize);

		void notify(uintptr_t target,uintptr_t eventid);

		void notify_all(uintptr_t eventid);

		void notify_timeout(uintptr_t target)
		{
			notify(target,timewheel_t::event());
		}

		void add_event(basic_actor_t * actor,helix_event * event)
		{
			std::unique_lock<std::mutex> lock(_mutex);

			if(actor->add_event(event))
			{
				_eventwaiters.insert(std::make_pair(event->id,actor));
			}
		}

		helix_event* remove_event(basic_actor_t * actor,uintptr_t eventid)
		{
			std::unique_lock<std::mutex> lock(_mutex);

			helix_event* result = actor->remove_event(eventid);

			if(result != nullptr)
			{
				auto range = _eventwaiters.equal_range(eventid);

				auto iter = std::find_if(range.first,range.second,[=](event_waiters::value_type & val){
					return val.second == actor;
				});


				if(range.second != iter)
				{
					_eventwaiters.erase(iter);
				}
			}

			return result;
		}

		void clear_events(basic_actor_t * actor)
		{
			std::unique_lock<std::mutex> lock(_mutex);

			actor->clear_events();
		}

		timewheel_t & timewheel(){ return _timewheel; }

	private:

		bool __notify(basic_actor_t * actor,uintptr_t eventid);

		void balance_dispatch(basic_actor_t * actor);

	private:
		bool												_status;
		volatile uintptr_t									_seq;
		helix_alloc_t										*_alloc;
		main_actor_t										_main_actor;
		main_runq											_main_runq;
		std::vector<runq*>									_runqs;
		std::unordered_map<uintptr_t,basic_actor_t*>		_actors;
		std::mutex											_mutex;
		timewheel_t											_timewheel;
		event_waiters										_eventwaiters;
	};

} }

#endif	//HELIX_RUNTIMES_HPP
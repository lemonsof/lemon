/**
* 
* @file     system
* @brief    Copyright (C) 2013  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2013/08/07
*/
#ifndef LEMON_SYSTEM_HPP
#define LEMON_SYSTEM_HPP

#include <queue>
#include <mutex>
#include <thread>
#include <lemon/abi.h>
#include <unordered_map>
#include <condition_variable>
#include <lemon/kernel/runq.hpp>
#include <lemon/kernel/actor.hpp>
#include <lemonxx/nocopyable.hpp>
#include <lemonxx/error_info.hpp>
#include <lemon/kernel/dtrace.hpp>
#include <lemon/kernel/timewheel.hpp>
#include <lemon/kernel/extensions.hpp>




namespace lemon{namespace impl{

	struct lemon_actor;

	class basic_lemon_runq;

	class lemon_system : private nocopyable
	{
	public:
		friend class main_lemon_runq;

		typedef std::vector<basic_lemon_runq*>				runqs;
		
		typedef std::unordered_map<lemon_t,lemon_actor*>		actors_t;

		typedef std::queue<lemon_actor*>						active_actors;

		lemon_system(size_t maxcoros,size_t maxchannels);

		~lemon_system();

	public:

		void stop();

		lemon_actor* dispatch();

		void kill_dispatch(basic_lemon_runq * runq);

		void wait_or_kill(lemon_actor * actor);

		actors_t & actors() { return _actors; }

		lemon_trace_system & trace_system(){ return _traceSystem; }

		lemon_actor_factory & actor_factory() { return _actorFactory; }

		void new_extension(const lemon_extension_vtable * vtable, void * userdata)
		{
			_extensionSystem.new_extension((lemon_state)get_main_runq(),vtable,userdata);
		}

		lemon_t go(lemon_state S,lemon_f f, void * userdata,size_t stacksize);

		bool notify(lemon_t target, const lemon_event_t * waitlist, size_t len);

		bool notify_timeout(lemon_t target);

		lemon_event_t timer_event()
		{
			return (lemon_event_t)&_timewheel;
		}

		void set_main_runq(basic_lemon_runq * Q){ _mainRunQ = Q; }

		basic_lemon_runq * get_main_runq() { return _mainRunQ; }
	private:

		void join();

	private:

		basic_lemon_runq								*_mainRunQ;

		lemon_t											_seq;

		bool											_exit;

		std::mutex										_actorMutex;

		std::mutex										_extensionMutex;

		std::mutex										_activeActorsMutex;

		std::mutex										_waitingActorsMutex;

		std::condition_variable							_condition;

		size_t											_maxcoros;

		size_t											_maxchannels;
		
		actors_t										_actors;

		actors_t										_waitingActors;

		active_actors									_activeActors;

		runqs											_runqs;

		lemon_trace_system								_traceSystem;

		lemon_actor_factory								_actorFactory;

		lemon_extension_system							_extensionSystem;

		lemon_timewheel									_timewheel;
	};
}}


#endif //LEMON_SYSTEM_HPP

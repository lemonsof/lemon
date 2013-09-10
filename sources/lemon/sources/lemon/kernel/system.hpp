/**
* 
* @file     system
* @brief    Copyright (C) 2013  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2013/08/21
*/
#ifndef LEMON_SYSTEM_HPP
#define LEMON_SYSTEM_HPP

#include <list>
#include <mutex>
#include <lemon/abi.h>
#include <unordered_set>
#include <lemon/assembly.h>
#include <condition_variable>
#include <lemon/kernel/runq.hpp>
#include <lemonxx/nocopyable.hpp>
#include <lemon/kernel/dtrace.hpp>
#include <lemon/kernel/timewheel.hpp>
#include <lemon/kernel/extensions.hpp>
#include <lemon/kernel/actor-system.hpp>
#include <lemon/kernel/channel-system.hpp>
#include <lemon/kernel/io_system_iocp.hpp>


#ifdef lemon_log
#undef lemon_log
#define lemon_log(sysm,level,...) if(sysm.status(level)){ sysm.trace(level,__VA_ARGS__);}
#endif

namespace lemon{namespace kernel{

	class lemon_system : private nocopyable
	{
	public:

		typedef std::unordered_set<lemon_t>					actors_type;

		typedef std::list<lemon_actor*>						actors_queue_type;	

		typedef std::vector<lemon_runq*>					runqs_t;

		lemon_system(size_t maxcoros,size_t maxchannels,size_t stacksize);

		~lemon_system();

		void join();

		void stop();

		bool exited() const { return _exit; }

	public:

		lemon_actor * main_actor() { return _mainRunq; }

		lemon_man_runq* main_runq() { return &_mainRunq;} 

		lemon_trace_system  & trace_system(){ return _traceSystem; }

		void new_extension(lemon_t source,const lemon_extension_vtable * vtable, void * userdata)
		{
			_extensionSystem.new_extension(source,vtable,userdata);
		}

		lemon_channel_system & channel_system() { return _channelSystem; }

		lemon_io_system& io_system() { return _ioSystem; }

	public:

		lemon_actor* dispatch_one();

		void wait_or_kill(lemon_actor * actor);

		bool notify_timeout(lemon_t target);

		bool notify(lemon_t target,const const_buff<lemon_event_t> & events);

		lemon_t go(lemon_t source,lemon_f f, void * userdata,size_t stacksize);

	private:

		std::mutex											_waitingActorsMutex;

		std::condition_variable								_condition;

		std::mutex											_activeActorsMutex;

		std::mutex											_actorsMutex;

		size_t												_maxcoros;
		
		size_t												_maxchannels;

		lemon_man_runq										_mainRunq;

		bool												_exit;

		uintptr_t											_seq;

		lemon_timewheel										_timewheel;

		lemon_trace_system									_traceSystem;

		lemon_actor_system									_actorSystem;

		lemon_extension_system								_extensionSystem;

		lemon_channel_system								_channelSystem;

		//lemon_io_system										_ioSystem;
		
		actors_type											_actors;

		actors_queue_type									_activeActors;

		actors_type											_waitingActors;

		runqs_t												_runqs;
	};

}}

#endif //LEMON_SYSTEM_HPP
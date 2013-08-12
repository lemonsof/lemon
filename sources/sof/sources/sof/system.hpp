/**
* 
* @file     system
* @brief    Copyright (C) 2013  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2013/08/07
*/
#ifndef SOF_SYSTEM_HPP
#define SOF_SYSTEM_HPP

#include <queue>
#include <mutex>
#include <thread>
#include <sof/abi.h>
#include <sof/runq.hpp>
#include <sof/actor.hpp>
#include <unordered_map>
#include <sof/dtrace.hpp>
#include <condition_variable>
#include <sof/extensions.hpp>
#include <sofxx/nocopyable.hpp>
#include <sofxx/error_info.hpp>


namespace sof{namespace impl{

	struct sof_actor;

	class basic_sof_runq;

	class sof_system : private nocopyable
	{
	public:
		friend class main_sof_runq;

		typedef std::vector<basic_sof_runq*>				runqs;
		
		typedef std::unordered_map<sof_t,sof_actor*>		actors_t;

		typedef std::queue<sof_actor*>						active_actors;

		sof_system(size_t maxcoros,size_t maxchannels);

		~sof_system();

	public:

		void stop();

		sof_actor* dispatch();

		void wait_or_kill(sof_actor * actor);

		actors_t & actors() { return _actors; }

		sof_trace_system & trace_system(){ return _traceSystem; }

		sof_t go(sof_state S,sof_f f, void * userdata,size_t stacksize);

		bool notify(sof_t target, const sof_event_t * waitlist, size_t len);

	private:

		void join();

	private:

		sof_t											_seq;

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

		sof_trace_system								_traceSystem;

		sof_actor_factory								_actorFactory;

		sof_extension_system							_extensionSystem;
	};
}}


#endif //SOF_SYSTEM_HPP
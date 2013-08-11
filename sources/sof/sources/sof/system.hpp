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
#include <unordered_map>
#include <sof/dtrace.hpp>
#include <condition_variable>
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

		void stop();

		sof_actor* dispatch();

	public:

		sof_trace_system & trace_system(){ return _traceSystem; }

	private:

		void join() const;

	private:

		bool											_exit;

		std::mutex										_mutex;

		std::mutex										_extensionMutex;

		std::condition_variable							_condition;

		size_t											_maxcoros;

		size_t											_maxchannels;
		
		actors_t										_actors;

		actors_t										_waitingActors;

		active_actors									_activeActors;

		runqs											_runqs;

		sof_trace_system								_traceSystem;
	};
}}


#endif //SOF_SYSTEM_HPP
/**
* 
* @file     runq
* @brief    Copyright (C) 2013  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2013/08/09
*/
#ifndef SOF_RUNQ_HPP
#define SOF_RUNQ_HPP

#include <mutex>
#include <thread>
#include <stdarg.h>
#include <sof/actor.hpp>
#include <sofxx/sof.hpp>

namespace sof{namespace impl{

	class sof_system;

	class basic_sof_runq 
	{
	public:
		
		basic_sof_runq(sof_system * system):_system(system){ SOF_RESET_ERRORINFO(_lasterror); }

		virtual ~basic_sof_runq(){ }

		sof_system * sysm() { return _system; }

		virtual void join() const {}

		void stop();

		virtual sof_t id() = 0;

	public:

		const sof_errno_info* last_errno() const;

		void raise_error(sof_state S,const char* msg ,const sof_errno_info* info);

		void raise_trace(sof_state S,const char * file, int lines);

		void reset_errno();

		void trace( dtrace::level level,const char * fmt,va_list arg);
	private:

		sof_errno_info						_lasterror;							

		sof_system							*_system;
	};


	class main_sof_runq : public basic_sof_runq
	{
	public:
		
		main_sof_runq(size_t maxcoros,size_t maxchannels);

		~main_sof_runq();

		void join();

		void stop();

		sof_t id() { return SOF_MAIN_ACTOR_ID; }
	};

	class sof_runq : public basic_sof_runq
	{
	public:
		
		sof_runq(sof_system * system);

		~sof_runq();

		void join();

		sof_t id() { return _current->id; }

	private:

		void proc();

	private:

		sof_actor											*_current;

		std::thread											_worker;
	};
}}

#endif //SOF_RUNQ_HPP
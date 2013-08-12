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
#include <assert.h>
#include <stdarg.h>
#include <sof/actor.hpp>
#include <sofxx/dtrace.hpp>
#include <sofxx/error_info.hpp>

namespace sof{namespace impl{

	class sof_system;

	class basic_sof_runq 
	{
	public:
		
		basic_sof_runq(sof_system * system):_system(system){ SOF_RESET_ERRORINFO(_lasterror); }

		virtual ~basic_sof_runq(){ }

		sof_t id() const
		{
			assert(_current);

			return _current->Id;
		}

		void stop();

		void set_current(sof_actor * current) 
		{ 
			_current = current; _current->Q = this; 
		}

		sof_actor* get_current() { return _current;}

		sof_system * sysm() { return _system; }

	public:

		virtual void join() = 0;

	public:

		void reset_errno();

		const sof_errno_info* last_errno() const;

		sof_context_t* context() { return &_context; }

		void raise_trace(const char * file, int lines);

		sof_t go(sof_f f, void * userdata,size_t stacksize);

		void raise_error(const char* msg ,const sof_errno_info* info);

		void trace( dtrace::level level,const char * fmt,va_list arg);

		bool notify(sof_t target, const sof_event_t * waitlist, size_t len);

		sof_event_t wait(const sof_mutext_t * mutex,const sof_event_t * waitlist, size_t len);

	protected:

		void dispatch();

	private:

		sof_actor										*_current;

		sof_errno_info									_lasterror;							

		sof_system										*_system;

		sof_context_t									_context;
	};


	class main_sof_runq : public basic_sof_runq
	{
	public:
		
		main_sof_runq(size_t maxcoros,size_t maxchannels);

		~main_sof_runq();

		void join();

	private:

		sof_actor										_mainActor;
	};

	class sof_runq : public basic_sof_runq
	{
	public:
		
		sof_runq(sof_system * system);

		~sof_runq();

		void join();

	private:

		void proc();

	private:

		sof_actor											*_current;

		std::thread											_worker;

		sof_context_t										_context;
	};
}}

#endif //SOF_RUNQ_HPP
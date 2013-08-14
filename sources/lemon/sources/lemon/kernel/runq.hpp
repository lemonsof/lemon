/**
* 
* @file     runq
* @brief    Copyright (C) 2013  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2013/08/09
*/
#ifndef LEMON_RUNQ_HPP
#define LEMON_RUNQ_HPP

#include <mutex>
#include <thread>
#include <assert.h>
#include <stdarg.h>
#include <lemonxx/dtrace.hpp>
#include <lemonxx/error_info.hpp>
#include <lemon/kernel/actor.hpp>

namespace lemon{namespace impl{

	class lemon_system;

	class basic_lemon_runq 
	{
	public:
		
		basic_lemon_runq(lemon_system * system):_system(system){ LEMON_RESET_ERRORINFO(_lasterror); }

		virtual ~basic_lemon_runq(){ }

		lemon_t id() const
		{
			assert(_current);

			return _current->Id;
		}

		void stop();

		void set_current(lemon_actor * current) 
		{ 
			_current = current; _current->Q = this; 
		}

		lemon_actor* get_current() { return _current;}

		lemon_system * sysm() { return _system; }

	public:

		virtual void join() = 0;

	public:

		lemon_context_t* context() { return &_context; }

		lemon_t go(lemon_f f, void * userdata,size_t stacksize);

		void trace( dtrace::level level,const char * fmt,va_list arg);

		bool notify(lemon_t target, const lemon_event_t * waitlist, size_t len);

		void new_extension(const lemon_extension_vtable * vtable, void * userdata);

		void sleep(size_t milliseconds);

	protected:

		void dispatch();

	private:

		lemon_actor										*_current;

		lemon_errno_info								_lasterror;							

		lemon_system									*_system;

		lemon_context_t									_context;
	};


	class main_lemon_runq : public basic_lemon_runq
	{
	public:
		
		main_lemon_runq(size_t maxcoros,size_t maxchannels);

		~main_lemon_runq();

		void join();

	private:

		lemon_actor										_mainActor;
	};

	class lemon_runq : public basic_lemon_runq
	{
	public:
		
		lemon_runq(lemon_system * system);

		~lemon_runq();

		void join();

	private:

		void proc();

	private:

		lemon_actor											*_current;

		std::thread											_worker;

		lemon_context_t										_context;
	};
}}

#endif //LEMON_RUNQ_HPP

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
		
		basic_lemon_runq(lemon_system * system):_system(system){ }

		virtual ~basic_lemon_runq(){ }

		void stop();

		lemon_system * sysm() { return _system; }

		lemon_context_t * context() { return &_context; }

	public:

		virtual void join() = 0;

	protected:

		void dispatch();

	private:						

		lemon_system									*_system;

		lemon_context_t									_context;
	};


	class main_lemon_runq : public basic_lemon_runq
	{
	public:
		
		main_lemon_runq(size_t maxcoros,size_t maxchannels);

		~main_lemon_runq();

		void join();

		lemon_actor * main_actor() { return &_mainActor; }

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

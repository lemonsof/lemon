/**
* 
* @file     actor-system
* @brief    Copyright (C) 2013  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2013/08/21
*/
#ifndef LEMON_KERNEL_ACTOR_SYSTEM_HPP
#define LEMON_KERNEL_ACTOR_SYSTEM_HPP
#include <mutex>
#include <condition_variable>
#include <lemon/kernel/actor.hpp>

namespace lemon{namespace kernel{

	class lemon_system;

	class lemon_actor_system : private nocopyable
	{
	public:

		void start(lemon_system * sysm){ _sysm = sysm; }

		void stop(){}

		lemon_actor* create(lemon_t source,lemon_f f, void * userdata,size_t stacksize);

		void close(lemon_actor* actor);

	private:

		lemon_system										*_sysm;
	};


	class lemon_man_runq : private nocopyable
	{
	public:
		lemon_man_runq();

		~lemon_man_runq();

		void start(lemon_system * sysm,size_t stacksize);

		operator lemon_actor* ()
		{
			return &_mainActor;
		}

		void notify();

		void stop()
		{
			_mainActor.killed();
		}

		void join();

	private:

		static void __f(lemon_man_runq * runq)
		{
			runq->proc();
		}

		void proc();

	private:
		bool											_exit;

		std::mutex										_mutex;

		std::condition_variable							_condition;

		lemon_system									*_sysm;

		lemon_context_t									*_context;

		lemon_context_t									_mainContext;

		lemon_actor										_mainActor;
	};

}}

#endif //LEMON_KERNEL_ACTOR_SYSTEM_HPP
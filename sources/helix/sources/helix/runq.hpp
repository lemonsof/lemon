/**
* 
* @file     runq
* @brief    Copyright (C) 2014   All Rights Reserved 
* @author   
* @version  1.0.0.0  
* @date     2014/02/08
*/
#ifndef HELIX_RUNQ_HPP
#define HELIX_RUNQ_HPP
#include <queue>
#include <mutex>
#include <thread>
#include <atomic>
#include <helix/stack.hpp>
#include <helix/actor.hpp>
#include <helix/object.hpp>
#include <condition_variable>
#include <helix/context/fcontext.h>

namespace helix{ namespace impl{
	
	class runtimes;

	class basic_runq : public object
	{
	public:
		
		basic_runq();

		runtimes * rt(){ return _runtimes; }

		helix_context_t* context()
		{
			return &_context;
		}

		void dispatch_one(basic_actor_t * actor);

		void dispatch_loop();

		void exit();

	protected:

		void rt(runtimes * rt){_runtimes = rt;}

		void context(helix_context_t * ctx);

	private:
		
		runtimes					*_runtimes;

		volatile bool				_status;

		helix_context_t				_context;

		std::queue<basic_actor_t*>		_runq;

		std::mutex					_mutex;
		
		std::condition_variable		_condition;
	};

	class runq : public basic_runq
	{
	public:
		runq(runtimes * rt);

	private:
		std::thread					_thread;
	};


	class main_runq : public basic_runq
	{
	public:
		void start(runtimes * rt);
	private:
		static void dispatch_loop(basic_actor_t * actor);
	private:
		
		basic_stack					_stack;

		helix_context_t				_context;
	};
} }

#endif	//HELIX_RUNQ_HPP
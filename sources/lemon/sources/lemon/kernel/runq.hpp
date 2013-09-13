/**
* 
* @file     runq
* @brief    Copyright (C) 2013  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2013/08/22
*/
#ifndef LEMON_KERNEL_RUNQ_HPP
#define LEMON_KERNEL_RUNQ_HPP
#include <thread>
#include <lemon/abi.h>
#include <lemonxx/nocopyable.hpp>
#include <lemon/context/fcontext.h>

namespace lemon{namespace kernel{

	class lemon_system;

	class lemon_runq : private nocopyable
	{
	public:
		lemon_runq(lemon_system * sysm);

		~lemon_runq();

		bool join();

		operator lemon_context_t* ()
		{
			return &_context;
		}

	private:

		void proc();

	private:

		lemon_system									*_sysm;

		std::thread										_worker;

		lemon_context_t									_context;
	};

}}

#endif //LEMON_KERNEL_RUNQ_HPP
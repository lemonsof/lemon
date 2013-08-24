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

}}

#endif //LEMON_KERNEL_ACTOR_SYSTEM_HPP
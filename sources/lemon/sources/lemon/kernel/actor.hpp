/**
* 
* @file     actor
* @brief    Copyright (C) 2013  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2013/08/09
*/
#ifndef LEMON_ACTOR_HPP
#define LEMON_ACTOR_HPP

#include <lemonxx/actor.hpp>
#include <lemonxx/nocopyable.hpp>
#include <lemon/context/fcontext.h>

namespace lemon{namespace impl{

	class basic_lemon_runq;

	enum class exit_status
	{
		killed = 0x01,exited = 0x02
	};

	class lemon_system;

	struct lemon_actor : private nocopyable
	{
		lemon_actor();

		lemon_actor(lemon_system * system,lemon_t id,lemon_f f, void * userdata,lemon_context_t* context);

		lemon_t									Id;

		actor_status							Status;

		lemon_context_t							*Context;

		lemon_f									F;

		void									*Userdata;

		basic_lemon_runq						*Q;

		lemon_system							*System;

		lemon_mutext_t							Mutex;

		int										Exit;

		const lemon_event_t						*WaitList;

		size_t									Waits;

		lemon_event_t							WaitResult;

		size_t									Timeout;

		lemon_errno_info						LastError;
	};

	class lemon_actor_factory : private nocopyable
	{
	public:

		lemon_actor * create(lemon_state S, lemon_t id, lemon_f f, void * userdata,size_t stacksize);

		void close(lemon_actor * actor);
	};
}}

#endif //LEMON_ACTOR_HPP

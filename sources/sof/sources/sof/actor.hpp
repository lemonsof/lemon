/**
* 
* @file     actor
* @brief    Copyright (C) 2013  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2013/08/09
*/
#ifndef SOF_ACTOR_HPP
#define SOF_ACTOR_HPP

#include <sofxx/actor.hpp>
#include <sofxx/nocopyable.hpp>
#include <sof/context/fcontext.h>

namespace sof{namespace impl{

	class basic_sof_runq;

	enum class exit_status
	{
		killed = 0x01,exited = 0x02
	};

	struct sof_actor : private nocopyable
	{
		sof_actor();

		sof_actor(sof_t id,sof_f f, void * userdata,sof_context_t* context);

		sof_t									Id;

		actor_status							Status;

		sof_context_t							*Context;

		sof_f									F;

		void									*Userdata;

		basic_sof_runq							*Q;

		sof_mutext_t							Mutex;

		int										Exit;

		const sof_event_t						*WaitList;

		size_t									Waits;

		sof_event_t								WaitResult;
	};

	class sof_actor_factory : private nocopyable
	{
	public:

		sof_actor * create(sof_state S, sof_t id, sof_f f, void * userdata,size_t stacksize);

		void close(sof_actor * actor);
	};
}}

#endif //SOF_ACTOR_HPP
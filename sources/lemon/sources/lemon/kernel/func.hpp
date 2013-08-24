/**
* 
* @file     func
* @brief    Copyright (C) 2013  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2013/08/22
*/
#ifndef LEMON_KERNEL_FUNC_HPP
#define LEMON_KERNEL_FUNC_HPP
#include <lemon/kernel/system.hpp>

namespace lemon{namespace kernel{

	inline lemon_event_t wait(lemon_actor * actor,const lemon_mutext_t * mutex,const const_buff<lemon_event_t> & events,size_t timeout)
	{
		actor->lasterror_reset();

		if(actor == actor->get_system()->main_actor())
		{
			lemon_declare_errinfo(errorCode);

			lemon_user_errno(errorCode,LEMON_UNSUPPORT_OPTION);

			throw errorCode;
		}

		if(actor->killed())
		{
			lemon_declare_errinfo(errorCode);

			lemon_user_errno(errorCode,LEMON_KILLED);

			throw errorCode;
		}

		actor->mutex_reset(mutex);

		actor->events_reset(events);

		actor->timeout_reset(timeout);

		actor->notified_reset();

		lemon_context_jump(*actor,actor->parent(),0);

		if(actor->killed())
		{
			lemon_declare_errinfo(errorCode);

			lemon_user_errno(errorCode,LEMON_KILLED);

			throw errorCode;
		}

		return actor->notified();
	}

}}

#endif //LEMON_KERNEL_FUNC_HPP
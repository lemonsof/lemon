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
#include <sofxx/sof.hpp>

namespace sof{namespace impl{

	struct sof_actor : private nocopyable
	{
		sof_actor():status(actor_status::init){}

		sof_t									id;

		actor_status							status;
	};

}}

#endif //SOF_ACTOR_HPP
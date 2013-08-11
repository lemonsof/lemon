/**
* 
* @file     actor
* @brief    Copyright (C) 2013  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2013/08/10
*/
#ifndef SOFXX_ACTOR_HPP
#define SOFXX_ACTOR_HPP

#include <sof/abi.h>
#include <sof/assembly.h>
#include <sofxx/error_info.hpp>
#include <sofxx/nocopyable.hpp>

#define check_throw() __rethrow(__FILE__,__LINE__)

namespace sof{

	enum class actor_status
	{
		unknown = SOF_ACTOR_UNKNOWN,

		init = SOF_ACTOR_INIT,

		running = SOF_ACTOR_RUNNING,

		sleeping = SOF_ACTOR_SLEEPING,

		stopped = SOF_ACTOR_STOPPED
	};

	class actor 
	{
	public:
		actor(sof_state S):_S(S){}

		operator sof_state () { return _S; }

		bool empty() const { return _S == nullptr; }

		const sof_errno_info * last_errno() const
		{
			return sof_last_errno(_S);
		}

		bool succeed() const
		{
			return nullptr == last_errno();
		}

		bool fail() const { return !succeed(); }

		void __rethrow(const char * file, int lines)
		{
			if(fail())
			{
				sof_raise_trace(_S,file,lines);

				throw error_info(*last_errno());
			}
		}

		void reset_errno()
		{
			sof_reset_errno(_S);
		}

	private:
		sof_state							_S;
	};

}
#endif //SOFXX_ACTOR_HPP
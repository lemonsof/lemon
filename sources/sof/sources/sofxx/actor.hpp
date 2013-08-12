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
#include <functional>
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

		typedef std::function<void(actor)>	function_t;

		static void __f(sof_state S,void * userdata)
		{
			function_t * f = (function_t*)userdata;

			try
			{
				(*f)(S);
			}
			catch(const error_info &)
			{
				sof_raise_trace(S,__FILE__,__LINE__);
			}
			catch(...)
			{
				sof_errno_info errorCode;

				SOF_USER_ERROR(errorCode,SOF_UNKNOWN_EXCEPTION);

				sof_raise_errno(S,"unknown exception",&errorCode);
			}

			delete f;
		}

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

		template<typename F>
		sof_t go(F && f)
		{
			function_t * data = new function_t(f);

			sof_t result = sof_go(_S,&actor::__f,data);

			if(fail())
			{
				delete data;

				check_throw();
			}

			return result;
		}

		template<typename F>
		sof_t go(F && f,size_t stacksize)
		{
			function_t * data = new function_t(f);

			sof_t result = sof_go(_S,&actor::__f,data,stacksize);

			if(fail())
			{
				delete data;

				check_throw();
			}

			return result;
		}

		void stop()
		{
			sof_stop(_S);

			check_throw();
		}

		template<size_t N>
		sof_event_t wait(sof_event_t(&waitlist) [N])
		{
			return wait(NULL,waitlist);
		}

		template<size_t N>
		sof_event_t wait(const sof_mutext_t * mutex , sof_event_t(&waitlist) [N])
		{
			sof_event_t result = sof_wait(_S,mutex,waitlist,N);

			check_throw();

			return result;
		}

		sof_event_t wait(const sof_mutext_t * mutex , sof_event_t evt)
		{
			sof_event_t result = sof_wait(_S,mutex,&evt,1);

			check_throw();

			return result;
		}

		sof_event_t wait( sof_event_t evt)
		{
			return wait(NULL,evt);
		}

		bool notify(sof_t target, sof_event_t evt)
		{
			bool result = sof_notify(_S,target,&evt,1);

			check_throw();

			return result;
		}

		template<size_t N>
		bool notify(sof_t target, sof_event_t(&waitlist) [N])
		{
			bool result = sof_notify(_S,waitlist,N);

			check_throw();

			return result;
		}

	private:

		sof_state							_S;
	};

}
#endif //SOFXX_ACTOR_HPP
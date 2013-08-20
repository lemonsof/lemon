/**
* 
* @file     actor
* @brief    Copyright (C) 2013  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2013/08/10
*/
#ifndef LEMONXX_ACTOR_HPP
#define LEMONXX_ACTOR_HPP

#include <functional>
#include <lemon/abi.h>
#include <lemon/assembly.h>
#include <lemonxx/error_info.hpp>
#include <lemonxx/nocopyable.hpp>

#define check_throw() __rethrow(__FILE__,__LINE__)

namespace lemon{

	enum class actor_status
	{
		unknown = LEMON_ACTOR_UNKNOWN,

		init = LEMON_ACTOR_INIT,

		running = LEMON_ACTOR_RUNNING,

		sleeping = LEMON_ACTOR_SLEEPING,

		stopped = LEMON_ACTOR_STOPPED
	};

	class actor 
	{
	public:

		typedef std::function<void(actor)>	function_t;

		static void __f(lemon_state S,void * userdata)
		{
			function_t * f = (function_t*)userdata;

			try
			{
				(*f)(S);
			}
			catch(const lemon_errno_info &)
			{
				lemon_raise_trace__(S,"actor::__f catch exception",__FILE__,__LINE__);
			}
			catch(...)
			{
				lemon_raise_errno(S,"unknown exception",LEMON_UNKNOWN_EXCEPTION);
			}

			delete f;
		}

		actor(lemon_state S):_S(S){}

		operator lemon_state () { return _S; }

		bool empty() const { return _S == nullptr; }

		const lemon_errno_info * last_errno() const
		{
			return lemon_last_errno(_S);
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
				lemon_raise_trace__(_S,NULL,file,lines);

				throw *last_errno();
			}
		}

		void reset_errno()
		{
			lemon_reset_errno(_S);
		}

		template<typename F>
		lemon_t go(F && f)
		{
			function_t * data = new function_t(f);

			lemon_t result = lemon_go(_S,&actor::__f,data);

			if(fail())
			{
				delete data;

				check_throw();
			}

			return result;
		}

		template<typename F>
		lemon_t go(F && f,size_t stacksize)
		{
			function_t * data = new function_t(f);

			lemon_t result = lemon_go(_S,&actor::__f,data,stacksize);

			if(fail())
			{
				delete data;

				check_throw();
			}

			return result;
		}

		void stop()
		{
			lemon_stop(_S);

			check_throw();
		}

		template<size_t N>
		lemon_event_t wait(lemon_event_t(&waitlist) [N])
		{
			return wait(NULL,waitlist);
		}

		template<size_t N>
		lemon_event_t wait(lemon_event_t(&waitlist) [N],size_t timeout)
		{
			return wait(NULL,waitlist,timeout);
		}

		template<size_t N>
		lemon_event_t wait(const lemon_mutext_t * mutex , lemon_event_t(&waitlist) [N])
		{
			lemon_event_t result = lemon_wait(_S,mutex,waitlist,N);

			check_throw();

			return result;
		}

		template<size_t N>
		lemon_event_t wait(const lemon_mutext_t * mutex , lemon_event_t(&waitlist) [N],size_t timeout)
		{
			lemon_event_t result = lemon_wait(_S,mutex,waitlist,N,timeout);

			check_throw();

			return result;
		}

		lemon_event_t wait(const lemon_mutext_t * mutex , lemon_event_t evt,size_t timeout)
		{
			lemon_event_t result = lemon_wait(_S,mutex,&evt,1,timeout);

			check_throw();

			return result;
		}

		lemon_event_t wait(const lemon_mutext_t * mutex , lemon_event_t evt)
		{
			lemon_event_t result = lemon_wait(_S,mutex,&evt,1);

			check_throw();

			return result;
		}

		lemon_event_t wait( lemon_event_t evt,size_t timeout)
		{
			return wait(NULL,evt,timeout);
		}

		lemon_event_t wait( lemon_event_t evt)
		{
			return wait(NULL,evt,LEMON_INFINITE);
		}

		void sleep(size_t timeout)
		{
			lemon_wait(_S,nullptr,nullptr,0,timeout);

			check_throw();
		}

		bool notify(lemon_t target, lemon_event_t evt)
		{
			bool result = lemon_notify(_S,target,&evt,1);

			check_throw();

			return result;
		}

		template<size_t N>
		bool notify(lemon_t target, lemon_event_t(&waitlist) [N])
		{
			bool result = lemon_notify(_S,waitlist,N);

			check_throw();

			return result;
		}

		void* recv_poll(const lemon_channel_t* channelist, size_t len,size_t timeout)
		{
			void * data = lemon_recv_poll(_S,channelist,len,timeout);

			check_throw();

			return data;
		}

		void* recv_poll(const lemon_channel_t* channelist, size_t len)
		{
			void * data = lemon_recv_poll(_S,channelist,len);

			check_throw();

			return data;
		}

		template<size_t N>
		void* recv_poll(const lemon_channel_t (&channelist)[N])
		{
			void * data = lemon_recv_poll(_S,channelist,N);

			check_throw();

			return data;
		}

		template<size_t N>
		void* recv_poll(const lemon_channel_t (&channelist)[N],size_t timeout)
		{
			void * data = lemon_recv_poll(_S,channelist,N,timeout);

			check_throw();

			return data;
		}

	private:

		lemon_state							_S;
	};

}
#endif //LEMONXX_ACTOR_HPP

/**
* 
* @file     actor
* @brief    Copyright (C) 2013  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2013/08/22
*/
#ifndef LEMONXX_ACTOR_HPP
#define LEMONXX_ACTOR_HPP
#include <memory>
#include <functional>
#include <lemon/abi.h>
#include <lemonxx/buffer.hpp>
#include <lemonxx/nocopyable.hpp>

namespace lemon{

	class actor 
	{
	public:
		typedef std::function<void(actor)>		func_t;

		static void __f(lemon_state S,func_t * f)
		{
			try
			{
				std::unique_ptr<func_t> userdata(f);

				(*f)(actor(S));
			}
			catch(...)
			{

			}
		}

		template<typename Handle>
		static lemon_t make_go(lemon_state S,Handle && handle,size_t stacksize = LEMON_DEFAULT_STACKSIZE)
		{
			std::unique_ptr<func_t> userdata(new func_t(handle));

			lemon_t target = lemon_go(S,(lemon_f)&actor::__f,userdata.get(),stacksize);

			lemon_check_throw(S);

			userdata.release();

			return target;
		}

		actor(lemon_state S):_S(S)
		{

		}

		operator lemon_state () const { return _S; }

		template<typename Handle>
		lemon_t go(Handle && handle,size_t stacksize = LEMON_DEFAULT_STACKSIZE)
		{
			return make_go(_S,HANDLE,stacksize);
		}

		void sleep(size_t timeout)
		{
			lemon_wait(_S,nullptr,nullptr,0,timeout);

			check_throw();
		}

		void check_throw()
		{
			lemon_check_throw(_S);
		}

		void stop()
		{
			lemon_stop(_S);

			lemon_check_throw(_S);
		}

		lemon_event_t wait(const lemon_mutext_t* mutex,lemon_event_t evt)
		{
			lemon_event_t result = lemon_wait(_S,mutex,&evt,1);

			lemon_check_throw(_S);

			return result;
		}

		lemon_event_t wait(const lemon_mutext_t* mutex,const const_buff<lemon_event_t>& events)
		{
			lemon_event_t result = lemon_wait(_S,mutex,events.buff,events.length);

			lemon_check_throw(_S);

			return result;
		}

		lemon_event_t wait(const const_buff<lemon_event_t>& events)
		{
			lemon_event_t result = lemon_wait(_S,nullptr,events.buff,events.length);

			lemon_check_throw(_S);

			return result;
		}

		lemon_event_t wait(lemon_event_t evt)
		{
			lemon_event_t result = lemon_wait(_S,nullptr,&evt,1);

			lemon_check_throw(_S);

			return result;
		}

		lemon_event_t wait_timeout(const lemon_mutext_t* mutex,lemon_event_t evt,size_t timeout)
		{
			lemon_event_t result = lemon_wait(_S,mutex,&evt,1,timeout);

			lemon_check_throw(_S);

			return result;
		}

		lemon_event_t wait_timeout(lemon_event_t evt,size_t timeout)
		{
			lemon_event_t result = lemon_wait(_S,nullptr,&evt,1,timeout);

			lemon_check_throw(_S);

			return result;
		}

		lemon_event_t wait_timeout(const const_buff<lemon_event_t>& events,size_t timeout)
		{
			lemon_event_t result = lemon_wait(_S,nullptr,events.buff,events.length,timeout);

			lemon_check_throw(_S);

			return result;
		}

		lemon_event_t wait_timeout(const lemon_mutext_t* mutex,const const_buff<lemon_event_t>& events,size_t timeout)
		{
			lemon_event_t result = lemon_wait(_S,mutex,events.buff,events.length,timeout);

			lemon_check_throw(_S);

			return result;
		}


		bool notify(lemon_t target,const const_buff<lemon_event_t>& events)
		{
			bool status = lemon_notify(_S,target,events.buff,events.length);

			lemon_check_throw(_S);

			return status;
		}

		bool notify(lemon_t target,lemon_event_t evt)
		{
			bool status = lemon_notify(_S,target,&evt,1);

			lemon_check_throw(_S);

			return status;
		}


	private:

		lemon_state										_S;
	};

	

	
}
#endif //LEMONXX_ACTOR_HPP
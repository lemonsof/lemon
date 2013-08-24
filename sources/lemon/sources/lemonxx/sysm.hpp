/**
* 
* @file     sysm
* @brief    Copyright (C) 2013  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2013/08/22
*/
#ifndef LEMONXX_SYSM_HPP
#define LEMONXX_SYSM_HPP
#include <lemon/abi.h>
#include <lemon/assembly.h>
#include <lemonxx/actor.hpp>
#include <lemonxx/nocopyable.hpp>


namespace lemon{

	class system : private nocopyable
	{
	public:
		system()
		{
			_S = lemon_new();

			if(!_S)
			{
				lemon_declare_errinfo(errorCode);

				lemon_user_errno(errorCode,LEMON_RESOURCE_ERROR);

				throw errorCode;
			}
		}

		system(size_t maxcoros,size_t maxchannels)
		{
			_S = lemon_new(maxcoros,maxchannels);

			if(!_S)
			{
				lemon_declare_errinfo(errorCode);

				lemon_user_errno(errorCode,LEMON_RESOURCE_ERROR);

				throw errorCode;
			}
		}

		~system()
		{
			lemon_close(_S);
		}

		template<typename Handle>
		lemon_t go(Handle && handle,size_t stacksize = LEMON_DEFAULT_STACKSIZE)
		{
			return actor::make_go(_S,handle,stacksize);
		}

		void join()
		{
			lemon_join(_S);

			lemon_check_throw(_S);
		}

		operator lemon_state () const { return _S; }

		bool notify(lemon_t target,lemon_event_t evt)
		{
			bool status = lemon_notify(_S,target,&evt,1);

			lemon_check_throw(_S);

			return status;
		}

	

		bool notify(lemon_t target,const const_buff<lemon_event_t>& events)
		{
			bool status = lemon_notify(_S,target,events.buff,events.length);

			lemon_check_throw(_S);

			return status;
		}

	private:

		lemon_state												_S;
	};

}
#endif //LEMONXX_SYSM_HPP
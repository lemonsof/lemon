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

	class system : public actor
	{
	public:
		system():actor(lemon_new())
		{
			if(!(lemon_state)*this)
			{
				lemon_declare_errinfo(errorCode);

				lemon_user_errno(errorCode,LEMON_RESOURCE_ERROR);

				throw errorCode;
			}
		}

		system(size_t maxcoros,size_t maxchannels):actor(lemon_new(maxcoros,maxchannels))
		{
			if(!(lemon_state)*this)
			{
				lemon_declare_errinfo(errorCode);

				lemon_user_errno(errorCode,LEMON_RESOURCE_ERROR);

				throw errorCode;
			}
		}

		~system()
		{
			lemon_close((lemon_state)*this);
		}

		void join()
		{
			lemon_join(*this);

			lemon_check_throw(*this);
		}

	private:

		lemon_state												_S;
	};

}
#endif //LEMONXX_SYSM_HPP
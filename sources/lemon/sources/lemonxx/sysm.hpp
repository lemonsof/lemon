/**
* 
* @file     sysm
* @brief    Copyright (C) 2013  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2013/08/10
*/
#ifndef LEMONXX_SYSM_HPP
#define LEMONXX_SYSM_HPP
#include <lemonxx/actor.hpp>
namespace lemon{

	class system : public actor, private nocopyable
	{
	public:

		system() :actor(lemon_new(0,0))
		{

		}

		system(size_t maxcors,size_t maxchannels)
			:actor(lemon_new(maxcors,maxchannels))
		{
			if(empty())
			{
				lemon_errno_info errorCode = {LEMON_RESOURCE_ERROR,__FILE__,__LINE__};

				throw errorCode;
			}
		}

		~system()
		{
			lemon_close(*this);
		}

		void join()
		{
			lemon_join(*this);

			check_throw();
		}
	};

}
#endif //LEMONXX_SYSM_HPP

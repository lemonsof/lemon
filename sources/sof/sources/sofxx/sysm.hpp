/**
* 
* @file     sysm
* @brief    Copyright (C) 2013  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2013/08/10
*/
#ifndef SOFXX_SYSM_HPP
#define SOFXX_SYSM_HPP
#include <sofxx/actor.hpp>
namespace sof{

	class system : public actor, private nocopyable
	{
	public:

		system() :actor(sof_new(0,0))
		{

		}

		system(size_t maxcors,size_t maxchannels)
			:actor(sof_new(maxcors,maxchannels))
		{
			if(empty())
			{
				error_info errorCode;

				SOF_USER_ERROR(errorCode,SOF_RESOURCE_ERROR);

				throw errorCode;
			}
		}

		~system()
		{
			sof_close(*this);
		}

		void join()
		{
			sof_join(*this);

			check_throw();
		}
	};

}
#endif //SOFXX_SYSM_HPP
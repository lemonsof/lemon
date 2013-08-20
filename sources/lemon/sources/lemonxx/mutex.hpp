/**
* 
* @file     mutex
* @brief    Copyright (C) 2013  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2013/08/20
*/
#ifndef LEMONXX_MUTEX_HPP
#define LEMONXX_MUTEX_HPP
#include <mutex>
#include <lemon/abi.h>
#include <lemonxx/nocopyable.hpp>
namespace lemon{

	template<typename Locker>
	class basic_lock : private nocopyable
	{
	public:
		basic_lock(Locker & locker)
		{
			_mutex.mutex = &locker;

			_mutex.lock = &basic_lock::lock;

			_mutex.unlock = &basic_lock::unlock;
		}

		operator  lemon_mutext_t * ()
		{
			return &_mutex;
		}

	private:

		static void lock(void *mutex)
		{
			reinterpret_cast<Locker*>(mutex)->lock();
		}

		static void unlock(void *mutex)
		{
			reinterpret_cast<Locker*>(mutex)->unlock();
		}

	private:


	private:

		 lemon_mutext_t							_mutex;
	};

}
#endif //LEMONXX_MUTEX_HPP
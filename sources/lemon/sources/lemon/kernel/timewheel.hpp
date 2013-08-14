/**
* 
* @file     timewheel
* @brief    Copyright (C) 2013  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2013/08/13
*/
#ifndef LEMON_TIMER_TIMEWHEEL_HPP
#define LEMON_TIMER_TIMEWHEEL_HPP
#include <mutex>
#include <thread>
#include <lemon/abi.h>
#include <condition_variable>
#include <lemonxx/extension.hpp>
#include <lemonxx/nocopyable.hpp>


namespace lemon{namespace impl{

	class lemon_system;

	class lemon_timewheel :  private nocopyable
	{
	public:

		struct timer
		{
			timer(lemon_t tt,size_t tk) 
				:next(nullptr)
				,prev(nullptr)
				,target(tt)
				,ticks((lemon_uint32_t)tk)
			{

			}

			timer									*next;

			timer									**prev;

			lemon_t									target;

			lemon_uint32_t							ticks;
		};

		struct cascade
		{
			cascade()
			{
				std::fill(std::begin(buckets),std::end(buckets),nullptr);
			}

			lemon_uint8_t								cursor;

			timer									*buckets[256];
		}; 

		lemon_timewheel();

		~lemon_timewheel(); 

	public:

		void join();

		void stop();

		void start(lemon_system * sysm);

		void create_timer(lemon_t target,size_t timeout);

	private:

		timer * new_timer(lemon_t target,size_t timeout);

		void free_timer(timer * val);

		void proc();

		void insert(timer * val);

		void tick();

		void __cascade(size_t index);

	private:

		lemon_system								*_sysm;

		bool										_exit;

		size_t										_timers;

		cascade										_cascades[4];

		std::thread									_worker;

		std::mutex									_mutex;

		std::condition_variable						_condition;
	};

}}

#endif //LEMON_TIMER_TIMEWHEEL_HPP

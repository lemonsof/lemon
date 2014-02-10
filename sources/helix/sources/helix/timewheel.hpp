/**
* 
* @file     timewheel
* @brief    Copyright (C) 2014  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2014/02/08
*/
#ifndef HELIX_TIMEWHEEL_HPP
#define HELIX_TIMEWHEEL_HPP
#include <mutex>
#include <chrono>
#include <thread>
#include <helix/object.hpp>
#include <condition_variable>
namespace helix{namespace impl{

	class runtimes;
	class timewheel_t : public object
	{
	public:
		static uintptr_t event()
		{
			static const uintptr_t unknown = 0;

			return (uintptr_t)&unknown;
		}
		struct timer : public object
		{
			timer(uintptr_t tt, size_t tk)
			:next(nullptr)
			, prev(nullptr)
			, target(tt)
			, ticks((helix_uint32_t)tk)
			{

			}

			timer									*next;

			timer									**prev;

			uintptr_t								target;

			helix_uint32_t							ticks;
		};

		struct cascade
		{
			cascade()
			{
				std::fill(std::begin(buckets), std::end(buckets), nullptr);
			}

			helix_uint8_t							cursor;

			timer									*buckets[256];
		};

		timewheel_t();

		~timewheel_t();

	public:

		void stop_and_join();

		void start(runtimes* rt);

		void create_timer(uintptr_t target, std::chrono::milliseconds timeout);

	private:

		timer * new_timer(uintptr_t target, std::chrono::milliseconds timeout);

		void free_timer(timer * val);

		void proc();

		void insert(timer * val);

		void tick();

		void __cascade(size_t index);

	private:

		runtimes									*_runtimes;

		bool										_exit;

		size_t										_timers;

		cascade										_cascades[4];

		std::thread									_worker;

		std::mutex									_mutex;

		std::condition_variable						_condition;
	};
}}
#endif	//HELIX_TIMEWHEEL_HPP
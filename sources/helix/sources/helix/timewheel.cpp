#include <cassert>
#include <helix/runtimes.hpp>
#include <helix/timewheel.hpp>

namespace helix{namespace impl{

	timewheel_t::timewheel_t() :_runtimes(nullptr), _exit(false)
	{

	}

	timewheel_t::~timewheel_t()
	{

	}


	void timewheel_t::stop_and_join()
	{
		{
			std::unique_lock<std::mutex> lock(_mutex);

			_exit = true;

			_condition.notify_one();
		}

		if (_worker.joinable()) _worker.join();
	}

	
	void timewheel_t::start(runtimes * sysm)
	{
		_runtimes = sysm;

		_worker = std::thread(&timewheel_t::proc, this);
	}

	void timewheel_t::create_timer(uintptr_t target, std::chrono::milliseconds timeout)
	{
		std::unique_lock<std::mutex> lock(_mutex);

		timer * t = new_timer(target, timeout);

		insert(t);
	}

	timewheel_t::timer * timewheel_t::new_timer(uintptr_t target, std::chrono::milliseconds timeout)
	{
		size_t counter = timeout.count() / HELIX_TICKS_OF_MILLISECOND + 1;

		return new(_runtimes->alloc()) timer(target, counter );
	}

	void timewheel_t::free_timer(timewheel_t::timer * val)
	{
		 release(val,_runtimes->alloc());
	}

	void timewheel_t::insert(timewheel_t::timer * t)
	{
		size_t buckets;

		cascade * cas;

		helix_uint32_t tick = t->ticks;

		assert(t->next == NULL && t->prev == NULL);

		if (0 == tick)
		{
			_mutex.unlock();

			_runtimes->notify_timeout(t->target);

			_mutex.lock();

			free_timer(t);

			_timers--;

			return;
		}

		if ((tick >> 24) & 0xff)
		{
			buckets = ((tick >> 24) & 0xff);

			cas = &_cascades[3];

		}
		else if ((tick >> 16) & 0xff)
		{
			buckets = ((tick >> 16) & 0xff);

			cas = &_cascades[2];
		}
		else if ((tick >> 8) & 0xff)
		{
			buckets = ((tick >> 8) & 0xff);

			cas = &_cascades[1];
		}
		else
		{
			buckets = tick;

			cas = &_cascades[0];
		}

		buckets = (buckets + cas->cursor - 1) % 256;

		t->next = cas->buckets[buckets];

		t->prev = &cas->buckets[buckets];

		if (cas->buckets[buckets]){

			assert(cas->buckets[buckets]->prev == &cas->buckets[buckets]);

			cas->buckets[buckets]->prev = &t->next;
		}

		cas->buckets[buckets] = t;
	}

	void timewheel_t::__cascade(size_t index)
	{
		cascade *cas = &_cascades[index];

		if (cas->cursor != 0 || index == 3) return;

		cascade *cascade_upper = &_cascades[++index];

		timer *timers = cascade_upper->buckets[cascade_upper->cursor];

		cascade_upper->buckets[cascade_upper->cursor++] = NULL;

		__cascade(index);

		while (timers){

			timer *next = timers->next;


			switch (index)
			{
			case 1:
			{
					  timers->ticks &= 0xff;

					  break;
			}
			case 2:
			{
					  timers->ticks &= 0xffff;

					  break;
			}
			case 3:
			{
					  timers->ticks &= 0xffffff;

					  break;
			}
			default:
			{
					   assert(false && "not here");
			}
			}

			timers->next = NULL; timers->prev = NULL;

			insert(timers);

			timers = next;

		};
	}

	void timewheel_t::tick()
	{
		cascade *cas = &_cascades[0];

		timer * timers = cas->buckets[cas->cursor];

		cas->buckets[cas->cursor] = NULL;

		cas->cursor++;

		__cascade(0);

		while (timers){

			_timers--;

			timer * next = timers->next;

			timers->next = NULL; timers->prev = NULL;

			_mutex.unlock();

			_runtimes->notify_timeout(timers->target);

			_mutex.lock();

			free_timer(timers);

			timers = next;
		}
	}

	void timewheel_t::proc()
	{
		std::unique_lock<std::mutex> lock(_mutex);

		while (!_exit)
		{
			if (std::cv_status::timeout == _condition.wait_for(lock, std::chrono::milliseconds(HELIX_TICKS_OF_MILLISECOND)))
			{
				tick();
			}
		}
	}

}}
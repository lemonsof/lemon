#include <assert.h>
#include <functional>
#include <lemon/kernel/system.hpp>
#include <lemon/kernel/timewheel.hpp>

namespace lemon{namespace kernel{

	lemon_timewheel::lemon_timewheel():_sysm(nullptr),_exit(false)
	{
		
	}

	lemon_timewheel::~lemon_timewheel()
	{

	}


	void lemon_timewheel::stop()
	{
		std::unique_lock<std::mutex> lock(_mutex);

		_exit = true;

		_condition.notify_one();
	}

	void lemon_timewheel::join()
	{
		if(_worker.joinable()) _worker.join();
	}

	void lemon_timewheel::start(lemon_system * sysm)
	{
		_sysm = sysm;

		_worker = std::thread(&lemon_timewheel::proc,this);
	}

	void lemon_timewheel::create_timer(lemon_t target,size_t timeout)
	{
		std::unique_lock<std::mutex> lock(_mutex);

		timer * t = new_timer(target,timeout);

		insert(t);
	}

	lemon_timewheel::timer * lemon_timewheel::new_timer(lemon_t target,size_t timeout)
	{
		return new timer(target,timeout / LEMON_MILLISECONDS_OF_TICK + 1);
	}

	void lemon_timewheel::free_timer(lemon_timewheel::timer * val)
	{
		delete val;
	}

	void lemon_timewheel::insert(lemon_timewheel::timer * t)
	{
		size_t buckets;

		cascade * cas;

		lemon_uint32_t tick = t->ticks ;

		assert(t->next == NULL && t->prev == NULL);

		if(0 == tick)
		{
			_mutex.unlock();

			_sysm->notify_timeout(t->target);

			_mutex.lock();

			free_timer(t);

			_timers -- ;

			return;
		}

		if((tick >> 24) & 0xff)
		{
			buckets = ((tick >> 24) & 0xff);

			cas = &_cascades[3];

		}
		else if((tick >> 16) & 0xff)
		{
			buckets = ((tick >> 16) & 0xff);

			cas = &_cascades[2];
		}
		else if((tick >> 8) & 0xff)
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

		if(cas->buckets[buckets]){

			assert(cas->buckets[buckets]->prev == &cas->buckets[buckets]);

			cas->buckets[buckets]->prev = &t->next;
		}

		cas->buckets[buckets] = t;
	}

	void lemon_timewheel::__cascade(size_t index)
	{
		cascade *cas = &_cascades[index];

		if(cas->cursor != 0 || index == 3) return;

		cascade *cascade_upper = &_cascades[++ index];

		timer *timers = cascade_upper->buckets[cascade_upper->cursor];

		cascade_upper->buckets[cascade_upper->cursor ++ ] = NULL;

		__cascade(index);

		while(timers){

			timer *next = timers->next;


			switch(index)
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
					assert (false && "not here");
				}
			}

			timers->next = NULL; timers->prev = NULL;

			insert(timers);

			timers = next;

		};
	}

	void lemon_timewheel::tick()
	{
		cascade *cas = &_cascades[0];

		timer * timers = cas->buckets[cas->cursor];

		cas->buckets[cas->cursor] = NULL;

		cas->cursor ++;

		__cascade(0);

		while(timers){

			_timers --;

			timer * next = timers->next;

			timers->next = NULL; timers->prev = NULL;

			_mutex.unlock();

			_sysm->notify_timeout(timers->target);

			_mutex.lock();

			free_timer(timers);

			timers = next;
		}
	}

	void lemon_timewheel::proc()
	{
		std::unique_lock<std::mutex> lock(_mutex);

		while(!_exit)
		{
			if(std::cv_status::timeout == _condition.wait_for(lock, std::chrono::milliseconds(LEMON_MILLISECONDS_OF_TICK)))
			{
				tick();
			}
		}
	}
}}


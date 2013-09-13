#include <atomic>
#include <random>
#include <lemonxx/lemon.hpp>
#include <lemonxx/unittest.hpp>

namespace lemon{namespace {

	class lemon_unittest{};

	LEMON_UNITTEST_CASE(lemon_unittest,main_actor_wait_test)
	{
		system sysm;

		dtrace::open_console(sysm,LEMON_TRACE_ALL);

		sysm.sleep(1000);
	}

	LEMON_UNITTEST_CASE(lemon_unittest,main_actor_notify_test)
	{
		system sysm;

		dtrace::open_console(sysm,LEMON_TRACE_ALL);

		auto id = sysm.go([](actor self){

			lemon_log_text(self,"wait .........");

			self.wait((lemon_event_t)1);

			lemon_log_text(self,"wakeup .........");

			self.stop();

		});

		while(!sysm.notify(id,(lemon_event_t)1));

		lemon_log_text(sysm,"notify success");

		sysm.join();
	}

	LEMON_UNITTEST_CASE(lemon_unittest,sleep_test)
	{
		system sysm;

		dtrace::open_console(sysm,LEMON_TRACE_ALL);

		sysm.go([](actor self){

			self.sleep(1000);

			self.stop();

		});

		sysm.join();
	}

	LEMON_UNITTEST_CASE(lemon_unittest,mass_actor_test)
	{
		system sysm;

		dtrace::open_console(sysm,LEMON_TRACE_ALL);

		std::default_random_engine e((unsigned int)time(0));

		size_t maxactors = 10000;

		std::atomic<size_t> counter = maxactors;

		for(size_t i = 0; i <  maxactors; ++ i)
		{
			size_t timeout = e() % 1000;

			sysm.go([&counter,timeout](actor self){

				self.sleep(timeout);

				if( -- counter == 0) self.stop();

			},1024);
		}

		sysm.join();
	}

	LEMON_UNITTEST_CASE(lemon_unittest,ping_pone_test)
	{
		system sysm;

		dtrace::open_console(sysm,LEMON_TRACE_ERROR);

		auto id = sysm.go([](actor self){

			for(size_t i = 0; i < 100000; ++ i)
			{
				//lemon_log_text(self,"begin wait notify :%d",i);

				self.wait((lemon_event_t)1);

				lemon_log_text(self,"receive notify :%d -- success",i);
			}

			self.stop();
		});

		sysm.go([id](actor self){

			for(;;)
			{
				self.notify(id,(lemon_event_t)1);
			}
		});

		sysm.join();
	}
	
}}
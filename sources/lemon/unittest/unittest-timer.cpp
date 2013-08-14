#include <lemonxx/lemon.hpp>
#include <lemonxx/unittest.hpp>

namespace lemon{namespace {

	class timer_unittest{};

	LEMON_UNITTEST_CASE(timer_unittest,sleep_test)
	{
		lemon::system sysm;

		lemon::dtrace::null_consumer consumer(sysm,LEMON_TRACE_ALL);

		for(size_t i = 0; i < 10 ; ++ i)
		{
			sysm.go([](lemon::actor self)
			{
				for(size_t i = 0; i < 10 ; ++ i)
				{
					lemon_log_text(self,"try to waiting");

					self.sleep(100);

					lemon_log_text(self,"wakeup");
				}

				self.stop();

			},1024 * 32);
		}

		sysm.join();
	}

	LEMON_UNITTEST_CASE(timer_unittest,notify_test)
	{
		lemon::system sysm;

		lemon::dtrace::null_consumer consumer(sysm,LEMON_TRACE_ALL);

		auto id = sysm.go([](lemon::actor self)
		{
			for(;;)	
			{
				lemon_log_text(self,"try to waiting");

				self.wait(1);

				lemon_log_text(self,"wakeup");
			}

			

		},1024 * 16);

		sysm.go([id](lemon::actor self)
		{
			for(size_t i = 0; i < 100000; ++ i)
			{
				while(!self.notify(id,1));
			}
			
			self.stop();

		},1024);

		sysm.join();
	}
}}
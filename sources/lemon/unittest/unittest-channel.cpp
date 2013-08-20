#include <lemonxx/lemon.hpp>
#include <lemonxx/unittest.hpp>

namespace lemon{namespace {

	class channel_unittest{};

	LEMON_UNITTEST_CASE(channel_unittest,echo_test)
	{
		lemon::system sysm;

		lemon::dtrace::null_consumer consumer(sysm,LEMON_TRACE_ALL);

		lemon_channel_t chan = channel(sysm);

		sysm.go([chan](lemon::actor self)
		{
			scope_channel exp(self,chan);

			for(size_t  i = 0; i < 1000 ; ++ i)
			{
				exp.send(new size_t(i));
			}

			self.stop();
			
		},1024 * 16);

		sysm.go([chan](lemon::actor self)
		{
			scope_channel imp(self,chan);

			for(;;)
			{
				size_t * counter = (size_t*)imp.recv();

				lemon_log_text(self,"counter :%u",*counter);

				delete counter;
			}

		},1024 * 16);

		sysm.join();
	}

	LEMON_UNITTEST_CASE(channel_unittest,publish_test)
	{
		lemon::system sysm;

		lemon::dtrace::null_consumer consumer(sysm, LEMON_TRACE_VERBOSE);

		lemon_channel_t chan = channel(sysm,1024,channel::mult_recv);

		for(size_t i = 0; i < 10; ++ i)
		{
			sysm.go([chan](lemon::actor self)
			{
				scope_channel imp(self,chan);

				for(;;)
				{
					size_t * counter = (size_t*)imp.recv();

					lemon_log_text(self,"counter :%u",*counter);

					delete counter;
				}

			},1024 * 32);
		}

		sysm.go([chan](lemon::actor self)
		{
			scope_channel exp(self,chan);

			for(size_t  i = 0; i < 1000 ; ++ i)
			{
				exp.send(new size_t(i));
			}

			lemon_log_verbose(self,"---")

			self.stop();

		},1024 * 32);

		

		sysm.join();
	}

	LEMON_UNITTEST_CASE(channel_unittest,poll_test)
	{
		lemon::system sysm;

		lemon::dtrace::console_consumer consumer(sysm, LEMON_TRACE_VERBOSE );

		std::vector<lemon_channel_t> chans(1000);

		for(auto & chan : chans)
		{
			chan = channel(sysm,1024,channel::mult_recv);
		}

		sysm.go([&chans](lemon::actor self)
		{
			for(size_t i = 0; i < chans.size(); ++ i)
			{
				size_t * counter = (size_t*)self.recv_poll(&chans[0],chans.size());

				lemon_log_verbose(self,"counter :%u",*counter);
			}

			self.stop();

		},1024 * 32);

		for(size_t i = 0; i < chans.size(); ++ i)
		{
			sysm.go([=,&chans](lemon::actor self)
			{
				scope_channel exp(self,chans[i]);

				exp.send(new size_t(i));
				
			},1024 * 32);
		}

		sysm.join();
	}
}}
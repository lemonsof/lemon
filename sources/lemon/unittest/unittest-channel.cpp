#include <lemonxx/lemon.hpp>
#include <lemonxx/unittest.hpp>

namespace lemon{namespace {

	class channel_unittest{};

	LEMON_UNITTEST_CASE(channel_unittest,push_block_flags_test)
	{
		system sysm;

		dtrace::open_console(sysm,LEMON_TRACE_ALL);

		channel chan(sysm,LEMON_CHANNEL_PUSH);

		lemon_channel_t id = chan;

		sysm.go([=](actor self){

			channel chan(self,id);

			lemon_expect_exception(chan.recv(LEMON_CHANNEL_EXPORT),lemon_errno_info);
		});

		sysm.go([=](actor self){

			channel chan(self,id);

			lemon_expect_exception(chan.send(nullptr,LEMON_CHANNEL_IMPORT),lemon_errno_info);

			self.stop();
		});


		sysm.join();
	}

	LEMON_UNITTEST_CASE(channel_unittest,push_block_test)
	{
		system sysm;

		dtrace::open_console(sysm,LEMON_TRACE_TEXT);

		channel chan(sysm,LEMON_CHANNEL_PUSH);

		channel chan2(sysm,LEMON_CHANNEL_PUSH);

		lemon_channel_t id = chan;

		lemon_channel_t id2 = chan2;

		int workers = 100;

		int loop = 10;

		sysm.go([=](actor self){

			channel chan(self,id);

			for(int i = 0; i < workers * loop; ++ i)
			{
				lemon_log_debug(self,"manager send %d",i);

				chan.send_noblock(nullptr);

				lemon_log_debug(self,"manager send %d -- success",i);
			}

			lemon_log_text(self,"manager send finish");

			chan.release();
		});

		sysm.go([=](actor self){

			channel chan2(self,id2);

			for(int i = 0; i < workers * loop; ++ i)
			{
				lemon_log_debug(self,"reduce recv %d",i);

				chan2.recv();

				lemon_log_debug(self,"reduce recv %d -- success",i);
			}

			self.stop();
		});

	

		for(int i = 0; i < workers; ++ i)
		{
			sysm.go([=](actor self){

				channel chan(self,id);

				channel chan2(self,id2);

				for(;;)
				{
					chan.recv();

					lemon_log_verbose(self,"worker(%p) recv",(lemon_state)self);

					chan2.send_noblock(nullptr);
				}
			});
		}

		sysm.join();
	}
	
}}
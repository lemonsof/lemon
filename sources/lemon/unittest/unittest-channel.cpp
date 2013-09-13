#include <lemonxx/lemon.hpp>
#include <lemonxx/unittest.hpp>

namespace lemon{namespace {

	class channel_unittest{};

	void __test()
	{
		system sysm;

		dtrace::open_console(sysm,LEMON_TRACE_VERBOSE);

		auto chan = channel_req_t::make(sysm);

		unique_channel scope_channel(sysm,chan);

		sysm.go([chan](actor self){

			for(size_t  i = 0; i < 1000; ++ i)
			{
				lemon_log_text(self,"server recv .........");

				int * counter = (int*)recv(self,chan,LEMON_CHANNEL_EXPORT);

				lemon_log_text(self,"server recv (%d)",*counter);

				(*counter) ++;

				send(self,chan,counter,LEMON_CHANNEL_EXPORT);

				lemon_log_text(self,"server send .........");
			}

			self.stop();
		});

		for(size_t i = 0; i < 10; ++ i)
		{

			sysm.go([chan](actor self){

				int counter = 0;

				for(;;)
				{
					lemon_log_text(self,"client send (%d)",counter);

					send(self,chan,&counter,LEMON_CHANNEL_IMPORT,0);

					lemon_check(&counter == recv(self,chan,LEMON_CHANNEL_IMPORT));

					lemon_log_text(self,"client recv (%d)",counter);
				}
			});

		}



		sysm.join();

		lemon_log_verbose(sysm,"exit the lemon system ...");
	}

	LEMON_UNITTEST_CASE(channel_unittest,request_test)
	{
		__test();
	}
	

	LEMON_UNITTEST_CASE(channel_unittest,request_timeout_test)
	{
		system sysm;

		dtrace::open_console(sysm,LEMON_TRACE_VERBOSE);

		auto chan = channel_req_t::make(sysm);

		unique_channel scope_channel(sysm,chan);

		sysm.go([chan](actor self){

			int counter = 0;

			send(self,chan,&counter,LEMON_CHANNEL_IMPORT,0);

			lemon_log_text(self,"client recv");

			lemon_check(recv(self,chan,LEMON_CHANNEL_IMPORT,1000)  == nullptr);

			lemon_log_text(self,"client recv timeout ");

			self.stop();
		});

		sysm.join();
	}

	LEMON_UNITTEST_CASE(channel_unittest,push_test)
	{
		system sysm;

		//dtrace::open_console(sysm,LEMON_TRACE_ALL);

		auto chan = channel_push_t::make(sysm);

		unique_channel scope_channel(sysm,chan);

		for(size_t i = 0; i < 10; ++ i)
		{

			sysm.go([chan](actor self){

				for(int i = 0;; ++i)
				{
					recv(self,chan);

					lemon_log_text(self,"client recv (%d)",i);
				}
			});

		}

		for(size_t  i = 0; i < 1000; ++ i)
		{

			lemon_log_text(sysm,"server send .........");

			send(sysm,chan,0,0);
		}

	}

	void* __msg_f(lemon_state S,int flag,void * msg)
	{
		if(LEMON_MSG_CLOSE == flag)
		{
			lemon_log_text(S,"close msg (%p)",msg);

			delete (int*)msg;

			return nullptr;
		}
		else
		{
			lemon_log_text(S,"clone msg (%p)",msg);

			return new int(*(int*)msg);
		}
	}

	LEMON_UNITTEST_CASE(channel_unittest,public_test)
	{
		system sysm;

		dtrace::open_console(sysm,LEMON_TRACE_DEBUG);

		auto chan = channel_pub_t::make(sysm,__msg_f);

		unique_channel scope_channel(sysm,chan);

		for(size_t i = 0; i < 10; ++ i)
		{

			sysm.go([chan](actor self){

				for(int i = 0;; ++i)
				{
					int* val = (int*)recv(self,chan);

					lemon_check(val != nullptr);

					lemon_log_text(self,"client recv (%d)",*val);

					delete val;
				}
			});

		}

		for(int  i = 0; i < 100000; ++ i)
		{
			lemon_log_text(sysm,"server send .........");

			send(sysm,chan,new int(i),0);
		}


	}

	
}}
/**
* 
* @file     lemon
* @brief    Copyright (C) 2013  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2013/08/09
*/
#include <chrono>
#include <future>
#include <thread>
#include <atomic>
#include <iostream>
#include <lemonxx/lemon.hpp>

using namespace lemon;

int main(int , char ** )
{
	using namespace std::chrono;

	steady_clock::time_point t1 = steady_clock::now();

	lemon::system sysm;

	lemon::dtrace::console_consumer consumer(sysm,LEMON_TRACE_ALL);

	for(size_t i = 0; i < 1000 ; ++ i)
	{
		sysm.go([](lemon::actor self)
		{

			for(;;)
			{
				lemon_log_text(self,"try to waiting");

				self.sleep(100);

				lemon_log_text(self,"wakeup");
			}

			//self.stop();

		},1024);
	}


	sysm.join();

	steady_clock::time_point t2 = steady_clock::now();

	duration<double> time_span = duration_cast<duration<double>>(t2 - t1);

	//std::cout << "It took me " << time_span.count() << " seconds." << std::endl;

	lemon_log_text(sysm,"It took me %f seconds",time_span.count());
}
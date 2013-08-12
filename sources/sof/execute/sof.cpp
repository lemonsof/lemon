/**
* 
* @file     sof
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
#include <sofxx/sof.hpp>

using namespace sof;

int main(int , char ** )
{
	using namespace std::chrono;

	steady_clock::time_point t1 = steady_clock::now();

	sof::system sysm;

	sof::dtrace::console_consumer consumer(sysm,SOF_TRACE_ALL);

	sof_t id = sysm.go([](sof::actor self){

		sof_log_text(self,"try to waiting");

		sof_event_t waitlist[] = {1,2,3,4,5};

		self.wait(waitlist);

		sof_log_text(self,"wakeup");

		self.stop();

	},1024);

	while(!sysm.notify(id,5));

	sysm.join();

	steady_clock::time_point t2 = steady_clock::now();

	duration<double> time_span = duration_cast<duration<double>>(t2 - t1);

	//std::cout << "It took me " << time_span.count() << " seconds." << std::endl;

	sof_log_text(sysm,"It took me %f seconds",time_span.count());
}
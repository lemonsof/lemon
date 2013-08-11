/**
* 
* @file     sof
* @brief    Copyright (C) 2013  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2013/08/09
*/
#include <future>
#include <thread>
#include <atomic>
#include <iostream>
#include <sofxx/sof.hpp>

using namespace sof;


int main(int , char ** )
{
	sof::system sysm;

	sof::dtrace::console_consumer consumer(sysm,SOF_TRACE_ALL);

	for(size_t i = 0 ; i < 100; ++i)
	{
		sof_log_error(sysm,"hello the world");



		sof_log_debug(sysm,"hello the world");
	}
	
}
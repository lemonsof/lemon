/**
* 
* @file     dtrace
* @brief    Copyright (C) 2013  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2013/08/10
*/
#ifndef SOFXX_DTRACE_HPP
#define SOFXX_DTRACE_HPP

#include <iomanip>
#include <iostream>
#include <sof/abi.h>
#include <sofxx/sysm.hpp>
#include <sofxx/nocopyable.hpp>


namespace sof{namespace dtrace{

	enum class level
	{
		slience = SOF_TRACE_SILENCE,
		
		fatal = SOF_TRACE_FATAL,

		error = SOF_TRACE_ERROR,

		warning = SOF_TRACE_WARNING,

		text = SOF_TRACE_TEXT,

		debug = SOF_TRACE_DEBUG,

		verbos = SOF_TRACE_VERBOSE
	};

	template<typename T> class basic_consumer : private nocopyable
	{
	public:
		basic_consumer(system& S,int levels):_sysm(S)
		{
			_handle = sof_new_trace(_sysm,&basic_consumer<T>::__f,this,levels);

			_sysm.check_throw();
		}

		~basic_consumer()
		{
			sof_close_trace(_sysm,_handle);
		}

	private:

		static void __f(void * userdata, int level, sof_int64_t timestamp,sof_t source, const char * msg)
		{
			reinterpret_cast<T*>(userdata)->consume(level,timestamp,source,msg);
		}

	private:

		system										&_sysm;

		sof_trace_t									_handle;
	};


	class console_consumer : public basic_consumer<console_consumer>
	{
	public:
		console_consumer(system& S,int levels):basic_consumer(S,levels)
		{

		}

		void consume(int l, sof_int64_t timestamp,sof_t source, const char * msg)
		{

			using namespace std::chrono;
			
			std::cout << std::hex;

			time_t tt = (time_t)timestamp / 1000000;
#ifdef WIN32
			tm t;

			localtime_s(&t,&tt);

			std::cout << std::put_time(&t, "%m/%d/%y %H:%M:%S");
#else
			std::cout << std::put_time(std::localtime(&tt), "%m/%d/%y %H:%M:%S");
#endif //WIN32

			std::cout << "." << std::setw(6) << std::setfill('0') << std::dec << timestamp % 1000000 << " [0x" << std::setw(8) << std::setfill('0') << source << "] [";
			switch (l)
			{
			case SOF_TRACE_FATAL:
				{
					std::cout << "FATAL] "; 

					break;
				}
			case SOF_TRACE_ERROR:
				{
					std::cout << "ERROR] "; 

					break;
				}
			case SOF_TRACE_WARNING:
				{
					std::cout << "WARNING] "; 

					break;
				}
			case SOF_TRACE_TEXT:
				{
					std::cout << "TEXT] "; 

					break;
				}
			case SOF_TRACE_DEBUG:
				{
					std::cout << "DEBUG] "; 

					break;
				}
			case SOF_TRACE_VERBOSE:
				{
					std::cout << "VERBOSE] "; 

					break;
				}
			default:
				{
					std::cout << "UNKNOWN] ";

				}
			}

			std::cout << msg << std::endl;
		}
	};
}}
#endif //SOFXX_DTRACE_HPP
/**
* 
* @file     dtrace
* @brief    Copyright (C) 2013  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2013/08/10
*/
#ifndef LEMONXX_DTRACE_HPP
#define LEMONXX_DTRACE_HPP
#include <chrono>
#include <iomanip>
#include <iostream>
#include <lemon/abi.h>
#include <lemonxx/sysm.hpp>
#include <lemonxx/nocopyable.hpp>


namespace lemon{namespace dtrace{

	enum class level
	{
		slience = LEMON_TRACE_SILENCE,
		
		fatal = LEMON_TRACE_FATAL,

		error = LEMON_TRACE_ERROR,

		warning = LEMON_TRACE_WARNING,

		text = LEMON_TRACE_TEXT,

		debug = LEMON_TRACE_DEBUG,

		verbos = LEMON_TRACE_VERBOSE
	};

	template<typename T> class basic_consumer : private nocopyable
	{
	public:
		basic_consumer(system& S,int levels):_sysm(S)
		{
			_handle = lemon_new_trace(_sysm,&basic_consumer<T>::__f,this,levels);

			_sysm.check_throw();
		}

		~basic_consumer()
		{
			lemon_close_trace(_sysm,_handle);
		}

	private:

		static void __f(void * userdata, int level, lemon_int64_t timestamp,lemon_t source, const char * msg)
		{
			reinterpret_cast<T*>(userdata)->consume(level,timestamp,source,msg);
		}

	private:

		system										&_sysm;

		lemon_trace_t									_handle;
	};

	class null_consumer : public basic_consumer<null_consumer>
	{
	public:
		null_consumer(system& S,int levels):basic_consumer(S,levels)
		{

		}

		void consume(int , lemon_int64_t ,lemon_t , const char * )
		{

		}
	};


	class console_consumer : public basic_consumer<console_consumer>
	{
	public:
		console_consumer(system& S,int levels):basic_consumer(S,levels)
		{

		}

		void consume(int l, lemon_int64_t timestamp,lemon_t source, const char * msg)
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
			case LEMON_TRACE_FATAL:
				{
					std::cout << "FATAL] "; 

					break;
				}
			case LEMON_TRACE_ERROR:
				{
					std::cout << "ERROR] "; 

					break;
				}
			case LEMON_TRACE_WARNING:
				{
					std::cout << "WARNING] "; 

					break;
				}
			case LEMON_TRACE_TEXT:
				{
					std::cout << "TEXT] "; 

					break;
				}
			case LEMON_TRACE_DEBUG:
				{
					std::cout << "DEBUG] "; 

					break;
				}
			case LEMON_TRACE_VERBOSE:
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
#endif //LEMONXX_DTRACE_HPP

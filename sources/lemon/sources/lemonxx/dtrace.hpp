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


	template<typename Consumer>
	class basic_consumer : private nocopyable
	{
	public:

		static void __f(void * userdata, int level, lemon_int64_t timestamp,lemon_t source, const char * msg)
		{
			reinterpret_cast<Consumer*>(userdata)->callback(level,timestamp,source,msg);
		}

		basic_consumer(lemon_state S,int levels):_S(S)
		{
			_handle = lemon_new_trace(S,&basic_consumer::__f,this,levels);

			lemon_check_throw(S);
		}

		~basic_consumer()
		{
			lemon_close_trace(_S,_handle);
		}

	private:

		lemon_state											_S;

		lemon_trace_t										_handle;
	};

	inline lemon_trace_t open_console(lemon_state S, int levels)
	{
		lemon_trace_t result = lemon_console_trace(S,levels);

		lemon_check_throw(S);

		return result;
	}
}}
#endif //LEMONXX_DTRACE_HPP

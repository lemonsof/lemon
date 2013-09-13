/**
* 
* @file     dtrace
* @brief    Copyright (C) 2013  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2013/08/10
*/
#ifndef LEMON_DTRACE_HPP
#define LEMON_DTRACE_HPP
#include <ctime>
#include <queue>
#include <atomic>
#include <thread>
#include <chrono>
#include <stdarg.h>
#include <lemon/abi.h>
#include <unordered_map>
#include <condition_variable>
#include <lemonxx/nocopyable.hpp>

namespace lemon{namespace kernel{

	struct lemon_trace_msg
	{
		lemon_trace_msg(lemon_t s, int l):counter(1),source(s),level(l)
		{
			using namespace std::chrono;
			using std::chrono::high_resolution_clock;

			auto duration = system_clock::now().time_since_epoch();

			timestamp = duration_cast<microseconds>(duration).count();

		}

		std::atomic<int>										counter;

		lemon_t													source;

		int														level;

		lemon_int64_t												timestamp;

		char													buff[1];
	};

	class lemon_trace_system;

	class lemon_trace : private nocopyable
	{
	public:

		typedef std::queue<lemon_trace_msg*>						msgs;

		lemon_trace(lemon_trace_system* sysm,lemon_trace_f f, void * userdata, int levels);

		~lemon_trace();

		void proc();

		void push(lemon_trace_msg * msg);

		void setlevels(int levels){ _flags = levels; }

		int getlevels() { return _flags; }

	private:

		int														_flags;

		lemon_trace_system										*_sysm;

		bool													_exit;

		lemon_trace_f												_f;

		void													*_userdata;

		std::mutex												_mutex;

		std::condition_variable									_condition;	

		msgs													_msgs;

		std::thread												_worker;
	};

	class lemon_trace_system : private nocopyable
	{
	public:
		typedef std::unordered_map<lemon_trace_t,lemon_trace*> traces_t;

		lemon_trace_system();

		~lemon_trace_system();

		void trace(int level,lemon_t source,const char * fmt,...)
		{
			va_list args;

			va_start(args,fmt);

			trace(source,level, fmt,args);
		}

		void trace(lemon_t source,int level, const char * fmt,va_list args);

		bool status(int level);

		void freemsg(lemon_trace_msg * msg);

		lemon_trace_t open_trace(lemon_trace_f f, void * userdata, int levels);

		void close_trace(lemon_trace_t trace);

		void setlevels(lemon_trace_t trace,int levels);

	private:

		void __flush_flags();

	private:

		uintptr_t												_seq;

		traces_t												_traces;

		int														_flags;

		std::mutex												_mutex;
	};

}}

#endif //LEMON_DTRACE_HPP

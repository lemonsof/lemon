/**
* 
* @file     dtrace
* @brief    Copyright (C) 2013  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2013/08/10
*/
#ifndef SOF_DTRACE_HPP
#define SOF_DTRACE_HPP
#include <ctime>
#include <queue>
#include <atomic>
#include <thread>
#include <chrono>
#include <stdarg.h>
#include <sof/abi.h>
#include <unordered_map>
#include <sofxx/dtrace.hpp>
#include <condition_variable>
#include <sofxx/nocopyable.hpp>

namespace sof{namespace impl{

	struct sof_trace_msg
	{
		sof_trace_msg(sof_t s, int l):counter(1),source(s),level(l)
		{
			using namespace std::chrono;
			using std::chrono::high_resolution_clock;

			auto duration = system_clock::now().time_since_epoch();

			timestamp = duration_cast<microseconds>(duration).count();

		}

		std::atomic<int>										counter;

		sof_t													source;

		int														level;

		sof_int64_t												timestamp;

		char													buff[1];
	};

	class sof_trace_system;

	class sof_trace : private nocopyable
	{
	public:

		typedef std::queue<sof_trace_msg*>						msgs;

		sof_trace(sof_trace_system* sysm,sof_trace_f f, void * userdata, int levels);

		~sof_trace();

		void proc();

		void push(sof_trace_msg * msg);

		void setlevels(int levels){ _flags = levels; }

		int getlevels() { return _flags; }

	private:

		int														_flags;

		sof_trace_system										*_sysm;

		bool													_exit;

		sof_trace_f												_f;

		void													*_userdata;

		std::mutex												_mutex;

		std::condition_variable									_condition;	

		msgs													_msgs;

		std::thread												_worker;
	};

	class sof_trace_system : private nocopyable
	{
	public:
		typedef std::unordered_map<sof_trace_t,sof_trace*> traces_t;

		sof_trace_system();

		~sof_trace_system();

		void trace(sof_t source,dtrace::level level, const char * fmt,va_list arg);

		bool status(dtrace::level level);

		void freemsg(sof_trace_msg * msg);

		sof_trace_t open_trace(sof_trace_f f, void * userdata, int levels);

		void close_trace(sof_trace_t trace);

		void setlevels(sof_trace_t trace,int levels);

	private:

		void __flush_flags();

	private:

		sof_trace_t												_seq;

		traces_t												_traces;

		int														_flags;

		std::mutex												_mutex;
	};

}}

#endif //SOF_DTRACE_HPP
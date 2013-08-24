#include <functional>
#include <lemon/kernel/dtrace.hpp>

namespace lemon{namespace kernel{

	lemon_trace::lemon_trace(lemon_trace_system* sysm,lemon_trace_f f, void * userdata, int levels) 
		:_flags(levels)
		,_sysm(sysm)
		,_exit(false)
		,_f(f)
		,_userdata(userdata)
		,_worker(std::bind(&lemon_trace::proc,this))
	{
		
	}

	lemon_trace::~lemon_trace()
	{
		{
			std::unique_lock<std::mutex>  lock(_mutex);

			_exit = true;

			_condition.notify_one();

		}

		_worker.join();

		while(!_msgs.empty())
		{
			auto msg = _msgs.front();

			try
			{
				_f(_userdata,msg->level,msg->timestamp,msg->source,msg->buff);
			}
			catch(...)
			{


			}

			_sysm->freemsg(msg);

			_msgs.pop();
		}
	}

	void lemon_trace::push(lemon_trace_msg * msg)
	{
		std::unique_lock<std::mutex>  lock(_mutex);

		if(msg->level & _flags)
		{
			msg->counter ++;

			_msgs.push(msg);

			_condition.notify_one();
		}
	}

	void lemon_trace::proc()
	{
		std::unique_lock<std::mutex>  lock(_mutex);

		while(!_exit)
		{
			if(_msgs.empty())
			{
				_condition.wait(lock);

				continue;
			}

			lemon_trace_msg * msg = _msgs.front();

			_msgs.pop();

			//lock.unlock();

			try
			{
				if(_f) _f(_userdata,msg->level,msg->timestamp,msg->source,msg->buff);
			}
			catch(...)
			{

			}

			_sysm->freemsg(msg);

			//lock.lock();
		}
	}

	lemon_trace_system::lemon_trace_system() : _seq(0)
	{

	}

	lemon_trace_system::~lemon_trace_system()
	{
		for(auto trace : _traces)
		{
			delete trace.second;
		}
	}

	void lemon_trace_system::trace(lemon_t source,int level, const char * fmt,va_list arg)
	{
		if(((int)level & _flags))
		{
			size_t blocksize = 512;

			lemon_trace_msg * traceMsg = NULL;

			for(;;)
			{
				void * block = malloc(blocksize);

				traceMsg = new(block) lemon_trace_msg(source,(int)level);

				size_t buffsize = blocksize - offsetof(lemon_trace_msg,buff);

				memset(traceMsg->buff,0,buffsize);
#ifdef WIN32
				if(-1 != vsnprintf_s(traceMsg->buff,buffsize,_TRUNCATE ,fmt,arg)) break;
#else
				if(-1 != vsnprintf_s(traceMsg->buff,buffsize,fmt,arg)) break;
#endif 

				blocksize *= 2;
			}

			for(auto trace : _traces)
			{
				trace.second->push(traceMsg);
			}

			freemsg(traceMsg);
		}
	}

	bool lemon_trace_system::status(int level)
	{
		return ((int)level & _flags) != 0;
	}

	void lemon_trace_system::freemsg(lemon_trace_msg * msg)
	{
		if( 0 == -- msg->counter)
		{
			msg->~lemon_trace_msg();

			::free(msg);
		}
	}

	void lemon_trace_system::close_trace(lemon_trace_t trace)
	{
		std::unique_lock<std::mutex>  lock(_mutex);

		auto iter = _traces.find(trace);

		if(iter != _traces.end())
		{
			delete iter->second;

			_traces.erase(iter);
		}
	}

	lemon_trace_t lemon_trace_system::open_trace(lemon_trace_f f, void * userdata, int levels)
	{
		std::unique_lock<std::mutex>  lock(_mutex);

		for(;;)
		{
			lemon_trace_t id = (lemon_trace_t)_seq ++;

			if(_traces.find(id) != _traces.end()) continue;

			_traces[id] = new lemon_trace(this,f,userdata,levels);

			__flush_flags();

			return id;
		}
	}

	void lemon_trace_system::setlevels(lemon_trace_t trace,int levels)
	{
		std::unique_lock<std::mutex>  lock(_mutex);

		auto iter = _traces.find(trace);

		if(iter == _traces.end()) return;

		iter->second->setlevels(levels);

		__flush_flags();
	}

	void lemon_trace_system::__flush_flags()
	{
		_flags = 0;

		for(auto trace : _traces)
		{
			_flags |= trace.second->getlevels();
		}
	}
}}


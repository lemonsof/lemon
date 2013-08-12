#include <sof/abi.h>
#include <sof/runq.hpp>
#include <sof/system.hpp>

using namespace sof;
using namespace sof::impl;

SOF_API sof_state sof_new(size_t maxcoros,size_t maxchannels)
{
	try
	{
		return (sof_state)(new main_sof_runq(maxcoros,maxchannels))->get_current();
	}
	catch(...)
	{
		return NULL;
	}
}

SOF_API void sof_stop(sof_state self)
{
	reinterpret_cast<sof_actor*>(self)->Q->stop();
}

SOF_API void sof_close(sof_state self)
{
	assert(reinterpret_cast<sof_actor*>(self)->Id == SOF_MAIN_ACTOR_ID);

	delete reinterpret_cast<sof_actor*>(self)->Q;
}

SOF_API void sof_raise_errno(sof_state self, const char* msg ,const sof_errno_info* info)
{
	reinterpret_cast<sof_actor*>(self)->Q->raise_error(msg,info);
}

SOF_API const sof_errno_info* sof_last_errno(sof_state self)
{
	return reinterpret_cast<sof_actor*>(self)->Q->last_errno();
}

SOF_API void sof_raise_trace(sof_state self, const char * file, int lines)
{
	reinterpret_cast<sof_actor*>(self)->Q->raise_trace(file,lines);
}

SOF_API void sof_reset_errno(sof_state self)
{
	reinterpret_cast<sof_actor*>(self)->Q->reset_errno();
}

SOF_API sof_trace_t sof_new_trace(sof_state self,sof_trace_f f, void * userdata, int levels)
{
	try
	{
		return reinterpret_cast<sof_actor*>(self)->Q->sysm()->trace_system().open_trace(f,userdata,levels);
	}
	catch(const error_info&)
	{
		return SOF_INVALID_HANDLE;
	}
	
}

SOF_API void sof_trace_levels(sof_state self,sof_trace_t trace,int levels)
{
	reinterpret_cast<sof_actor*>(self)->Q->sysm()->trace_system().setlevels(trace,levels);
}

SOF_API void sof_close_trace(sof_state self,sof_trace_t trace)
{
	reinterpret_cast<sof_actor*>(self)->Q->sysm()->trace_system().close_trace(trace);
}

SOF_API bool sof_trace_status(sof_state self, int level)
{
	return reinterpret_cast<sof_actor*>(self)->Q->sysm()->trace_system().status((dtrace::level)level);
}

SOF_API void __sof_trace(sof_state self,int level, const char * fmt, ...)
{
	va_list args;

	va_start(args,fmt);

	reinterpret_cast<sof_actor*>(self)->Q->trace((dtrace::level)level,fmt,args);
}

SOF_API sof_t sof_go(sof_state self, sof_f f, void * userdata,size_t stacksize)
{
	try
	{
		return reinterpret_cast<sof_actor*>(self)->Q->go(f,userdata,stacksize);
	}
	catch(const error_info&)
	{
		return SOF_INVALID_HANDLE;
	}
}


SOF_API sof_event_t sof_wait(sof_state self,const sof_mutext_t * mutex,const sof_event_t * waitlist, size_t len)
{
	try
	{
		return reinterpret_cast<sof_actor*>(self)->Q->wait(mutex,waitlist,len);
	}
	catch(const error_info&)
	{
		return SOF_INVALID_HANDLE;
	}
}

SOF_API bool sof_notify(sof_state self,sof_t target, const sof_event_t * waitlist, size_t len)
{
	try
	{
		return reinterpret_cast<sof_actor*>(self)->Q->notify(target,waitlist,len);
	}
	catch(const error_info&)
	{
		return false;
	}
}

SOF_API void sof_join(sof_state self)
{
	try
	{
		reinterpret_cast<sof_actor*>(self)->Q->join();
	}
	catch(const error_info&)
	{
		
	}
}
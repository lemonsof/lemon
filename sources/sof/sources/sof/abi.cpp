#include <sof/abi.h>
#include <sof/runq.hpp>
#include <sof/system.hpp>

using namespace sof;
using namespace sof::impl;

SOF_API sof_state sof_new(size_t maxcoros,size_t maxchannels)
{
	try
	{
		return (sof_state)new main_sof_runq(maxcoros,maxchannels);
	}
	catch(...)
	{
		return NULL;
	}
}

SOF_API void sof_close(sof_state self)
{
	delete reinterpret_cast<main_sof_runq*>(self);
}

SOF_API void sof_raise_errno(sof_state self, const char* msg ,const sof_errno_info* info)
{
	reinterpret_cast<basic_sof_runq*>(self)->raise_error(self,msg,info);
}

SOF_API const sof_errno_info* sof_last_errno(sof_state self)
{
	return reinterpret_cast<basic_sof_runq*>(self)->last_errno();
}

SOF_API void sof_raise_trace(sof_state self, const char * file, int lines)
{
	reinterpret_cast<basic_sof_runq*>(self)->raise_trace(self,file,lines);
}

SOF_API void sof_reset_errno(sof_state self)
{
	reinterpret_cast<basic_sof_runq*>(self)->reset_errno();
}

SOF_API sof_trace_t sof_new_trace(sof_state self,sof_trace_f f, void * userdata, int levels)
{
	try
	{
		return reinterpret_cast<basic_sof_runq*>(self)->sysm()->trace_system().open_trace(f,userdata,levels);
	}
	catch(const error_info&)
	{
		return SOF_INVALID_HANDLE;
	}
	
}

SOF_API void sof_trace_levels(sof_state self,sof_trace_t trace,int levels)
{
	reinterpret_cast<basic_sof_runq*>(self)->sysm()->trace_system().setlevels(trace,levels);
}

SOF_API void sof_close_trace(sof_state self,sof_trace_t trace)
{
	reinterpret_cast<basic_sof_runq*>(self)->sysm()->trace_system().close_trace(trace);
}

SOF_API bool sof_trace_status(sof_state self, int level)
{
	return reinterpret_cast<basic_sof_runq*>(self)->sysm()->trace_system().status((dtrace::level)level);
}

SOF_API void __sof_trace(sof_state self,int level, const char * fmt, ...)
{
	va_list args;

	va_start(args,fmt);

	reinterpret_cast<basic_sof_runq*>(self)->trace((dtrace::level)level,fmt,args);
}
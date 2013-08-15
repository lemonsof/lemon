#include <lemon/abi.h>
#include <lemon/kernel/runq.hpp>
#include <lemon/kernel/system.hpp>

using namespace lemon;
using namespace lemon::impl;

LEMON_API lemon_state lemon_new(size_t maxcoros,size_t maxchannels)
{
	try
	{
		return (lemon_state)(new main_lemon_runq(maxcoros,maxchannels))->main_actor();
	}
	catch(...)
	{
		return NULL;
	}
}

LEMON_API void lemon_stop(lemon_state self)
{
	reinterpret_cast<lemon_actor*>(self)->System->stop();
}

LEMON_API void lemon_close(lemon_state self)
{
	assert(reinterpret_cast<lemon_actor*>(self)->Id == LEMON_MAIN_ACTOR_ID);

	delete reinterpret_cast<lemon_actor*>(self)->Q;
}

LEMON_API const lemon_errno_info* lemon_raise_errno__(lemon_state self, const char* msg , uintptr_t code,const lemon_uuid_t * catalog,const char * file,int lines)
{
	lemon_actor * actor = reinterpret_cast<lemon_actor*>(self);

	actor->LastError.error.code = code;

	actor->LastError.error.catalog = catalog;

	actor->LastError.file = file;

	actor->LastError.lines = lines;

	const lemon_byte_t * bytes = (const lemon_byte_t *)catalog;

	if(msg)
	{
		lemon_log_error(self,"raise exception: %s\n\tcode:%d\n\t:%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x\n\tfile:%s\n\tlines:%d\n\t",
			msg,
			code,
			bytes[3],bytes[2],bytes[1],bytes[0],bytes[5],bytes[4],bytes[7],bytes[6],
			bytes[8],bytes[9],bytes[10],bytes[11],bytes[12],bytes[13],bytes[14],bytes[15],
			file,
			lines);
	}
	else
	{
		lemon_log_error(self,"raise exception: \n\tcode:%d\n\t:%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x\n\tfile:%s\n\tlines:%d\n\t",
			code,
			bytes[3],bytes[2],bytes[1],bytes[0],bytes[5],bytes[4],bytes[7],bytes[6],
			bytes[8],bytes[9],bytes[10],bytes[11],bytes[12],bytes[13],bytes[14],bytes[15],
			file,
			lines);
	}

	return &actor->LastError;
}

LEMON_API const lemon_errno_info* lemon_last_errno(lemon_state self)
{
	if(lemon_failed(reinterpret_cast<lemon_actor*>(self)->LastError))
	{
		return &reinterpret_cast<lemon_actor*>(self)->LastError;
	}

	return nullptr;
}

LEMON_API void lemon_raise_trace__(lemon_state self, const char * file, int lines)
{
	lemon_actor * actor = reinterpret_cast<lemon_actor*>(self);

	const lemon_byte_t * bytes = (const lemon_byte_t *)actor->LastError.error.catalog;

	lemon_log_error(self,"trace exception\n\tcode:%d\n\t:%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x\n\tfile:%s\n\tlines:%d\n\t",
		actor->LastError.error.code,
		bytes[3],bytes[2],bytes[1],bytes[0],bytes[5],bytes[4],bytes[7],bytes[6],
		bytes[8],bytes[9],bytes[10],bytes[11],bytes[12],bytes[13],bytes[14],bytes[15],
		file,
		lines);
}

LEMON_API void lemon_reset_errno(lemon_state self)
{
	lemon_reset_errorinfo(reinterpret_cast<lemon_actor*>(self)->LastError);
}

LEMON_API lemon_trace_t lemon_new_trace(lemon_state self,lemon_trace_f f, void * userdata, int levels)
{
	try
	{
		return reinterpret_cast<lemon_actor*>(self)->System->trace_system().open_trace(f,userdata,levels);
	}
	catch(const lemon_errno_info &)
	{
		lemon_raise_trace(self);

		return LEMON_INVALID_HANDLE;
	}
	
}

LEMON_API void lemon_trace_levels(lemon_state self,lemon_trace_t trace,int levels)
{
	reinterpret_cast<lemon_actor*>(self)->System->trace_system().setlevels(trace,levels);
}

LEMON_API void lemon_close_trace(lemon_state self,lemon_trace_t trace)
{
	reinterpret_cast<lemon_actor*>(self)->System->trace_system().close_trace(trace);
}

LEMON_API bool lemon_trace_status(lemon_state self, int level)
{
	return reinterpret_cast<lemon_actor*>(self)->System->trace_system().status((dtrace::level)level);
}

LEMON_API void __lemon_trace(lemon_state self,int level, const char * fmt, ...)
{
	va_list args;

	va_start(args,fmt);

	auto actor = reinterpret_cast<lemon_actor*>(self);

	actor->System->trace_system().trace(actor->Id,(dtrace::level)level,fmt,args);
}

LEMON_API lemon_t lemon_go(lemon_state self, lemon_f f, void * userdata,size_t stacksize)
{
	try
	{
		return reinterpret_cast<lemon_actor*>(self)->System->go(self,f,userdata,stacksize);
	}
	catch(const lemon_errno_info &)
	{
		lemon_raise_trace(self);

		return LEMON_INVALID_HANDLE;
	}
}


LEMON_API lemon_event_t lemon_wait(lemon_state self,const lemon_mutext_t * mutex,const lemon_event_t * waitlist, size_t len,size_t timeout)
{
	lemon_actor * actor = reinterpret_cast<lemon_actor*>(self);

	if(actor->Id == LEMON_MAIN_ACTOR_ID)
	{
		lemon_raise_errno(self,NULL,LEMON_UNSUPPORT_OPTION);

		return LEMON_INVALID_HANDLE;
	}

	if(actor->Exit & (int)exit_status::killed)
	{
		lemon_raise_errno(self,NULL,LEMON_KILLED);

		return LEMON_INVALID_HANDLE;
	}

	if(mutex) actor->Mutex = *mutex;

	actor->WaitList = waitlist;

	actor->Waits = len;

	actor->Timeout = timeout;

	actor->WaitResult = LEMON_INVALID_HANDLE;

	lemon_context_jump(actor->Context,actor->Q->context(),0);

	if(actor->Exit & (int)exit_status::killed)
	{
		lemon_raise_errno(self,NULL,LEMON_KILLED);

		return LEMON_INVALID_HANDLE;
	}

	actor->Mutex.mutex = nullptr;

	actor->WaitList = nullptr;

	actor->Waits = 0;

	return actor->WaitResult;
}

LEMON_API bool lemon_notify(lemon_state self,lemon_t target, const lemon_event_t * waitlist, size_t len)
{
	try
	{
		return reinterpret_cast<lemon_actor*>(self)->System->notify(target,waitlist,len);;
	}
	catch(const lemon_errno_info&)
	{
		lemon_raise_trace(self);

		return false;
	}
}

LEMON_API void lemon_join(lemon_state self)
{
	try
	{
		if(reinterpret_cast<lemon_actor*>(self)->Id != LEMON_MAIN_ACTOR_ID)
		{
			lemon_raise_errno(self,NULL,LEMON_KILLED);

			return;
		}

		reinterpret_cast<lemon_actor*>(self)->Q->join();
	}
	catch(const lemon_errno_info&)
	{
		lemon_raise_trace(self);
	}
}

LEMON_API void lemon_new_extension(lemon_state self, const lemon_extension_vtable * vtable, void * userdata)
{
	try
	{
		reinterpret_cast<lemon_actor*>(self)->System->new_extension(vtable,userdata);
	}
	catch(const lemon_errno_info&)
	{
		lemon_raise_trace(self);
	}
}
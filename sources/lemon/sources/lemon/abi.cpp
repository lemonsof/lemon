#include <assert.h>
#include <lemonxx/buffer.hpp>
#include <lemon/kernel/func.hpp>
#include <lemon/kernel/system.hpp>

using namespace lemon;
using namespace lemon::kernel;

LEMON_API lemon_state lemon_new(size_t maxcoros,size_t maxchannels,size_t stacksize)
{
	try
	{
		return (lemon_state)(new lemon_system(maxcoros,maxchannels,stacksize))->main_actor();
	}
	catch(...)
	{
		return NULL;
	}
}

LEMON_API void lemon_stop(lemon_state self)
{
	reinterpret_cast<lemon_actor*>(self)->get_system()->stop();
}

LEMON_API void lemon_close(lemon_state self)
{
	auto actor = reinterpret_cast<lemon_actor*>(self);

	assert(actor->get_system()->main_actor() == actor);

	if(actor != actor->get_system()->main_actor())
	{
		lemon_declare_errinfo(errorCode);

		lemon_user_errno(errorCode,LEMON_UNSUPPORT_OPTION);

		actor->lasterror_reset(errorCode);

		return;
	}

	delete actor->get_system();
}


LEMON_API const lemon_errno_info* lemon_raise_errno__(lemon_state self, const char* msg , uintptr_t code,const lemon_uuid_t * catalog,const char * file,int lines)
{
	lemon_actor * actor = reinterpret_cast<lemon_actor*>(self);

	actor->lasterror_reset(code,catalog,file,lines);

	const lemon_byte_t * bytes = (const lemon_byte_t *)catalog;

	if(msg)
	{
		lemon_log_error(actor->get_system()->trace_system(),*actor,"raise exception: %s\n\tcode:%d\n\t:%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x\n\tfile:%s\n\tlines:%d\n\t",
			msg,
			code,
			bytes[3],bytes[2],bytes[1],bytes[0],bytes[5],bytes[4],bytes[7],bytes[6],
			bytes[8],bytes[9],bytes[10],bytes[11],bytes[12],bytes[13],bytes[14],bytes[15],
			file,
			lines);
	}
	else
	{
		lemon_log_error(actor->get_system()->trace_system(),*actor,"raise exception: \n\tcode:%d\n\t:%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x\n\tfile:%s\n\tlines:%d\n\t",
			code,
			bytes[3],bytes[2],bytes[1],bytes[0],bytes[5],bytes[4],bytes[7],bytes[6],
			bytes[8],bytes[9],bytes[10],bytes[11],bytes[12],bytes[13],bytes[14],bytes[15],
			file,
			lines);
	}

	return &actor->lasterror();
}

LEMON_API const lemon_errno_info* lemon_last_errno(lemon_state self)
{
	if(reinterpret_cast<lemon_actor*>(self)->failed())
	{
		return &reinterpret_cast<lemon_actor*>(self)->lasterror();
	}

	return nullptr;
}

LEMON_API void lemon_reset_errno(lemon_state self)
{
	reinterpret_cast<lemon_actor*>(self)->lasterror_reset();
}

LEMON_API lemon_trace_t lemon_new_trace(lemon_state self,lemon_trace_f f, void * userdata, int levels)
{
	auto actor = reinterpret_cast<lemon_actor*>(self);

	if(actor != actor->get_system()->main_actor())
	{
		lemon_log_error(actor->get_system()->trace_system(),*actor,"call lemon_new_trace only in main actor");

		lemon_declare_errinfo(errorCode);
		
		lemon_user_errno(errorCode,LEMON_UNSUPPORT_OPTION);

		actor->lasterror_reset(errorCode);

		return LEMON_INVALID_HANDLE(lemon_trace_t);
	}

	try
	{
		return actor->get_system()->trace_system().open_trace(f,userdata,levels);
	}
	catch(const lemon_errno_info & e)
	{
		lemon_raise_errninfo(self,"call lemon_new_trace failed",e);
		
		return LEMON_INVALID_HANDLE(lemon_trace_t);
	}
}


LEMON_API void lemon_trace_levels(lemon_state self,lemon_trace_t trace,int levels)
{
	reinterpret_cast<lemon_actor*>(self)->get_system()->trace_system().setlevels(trace,levels);
}

LEMON_API void lemon_close_trace(lemon_state self,lemon_trace_t trace)
{
	reinterpret_cast<lemon_actor*>(self)->get_system()->trace_system().close_trace(trace);
}

LEMON_API bool lemon_trace_status(lemon_state self, int level)
{
	return reinterpret_cast<lemon_actor*>(self)->get_system()->trace_system().status(level);
}

LEMON_API void __lemon_trace(lemon_state self,int level, const char * fmt, ...)
{
	va_list args;

	va_start(args,fmt);

	auto actor = reinterpret_cast<lemon_actor*>(self);

	actor->get_system()->trace_system().trace(*actor,level,fmt,args);
}


LEMON_API lemon_t lemon_go(lemon_state self, lemon_f f, void * userdata,size_t stacksize)
{
	auto actor = reinterpret_cast<lemon_actor*>(self);

	if(actor->killed())
	{
		lemon_raise_errno(self,"the actor is killed",LEMON_KILLED);

		return false;
	}

	actor->lasterror_reset();

	try
	{
		return actor->get_system()->go(*actor,f,userdata,stacksize);
	}
	catch(const lemon_errno_info & e)
	{
		lemon_raise_errninfo(self,"call lemon_go failed",e);

		return LEMON_INVALID_HANDLE(lemon_t);
	}
}

LEMON_API lemon_event_t lemon_wait(lemon_state self,const lemon_mutext_t * mutex,const lemon_event_t * waitlist, size_t len,size_t timeout)
{
	lemon_actor * actor = reinterpret_cast<lemon_actor*>(self);

	try
	{
		return wait(actor,mutex,cbuff(waitlist,len),timeout);
	}
	catch(const lemon_errno_info& e)
	{
		lemon_raise_errninfo(self,"call lemon_wait failed",e);

		return LEMON_INVALID_HANDLE(lemon_event_t);
	}

	
}

LEMON_API bool lemon_notify(lemon_state self,lemon_t target, const lemon_event_t * waitlist, size_t len)
{
	lemon_actor * actor = reinterpret_cast<lemon_actor*>(self);

	if(actor->killed())
	{
		lemon_raise_errno(self,"the actor is killed",LEMON_KILLED);

		return false;
	}

	actor->lasterror_reset();

	try
	{
		
		return actor->get_system()->notify(target,lemon::cbuff(waitlist,len));
	}
	catch(const lemon_errno_info& e)
	{
		lemon_raise_errninfo(self,"call lemon_notify failed",e);

		return false;
	}
}

LEMON_API void lemon_join(lemon_state self)
{
	lemon_actor * actor = reinterpret_cast<lemon_actor*>(self);

	actor->lasterror_reset();

	if(actor != actor->get_system()->main_actor())
	{
		lemon_raise_errno(self,"call lemon_join only in main actor",LEMON_UNSUPPORT_OPTION);

		return ;
	}

	try
	{
		reinterpret_cast<lemon_actor*>(self)->get_system()->join();
	}
	catch(const lemon_errno_info& e)
	{
		lemon_raise_errninfo(self,"call lemon_join failed",e);
	}
}

LEMON_API void lemon_new_extension(lemon_state self, const lemon_extension_vtable * vtable, void * userdata)
{
	lemon_actor * actor = reinterpret_cast<lemon_actor*>(self);

	if(actor != actor->get_system()->main_actor())
	{
		lemon_raise_errno(self,"call lemon_new_extension only in main actor",LEMON_UNSUPPORT_OPTION);

		return ;
	}

	actor->lasterror_reset();

	try
	{
		actor->get_system()->new_extension(*actor,vtable,userdata);
	}
	catch(const lemon_errno_info& e)
	{
		lemon_raise_errninfo(self,"call lemon_new_extension failed",e);
	}
}


LEMON_API lemon_channel_t lemon_new_channel(lemon_state S, int type,size_t maxlen,lemon_msg_f f,void * userdata)
{
	lemon_actor * actor = reinterpret_cast<lemon_actor*>(S);

	if(actor->killed())
	{
		lemon_raise_errno(S,"the actor is killed",LEMON_KILLED);

		return LEMON_INVALID_HANDLE(lemon_channel_t);
	}

	actor->lasterror_reset();

	try
	{
		return actor->get_system()->channel_system().make_channel(*actor,type,maxlen,f,userdata);
	}
	catch(const lemon_errno_info& e)
	{
		lemon_raise_errninfo(S,"call lemon_new_extension failed",e);

		return LEMON_INVALID_HANDLE(lemon_channel_t);
	}
}

LEMON_API void lemon_close_channel(lemon_state S, lemon_channel_t channel)
{
	lemon_actor * actor = reinterpret_cast<lemon_actor*>(S);

	if(actor->killed())
	{
		lemon_raise_errno(S,"the actor is killed",LEMON_KILLED);

		return;
	}

	actor->lasterror_reset();

	try
	{
		actor->get_system()->channel_system().close_channel(*actor,channel);
	}
	catch(const lemon_errno_info& e)
	{
		lemon_raise_errninfo(S,"call lemon_close_channel failed",e);
	}
}

LEMON_API void* lemon_recv(lemon_state S, lemon_channel_t channel,int flags,size_t timeout)
{
	lemon_actor * actor = reinterpret_cast<lemon_actor*>(S);

	if(actor->killed())
	{
		lemon_raise_errno(S,"the actor is killed",LEMON_KILLED);

		return nullptr;
	}

	actor->lasterror_reset();

	try
	{
		void* result = actor->get_system()->channel_system().recv(*actor,channel,flags,timeout);

		if(actor->killed())
		{
			lemon_raise_errno(S,"the actor is killed",LEMON_KILLED);

			return nullptr;
		}

		return result;
	}
	catch(const lemon_errno_info& e)
	{
		lemon_raise_errninfo(S,"call lemon_recv failed",e);

		return nullptr;
	}
}

LEMON_API bool lemon_send(lemon_state S, lemon_channel_t channel,void* msg,int flags,size_t timeout)
{
	lemon_actor * actor = reinterpret_cast<lemon_actor*>(S);

	if(actor->killed())
	{
		lemon_raise_errno(S,"the actor is killed",LEMON_KILLED);

		return false;
	}

	actor->lasterror_reset();

	try
	{
		bool status = actor->get_system()->channel_system().send(*actor,channel,msg,flags,timeout);

		if(actor->killed())
		{
			lemon_raise_errno(S,"the actor is killed",LEMON_KILLED);

			return false;
		}

		return status;
	}
	catch(const lemon_errno_info& e)
	{
		lemon_raise_errninfo(S,"call lemon_send failed",e);

		return false;
	}
}
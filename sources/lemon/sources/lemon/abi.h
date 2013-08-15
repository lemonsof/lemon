/**
* 
* @file     abi
* @brief    Copyright (C) 2013  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2013/08/07
*/
#ifndef LEMON_ABI_H
#define LEMON_ABI_H

#include <lemon/configure.h>


#define LEMON_HANDLE_STRUCT_NAME(name) name##__

#define LEMON_DECLARE_HANDLE(name) typedef struct LEMON_HANDLE_STRUCT_NAME(name) *name;

#define LEMON_IMPLEMENT_HANDLE(name) struct LEMON_HANDLE_STRUCT_NAME(name)

//////////////////////////////////////////////////////////////////////////
#define lemon_failed(ei)							((ei).error.catalog != NULL)	

#define lemon_success(ei)							(!lemon_failed(ei))

#define lemon_reset_errorinfo(ei)					ei.error.catalog = NULL

#define lemon_raise_trace(S)						lemon_raise_trace__(S,__FILE__,__LINE__)

#define lemon_raise_errno(S,msg,error)				*lemon_raise_errno__(S,msg,error.code,error.catalog,__FILE__,__LINE__)

#define lemon_raise_win32_errno(S,msg,error)		*lemon_raise_errno__(S,msg,error.code,&LEMON_WIN32_ERROR_CATALOG,__FILE__,__LINE__)

#define lemon_raise_posix_errno(S,msg,error)		*lemon_raise_errno__(S,msg,error.code,&LEMON_POSIX_ERROR_CATALOG,__FILE__,__LINE__)

#define lemon_raise_com_errno(S,msg,error)			*lemon_raise_errno__(S,msg,error.code,&LEMON_COM_ERROR_CATALOG,__FILE__,__LINE__)

//////////////////////////////////////////////////////////////////////////

#define lemon_log(self,level,...)					if(lemon_trace_status(self,level)){ __lemon_trace(self,level,__VA_ARGS__);}

#define lemon_log_verbose(provider,...)				lemon_log((provider),LEMON_TRACE_VERBOSE,__VA_ARGS__)

#define lemon_log_debug(provider,...)				lemon_log((provider),LEMON_TRACE_DEBUG,__VA_ARGS__)

#define lemon_log_text(provider,...)				lemon_log((provider),LEMON_TRACE_TEXT,__VA_ARGS__)

#define lemon_log_warn(provider,...)				lemon_log((provider),LEMON_TRACE_WARNING,__VA_ARGS__)

#define lemon_log_error(provider,...)				lemon_log((provider),LEMON_TRACE_ERROR,__VA_ARGS__)

#define lemon_log_fatal(provider,...)				lemon_log((provider),LEMON_TRACE_FATAL,__VA_ARGS__)

//////////////////////////////////////////////////////////////////////////

#define LEMON_SYNC_SEND								0x01
#define LEMON_SINGLE_RECV							0x02

#define LEMON_TRACE_SILENCE							0x00
#define LEMON_TRACE_FATAL							0x01
#define LEMON_TRACE_ERROR							0x02
#define LEMON_TRACE_WARNING							0x04
#define LEMON_TRACE_TEXT							0x08
#define LEMON_TRACE_DEBUG							0x10
#define LEMON_TRACE_VERBOSE							0x20

#define LEMON_ACTOR_UNKNOWN							0x00
#define LEMON_ACTOR_INIT							0x01
#define LEMON_ACTOR_RUNNING							0x02
#define LEMON_ACTOR_SLEEPING						0x03
#define LEMON_ACTOR_STOPPED							0x04

#define LEMON_TRACE_ALL								(LEMON_TRACE_FATAL|LEMON_TRACE_ERROR|LEMON_TRACE_WARNING|LEMON_TRACE_TEXT|LEMON_TRACE_DEBUG|LEMON_TRACE_VERBOSE)


#define LEMON_MAX_COROS								(1024 * 1024 * 64)
#define LEMON_MAX_CHANNELS							(LEMON_MAX_COROS * 4)

#define LEMON_INVALID_HANDLE						(uintptr_t)-1

#define LEMON_MAIN_ACTOR_ID							0

#define LEMON_DEFAULT_STACKSIZE						(1024 * 4)

#define LEMON_INFINITE								(size_t)-1

#define LEMON_MS_OF_TICK							10
//////////////////////////////////////////////////////////////////////////


typedef uintptr_t									lemon_t;

typedef uintptr_t									lemon_channel_t;

typedef uintptr_t									lemon_event_t;

typedef uintptr_t                                   lemon_trace_t;	

typedef uintptr_t									lemon_extension_t;

LEMON_DECLARE_HANDLE								(lemon_state);


typedef void										(*lemon_f)(lemon_state self,void * userdata);
typedef void										(*lemon_msg_close_f)(void * userdata, void * msg);
typedef void										(*lemon_trace_f)(void * userdata, int level, lemon_int64_t timestamp,lemon_t source, const char * msg);

//////////////////////////////////////////////////////////////////////////
typedef struct lemon_uuid_t{
	lemon_uint32_t	Data1;
	lemon_uint16_t	Data2;
	lemon_uint16_t	Data3;
	lemon_byte_t	Data4[8];
}													lemon_uuid_t;

typedef struct{

	const lemon_uuid_t *							catalog;

	uintptr_t										code;

}													lemon_errno_t;

typedef struct{

	lemon_errno_t									error;

	const char*										file;

	int												lines;

}													lemon_errno_info;

typedef struct{

	lemon_uint16_t									major;
	
	lemon_uint16_t									minor;
	
	lemon_uint16_t									build;
	
	lemon_uint16_t									reversion;

}													lemon_version_t;

typedef struct{
	void											(*invoke_send)(lemon_state state,void * userdata, void * msg);
	void											(*invoke_recv)(lemon_state state,void * userdata, void * msg);
}													lemon_channel_vtable;

typedef struct{
	void											*msg;
	lemon_msg_close_f								closef;
	void											*userdata;
}													lemon_msg;

typedef struct{
	lemon_extension_t								id;
	void											(*start)(void * userdata,lemon_state S);
	void											(*stop)(void * userdata);
	void											(*close)(void * userdata);
}													lemon_extension_vtable;

typedef struct{
	void											*mutex;
	void											(*lock)(void *mutex);
	void											(*unlock)(void *mutex);
}													lemon_mutext_t;


LEMON_API lemon_uuid_t								LEMON_WIN32_ERROR_CATALOG;

LEMON_API lemon_uuid_t								LEMON_COM_ERROR_CATALOG;

LEMON_API lemon_uuid_t								LEMON_POSIX_ERROR_CATALOG;

LEMON_API lemon_uuid_t								LEMON_UNITTEST_ERROR_CATALOG;

//////////////////////////////////////////////////////////////////////////

LEMON_API lemon_state lemon_new(size_t maxcoros,size_t maxchannels);

LEMON_API void lemon_stop(lemon_state self);

LEMON_API void lemon_close(lemon_state self);

LEMON_API void lemon_join(lemon_state self);

LEMON_API const lemon_errno_info* lemon_raise_errno__(lemon_state self, const char* msg , uintptr_t code,const lemon_uuid_t * catalog,const char * file,int lines);

LEMON_API void lemon_raise_trace__(lemon_state self, const char * file, int lines);

LEMON_API void lemon_reset_errno(lemon_state self);

LEMON_API const lemon_errno_info* lemon_last_errno(lemon_state self);

LEMON_API lemon_trace_t lemon_new_trace(lemon_state self,lemon_trace_f f, void * userdata, int levels);

LEMON_API void lemon_trace_levels(lemon_state self,lemon_trace_t trace,int levels);

LEMON_API void lemon_close_trace(lemon_state self,lemon_trace_t trace);

LEMON_API bool lemon_trace_status(lemon_state self, int level);

LEMON_API void __lemon_trace(lemon_state self, int level,const char * fmt, ...);

LEMON_API void lemon_new_extension(lemon_state self, const lemon_extension_vtable * vtable, void * userdata);

LEMON_API lemon_t lemon_go(lemon_state self, lemon_f f, void * userdata,size_t stacksize = LEMON_DEFAULT_STACKSIZE);

LEMON_API lemon_event_t lemon_wait(lemon_state self,const lemon_mutext_t * mutex,const lemon_event_t * waitlist, size_t len,size_t timeout = LEMON_INFINITE);

LEMON_API bool lemon_notify(lemon_state sel,lemon_t target, const lemon_event_t * waitlist, size_t len);

//////////////////////////////////////////////////////////////////////////


#endif //LEMON_ABI_H
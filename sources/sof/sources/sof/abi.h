/**
* 
* @file     abi
* @brief    Copyright (C) 2013  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2013/08/07
*/
#ifndef SOF_ABI_H
#define SOF_ABI_H

#include <sof/configure.h>

#define SOF_SYNC_SEND					0x01
#define SOF_SINGLE_RECV					0x02

#define SOF_TRACE_SILENCE				0x00
#define SOF_TRACE_FATAL					0x01
#define SOF_TRACE_ERROR					0x02
#define SOF_TRACE_WARNING				0x04
#define SOF_TRACE_TEXT					0x08
#define SOF_TRACE_DEBUG					0x10
#define SOF_TRACE_VERBOSE				0x20

#define SOF_ACTOR_UNKNOWN				0x00
#define SOF_ACTOR_INIT					0x01
#define SOF_ACTOR_RUNNING				0x02
#define SOF_ACTOR_SLEEPING				0x03
#define SOF_ACTOR_STOPPED				0x04

#define SOF_TRACE_ALL					(SOF_TRACE_FATAL|SOF_TRACE_ERROR|SOF_TRACE_WARNING|SOF_TRACE_TEXT|SOF_TRACE_DEBUG|SOF_TRACE_VERBOSE)


#define SOF_MAX_COROS					(1024 * 1024 * 64)
#define SOF_MAX_CHANNELS				(SOF_MAX_COROS * 4)

#define SOF_INVALID_HANDLE				(uintptr_t)-1

#define SOF_MAIN_ACTOR_ID				0

//////////////////////////////////////////////////////////////////////////

#define SOF_HANDLE_STRUCT_NAME(name) name##__

#define SOF_DECLARE_HANDLE(name) typedef struct SOF_HANDLE_STRUCT_NAME(name) *name;

#define SOF_IMPLEMENT_HANDLE(name) struct SOF_HANDLE_STRUCT_NAME(name)

//////////////////////////////////////////////////////////////////////////

typedef uintptr_t									sof_t;

typedef uintptr_t									sof_channel_t;

typedef uintptr_t									sof_event_t;

typedef uintptr_t                                   sof_trace_t;	

typedef uintptr_t									sof_extension_t;

SOF_DECLARE_HANDLE									(sof_state);


typedef void										(*sof_f)(sof_state self,void * userdata);
typedef void										(*sof_msg_close_f)(void * userdata, void * msg);
typedef void										(*sof_trace_f)(void * userdata, int level, sof_int64_t timestamp,sof_t source, const char * msg);

//////////////////////////////////////////////////////////////////////////
typedef struct sof_uuid_t{
	sof_uint32_t	Data1;
	sof_uint16_t	Data2;
	sof_uint16_t	Data3;
	sof_byte_t		Data4[8];
}													sof_uuid_t;

typedef struct{

	const sof_uuid_t *								catalog;

	uintptr_t										code;

}													sof_errno_t;

typedef struct{

	sof_errno_t										error;

	const char*										file;

	int												lines;

}													sof_errno_info;

typedef struct{

	sof_uint16_t									major;
	
	sof_uint16_t									minor;
	
	sof_uint16_t									build;
	
	sof_uint16_t									reversion;

}													sof_version_t;

typedef struct{
	void											(*invoke_send)(sof_state state,void * userdata, void * msg);
	void											(*invoke_recv)(sof_state state,void * userdata, void * msg);
}													sof_channel_vtable;

typedef struct{
	void											*msg;
	sof_msg_close_f									closef;
	void											*userdata;
}													sof_msg;

typedef struct{
	sof_extension_t									id;
	void											(*stop)(sof_state S, void * userdata);
	void											(*close)(void * userdata);
}													sof_extension_vtable;



//////////////////////////////////////////////////////////////////////////

SOF_API sof_uuid_t SOF_WIN32_ERROR_CATALOG;

SOF_API sof_uuid_t SOF_COM_ERROR_CATALOG;

SOF_API sof_uuid_t SOF_POSIX_ERROR_CATALOG;

SOF_API sof_uuid_t SOF_UNITTEST_ERROR_CATALOG;

#define SOF_RESET_ERRORINFO(errorinfo) {(errorinfo).error.catalog = 0;}

#define SOF_DECLARE_ERRORINFO(name) sof_errno_info name = {{0,0},0,0}

#define SOF_SUCCESS(errorinfo) ((errorinfo).error.catalog == 0)

#define SOF_FAILED(errorinfo) ((errorinfo).error.catalog != 0)

#define SOF_MAKE_ERROR(error,catalog,errorCode) {error.catalog = &catalog;error.code = errorCode;}

#define SOF_MAKE_ERRORINFO(errorinfo,catalog,errorCode) \
	SOF_MAKE_ERROR(errorinfo.error,catalog,errorCode);\
	errorinfo.file = __FILE__;\
	errorinfo.lines = __LINE__;

#define SOF_WIN32_ERROR(errorinfo,ec) SOF_MAKE_ERRORINFO((errorinfo),SOF_WIN32_ERROR_CATALOG,ec)

#define SOF_COM_ERROR(errorinfo,ec) SOF_MAKE_ERRORINFO((errorinfo),SOF_COM_ERROR_CATALOG,ec)

#define SOF_POSIX_ERROR(errorinfo,ec) SOF_MAKE_ERRORINFO((errorinfo),SOF_POSIX_ERROR_CATALOG,ec)

#define SOF_UNITTEST_ERROR(errorinfo,ec) SOF_MAKE_ERRORINFO((errorinfo),SOF_UNITTEST_ERROR_CATALOG,ec)

#define SOF_USER_ERROR(errorinfo,ec) {(errorinfo).error = ec;(errorinfo).file = __FILE__; (errorinfo).lines = __LINE__;}


#define sof_log(self,level,...)\
	if(sof_trace_status(self,level)){\
		__sof_trace(self,level,__VA_ARGS__);\
	}

#define sof_log_verbose(provider,...)		sof_log((provider),SOF_TRACE_VERBOSE,__VA_ARGS__)

#define sof_log_debug(provider,...)			sof_log((provider),SOF_TRACE_DEBUG,__VA_ARGS__)

#define sof_log_text(provider,...)			sof_log((provider),SOF_TRACE_TEXT,__VA_ARGS__)

#define sof_log_warn(provider,...)			sof_log((provider),SOF_TRACE_WARNING,__VA_ARGS__)

#define sof_log_error(provider,...)			sof_log((provider),SOF_TRACE_ERROR,__VA_ARGS__)

#define sof_log_fatal(provider,...)			sof_log((provider),SOF_TRACE_FATAL,__VA_ARGS__)


//////////////////////////////////////////////////////////////////////////

SOF_API sof_state sof_new(size_t maxcoros,size_t maxchannels);

SOF_API void sof_close(sof_state self);

SOF_API void sof_raise_errno(sof_state self, const char* msg ,const sof_errno_info* info);

SOF_API void sof_raise_trace(sof_state self, const char * file, int lines);

SOF_API void sof_reset_errno(sof_state self);

SOF_API const sof_errno_info* sof_last_errno(sof_state self);

SOF_API sof_trace_t sof_new_trace(sof_state self,sof_trace_f f, void * userdata, int levels);

SOF_API void sof_trace_levels(sof_state self,sof_trace_t trace,int levels);

SOF_API void sof_close_trace(sof_state self,sof_trace_t trace);

SOF_API bool sof_trace_status(sof_state self, int level);

SOF_API void __sof_trace(sof_state self, int level,const char * fmt, ...);

SOF_API void sof_new_extension(sof_state self, const sof_extension_vtable * vtable, void * userdata);

SOF_API sof_t sof_go(sof_state self, sof_f f, void * userdata);

SOF_API sof_event_t sof_wait(sof_state self,const sof_event_t * waitlist, size_t len);

SOF_API void sof_notify(sof_state sel,sof_t target, const sof_event_t * waitlist, size_t len);

//////////////////////////////////////////////////////////////////////////


#endif //SOF_ABI_H
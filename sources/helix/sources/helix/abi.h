/**
 * 
 * @file     abi
 * @brief    Copyright (C) 2013  yayanyang All Rights Reserved 
 * @author   yayanyang
 * @version  1.0.0.0  
 * @date     2013/08/07
 */
#ifndef HELIX_ABI_H
#define HELIX_ABI_H
#include <helix/configure.h>

#define HELIX_MAX_EVENTS									16
#define HELIX_STACK_SIZE									1024 * 56
#define HELIX_INVALID_HANDLE								uintptr_t(-1)
#define HELIX_TICKS_OF_MILLISECOND							100
//////////////////////////////////////////////////////////////////////////

#define HELIX_HANDLE_STRUCT_NAME(name) name##__

#define HELIX_DECLARE_HANDLE(name) typedef struct HELIX_HANDLE_STRUCT_NAME(name) *name;

#define HELIX_IMPLEMENT_HANDLE(name) struct HELIX_HANDLE_STRUCT_NAME(name)

#define HELIX_HANDLE_SIZEOF(name) sizeof(struct HELIX_HANDLE_STRUCT_NAME(name))

//////////////////////////////////////////////////////////////////////////
#define helix_failed(ei)									((ei).error.catalog != NULL)	

#define helix_success(ei)									(!helix_failed(ei))

#define helix_reset_errorcode(ei)							(ei).error.catalog = NULL

#define helix_make_errorinfo(errorinfo,c,errorCode)\
	errorinfo.error.catalog = &c;\
	errorinfo.error.code = errorCode;\
	errorinfo.file = __FILE__;\
	errorinfo.lines = __LINE__;

#define helix_win32_errno(errorinfo,ec)						helix_make_errorinfo(errorinfo,HELIX_WIN32_ERROR_CATALOG,ec)

#define helix_com_errno(errorinfo,ec)						helix_make_errorinfo(errorinfo,HELIX_COM_ERROR_CATALOG,ec)

#define helix_posix_errno(errorinfo,ec)						helix_make_errorinfo(errorinfo,HELIX_POSIX_ERROR_CATALOG,ec)

#define helix_user_errno(errorinfo,ec)						{(errorinfo).error = ec;(errorinfo).file = __FILE__; (errorinfo).lines = __LINE__;}

#define helix_declare_errcode(name)							helix_errcode name = {{0,0},0,0}

#define helix_mutex_lock(mutex)								mutex->lock(mutex)

#define helix_mutex_unlock(mutex)							mutex->unlock(mutex)

#define helix_condition_wait(condition,mutex)				condition->wait(condition,mutex)

#define helix_condition_notify_one(condition)				condition->notify_one(condition)

#define helix_condition_notify_all(condition)				condition->notify_all(condition)

#define helix_alloc(alloc_t,nsize)							(alloc_t)->alloc((alloc_t),NULL,0,nsize);

#define helix_free(alloc_t,ptr)								(alloc_t)->alloc((alloc_t),ptr,0,0);

//////////////////////////////////////////////////////////////////////////

typedef struct helix_uuid_t {
    helix_uint32_t Data1;
    helix_uint16_t Data2;
    helix_uint16_t Data3;
    helix_byte_t Data4[8];
} helix_uuid_t;

typedef struct {
    const helix_uuid_t * catalog;

    uintptr_t code;

} helix_errno_t;

typedef struct helix_errcode {
    helix_errno_t error;

    const char* file;

    int lines;

} helix_errcode;

typedef struct {
    helix_uint16_t major;

    helix_uint16_t minor;

    helix_uint16_t build;

    helix_uint16_t reversion;

} helix_version_t;

typedef enum {
    helix_hours,
    helix_minutes,
    helix_seconds,
    helix_milliseconds,
    helix_microseconds,
    helix_nanoseconds
} helix_duration_t;

typedef struct {
    helix_duration_t duration_t; /*the duration period type*/
    long count; /*the period internal count*/
} helix_duration;

typedef struct {
    helix_duration time_since_epoch;
} helix_time_point;

typedef struct {
    uintptr_t id;
    void* data;
} helix_event;

typedef struct {
    int N;
    int D;
} helix_ratio;

typedef struct helix_alloc_t {
    void* (*alloc)(struct helix_alloc_t*, void * ptr, size_t size, size_t nsize);
} helix_alloc_t;


HELIX_API helix_uuid_t HELIX_WIN32_ERROR_CATALOG;

HELIX_API helix_uuid_t HELIX_COM_ERROR_CATALOG;

HELIX_API helix_uuid_t HELIX_POSIX_ERROR_CATALOG;

HELIX_API helix_uuid_t HELIX_UNITTEST_ERROR_CATALOG;

//////////////////////////////////////////////////////////////////////////

HELIX_DECLARE_HANDLE(helix_t);

//core apis
HELIX_API helix_t helix_open(helix_alloc_t * alloc, helix_errcode * errorCode);

HELIX_API void helix_exit(helix_t helix);
//only call in the same thread which create the helix
HELIX_API void helix_close(helix_t helix);

HELIX_API helix_errcode * helix_lasterror(helix_t helix);

HELIX_API uintptr_t helix_go(helix_t helix, void(*f)(helix_t, void*), void* userdata, size_t stacksize = HELIX_STACK_SIZE);

HELIX_API void helix_event_add(helix_t helix, helix_event *event);

HELIX_API helix_event* helix_event_remove(helix_t helix, uintptr_t eventid);

HELIX_API void helix_event_clear(helix_t helix);

HELIX_API helix_event* helix_event_poll(helix_t helix, helix_duration *duration);

HELIX_API void helix_notify(helix_t helix, uintptr_t target, uintptr_t eventid);

HELIX_API void helix_notify_one(helix_t helix, uintptr_t eventid);

HELIX_API void helix_notify_all(helix_t helix, uintptr_t eventid);

HELIX_API void helix_duration_cast(helix_duration * source, helix_duration* target);

HELIX_DECLARE_HANDLE(helix_sock_t);

//socket apis
HELIX_API helix_sock_t helix_sock(helix_t helix,int af,int type,int protocol);

HELIX_API void helix_close_sock(helix_t helix,helix_sock_t sock);

HELIX_API void helix_bind(helix_t helix,helix_sock_t sock,const struct sockaddr *addr,socklen_t addrlen);

HELIX_API void helix_listen(helix_t helix,helix_sock_t sock,int backlog);

HELIX_API uintptr_t helix_accept(helix_t helix,helix_sock_t sock,struct sockaddr *addr, socklen_t *addrlen);

HELIX_API void helix_connect(helix_t helix,helix_sock_t sock,struct sockaddr *addr, socklen_t *addrlen);

HELIX_API size_t helix_send(helix_t helix,helix_sock_t sock,const void* buf,size_t length,int flags);

HELIX_API size_t helix_sendto(helix_t helix,helix_sock_t sock,const void* buf,size_t length,int flags,const struct sockaddr *dest_addr, socklen_t addrlen);

HELIX_API size_t helix_recv(helix_t helix,helix_sock_t sock,void* buf,size_t length,int flags);

HELIX_API size_t helix_recvfrom(helix_t helix,helix_sock_t sock,void* buf,size_t length,int flags,struct sockaddr *src_addr, socklen_t *addrlen);

#endif //HELIX_ABI_H
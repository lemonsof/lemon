/**
* 
* @file     thread
* @brief    Copyright (C) 2014  yayanyang@gmail.com All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2014/02/05
*/
#ifndef HELIX_THREAD_H
#define HELIX_THREAD_H
#include <helix/abi.h>
#include <helix/context/fcontext.h>

HELIX_DECLARE_HANDLE(helix_runq_t);

HELIX_DECLARE_HANDLE(helix_kernel_t);

HELIX_IMPLEMENT_HANDLE(helix_runq_t){
	
	volatile helix_status_t				helix_status;

	helix_kernel_t						helix_kenerl;

	helix_thread						*dispatch_thread;

	helix_mutex							*actor_list_mutex;

	helix_condition						*actor_list_condition_variable;

	helix_t								actor_list_header;

	helix_t								actor_list_tail;

	helix_context_t						context;
};

HELIX_PRIVATE void init_main_runq(helix_kernel_t helix_kenerl, helix_runq_t current);

HELIX_PRIVATE void init_runq(helix_kernel_t helix_kenerl, helix_runq_t current);

HELIX_PRIVATE void close_thread_and_join(helix_runq_t current);

HELIX_PRIVATE void helix_dispatch(helix_runq_t current);

HELIX_PRIVATE void helix_main_dispatch(helix_runq_t current);

HELIX_PRIVATE helix_t helix_actor_pop(helix_runq_t current);

HELIX_PRIVATE void helix_actor_push(helix_runq_t current, helix_t actor);

HELIX_PRIVATE void helix_actor_push_p(helix_runq_t current, helix_t actor);

#endif	//HELIX_THREAD_H
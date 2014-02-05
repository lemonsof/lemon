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
#include <helix/kenerl.h>
#include <helix/context/fcontext.h>

HELIX_DECLARE_HANDLE(helix_actor_t);

HELIX_DECLARE_HANDLE(helix_event_node_t);

HELIX_IMPLEMENT_HANDLE(helix_event_node_t){
	helix_event_node_t					next;
	helix_event							event;
	helix_mutex							mutex;
};

HELIX_IMPLEMENT_HANDLE(helix_actor_t){

	helix_actor_t						next;	

	helix_event_node_t					event_list;
};

HELIX_IMPLEMENT_HANDLE(helix_t){
	
	helix_status_t						helix_status;

	helix_kernel_t						helix_kenerl;

	helix_thread						*dispatch_thread;

	helix_mutex							*actor_list_mutex;

	helix_condition						*actor_list_condition_variable;

	helix_context_t						context_t;

	helix_actor_t						actor_list_header;

	helix_actor_t						actor_list_tail;
};

HELIX_PRIVATE void init_main_thread(helix_kernel_t helix_kenerl, helix_t current, helix_error_code * errorCode);

HELIX_PRIVATE void init_thread(helix_kernel_t helix_kenerl, helix_t current, helix_error_code * errorCode);

HELIX_PRIVATE void close_thread_and_join(helix_t current);

HELIX_PRIVATE void helix_thread_run(helix_t current);

HELIX_PRIVATE helix_actor_t helix_actor_pop(helix_t current);

#endif	//HELIX_THREAD_H
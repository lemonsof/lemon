/**
* 
* @file     helix
* @brief    Copyright (C) 2014  yayanyang@gmail.com All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2014/02/05
*/
#ifndef HELIX_HELIX_H
#define HELIX_HELIX_H

#include <helix/abi.h>
#include <helix/runq.h>
#include <helix/actor.h>

HELIX_DECLARE_HANDLE(helix_kernel_t);

HELIX_IMPLEMENT_HANDLE(helix_kernel_t){

	HELIX_HANDLE_STRUCT_NAME(helix_runq_t)		*thread_list;

	size_t										thread_list_counter;

	HELIX_HANDLE_STRUCT_NAME(helix_runq_t)		main_runq;

	HELIX_HANDLE_STRUCT_NAME(helix_t)			main_actor;

	helix_alloc_t								*alloc;
};

HELIX_PRIVATE void helix_actor_sleep(helix_kernel_t kernel,helix_t actor);


#endif	//HELIX_HELIX_H
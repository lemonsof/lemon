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

HELIX_DECLARE_HANDLE(helix_kernel_t);

HELIX_IMPLEMENT_HANDLE(helix_kernel_t){
	HELIX_HANDLE_STRUCT_NAME(helix_t)		*thread_list;
	
	size_t									thread_list_counter;

	HELIX_HANDLE_STRUCT_NAME(helix_t)		main_thread;

	HELIX_HANDLE_STRUCT_NAME(helix_actor_t) main_actor;
};


HELIX_PRIVATE int hardware_concurrency();




#endif	//HELIX_HELIX_H
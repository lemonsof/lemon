/**
* 
* @file     actor
* @brief    Copyright (C) 2014  yayanyang@gmail.com All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2014/02/07
*/
#ifndef HELIX_ACTOR_H
#define HELIX_ACTOR_H

#include <helix/abi.h>
#include <helix/context/fcontext.h>

HELIX_DECLARE_HANDLE(helix_runq_t);

HELIX_IMPLEMENT_HANDLE(helix_t){

	helix_t								next;

	uintptr_t							id;

	helix_runq_t						runq;

	helix_event							event;

	union{
		helix_context_t					obj;
		helix_context_t*				ptr;
	}									context;
};

#endif	//HELIX_ACTOR_H
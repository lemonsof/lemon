/**
* 
* @file     fcontext_x86_64_win
* @brief    Copyright (C) 2013  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2013/05/19
*/
#ifndef HELIX_COROUTINE_FCONTEXT_X86_64_WIN_H
#define HELIX_COROUTINE_FCONTEXT_X86_64_WIN_H
#include <helix/configure.h>

struct stack_t
{
	void    *   sp;
	size_t		size;
	void    *   limit;
};

struct helix_context_t
{
	helix_uint64_t      fc_greg[10];
	stack_t             fc_stack;
	void            *   fc_local_storage;
	helix_uint64_t      fc_fp[24];
};

#endif //HELIX_COROUTINE_FCONTEXT_X86_64_WIN_H

/**
* 
* @file     fcontext_x86_64_win
* @brief    Copyright (C) 2013  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2013/05/19
*/
#ifndef LEMON_COROUTINE_FCONTEXT_X86_64_WIN_H
#define LEMON_COROUTINE_FCONTEXT_X86_64_WIN_H
#include <sof/configure.h>

struct stack_t
{
	void    *   sp;
	size_t		size;
	void    *   limit;
};

struct sof_context_t
{
	sof_uint64_t      fc_greg[10];
	stack_t             fc_stack;
	void            *   fc_local_storage;
	sof_uint64_t      fc_fp[24];
};

#endif //LEMON_COROUTINE_FCONTEXT_X86_64_WIN_H

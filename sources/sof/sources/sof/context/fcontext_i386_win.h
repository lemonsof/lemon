/**
* 
* @file     fcontext_i386_win
* @brief    Copyright (C) 2013  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2013/05/19
*/
#ifndef LEMON_COROUTINE_FCONTEXT_I386_WIN_H
#define LEMON_COROUTINE_FCONTEXT_I386_WIN_H

#include <sof/configure.h>

struct stack_t
{
	void    *   sp;
	size_t		size;
	void    *   limit;
};

struct fp_t
{
	sof_uint32_t     fc_freg[2];
};

struct sof_context_t
{
	sof_uint32_t      fc_greg[6];
	stack_t             fc_stack;
	void            *   fc_excpt_lst;
	void            *   fc_local_storage;
	fp_t                fc_fp;
	sof_uint32_t      fc_dealloc;
};

#endif //LEMON_COROUTINE_FCONTEXT_I386_WIN_H

/**
* 
* @file     fcontext_i386_win
* @brief    Copyright (C) 2013  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2013/05/19
*/
#ifndef HELIX_COROUTINE_FCONTEXT_I386_WIN_H
#define HELIX_COROUTINE_FCONTEXT_I386_WIN_H

#include <helix/configure.h>

struct stack_t
{
	void    *   sp;
	size_t		size;
	void    *   limit;
};

struct fp_t
{
	helix_uint32_t     fc_freg[2];
};

struct helix_context_t
{
	helix_uint32_t      fc_greg[6];
	stack_t             fc_stack;
	void            *   fc_excpt_lst;
	void            *   fc_local_storage;
	fp_t                fc_fp;
	helix_uint32_t      fc_dealloc;
};

#endif //HELIX_COROUTINE_FCONTEXT_I386_WIN_H

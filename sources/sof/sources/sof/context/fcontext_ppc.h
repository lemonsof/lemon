/**
* 
* @file     fcontext_ppc
* @brief    Copyright (C) 2013  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2013/05/20
*/
#ifndef LEMON_COROUTINE_FCONTEXT_PPC_H
#define LEMON_COROUTINE_FCONTEXT_PPC_H
#include <sof/configure.h>

struct stack_t
{
	void    *   sp;
	size_t size;

	
};

struct fp_t
{
	sof_uint64_t     fc_freg[19];
};

struct sof_context_t
{
# if defined(__powerpc64__)
	sof_uint32_t		fc_greg[23];
# else
	sof_uint32_t		fc_greg[23];
# endif
	stack_t             fc_stack;
	fp_t                fc_fp;
};

#endif  //LEMON_COROUTINE_FCONTEXT_PPC_H

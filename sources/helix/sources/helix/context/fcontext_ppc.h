/**
* 
* @file     fcontext_ppc
* @brief    Copyright (C) 2013  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2013/05/20
*/
#ifndef HELIX_COROUTINE_FCONTEXT_PPC_H
#define HELIX_COROUTINE_FCONTEXT_PPC_H
#include <helix/configure.h>

struct stack_t
{
	void    *   sp;
	size_t size;

	
};

struct fp_t
{
	helix_uint64_t     fc_freg[19];
};

struct helix_context_t
{
# if defined(__powerpc64__)
	helix_uint32_t		fc_greg[23];
# else
	helix_uint32_t		fc_greg[23];
# endif
	stack_t             fc_stack;
	fp_t                fc_fp;
};

#endif  //HELIX_COROUTINE_FCONTEXT_PPC_H

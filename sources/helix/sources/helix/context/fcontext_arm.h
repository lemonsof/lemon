/**
* 
* @file     fcontext_arm
* @brief    Copyright (C) 2013  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2013/05/20
*/
#ifndef HELIX_COROUTINE_FCONTEXT_ARM_H
#define HELIX_COROUTINE_FCONTEXT_ARM_H
#include <helix/configure.h>

struct stack_t
{
	void    *   sp;
	size_t		size;
};

struct fp_t
{
	helix_uint32_t		fc_freg[16];
};

struct helix_context_t
{
	helix_uint32_t		fc_greg[11];
	stack_t             fc_stack;
	fp_t                fc_fp;
};
#endif  //HELIX_COROUTINE_FCONTEXT_ARM_H

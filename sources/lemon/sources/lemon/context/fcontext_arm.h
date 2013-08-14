/**
* 
* @file     fcontext_arm
* @brief    Copyright (C) 2013  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2013/05/20
*/
#ifndef LEMON_COROUTINE_FCONTEXT_ARM_H
#define LEMON_COROUTINE_FCONTEXT_ARM_H
#include <lemon/configure.h>

struct stack_t
{
	void    *   sp;
	size_t		size;
};

struct fp_t
{
	lemon_uint32_t		fc_freg[16];
};

struct lemon_context_t
{
	lemon_uint32_t		fc_greg[11];
	stack_t             fc_stack;
	fp_t                fc_fp;
};
#endif  //LEMON_COROUTINE_FCONTEXT_ARM_H

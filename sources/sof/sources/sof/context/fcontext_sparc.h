/**
* 
* @file     fcontext_sparc
* @brief    Copyright (C) 2013  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2013/05/20
*/
#ifndef LEMON_COROUTINE_FCONTEXT_SPARC_H
#define LEMON_COROUTINE_FCONTEXT_SPARC_H
#include <sof/configure.h>

struct stack_t
{
	void		*sp;
	size_t		size;
};

struct fp_t
{
#ifdef _LP64
	sof_uint64_t		fp_freg[32];
	sof_uint64_t		fp_fprs, fp_fsr;
#else
	sof_uint64_t		fp_freg[16];
	sof_uint32_t		fp_fsr;
#endif
}
#ifdef _LP64
__attribute__((__aligned__(64)))	// allow VIS instructions to be used
#endif
	;

struct sof_context_t
{
	fp_t                fc_fp;	// fpu stuff first, for easier alignement
#ifdef _LP64
	sof_uint64_t
#else
	sof_uint32_t
#endif
						fc_greg[8];

	stack_t             fc_stack;
};
#endif  //LEMON_COROUTINE_FCONTEXT_SPARC_H

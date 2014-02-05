/**
* 
* @file     fcontext_x86_x64_elf
* @brief    Copyright (C) 2013  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2013/05/20
*/
#ifndef HELIX_COROUTINE_FCONTEXT_X86_X64_ELF_H
#define HELIX_COROUTINE_FCONTEXT_X86_X64_ELF_H
#include <helix/configure.h>

struct stack_t
{
	void    *sp;
	size_t	size;
};

struct fp_t
{
	helix_uint32_t     fc_freg[2];
};

struct helix_context_t
{
	helix_uint64_t      fc_greg[8];
	stack_t             fc_stack;
	fp_t                fc_fp;
};
#endif  //HELIX_COROUTINE_FCONTEXT_X86_X64_ELF_H

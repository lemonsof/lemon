/**
* 
* @file     fcontext_i386_elf
* @brief    Copyright (C) 2013  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2013/05/20
*/
#ifndef HELIX_COROUTINE_FCONTEXT_I386_ELF_H
#define HELIX_COROUTINE_FCONTEXT_I386_ELF_H
#include <helix/configure.h>

struct stack_t
{
	void           *sp;
	size_t         size;

};

struct helix_context_t
{
	helix_uint32_t     fc_greg[6];
	stack_t            fc_stack;
	helix_uint32_t     fc_freg[2];
};
#endif  //HELIX_COROUTINE_FCONTEXT_I386_ELF_H

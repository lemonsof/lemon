/**
* 
* @file     stack
* @brief    Copyright (C) 2014  yayanyang@gmail.com All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2014/02/07
*/
#ifndef HELIX_STACK_H
#define HELIX_STACK_H
#include <helix/abi.h>

HELIX_PRIVATE void* alloc_stack(helix_alloc_t * alloc, size_t stacksize, size_t extsize);

HELIX_PRIVATE void free_stack(helix_alloc_t * alloc, void* stack, size_t stacksize);

#endif	//HELIX_STACK_H
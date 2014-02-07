/**
* 
* @file     utility
* @brief    Copyright (C) 2014  yayanyang@gmail.com All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2014/02/07
*/
#ifndef HELIX_UTILITY_H
#define HELIX_UTILITY_H
#include <helix/abi.h>

HELIX_PRIVATE int hardware_concurrency();

HELIX_PRIVATE helix_alloc_t* get_default_alloc();

#endif	//HELIX_UTILITY_H
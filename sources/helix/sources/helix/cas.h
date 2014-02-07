/**
* 
* @file     cas
* @brief    Copyright (C) 2014  yayanyang@gmail.com All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2014/02/07
*/
#ifndef HELIX_CAS_H
#define HELIX_CAS_H
#include <atomic>
#include <helix/abi.h>

#ifdef WIN32
typedef volatile LONG		helix_atomic_uint32;
typedef volatile LONGLONG	helix_atomic_uint64;



#else
# error "not implement"
#endif //WIN32

#endif	//HELIX_CAS_H
/**
* 
* @file     alloc
* @brief    Copyright (C) 2014  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2014/02/08
*/
#ifndef HELIX_ALLOC_HPP
#define HELIX_ALLOC_HPP
#include <helix/abi.h>
#include <helix++/nocopyable.hpp>

namespace helix{namespace impl{
	helix_alloc_t* default_alloc();
}}

#endif	//HELIX_ALLOC_HPP
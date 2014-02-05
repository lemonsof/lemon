/**
* 
* @file     fcontext
* @brief    Copyright (C) 2013  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2013/05/19
*/
#ifndef HELIX_COROUTINE_FCONTEXT_H
#define HELIX_COROUTINE_FCONTEXT_H
#ifdef WIN32
#       ifdef _WIN64
#               include <helix/context/fcontext_x86_64_win.h>
#       else
#               include <helix/context/fcontext_i386_win.h>
#       endif
#else
#       if defined(__x86_64__) || defined(__x86_64) || defined(__amd64__) || defined(__amd64) || defined(_M_X64) || defined(_M_AMD64)
#               include<helix/context/fcontext_x86_64_elf.h>
#		elif defined(i386) || defined(__i386__) || defined(__i386) || defined(__i486__) || defined(__i586__) || defined(__i686__) || defined(__X86__) || defined(_X86_) || defined(__THW_INTEL__) || defined(__I86__) || defined(__INTEL__) || defined(__IA32__) || defined(_M_IX86) || defined(_I86_)
#               include<helix/context/fcontext_i386_elf.h>
#       elif defined(__arm__) || defined(__thumb__) || defined(__TARGET_ARCH_ARM) || defined(__TARGET_ARCH_THUMB) || defined(_ARM) || defined(_M_ARM)
#               include<helix/context/fcontext_arm.h>
#       elif  (defined(__mips) && __mips == 1) || defined(_MIPS_ISA_MIPS1) || defined(_R3000)
#               include<helix/context/fcontext_mips.h>
#       elif defined(__powerpc) || defined(__powerpc__) || defined(__ppc) || defined(__ppc__) || defined(_ARCH_PPC) || defined(__POWERPC__) || defined(__PPCGECKO__) || defined(__PPCBROADWAY) || defined(_XENON)
#               include<helix/context/fcontext_ppc.h>
#       elif defined(__sparc__) || defined(__sparc)
#               include<helix/context/fcontext_sparc.h>
#       else
#               error "platform not support"
#       endif
#endif



#include <helix/configure.h>

HELIX_API intptr_t helix_context_jump( helix_context_t * ofc, helix_context_t const* nfc, intptr_t vp, bool preserve_fpu = true);

HELIX_API helix_context_t* helix_make_context( void * sp, size_t size, void (* fn)( intptr_t) );

#endif //HELIX_COROUTINE_FCONTEXT_H

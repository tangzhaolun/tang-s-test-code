/**********************************************************************************************************************
* FILENAME: system.hpp
* AUTHOR:   Tom Malnar
* CREATED:  2006/06/22                                                  
*                                                                             
* Copyright (c) 1995-2006 Christie Digital Canada Inc. All rights reserved.   
*                                                                             
* DESCRIPTION: This file contains linux system helper functions to be used across all projector processes/threads.
**********************************************************************************************************************/

#if !defined( _SYSTEM_HPP_ )
#define _SYSTEM_HPP_

#include "cdsTypes.h"

/**************************************************************************
 * \author Mike Loder
 * \date   2006/12/07
 * 
 * Detect endian of the architecture
 ***************************************************************************/

#if defined(BYTE_ORDER)
#   if BYTE_ORDER == LITTLE_ENDIAN
#       define C_LITTLE_ENDIAN  
#   else
#       define C_BIG_ENDIAN 
#   endif
#else
#   if !defined(LITTLE_ENDIAN) && !defined(BIG_ENDIAN)
#       if defined(__i386__) || defined(__i686__) || defined(__x86_64__)
#           define C_LITTLE_ENDIAN
#       elif defined(_ARCH_PPC) || defined(__PPC__) || defined(__PPC)
#           define C_BIG_ENDIAN
#       elif defined(_M68K_) || defined(__M68K__) || defined(__M68K) || defined(__m68k) || defined(_m68k) || defined(__m68k__)
#           define C_BIG_ENDIAN
#       else
#           error "Unable to detect Endian type."
#       endif
#   elif defined(LITTLE_ENDIAN) && defined(BIG_ENDIAN)
#       error "Both LITTLE and BIG endian are defined."
#   endif
#endif

/**************************************************************************
 * \author Mike Loder
 * \date   2006/12/07
 * 
 * Detect the operating system
 *   OS_WIN32   - Win32 (Windows 95/98/ME and Windows NT/2000/XP)
 *   OS_WIN64   - Windows 64Bit
 *   OS_LINUX   - Linux
 **************************************************************************/

#if !defined(SAG_COM) && (defined(WIN64) || defined(_WIN64) || defined(__WIN64__))
#   define OS_WIN32
#   define OS_WIN64
#elif !defined(SAG_COM) && (defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__))
#   define OS_WIN32
#elif defined(__MWERKS__) && defined(__INTEL__)
#   define OS_WIN32
#elif defined(__linux__) || defined(__linux)
#   define OS_LINUX
#else
#   error "The application has not been ported to this operating system."
#endif


/**************************************************************************
 * \author Mike Loder
 * \date   2006/12/07
 * 
 * Detect the compiler
 *
 *    CC_MWERKS   - Metrowerks CodeWarrior
 *    CC_MSVC     - Microsoft Visual C/C++
 *    CC_MSVC_NET - Microsoft Visual C/C++ .NET
 *    CC_BOR      - Borland/Turbo C++
 *    CC_WAT      - Watcom C++
 *    CC_GNU      - GNU C++
 *    CC_INTEL    - Intel C++ for Linux, Intel C++ for Windows
 * 
 ***************************************************************************/

#if defined(__MWERKS__)
#   define CC_MWERKS
#elif defined(_MSC_VER)
#   define CC_MSVC
#   if _MSC_VER >= 1300
#       define CC_MSVC_NET
#   endif
#   if defined(__INTEL_COMPILER)
#       define CC_INTEL
#   endif
#elif defined(__BORLANDC__) || defined(__TURBOC__)
#   define CC_BOR
#elif defined(__WATCOMC__)
#   define CC_WAT
#elif defined(__GNUC__)
#   define CC_GNU
#   if defined(__INTEL_COMPILER)
#       define CC_INTEL
#   endif
#else
#   error "Unable to detect compiler type."
#endif


/**************************************************************************
 * \author Mike Loder
 * \date   2006/12/07
 * 
 * Dynamic Linked Library Import / Export Types
 **************************************************************************/

#ifdef OS_WIN
#   define DLL_EXPORT __declspec(dllexport)
#   define DLL_IMPORT __declspec(dllimport)
#else
#   define DLL_EXPORT 
#   define DLL_IMPORT 
#endif


/**************************************************************************
 * \author Mike Loder
 * \date   2006/12/07
 * 
 * Endian Swapping Macros
 **************************************************************************/

//Perform a 16 bit endian swap
#define EndianSwap_16(value)        \
        ((((value) << 8) & 0xFF00) |    \
        (((value) >> 8) & 0x00FF))

//Perform a 32 bit endian swap
#define EndianSwap_32(value)            \
        ((((value) << 24) & 0xFF000000) | \
         (((value) << 8) & 0x00FF0000)  | \
         (((value) >> 8) & 0x0000FF00)  | \
         (((value) >> 24) & 0x000000FF))


//Perform a 64 bit endian swap
#define EndianSwap_64(value)                         \
        ((((value) << 56) & 0xFF00000000000000ULL)  | \
         (((value) << 40) & 0x00FF000000000000ULL)  | \
         (((value) << 24) & 0x0000FF0000000000ULL)  | \
         (((value) << 8)  & 0x000000FF00000000ULL)  | \
         (((value) >> 8)  & 0x00000000FF000000ULL)  | \
         (((value) >> 24) & 0x0000000000FF0000ULL)  | \
         (((value) >> 40) & 0x000000000000FF00ULL)  | \
         (((value) >> 56) & 0x00000000000000FFULL))
         
//These macros are to improve code readability        
#ifdef C_LITTLE_ENDIAN
#   define cpu_to_le16(A)   (A) 
#   define cpu_to_le32(A)   (A) 
#   define cpu_to_le64(A)   (A) 
#   define le16_to_cpu(A)   (A) 
#   define le32_to_cpu(A)   (A) 
#   define le64_to_cpu(A)   (A) 
#   define cpu_to_be16(A)   EndianSwap_16(A)    
#   define cpu_to_be32(A)   EndianSwap_32(A)    
#   define cpu_to_be64(A)   EndianSwap_64(A)    
#   define be16_to_cpu(A)   EndianSwap_16(A)    
#   define be32_to_cpu(A)   EndianSwap_32(A)    
#   define be64_to_cpu(A)   EndianSwap_64(A)    
#else
#   define cpu_to_le16(A)   EndianSwap_16(A)    
#   define cpu_to_le32(A)   EndianSwap_32(A)    
#   define cpu_to_le64(A)   EndianSwap_64(A)    
#   define le16_to_cpu(A)   EndianSwap_16(A)    
#   define le32_to_cpu(A)   EndianSwap_32(A)    
#   define le64_to_cpu(A)   EndianSwap_64(A)    
#   define cpu_to_be16(A)   (A) 
#   define cpu_to_be32(A)   (A) 
#   define cpu_to_be64(A)   (A) 
#   define be16_to_cpu(A)   (A) 
#   define be32_to_cpu(A)   (A) 
#   define be64_to_cpu(A)   (A) 
#endif     
    
#endif      // #if !defined( _SYSTEM_HPP_ ) 


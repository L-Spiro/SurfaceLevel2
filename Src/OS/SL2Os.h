/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Includes all OS headers.
 */

#pragma once

#include "SL2Windows.h"

#ifndef SL2_FASTCALL
#define SL2_FASTCALL
#endif	// SL2_FASTCALL

#define SL2_ASM_BEGIN										__asm {
#define SL2_ASM_END											}
#ifdef _M_IX86
#define SL2_X86												1
#elif defined( _M_X64 )
#define SL2_X64												1
#endif	// #ifdef _M_IX86

#if defined( _MSC_VER )
// Microsoft Visual Studio Compiler
#define														SL2_ALIGN( N ) 						__declspec( align( N ) )
#elif defined( __GNUC__ ) || defined( __clang__ )
// GNU Compiler Collection (GCC) or Clang
#define														SL2_ALIGN( N ) 						__attribute__( (aligned( N )) )
#else
#error "Unsupported compiler"
#endif
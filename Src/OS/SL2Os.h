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

#if defined( __APPLE__ )
inline uint16_t _byteswap_ushort( uint16_t _ui16Val ) { return (_ui16Val >> 8) | (_ui16Val << 8); }
inline unsigned long _byteswap_ulong( unsigned long _ui32Val ) { return uint32_t( (uint32_t( _ui32Val ) >> 24) |
	((_ui32Val >> 8) & 0x0000FF00) |
	((_ui32Val << 8) & 0x00FF0000) |
	(_ui32Val << 24) ); }
inline uint64_t _byteswap_uint64( uint64_t _ui64Val ) { return (_ui64Val >> 56) |
	((_ui64Val >> 40) & 0x000000000000FF00ULL) |
	((_ui64Val >> 24) & 0x0000000000FF0000ULL) |
	((_ui64Val >> 8) & 0x00000000FF000000ULL) |
	((_ui64Val << 8) & 0x000000FF00000000ULL) |
	((_ui64Val << 24) & 0x0000FF0000000000ULL) |
	((_ui64Val << 40) & 0x00FF000000000000ULL) |
	(_ui64Val << 56); }
#endif	// #if defined( __APPLE__ )

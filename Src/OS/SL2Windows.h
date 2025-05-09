/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Windows macros and header.
 */

#pragma once

/*
// Just for reference.
#define _WIN32_WINNT_NT4						0x0400 // Windows NT 4.0  
#define _WIN32_WINNT_WIN2K						0x0500 // Windows 2000  
#define _WIN32_WINNT_WINXP						0x0501 // Windows XP  
#define _WIN32_WINNT_WS03						0x0502 // Windows Server 2003  
#define _WIN32_WINNT_WIN6						0x0600 // Windows Vista  
#define _WIN32_WINNT_VISTA						0x0600 // Windows Vista  
#define _WIN32_WINNT_WS08						0x0600 // Windows Server 2008  
#define _WIN32_WINNT_LONGHORN					0x0600 // Windows Vista  
#define _WIN32_WINNT_WIN7						0x0601 // Windows 7  
#define _WIN32_WINNT_WIN8						0x0602 // Windows 8  
#define _WIN32_WINNT_WINBLUE					0x0603 // Windows 8.1  
#define _WIN32_WINNT_WINTHRESHOLD				0x0A00 // Windows 10  
#define _WIN32_WINNT_WIN10						0x0A00 // Windows 10  
*/

#define _WIN32_IE								0x0601
#define WINVER									0x0601
#define _WIN32_WINNT							0x0601
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>


#if defined( WIN32 ) || defined( _WIN32 ) || defined( _WIN64 )
#define SL2_WINDOWS
#if defined( WIN32 ) || defined( _WIN32 )
#define SL2_WIN32
#endif	// #ifdef defined( WIN32 ) || defined( _WIN32 )
#ifdef _WIN64
#define SL2_WIN64
#endif	// #ifdef _WIN64
#endif	// #if defined( WIN32 ) || defined( _WIN32 ) || defined( _WIN64 )


#ifdef SL2_WINDOWS

#define SL2_FASTCALL							__fastcall
#define SL2_EXPECT( COND, VAL )			(COND)
#define SL2_PREFETCH_LINE( ADDR )		_mm_prefetch( reinterpret_cast<const char *>(ADDR), _MM_HINT_T0 )
#define SL2_PREFETCH_LINE_WRITE( ADDR )
#define SL2_LIKELY( x )					( x ) [[likely]]
#define SL2_UNLIKELY( x )				( x ) [[unlikely]]

#ifndef SL2_WIN64
// Mercilessly ripped from: https://stackoverflow.com/a/46924301
// Still need to find somewhere from which to mercilessly rip a _udiv128() implementation.  MERCILESSLY.
#include <cstdint>
#include <intrin.h>
#include <stdexcept>
//#include <winnt.h>
inline uint64_t SL2_FASTCALL _umul128( uint64_t _ui64Multiplier, uint64_t _ui64Multiplicand, 
    uint64_t *_pui64ProductHi ) {
#if 0
    return UnsignedMultiply128( _ui64Multiplier, _ui64Multiplicand, _pui64ProductHi );
#else
    // _ui64Multiplier   = ab = a * 2^32 + b
    // _ui64Multiplicand = cd = c * 2^32 + d
    // ab * cd = a * c * 2^64 + (a * d + b * c) * 2^32 + b * d
    uint64_t ui64A = _ui64Multiplier >> 32;
    uint64_t ui64B = static_cast<uint32_t>(_ui64Multiplier);
    uint64_t ui64C = _ui64Multiplicand >> 32;
    uint64_t ui64D = static_cast<uint32_t>(_ui64Multiplicand);

    uint64_t ui64Ad = __emulu( static_cast<unsigned int>(ui64A), static_cast<unsigned int>(ui64D) );
    uint64_t ui64Bd = __emulu( static_cast<unsigned int>(ui64B), static_cast<unsigned int>(ui64D) );

    uint64_t ui64Abdc = ui64Ad + __emulu( static_cast<unsigned int>(ui64B), static_cast<unsigned int>(ui64C) );
    uint64_t ui64AbdcCarry = (ui64Abdc < ui64Ad);

    // _ui64Multiplier * _ui64Multiplicand = _pui64ProductHi * 2^64 + ui64ProductLo
    uint64_t ui64ProductLo = ui64Bd + (ui64Abdc << 32);
    uint64_t ui64ProductLoCarry = (ui64ProductLo < ui64Bd);
    (*_pui64ProductHi) = __emulu( static_cast<unsigned int>(ui64A), static_cast<unsigned int>(ui64C) ) + (ui64Abdc >> 32) + (ui64AbdcCarry << 32) + ui64ProductLoCarry;

    return ui64ProductLo;
#endif  // 1
}

inline uint64_t SL2_FASTCALL _udiv128( uint64_t _ui64High, uint64_t _ui64Low, uint64_t _ui64Divisor, uint64_t * _pui64Remainder ) {
	if ( _ui64Divisor == 0 ) {
		throw std::overflow_error( "Division by zero is not allowed." );
	}

	if ( _ui64High >= _ui64Divisor ) {
		throw std::overflow_error( "The division would overflow the 64-bit quotient." );
	}

	if ( _ui64High == 0 ) {
		if ( _pui64Remainder ) { (*_pui64Remainder) = _ui64Low % _ui64Divisor; }
		return _ui64Low / _ui64Divisor;
	}

	uint64_t ui64Q = 0;
	uint64_t ui64R = _ui64High;

	for ( int I = 63; I >= 0; --I ) {
		ui64R = (ui64R << 1) | ((_ui64Low >> I) & 1);

		if ( ui64R >= _ui64Divisor ) {
			ui64R -= _ui64Divisor;
			ui64Q |= 1ULL << I;
		}
	}

	if ( _pui64Remainder ) { (*_pui64Remainder) = ui64R; }
	return ui64Q;
}


#endif  // #ifndef SL2_WIN64

extern "C" {

#if defined( SL2_WIN64 )
extern void sincos( double _dAngle, double * _pdSin, double * _pdCos );
extern void sincosf( float _fAngle, float * _pfSin, float * _pfCos );
#else

// 32 bit implementation in inline assembly
inline void sincos( double _dAngle, double * _pdSin, double * _pdCos ) {
	double dSin, dCos;
	__asm {
		fld QWORD PTR[_dAngle]
		fsincos
		fstp QWORD PTR[dCos]
		fstp QWORD PTR[dSin]
		fwait
	}
	(*_pdSin) = dSin;
	(*_pdCos) = dCos;
}

inline void sincosf( float _fAngle, float * _pfSin, float * _pfCos ) {
    float fSinT, fCosT;
    __asm {
        fld DWORD PTR[_fAngle]		// Load the 32-bit float into the FPU stack.
        fsincos						// Compute cosine and sine.
        fstp DWORD PTR[fCosT]		// Store the cosine value.
        fstp DWORD PTR[fSinT]		// Store the sine value.
        fwait						// Wait for the FPU to finish.
    }
    (*_pfSin) = fSinT;
    (*_pfCos) = fCosT;
}

#endif	// #if defined( SL2_WIN64 )

}	// extern "C"

#endif	// #ifdef SL2_WINDOWS


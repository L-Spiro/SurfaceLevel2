﻿/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Let's convert and modify textures!
 */

#ifndef SL2_LIB

#include "SL2SurfaceLevel2.h"
#include "Files/SL2StdFile.h"
#include "Image/detex/misc.h"
#include "Image/DDS/SL2Dds.h"
#include "Image/SL2Image.h"
#include "Image/SL2KtxTexture.h"
#include "Time/SL2Clock.h"
#include "Utilities/SL2Stream.h"

#include <filesystem>
#include <format>
#include <iostream>

void FreeImageErrorHandler(FREE_IMAGE_FORMAT fif, const char* message) {
	std::cerr << "FreeImage error: " << message << std::endl;
}


int wmain( int _iArgC, wchar_t const * _wcpArgV[] ) {
    --_iArgC;
    std::u16string sThisDir = sl2::CFileBase::GetFilePath( reinterpret_cast<const char16_t *>((*_wcpArgV++)) );
    ::FreeImage_Initialise();									// Version 3.16.0.
    ::FreeImage_SetOutputMessage( ::FreeImageErrorHandler );
    sl2::CFormat::Init();
    sl2::SL2_OPTIONS oOptions;

    /*double dVal = 0.0;
    while ( dVal <= 1.0 ) {
        double dThis = sl2::CUtilities::LinearToACEScc( dVal );
        double dThat = sl2::CUtilities::ACESccToLinear( dThis );
        dVal += 1.0 / 7.0;
    }*/

#define SL2_ERRORT( TXT, CODE )					sl2::PrintError( reinterpret_cast<const char16_t *>(TXT), (CODE) );						\
												if ( oOptions.bPause ) { ::system( "pause" ); }	                                        \
												::FreeImage_DeInitialise();                                                             \
												::detexFreeErrorMessage();                                                              \
												return int( CODE )
#define SL2_ERROR( CODE )						SL2_ERRORT( nullptr, (CODE) )

#define SL2_CHECK( TOTAL, NAME )				_iArgC >= (TOTAL) && ::_wcsicmp( &(*_wcpArgV)[1], L ## #NAME ) == 0
#define SL2_ADV( VAL )							_iArgC -= (VAL); _wcpArgV += (VAL); continue
    while ( _iArgC ) {
        if ( (*_wcpArgV)[0] == L'-' ) {
			if ( SL2_CHECK( 2, file ) ) {
				try {
					sl2::SL2_OPEN_FILE ofFile = { .u16Path = reinterpret_cast<const char16_t *>((_wcpArgV[1])) };
					oOptions.vInputs.push_back( ofFile );
				}
				catch ( ... ) { SL2_ERROR( sl2::SL2_E_OUTOFMEMORY ); }
				SL2_ADV( 2 );
			}
			if ( SL2_CHECK( 2, outfile ) || SL2_CHECK( 2, out_file ) ) {
				// Make sure the output list has at least 1 fewer entries than the input list.
				if ( oOptions.vOutputs.size() >= oOptions.vInputs.size() ) {
					// Too many outputs have already been submitted.
					SL2_ERRORT( u"Too many outputs for the given number of inputs.\r\n", sl2::SL2_E_INVALIDCALL );
				}
				try {
					std::u16string sThis = reinterpret_cast<const char16_t *>((_wcpArgV[1]));
					// Get the provided extension.
					std::u16string sExt = sl2::CFileBase::GetFileExtension( sThis );
					std::u16string sPath = sl2::CFileBase::GetFilePath( sThis );
					std::u16string sName = sl2::CFileBase::GetFileName( sThis );
					if ( sName.size() == 1 && sName.c_str()[0] == u'*' || sName.c_str()[0] == u'.' ) { sName.clear(); }
					if ( !sPath.size() ) { sPath = sThisDir; }
					for ( size_t J = oOptions.vOutputs.size(); oOptions.vOutputs.size() < oOptions.vInputs.size() - 1; ++J ) {
						std::u16string sSrc = sPath;
						sSrc += sl2::CFileBase::NoExtension( sl2::CFileBase::GetFileName( oOptions.vInputs[J].u16Path ) );
						sSrc += u".";
						// If the string is empty, keep the extension of the inputs.
						if ( !sExt.size() ) {
							sSrc += sl2::CFileBase::GetFileExtension( oOptions.vInputs[J].u16Path );
						}
						else {
							sSrc += sExt;
						}

						oOptions.vOutputs.push_back( sSrc );
					}
                    
					std::u16string sSrc = sPath;
					if ( !sName.size() ) {
						sSrc += sl2::CFileBase::NoExtension( sl2::CFileBase::GetFileName( oOptions.vInputs[oOptions.vOutputs.size()].u16Path ) );
					}
					else {
						sSrc += sl2::CFileBase::NoExtension( sl2::CFileBase::GetFileName( sThis ) );
					}
					sSrc += u".";
					// If the string is empty, keep the extension of the inputs.
					if ( !sExt.size() ) {
						sSrc += sl2::CFileBase::GetFileExtension( oOptions.vInputs[oOptions.vOutputs.size()].u16Path );
					}
					else {
						sSrc += sExt;
					}
					oOptions.vOutputs.push_back( sSrc );
				}
				catch ( ... ) { SL2_ERROR( sl2::SL2_E_OUTOFMEMORY ); }
				SL2_ADV( 2 );
			}
			if ( SL2_CHECK( 4, yuv_file ) ) {
				try {
					sl2::SL2_OPEN_FILE ofFile = { .u16Path = reinterpret_cast<const char16_t *>((_wcpArgV[1])),
						.ui32YuvW = uint32_t( ::_wtoi( _wcpArgV[2] ) ),
						.ui32YuvH = uint32_t( ::_wtoi( _wcpArgV[3] ) ),
					};
					oOptions.vInputs.push_back( ofFile );
				}
				catch ( ... ) { SL2_ERROR( sl2::SL2_E_OUTOFMEMORY ); }
				SL2_ADV( 4 );
			}
			if ( SL2_CHECK( 1, from_clipboard ) || SL2_CHECK( 1, from_cb ) || SL2_CHECK( 1, clipboard_in ) || SL2_CHECK( 1, cb_in ) ) {
				try {
					sl2::SL2_OPEN_FILE ofFile = { .bFromClipBoard = true };
					oOptions.vInputs.push_back( ofFile );
				}
				catch ( ... ) { SL2_ERROR( sl2::SL2_E_OUTOFMEMORY ); }
				SL2_ADV( 1 );
			}
			if ( SL2_CHECK( 1, to_clipboard ) || SL2_CHECK( 1, to_cb ) || SL2_CHECK( 1, clipboard_out ) || SL2_CHECK( 1, cb_out ) ) {
				// Make sure the output list has at least 1 fewer entries than the input list.
				if ( oOptions.vOutputs.size() >= oOptions.vInputs.size() ) {
					// Too many outputs have already been submitted.
					SL2_ERRORT( u"Too many outputs for the given number of inputs.\r\n", sl2::SL2_E_INVALIDCALL );
				}
				try {
					for ( size_t J = oOptions.vOutputs.size(); oOptions.vOutputs.size() < oOptions.vInputs.size() - 1; ++J ) {
						oOptions.vOutputs.push_back( std::u16string() );
					}
					oOptions.vOutputs.push_back( std::u16string() );
				}
				catch ( ... ) { SL2_ERROR( sl2::SL2_E_OUTOFMEMORY ); }
				SL2_ADV( 1 );
			}
            

			if ( SL2_CHECK( 4, weight ) || SL2_CHECK( 4, weights ) ) {
				sl2::CFormat::SetLuma( ::_wtof( _wcpArgV[1] ), ::_wtof( _wcpArgV[2] ), ::_wtof( _wcpArgV[3] ) );
				SL2_ADV( 4 );
			}
			if ( SL2_CHECK( 2, luma ) ) {
				if ( ::_wcsicmp( _wcpArgV[1], L"REC_709" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"REC709" ) == 0 ) {
					sl2::CFormat::SetLuma( sl2::SL2_LS_REC_709 );
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"REC_2020" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"REC2020" ) == 0 ) {
					sl2::CFormat::SetLuma( sl2::SL2_LS_REC_2020 );
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"SMPTC" ) == 0 ) {
					sl2::CFormat::SetLuma( sl2::SL2_LS_SMPTC );
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"REC_601" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"REC601" ) == 0 ) {
					sl2::CFormat::SetLuma( sl2::SL2_LS_REC_601 );
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"CIE_1931" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"CIE1931" ) == 0 ) {
					sl2::CFormat::SetLuma( sl2::SL2_LS_CIE_1931 );
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"NTSC_1953" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"NTSC1953" ) == 0 ) {
					sl2::CFormat::SetLuma( sl2::SL2_LS_NTSC_1953 );
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"EBU_TECH_3213" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"EBUTECH3213" ) == 0 ) {
					sl2::CFormat::SetLuma( sl2::SL2_LS_EBU_TECH_3213 );
				}
				else {
					SL2_ERRORT( std::format( L"Invalid \"luma\": \"{}\". Must be REC_709, REC_2020, SMPTC, REC_601, CIE_1931, NTSC_1953, or EBU_TECH_3213.",
						_wcpArgV[1] ).c_str(), sl2::SL2_E_INVALIDCALL );
				}
				SL2_ADV( 2 );
			}
			if ( SL2_CHECK( 2, gamma ) || SL2_CHECK( 2, g ) ) {
				oOptions.dGamma = ::_wtof( _wcpArgV[1] );
				oOptions.bManuallySetGamma = true;
				SL2_ADV( 2 );
			}
			if ( SL2_CHECK( 1, rgbe ) || SL2_CHECK( 1, linear ) ) {
				oOptions.dGamma = 0.0;
				oOptions.bManuallySetGamma = true;
				oOptions.dTargetGamma = 0.0;
				SL2_ADV( 1 );
			}
			if ( SL2_CHECK( 1, srgb ) ) {
				oOptions.dGamma = -2.2;
				oOptions.bManuallySetGamma = true;
				SL2_ADV( 1 );
			}
			if ( SL2_CHECK( 2, targetgamma ) ) {
				oOptions.dTargetGamma = ::_wtof( _wcpArgV[1] );
				oOptions.bManuallySetTargetGamma = true;
				SL2_ADV( 2 );
			}
			if ( SL2_CHECK( 1, target_srgb ) ) {
				oOptions.dTargetGamma = -2.2;
				oOptions.bManuallySetTargetGamma = true;
				SL2_ADV( 1 );
			}

 #define SL2_COLORSPACE( CMD, VAR, FILEVAR, EXTRA )                                                                                                                                                                                                                                                                                                                                  \
	if ( SL2_CHECK( 2, CMD ) ) {                                                                                                                                                                                                                                                                                                                                                    \
		if ( ::_wcsicmp( _wcpArgV[1], L"sRGB" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"sRGB_precise" ) == 0 ) {                                                                                                                                                                                                                                                                         \
			oOptions.VAR = sl2::SL2_CGC_sRGB_PRECISE;                                                                                                                                                                                                                                                                                                                               \
		}                                                                                                                                                                                                                                                                                                                                                                           \
		else if ( ::_wcsicmp( _wcpArgV[1], L"sRGB_std" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"sRGB_standard" ) == 0 ) {                                                                                                                                                                                                                                                               \
			oOptions.VAR = sl2::SL2_CGC_sRGB_STANDARD;                                                                                                                                                                                                                                                                                                                              \
		}                                                                                                                                                                                                                                                                                                                                                                           \
		else if ( ::_wcsicmp( _wcpArgV[1], L"smpte_170_std" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"smpte_170m_std" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"170m_std" ) == 0 ||                                                                                                                                                                                                           \
			::_wcsicmp( _wcpArgV[1], L"smpte_170_standard" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"smpte_170m_standard" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"170m_standard" ) == 0 ) {                                                                                                                                                                                                 \
			oOptions.VAR = sl2::SL2_CGC_SMPTE_170M_1999;                                                                                                                                                                                                                                                                                                                            \
		}                                                                                                                                                                                                                                                                                                                                                                           \
		else if ( ::_wcsicmp( _wcpArgV[1], L"smpte_170" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"smpte_170m" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"170m" ) == 0 ) {                                                                                                                                                                                                                      \
			oOptions.VAR = sl2::SL2_CGC_SMPTE_170M_1999_PRECISE;                                                                                                                                                                                                                                                                                                                    \
		}                                                                                                                                                                                                                                                                                                                                                                           \
		else if ( ::_wcsicmp( _wcpArgV[1], L"rec709_std" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"rec.709_std" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"bt709_std" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"bt.709_std" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"itu_bt709_std" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"itu_bt.709_std" ) == 0 ||                                                      \
			::_wcsicmp( _wcpArgV[1], L"rec709_standard" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"rec.709_standard" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"bt709_standard" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"bt.709_standard" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"itu_bt709_standard" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"itu_bt.709_standard" ) == 0 ) {                             \
			oOptions.VAR = sl2::SL2_CGC_ITU_BT_709;                                                                                                                                                                                                                                                                                                                                 \
		}                                                                                                                                                                                                                                                                                                                                                                           \
		else if ( ::_wcsicmp( _wcpArgV[1], L"rec709" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"rec.709" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"bt709" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"bt.709" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"itu_bt709" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"itu_bt.709" ) == 0 ) {                                                                             \
			oOptions.VAR = sl2::SL2_CGC_ITU_BT_709_PRECISE;                                                                                                                                                                                                                                                                                                                         \
		}                                                                                                                                                                                                                                                                                                                                                                           \
		else if ( ::_wcsicmp( _wcpArgV[1], L"adobe" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"adobergb" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"adobe_rgb" ) == 0 ) {                                                                                                                                                                                                                       \
			oOptions.VAR = sl2::SL2_CGC_ADOBE_RGB;                                                                                                                                                                                                                                                                                                                                  \
		}                                                                                                                                                                                                                                                                                                                                                                           \
		else if ( ::_wcsicmp( _wcpArgV[1], L"bt2020_std" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"bt.2020_std" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"itu_bt2020_std" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"itu_bt.2020_std" ) == 0 ||                                                                                                                                                     \
			::_wcsicmp( _wcpArgV[1], L"bt2020_standard" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"bt.2020_standard" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"itu_bt2020_standard" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"itu_bt.2020_standard" ) == 0 ) {                                                                                                                                      \
			oOptions.VAR = sl2::SL2_CGC_ITU_BT_2020;                                                                                                                                                                                                                                                                                                                                \
		}                                                                                                                                                                                                                                                                                                                                                                           \
		else if ( ::_wcsicmp( _wcpArgV[1], L"bt2020" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"bt.2020" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"itu_bt2020" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"itu_bt.2020" ) == 0 ) {                                                                                                                                                                    \
			oOptions.VAR = sl2::SL2_CGC_ITU_BT_2020_PRECISE;                                                                                                                                                                                                                                                                                                                        \
		}                                                                                                                                                                                                                                                                                                                                                                           \
		else if ( ::_wcsicmp( _wcpArgV[1], L"dcip3" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"dci-p3" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"dci_p3" ) == 0 ) {                                                                                                                                                                                                                            \
			oOptions.VAR = sl2::SL2_CGC_DCI_P3;                                                                                                                                                                                                                                                                                                                                     \
		}                                                                                                                                                                                                                                                                                                                                                                           \
		else if ( ::_wcsicmp( _wcpArgV[1], L"smpte_240_std" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"smpte_240m_std" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"240m_std" ) == 0 ||                                                                                                                                                                                                           \
			::_wcsicmp( _wcpArgV[1], L"smpte_240_standard" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"smpte_240m_standard" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"240m_standard" ) == 0 ) {                                                                                                                                                                                                 \
			oOptions.VAR = sl2::SL2_CGC_SMPTE_240M_1999;                                                                                                                                                                                                                                                                                                                            \
		}                                                                                                                                                                                                                                                                                                                                                                           \
		else if ( ::_wcsicmp( _wcpArgV[1], L"smpte_240" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"smpte_240m" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"240m" ) == 0 ) {                                                                                                                                                                                                                      \
			oOptions.VAR = sl2::SL2_CGC_SMPTE_240M_1999_PRECISE;                                                                                                                                                                                                                                                                                                                    \
		}                                                                                                                                                                                                                                                                                                                                                                           \
		else if ( ::_wcsicmp( _wcpArgV[1], L"ntsc_1953_std" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"ntsc1953_std" ) == 0 ||                                                                                                                                                                                                                                                            \
			::_wcsicmp( _wcpArgV[1], L"ntsc_1953_standard" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"ntsc1953_standard" ) == 0 ) {                                                                                                                                                                                                                                                       \
			oOptions.VAR = sl2::SL2_CGC_NTSC_1953;                                                                                                                                                                                                                                                                                                                                  \
		}                                                                                                                                                                                                                                                                                                                                                                           \
		else if ( ::_wcsicmp( _wcpArgV[1], L"ntsc_1953" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"ntsc1953" ) == 0 ) {                                                                                                                                                                                                                                                                   \
			oOptions.VAR = sl2::SL2_CGC_NTSC_1953_PRECISE;                                                                                                                                                                                                                                                                                                                          \
		}                                                                                                                                                                                                                                                                                                                                                                           \
		else if ( ::_wcsicmp( _wcpArgV[1], L"tech_3213_std" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"tech3213_std" ) == 0 ||                                                                                                                                                                                                                                                            \
			::_wcsicmp( _wcpArgV[1], L"tech_3213_standard" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"tech3213_standard" ) == 0 ) {                                                                                                                                                                                                                                                       \
			oOptions.VAR = sl2::SL2_CGC_EBU_TECH_3213;                                                                                                                                                                                                                                                                                                                              \
		}                                                                                                                                                                                                                                                                                                                                                                           \
		else if ( ::_wcsicmp( _wcpArgV[1], L"tech_3213" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"tech3213" ) == 0 ) {                                                                                                                                                                                                                                                                   \
			oOptions.VAR = sl2::SL2_CGC_EBU_TECH_3213_PRECISE;                                                                                                                                                                                                                                                                                                                      \
		}                                                                                                                                                                                                                                                                                                                                                                           \
		else if ( ::_wcsicmp( _wcpArgV[1], L"displayp3_std" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"display-p3_std" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"display_p3_std" ) == 0 ||                                                                                                                                                                                                     \
			::_wcsicmp( _wcpArgV[1], L"displayp3_standard" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"display-p3_standard" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"display_p3_standard" ) == 0 ) {                                                                                                                                                                                           \
			oOptions.VAR = sl2::SL2_CGC_EBU_DISPLAY_P3;                                                                                                                                                                                                                                                                                                                             \
		}                                                                                                                                                                                                                                                                                                                                                                           \
		else if ( ::_wcsicmp( _wcpArgV[1], L"displayp3" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"display-p3" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"display_p3" ) == 0 ) {                                                                                                                                                                                                                \
			oOptions.VAR = sl2::SL2_CGC_EBU_DISPLAY_P3_PRECISE;                                                                                                                                                                                                                                                                                                                     \
		}                                                                                                                                                                                                                                                                                                                                                                           \
		else if ( ::_wcsicmp( _wcpArgV[1], L"rec601_std" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"rec.601_std" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"bt601_std" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"bt.601_std" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"itu_bt601_std" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"itu_bt.601_std" ) == 0 ||                                                      \
			::_wcsicmp( _wcpArgV[1], L"rec601_standard" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"rec.601_standard" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"bt601_standard" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"bt.601_standard" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"itu_bt601_standard" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"itu_bt.601_standard" ) == 0 ) {                             \
			oOptions.VAR = sl2::SL2_CGC_ITU_BT_601_525;                                                                                                                                                                                                                                                                                                                             \
		}                                                                                                                                                                                                                                                                                                                                                                           \
		else if ( ::_wcsicmp( _wcpArgV[1], L"rec601" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"rec.601" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"bt601" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"bt.601" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"itu_bt601" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"itu_bt.601" ) == 0 ) {                                                                             \
			oOptions.VAR = sl2::SL2_CGC_ITU_BT_601_525_PRECISE;                                                                                                                                                                                                                                                                                                                     \
		}                                                                                                                                                                                                                                                                                                                                                                           \
		else if ( ::_wcsicmp( _wcpArgV[1], L"rec601_pal_std" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"rec.601_pal_std" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"bt601_pal_std" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"bt.601_pal_std" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"itu_bt601_pal_std" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"itu_bt.601_pal_std" ) == 0 ||                              \
			::_wcsicmp( _wcpArgV[1], L"rec601_pal_standard" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"rec.601_pal_standard" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"bt601_pal_standard" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"bt.601_pal_standard" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"itu_bt601_pal_standard" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"itu_bt.601_pal_standard" ) == 0 ) {     \
			oOptions.VAR = sl2::SL2_CGC_ITU_BT_601_625;                                                                                                                                                                                                                                                                                                                             \
		}                                                                                                                                                                                                                                                                                                                                                                           \
		else if ( ::_wcsicmp( _wcpArgV[1], L"rec601_pal" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"rec.601_pal" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"bt601_pal" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"bt.601_pal" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"itu_bt601_pal" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"itu_bt.601_pal" ) == 0 ) {                                                     \
			oOptions.VAR = sl2::SL2_CGC_ITU_BT_601_625_PRECISE;                                                                                                                                                                                                                                                                                                                     \
		}                                                                                                                                                                                                                                                                                                                                                                           \
		else if ( ::_wcsicmp( _wcpArgV[1], L"generic_film" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"film" ) == 0 ) {                                                                                                                                                                                                                                                                    \
			oOptions.VAR = sl2::SL2_CGC_GENERIC_FILM;                                                                                                                                                                                                                                                                                                                               \
		}                                                                                                                                                                                                                                                                                                                                                                           \
		else if ( ::_wcsicmp( _wcpArgV[1], L"bt470_ntsc" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"bt470_m_ntsc" ) == 0 ) {                                                                                                                                                                                                                                                              \
			oOptions.VAR = sl2::SL2_CGC_ITU_BT_470_M_NTSC;                                                                                                                                                                                                                                                                                                                          \
		}                                                                                                                                                                                                                                                                                                                                                                           \
		else if ( ::_wcsicmp( _wcpArgV[1], L"bt470_pal" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"bt470_m_pal" ) == 0 ) {                                                                                                                                                                                                                                                                \
			oOptions.VAR = sl2::SL2_CGC_ITU_BT_470_M_PAL;                                                                                                                                                                                                                                                                                                                           \
		}                                                                                                                                                                                                                                                                                                                                                                           \
		else if ( ::_wcsicmp( _wcpArgV[1], L"bt470_b" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"bt470_b1" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"bt470_d" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"bt470_d1" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"bt470_g" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"bt470_h" ) == 0 ||                                                                             \
			::_wcsicmp( _wcpArgV[1], L"bt470_k" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"bt470_k1" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"bt470_l" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"bt470_n_pal" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"bt470_secam" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"bt470_l_secam" ) == 0 ) {                                                                     \
			oOptions.VAR = sl2::SL2_CGC_ITU_BT_470_B_N_PAL;                                                                                                                                                                                                                                                                                                                         \
		}                                                                                                                                                                                                                                                                                                                                                                           \
		/*else if ( ::_wcsicmp( _wcpArgV[1], L"acescg" ) == 0 ) {*/                                                                                                                                                                                                                                                                                                                     \
		/*    oOptions.VAR = sl2::SL2_CGC_ACESCG;*/                                                                                                                                                                                                                                                                                                                                     \
		/*}*/                                                                                                                                                                                                                                                                                                                                                                           \
		else if ( ::_wcsicmp( _wcpArgV[1], L"ntsc_1987" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"smpte_c" ) == 0 ) {                                                                                                                                                                                                                                                                    \
			oOptions.VAR = sl2::SL2_CGC_NTSC_1987;                                                                                                                                                                                                                                                                                                                                  \
		}                                                                                                                                                                                                                                                                                                                                                                           \
		else if ( ::_wcsicmp( _wcpArgV[1], L"ntsc_1987_std" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"smpte_c_std" ) == 0 ) {                                                                                                                                                                                                                                                            \
			oOptions.VAR = sl2::SL2_CGC_NTSC_1987_STANDARD;                                                                                                                                                                                                                                                                                                                         \
		}                                                                                                                                                                                                                                                                                                                                                                           \
		else if ( ::_wcsicmp( _wcpArgV[1], L"romm_rgb" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"rommrgb" ) == 0 ) {                                                                                                                                                                                                                                                                     \
			oOptions.VAR = sl2::SL2_CGC_ROMM_RGB;                                                                                                                                                                                                                                                                                                                                   \
		}                                                                                                                                                                                                                                                                                                                                                                           \
		else if ( ::_wcsicmp( _wcpArgV[1], L"rimm_rgb" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"rimmrgb" ) == 0 ) {                                                                                                                                                                                                                                                                     \
			oOptions.VAR = sl2::SL2_CGC_RIMM_RGB;                                                                                                                                                                                                                                                                                                                                   \
		}                                                                                                                                                                                                                                                                                                                                                                           \
		else if ( ::_wcsicmp( _wcpArgV[1], L"erimm_rgb" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"erimmrgb" ) == 0 ) {                                                                                                                                                                                                                                                                   \
			oOptions.VAR = sl2::SL2_CGC_ERIMM_RGB;                                                                                                                                                                                                                                                                                                                                  \
		}                                                                                                                                                                                                                                                                                                                                                                           \
		else if ( ::_wcsicmp( _wcpArgV[1], L"plasa" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"plasa_ansi" ) == 0 ) {                                                                                                                                                                                                                                                                     \
			oOptions.VAR = sl2::SL2_CGC_PLASA_ANSI_E154;                                                                                                                                                                                                                                                                                                                            \
		}                                                                                                                                                                                                                                                                                                                                                                           \
		else if ( ::_wcsicmp( _wcpArgV[1], L"protune" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"gopro" ) == 0 ) {                                                                                                                                                                                                                                                                        \
			oOptions.VAR = sl2::SL2_CGC_PROTUNE;                                                                                                                                                                                                                                                                                                                                    \
		}                                                                                                                                                                                                                                                                                                                                                                           \
		else if ( ::_wcsicmp( _wcpArgV[1], L"s-gamut" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"sgamut" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"s_gamut" ) == 0 ) {                                                                                                                                                                                                                         \
			oOptions.VAR = sl2::SL2_CGC_S_GAMUT;                                                                                                                                                                                                                                                                                                                                    \
		}                                                                                                                                                                                                                                                                                                                                                                           \
		else if ( ::_wcsicmp( _wcpArgV[1], L"s-gamut3" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"sgamut3" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"s_gamut3" ) == 0 ) {                                                                                                                                                                                                                      \
			oOptions.VAR = sl2::SL2_CGC_S_GAMUT3;                                                                                                                                                                                                                                                                                                                                   \
		}                                                                                                                                                                                                                                                                                                                                                                           \
		else if ( ::_wcsicmp( _wcpArgV[1], L"s-gamut3cine" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"sgamut3cine" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"s_gamut3cine" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"s-gamut3_cine" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"sgamut3_cine" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"s_gamut3_cine" ) == 0 ) {                                               \
			oOptions.VAR = sl2::SL2_CGC_S_GAMUT3_CINE;                                                                                                                                                                                                                                                                                                                              \
		}                                                                                                                                                                                                                                                                                                                                                                           \
		else {                                                                                                                                                                                                                                                                                                                                                                      \
			SL2_ERRORT( std::format( L"Invalid \"" #CMD "\": \"{}\". Must be sRGB, sRGB_std, smpte_170, smpte_170_std, rec601, rec601_std, rec601_pal, rec601_pal_std, rec709, rec709_std, adobergb, bt2020, bt2020_std, dci_p3, smpte_240, smpte_240_std,"                                                                                                                         \
				"ntsc_1953, ntsc_1953_std, tech_3213, tech_3213_std, generic_film, bt470_ntsc, bt470_pal, bt470_secam, ntsc_1987, ntsc_1987_std, smpte_c, smpte_c_std, romm_rgb, rimm_rgb, erimm_rgb, gopro, plasa, s-gamut, s-gamut3, s-gamut3_cine, displayp3, or displayp3_std.",                                                                                                \
				_wcpArgV[1] ).c_str(), sl2::SL2_E_INVALIDCALL );                                                                                                                                                                                                                                                                                                                    \
		}                                                                                                                                                                                                                                                                                                                                                                           \
		sl2::CIcc::SL2_CMS_PROFILE cpProfile;                                                                                                                                                                                                                                                                                                                                       \
		if ( !sl2::CIcc::CreateProfile( NULL, oOptions.VAR, cpProfile, true ) ) {                                                                                                                                                                                                                                                                                                   \
			SL2_ERRORT( std::format( L"\"" #CMD "\": \"{}\". Failed to create colorspace profile.",                                                                                                                                                                                                                                                                                 \
				_wcpArgV[1] ).c_str(), sl2::SL2_E_OUTOFMEMORY );                                                                                                                                                                                                                                                                                                                    \
		}                                                                                                                                                                                                                                                                                                                                                                           \
		if ( !sl2::CIcc::SaveProfileToMemory( cpProfile, oOptions.FILEVAR ) ) {                                                                                                                                                                                                                                                                                                     \
			SL2_ERRORT( std::format( L"\"" #CMD "\": \"{}\". Failed to save colorspace profile.",                                                                                                                                                                                                                                                                                   \
				_wcpArgV[1] ).c_str(), sl2::SL2_E_OUTOFMEMORY );                                                                                                                                                                                                                                                                                                                    \
		}                                                                                                                                                                                                                                                                                                                                                                           \
		EXTRA                                                                                                                                                                                                                                                                                                                                                                       \
		SL2_ADV( 2 );                                                                                                                                                                                                                                                                                                                                                               \
	}

			SL2_COLORSPACE( target_colorspace, cgcOutputGammaCurve, vOutColorProfile, if ( !oOptions.bManuallySetTargetGamma ) { oOptions.bManuallySetTargetGamma = true; oOptions.dTargetGamma = 0.0; } oOptions.bEmbedColorProfile = true; )
			SL2_COLORSPACE( input_colorspace, cgcInputGammaCurve, vInColorProfile, if ( !oOptions.bManuallySetGamma ) { oOptions.bManuallySetGamma = true; oOptions.dGamma = 0.0; } )

			if ( SL2_CHECK( 1, dont_embed_icc ) ) {
				oOptions.bEmbedColorProfile = false;
				SL2_ADV( 1 );
			}
			if ( SL2_CHECK( 1, embed_icc ) ) {
				oOptions.bEmbedColorProfile = true;
				SL2_ADV( 1 );
			}
			if ( SL2_CHECK( 1, ignore_input_colorspace_gamma ) ) {
				oOptions.bIgnoreSourceColorspaceGamma = true;
				SL2_ADV( 1 );
			}
			if ( SL2_CHECK( 2, target_colorspace_file ) ) {
				try {
					sl2::CStdFile sfFile;
					if ( !sfFile.Open( reinterpret_cast<const char16_t *>((_wcpArgV[1])) ) ) {
						SL2_ERRORT( std::format( L"\"target_colorspace_file\": \"{}\". File not found.",
							_wcpArgV[1] ).c_str(), sl2::SL2_E_FILENOTFOUND );
					}
					if ( !sfFile.LoadToMemory( oOptions.vOutColorProfile ) ) {
						SL2_ERRORT( std::format( L"\"target_colorspace_file\": \"{}\". Not enough memory to load the given file to memory.",
							_wcpArgV[1] ).c_str(), sl2::SL2_E_OUTOFMEMORY );
					}
				}
				catch ( ... ) { SL2_ERROR( sl2::SL2_E_OUTOFMEMORY ); }
				if ( !oOptions.bManuallySetTargetGamma ) {
					oOptions.bManuallySetTargetGamma = true;
					oOptions.dTargetGamma = 0.0;
				}
				oOptions.bEmbedColorProfile = true;
				SL2_ADV( 2 );
			}
			if ( SL2_CHECK( 2, input_colorspace_file ) ) {
				try {
					sl2::CStdFile sfFile;
					if ( !sfFile.Open( reinterpret_cast<const char16_t *>((_wcpArgV[1])) ) ) {
						SL2_ERRORT( std::format( L"\"input_colorspace_file\": \"{}\". File not found.",
							_wcpArgV[1] ).c_str(), sl2::SL2_E_FILENOTFOUND );
					}
					if ( !sfFile.LoadToMemory( oOptions.vInColorProfile ) ) {
						SL2_ERRORT( std::format( L"\"input_colorspace_file\": \"{}\". Not enough memory to load the given file to memory.",
							_wcpArgV[1] ).c_str(), sl2::SL2_E_OUTOFMEMORY );
					}
				}
				catch ( ... ) { SL2_ERROR( sl2::SL2_E_OUTOFMEMORY ); }
				if ( !oOptions.bManuallySetGamma ) {
					oOptions.bManuallySetGamma = true;
					oOptions.dGamma = 0.0;
				}
				SL2_ADV( 2 );
			}

			if ( SL2_CHECK( 1, photo ) ) {
				// -nomips
				oOptions.mhMipHandling = sl2::SL2_MH_REMOVE_EXISTING;

				// -target_colorspace sRGB
				oOptions.cgcOutputGammaCurve = sl2::SL2_CGC_sRGB_PRECISE;
				sl2::CIcc::SL2_CMS_PROFILE cpProfile;
				if ( !sl2::CIcc::CreateProfile( NULL, oOptions.cgcOutputGammaCurve, cpProfile, true ) ) {
					SL2_ERRORT( std::format( L"\"photo\": Failed to create sRGB colorspace profile." ).c_str(), sl2::SL2_E_OUTOFMEMORY );
				}
				if ( !sl2::CIcc::SaveProfileToMemory( cpProfile, oOptions.vOutColorProfile ) ) {
					SL2_ERRORT( std::format( L"\"photo\": Failed to save sRGB colorspace profile." ).c_str(), sl2::SL2_E_OUTOFMEMORY );
				}
				if ( !oOptions.bManuallySetTargetGamma ) {
					oOptions.bManuallySetTargetGamma = true;
					oOptions.dTargetGamma = 0.0;
				}
				oOptions.bEmbedColorProfile = true;
				oOptions.rResample.taColorW = oOptions.rResample.taColorH = oOptions.rResample.taColorD = sl2::SL2_TA_NULL_BORDER;
				oOptions.rResample.taAlphaW = oOptions.rResample.taAlphaH = oOptions.rResample.taAlphaD = sl2::SL2_TA_NULL_BORDER;

				oOptions.dGamma = -2.2;
				//oOptions.bManuallySetGamma = false;
				SL2_ADV( 1 );
			}

			if ( SL2_CHECK( 2, rendering_intent ) || SL2_CHECK( 2, render_intent ) ) {
				if ( ::_wcsicmp( _wcpArgV[1], L"perceptual" ) == 0 ) {
					oOptions.i32InRenderingIntent = INTENT_PERCEPTUAL;
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"relative_colorimetric" ) == 0 ) {
					oOptions.i32InRenderingIntent = INTENT_RELATIVE_COLORIMETRIC;
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"saturation" ) == 0 ) {
					oOptions.i32InRenderingIntent = INTENT_SATURATION;
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"absolute_colorimetric" ) == 0 ) {
					oOptions.i32InRenderingIntent = INTENT_ABSOLUTE_COLORIMETRIC;
				}
				else {
					SL2_ERRORT( std::format( L"Invalid \"rendering_intent\": \"{}\". Must be perceptual, relative_colorimetric, saturation, or absolute_colorimetric.",
						_wcpArgV[1] ).c_str(), sl2::SL2_E_INVALIDCALL );
				}
				oOptions.i32OutRenderingIntent = oOptions.i32InRenderingIntent;
				SL2_ADV( 2 );
			}

			if ( SL2_CHECK( 1, rot0 ) ) {
				oOptions.qrQuickRot = sl2::SL2_QR_ROT_0;
				SL2_ADV( 1 );
			}
			if ( SL2_CHECK( 1, rot90 ) ) {
				oOptions.qrQuickRot = sl2::SL2_QR_ROT_90;
				SL2_ADV( 1 );
			}
			if ( SL2_CHECK( 1, rot180 ) ) {
				oOptions.qrQuickRot = sl2::SL2_QR_ROT_180;
				SL2_ADV( 1 );
			}
			if ( SL2_CHECK( 1, rot270 ) ) {
				oOptions.qrQuickRot = sl2::SL2_QR_ROT_270;
				SL2_ADV( 1 );
			}

			if ( SL2_CHECK( 1, printformats ) || SL2_CHECK( 1, print_formats ) ) {
				sl2::CFormat::PrintFormats_List();
				SL2_ADV( 1 );
			}
            
			if ( SL2_CHECK( 2, format ) ) {
				std::string sString = sl2::CUtilities::Utf16ToUtf8( reinterpret_cast<const char16_t *>((_wcpArgV[1])) );
				oOptions.pkifdFinalFormat = sl2::CFormat::FindFormatDataByVulkan( sString.c_str() );
				if ( !oOptions.pkifdFinalFormat ) {
					oOptions.pkifdFinalFormat = sl2::CFormat::FindFormatDataByDx( sString.c_str() );
				}
				if ( !oOptions.pkifdFinalFormat ) {
					oOptions.pkifdFinalFormat = sl2::CFormat::FindFormatDataByMetal( sString.c_str() );
				}
				if ( !oOptions.pkifdFinalFormat ) {
					oOptions.pkifdFinalFormat = sl2::CFormat::FindFormatDataByOgl( sString.c_str() );
				}
				if ( !oOptions.pkifdFinalFormat ) {
					SL2_ERRORT( std::format( L"Invalid \"format\": \"{}\".",
						_wcpArgV[1] ).c_str(), sl2::SL2_E_INVALIDCALL );
				}
				SL2_ADV( 2 );
			}
			if ( SL2_CHECK( 4, ogl_format ) ) {
				std::string sStringIf = sl2::CUtilities::Utf16ToUtf8( reinterpret_cast<const char16_t *>((_wcpArgV[1])) );
				std::string sStringType = sl2::CUtilities::Utf16ToUtf8( reinterpret_cast<const char16_t *>((_wcpArgV[2])) );
				std::string sStringBif = sl2::CUtilities::Utf16ToUtf8( reinterpret_cast<const char16_t *>((_wcpArgV[3])) );

				oOptions.pkifdFinalFormat = sl2::CFormat::FindFormatDataByOgl( sStringIf.c_str(), sStringType.c_str(), sStringBif.c_str() );
				if ( !oOptions.pkifdFinalFormat ) {
					SL2_ERRORT( std::format( L"Invalid \"ogl_format\": \"{}\".",
						_wcpArgV[1] ).c_str(), sl2::SL2_E_INVALIDCALL );
				}
				SL2_ADV( 4 );
			}

			if ( SL2_CHECK( 1, quality_highest ) || SL2_CHECK( 1, very_slow ) ) {
				sl2::CFormat::SetPerfLevel( 0 );
				SL2_ADV( 1 );
			}
			if ( SL2_CHECK( 1, quality_production ) || SL2_CHECK( 1, slow ) ) {
				sl2::CFormat::SetPerfLevel( 1 );
				SL2_ADV( 1 );
			}
			if ( SL2_CHECK( 1, quality_normal ) || SL2_CHECK( 1, basic ) ) {
				sl2::CFormat::SetPerfLevel( 2 );
				SL2_ADV( 1 );
			}
			if ( SL2_CHECK( 1, fast ) ) {
				sl2::CFormat::SetPerfLevel( 3 );
				SL2_ADV( 1 );
			}
			if ( SL2_CHECK( 1, quick ) || SL2_CHECK( 1, veryfast ) ) {
				sl2::CFormat::SetPerfLevel( 4 );
				SL2_ADV( 1 );
			}
			if ( SL2_CHECK( 1, ultrafast ) ) {
				sl2::CFormat::SetPerfLevel( 5 );
				SL2_ADV( 1 );
			}

			if ( SL2_CHECK( 1, ignore_alpha ) ) {
				oOptions.bIgnoreAlpha = true;
				SL2_ADV( 1 );
			}
			if ( SL2_CHECK( 2, alpha_threshold ) ) {
				double dVal = ::_wtof( _wcpArgV[1] );
				if ( dVal < 0.0 || dVal > 255.0 ) {
					SL2_ERRORT( std::format( L"Invalid \"alpha_threshold\": \"{}\". Must be between 0 and 255.",
						_wcpArgV[1] ).c_str(), sl2::SL2_E_INVALIDCALL );
				}
				sl2::CFormat::SetAlphaCutoff( uint8_t( std::round( dVal ) ) );
				SL2_ADV( 2 );
			}
			if ( SL2_CHECK( 1, premultiply_alpha ) || SL2_CHECK( 1, premult_alpha ) ) {
				oOptions.bNeedsPreMultiply = true;
				SL2_ADV( 1 );
			}
			if ( SL2_CHECK( 2, swizzle ) ) {
				if ( !sl2::CFormat::CreateSwizzleFromString( _wcpArgV[1], oOptions.sSwizzle ) ) {
					SL2_ERRORT( std::format( L"Invalid \"swizzle\": \"{}\". Must be 4 characters in the RegEx format: /^[rgbaxyzw01]{{4}}$/.",
						_wcpArgV[1] ).c_str(), sl2::SL2_E_INVALIDCALL );
				}
				SL2_ADV( 2 );
			}
			if ( SL2_CHECK( 1, swap ) ) {
				oOptions.bSwap = true;
				SL2_ADV( 1 );
			}

			if ( SL2_CHECK( 1, flipx ) ) {
				oOptions.bFlipX = true;
				SL2_ADV( 1 );
			}
			if ( SL2_CHECK( 1, flipy ) ) {
				oOptions.bFlipY = true;
				SL2_ADV( 1 );
			}
			if ( SL2_CHECK( 1, flipz ) ) {
				oOptions.bFlipZ = true;
				SL2_ADV( 1 );
			}

			if ( SL2_CHECK( 5, crop ) ) {
				oOptions.wCropWindow.i32X = ::_wtoi( _wcpArgV[1] );
				oOptions.wCropWindow.i32Y = ::_wtoi( _wcpArgV[2] );
				oOptions.wCropWindow.ui32W = ::_wtoi( _wcpArgV[3] );
				oOptions.wCropWindow.ui32H = ::_wtoi( _wcpArgV[4] );
				SL2_ADV( 5 );
			}
			if ( SL2_CHECK( 7, crop3 ) ) {
				oOptions.wCropWindow.i32X = ::_wtoi( _wcpArgV[1] );
				oOptions.wCropWindow.i32Y = ::_wtoi( _wcpArgV[2] );
				oOptions.wCropWindow.i32Z = ::_wtoi( _wcpArgV[3] );
				oOptions.wCropWindow.ui32W = ::_wtoi( _wcpArgV[4] );
				oOptions.wCropWindow.ui32H = ::_wtoi( _wcpArgV[5] );
				oOptions.wCropWindow.ui32D = ::_wtoi( _wcpArgV[6] );
				SL2_ADV( 7 );
			}

			if ( SL2_CHECK( 3, prescale ) ) {
				oOptions.rResample.ui32NewW = ::_wtoi( _wcpArgV[1] );
				oOptions.rResample.ui32NewH = ::_wtoi( _wcpArgV[2] );
				oOptions.i32ScaleDims = 2;
				SL2_ADV( 3 );
			}
			if ( SL2_CHECK( 4, resample_size ) || SL2_CHECK( 4, prescale3 ) ) {
				oOptions.rResample.ui32NewW = ::_wtoi( _wcpArgV[1] );
				oOptions.rResample.ui32NewH = ::_wtoi( _wcpArgV[2] );
				oOptions.rResample.ui32NewD = ::_wtoi( _wcpArgV[3] );
				oOptions.i32ScaleDims = 3;
				SL2_ADV( 4 );
			}
			if ( SL2_CHECK( 2, rescale ) || SL2_CHECK( 2, rescale_to ) || SL2_CHECK( 2, resample_to ) ) {
				if ( ::_wcsicmp( _wcpArgV[1], L"nearest" ) == 0 ) {
					oOptions.rtResampleTo = sl2::SL2_RT_NEAREST;
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"hi" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"next_hi" ) == 0 ) {
					oOptions.rtResampleTo = sl2::SL2_RT_NEXT_HI;
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"lo" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"next_lo" ) == 0 ) {
					oOptions.rtResampleTo = sl2::SL2_RT_NEXT_LO;
				}
				else {
					SL2_ERRORT( std::format( L"Invalid \"rescale\": \"{}\". Must be nearest, [hi|next_hi], or [lo|next_lo].",
						_wcpArgV[1] ).c_str(), sl2::SL2_E_INVALIDCALL );
				}
				SL2_ADV( 2 );
			}
			if ( SL2_CHECK( 3, rel_scale ) ) {
				oOptions.dRelScaleW = ::_wtof( _wcpArgV[1] );
				oOptions.dRelScaleH = ::_wtof( _wcpArgV[2] );
				SL2_ADV( 3 );
			}
			if ( SL2_CHECK( 4, rel_scale3 ) ) {
				oOptions.dRelScaleW = ::_wtof( _wcpArgV[1] );
				oOptions.dRelScaleH = ::_wtof( _wcpArgV[2] );
				oOptions.dRelScaleD = ::_wtof( _wcpArgV[3] );
				SL2_ADV( 4 );
			}
			if ( SL2_CHECK( 3, fit ) ) {
				oOptions.ui32FitW = ::_wtoi( _wcpArgV[1] );
				oOptions.ui32FitH = ::_wtoi( _wcpArgV[2] );
				oOptions.rtResampleTo = sl2::SL2_RT_FIT;
				SL2_ADV( 3 );
			}
			if ( SL2_CHECK( 4, fit3 ) ) {
				oOptions.ui32FitW = ::_wtoi( _wcpArgV[1] );
				oOptions.ui32FitH = ::_wtoi( _wcpArgV[2] );
				oOptions.ui32FitD = ::_wtoi( _wcpArgV[3] );
				oOptions.rtResampleTo = sl2::SL2_RT_FIT;
				SL2_ADV( 4 );
			}

#define SL2_RESAMPLE( COND, ASSIGN, COPY )                                                                                                                                                                                          \
	if ( SL2_CHECK( 2, COND ) ) {                                                                                                                                                                                                   \
		if ( ::_wcsicmp( _wcpArgV[1], L"box" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"point" ) == 0 ) {                                                                                                                                 \
			oOptions.ASSIGN = sl2::CResampler::SL2_FF_POINT;                                                                                                                                                                        \
		}                                                                                                                                                                                                                           \
		else if ( ::_wcsicmp( _wcpArgV[1], L"tent" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"bilinear" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"linear" ) == 0 ) {                                                                           \
			oOptions.ASSIGN = sl2::CResampler::SL2_FF_LINEAR;                                                                                                                                                                       \
		}                                                                                                                                                                                                                           \
		else if ( ::_wcsicmp( _wcpArgV[1], L"quadraticsharp" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"quadratic_sharp" ) == 0 ) {                                                                                                       \
			oOptions.ASSIGN = sl2::CResampler::SL2_FF_QUADRATICSHARP;                                                                                                                                                               \
		}                                                                                                                                                                                                                           \
		else if ( ::_wcsicmp( _wcpArgV[1], L"quadratic" ) == 0 ) {                                                                                                                                                                  \
			oOptions.ASSIGN = sl2::CResampler::SL2_FF_QUADRATIC;                                                                                                                                                                    \
		}                                                                                                                                                                                                                           \
		else if ( ::_wcsicmp( _wcpArgV[1], L"quadraticapprox" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"quadraticapproximate" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"quadratic_approximate" ) == 0 ) {                                     \
			oOptions.ASSIGN = sl2::CResampler::SL2_FF_QUADRATICAPPROX;                                                                                                                                                              \
		}                                                                                                                                                                                                                           \
		else if ( ::_wcsicmp( _wcpArgV[1], L"quadraticmix" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"quadratic_mix" ) == 0 ) {                                                                                                           \
			oOptions.ASSIGN = sl2::CResampler::SL2_FF_QUADRATICMIX;                                                                                                                                                                 \
		}                                                                                                                                                                                                                           \
		else if ( ::_wcsicmp( _wcpArgV[1], L"kaiser" ) == 0 ) {                                                                                                                                                                     \
			oOptions.ASSIGN = sl2::CResampler::SL2_FF_KAISER;                                                                                                                                                                       \
		}                                                                                                                                                                                                                           \
		else if ( ::_wcsicmp( _wcpArgV[1], L"lanczos2" ) == 0 ) {                                                                                                                                                                   \
			oOptions.ASSIGN = sl2::CResampler::SL2_FF_LANCZOS2;                                                                                                                                                                     \
		}                                                                                                                                                                                                                           \
		else if ( ::_wcsicmp( _wcpArgV[1], L"lanczos3" ) == 0 ) {                                                                                                                                                                   \
			oOptions.ASSIGN = sl2::CResampler::SL2_FF_LANCZOS3;                                                                                                                                                                     \
		}                                                                                                                                                                                                                           \
		else if ( ::_wcsicmp( _wcpArgV[1], L"lanczos4" ) == 0 ) {                                                                                                                                                                   \
			oOptions.ASSIGN = sl2::CResampler::SL2_FF_LANCZOS4;                                                                                                                                                                     \
		}                                                                                                                                                                                                                           \
		else if ( ::_wcsicmp( _wcpArgV[1], L"lanczos6" ) == 0 ) {                                                                                                                                                                   \
			oOptions.ASSIGN = sl2::CResampler::SL2_FF_LANCZOS6;                                                                                                                                                                     \
		}                                                                                                                                                                                                                           \
		else if ( ::_wcsicmp( _wcpArgV[1], L"lanczos8" ) == 0 ) {                                                                                                                                                                   \
			oOptions.ASSIGN = sl2::CResampler::SL2_FF_LANCZOS8;                                                                                                                                                                     \
		}                                                                                                                                                                                                                           \
		else if ( ::_wcsicmp( _wcpArgV[1], L"lanczos12" ) == 0 ) {                                                                                                                                                                  \
			oOptions.ASSIGN = sl2::CResampler::SL2_FF_LANCZOS12;                                                                                                                                                                    \
		}                                                                                                                                                                                                                           \
		else if ( ::_wcsicmp( _wcpArgV[1], L"lanczos64" ) == 0 ) {                                                                                                                                                                  \
			oOptions.ASSIGN = sl2::CResampler::SL2_FF_LANCZOS64;                                                                                                                                                                    \
		}                                                                                                                                                                                                                           \
		else if ( ::_wcsicmp( _wcpArgV[1], L"mitchell" ) == 0 ) {                                                                                                                                                                   \
			oOptions.ASSIGN = sl2::CResampler::SL2_FF_MITCHELL;                                                                                                                                                                     \
		}                                                                                                                                                                                                                           \
		else if ( ::_wcsicmp( _wcpArgV[1], L"catmul" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"catmulrom" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"catmul_rom" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"catmul-rom" ) == 0 ) {                   \
			oOptions.ASSIGN = sl2::CResampler::SL2_FF_CATMULLROM;                                                                                                                                                                   \
		}                                                                                                                                                                                                                           \
		else if ( ::_wcsicmp( _wcpArgV[1], L"bspline" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"b-spline" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"b_spline" ) == 0 ) {                                                                      \
			oOptions.ASSIGN = sl2::CResampler::SL2_FF_BSPLINE;                                                                                                                                                                      \
		}                                                                                                                                                                                                                           \
		else if ( ::_wcsicmp( _wcpArgV[1], L"cardinal" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"card" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"cardinaluniform" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"cardinal_uniform" ) == 0 ) {           \
			oOptions.ASSIGN = sl2::CResampler::SL2_FF_CARDINALSPLINEUNIFORM;                                                                                                                                                        \
		}                                                                                                                                                                                                                           \
		else if ( ::_wcsicmp( _wcpArgV[1], L"hermite" ) == 0 ) {                                                                                                                                                                    \
			oOptions.ASSIGN = sl2::CResampler::SL2_FF_HERMITE;                                                                                                                                                                      \
		}                                                                                                                                                                                                                           \
		else if ( ::_wcsicmp( _wcpArgV[1], L"hamming" ) == 0 ) {                                                                                                                                                                    \
			oOptions.ASSIGN = sl2::CResampler::SL2_FF_HAMMING;                                                                                                                                                                      \
		}                                                                                                                                                                                                                           \
		else if ( ::_wcsicmp( _wcpArgV[1], L"hanning" ) == 0 ) {                                                                                                                                                                    \
			oOptions.ASSIGN = sl2::CResampler::SL2_FF_HANNING;                                                                                                                                                                      \
		}                                                                                                                                                                                                                           \
		else if ( ::_wcsicmp( _wcpArgV[1], L"blackman" ) == 0 ) {                                                                                                                                                                   \
			oOptions.ASSIGN = sl2::CResampler::SL2_FF_BLACKMAN;                                                                                                                                                                     \
		}                                                                                                                                                                                                                           \
		else if ( ::_wcsicmp( _wcpArgV[1], L"gaussiansharp" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"gaussian_sharp" ) == 0 ) {                                                                                                         \
			oOptions.ASSIGN = sl2::CResampler::SL2_FF_GAUSSIANSHARP;                                                                                                                                                                \
		}                                                                                                                                                                                                                           \
		else if ( ::_wcsicmp( _wcpArgV[1], L"gaussian" ) == 0 ) {                                                                                                                                                                   \
			oOptions.ASSIGN = sl2::CResampler::SL2_FF_GAUSSIAN;                                                                                                                                                                     \
		}                                                                                                                                                                                                                           \
		else if ( ::_wcsicmp( _wcpArgV[1], L"bell" ) == 0 ) {                                                                                                                                                                       \
			oOptions.ASSIGN = sl2::CResampler::SL2_FF_BELL;                                                                                                                                                                         \
		}                                                                                                                                                                                                                           \
		else {                                                                                                                                                                                                                      \
			SL2_ERRORT( std::format( L"Invalid \"" #COND "\": \"{}\". Must be box, tent, quadraticsharp, quadratic, quadraticapprox, quadraticmix, kaiser, lanczos2, lanczos3, lanczos4, lanczos6, lanczos8, lanczos12, lanczos64, "\
				"mitchell, catmul, bspline, cardinal, hermite, hamming, hanning, blackman, gaussiansharp, gaussian, or bell.",                                                                                                      \
				_wcpArgV[1] ).c_str(), sl2::SL2_E_INVALIDCALL );                                                                                                                                                                    \
		}                                                                                                                                                                                                                           \
		COPY = oOptions.ASSIGN;                                                                                                                                                                                                     \
		SL2_ADV( 2 );                                                                                                                                                                                                               \
	}

			SL2_RESAMPLE( filter, fFilterFuncW, oOptions.fFilterFuncH = oOptions.fFilterFuncD = oOptions.fAlphaFilterFuncW = oOptions.fAlphaFilterFuncH = oOptions.fAlphaFilterFuncD );
			SL2_RESAMPLE( filtera, fAlphaFilterFuncW, oOptions.fAlphaFilterFuncH = oOptions.fAlphaFilterFuncD );
			SL2_RESAMPLE( filter_alpha, fAlphaFilterFuncW, oOptions.fAlphaFilterFuncH = oOptions.fAlphaFilterFuncD );
			SL2_RESAMPLE( filterw, fFilterFuncW, oOptions.fAlphaFilterFuncW );
			SL2_RESAMPLE( filterh, fFilterFuncH, oOptions.fAlphaFilterFuncH );
			SL2_RESAMPLE( filterd, fFilterFuncD, oOptions.fAlphaFilterFuncD );
			SL2_RESAMPLE( filterw_color, fFilterFuncW, oOptions.fFilterFuncW );
			SL2_RESAMPLE( filterh_color, fFilterFuncH, oOptions.fFilterFuncH );
			SL2_RESAMPLE( filterd_color, fFilterFuncD, oOptions.fFilterFuncD );
			SL2_RESAMPLE( filterw_alpha, fAlphaFilterFuncW, oOptions.fAlphaFilterFuncW );
			SL2_RESAMPLE( filterh_alpha, fAlphaFilterFuncH, oOptions.fAlphaFilterFuncH );
			SL2_RESAMPLE( filterd_alpha, fAlphaFilterFuncD, oOptions.fAlphaFilterFuncD );

			SL2_RESAMPLE( mip_filter, fMipFilterFuncW, oOptions.fMipFilterFuncH = oOptions.fMipFilterFuncD = oOptions.fMipAlphaFilterFuncW = oOptions.fMipAlphaFilterFuncH = oOptions.fMipAlphaFilterFuncD );
			SL2_RESAMPLE( mip_filtera, fMipAlphaFilterFuncW, oOptions.fMipAlphaFilterFuncH = oOptions.fMipAlphaFilterFuncD );
			SL2_RESAMPLE( mip_filter_alpha, fMipAlphaFilterFuncW, oOptions.fMipAlphaFilterFuncH = oOptions.fMipAlphaFilterFuncD );
			SL2_RESAMPLE( mip_filterw, fMipFilterFuncW, oOptions.fMipAlphaFilterFuncW );
			SL2_RESAMPLE( mip_filterh, fMipFilterFuncH, oOptions.fMipAlphaFilterFuncH );
			SL2_RESAMPLE( mip_filterd, fMipFilterFuncD, oOptions.fMipAlphaFilterFuncD );
			SL2_RESAMPLE( mip_filterw_color, fMipFilterFuncW, oOptions.fMipFilterFuncW );
			SL2_RESAMPLE( mip_filterh_color, fMipFilterFuncH, oOptions.fMipFilterFuncH );
			SL2_RESAMPLE( mip_filterd_color, fMipFilterFuncD, oOptions.fMipFilterFuncD );
			SL2_RESAMPLE( mip_filterw_alpha, fMipAlphaFilterFuncW, oOptions.fMipAlphaFilterFuncW );
			SL2_RESAMPLE( mip_filterh_alpha, fMipAlphaFilterFuncH, oOptions.fMipAlphaFilterFuncH );
			SL2_RESAMPLE( mip_filterd_alpha, fMipAlphaFilterFuncD, oOptions.fMipAlphaFilterFuncD );
                
#undef SL2_RESAMPLE

			if ( SL2_CHECK( 1, nomips ) || SL2_CHECK( 1, nomipmaps ) || SL2_CHECK( 1, no_mips ) || SL2_CHECK( 1, no_mipmaps ) ) {
				oOptions.mhMipHandling = sl2::SL2_MH_REMOVE_EXISTING;
				SL2_ADV( 1 );
			}
			if ( SL2_CHECK( 2, nmips ) ) {
				oOptions.sTotalMips = ::_wtoi( _wcpArgV[1] );
				SL2_ADV( 2 );
			}
			if ( SL2_CHECK( 1, keepmips ) || SL2_CHECK( 1, keepmipmaps ) || SL2_CHECK( 1, keep_mips ) || SL2_CHECK( 1, keep_mipmaps ) ) {
				oOptions.mhMipHandling = sl2::SL2_MH_KEEP_EXISTING;
				SL2_ADV( 1 );
			}

			if ( SL2_CHECK( 3, clamp ) || SL2_CHECK( 3, clamp2 ) ) {
				oOptions.ui32ClampW = ::_wtoi( _wcpArgV[1] );
				oOptions.ui32ClampH = ::_wtoi( _wcpArgV[2] );
				SL2_ADV( 3 );
			}
			if ( SL2_CHECK( 4, clamp3 ) ) {
				oOptions.ui32ClampW = ::_wtoi( _wcpArgV[1] );
				oOptions.ui32ClampH = ::_wtoi( _wcpArgV[2] );
				oOptions.ui32ClampD = ::_wtoi( _wcpArgV[3] );
				SL2_ADV( 4 );
			}

			if ( SL2_CHECK( 2, textureaddressing ) || SL2_CHECK( 2, ta ) ) {
				if ( ::_wcsicmp( _wcpArgV[1], L"clamp" ) == 0 ) {
					oOptions.rResample.taColorW = sl2::SL2_TA_CLAMP;
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"repeat" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"wrap" ) == 0 ) {
					oOptions.rResample.taColorW = sl2::SL2_TA_WRAP;
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"mirror" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"reflect" ) == 0 ) {
					oOptions.rResample.taColorW = sl2::SL2_TA_MIRROR;
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"mirroronce" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"mirror_once" ) == 0 ) {
					oOptions.rResample.taColorW = sl2::SL2_TA_MIRROR_ONCE;
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"border" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"bordercolor" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"border_color" ) == 0 ) {
					oOptions.rResample.taColorW = sl2::SL2_TA_BORDER;
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"no_border" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"nul_border" ) == 0 ) {
					oOptions.rResample.taColorW = sl2::SL2_TA_NULL_BORDER;
				}
				else {
					SL2_ERRORT( std::format( L"Invalid \"textureaddressing\"|\"ta\": \"{}\". Must be clamp, wrap, mirror, mirroronce, border, or no_border.",
						_wcpArgV[1] ).c_str(), sl2::SL2_E_INVALIDCALL );
				}
				oOptions.rResample.taColorH = oOptions.rResample.taColorD = oOptions.rResample.taAlphaW = oOptions.rResample.taAlphaH = oOptions.rResample.taAlphaD = oOptions.rResample.taColorW;
				oOptions.rMipResample.taColorW = oOptions.rMipResample.taAlphaW = 
					oOptions.rMipResample.taColorH = oOptions.rMipResample.taAlphaH = 
					oOptions.rMipResample.taColorD = oOptions.rMipResample.taAlphaD = oOptions.rResample.taColorW;
				SL2_ADV( 2 );
			}

			if ( SL2_CHECK( 2, textureaddressingw ) || SL2_CHECK( 2, taw ) ) {
				if ( ::_wcsicmp( _wcpArgV[1], L"clamp" ) == 0 ) {
					oOptions.rResample.taColorW = sl2::SL2_TA_CLAMP;
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"repeat" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"wrap" ) == 0 ) {
					oOptions.rResample.taColorW = sl2::SL2_TA_WRAP;
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"mirror" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"reflect" ) == 0 ) {
					oOptions.rResample.taColorW = sl2::SL2_TA_MIRROR;
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"mirroronce" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"mirror_once" ) == 0 ) {
					oOptions.rResample.taColorW = sl2::SL2_TA_MIRROR_ONCE;
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"border" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"bordercolor" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"border_color" ) == 0 ) {
					oOptions.rResample.taColorW = sl2::SL2_TA_BORDER;
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"no_border" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"nul_border" ) == 0 ) {
					oOptions.rResample.taColorW = sl2::SL2_TA_NULL_BORDER;
				}
				else {
					SL2_ERRORT( std::format( L"Invalid \"textureaddressingw\"|\"taw\": \"{}\". Must be clamp, wrap, mirror, mirroronce, border, or no_border.",
						_wcpArgV[1] ).c_str(), sl2::SL2_E_INVALIDCALL );
				}
				oOptions.rResample.taAlphaW = oOptions.rResample.taColorW;
				oOptions.rMipResample.taColorW = oOptions.rMipResample.taAlphaW = oOptions.rResample.taColorW;
				SL2_ADV( 2 );
			}
			if ( SL2_CHECK( 2, textureaddressingh ) || SL2_CHECK( 2, tah ) ) {
				if ( ::_wcsicmp( _wcpArgV[1], L"clamp" ) == 0 ) {
					oOptions.rResample.taColorH = sl2::SL2_TA_CLAMP;
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"repeat" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"wrap" ) == 0 ) {
					oOptions.rResample.taColorH = sl2::SL2_TA_WRAP;
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"mirror" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"reflect" ) == 0 ) {
					oOptions.rResample.taColorH = sl2::SL2_TA_MIRROR;
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"mirroronce" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"mirror_once" ) == 0 ) {
					oOptions.rResample.taColorH = sl2::SL2_TA_MIRROR_ONCE;
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"border" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"bordercolor" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"border_color" ) == 0 ) {
					oOptions.rResample.taColorH = sl2::SL2_TA_BORDER;
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"no_border" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"nul_border" ) == 0 ) {
					oOptions.rResample.taColorW = sl2::SL2_TA_NULL_BORDER;
				}
				else {
					SL2_ERRORT( std::format( L"Invalid \"textureaddressingh\"|\"tah\": \"{}\". Must be clamp, wrap, mirror, mirroronce, border, or no_border.",
						_wcpArgV[1] ).c_str(), sl2::SL2_E_INVALIDCALL );
				}
				oOptions.rResample.taAlphaH = oOptions.rResample.taColorH;
				oOptions.rMipResample.taColorH = oOptions.rMipResample.taAlphaH = oOptions.rResample.taColorH;
				SL2_ADV( 2 );
			}
			if ( SL2_CHECK( 2, textureaddressingd ) || SL2_CHECK( 2, tad ) ) {
				if ( ::_wcsicmp( _wcpArgV[1], L"clamp" ) == 0 ) {
					oOptions.rResample.taColorD = sl2::SL2_TA_CLAMP;
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"repeat" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"wrap" ) == 0 ) {
					oOptions.rResample.taColorD = sl2::SL2_TA_WRAP;
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"mirror" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"reflect" ) == 0 ) {
					oOptions.rResample.taColorD = sl2::SL2_TA_MIRROR;
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"mirroronce" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"mirror_once" ) == 0 ) {
					oOptions.rResample.taColorD = sl2::SL2_TA_MIRROR_ONCE;
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"border" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"bordercolor" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"border_color" ) == 0 ) {
					oOptions.rResample.taColorD = sl2::SL2_TA_BORDER;
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"no_border" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"nul_border" ) == 0 ) {
					oOptions.rResample.taColorW = sl2::SL2_TA_NULL_BORDER;
				}
				else {
					SL2_ERRORT( std::format( L"Invalid \"textureaddressingd\"|\"tad\": \"{}\". Must be clamp, wrap, mirror, mirroronce, border, or no_border.",
						_wcpArgV[1] ).c_str(), sl2::SL2_E_INVALIDCALL );
				}
				oOptions.rResample.taAlphaD = oOptions.rResample.taColorD;
				oOptions.rMipResample.taColorD = oOptions.rMipResample.taAlphaD = oOptions.rResample.taColorD;
				SL2_ADV( 2 );
			}

			if ( SL2_CHECK( 2, textureaddressingw_color ) || SL2_CHECK( 2, taw_color ) ) {
				if ( ::_wcsicmp( _wcpArgV[1], L"clamp" ) == 0 ) {
					oOptions.rResample.taColorW = sl2::SL2_TA_CLAMP;
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"repeat" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"wrap" ) == 0 ) {
					oOptions.rResample.taColorW = sl2::SL2_TA_WRAP;
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"mirror" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"reflect" ) == 0 ) {
					oOptions.rResample.taColorW = sl2::SL2_TA_MIRROR;
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"mirroronce" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"mirror_once" ) == 0 ) {
					oOptions.rResample.taColorW = sl2::SL2_TA_MIRROR_ONCE;
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"border" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"bordercolor" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"border_color" ) == 0 ) {
					oOptions.rResample.taColorW = sl2::SL2_TA_BORDER;
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"no_border" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"nul_border" ) == 0 ) {
					oOptions.rResample.taColorW = sl2::SL2_TA_NULL_BORDER;
				}
				else {
					SL2_ERRORT( std::format( L"Invalid \"textureaddressingw_color\"|\"taw_color\": \"{}\". Must be clamp, wrap, mirror, mirroronce, border, or no_border.",
						_wcpArgV[1] ).c_str(), sl2::SL2_E_INVALIDCALL );
				}
				oOptions.rMipResample.taColorW = oOptions.rResample.taColorW;
				SL2_ADV( 2 );
			}
			if ( SL2_CHECK( 2, textureaddressingh_color ) || SL2_CHECK( 2, tah_color ) ) {
				if ( ::_wcsicmp( _wcpArgV[1], L"clamp" ) == 0 ) {
					oOptions.rResample.taColorH = sl2::SL2_TA_CLAMP;
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"repeat" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"wrap" ) == 0 ) {
					oOptions.rResample.taColorH = sl2::SL2_TA_WRAP;
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"mirror" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"reflect" ) == 0 ) {
					oOptions.rResample.taColorH = sl2::SL2_TA_MIRROR;
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"mirroronce" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"mirror_once" ) == 0 ) {
					oOptions.rResample.taColorH = sl2::SL2_TA_MIRROR_ONCE;
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"border" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"bordercolor" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"border_color" ) == 0 ) {
					oOptions.rResample.taColorH = sl2::SL2_TA_BORDER;
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"no_border" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"nul_border" ) == 0 ) {
					oOptions.rResample.taColorW = sl2::SL2_TA_NULL_BORDER;
				}
				else {
					SL2_ERRORT( std::format( L"Invalid \"textureaddressingh_color\"|\"tah_color\": \"{}\". Must be clamp, wrap, mirror, mirroronce, border, or no_border.",
						_wcpArgV[1] ).c_str(), sl2::SL2_E_INVALIDCALL );
				}
				oOptions.rMipResample.taColorH = oOptions.rResample.taColorH;
				SL2_ADV( 2 );
			}
			if ( SL2_CHECK( 2, textureaddressingd_color ) || SL2_CHECK( 2, tad_color ) ) {
				if ( ::_wcsicmp( _wcpArgV[1], L"clamp" ) == 0 ) {
					oOptions.rResample.taColorD = sl2::SL2_TA_CLAMP;
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"repeat" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"wrap" ) == 0 ) {
					oOptions.rResample.taColorD = sl2::SL2_TA_WRAP;
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"mirror" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"reflect" ) == 0 ) {
					oOptions.rResample.taColorD = sl2::SL2_TA_MIRROR;
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"mirroronce" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"mirror_once" ) == 0 ) {
					oOptions.rResample.taColorD = sl2::SL2_TA_MIRROR_ONCE;
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"border" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"bordercolor" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"border_color" ) == 0 ) {
					oOptions.rResample.taColorD = sl2::SL2_TA_BORDER;
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"no_border" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"nul_border" ) == 0 ) {
					oOptions.rResample.taColorW = sl2::SL2_TA_NULL_BORDER;
				}
				else {
					SL2_ERRORT( std::format( L"Invalid \"textureaddressingd_color\"|\"tad_color\": \"{}\". Must be clamp, wrap, mirror, mirroronce, border, or no_border.",
						_wcpArgV[1] ).c_str(), sl2::SL2_E_INVALIDCALL );
				}
				oOptions.rMipResample.taColorD = oOptions.rResample.taColorD;
				SL2_ADV( 2 );
			}

			if ( SL2_CHECK( 2, textureaddressingw_alpha ) || SL2_CHECK( 2, taw_alpha ) ) {
				if ( ::_wcsicmp( _wcpArgV[1], L"clamp" ) == 0 ) {
					oOptions.rResample.taAlphaW = sl2::SL2_TA_CLAMP;
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"repeat" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"wrap" ) == 0 ) {
					oOptions.rResample.taAlphaW = sl2::SL2_TA_WRAP;
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"mirror" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"reflect" ) == 0 ) {
					oOptions.rResample.taAlphaW = sl2::SL2_TA_MIRROR;
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"mirroronce" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"mirror_once" ) == 0 ) {
					oOptions.rResample.taAlphaW = sl2::SL2_TA_MIRROR_ONCE;
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"border" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"bordercolor" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"border_color" ) == 0 ) {
					oOptions.rResample.taAlphaW = sl2::SL2_TA_BORDER;
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"no_border" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"nul_border" ) == 0 ) {
					oOptions.rResample.taColorW = sl2::SL2_TA_NULL_BORDER;
				}
				else {
					SL2_ERRORT( std::format( L"Invalid \"textureaddressingw_alpha\"|\"taw_alpha\": \"{}\". Must be clamp, wrap, mirror, mirroronce, border, or no_border.",
						_wcpArgV[1] ).c_str(), sl2::SL2_E_INVALIDCALL );
				}
				oOptions.rMipResample.taAlphaW = oOptions.rResample.taAlphaW;
				SL2_ADV( 2 );
			}
			if ( SL2_CHECK( 2, textureaddressingh_alpha ) || SL2_CHECK( 2, tah_alpha ) ) {
				if ( ::_wcsicmp( _wcpArgV[1], L"clamp" ) == 0 ) {
					oOptions.rResample.taAlphaH = sl2::SL2_TA_CLAMP;
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"repeat" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"wrap" ) == 0 ) {
					oOptions.rResample.taAlphaH = sl2::SL2_TA_WRAP;
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"mirror" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"reflect" ) == 0 ) {
					oOptions.rResample.taAlphaH = sl2::SL2_TA_MIRROR;
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"mirroronce" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"mirror_once" ) == 0 ) {
					oOptions.rResample.taAlphaH = sl2::SL2_TA_MIRROR_ONCE;
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"border" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"bordercolor" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"border_color" ) == 0 ) {
					oOptions.rResample.taAlphaH = sl2::SL2_TA_BORDER;
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"no_border" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"nul_border" ) == 0 ) {
					oOptions.rResample.taColorW = sl2::SL2_TA_NULL_BORDER;
				}
				else {
					SL2_ERRORT( std::format( L"Invalid \"textureaddressingh_alpha\"|\"tah_alpha\": \"{}\". Must be clamp, wrap, mirror, mirroronce, border, or no_border.",
						_wcpArgV[1] ).c_str(), sl2::SL2_E_INVALIDCALL );
				}
				oOptions.rMipResample.taAlphaH = oOptions.rResample.taAlphaH;
				SL2_ADV( 2 );
			}
			if ( SL2_CHECK( 2, textureaddressingd_alpha ) || SL2_CHECK( 2, tad_alpha ) ) {
				if ( ::_wcsicmp( _wcpArgV[1], L"clamp" ) == 0 ) {
					oOptions.rResample.taAlphaD = sl2::SL2_TA_CLAMP;
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"repeat" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"wrap" ) == 0 ) {
					oOptions.rResample.taAlphaD = sl2::SL2_TA_WRAP;
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"mirror" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"reflect" ) == 0 ) {
					oOptions.rResample.taAlphaD = sl2::SL2_TA_MIRROR;
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"mirroronce" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"mirror_once" ) == 0 ) {
					oOptions.rResample.taAlphaD = sl2::SL2_TA_MIRROR_ONCE;
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"border" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"bordercolor" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"border_color" ) == 0 ) {
					oOptions.rResample.taAlphaD = sl2::SL2_TA_BORDER;
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"no_border" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"nul_border" ) == 0 ) {
					oOptions.rResample.taColorW = sl2::SL2_TA_NULL_BORDER;
				}
				else {
					SL2_ERRORT( std::format( L"Invalid \"textureaddressingd_alpha\"|\"tad_alpha\": \"{}\". Must be clamp, wrap, mirror, mirroronce, border, or no_border.",
						_wcpArgV[1] ).c_str(), sl2::SL2_E_INVALIDCALL );
				}
				oOptions.rMipResample.taAlphaD = oOptions.rResample.taAlphaD;
				SL2_ADV( 2 );
			}

			if ( SL2_CHECK( 5, border_color ) ) {
				oOptions.rResample.dBorderColor[0] = ::_wtoi( _wcpArgV[1] );
				oOptions.rResample.dBorderColor[1] = ::_wtoi( _wcpArgV[2] );
				oOptions.rResample.dBorderColor[2] = ::_wtoi( _wcpArgV[3] );
				oOptions.rResample.dBorderColor[3] = ::_wtoi( _wcpArgV[4] );
				oOptions.rMipResample.dBorderColor[0] = oOptions.rResample.dBorderColor[0];
				oOptions.rMipResample.dBorderColor[1] = oOptions.rResample.dBorderColor[1];
				oOptions.rMipResample.dBorderColor[2] = oOptions.rResample.dBorderColor[2];
				oOptions.rMipResample.dBorderColor[3] = oOptions.rResample.dBorderColor[3];
				SL2_ADV( 5 );
			}


			if ( SL2_CHECK( 3, bake_tex_mapping_u ) ) {
				if ( ::_wcsicmp( _wcpArgV[1], L"clamp" ) == 0 ) {
					oOptions.rResample.taColorW = sl2::SL2_TA_CLAMP;
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"repeat" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"wrap" ) == 0 ) {
					oOptions.rResample.taColorW = sl2::SL2_TA_WRAP;
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"mirror" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"reflect" ) == 0 ) {
					oOptions.rResample.taColorW = sl2::SL2_TA_MIRROR;
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"mirroronce" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"mirror_once" ) == 0 ) {
					oOptions.rResample.taColorW = sl2::SL2_TA_MIRROR_ONCE;
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"border" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"bordercolor" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"border_color" ) == 0 ) {
					oOptions.rResample.taColorW = sl2::SL2_TA_BORDER;
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"no_border" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"nul_border" ) == 0 ) {
					oOptions.rResample.taColorW = sl2::SL2_TA_NULL_BORDER;
				}
				else {
					SL2_ERRORT( std::format( L"Invalid \"bake_tex_mapping_u\": \"{}\". Must be clamp, wrap, mirror, mirroronce, border, or no_border.",
						_wcpArgV[1] ).c_str(), sl2::SL2_E_INVALIDCALL );
				}
				oOptions.ui32BakedW = ::_wtoi( _wcpArgV[2] );
				oOptions.rResample.taAlphaW = oOptions.rResample.taColorW;
				oOptions.rMipResample.taColorW = oOptions.rMipResample.taAlphaW = oOptions.rResample.taColorW;
				SL2_ADV( 3 );
			}
			if ( SL2_CHECK( 3, bake_tex_mapping_v ) ) {
				if ( ::_wcsicmp( _wcpArgV[1], L"clamp" ) == 0 ) {
					oOptions.rResample.taColorH = sl2::SL2_TA_CLAMP;
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"repeat" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"wrap" ) == 0 ) {
					oOptions.rResample.taColorH = sl2::SL2_TA_WRAP;
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"mirror" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"reflect" ) == 0 ) {
					oOptions.rResample.taColorH = sl2::SL2_TA_MIRROR;
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"mirroronce" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"mirror_once" ) == 0 ) {
					oOptions.rResample.taColorH = sl2::SL2_TA_MIRROR_ONCE;
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"border" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"bordercolor" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"border_color" ) == 0 ) {
					oOptions.rResample.taColorH = sl2::SL2_TA_BORDER;
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"no_border" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"nul_border" ) == 0 ) {
					oOptions.rResample.taColorH = sl2::SL2_TA_NULL_BORDER;
				}
				else {
					SL2_ERRORT( std::format( L"Invalid \"bake_tex_mapping_v\": \"{}\". Must be clamp, wrap, mirror, mirroronce, border, or no_border.",
						_wcpArgV[1] ).c_str(), sl2::SL2_E_INVALIDCALL );
				}
				oOptions.ui32BakedH = ::_wtoi( _wcpArgV[2] );
				oOptions.rResample.taAlphaH = oOptions.rResample.taColorH;
				oOptions.rMipResample.taColorH = oOptions.rMipResample.taAlphaH = oOptions.rResample.taColorH;
				SL2_ADV( 3 );
			}
			if ( SL2_CHECK( 3, bake_tex_mapping_w ) ) {
				if ( ::_wcsicmp( _wcpArgV[1], L"clamp" ) == 0 ) {
					oOptions.rResample.taColorD = sl2::SL2_TA_CLAMP;
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"repeat" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"wrap" ) == 0 ) {
					oOptions.rResample.taColorD = sl2::SL2_TA_WRAP;
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"mirror" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"reflect" ) == 0 ) {
					oOptions.rResample.taColorD = sl2::SL2_TA_MIRROR;
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"mirroronce" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"mirror_once" ) == 0 ) {
					oOptions.rResample.taColorD = sl2::SL2_TA_MIRROR_ONCE;
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"border" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"bordercolor" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"border_color" ) == 0 ) {
					oOptions.rResample.taColorD = sl2::SL2_TA_BORDER;
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"no_border" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"nul_border" ) == 0 ) {
					oOptions.rResample.taColorD = sl2::SL2_TA_NULL_BORDER;
				}
				else {
					SL2_ERRORT( std::format( L"Invalid \"bake_tex_mapping_w\": \"{}\". Must be clamp, wrap, mirror, mirroronce, border, or no_border.",
						_wcpArgV[1] ).c_str(), sl2::SL2_E_INVALIDCALL );
				}
				oOptions.ui32BakedD = ::_wtoi( _wcpArgV[2] );
				oOptions.rResample.taAlphaD = oOptions.rResample.taColorD;
				oOptions.rMipResample.taColorD = oOptions.rMipResample.taAlphaD = oOptions.rResample.taColorD;
				SL2_ADV( 3 );
			}


			if ( SL2_CHECK( 2, nm_channel ) ) {
				if ( ::_wcsicmp( _wcpArgV[1], L"r" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"red" ) == 0 ) {
					oOptions.caChannelAccess = sl2::SL2_CA_R;
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"g" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"green" ) == 0 ) {
					oOptions.caChannelAccess = sl2::SL2_CA_G;
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"b" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"blue" ) == 0 ) {
					oOptions.caChannelAccess = sl2::SL2_CA_B;
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"a" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"alpha" ) == 0 ) {
					oOptions.caChannelAccess = sl2::SL2_CA_A;
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"max" ) == 0 ) {
					oOptions.caChannelAccess = sl2::SL2_CA_MAX;
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"rgb" ) == 0 ) {
					oOptions.caChannelAccess = sl2::SL2_CA_AVERAGE;
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"colorspace" ) == 0 ) {
					oOptions.caChannelAccess = sl2::SL2_CA_WEIGHTED_AVERAGE;
				}
				else {
					SL2_ERRORT( std::format( L"Invalid \"nm_channel\": \"{}\". Must be r, g, b, a, rgb, max, or colorspace.",
						_wcpArgV[1] ).c_str(), sl2::SL2_E_INVALIDCALL );
				}
				SL2_ADV( 2 );
			}
			if ( SL2_CHECK( 1, norm ) || SL2_CHECK( 1, normalize ) ) {
				oOptions.bNormalizeMips = true;
				SL2_ADV( 1 );
			}
			if ( SL2_CHECK( 1, opengl ) || SL2_CHECK( 1, unity ) || SL2_CHECK( 1, blender ) || SL2_CHECK( 1, maya ) ) {
				oOptions.dNormalYAxis = 1.0;
				SL2_ADV( 1 );
			}
			if ( SL2_CHECK( 1, directx ) || SL2_CHECK( 1, ue4 ) || SL2_CHECK( 1, unreal ) || SL2_CHECK( 1, unrealengine ) || SL2_CHECK( 1, ue ) || SL2_CHECK( 1, 3dsmax ) ) {
				oOptions.dNormalYAxis = -1.0;
				SL2_ADV( 1 );
			}
			if ( SL2_CHECK( 1, n3x3 ) ) {
				if ( !oOptions.kKernel.CreateSobel3x3() ) {
					SL2_ERRORT( std::format( L"\"n3x3\": Out of memory allocating Sobel kernel." ).c_str(), sl2::SL2_E_OUTOFMEMORY );
				}
				oOptions.bNormalizeMips = true;
				SL2_ADV( 1 );
			}
			if ( SL2_CHECK( 1, n5x5 ) ) {
				if ( !oOptions.kKernel.CreateSobel5x5() ) {
					SL2_ERRORT( std::format( L"\"n5x5\": Out of memory allocating Sobel kernel." ).c_str(), sl2::SL2_E_OUTOFMEMORY );
				}
				oOptions.bNormalizeMips = true;
				SL2_ADV( 1 );
			}
			if ( SL2_CHECK( 1, n7x7 ) ) {
				if ( !oOptions.kKernel.CreateSobel7x7() ) {
					SL2_ERRORT( std::format( L"\"n7x7\": Out of memory allocating Sobel kernel." ).c_str(), sl2::SL2_E_OUTOFMEMORY );
				}
				oOptions.bNormalizeMips = true;
				SL2_ADV( 1 );
			}
			if ( SL2_CHECK( 1, n9x9 ) ) {
				if ( !oOptions.kKernel.CreateSobel9x9() ) {
					SL2_ERRORT( std::format( L"\"n9x9\": Out of memory allocating Sobel kernel." ).c_str(), sl2::SL2_E_OUTOFMEMORY );
				}
				oOptions.bNormalizeMips = true;
				SL2_ADV( 1 );
			}
			if ( SL2_CHECK( 2, scale ) || SL2_CHECK( 2, nm_z ) ) {
				oOptions.dNormalScale = ::_wtof( _wcpArgV[1] );
				SL2_ADV( 2 );
			}


			if ( SL2_CHECK( 1, gen_pal ) || SL2_CHECK( 1, gen_palette ) ) {
				oOptions.bGenNewPalatte = true;
				SL2_ADV( 1 );
			}
			if ( SL2_CHECK( 2, gen_pal_iterations ) ) {
				sl2::CFormat::m_skMeansIterations = ::_wtoi( _wcpArgV[1] );
				SL2_ADV( 2 );
			}
			if ( SL2_CHECK( 2, pal_dither ) ) {
				if ( ::_wcsicmp( _wcpArgV[1], L"floyd" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"floyd-steinburg" ) == 0 ) {
					sl2::CFormat::m_dDither = sl2::SL2_D_FLOYD_STEINBERG;
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"jjn" ) == 0 ) {
					sl2::CFormat::m_dDither = sl2::SL2_D_JARVIS_JUDICE_NINKE;
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"stucki" ) == 0 ) {
					sl2::CFormat::m_dDither = sl2::SL2_D_STUCKI;
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"burkes" ) == 0 ) {
					sl2::CFormat::m_dDither = sl2::SL2_D_BURKES;
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"sierra" ) == 0 ) {
					sl2::CFormat::m_dDither = sl2::SL2_D_SIERRA;
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"sierra2row" ) == 0 ) {
					sl2::CFormat::m_dDither = sl2::SL2_D_SIERRA_2;
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"sierralite" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"sierra_lite" ) == 0 ) {
					sl2::CFormat::m_dDither = sl2::SL2_D_SIERRA_LITE;
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"atkinson" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"atk" ) == 0 ) {
					sl2::CFormat::m_dDither = sl2::SL2_D_ATKINSON;
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"bayer4" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"bayer4x4" ) == 0 ) {
					sl2::CFormat::m_dDither = sl2::SL2_D_BAYER_4X4;
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"bayer8" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"bayer8x8" ) == 0 ) {
					sl2::CFormat::m_dDither = sl2::SL2_D_BAYER_8X8;
				}
				else {
					SL2_ERRORT( std::format( L"Invalid \"pal_dither\": \"{}\". Must be floyd, jjn, stucki, burkes, sierra, sierra2row, sierralite, atkinson, bayer4 or bayer8.",
						_wcpArgV[1] ).c_str(), sl2::SL2_E_INVALIDCALL );
				}
				SL2_ADV( 2 );
			}
			if ( SL2_CHECK( 5, dither_error_weight ) || SL2_CHECK( 5, dither_error_weights ) ) {
				sl2::CFormat::m_vDitherFactor = sl2::CVector4<SL2_ST_RAW>( ::_wtof( _wcpArgV[1] ), ::_wtof( _wcpArgV[2] ), ::_wtof( _wcpArgV[3] ), ::_wtof( _wcpArgV[4] ) );
				if ( sl2::CFormat::m_vDitherFactor.IsNan() ) {
					SL2_ERRORT( std::format( L"Invalid \"dither_error_weight\": \"{}\". Invalid paramater.  Must be <float> <float> <float> <float>.",
						_wcpArgV[1] ).c_str(), sl2::SL2_E_INVALIDCALL );
				}
				SL2_ADV( 5 );
			}
			if ( SL2_CHECK( 1, dither_error_weight_full ) || SL2_CHECK( 1, dither_error_weight_100 ) ) {
				sl2::CFormat::m_vDitherFactor = sl2::CVector4<SL2_ST_RAW>( 1.0, 1.0, 1.0, 1.0 );
				SL2_ADV( 1 );
			}
			if ( SL2_CHECK( 1, dither_error_weight_75 ) ) {
				sl2::CFormat::m_vDitherFactor = sl2::CVector4<SL2_ST_RAW>( 0.75, 0.75, 0.75, 1.0 );
				SL2_ADV( 1 );
			}
			if ( SL2_CHECK( 1, dither_error_weight_half ) || SL2_CHECK( 1, dither_error_weight_50 ) ) {
				sl2::CFormat::m_vDitherFactor = sl2::CVector4<SL2_ST_RAW>( 0.5, 0.5, 0.5, 1.0 );
				SL2_ADV( 1 );
			}
			if ( SL2_CHECK( 1, dither_error_weight_25 ) ) {
				sl2::CFormat::m_vDitherFactor = sl2::CVector4<SL2_ST_RAW>( 0.25, 0.25, 0.25, 1.0 );
				SL2_ADV( 1 );
			}
			if ( SL2_CHECK( 2, dither_error_weight_perceptual ) || SL2_CHECK( 2, dither_error_weight_perc ) ) {
				if ( ::_wcsicmp( _wcpArgV[1], L"REC_709" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"REC709" ) == 0 ) {
					sl2::CFormat::m_vDitherFactor = sl2::CVector4<SL2_ST_RAW>(
						sl2::CFormat::Luma( sl2::SL2_LS_REC_709 ).dRgb[0],
						sl2::CFormat::Luma( sl2::SL2_LS_REC_709 ).dRgb[1],
						sl2::CFormat::Luma( sl2::SL2_LS_REC_709 ).dRgb[2], 1.0 );
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"REC_2020" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"REC2020" ) == 0 ) {
					sl2::CFormat::m_vDitherFactor = sl2::CVector4<SL2_ST_RAW>(
						sl2::CFormat::Luma( sl2::SL2_LS_REC_2020 ).dRgb[0],
						sl2::CFormat::Luma( sl2::SL2_LS_REC_2020 ).dRgb[1],
						sl2::CFormat::Luma( sl2::SL2_LS_REC_2020 ).dRgb[2], 1.0 );
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"SMPTC" ) == 0 ) {
					sl2::CFormat::m_vDitherFactor = sl2::CVector4<SL2_ST_RAW>(
						sl2::CFormat::Luma( sl2::SL2_LS_SMPTC ).dRgb[0],
						sl2::CFormat::Luma( sl2::SL2_LS_SMPTC ).dRgb[1],
						sl2::CFormat::Luma( sl2::SL2_LS_SMPTC ).dRgb[2], 1.0 );
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"REC_601" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"REC601" ) == 0 ) {
					sl2::CFormat::m_vDitherFactor = sl2::CVector4<SL2_ST_RAW>(
						sl2::CFormat::Luma( sl2::SL2_LS_REC_601 ).dRgb[0],
						sl2::CFormat::Luma( sl2::SL2_LS_REC_601 ).dRgb[1],
						sl2::CFormat::Luma( sl2::SL2_LS_REC_601 ).dRgb[2], 1.0 );
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"CIE_1931" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"CIE1931" ) == 0 ) {
					sl2::CFormat::m_vDitherFactor = sl2::CVector4<SL2_ST_RAW>(
						sl2::CFormat::Luma( sl2::SL2_LS_CIE_1931 ).dRgb[0],
						sl2::CFormat::Luma( sl2::SL2_LS_CIE_1931 ).dRgb[1],
						sl2::CFormat::Luma( sl2::SL2_LS_CIE_1931 ).dRgb[2], 1.0 );
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"NTSC_1953" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"NTSC1953" ) == 0 ) {
					sl2::CFormat::m_vDitherFactor = sl2::CVector4<SL2_ST_RAW>(
						sl2::CFormat::Luma( sl2::SL2_LS_NTSC_1953 ).dRgb[0],
						sl2::CFormat::Luma( sl2::SL2_LS_NTSC_1953 ).dRgb[1],
						sl2::CFormat::Luma( sl2::SL2_LS_NTSC_1953 ).dRgb[2], 1.0 );
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"EBU_TECH_3213" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"EBUTECH3213" ) == 0 ) {
					sl2::CFormat::m_vDitherFactor = sl2::CVector4<SL2_ST_RAW>(
						sl2::CFormat::Luma( sl2::SL2_LS_EBU_TECH_3213 ).dRgb[0],
						sl2::CFormat::Luma( sl2::SL2_LS_EBU_TECH_3213 ).dRgb[1],
						sl2::CFormat::Luma( sl2::SL2_LS_EBU_TECH_3213 ).dRgb[2], 1.0 );
				}
				else {
					SL2_ERRORT( std::format( L"Invalid \"dither_error_weight_perceptual\": \"{}\". Must be REC_709, REC_2020, SMPTC, REC_601, CIE_1931, NTSC_1953, or EBU_TECH_3213.",
						_wcpArgV[1] ).c_str(), sl2::SL2_E_INVALIDCALL );
				}
				SL2_ADV( 2 );
			}
			if ( SL2_CHECK( 2, dither_error_weight_scale ) ) {
				double dScale = ::_wtof( _wcpArgV[1] );
				sl2::CFormat::m_vDitherFactor = sl2::CFormat::m_vDitherFactor * dScale;
				if ( sl2::CFormat::m_vDitherFactor.IsNan() ) {
					SL2_ERRORT( std::format( L"Invalid \"dither_error_weight_scale\": \"{}\". Invalid paramater.  Must be <float> <float> <float> <float>.",
						_wcpArgV[1] ).c_str(), sl2::SL2_E_INVALIDCALL );
				}
				SL2_ADV( 2 );
			}
            


            
			if ( SL2_CHECK( 1, png_default ) ) {
				oOptions.iPngSaveOption = (oOptions.iPngSaveOption & 0xFF00) | PNG_Z_DEFAULT_COMPRESSION;
				SL2_ADV( 1 );
			}
			if ( SL2_CHECK( 1, png_bestspeed ) ) {
				oOptions.iPngSaveOption = (oOptions.iPngSaveOption & 0xFF00) | PNG_Z_BEST_SPEED;
				SL2_ADV( 1 );
			}
			if ( SL2_CHECK( 1, png_bestcompression ) ) {
				oOptions.iPngSaveOption = (oOptions.iPngSaveOption & 0xFF00) | PNG_Z_BEST_COMPRESSION;
				SL2_ADV( 1 );
			}
			if ( SL2_CHECK( 2, png_level ) ) {
				oOptions.iPngSaveOption = (oOptions.iPngSaveOption & 0xFF00) | std::clamp( ::_wtoi( _wcpArgV[1] ), 0, 9 );
				SL2_ADV( 2 );
			}
			if ( SL2_CHECK( 1, png_nocompression ) ) {
				oOptions.iPngSaveOption |= PNG_Z_NO_COMPRESSION;
				SL2_ADV( 1 );
			}
			if ( SL2_CHECK( 1, png_interlaced ) ) {
				oOptions.iPngSaveOption |= PNG_INTERLACED;
				SL2_ADV( 1 );
			}
			if ( SL2_CHECK( 2, png_format ) ) {
				if ( ::_wcsicmp( _wcpArgV[1], L"RGB24" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"RGB" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"R8G8B8" ) == 0 ) {
					oOptions.pkifdPngFormat = sl2::CFormat::FindFormatDataByVulkan( sl2::SL2_VK_FORMAT_R8G8B8_UNORM );
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"RGB24_SRGB" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"RGB_SRGB" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"R8G8B8_SRGB" ) == 0 ) {
					oOptions.pkifdPngFormat = sl2::CFormat::FindFormatDataByVulkan( sl2::SL2_VK_FORMAT_R8G8B8_SRGB );
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"RGBA32" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"RGBA" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"R8G8B8A8" ) == 0 ) {
					oOptions.pkifdPngFormat = sl2::CFormat::FindFormatDataByVulkan( sl2::SL2_VK_FORMAT_R8G8B8A8_UNORM );
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"RGBA32_SRGB" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"RGBA_SRGB" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"R8G8B8A8_SRGB" ) == 0 ) {
					oOptions.pkifdPngFormat = sl2::CFormat::FindFormatDataByVulkan( sl2::SL2_VK_FORMAT_R8G8B8A8_SRGB );
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"RGB16" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"R16G16B16" ) == 0 ) {
					oOptions.pkifdPngFormat = sl2::CFormat::FindFormatDataByVulkan( sl2::SL2_VK_FORMAT_R16G16B16_UNORM );
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"RGBA16" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"R16G16B16A16" ) == 0 ) {
					oOptions.pkifdPngFormat = sl2::CFormat::FindFormatDataByVulkan( sl2::SL2_VK_FORMAT_R16G16B16A16_UNORM );
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"L8" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"GREYSCALE8" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"GRAYSCALE8" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"LUMINANCE8" ) == 0 ) {
					oOptions.pkifdPngFormat = sl2::CFormat::FindFormatDataByOgl( sl2::SL2_GL_LUMINANCE8 );
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"L16" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"GREYSCALE16" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"GRAYSCALE16" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"LUMINANCE16" ) == 0 ) {
					oOptions.pkifdPngFormat = sl2::CFormat::FindFormatDataByOgl( sl2::SL2_GL_LUMINANCE16 );
				}

				else if ( ::_wcsicmp( _wcpArgV[1], L"I1" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"INDEXED1" ) == 0 ) {
					oOptions.pkifdPngFormat = sl2::CFormat::FindFormatDataByOgl( sl2::SL2_GL_COLOR_INDEX1_EXT );
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"I2" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"INDEXED2" ) == 0 ) {
					oOptions.pkifdPngFormat = sl2::CFormat::FindFormatDataByOgl( sl2::SL2_GL_COLOR_INDEX2_EXT );
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"I4" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"INDEXED4" ) == 0 ) {
					oOptions.pkifdPngFormat = sl2::CFormat::FindFormatDataByOgl( sl2::SL2_GL_COLOR_INDEX4_EXT );
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"I8" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"INDEXED8" ) == 0 ) {
					oOptions.pkifdPngFormat = sl2::CFormat::FindFormatDataByOgl( sl2::SL2_GL_COLOR_INDEX8_EXT );
				}
				else {
					SL2_ERRORT( std::format( L"Invalid \"bmp_format\": \"{}\". Must be R8G8B8, R8G8B8_SRGB, R8G8B8A8, R8G8B8A8_SRGB, R16G16B16, R16G16B16A16, L8, or L16.",
						_wcpArgV[1] ).c_str(), sl2::SL2_E_INVALIDCALL );
				}
				SL2_ADV( 2 );
			}

			if ( SL2_CHECK( 1, bmp_rle ) ) {
				oOptions.iBmpSaveOption = BMP_SAVE_RLE;
				SL2_ADV( 1 );
			}
			if ( SL2_CHECK( 1, bmp_noalpha ) ) {
				oOptions.bBmpHasAlpha = false;
				SL2_ADV( 1 );
			}
			if ( SL2_CHECK( 1, bmp_nobitmask ) || SL2_CHECK( 1, bmp_nomask ) ) {
				oOptions.bBmpStoreBitmask = false;
				SL2_ADV( 1 );
			}
			if ( SL2_CHECK( 2, bmp_format ) ) {
				if ( ::_wcsicmp( _wcpArgV[1], L"RGB24" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"RGB" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"R8G8B8" ) == 0 ) {
					oOptions.vkBmpFormat = sl2::SL2_VK_FORMAT_R8G8B8_UNORM;
					oOptions.vkBmpFormatNoMask = oOptions.vkBmpFormat;
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"B8G8R8" ) == 0 ) {
					oOptions.vkBmpFormat = sl2::SL2_VK_FORMAT_B8G8R8_UNORM;
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"R8G8B8_SRGB" ) == 0 ) {
					oOptions.vkBmpFormat = sl2::SL2_VK_FORMAT_R8G8B8_SRGB;
					oOptions.vkBmpFormatNoMask = oOptions.vkBmpFormat;
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"RGBA32" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"RGBA" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"R8G8B8A8" ) == 0 ) {
					oOptions.vkBmpFormat = sl2::SL2_VK_FORMAT_R8G8B8A8_UNORM;
					oOptions.vkBmpFormatNoMask = oOptions.vkBmpFormat;
				}

				else if ( ::_wcsicmp( _wcpArgV[1], L"R8G8B8A8_SRGB" ) == 0 ) {
					oOptions.vkBmpFormat = sl2::SL2_VK_FORMAT_R8G8B8A8_SRGB;
					oOptions.vkBmpFormatNoMask = oOptions.vkBmpFormat;
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"B8G8R8A8" ) == 0 ) {
					oOptions.vkBmpFormat = sl2::SL2_VK_FORMAT_B8G8R8A8_UNORM;
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"B8G8R8A8_SRGB" ) == 0 ) {
					oOptions.vkBmpFormat = sl2::SL2_VK_FORMAT_B8G8R8A8_SRGB;
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"A8B8G8R8" ) == 0 ) {
					oOptions.vkBmpFormat = sl2::SL2_VK_FORMAT_A8B8G8R8_UNORM_PACK32;
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"A8B8G8R8_SRGB" ) == 0 ) {
					oOptions.vkBmpFormat = sl2::SL2_VK_FORMAT_A8B8G8R8_SRGB_PACK32;
				}

				else if ( ::_wcsicmp( _wcpArgV[1], L"R4G4B4A4" ) == 0 ) {
					oOptions.vkBmpFormat = sl2::SL2_VK_FORMAT_R4G4B4A4_UNORM_PACK16;
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"B4G4R4A4" ) == 0 ) {
					oOptions.vkBmpFormat = sl2::SL2_VK_FORMAT_B4G4R4A4_UNORM_PACK16;
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"A4R4G4B4" ) == 0 ) {
					oOptions.vkBmpFormat = sl2::SL2_VK_FORMAT_A4R4G4B4_UNORM_PACK16;
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"A4B4G4R4" ) == 0 ) {
					oOptions.vkBmpFormat = sl2::SL2_VK_FORMAT_A4B4G4R4_UNORM_PACK16;
				}

				else if ( ::_wcsicmp( _wcpArgV[1], L"R5G6B5" ) == 0 ) {
					oOptions.vkBmpFormat = sl2::SL2_VK_FORMAT_R5G6B5_UNORM_PACK16;
					oOptions.vkBmpFormatNoMask = oOptions.vkBmpFormat;
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"B5G6R5" ) == 0 ) {
					oOptions.vkBmpFormat = sl2::SL2_VK_FORMAT_B5G6R5_UNORM_PACK16;
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"R5G5B5A1" ) == 0 ) {
					oOptions.vkBmpFormat = sl2::SL2_VK_FORMAT_R5G5B5A1_UNORM_PACK16;
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"A1B5G5R5" ) == 0 ) {
					oOptions.vkBmpFormat = sl2::SL2_VK_FORMAT_A1B5G5R5_UNORM_PACK16_KHR;
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"A1R5G5B5" ) == 0 ) {
					oOptions.vkBmpFormat = sl2::SL2_VK_FORMAT_A1R5G5B5_UNORM_PACK16;
					oOptions.vkBmpFormatNoMask = oOptions.vkBmpFormat;
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"A4B4G4R4" ) == 0 ) {
					oOptions.vkBmpFormat = sl2::SL2_VK_FORMAT_A4B4G4R4_UNORM_PACK16;
				}
				else {
					SL2_ERRORT( std::format( L"Invalid \"bmp_format\": \"{}\". Must be R8G8B8, B8G8R8, R8G8B8_SRGB, R8G8B8A8, R8G8B8A8_SRGB, B8G8R8A8, B8G8R8A8_SRGB, A8B8G8R8, A8B8G8R8_SRGB, R4G4B4A4, B4G4R4A4, A4R4G4B4, A4B4G4R4, R5G6B5, B5G6R5, R5G5B5A1, A1B5G5R5, A1R5G5B5, or A4B4G4R4. "
						"Only R8G8B8, R8G8B8_SRGB, R8G8B8A8, R8G8B8A8_SRGB, R5G6B5, and A1R5G5B5 are available without a bitmask (nearest format will be selected).",
						_wcpArgV[1] ).c_str(), sl2::SL2_E_INVALIDCALL );
				}
				SL2_ADV( 2 );
			}

			if ( SL2_CHECK( 1, exr_float ) ) {
				oOptions.iExrSaveOption |= EXR_FLOAT;
				SL2_ADV( 1 );
			}
			if ( SL2_CHECK( 1, exr_none ) || SL2_CHECK( 1, exr_nocompression ) ) {
				oOptions.iExrSaveOption |= EXR_NONE;
				SL2_ADV( 1 );
			}
			if ( SL2_CHECK( 1, exr_zip ) ) {
				oOptions.iExrSaveOption |= EXR_ZIP;
				SL2_ADV( 1 );
			}
			if ( SL2_CHECK( 1, exr_piz ) ) {
				oOptions.iExrSaveOption |= EXR_PIZ;
				SL2_ADV( 1 );
			}
			if ( SL2_CHECK( 1, exr_pxr24 ) ) {
				oOptions.iExrSaveOption |= EXR_PXR24;
				SL2_ADV( 1 );
			}
			if ( SL2_CHECK( 1, exr_b44 ) ) {
				oOptions.iExrSaveOption |= EXR_B44;
				SL2_ADV( 1 );
			}
			if ( SL2_CHECK( 1, exr_lc ) ) {
				oOptions.iExrSaveOption |= EXR_LC;
				SL2_ADV( 1 );
			}

			if ( SL2_CHECK( 2, j2k_comp ) || SL2_CHECK( 2, j2k_compression ) ) {
				oOptions.iJ2kSaveOption = ::_wtoi( _wcpArgV[1] );
				if ( oOptions.iJ2kSaveOption < 1 || oOptions.iJ2kSaveOption > 512 ) {
					SL2_ERRORT( std::format( L"Invalid \"j2k_comp\": \"{}\". Must be between 1 and 512.",
						_wcpArgV[1] ).c_str(), sl2::SL2_E_INVALIDCALL );
				}
				SL2_ADV( 2 );
			}
			if ( SL2_CHECK( 2, j2k_format ) ) {
				if ( ::_wcsicmp( _wcpArgV[1], L"RGB24" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"RGB" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"R8G8B8" ) == 0 ) {
					oOptions.pkifdPngFormat = sl2::CFormat::FindFormatDataByVulkan( sl2::SL2_VK_FORMAT_R8G8B8_UNORM );
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"RGB24_SRGB" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"RGB_SRGB" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"R8G8B8_SRGB" ) == 0 ) {
					oOptions.pkifdPngFormat = sl2::CFormat::FindFormatDataByVulkan( sl2::SL2_VK_FORMAT_R8G8B8_SRGB );
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"RGBA32" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"RGBA" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"R8G8B8A8" ) == 0 ) {
					oOptions.pkifdPngFormat = sl2::CFormat::FindFormatDataByVulkan( sl2::SL2_VK_FORMAT_R8G8B8A8_UNORM );
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"RGBA32_SRGB" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"RGBA_SRGB" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"R8G8B8A8_SRGB" ) == 0 ) {
					oOptions.pkifdPngFormat = sl2::CFormat::FindFormatDataByVulkan( sl2::SL2_VK_FORMAT_R8G8B8A8_SRGB );
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"L16" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"GREYSCALE16" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"GRAYSCALE16" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"LUMINANCE16" ) == 0 ) {
					oOptions.pkifdPngFormat = sl2::CFormat::FindFormatDataByOgl( sl2::SL2_GL_LUMINANCE16 );
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"RGB16" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"R16G16B16" ) == 0 ) {
					oOptions.pkifdPngFormat = sl2::CFormat::FindFormatDataByVulkan( sl2::SL2_VK_FORMAT_R16G16B16_UNORM );
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"RGBA16" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"R16G16B16A16" ) == 0 ) {
					oOptions.pkifdPngFormat = sl2::CFormat::FindFormatDataByVulkan( sl2::SL2_VK_FORMAT_R16G16B16A16_UNORM );
				}
				else {
					SL2_ERRORT( std::format( L"Invalid \"j2k_format\": \"{}\". Must be R8G8B8, R8G8B8_SRGB, R8G8B8A8, R8G8B8A8_SRGB, R16G16B16, R16G16B16A16, or L16.",
						_wcpArgV[1] ).c_str(), sl2::SL2_E_INVALIDCALL );
				}
				SL2_ADV( 2 );
			}

			if ( SL2_CHECK( 2, jp2_comp ) || SL2_CHECK( 2, jp2_compression ) ) {
				oOptions.iJ2kSaveOption = ::_wtoi( _wcpArgV[1] );
				if ( oOptions.iJ2kSaveOption < 1 || oOptions.iJ2kSaveOption > 512 ) {
					SL2_ERRORT( std::format( L"Invalid \"jp2_comp\": \"{}\". Must be between 1 and 512.",
						_wcpArgV[1] ).c_str(), sl2::SL2_E_INVALIDCALL );
				}
				SL2_ADV( 2 );
			}
			if ( SL2_CHECK( 2, jp2_format ) ) {
				if ( ::_wcsicmp( _wcpArgV[1], L"RGB24" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"RGB" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"R8G8B8" ) == 0 ) {
					oOptions.pkifdPngFormat = sl2::CFormat::FindFormatDataByVulkan( sl2::SL2_VK_FORMAT_R8G8B8_UNORM );
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"RGB24_SRGB" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"RGB_SRGB" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"R8G8B8_SRGB" ) == 0 ) {
					oOptions.pkifdPngFormat = sl2::CFormat::FindFormatDataByVulkan( sl2::SL2_VK_FORMAT_R8G8B8_SRGB );
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"RGBA32" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"RGBA" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"R8G8B8A8" ) == 0 ) {
					oOptions.pkifdPngFormat = sl2::CFormat::FindFormatDataByVulkan( sl2::SL2_VK_FORMAT_R8G8B8A8_UNORM );
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"RGBA32_SRGB" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"RGBA_SRGB" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"R8G8B8A8_SRGB" ) == 0 ) {
					oOptions.pkifdPngFormat = sl2::CFormat::FindFormatDataByVulkan( sl2::SL2_VK_FORMAT_R8G8B8A8_SRGB );
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"L16" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"GREYSCALE16" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"GRAYSCALE16" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"LUMINANCE16" ) == 0 ) {
					oOptions.pkifdPngFormat = sl2::CFormat::FindFormatDataByOgl( sl2::SL2_GL_LUMINANCE16 );
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"RGB16" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"R16G16B16" ) == 0 ) {
					oOptions.pkifdPngFormat = sl2::CFormat::FindFormatDataByVulkan( sl2::SL2_VK_FORMAT_R16G16B16_UNORM );
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"RGBA16" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"R16G16B16A16" ) == 0 ) {
					oOptions.pkifdPngFormat = sl2::CFormat::FindFormatDataByVulkan( sl2::SL2_VK_FORMAT_R16G16B16A16_UNORM );
				}
				else {
					SL2_ERRORT( std::format( L"Invalid \"jp2_format\": \"{}\". Must be R8G8B8, R8G8B8_SRGB, R8G8B8A8, R8G8B8A8_SRGB, R16G16B16, R16G16B16A16, or L16.",
						_wcpArgV[1] ).c_str(), sl2::SL2_E_INVALIDCALL );
				}
				SL2_ADV( 2 );
			}

			if ( SL2_CHECK( 1, jpg_qualitysuperb ) ) {
				oOptions.iJpgSaveOption = (oOptions.iJpgSaveOption & 0x2F00) | JPEG_QUALITYSUPERB;
				SL2_ADV( 1 );
			}
			if ( SL2_CHECK( 1, jpg_qualitygood ) ) {
				oOptions.iJpgSaveOption = (oOptions.iJpgSaveOption & 0x2F00) | JPEG_QUALITYGOOD;
				SL2_ADV( 1 );
			}
			if ( SL2_CHECK( 1, jpg_qualitynormal ) ) {
				oOptions.iJpgSaveOption = (oOptions.iJpgSaveOption & 0x2F00) | JPEG_QUALITYNORMAL;
				SL2_ADV( 1 );
			}
			if ( SL2_CHECK( 1, jpg_qualityaverage ) ) {
				oOptions.iJpgSaveOption = (oOptions.iJpgSaveOption & 0x2F00) | JPEG_QUALITYAVERAGE;
				SL2_ADV( 1 );
			}
			if ( SL2_CHECK( 1, jpg_qualitybad ) ) {
				oOptions.iJpgSaveOption = (oOptions.iJpgSaveOption & 0x2F00) | JPEG_QUALITYBAD;
				SL2_ADV( 1 );
			}
			if ( SL2_CHECK( 2, jpg_quality ) ) {
				int iPrior = oOptions.iJpgSaveOption;
				oOptions.iJpgSaveOption = ::_wtoi( _wcpArgV[1] );
				if ( oOptions.iJpgSaveOption < 0 || oOptions.iJpgSaveOption > 100 ) {
					SL2_ERRORT( std::format( L"Invalid \"jpg_quality\": \"{}\". Must be between 0 and 100.",
						_wcpArgV[1] ).c_str(), sl2::SL2_E_INVALIDCALL );
				}
				oOptions.iJpgSaveOption = oOptions.iJpgSaveOption | (oOptions.iJpgSaveOption & 0x2000);
				SL2_ADV( 2 );
			}
			if ( SL2_CHECK( 1, jpg_progressive ) ) {
				oOptions.iJpgSaveOption = (oOptions.iJpgSaveOption & 0x2FFF) | JPEG_PROGRESSIVE;
				SL2_ADV( 1 );
			}
			if ( SL2_CHECK( 1, jpg_subsampling_411 ) ) {
				oOptions.iJpgSaveOption = JPEG_SUBSAMPLING_411;
				SL2_ADV( 1 );
			}
			if ( SL2_CHECK( 1, jpg_subsampling_420 ) ) {
				oOptions.iJpgSaveOption = JPEG_SUBSAMPLING_420;
				SL2_ADV( 1 );
			}
			if ( SL2_CHECK( 1, jpg_subsampling_422 ) ) {
				oOptions.iJpgSaveOption = JPEG_SUBSAMPLING_422;
				SL2_ADV( 1 );
			}
			if ( SL2_CHECK( 1, jpg_subsampling_444 ) ) {
				oOptions.iJpgSaveOption = JPEG_SUBSAMPLING_444;
				SL2_ADV( 1 );
			}
			if ( SL2_CHECK( 1, jpg_optimize ) ) {
				oOptions.iJpgSaveOption |= JPEG_OPTIMIZE;
				SL2_ADV( 1 );
			}
			if ( SL2_CHECK( 1, jpg_baseline ) ) {
				oOptions.iJpgSaveOption |= JPEG_BASELINE;
				SL2_ADV( 1 );
			}

			if ( SL2_CHECK( 1, tga_rle ) ) {
				oOptions.iTgaSaveOption = TARGA_SAVE_RLE;
				SL2_ADV( 1 );
			}
			if ( SL2_CHECK( 2, tga_format ) ) {
				if ( ::_wcsicmp( _wcpArgV[1], L"RGB24" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"RGB" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"R8G8B8" ) == 0 ) {
					oOptions.vkTgaFormat = sl2::SL2_VK_FORMAT_R8G8B8_UNORM;
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"RGB24_SRGB" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"RGB_SRGB" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"R8G8B8_SRGB" ) == 0 ) {
					oOptions.vkTgaFormat = sl2::SL2_VK_FORMAT_R8G8B8_SRGB;
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"RGBA32" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"RGBA" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"R8G8B8A8" ) == 0 ) {
					oOptions.vkTgaFormat = sl2::SL2_VK_FORMAT_R8G8B8A8_UNORM;
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"RGBA32_SRGB" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"RGBA_SRGB" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"R8G8B8A8_SRGB" ) == 0 ) {
					oOptions.vkTgaFormat = sl2::SL2_VK_FORMAT_R8G8B8A8_SRGB;
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"A1RGB5" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"A1R5G5B5" ) == 0 ) {
					oOptions.vkTgaFormat = sl2::SL2_VK_FORMAT_A1R5G5B5_UNORM_PACK16;
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"L8" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"GREYSCALE8" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"GRAYSCALE8" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"LUMINANCE8" ) == 0 ) {
					oOptions.vkTgaFormat = sl2::SL2_VK_FORMAT_R8_UNORM;
				}
				else {
					SL2_ERRORT( std::format( L"Invalid \"tga_format\": \"{}\". Must be R8G8B8, R8G8B8_SRGB, R8G8B8A8, R8G8B8A8_SRGB, A1R5G5B5, or L8.",
						_wcpArgV[1] ).c_str(), sl2::SL2_E_INVALIDCALL );
				}
				SL2_ADV( 2 );
			}

			if ( SL2_CHECK( 1, yuv_input_pc ) ) {
				sl2::CFormat::m_ycoYuvToRgb.bFullAlgorithm = true;
				sl2::CFormat::m_ycoYuvToRgb.dBlack = 0.0;
				sl2::CFormat::m_ycoYuvToRgb.dS = 1.0;
				sl2::CFormat::m_ycoYuvToRgb.dKr = sl2::CFormat::Luma( sl2::SL2_LS_REC_709 ).dRgb[0];
				sl2::CFormat::m_ycoYuvToRgb.dKb = sl2::CFormat::Luma( sl2::SL2_LS_REC_709 ).dRgb[2];
				SL2_ADV( 1 );
			}
			if ( SL2_CHECK( 1, yuv_pc ) ) {
				sl2::CFormat::m_ycoRgbToYuv.bFullAlgorithm = true;
				sl2::CFormat::m_ycoRgbToYuv.dBlack = 0.0;
				sl2::CFormat::m_ycoRgbToYuv.dS = 1.0;
				sl2::CFormat::m_ycoRgbToYuv.dKr = sl2::CFormat::Luma( sl2::SL2_LS_REC_709 ).dRgb[0];
				sl2::CFormat::m_ycoRgbToYuv.dKb = sl2::CFormat::Luma( sl2::SL2_LS_REC_709 ).dRgb[2];
				SL2_ADV( 1 );
			}
			if ( SL2_CHECK( 1, yuv_input_studio ) ) {
				sl2::CFormat::m_ycoYuvToRgb.bFullAlgorithm = true;
				sl2::CFormat::m_ycoYuvToRgb.dBlack = 16.0 / 255.0;
				sl2::CFormat::m_ycoYuvToRgb.dS = 219.0 / 255.0;
				sl2::CFormat::m_ycoYuvToRgb.dKr = sl2::CFormat::Luma( sl2::SL2_LS_REC_709 ).dRgb[0];
				sl2::CFormat::m_ycoYuvToRgb.dKb = sl2::CFormat::Luma( sl2::SL2_LS_REC_709 ).dRgb[2];
				SL2_ADV( 1 );
			}
			if ( SL2_CHECK( 1, yuv_studio ) ) {
				sl2::CFormat::m_ycoRgbToYuv.bFullAlgorithm = true;
				sl2::CFormat::m_ycoRgbToYuv.dBlack = 16.0 / 255.0;
				sl2::CFormat::m_ycoRgbToYuv.dS = 219.0 / 255.0;
				sl2::CFormat::m_ycoRgbToYuv.dKr = sl2::CFormat::Luma( sl2::SL2_LS_REC_709 ).dRgb[0];
				sl2::CFormat::m_ycoRgbToYuv.dKb = sl2::CFormat::Luma( sl2::SL2_LS_REC_709 ).dRgb[2];
				SL2_ADV( 1 );
			}
			if ( SL2_CHECK( 2, yuv_input_set_s ) || SL2_CHECK( 2, yuv_input_set_scale ) ) {
				sl2::CFormat::m_ycoYuvToRgb.dS = ::_wtof( _wcpArgV[1] );
				if ( sl2::CFormat::m_ycoYuvToRgb.dS < 0.0 || sl2::CFormat::m_ycoYuvToRgb.dS > 1.0 ) {
					SL2_ERRORT( std::format( L"Invalid \"{}\": \"{}\". Parameters must be between 0 and 1.",
						_wcpArgV[0], _wcpArgV[1] ).c_str(), sl2::SL2_E_INVALIDCALL );
				}
				SL2_ADV( 2 );
			}
			if ( SL2_CHECK( 2, yuv_set_s ) || SL2_CHECK( 2, yuv_set_scale ) ) {
				sl2::CFormat::m_ycoRgbToYuv.dS = ::_wtof( _wcpArgV[1] );
				if ( sl2::CFormat::m_ycoRgbToYuv.dS < 0.0 || sl2::CFormat::m_ycoRgbToYuv.dS > 1.0 ) {
					SL2_ERRORT( std::format( L"Invalid \"{}\": \"{}\". Parameters must be between 0 and 1.",
						_wcpArgV[0], _wcpArgV[1] ).c_str(), sl2::SL2_E_INVALIDCALL );
				}
				SL2_ADV( 2 );
			}
			if ( SL2_CHECK( 2, yuv_input_set_z ) || SL2_CHECK( 2, yuv_input_set_black ) ) {
				sl2::CFormat::m_ycoYuvToRgb.dBlack = ::_wtof( _wcpArgV[1] );
				if ( sl2::CFormat::m_ycoYuvToRgb.dBlack < 0.0 || sl2::CFormat::m_ycoYuvToRgb.dBlack > 1.0 ) {
					SL2_ERRORT( std::format( L"Invalid \"{}\": \"{}\". Parameters must be between 0 and 1.",
						_wcpArgV[0], _wcpArgV[1] ).c_str(), sl2::SL2_E_INVALIDCALL );
				}
				SL2_ADV( 2 );
			}
			if ( SL2_CHECK( 2, yuv_set_z ) || SL2_CHECK( 2, yuv_set_black ) ) {
				sl2::CFormat::m_ycoRgbToYuv.dBlack = ::_wtof( _wcpArgV[1] );
				if ( sl2::CFormat::m_ycoRgbToYuv.dBlack < 0.0 || sl2::CFormat::m_ycoRgbToYuv.dBlack > 1.0 ) {
					SL2_ERRORT( std::format( L"Invalid \"{}\": \"{}\". Parameters must be between 0 and 1.",
						_wcpArgV[0], _wcpArgV[1] ).c_str(), sl2::SL2_E_INVALIDCALL );
				}
				SL2_ADV( 2 );
			}
			if ( SL2_CHECK( 2, yuv_input_kr_kb ) ) {
				if ( ::_wcsicmp( _wcpArgV[1], L"REC_709" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"REC709" ) == 0 ) {
					sl2::CFormat::m_ycoYuvToRgb.dKr = sl2::CFormat::Luma( sl2::SL2_LS_REC_709 ).dRgb[0];
					sl2::CFormat::m_ycoYuvToRgb.dKb = sl2::CFormat::Luma( sl2::SL2_LS_REC_709 ).dRgb[2];
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"REC_2020" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"REC2020" ) == 0 ) {
					sl2::CFormat::m_ycoYuvToRgb.dKr = sl2::CFormat::Luma( sl2::SL2_LS_REC_2020 ).dRgb[0];
					sl2::CFormat::m_ycoYuvToRgb.dKb = sl2::CFormat::Luma( sl2::SL2_LS_REC_2020 ).dRgb[2];
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"SMPTC" ) == 0 ) {
					sl2::CFormat::m_ycoYuvToRgb.dKr = sl2::CFormat::Luma( sl2::SL2_LS_SMPTC ).dRgb[0];
					sl2::CFormat::m_ycoYuvToRgb.dKb = sl2::CFormat::Luma( sl2::SL2_LS_SMPTC ).dRgb[2];
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"REC_601" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"REC601" ) == 0 ) {
					sl2::CFormat::m_ycoYuvToRgb.dKr = sl2::CFormat::Luma( sl2::SL2_LS_REC_601 ).dRgb[0];
					sl2::CFormat::m_ycoYuvToRgb.dKb = sl2::CFormat::Luma( sl2::SL2_LS_REC_601 ).dRgb[2];
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"CIE_1931" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"CIE1931" ) == 0 ) {
					sl2::CFormat::m_ycoYuvToRgb.dKr = sl2::CFormat::Luma( sl2::SL2_LS_CIE_1931 ).dRgb[0];
					sl2::CFormat::m_ycoYuvToRgb.dKb = sl2::CFormat::Luma( sl2::SL2_LS_CIE_1931 ).dRgb[2];
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"NTSC_1953" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"NTSC1953" ) == 0 ) {
					sl2::CFormat::m_ycoYuvToRgb.dKr = sl2::CFormat::Luma( sl2::SL2_LS_NTSC_1953 ).dRgb[0];
					sl2::CFormat::m_ycoYuvToRgb.dKb = sl2::CFormat::Luma( sl2::SL2_LS_NTSC_1953 ).dRgb[2];
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"EBU_TECH_3213" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"EBUTECH3213" ) == 0 ) {
					sl2::CFormat::m_ycoYuvToRgb.dKr = sl2::CFormat::Luma( sl2::SL2_LS_EBU_TECH_3213 ).dRgb[0];
					sl2::CFormat::m_ycoYuvToRgb.dKb = sl2::CFormat::Luma( sl2::SL2_LS_EBU_TECH_3213 ).dRgb[2];
				}
				else {
					SL2_ERRORT( std::format( L"Invalid \"yuv_input_kr_kb\": \"{}\". Must be REC_709, REC_2020, SMPTC, REC_601, CIE_1931, NTSC_1953, or EBU_TECH_3213.",
						_wcpArgV[1] ).c_str(), sl2::SL2_E_INVALIDCALL );
				}
				SL2_ADV( 2 );
			}
			if ( SL2_CHECK( 2, yuv_kr_kb ) ) {
				if ( ::_wcsicmp( _wcpArgV[1], L"REC_709" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"REC709" ) == 0 ) {
					sl2::CFormat::m_ycoRgbToYuv.dKr = sl2::CFormat::Luma( sl2::SL2_LS_REC_709 ).dRgb[0];
					sl2::CFormat::m_ycoRgbToYuv.dKb = sl2::CFormat::Luma( sl2::SL2_LS_REC_709 ).dRgb[2];
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"REC_2020" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"REC2020" ) == 0 ) {
					sl2::CFormat::m_ycoRgbToYuv.dKr = sl2::CFormat::Luma( sl2::SL2_LS_REC_2020 ).dRgb[0];
					sl2::CFormat::m_ycoRgbToYuv.dKb = sl2::CFormat::Luma( sl2::SL2_LS_REC_2020 ).dRgb[2];
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"SMPTC" ) == 0 ) {
					sl2::CFormat::m_ycoRgbToYuv.dKr = sl2::CFormat::Luma( sl2::SL2_LS_SMPTC ).dRgb[0];
					sl2::CFormat::m_ycoRgbToYuv.dKb = sl2::CFormat::Luma( sl2::SL2_LS_SMPTC ).dRgb[2];
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"REC_601" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"REC601" ) == 0 ) {
					sl2::CFormat::m_ycoRgbToYuv.dKr = sl2::CFormat::Luma( sl2::SL2_LS_REC_601 ).dRgb[0];
					sl2::CFormat::m_ycoRgbToYuv.dKb = sl2::CFormat::Luma( sl2::SL2_LS_REC_601 ).dRgb[2];
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"CIE_1931" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"CIE1931" ) == 0 ) {
					sl2::CFormat::m_ycoRgbToYuv.dKr = sl2::CFormat::Luma( sl2::SL2_LS_CIE_1931 ).dRgb[0];
					sl2::CFormat::m_ycoRgbToYuv.dKb = sl2::CFormat::Luma( sl2::SL2_LS_CIE_1931 ).dRgb[2];
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"NTSC_1953" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"NTSC1953" ) == 0 ) {
					sl2::CFormat::m_ycoRgbToYuv.dKr = sl2::CFormat::Luma( sl2::SL2_LS_NTSC_1953 ).dRgb[0];
					sl2::CFormat::m_ycoRgbToYuv.dKb = sl2::CFormat::Luma( sl2::SL2_LS_NTSC_1953 ).dRgb[2];
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"EBU_TECH_3213" ) == 0 || ::_wcsicmp( _wcpArgV[1], L"EBUTECH3213" ) == 0 ) {
					sl2::CFormat::m_ycoRgbToYuv.dKr = sl2::CFormat::Luma( sl2::SL2_LS_EBU_TECH_3213 ).dRgb[0];
					sl2::CFormat::m_ycoRgbToYuv.dKb = sl2::CFormat::Luma( sl2::SL2_LS_EBU_TECH_3213 ).dRgb[2];
				}
				else {
					SL2_ERRORT( std::format( L"Invalid \"yuv_kr_kb\": \"{}\". Must be REC_709, REC_2020, SMPTC, REC_601, CIE_1931, NTSC_1953, or EBU_TECH_3213.",
						_wcpArgV[1] ).c_str(), sl2::SL2_E_INVALIDCALL );
				}
				SL2_ADV( 2 );
			}
			if ( SL2_CHECK( 3, yuv_input_set_kr_kb ) ) {
				sl2::CFormat::m_ycoYuvToRgb.dKr = ::_wtof( _wcpArgV[1] );
				sl2::CFormat::m_ycoYuvToRgb.dKr = ::_wtof( _wcpArgV[2] );
				if ( sl2::CFormat::m_ycoYuvToRgb.dKr < 0.0 || sl2::CFormat::m_ycoYuvToRgb.dKr > 1.0 ||
					sl2::CFormat::m_ycoYuvToRgb.dKr < 0.0 || sl2::CFormat::m_ycoYuvToRgb.dKr > 1.0 ) {
					SL2_ERRORT( std::format( L"Invalid \"yuv_input_set_kr_kb\": \"{}\" \"{}\". Parameters must be between 0 and 1.",
						_wcpArgV[1], _wcpArgV[2] ).c_str(), sl2::SL2_E_INVALIDCALL );
				}
				SL2_ADV( 3 );
			}
			if ( SL2_CHECK( 3, yuv_set_kr_kb ) ) {
				sl2::CFormat::m_ycoRgbToYuv.dKr = ::_wtof( _wcpArgV[1] );
				sl2::CFormat::m_ycoRgbToYuv.dKr = ::_wtof( _wcpArgV[2] );
				if ( sl2::CFormat::m_ycoRgbToYuv.dKr < 0.0 || sl2::CFormat::m_ycoRgbToYuv.dKr > 1.0 ||
					sl2::CFormat::m_ycoRgbToYuv.dKr < 0.0 || sl2::CFormat::m_ycoRgbToYuv.dKr > 1.0 ) {
					SL2_ERRORT( std::format( L"Invalid \"yuv_set_kr_kb\": \"{}\" \"{}\". Parameters must be between 0 and 1.",
						_wcpArgV[1], _wcpArgV[2] ).c_str(), sl2::SL2_E_INVALIDCALL );
				}
				SL2_ADV( 3 );
			}
			if ( SL2_CHECK( 1, yuv_input_use_approx ) ) {
				sl2::CFormat::m_ycoYuvToRgb.bFullAlgorithm = false;
				SL2_ADV( 1 );
			}
			if ( SL2_CHECK( 1, yuv_use_approx ) ) {
				sl2::CFormat::m_ycoRgbToYuv.bFullAlgorithm = false;
				SL2_ADV( 1 );
			}
			if ( SL2_CHECK( 2, yuv_input_format ) ) {
				if ( oOptions.vInputs.size() == 0 ) {
					SL2_ERRORT( std::format( L"Invalid \"yuv_input_format\": \"{}\". No input file provided yet. -yuv_file must come before yuv_input.",
						_wcpArgV[1] ).c_str(), sl2::SL2_E_INVALIDCALL );
				}
				size_t sIdx = oOptions.vInputs.size() - 1;
				std::string sString = sl2::CUtilities::Utf16ToUtf8( reinterpret_cast<const char16_t *>((_wcpArgV[1])) );
				oOptions.vInputs[sIdx].pkifduvFormat = sl2::CFormat::FindFormatDataByVulkan( sString.c_str() );
				if ( oOptions.vInputs[sIdx].pkifduvFormat && !SL2_GET_YUV_FLAG( oOptions.vInputs[sIdx].pkifduvFormat->ui32Flags ) ) {
					oOptions.vInputs[sIdx].pkifduvFormat = nullptr;
				}
				if ( !oOptions.vInputs[sIdx].pkifduvFormat ) {
					oOptions.vInputs[sIdx].pkifduvFormat = sl2::CFormat::FindFormatDataByDx( sString.c_str() );
					if ( oOptions.vInputs[sIdx].pkifduvFormat && !SL2_GET_YUV_FLAG( oOptions.vInputs[sIdx].pkifduvFormat->ui32Flags ) ) {
						oOptions.vInputs[sIdx].pkifduvFormat = nullptr;
					}
				}
				if ( !oOptions.vInputs[sIdx].pkifduvFormat ) {
					oOptions.vInputs[sIdx].pkifduvFormat = sl2::CFormat::FindFormatDataByMetal( sString.c_str() );
					if ( oOptions.vInputs[sIdx].pkifduvFormat && !SL2_GET_YUV_FLAG( oOptions.vInputs[sIdx].pkifduvFormat->ui32Flags ) ) {
						oOptions.vInputs[sIdx].pkifduvFormat = nullptr;
					}
				}
				if ( oOptions.vInputs[sIdx].pkifduvFormat && SL2_GET_YUV_FLAG( oOptions.vInputs[sIdx].pkifduvFormat->ui32Flags ) ) {
					SL2_ADV( 2 );
				}

				if ( ::_wcsicmp( _wcpArgV[1], L"nv12" ) == 0 ) {
					oOptions.vInputs[sIdx].pkifduvFormat = sl2::CFormat::FindFormatDataByDx( sl2::SL2_DXGI_FORMAT_NV12 );
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"nv21" ) == 0 ) {
					oOptions.vInputs[sIdx].pkifduvFormat = sl2::CFormat::FindFormatDataByDx( sl2::SL2_DXGI_FORMAT_NV21 );
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"yv12" ) == 0 ) {
					oOptions.vInputs[sIdx].pkifduvFormat = sl2::CFormat::FindFormatDataByDx( sl2::SL2_DXGI_FORMAT_YV12 );
				}
				/*else if ( ::_wcsicmp( _wcpArgV[1], L"yv21" ) == 0 ) {
					oOptions.vInputs[sIdx].pkifduvFormat = sl2::CFormat::FindFormatDataByDx( sl2::SL2_DXGI_FORMAT_YV21 );
				}*/
				else if ( ::_wcsicmp( _wcpArgV[1], L"yuy2" ) == 0 ) {
					oOptions.vInputs[sIdx].pkifduvFormat = sl2::CFormat::FindFormatDataByDx( sl2::SL2_DXGI_FORMAT_YUY2 );
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"uyvy" ) == 0 ) {
					oOptions.vInputs[sIdx].pkifduvFormat = sl2::CFormat::FindFormatDataByDx( sl2::SL2_DXGI_FORMAT_R8G8_B8G8_UNORM );
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"p010" ) == 0 ) {
					oOptions.vInputs[sIdx].pkifduvFormat = sl2::CFormat::FindFormatDataByDx( sl2::SL2_DXGI_FORMAT_P010 );
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"p016" ) == 0 ) {
					oOptions.vInputs[sIdx].pkifduvFormat = sl2::CFormat::FindFormatDataByDx( sl2::SL2_DXGI_FORMAT_P016 );
				}
				/*else if ( ::_wcsicmp( _wcpArgV[1], L"y010" ) == 0 ) {
					oOptions.vInputs[sIdx].pkifduvFormat = sl2::CFormat::FindFormatDataByVulkan( sl2::SL2_VK_FORMAT_G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16 );
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"y016" ) == 0 ) {
					oOptions.vInputs[sIdx].pkifduvFormat = sl2::CFormat::FindFormatDataByDx( sl2::SL2_DXGI_FORMAT_Y016 );
				}*/
				else if ( ::_wcsicmp( _wcpArgV[1], L"p210" ) == 0 ) {
					oOptions.vInputs[sIdx].pkifduvFormat = sl2::CFormat::FindFormatDataByDx( sl2::SL2_DXGI_FORMAT_P210 );
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"p216" ) == 0 ) {
					oOptions.vInputs[sIdx].pkifduvFormat = sl2::CFormat::FindFormatDataByDx( sl2::SL2_DXGI_FORMAT_P216 );
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"y210" ) == 0 ) {
					oOptions.vInputs[sIdx].pkifduvFormat = sl2::CFormat::FindFormatDataByDx( sl2::SL2_DXGI_FORMAT_Y210 );
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"y216" ) == 0 ) {
					oOptions.vInputs[sIdx].pkifduvFormat = sl2::CFormat::FindFormatDataByDx( sl2::SL2_DXGI_FORMAT_Y216 );
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"y416" ) == 0 ) {
					oOptions.vInputs[sIdx].pkifduvFormat = sl2::CFormat::FindFormatDataByDx( sl2::SL2_DXGI_FORMAT_Y416 );
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"y410" ) == 0 ) {
					oOptions.vInputs[sIdx].pkifduvFormat = sl2::CFormat::FindFormatDataByDx( sl2::SL2_DXGI_FORMAT_Y410 );
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"ayuv" ) == 0 ) {
					oOptions.vInputs[sIdx].pkifduvFormat = sl2::CFormat::FindFormatDataByDx( sl2::SL2_DXGI_FORMAT_AYUV );
				}
				else {
					SL2_ERRORT( std::format( L"Invalid \"yuv_input_format\": \"{}\". Must be one of the Vulkan/DXGI YUV formats or nv12, nv21, yv12, yuy2, uyvy, p010, p016, p210, p216, y210, y216, y416, y410, or ayuv.",
						_wcpArgV[1] ).c_str(), sl2::SL2_E_INVALIDCALL );
				}
				SL2_ADV( 2 );
			}
			if ( SL2_CHECK( 2, yuv_format ) ) {
				std::string sString = sl2::CUtilities::Utf16ToUtf8( reinterpret_cast<const char16_t *>((_wcpArgV[1])) );
				oOptions.pkifdYuvFormat = sl2::CFormat::FindFormatDataByVulkan( sString.c_str() );
				if ( oOptions.pkifdYuvFormat && !SL2_GET_YUV_FLAG( oOptions.pkifdYuvFormat->ui32Flags ) ) {
					oOptions.pkifdYuvFormat = nullptr;
				}
				if ( !oOptions.pkifdYuvFormat ) {
					oOptions.pkifdYuvFormat = sl2::CFormat::FindFormatDataByDx( sString.c_str() );
					if ( oOptions.pkifdYuvFormat && !SL2_GET_YUV_FLAG( oOptions.pkifdYuvFormat->ui32Flags ) ) {
						oOptions.pkifdYuvFormat = nullptr;
					}
				}
				if ( !oOptions.pkifdYuvFormat ) {
					oOptions.pkifdYuvFormat = sl2::CFormat::FindFormatDataByMetal( sString.c_str() );
					if ( oOptions.pkifdYuvFormat && !SL2_GET_YUV_FLAG( oOptions.pkifdYuvFormat->ui32Flags ) ) {
						oOptions.pkifdYuvFormat = nullptr;
					}
				}
				if ( oOptions.pkifdYuvFormat && SL2_GET_YUV_FLAG( oOptions.pkifdYuvFormat->ui32Flags ) ) {
					SL2_ADV( 2 );
				}

				if ( ::_wcsicmp( _wcpArgV[1], L"nv12" ) == 0 ) {
					oOptions.pkifdYuvFormat = sl2::CFormat::FindFormatDataByDx( sl2::SL2_DXGI_FORMAT_NV12 );
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"nv21" ) == 0 ) {
					oOptions.pkifdYuvFormat = sl2::CFormat::FindFormatDataByDx( sl2::SL2_DXGI_FORMAT_NV21 );
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"yv12" ) == 0 ) {
					oOptions.pkifdYuvFormat = sl2::CFormat::FindFormatDataByDx( sl2::SL2_DXGI_FORMAT_YV12 );
				}
				/*else if ( ::_wcsicmp( _wcpArgV[1], L"yv21" ) == 0 ) {
					oOptions.pkifdYuvFormat = sl2::CFormat::FindFormatDataByDx( sl2::SL2_DXGI_FORMAT_YV21 );
				}*/
				else if ( ::_wcsicmp( _wcpArgV[1], L"yuy2" ) == 0 ) {
					oOptions.pkifdYuvFormat = sl2::CFormat::FindFormatDataByDx( sl2::SL2_DXGI_FORMAT_YUY2 );
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"uyvy" ) == 0 ) {
					oOptions.pkifdYuvFormat = sl2::CFormat::FindFormatDataByDx( sl2::SL2_DXGI_FORMAT_R8G8_B8G8_UNORM );
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"p010" ) == 0 ) {
					oOptions.pkifdYuvFormat = sl2::CFormat::FindFormatDataByDx( sl2::SL2_DXGI_FORMAT_P010 );
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"p016" ) == 0 ) {
					oOptions.pkifdYuvFormat = sl2::CFormat::FindFormatDataByDx( sl2::SL2_DXGI_FORMAT_P016 );
				}
				/*else if ( ::_wcsicmp( _wcpArgV[1], L"y010" ) == 0 ) {
					oOptions.pkifdYuvFormat = sl2::CFormat::FindFormatDataByVulkan( sl2::SL2_VK_FORMAT_G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16 );
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"y016" ) == 0 ) {
					oOptions.pkifdYuvFormat = sl2::CFormat::FindFormatDataByDx( sl2::SL2_DXGI_FORMAT_Y016 );
				}*/
				else if ( ::_wcsicmp( _wcpArgV[1], L"p210" ) == 0 ) {
					oOptions.pkifdYuvFormat = sl2::CFormat::FindFormatDataByDx( sl2::SL2_DXGI_FORMAT_P210 );
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"p216" ) == 0 ) {
					oOptions.pkifdYuvFormat = sl2::CFormat::FindFormatDataByDx( sl2::SL2_DXGI_FORMAT_P216 );
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"y210" ) == 0 ) {
					oOptions.pkifdYuvFormat = sl2::CFormat::FindFormatDataByDx( sl2::SL2_DXGI_FORMAT_Y210 );
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"y216" ) == 0 ) {
					oOptions.pkifdYuvFormat = sl2::CFormat::FindFormatDataByDx( sl2::SL2_DXGI_FORMAT_Y216 );
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"y416" ) == 0 ) {
					oOptions.pkifdYuvFormat = sl2::CFormat::FindFormatDataByDx( sl2::SL2_DXGI_FORMAT_Y416 );
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"y410" ) == 0 ) {
					oOptions.pkifdYuvFormat = sl2::CFormat::FindFormatDataByDx( sl2::SL2_DXGI_FORMAT_Y410 );
				}
				else if ( ::_wcsicmp( _wcpArgV[1], L"ayuv" ) == 0 ) {
					oOptions.pkifdYuvFormat = sl2::CFormat::FindFormatDataByDx( sl2::SL2_DXGI_FORMAT_AYUV );
				}
				else {
					SL2_ERRORT( std::format( L"Invalid \"yuv_format\": \"{}\". Must be one of the Vulkan/DXGI YUV formats or nv12, nv21, yv12, yuy2, uyvy, p010, p016, p210, p216, y210, y216, y416, y410, or ayuv.",
						_wcpArgV[1] ).c_str(), sl2::SL2_E_INVALIDCALL );
				}
				SL2_ADV( 2 );
			}

            // ============
            // == LEGACY ==
            // ============
            {
#define SL2_RESAMPLE( CONDITION, FILTER )                                                                                   \
    if ( CONDITION ) {                                                                                                      \
        oOptions.fFilterFuncW = sl2::CResampler::FILTER;                                                                    \
        oOptions.fFilterFuncH = oOptions.fFilterFuncW;                                                                      \
        oOptions.fFilterFuncD = oOptions.fFilterFuncW;                                                                      \
                                                                                                                            \
        oOptions.fAlphaFilterFuncW = oOptions.fFilterFuncW;                                                                 \
        oOptions.fAlphaFilterFuncH = oOptions.fFilterFuncH;                                                                 \
        oOptions.fAlphaFilterFuncD = oOptions.fFilterFuncD;                                                                 \
        SL2_ADV( 1 );                                                                                                       \
    }
                SL2_RESAMPLE( SL2_CHECK( 1, RescaleBox ) || SL2_CHECK( 1, ResampleBox ) || SL2_CHECK( 1, ResamplePoint ), SL2_FF_POINT )
                SL2_RESAMPLE( SL2_CHECK( 1, RescaleTent ) || SL2_CHECK( 1, ResampleTent ) || SL2_CHECK( 1, ResampleBilinear ), SL2_FF_LINEAR )
                SL2_RESAMPLE( SL2_CHECK( 1, RescaleQuadraticSharp ) || SL2_CHECK( 1, ResampleQuadraticSharp ), SL2_FF_QUADRATICSHARP )
                SL2_RESAMPLE( SL2_CHECK( 1, RescaleQuadratic ) || SL2_CHECK( 1, ResampleQuadratic ) || SL2_CHECK( 1, ResampleQuad ), SL2_FF_QUADRATIC )
                SL2_RESAMPLE( SL2_CHECK( 1, RescaleQuadraticApprox ) || SL2_CHECK( 1, ResampleQuadraticApprox ), SL2_FF_QUADRATICAPPROX )
                SL2_RESAMPLE( SL2_CHECK( 1, RescaleQuadraticMix ) || SL2_CHECK( 1, ResampleQuadraticMix ), SL2_FF_QUADRATICMIX )
                SL2_RESAMPLE( SL2_CHECK( 1, RescaleKaiser ) || SL2_CHECK( 1, ResampleKaiser ), SL2_FF_KAISER )
                SL2_RESAMPLE( SL2_CHECK( 1, RescaleLanczos2 ) || SL2_CHECK( 1, ResampleLanczos2 ), SL2_FF_LANCZOS2 )
                SL2_RESAMPLE( SL2_CHECK( 1, RescaleLanczos3 ) || SL2_CHECK( 1, ResampleLanczos3 ), SL2_FF_LANCZOS3 )
                SL2_RESAMPLE( SL2_CHECK( 1, RescaleLanczos4 ) || SL2_CHECK( 1, ResampleLanczos4 ), SL2_FF_LANCZOS4 )
                SL2_RESAMPLE( SL2_CHECK( 1, RescaleLanczos6 ) || SL2_CHECK( 1, ResampleLanczos6 ), SL2_FF_LANCZOS6 )
                SL2_RESAMPLE( SL2_CHECK( 1, RescaleLanczos8 ) || SL2_CHECK( 1, ResampleLanczos8 ), SL2_FF_LANCZOS8 )
                SL2_RESAMPLE( SL2_CHECK( 1, RescaleLanczos12 ) || SL2_CHECK( 1, ResampleLanczos12 ), SL2_FF_LANCZOS12 )
                SL2_RESAMPLE( SL2_CHECK( 1, RescaleLanczos64 ) || SL2_CHECK( 1, ResampleLanczos64 ), SL2_FF_LANCZOS64 )
                SL2_RESAMPLE( SL2_CHECK( 1, RescaleMitchell ) || SL2_CHECK( 1, ResampleMitchell ), SL2_FF_MITCHELL )
                SL2_RESAMPLE( SL2_CHECK( 1, RescaleCatrom ) || SL2_CHECK( 1, ResampleCatrom ), SL2_FF_CATMULLROM )
                SL2_RESAMPLE( SL2_CHECK( 1, RescaleBSpline ) || SL2_CHECK( 1, ResampleBSpline ), SL2_FF_BSPLINE )
                SL2_RESAMPLE( SL2_CHECK( 1, ResampleCardinalUniform ) || SL2_CHECK( 1, ResampleCardinal ), SL2_FF_CARDINALSPLINEUNIFORM )
                SL2_RESAMPLE( SL2_CHECK( 1, ResampleHermite ), SL2_FF_HERMITE )
                SL2_RESAMPLE( SL2_CHECK( 1, ResampleHamming ), SL2_FF_HAMMING )
                SL2_RESAMPLE( SL2_CHECK( 1, ResampleHanning ), SL2_FF_HANNING )
                SL2_RESAMPLE( SL2_CHECK( 1, RescaleBlackman ) || SL2_CHECK( 1, ResampleBlackman ), SL2_FF_BLACKMAN )
                SL2_RESAMPLE( SL2_CHECK( 1, RescaleGaussianSharp ) || SL2_CHECK( 1, ResampleGaussianSharp ), SL2_FF_GAUSSIANSHARP )
                SL2_RESAMPLE( SL2_CHECK( 1, RescaleGaussian ) || SL2_CHECK( 1, ResampleGaussian ), SL2_FF_GAUSSIAN )
                SL2_RESAMPLE( SL2_CHECK( 1, RescaleBell ) || SL2_CHECK( 1, ResampleBell ), SL2_FF_BELL )

#undef SL2_RESAMPLE

                if ( SL2_CHECK( 1, dxt1c ) || SL2_CHECK( 1, bc1 ) ) {
                    oOptions.pkifdFinalFormat = sl2::CFormat::FindFormatDataByOgl( sl2::SL2_GL_COMPRESSED_RGB_S3TC_DXT1_EXT );
                    SL2_ADV( 1 );
                }
                if ( SL2_CHECK( 1, dxt1a ) || SL2_CHECK( 1, bc1a ) ) {
                    oOptions.pkifdFinalFormat = sl2::CFormat::FindFormatDataByOgl( sl2::SL2_GL_COMPRESSED_RGBA_S3TC_DXT1_EXT );
                    SL2_ADV( 1 );
                }
                if ( SL2_CHECK( 1, dxt2 ) ) {
                    oOptions.pkifdFinalFormat = sl2::CFormat::FindFormatDataByOgl( sl2::SL2_GL_COMPRESSED_RGBA_S3TC_DXT3_EXT );
                    oOptions.bNeedsPreMultiply = true;
                    SL2_ADV( 1 );
                }
                if ( SL2_CHECK( 1, dxt3 ) || SL2_CHECK( 1, bc2 ) ) {
                    oOptions.pkifdFinalFormat = sl2::CFormat::FindFormatDataByOgl( sl2::SL2_GL_COMPRESSED_RGBA_S3TC_DXT3_EXT );
                    SL2_ADV( 1 );
                }
                if ( SL2_CHECK( 1, dxt4 ) ) {
                    oOptions.pkifdFinalFormat = sl2::CFormat::FindFormatDataByOgl( sl2::SL2_GL_COMPRESSED_RGBA_S3TC_DXT5_EXT );
                    oOptions.bNeedsPreMultiply = true;
                    SL2_ADV( 1 );
                }
                if ( SL2_CHECK( 1, dxt5 ) || SL2_CHECK( 1, bc3 ) ) {
                    oOptions.pkifdFinalFormat = sl2::CFormat::FindFormatDataByOgl( sl2::SL2_GL_COMPRESSED_RGBA_S3TC_DXT5_EXT );
                    SL2_ADV( 1 );
                }
                if ( SL2_CHECK( 1, bc4 ) ) {
                    oOptions.pkifdFinalFormat = sl2::CFormat::FindFormatDataByDx( sl2::SL2_DXGI_FORMAT_BC4_UNORM );
                    SL2_ADV( 1 );
                }
                if ( SL2_CHECK( 1, bc5 ) ) {
                    oOptions.pkifdFinalFormat = sl2::CFormat::FindFormatDataByDx( sl2::SL2_DXGI_FORMAT_BC5_UNORM );
                    SL2_ADV( 1 );
                }
                if ( SL2_CHECK( 1, bc6 ) || SL2_CHECK( 1, bc6h ) ) {
                    oOptions.pkifdFinalFormat = sl2::CFormat::FindFormatDataByDx( sl2::SL2_DXGI_FORMAT_BC6H_UF16 );
                    SL2_ADV( 1 );
                }
                if ( SL2_CHECK( 1, bc7 ) ) {
                    oOptions.pkifdFinalFormat = sl2::CFormat::FindFormatDataByDx( sl2::SL2_DXGI_FORMAT_BC7_UNORM );
                    SL2_ADV( 1 );
                }
                if ( SL2_CHECK( 1, A8B8G8R8 ) ) {
                    oOptions.pkifdFinalFormat = sl2::CFormat::FindFormatDataByVulkan( sl2::SL2_VK_FORMAT_A8B8G8R8_UNORM_PACK32 );
                    SL2_ADV( 1 );
                }
                if ( SL2_CHECK( 1, R8G8B8A8 ) ) {
                    oOptions.pkifdFinalFormat = sl2::CFormat::FindFormatDataByVulkan( sl2::SL2_VK_FORMAT_R8G8B8A8_UNORM );
                    SL2_ADV( 1 );
                }
                if ( SL2_CHECK( 1, R16G16 ) ) {
                    oOptions.pkifdFinalFormat = sl2::CFormat::FindFormatDataByVulkan( sl2::SL2_VK_FORMAT_R16G16_UNORM );
                    SL2_ADV( 1 );
                }
                if ( SL2_CHECK( 1, A2B10G10R10 ) ) {
                    oOptions.pkifdFinalFormat = sl2::CFormat::FindFormatDataByVulkan( sl2::SL2_VK_FORMAT_A2B10G10R10_UNORM_PACK32 );
                    SL2_ADV( 1 );
                }
                if ( SL2_CHECK( 1, R10G10B10A2 ) ) {
                    oOptions.pkifdFinalFormat = sl2::CFormat::FindFormatDataByDx( sl2::SL2_DXGI_FORMAT_R10G10B10A2_UNORM );
                    SL2_ADV( 1 );
                }
                if ( SL2_CHECK( 1, A1R5G5B5 ) ) {
                    oOptions.pkifdFinalFormat = sl2::CFormat::FindFormatDataByVulkan( sl2::SL2_VK_FORMAT_A1R5G5B5_UNORM_PACK16 );
                    SL2_ADV( 1 );
                }
                if ( SL2_CHECK( 1, R5G5B5A1 ) ) {
                    oOptions.pkifdFinalFormat = sl2::CFormat::FindFormatDataByVulkan( sl2::SL2_VK_FORMAT_R5G5B5A1_UNORM_PACK16 );
                    SL2_ADV( 1 );
                }
                if ( SL2_CHECK( 1, R5G5B5X1 ) ) {
                    oOptions.pkifdFinalFormat = sl2::CFormat::FindFormatDataByOgl( sl2::SL2_GL_RGB5 );
                    SL2_ADV( 1 );
                }
                if ( SL2_CHECK( 1, R5G6B5 ) ) {
                    oOptions.pkifdFinalFormat = sl2::CFormat::FindFormatDataByVulkan( sl2::SL2_VK_FORMAT_R5G6B5_UNORM_PACK16 );
                    SL2_ADV( 1 );
                }
                if ( SL2_CHECK( 1, A8 ) ) {
                    oOptions.pkifdFinalFormat = sl2::CFormat::FindFormatDataByOgl( sl2::SL2_GL_ALPHA8 );
                    SL2_ADV( 1 );
                }
                if ( SL2_CHECK( 1, B8G8R8X8 ) ) {
                    oOptions.pkifdFinalFormat = sl2::CFormat::FindFormatDataByDx( sl2::SL2_DXGI_FORMAT_B8G8R8X8_UNORM );
                    SL2_ADV( 1 );
                }
                if ( SL2_CHECK( 1, R8G8B8 ) ) {
                    oOptions.pkifdFinalFormat = sl2::CFormat::FindFormatDataByVulkan( sl2::SL2_VK_FORMAT_R8G8B8_UNORM );
                    SL2_ADV( 1 );
                }
                if ( SL2_CHECK( 1, A4R4G4B4 ) ) {
                    oOptions.pkifdFinalFormat = sl2::CFormat::FindFormatDataByVulkan( sl2::SL2_VK_FORMAT_A4R4G4B4_UNORM_PACK16 );
                    SL2_ADV( 1 );
                }
                if ( SL2_CHECK( 1, R4G4B4A4 ) ) {
                    oOptions.pkifdFinalFormat = sl2::CFormat::FindFormatDataByVulkan( sl2::SL2_VK_FORMAT_R4G4B4A4_UNORM_PACK16 );
                    SL2_ADV( 1 );
                }
                if ( SL2_CHECK( 1, L8A8 ) ) {
                    oOptions.pkifdFinalFormat = sl2::CFormat::FindFormatDataByOgl( sl2::SL2_GL_LUMINANCE8_ALPHA8 );
                    SL2_ADV( 1 );
                }
                if ( SL2_CHECK( 1, L16 ) ) {
                    oOptions.pkifdFinalFormat = sl2::CFormat::FindFormatDataByOgl( sl2::SL2_GL_LUMINANCE16 );
                    SL2_ADV( 1 );
                }
                if ( SL2_CHECK( 1, L8 ) ) {
                    oOptions.pkifdFinalFormat = sl2::CFormat::FindFormatDataByOgl( sl2::SL2_GL_LUMINANCE8 );
                    SL2_ADV( 1 );
                }
                if ( SL2_CHECK( 1, L4A4 ) ) {
                    oOptions.pkifdFinalFormat = sl2::CFormat::FindFormatDataByOgl( sl2::SL2_GL_LUMINANCE4_ALPHA4 );
                    SL2_ADV( 1 );
                }
            }

            SL2_ERRORT( std::format( L"Invalid command: \"{}\".",
                _wcpArgV[0] ).c_str(), sl2::SL2_E_INVALIDCALL );
        }
        else {
            SL2_ERRORT( std::format( L"Invalid command: \"{}\".",
                _wcpArgV[0] ).c_str(), sl2::SL2_E_INVALIDCALL );
        }


        --_iArgC;
        ++_wcpArgV;
    }

#undef SL2_ADV
#undef SL2_CHECK

	const sl2::CFormat::SL2_KTX_INTERNAL_FORMAT_DATA * pkifdFormat = oOptions.pkifdFinalFormat;
	for ( size_t I = 0; I < oOptions.vInputs.size(); ++I ) {
		sl2::CImage iImage;
        
		iImage.SetYuvSize( oOptions.vInputs[I].pkifduvFormat, oOptions.vInputs[I].ui32YuvW, oOptions.vInputs[I].ui32YuvH );
		sl2::SL2_ERRORS eError;
		if ( oOptions.vInputs[I].bFromClipBoard ) {
			eError = iImage.LoadFromClipboard();
			if ( eError != sl2::SL2_E_SUCCESS ) {
				SL2_ERRORT( L"Failed to load clipboard image.", eError );
			}
		}
		else {
			eError = iImage.LoadFile( oOptions.vInputs[I].u16Path.c_str() );
			if ( eError != sl2::SL2_E_SUCCESS ) {
				SL2_ERRORT( std::format( L"Failed to load file: \"{}\".",
					reinterpret_cast<const wchar_t *>(oOptions.vInputs[I].u16Path.c_str()) ).c_str(), eError );
			}
		}
		
		iImage.SetCrop( oOptions.wCropWindow );
		iImage.SetQuickRotate( oOptions.qrQuickRot );
		FixResampling( oOptions, iImage );
		iImage.Resampling() = oOptions.rResample;
		iImage.MipResampling() = oOptions.rMipResample;
		iImage.SetNeedsPreMultiply( oOptions.bNeedsPreMultiply );
		iImage.SetIgnoreColorspaceGamma( oOptions.bIgnoreSourceColorspaceGamma );
		if ( oOptions.bManuallySetGamma == true ) {
			iImage.SetGamma( oOptions.dGamma );
		}
		if ( oOptions.bManuallySetTargetGamma == true ) {
			iImage.SetTargetGamma( oOptions.dTargetGamma );
		}
		iImage.SetRenderingIntents( oOptions.i32InRenderingIntent, oOptions.i32OutRenderingIntent );
		iImage.SetColorSpace( oOptions.cgcInputGammaCurve, oOptions.cgcOutputGammaCurve );
		iImage.SetInputColorSpace( oOptions.vInColorProfile );
		iImage.SetOutputColorSpace( oOptions.vOutColorProfile );
		iImage.SetSwizzle( oOptions.sSwizzle );
		iImage.SetSwap( oOptions.bSwap );
		iImage.SetFlip( oOptions.bFlipX, oOptions.bFlipY, oOptions.bFlipZ );
		iImage.SetMipParms( oOptions.mhMipHandling, oOptions.sTotalMips );
		iImage.SetIgnoreAlpha( oOptions.bIgnoreAlpha );
		iImage.SetNormalMapParms( oOptions.kKernel, oOptions.dNormalScale, oOptions.caChannelAccess, oOptions.dNormalYAxis );
		oOptions.pkifdFinalFormat = pkifdFormat;
		if ( !oOptions.pkifdFinalFormat ) {
			oOptions.pkifdFinalFormat = iImage.Format();
		}
		sl2::CFormat::ApplySettings( oOptions.pkifdFinalFormat->ui8ABits != 0, oOptions.pkifdFinalFormat->ui32BlockWidth, oOptions.pkifdFinalFormat->ui32BlockHeight );
		sl2::CImage iConverted;
		sl2::CClock cClock;
		iImage.ConvertToFormat( oOptions.pkifdFinalFormat, iConverted );
		uint64_t ui64Time = cClock.GetRealTick() - cClock.GetStartTick();
		iImage.Reset();
		char szPrintfMe[128];
		::sprintf_s( szPrintfMe, "Conversion time: %.13f seconds.\r\n", ui64Time / static_cast<double>(cClock.GetResolution()) );
		::OutputDebugStringA( szPrintfMe );
		if ( oOptions.bShowTime ) {
			::printf( "Conversion time: %.13f seconds.\r\n", ui64Time / static_cast<double>(cClock.GetResolution()) );
		}
		cClock.SetStartingTick();
#define SL2_CHECKEXT( EXT )     sl2::CFileBase::CmpFileExtension( oOptions.vOutputs[I], u ## #EXT )
		if ( SL2_CHECKEXT( png ) || !oOptions.vOutputs[I].size() ) {
			eError = sl2::ExportAsPng( iConverted, oOptions.vOutputs[I], oOptions );
			if ( sl2::SL2_E_SUCCESS != eError ) {
				SL2_ERRORT( std::format( L"Failed to save file: \"{}\".",
					reinterpret_cast<const wchar_t *>(oOptions.vOutputs[I].c_str()) ).c_str(), eError );
			}
		}
		else if ( SL2_CHECKEXT( bmp ) ) {
			eError = sl2::ExportAsBmp( iConverted, oOptions.vOutputs[I], oOptions );
			if ( sl2::SL2_E_SUCCESS != eError ) {
				SL2_ERRORT( std::format( L"Failed to save file: \"{}\".",
					reinterpret_cast<const wchar_t *>(oOptions.vOutputs[I].c_str()) ).c_str(), eError );
			}
		}
		else if ( SL2_CHECKEXT( exr ) ) {
			eError = sl2::ExportAsExr( iConverted, oOptions.vOutputs[I], oOptions );
			if ( sl2::SL2_E_SUCCESS != eError ) {
				SL2_ERRORT( std::format( L"Failed to save file: \"{}\".",
					reinterpret_cast<const wchar_t *>(oOptions.vOutputs[I].c_str()) ).c_str(), eError );
			}
		}
		else if ( SL2_CHECKEXT( j2k ) ) {
			eError = sl2::ExportAsJ2k( iConverted, oOptions.vOutputs[I], oOptions );
			if ( sl2::SL2_E_SUCCESS != eError ) {
				SL2_ERRORT( std::format( L"Failed to save file: \"{}\".",
					reinterpret_cast<const wchar_t *>(oOptions.vOutputs[I].c_str()) ).c_str(), eError );
			}
		}
		else if ( SL2_CHECKEXT( jp2 ) ) {
			eError = sl2::ExportAsJp2( iConverted, oOptions.vOutputs[I], oOptions );
			if ( sl2::SL2_E_SUCCESS != eError ) {
				SL2_ERRORT( std::format( L"Failed to save file: \"{}\".",
					reinterpret_cast<const wchar_t *>(oOptions.vOutputs[I].c_str()) ).c_str(), eError );
			}
		}
		else if ( SL2_CHECKEXT( jpg ) || SL2_CHECKEXT( jpeg ) ) {
			eError = sl2::ExportAsJpg( iConverted, oOptions.vOutputs[I], oOptions );
			if ( sl2::SL2_E_SUCCESS != eError ) {
				SL2_ERRORT( std::format( L"Failed to save file: \"{}\".",
					reinterpret_cast<const wchar_t *>(oOptions.vOutputs[I].c_str()) ).c_str(), eError );
			}
		}
		else if ( SL2_CHECKEXT( dds ) ) {
			eError = sl2::ExportAsDds( iConverted, oOptions.vOutputs[I], oOptions );
			if ( sl2::SL2_E_SUCCESS != eError ) {
				SL2_ERRORT( std::format( L"Failed to save file: \"{}\".",
					reinterpret_cast<const wchar_t *>(oOptions.vOutputs[I].c_str()) ).c_str(), eError );
			}
		}
		else if ( SL2_CHECKEXT( ktx ) ) {
			eError = sl2::ExportAsKtx1( iConverted, oOptions.vOutputs[I], oOptions );
			if ( sl2::SL2_E_SUCCESS != eError ) {
				SL2_ERRORT( std::format( L"Failed to save file: \"{}\".",
					reinterpret_cast<const wchar_t *>(oOptions.vOutputs[I].c_str()) ).c_str(), eError );
			}
		}
		else if ( SL2_CHECKEXT( tga ) ) {
			eError = sl2::ExportAsTga( iConverted, oOptions.vOutputs[I], oOptions );
			if ( sl2::SL2_E_SUCCESS != eError ) {
				SL2_ERRORT( std::format( L"Failed to save file: \"{}\".",
					reinterpret_cast<const wchar_t *>(oOptions.vOutputs[I].c_str()) ).c_str(), eError );
			}
		}
		else if ( SL2_CHECKEXT( pvr ) ) {
			eError = sl2::ExportAsPvr( iConverted, oOptions.vOutputs[I], oOptions );
			if ( sl2::SL2_E_SUCCESS != eError ) {
				SL2_ERRORT( std::format( L"Failed to save file: \"{}\".",
					reinterpret_cast<const wchar_t *>(oOptions.vOutputs[I].c_str()) ).c_str(), eError );
			}
		}
		else if ( SL2_CHECKEXT( pbm ) ) {
			eError = sl2::ExportAsPbm( iConverted, oOptions.vOutputs[I], oOptions );
			if ( sl2::SL2_E_SUCCESS != eError ) {
				SL2_ERRORT( std::format( L"Failed to save file: \"{}\".",
					reinterpret_cast<const wchar_t *>(oOptions.vOutputs[I].c_str()) ).c_str(), eError );
			}
		}
		else if ( SL2_CHECKEXT( pgm ) ) {
			eError = sl2::ExportAsPgm( iConverted, oOptions.vOutputs[I], oOptions );
			if ( sl2::SL2_E_SUCCESS != eError ) {
				SL2_ERRORT( std::format( L"Failed to save file: \"{}\".",
					reinterpret_cast<const wchar_t *>(oOptions.vOutputs[I].c_str()) ).c_str(), eError );
			}
		}
		else if ( SL2_CHECKEXT( ico ) ) {
			eError = sl2::ExportAsIco( iConverted, oOptions.vOutputs[I], oOptions );
			if ( sl2::SL2_E_SUCCESS != eError ) {
				SL2_ERRORT( std::format( L"Failed to save file: \"{}\".",
					reinterpret_cast<const wchar_t *>(oOptions.vOutputs[I].c_str()) ).c_str(), eError );
			}
		}
#define SL2_YUV_FILE( FMT, EXT )                                                                                                                                \
	else if ( ((oOptions.pkifdYuvFormat && oOptions.pkifdYuvFormat->vfVulkanFormat == sl2::SL2_ ## FMT && (SL2_CHECKEXT( EXT ) || SL2_CHECKEXT( yuv ))) ||      \
		(!oOptions.pkifdYuvFormat && SL2_CHECKEXT( EXT ))) ) {                                                                                                  \
		if ( !oOptions.pkifdYuvFormat ) { oOptions.pkifdYuvFormat = sl2::CFormat::FindFormatDataByVulkan( sl2::SL2_ ## FMT ); }                                 \
		eError = sl2::ExportAsYuv( iConverted, oOptions.vOutputs[I], oOptions );                                                                                \
		if ( sl2::SL2_E_SUCCESS != eError ) {                                                                                                                   \
			SL2_ERRORT( std::format( L"Failed to save file: \"{}\".",                                                                                           \
				reinterpret_cast<const wchar_t *>(oOptions.vOutputs[I].c_str()) ).c_str(), eError );                                                            \
		}                                                                                                                                                       \
	}
		SL2_YUV_FILE( VK_FORMAT_G16_B16_R16_3PLANE_444_UNORM, yuv444p16 )
		SL2_YUV_FILE( VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_444_UNORM_3PACK16, yuv444p12le )
		SL2_YUV_FILE( VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_444_UNORM_3PACK16, yuv444p10le )
		SL2_YUV_FILE( VK_FORMAT_G8_B8_R8_3PLANE_444_UNORM, yuv444p )

		SL2_YUV_FILE( VK_FORMAT_G16_B16R16_2PLANE_444_UNORM, yuv444y16 )
		SL2_YUV_FILE( VK_FORMAT_G12X4_B12X4R12X4_2PLANE_444_UNORM_3PACK16, yuv444y12le )
		SL2_YUV_FILE( VK_FORMAT_G10X6_B10X6R10X6_2PLANE_444_UNORM_3PACK16, yuv444y10le )
		SL2_YUV_FILE( VK_FORMAT_G8_B8R8_2PLANE_444_UNORM, yuv444y )

		SL2_YUV_FILE( VK_FORMAT_G16_B16_R16_3PLANE_422_UNORM, yuv422p16 )
		SL2_YUV_FILE( VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_422_UNORM_3PACK16, yuv422p12le )
		SL2_YUV_FILE( VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_422_UNORM_3PACK16, yuv422p10le )
		SL2_YUV_FILE( VK_FORMAT_G8_B8_R8_3PLANE_422_UNORM, yuv422p )

		SL2_YUV_FILE( VK_FORMAT_G16_B16R16_2PLANE_422_UNORM, yuv422y16 )
		SL2_YUV_FILE( VK_FORMAT_G12X4_B12X4R12X4_2PLANE_422_UNORM_3PACK16, yuv422y12le )
		SL2_YUV_FILE( VK_FORMAT_G10X6_B10X6R10X6_2PLANE_422_UNORM_3PACK16, yuv422y10le )
		SL2_YUV_FILE( VK_FORMAT_G8_B8R8_2PLANE_422_UNORM, yuv422y )

		SL2_YUV_FILE( VK_FORMAT_G16_B16_R16_3PLANE_420_UNORM, yuv420p16 )
		SL2_YUV_FILE( VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_420_UNORM_3PACK16, yuv420p12le )
		SL2_YUV_FILE( VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_420_UNORM_3PACK16, yuv420p10le )
		SL2_YUV_FILE( VK_FORMAT_G8_B8_R8_3PLANE_420_UNORM, yuv420p )

		SL2_YUV_FILE( VK_FORMAT_G16_B16R16_2PLANE_420_UNORM, yuv420y16 )
		SL2_YUV_FILE( VK_FORMAT_G12X4_B12X4R12X4_2PLANE_420_UNORM_3PACK16, yuv420y12le )
		SL2_YUV_FILE( VK_FORMAT_G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16, yuv420y10le )
		SL2_YUV_FILE( VK_FORMAT_G8_B8R8_2PLANE_420_UNORM, yuv420y )

		SL2_YUV_FILE( VK_FORMAT_G16B16G16R16_422_UNORM, yuyv16 )
		SL2_YUV_FILE( VK_FORMAT_G12X4B12X4G12X4R12X4_422_UNORM_4PACK16, yuyv12le )
		SL2_YUV_FILE( VK_FORMAT_G10X6B10X6G10X6R10X6_422_UNORM_4PACK16, yuyv10le )
		SL2_YUV_FILE( VK_FORMAT_G8B8G8R8_422_UNORM, yuy2 )

		SL2_YUV_FILE( VK_FORMAT_B16G16R16G16_422_UNORM, uyvy16 )
		SL2_YUV_FILE( VK_FORMAT_B12X4G12X4R12X4G12X4_422_UNORM_4PACK16, uyvy12le )
		SL2_YUV_FILE( VK_FORMAT_B10X6G10X6R10X6G10X6_422_UNORM_4PACK16, uyvy10le )
		SL2_YUV_FILE( VK_FORMAT_B8G8R8G8_422_UNORM, uyvy )

		SL2_YUV_FILE( VK_FORMAT_R12X4G12X4B12X4A12X4_UNORM_4PACK16, yuva12le )
		SL2_YUV_FILE( VK_FORMAT_R10X6G10X6B10X6A10X6_UNORM_4PACK16, yuva10le )
        
#undef SL2_YUV_FILE

#define SL2_YUV_FILE( FMT, EXT )                                                                                                                                \
    else if ( ((oOptions.pkifdYuvFormat && oOptions.pkifdYuvFormat->dfDxFormat == sl2::SL2_ ## FMT && (SL2_CHECKEXT( EXT ) || SL2_CHECKEXT( yuv ))) ||          \
        (!oOptions.pkifdYuvFormat && SL2_CHECKEXT( EXT ))) ) {                                                                                                  \
        if ( !oOptions.pkifdYuvFormat ) { oOptions.pkifdYuvFormat = sl2::CFormat::FindFormatDataByDx( sl2::SL2_ ## FMT ); }                                     \
        eError = sl2::ExportAsYuv( iConverted, oOptions.vOutputs[I], oOptions );                                                                                \
        if ( sl2::SL2_E_SUCCESS != eError ) {                                                                                                                   \
            SL2_ERRORT( std::format( L"Failed to save file: \"{}\".",                                                                                           \
                reinterpret_cast<const wchar_t *>(oOptions.vOutputs[I].c_str()) ).c_str(), eError );                                                            \
        }                                                                                                                                                       \
    }

        SL2_YUV_FILE( DXGI_FORMAT_P216, p216 )
        SL2_YUV_FILE( DXGI_FORMAT_P210, p210 )
        SL2_YUV_FILE( DXGI_FORMAT_P208, p208 )

        SL2_YUV_FILE( DXGI_FORMAT_420_OPAQUE, yv12 )
        SL2_YUV_FILE( DXGI_FORMAT_YV12, yv12 )

        SL2_YUV_FILE( DXGI_FORMAT_P016, p016 )
        SL2_YUV_FILE( DXGI_FORMAT_P010, p010 )
        SL2_YUV_FILE( DXGI_FORMAT_NV12, nv12 )

        SL2_YUV_FILE( DXGI_FORMAT_NV21, nv21 )

        SL2_YUV_FILE( DXGI_FORMAT_Y216, y216 )
        SL2_YUV_FILE( DXGI_FORMAT_Y210, y210 )
        SL2_YUV_FILE( DXGI_FORMAT_G8R8_G8B8_UNORM, yuy2 )
        SL2_YUV_FILE( DXGI_FORMAT_YUY2, yuy2 )

        SL2_YUV_FILE( DXGI_FORMAT_R8G8_B8G8_UNORM, uyv2 )

        SL2_YUV_FILE( DXGI_FORMAT_Y416, y416 )
        SL2_YUV_FILE( DXGI_FORMAT_Y410, y410 )
        SL2_YUV_FILE( DXGI_FORMAT_AYUV, ayuv )

#undef SL2_YUV_FILE
#undef SL2_CHECKEXT
        
		ui64Time = cClock.GetRealTick() - cClock.GetStartTick();
		::sprintf_s( szPrintfMe, "Save time: %.13f seconds.\r\n", ui64Time / static_cast<double>(cClock.GetResolution()) );
		::OutputDebugStringA( szPrintfMe );
		if ( oOptions.bShowTime ) {
			::printf( "Save time: %.13f seconds.\r\n", ui64Time / static_cast<double>(cClock.GetResolution()) );
		}
		auto sStr = std::format( L"Saved file: \"{}\".\r\n", oOptions.vOutputs[I].size() ? reinterpret_cast<const wchar_t *>(oOptions.vOutputs[I].c_str()) : L"<clipboard>" );
		::OutputDebugStringW( sStr.c_str() );
		::wprintf( sStr.c_str() );
	}


	SL2_ERROR( sl2::SL2_E_SUCCESS );

#undef SL2_ERROR
#undef SL2_ERRORT
}



namespace sl2 {

    // == Functions.
    /**
     * Returns a string representing the given error code.
     * 
     * \param _eError The error code to print to a string.
     * \return Returns a string representing the given error code.
     **/
	std::u16string ErrorToString( SL2_ERRORS _eError ) {
		switch ( _eError ) {
			case SL2_E_OUTOFMEMORY : {
				return std::u16string( u"Out of memory." );
			}
			case SL2_E_FILENOTFOUND : {
				return std::u16string( u"File not found." );
			}
			case SL2_E_INVALIDWRITEPERMISSIONS : {
				return std::u16string( u"Invalid write permissions." );
			}
			case SL2_E_NODISKSPACE : {
				return std::u16string( u"Not enough disk space for file write operation." );
			}
			case SL2_E_INVALIDFILETYPE : {
				return std::u16string( u"File exists but is in an unexpected format." );
			}
			case SL2_E_INVALIDCALL : {
				return std::u16string( u"Invalid call." );
			}
			case SL2_E_INVALIDDATA : {
				return std::u16string( u"Invalid data." );
			}
			case SL2_E_INTERNALERROR : {
				return std::u16string( u"Internal error." );
			}
			case SL2_E_FEATURENOTSUPPORTED : {
				return std::u16string( u"Feature not yet supported." );
			}
			case SL2_E_PARTIALFAILURE : {
				return std::u16string( u"One or more tasks failed." );
			}
			case SL2_E_BADVERSION : {
				return std::u16string( u"Invalid version." );
			}
			case SL2_E_FILEOVERFLOW : {
				return std::u16string( u"File overflow." );
			}
			case SL2_E_FILEWRITEERROR : {
				return std::u16string( u"File write error." );
			}
			case SL2_E_BADFORMAT : {
				return std::u16string( u"Bad data format." );
			}
			case SL2_E_UNSUPPORTEDSIZE : {
				return std::u16string( u"A value is too large for the type required by a given file format." );
			}
			case SL2_E_MULTIFILECLIPBOARD : {
				return std::u16string( u"Only single-file outputs can be sent to the clipboard." );
			}
			case SL2_E_UNAVAILABLECLIPBOARD : {
				return std::u16string( u"Unable to access or write to the clipboard." );
			}
			case SL2_E_PNGUNAVAILABLE : {
				return std::u16string( u"The PNG clipboard format is unavailable." );
			}
		}
		return std::u16string();
	}

    /**
	 * Prints a given error code to the console.
	 * 
	 * \param _pcText Text to print.
	 * \param _eError The error code to print.
	 **/
	void PrintError( const char16_t * _pcText, SL2_ERRORS _eError ) {
		if ( _eError != SL2_E_SUCCESS ) {
			std::u16string sError;
			if ( _pcText ) {
				sError = _pcText;
				sError += u"\r\n";
			}
			sError += ErrorToString( _eError );
			std::wcout << reinterpret_cast<const wchar_t *>(sError.c_str()) << std::endl;
#ifdef SL2_WINDOWS
			::OutputDebugStringW( reinterpret_cast<const wchar_t *>((sError + u"\r\n").c_str()) );
#endif  // #ifdef SL2_WINDOWS
		}
	}

    /**
	 * Fix up the resampling parameters.
	 * 
	 * \param _oOptions The options to fix up.
     * \param _iImage The image off of which to base the adjustments.
	 **/
	void FixResampling( SL2_OPTIONS &_oOptions, CImage &_iImage ) {
		if ( _oOptions.ui32BakedW ) {
			_oOptions.wCropWindow.i32X = -static_cast<int32_t>(_iImage.Width() * _oOptions.ui32BakedW);
			_oOptions.wCropWindow.ui32W = (1 + _oOptions.ui32BakedW * 2) * _iImage.Width();
		}
		if ( _oOptions.ui32BakedH ) {
			_oOptions.wCropWindow.i32Y = -static_cast<int32_t>(_iImage.Height() * _oOptions.ui32BakedH);
			_oOptions.wCropWindow.ui32H = (1 + _oOptions.ui32BakedH * 2) * _iImage.Height();
		}
		if ( _oOptions.ui32BakedD ) {
			_oOptions.wCropWindow.i32Z = -static_cast<int32_t>(_iImage.Depth() * _oOptions.ui32BakedD);
			_oOptions.wCropWindow.ui32D = (1 + _oOptions.ui32BakedD * 2) * _iImage.Depth();
		}

		if ( !_oOptions.wCropWindow.ui32W ) {
			_oOptions.wCropWindow.i32X = 0;
			_oOptions.wCropWindow.ui32W = _iImage.Width();
		}
		if ( !_oOptions.wCropWindow.ui32H ) {
			_oOptions.wCropWindow.i32Y = 0;
			_oOptions.wCropWindow.ui32H = _iImage.Height();
		}
		if ( !_oOptions.wCropWindow.ui32D ) {
			_oOptions.wCropWindow.i32Z = 0;
			_oOptions.wCropWindow.ui32D = _iImage.Depth();
		}

		// Determine the resampling size.
		uint32_t ui32NewWidth = _oOptions.rResample.ui32NewW;
		uint32_t ui32NewHeight = _oOptions.rResample.ui32NewH;
		uint32_t ui32NewDepth = _oOptions.rResample.ui32NewD;
		if ( _oOptions.i32ScaleDims == 2 ) {
			if ( ui32NewWidth && !ui32NewHeight ) {
				ui32NewHeight = static_cast<uint32_t>(std::round( double( ui32NewWidth ) / _oOptions.wCropWindow.ui32W * _oOptions.wCropWindow.ui32H ));
			}
			else if ( !ui32NewWidth && ui32NewHeight ) {
				ui32NewWidth = static_cast<uint32_t>(std::round( double( ui32NewHeight ) / _oOptions.wCropWindow.ui32H * _oOptions.wCropWindow.ui32W ));
			}
		}
		else if ( _oOptions.i32ScaleDims == 3 ) {
		}

		if ( !ui32NewWidth ) { ui32NewWidth = _oOptions.wCropWindow.ui32W; }
		if ( !ui32NewHeight ) { ui32NewHeight = _oOptions.wCropWindow.ui32H; }
		if ( !ui32NewDepth ) { ui32NewDepth = _oOptions.wCropWindow.ui32D; }

		// If relative scaling is applied, apply it.
		ui32NewWidth = static_cast<uint32_t>(ui32NewWidth * _oOptions.dRelScaleW);
		ui32NewHeight = static_cast<uint32_t>(ui32NewHeight * _oOptions.dRelScaleH);
		ui32NewDepth = static_cast<uint32_t>(ui32NewDepth * _oOptions.dRelScaleD);

		// Rounding to any powers of 2?
		switch ( _oOptions.rtResampleTo ) {
			case sl2::SL2_RT_NONE : { break; }
			case sl2::SL2_RT_NEAREST : {
				uint32_t ui32NearestLowW = sl2::CUtilities::GetLowestPo2( ui32NewWidth ) >> 1;
				uint32_t ui32NearestLowH = sl2::CUtilities::GetLowestPo2( ui32NewHeight ) >> 1;
				uint32_t ui32NearestLowD = sl2::CUtilities::GetLowestPo2( ui32NewDepth ) >> 1;

				uint32_t ui32NearestHiW = sl2::CUtilities::GetLowestPo2( ui32NewWidth );
				uint32_t ui32NearestHiH = sl2::CUtilities::GetLowestPo2( ui32NewHeight );
				uint32_t ui32NearestHiD = sl2::CUtilities::GetLowestPo2( ui32NewDepth );

				ui32NewWidth = (ui32NearestHiW - ui32NewWidth) < (ui32NewWidth - ui32NearestLowW) ? ui32NearestHiW : ui32NearestLowW;
				ui32NewHeight = (ui32NearestHiH - ui32NewHeight) < (ui32NewHeight - ui32NearestLowH) ? ui32NearestHiH : ui32NearestLowH;
				ui32NewDepth = (ui32NearestHiD - ui32NewDepth) < (ui32NewDepth - ui32NearestLowD) ? ui32NearestHiD : ui32NearestLowD;
				break;
			}
			case sl2::SL2_RT_NEXT_HI : {
				ui32NewWidth = sl2::CUtilities::GetLowestPo2( ui32NewWidth );
				ui32NewHeight = sl2::CUtilities::GetLowestPo2( ui32NewHeight );
				ui32NewDepth = sl2::CUtilities::GetLowestPo2( ui32NewDepth );
				break;
			}
			case sl2::SL2_RT_NEXT_LO : {
				uint32_t ui32TempW = sl2::CUtilities::GetLowestPo2( ui32NewWidth );
				uint32_t ui32TempH = sl2::CUtilities::GetLowestPo2( ui32NewHeight );
				uint32_t ui32TempD = sl2::CUtilities::GetLowestPo2( ui32NewDepth );
				ui32NewWidth = ui32TempW == ui32NewWidth ? ui32NewWidth : ui32TempW >> 1;
				ui32NewHeight = ui32TempH == ui32NewHeight ? ui32NewHeight : ui32TempH >> 1;
				ui32NewDepth = ui32TempD == ui32NewDepth ? ui32NewDepth : ui32TempD >> 1;
				break;
			}
			case sl2::SL2_RT_FIT : {
				double dScale = 0.0;
				if ( _oOptions.ui32FitW ) {
					double dTmp = double( _oOptions.ui32FitW ) / ui32NewWidth;
					if ( dScale == 0.0 || dTmp < dScale ) { dScale = dTmp; }
				}
				if ( _oOptions.ui32FitH ) {
					double dTmp = double( _oOptions.ui32FitH ) / ui32NewHeight;
					if ( dScale == 0.0 || dTmp < dScale ) { dScale = dTmp; }
				}
				if ( _oOptions.ui32FitD ) {
					double dTmp = double( _oOptions.ui32FitD ) / ui32NewDepth;
					if ( dScale == 0.0 || dTmp < dScale ) { dScale = dTmp; }
				}
				if ( dScale != 0.0 ) {
					ui32NewWidth = uint32_t( std::round( ui32NewWidth * dScale ) );
					ui32NewHeight = uint32_t( std::round( ui32NewHeight * dScale ) );
					if ( _oOptions.ui32FitD || _iImage.Depth() != 1 ) {
						ui32NewDepth = uint32_t( std::round( ui32NewDepth * dScale ) );
					}
				}
				break;
			}
		}
		// Apply clamps.
		if ( _oOptions.ui32ClampW ) {
			ui32NewWidth = std::min( _oOptions.ui32ClampW, ui32NewWidth );
		}
		if ( _oOptions.ui32ClampH ) {
			ui32NewHeight = std::min( _oOptions.ui32ClampH, ui32NewHeight );
		}
		if ( _oOptions.ui32ClampD ) {
			ui32NewDepth = std::min( _oOptions.ui32ClampD, ui32NewDepth );
		}

		_oOptions.rResample.ui32NewW = ui32NewWidth;
		_oOptions.rResample.ui32NewH = ui32NewHeight;
		_oOptions.rResample.ui32NewD = ui32NewDepth;

		_oOptions.rResample.fFilterW = CResampler::m_fFilter[_oOptions.fFilterFuncW];
		_oOptions.rResample.fFilterH = CResampler::m_fFilter[_oOptions.fFilterFuncH];
		_oOptions.rResample.fFilterD = CResampler::m_fFilter[_oOptions.fFilterFuncD];

		_oOptions.rResample.fAlphaFilterW = CResampler::m_fFilter[_oOptions.fAlphaFilterFuncW];
		_oOptions.rResample.fAlphaFilterH = CResampler::m_fFilter[_oOptions.fAlphaFilterFuncH];
		_oOptions.rResample.fAlphaFilterD = CResampler::m_fFilter[_oOptions.fAlphaFilterFuncD];


		_oOptions.rMipResample.fFilterW = CResampler::m_fFilter[_oOptions.fMipFilterFuncW];
		_oOptions.rMipResample.fFilterH = CResampler::m_fFilter[_oOptions.fMipFilterFuncH];
		_oOptions.rMipResample.fFilterD = CResampler::m_fFilter[_oOptions.fMipFilterFuncD];

		_oOptions.rMipResample.fAlphaFilterW = CResampler::m_fFilter[_oOptions.fMipAlphaFilterFuncW];
		_oOptions.rMipResample.fAlphaFilterH = CResampler::m_fFilter[_oOptions.fMipAlphaFilterFuncH];
		_oOptions.rMipResample.fAlphaFilterD = CResampler::m_fFilter[_oOptions.fMipAlphaFilterFuncD];
	}

    /**
	 * Prints a given error code to the console.
	 * 
	 * \param _eError The error code to print.
	 **/
	void PrintError( SL2_ERRORS _eError ) {
		PrintError( nullptr, _eError );
	}

    /**
	 * Exports as PNG.
	 * 
	 * \param _iImage The image to export.
	 * \param _sPath The path to which to export _iImage.
	 * \param _oOptions Export options.
	 * \return Returns an error code.
	 **/
	SL2_ERRORS ExportAsPng( CImage &_iImage, const std::u16string &_sPath, SL2_OPTIONS &_oOptions ) {
		if ( _iImage.Mipmaps() == 1 && _iImage.ArraySize() == 1 && _iImage.Faces() == 1 && _iImage.Depth() == 1 ) {
			return ExportAsPng( _iImage, _sPath, _oOptions, 0, 0, 0, 0 );
		}
		else {
			if ( _sPath.size() == 0 ) {
				// Only individual files can be output to the clipboard.
				return SL2_E_MULTIFILECLIPBOARD;
			}
			wchar_t szBuffer[64];
			std::u16string sRoot = CUtilities::GetFilePath( _sPath ) + CUtilities::NoExtension( _sPath );
			for ( uint32_t M = 0; M < _iImage.Mipmaps(); ++M ) {
				for ( uint32_t A = 0; A < _iImage.ArraySize(); ++A ) {
					for ( uint32_t F = 0; F < _iImage.Faces(); ++F ) {
						for ( uint32_t D = 0; D < _iImage.Depth(); ++D ) {
							wchar_t * pwcBuf = szBuffer;
							if ( _iImage.Mipmaps() > 1 ) { pwcBuf += ::wsprintfW( pwcBuf, L"_M%.2u", M ); }
							if ( _iImage.ArraySize() > 1 ) { pwcBuf += ::wsprintfW( pwcBuf, L"_A%.2u", A ); }
							if ( _iImage.Faces() > 1 ) { pwcBuf += ::wsprintfW( pwcBuf, L"_F%.2u", F ); }
							if ( _iImage.Depth() > 1 ) { pwcBuf += ::wsprintfW( pwcBuf, L"_D%.2u", D ); }
							pwcBuf += ::wsprintfW( pwcBuf, L".png" );
							//::wsprintfW( szBuffer, L"_M%.2u_A%.2u_F%.2u_D%.2u.png", M, A, F, D );
							SL2_ERRORS eErr = ExportAsPng( _iImage, CUtilities::Append( sRoot, szBuffer ), _oOptions, M, A, F, D );
							if ( eErr != SL2_E_SUCCESS ) { return eErr; }
						}
					}
				}
			}
		}
		return SL2_E_SUCCESS;
	}

    /**
	 * Exports as PNG.
	 * 
	 * \param _iImage The image to export.
	 * \param _sPath The path to which to export _iImage.
	 * \param _oOptions Export options.
	 * \param _sMip The mipmap level to export.
	 * \param _sArray The array index to export.
	 * \param _sFace The face to export.
     * \param _sSlice The slice to export.
	 * \return Returns an error code.
	 **/
	SL2_ERRORS ExportAsPng( CImage &_iImage, const std::u16string &_sPath, SL2_OPTIONS &_oOptions, size_t _sMip, size_t _sArray, size_t _sFace, size_t _sSlice ) {
		const CFormat::SL2_BEST_INTERNAL_FORMAT bifFormats[] = {
			{ CFormat::FindFormatDataByVulkan( SL2_VK_FORMAT_R8G8B8_UNORM ), },
			{ CFormat::FindFormatDataByVulkan( SL2_VK_FORMAT_R8G8B8_SRGB ), },
			{ CFormat::FindFormatDataByVulkan( SL2_VK_FORMAT_R8G8B8A8_UNORM ), },
			{ CFormat::FindFormatDataByVulkan( SL2_VK_FORMAT_R8G8B8A8_SRGB ), },

			{ CFormat::FindFormatDataByVulkan( SL2_VK_FORMAT_R16G16B16_UNORM ) },
			{ CFormat::FindFormatDataByVulkan( SL2_VK_FORMAT_R16G16B16A16_UNORM ) },

			{ CFormat::FindFormatDataByOgl( SL2_GL_LUMINANCE8 ) },
			{ CFormat::FindFormatDataByOgl( SL2_GL_LUMINANCE16 ) },

			{ CFormat::FindFormatDataByOgl( SL2_GL_COLOR_INDEX1_EXT ) },
			{ CFormat::FindFormatDataByOgl( SL2_GL_COLOR_INDEX2_EXT ) },
			{ CFormat::FindFormatDataByOgl( SL2_GL_COLOR_INDEX4_EXT ) },
			{ CFormat::FindFormatDataByOgl( SL2_GL_COLOR_INDEX8_EXT ) },
		};
		const CFormat::SL2_BEST_INTERNAL_FORMAT * pbifUseMe = nullptr;

		if ( _oOptions.pkifdPngFormat ) {
			switch ( _oOptions.pkifdPngFormat->vfVulkanFormat ) {
				case SL2_VK_FORMAT_R8G8B8_UNORM : {
					pbifUseMe = &bifFormats[0];
					break;
				}
				case SL2_VK_FORMAT_R8G8B8A8_UNORM : {
					pbifUseMe = &bifFormats[1];
					break;
				}
				case SL2_VK_FORMAT_R16G16B16_UNORM : {
					pbifUseMe = &bifFormats[2];
					break;
				}
				case SL2_VK_FORMAT_R16G16B16A16_UNORM : {
					pbifUseMe = &bifFormats[3];
					break;
				}
			}
			switch ( _oOptions.pkifdPngFormat->kifInternalFormat ) {
				case SL2_GL_LUMINANCE8 : {
					pbifUseMe = &bifFormats[4];
					break;
				}
				case SL2_GL_LUMINANCE16 : {
					pbifUseMe = &bifFormats[5];
					break;
				}
				case SL2_GL_COLOR_INDEX1_EXT : {
					pbifUseMe = &bifFormats[8];
					break;
				}
				case SL2_GL_COLOR_INDEX2_EXT : {
					pbifUseMe = &bifFormats[9];
					break;
				}
				case SL2_GL_COLOR_INDEX4_EXT : {
					pbifUseMe = &bifFormats[10];
					break;
				}
				case SL2_GL_COLOR_INDEX8_EXT : {
					pbifUseMe = &bifFormats[11];
					break;
				}
			}
		}
		else { pbifUseMe = CFormat::FindBestFormat( _iImage.Format(), bifFormats, SL2_ELEMENTS( bifFormats ) ); }

		if ( !pbifUseMe ) {
			return SL2_E_BADFORMAT;
		}
		FREE_IMAGE_TYPE fitType = FIT_BITMAP;
		if ( pbifUseMe->pkifdFormat->kifInternalFormat == SL2_GL_LUMINANCE8 ) {
		}
		else if ( pbifUseMe->pkifdFormat->kifInternalFormat == SL2_GL_LUMINANCE16 ) {
			fitType = FIT_UINT16;
		}
		else if ( pbifUseMe->pkifdFormat->kifInternalFormat == SL2_GL_LUMINANCE8_ALPHA8 ) {
		}
		else if ( pbifUseMe->pkifdFormat->kifInternalFormat == SL2_GL_LUMINANCE16_ALPHA16 ) {
			fitType = FIT_RGBA16;
		}
		else if ( pbifUseMe->pkifdFormat->ui32BlockSizeInBits == 16 * 3 ) {
			fitType = FIT_RGB16;
		}
		else if ( pbifUseMe->pkifdFormat->ui32BlockSizeInBits == 16 * 4 ) {
			fitType = FIT_RGBA16;
		}

		if ( SL2_GET_IDX_FLAG( pbifUseMe->pkifdFormat->ui32Flags ) ) {
			return ExportAsPng_Indexed( _iImage, _sPath, _oOptions, _sMip, _sArray, _sFace, _sSlice, pbifUseMe );
		}
		CImage::SL2_FREEIMAGE_ALLOCATET fiImage( fitType, _iImage.GetMipmaps()[_sMip]->Width(), _iImage.GetMipmaps()[_sMip]->Height(), pbifUseMe->pkifdFormat->ui32BlockSizeInBits );
		if ( !fiImage.pbBitmap ) { return SL2_E_OUTOFMEMORY; }

		std::vector<uint8_t> vConverted;
		SL2_ERRORS eError = _iImage.ConvertToFormat( pbifUseMe->pkifdFormat, _sMip, _sArray, _sFace, vConverted, true );
		if ( eError != SL2_E_SUCCESS ) { return eError; }


		size_t sPitch = CFormat::GetRowSize( pbifUseMe->pkifdFormat, _iImage.GetMipmaps()[_sMip]->Width() );
		uint32_t ui32Slice = uint32_t( sPitch * _iImage.GetMipmaps()[_sMip]->Height() * _sSlice );
		for ( uint32_t H = 0; H < _iImage.GetMipmaps()[_sMip]->Height(); ++H ) {
			BYTE * pui8Bits = ::FreeImage_GetScanLine( fiImage.pbBitmap, int( H ) );
			uint8_t * pui8Src = vConverted.data() + ui32Slice + sPitch * H;
			switch ( fitType ) {
				case FIT_BITMAP : {
					switch ( pbifUseMe->pkifdFormat->ui32BlockSizeInBits ) {
						case 8 * 1 : {
							for ( uint32_t X = 0; X < _iImage.GetMipmaps()[_sMip]->Width(); ++X ) {
								uint8_t * prgbDst = pui8Bits + X;
								const uint8_t * pui8This = pui8Src + X;
								(*prgbDst) = (*pui8This);
							}
							break;
						}
						case 8 * 2 : {
							for ( uint32_t X = 0; X < _iImage.GetMipmaps()[_sMip]->Width(); ++X ) {
								uint16_t * prgbDst = reinterpret_cast<uint16_t *>(pui8Bits) + X;
								const uint16_t * prgbSrc = reinterpret_cast<uint16_t *>(pui8Src) + X;
								/*prgbDst->red = (*prgbSrc) & 0xFF;
								prgbDst->alpha = (*prgbSrc) >> 8;*/
								(*prgbDst) = (*prgbSrc);
							}
							break;
						}
						case 8 * 3 : {
							for ( uint32_t X = 0; X < _iImage.GetMipmaps()[_sMip]->Width(); ++X ) {
								RGBTRIPLE * prgbDst = reinterpret_cast<RGBTRIPLE *>(pui8Bits) + X;
								const CFormat::SL2_RGB_UNORM * prgbSrc = reinterpret_cast<CFormat::SL2_RGB_UNORM *>(pui8Src) + X;
								prgbDst->rgbtRed = prgbSrc->ui8Rgb[SL2_PC_R];
								prgbDst->rgbtGreen = prgbSrc->ui8Rgb[SL2_PC_G];
								prgbDst->rgbtBlue = prgbSrc->ui8Rgb[SL2_PC_B];
							}
							break;
						}
						case 8 * 4 : {
							for ( uint32_t X = 0; X < _iImage.GetMipmaps()[_sMip]->Width(); ++X ) {
								RGBQUAD * prgbDst = reinterpret_cast<RGBQUAD *>(pui8Bits) + X;
								const CFormat::SL2_RGBA_UNORM * prgbSrc = reinterpret_cast<CFormat::SL2_RGBA_UNORM *>(pui8Src) + X;
								prgbDst->rgbRed = prgbSrc->ui8Rgba[SL2_PC_R];
								prgbDst->rgbGreen = prgbSrc->ui8Rgba[SL2_PC_G];
								prgbDst->rgbBlue = prgbSrc->ui8Rgba[SL2_PC_B];
								prgbDst->rgbReserved = prgbSrc->ui8Rgba[SL2_PC_A];
							}
							break;
						}
					}
					break;
				}
				case FIT_UINT16 : {
					for ( uint32_t X = 0; X < _iImage.GetMipmaps()[_sMip]->Width(); ++X ) {
						uint16_t * prgbDst = reinterpret_cast<uint16_t *>(pui8Bits) + X;
						const uint16_t * prgbSrc = reinterpret_cast<uint16_t *>(pui8Src) + X;
						(*prgbDst) = (*prgbSrc);
					}
					break;
				}
				case FIT_RGB16 : {
					for ( uint32_t X = 0; X < _iImage.GetMipmaps()[_sMip]->Width(); ++X ) {
						FIRGB16 * prgbDst = reinterpret_cast<FIRGB16 *>(pui8Bits) + X;
						const CFormat::SL2_RGB16_UNORM * prgbSrc = reinterpret_cast<CFormat::SL2_RGB16_UNORM *>(pui8Src) + X;
						prgbDst->red = prgbSrc->ui16Rgb[SL2_PC_R];
						prgbDst->green = prgbSrc->ui16Rgb[SL2_PC_G];
						prgbDst->blue = prgbSrc->ui16Rgb[SL2_PC_B];
					}
					break;
				}
				case FIT_RGBA16 : {
					for ( uint32_t X = 0; X < _iImage.GetMipmaps()[_sMip]->Width(); ++X ) {
						FIRGBA16 * prgbDst = reinterpret_cast<FIRGBA16 *>(pui8Bits) + X;
						const CFormat::SL2_RGBA16_UNORM * prgbSrc = reinterpret_cast<CFormat::SL2_RGBA16_UNORM *>(pui8Src) + X;
						prgbDst->red = prgbSrc->ui16Rgba[SL2_PC_R];
						prgbDst->green = prgbSrc->ui16Rgba[SL2_PC_G];
						prgbDst->blue = prgbSrc->ui16Rgba[SL2_PC_B];
						prgbDst->alpha = prgbSrc->ui16Rgba[SL2_PC_A];
					}
					break;
				}
			}
		}


		CImage::SL2_FREE_IMAGE fiBuffer;
		if ( !fiBuffer.pmMemory ) { return SL2_E_OUTOFMEMORY; }

		if ( _oOptions.bEmbedColorProfile && _iImage.OutputColorSpace().size() && static_cast<size_t>(static_cast<long>(_iImage.OutputColorSpace().size())) == _iImage.OutputColorSpace().size() && static_cast<long>(_iImage.OutputColorSpace().size()) > 0 ) {
			if ( !::FreeImage_CreateICCProfile( fiImage.pbBitmap, static_cast<void *>(const_cast<uint8_t *>(_iImage.OutputColorSpace().data())), static_cast<long>(_iImage.OutputColorSpace().size()) ) ) { return SL2_E_OUTOFMEMORY; }
		}

		if ( !::FreeImage_SaveToMemory( FIF_PNG, fiImage.pbBitmap, fiBuffer.pmMemory, _oOptions.iPngSaveOption ) ) {
			return SL2_E_OUTOFMEMORY;
		}

		BYTE * pbData = nullptr;
		DWORD dwSize = 0;
		if ( !::FreeImage_AcquireMemory( fiBuffer.pmMemory, &pbData, &dwSize ) ) {
			return SL2_E_INTERNALERROR;
		}
		try {
			vConverted.resize( dwSize );
		}
		catch ( ... ) {
			return SL2_E_OUTOFMEMORY;
		}
		std::memcpy( vConverted.data(), pbData, dwSize );
        
		if ( _sPath.size() ) {
			{
				CStdFile sfFile;
				if ( !sfFile.Create( _sPath.c_str() ) ) {
					return SL2_E_INVALIDWRITEPERMISSIONS;
				}
				if ( !sfFile.WriteToFile( vConverted ) ) {
					return SL2_E_FILEWRITEERROR;
				}
			}
		}
		else {
			// Save to the clipboard.
			try {
#ifdef _WIN32
				if ( !CUtilities::ImageToClipBoard( CUtilities::SL2_CF_PNG, vConverted ) ) { return SL2_E_UNAVAILABLECLIPBOARD; }
				else {
					return SL2_E_PNGUNAVAILABLE;
				}
#endif	// #ifdef _WIN32
			}
			catch ( ... ) {
				return SL2_E_OUTOFMEMORY;
			}
		}

		return SL2_E_SUCCESS;
	}

    /**
	 * Exports as PNG.
	 * 
	 * \param _iImage The image to export.
	 * \param _sPath The path to which to export _iImage.
	 * \param _oOptions Export options.
	 * \param _sMip The mipmap level to export.
	 * \param _sArray The array index to export.
	 * \param _sFace The face to export.
	 * \param _sSlice The slice to export.
	 * \param _pbifFormat The target indexed format.
	 * \return Returns an error code.
	 **/
	SL2_ERRORS ExportAsPng_Indexed( CImage &_iImage, const std::u16string &_sPath, SL2_OPTIONS &_oOptions, size_t _sMip, size_t _sArray, size_t _sFace, size_t _sSlice,
		const CFormat::SL2_BEST_INTERNAL_FORMAT * _pbifFormat ) {
		size_t sMax = size_t( 1ULL << _pbifFormat->pkifdFormat->ui32BlockSizeInBits );
		//_iImage.GeneratePalette( uint32_t( sMax ) );
		//if ( _iImage.Palette().Palette().size() > sMax ) { return SL2_E_UNSUPPORTEDSIZE; }
		CPalette pPalette;
		CImage::SL2_FREEIMAGE_ALLOCATET fiImage( FIT_BITMAP, _iImage.GetMipmaps()[_sMip]->Width(), _iImage.GetMipmaps()[_sMip]->Height(), _pbifFormat->pkifdFormat->ui32BlockSizeInBits );
		if ( !fiImage.pbBitmap ) { return SL2_E_OUTOFMEMORY; }

		std::vector<uint8_t> vConverted;
		SL2_ERRORS eError = _iImage.ConvertToFormat( _pbifFormat->pkifdFormat, _sMip, _sArray, _sFace, vConverted, true, true, &pPalette );
		if ( eError != SL2_E_SUCCESS ) { return eError; }

		// Set the palette.
		RGBQUAD * prgbVal = ::FreeImage_GetPalette( fiImage.pbBitmap );
		for ( size_t I = 0; I < pPalette.Palette().size(); ++I ) {
			prgbVal[I].rgbRed = static_cast<BYTE>(std::clamp( std::round( pPalette.Palette()[I].X() * 255.0 ), 0.0, 255.0 ));
			prgbVal[I].rgbGreen = static_cast<BYTE>(std::clamp( std::round( pPalette.Palette()[I].Y() * 255.0 ), 0.0, 255.0 ));
			prgbVal[I].rgbBlue = static_cast<BYTE>(std::clamp( std::round( pPalette.Palette()[I].Z() * 255.0 ), 0.0, 255.0 ));
			prgbVal[I].rgbReserved = static_cast<BYTE>(std::clamp( std::round( pPalette.Palette()[I].W() * 255.0 ), 0.0, 255.0 ));
		}

		size_t sPitch = CFormat::GetRowSize( _pbifFormat->pkifdFormat, _iImage.GetMipmaps()[_sMip]->Width() );
		uint32_t ui32Slice = uint32_t( sPitch * _iImage.GetMipmaps()[_sMip]->Height() * _sSlice );
		for ( uint32_t H = 0; H < _iImage.GetMipmaps()[_sMip]->Height(); ++H ) {
			BYTE * pui8Bits = ::FreeImage_GetScanLine( fiImage.pbBitmap, int( H ) );
			uint8_t * pui8Src = vConverted.data() + ui32Slice + sPitch * H;
			switch ( _pbifFormat->pkifdFormat->ui32BlockSizeInBits ) {
				case 1 : {
					size_t sMask = size_t( (1LL << _pbifFormat->pkifdFormat->ui32BlockSizeInBits) - 1 );
					size_t sSegs = 8 / _pbifFormat->pkifdFormat->ui32BlockSizeInBits;
					for ( uint32_t X = 0; X < _iImage.GetMipmaps()[_sMip]->Width(); ++X ) {
						size_t sOff = X * _pbifFormat->pkifdFormat->ui32BlockSizeInBits / 8L;
						size_t sShift = ((sSegs) - (X & ((sSegs) - 1)) - 1) * _pbifFormat->pkifdFormat->ui32BlockSizeInBits;
						uint8_t * prgbDst = pui8Bits + sOff;
						const uint8_t * pui8This = pui8Src + X;
						(*prgbDst) = (((*pui8This) & sMask) << sShift) | ((*prgbDst) & ~(sMask << sShift));
					}
					break;
				}
				case 2 : {
					size_t sMask = size_t( (1LL << _pbifFormat->pkifdFormat->ui32BlockSizeInBits) - 1 );
					for ( uint32_t X = 0; X < _iImage.GetMipmaps()[_sMip]->Width(); ++X ) {
						uint8_t * prgbDst = pui8Bits + X;
						const uint8_t * pui8This = pui8Src + X;
						(*prgbDst) = (*pui8This) & sMask;
					}
					break;
				}
				case 4 : {
					size_t sMask = size_t( (1LL << _pbifFormat->pkifdFormat->ui32BlockSizeInBits) - 1 );
					size_t sSegs = 8 / _pbifFormat->pkifdFormat->ui32BlockSizeInBits;
					for ( uint32_t X = 0; X < _iImage.GetMipmaps()[_sMip]->Width(); ++X ) {
						size_t sOff = X * _pbifFormat->pkifdFormat->ui32BlockSizeInBits / 8L;
						size_t sShift = uint8_t( !(X & 1L) ) * _pbifFormat->pkifdFormat->ui32BlockSizeInBits;
						//size_t sShift = ((sSegs) - (X & ((sSegs) - 1)) - 1) * _pbifFormat->pkifdFormat->ui32BlockSizeInBits;
						uint8_t * prgbDst = pui8Bits + sOff;
						const uint8_t * pui8This = pui8Src + X;
						(*prgbDst) = (((*pui8This) & sMask) << sShift) | ((*prgbDst) & ~(sMask << sShift));
					}
					break;
				}
				case 8 : {
					for ( uint32_t X = 0; X < _iImage.GetMipmaps()[_sMip]->Width(); ++X ) {
						uint8_t * prgbDst = pui8Bits + X;
						const uint8_t * pui8This = pui8Src + X;
						(*prgbDst) = (*pui8This);
					}
					break;
				}
			}
		}

		CImage::SL2_FREE_IMAGE fiBuffer;
		if ( !fiBuffer.pmMemory ) { return SL2_E_OUTOFMEMORY; }

		if ( _oOptions.bEmbedColorProfile && _iImage.OutputColorSpace().size() && static_cast<size_t>(static_cast<long>(_iImage.OutputColorSpace().size())) == _iImage.OutputColorSpace().size() && static_cast<long>(_iImage.OutputColorSpace().size()) > 0 ) {
			if ( !::FreeImage_CreateICCProfile( fiImage.pbBitmap, static_cast<void *>(const_cast<uint8_t *>(_iImage.OutputColorSpace().data())), static_cast<long>(_iImage.OutputColorSpace().size()) ) ) { return SL2_E_OUTOFMEMORY; }
		}

		if ( !::FreeImage_SaveToMemory( FIF_PNG, fiImage.pbBitmap, fiBuffer.pmMemory, _oOptions.iPngSaveOption ) ) {
			return SL2_E_OUTOFMEMORY;
		}
        

		BYTE * pbData = nullptr;
		DWORD dwSize = 0;
		if ( !::FreeImage_AcquireMemory( fiBuffer.pmMemory, &pbData, &dwSize ) ) {
			return SL2_E_INTERNALERROR;
		}
		try {
			vConverted.resize( dwSize );
		}
		catch ( ... ) {
			return SL2_E_OUTOFMEMORY;
		}
		std::memcpy( vConverted.data(), pbData, dwSize );
		{
			CStdFile sfFile;
			if ( !sfFile.Create( _sPath.c_str() ) ) {
				return SL2_E_INVALIDWRITEPERMISSIONS;
			}
			if ( !sfFile.WriteToFile( vConverted ) ) {
				return SL2_E_FILEWRITEERROR;
			}
		}

		return SL2_E_SUCCESS;
	}

    /**
	 * Exports as BMP.
	 * 
	 * \param _iImage The image to export.
	 * \param _sPath The path to which to export _iImage.
	 * \param _oOptions Export options.
	 * \return Returns an error code.
	 **/
	SL2_ERRORS ExportAsBmp( CImage &_iImage, const std::u16string &_sPath, SL2_OPTIONS &_oOptions ) {
		if ( _iImage.Mipmaps() == 1 && _iImage.ArraySize() == 1 && _iImage.Faces() == 1 && _iImage.Depth() == 1 ) {
			return ExportAsBmp( _iImage, _sPath, _oOptions, 0, 0, 0, 0 );
		}
		else {
			wchar_t szBuffer[64];
			std::u16string sRoot = CUtilities::GetFilePath( _sPath ) + CUtilities::NoExtension( _sPath );
			for ( uint32_t M = 0; M < _iImage.Mipmaps(); ++M ) {
				for ( uint32_t A = 0; A < _iImage.ArraySize(); ++A ) {
					for ( uint32_t F = 0; F < _iImage.Faces(); ++F ) {
						for ( uint32_t D = 0; D < _iImage.Depth(); ++D ) {
							wchar_t * pwcBuf = szBuffer;
							if ( _iImage.Mipmaps() > 1 ) { pwcBuf += ::wsprintfW( pwcBuf, L"_M%.2u", M ); }
							if ( _iImage.ArraySize() > 1 ) { pwcBuf += ::wsprintfW( pwcBuf, L"_A%.2u", A ); }
							if ( _iImage.Faces() > 1 ) { pwcBuf += ::wsprintfW( pwcBuf, L"_F%.2u", F ); }
							if ( _iImage.Depth() > 1 ) { pwcBuf += ::wsprintfW( pwcBuf, L"_D%.2u", D ); }
							pwcBuf += ::wsprintfW( pwcBuf, L".bmp" );
							//::wsprintfW( szBuffer, L"_M%.2u_A%.2u_F%.2u_D%.2u.png", M, A, F, D );
							SL2_ERRORS eErr = ExportAsBmp( _iImage, CUtilities::Append( sRoot, szBuffer ), _oOptions, M, A, F, D );
							if ( eErr != SL2_E_SUCCESS ) { return eErr; }
						}
					}
				}
			}
		}
		return SL2_E_SUCCESS;
	}

	/**
	 * Exports as BMP.
	 * 
	 * \param _iImage The image to export.
	 * \param _sPath The path to which to export _iImage.
	 * \param _oOptions Export options.
	 * \param _sMip The mipmap level to export.
	 * \param _sArray The array index to export.
	 * \param _sFace The face to export.
	 * \param _sSlice The slice to export.
	 * \return Returns an error code.
	 **/
	SL2_ERRORS ExportAsBmp( CImage &_iImage, const std::u16string &_sPath, SL2_OPTIONS &_oOptions, size_t _sMip, size_t _sArray, size_t _sFace, size_t _sSlice ) {
		if ( _oOptions.bBmpStoreBitmask && SL2_E_SUCCESS == ExportAsBmpWithMasks( _iImage, _sPath, _oOptions, _sMip, _sArray, _sFace, _sSlice ) ) {
			return SL2_E_SUCCESS;
		}
		const CFormat::SL2_BEST_INTERNAL_FORMAT bifFormats[] = {
			{ CFormat::FindFormatDataByVulkan( SL2_VK_FORMAT_R8G8B8_UNORM ), },
			{ CFormat::FindFormatDataByVulkan( SL2_VK_FORMAT_R8G8B8_SRGB ), },
			{ CFormat::FindFormatDataByVulkan( SL2_VK_FORMAT_R8G8B8A8_UNORM ), },
			{ CFormat::FindFormatDataByVulkan( SL2_VK_FORMAT_R8G8B8A8_SRGB ), },

			{ CFormat::FindFormatDataByVulkan( SL2_VK_FORMAT_A1R5G5B5_UNORM_PACK16 ), },
			{ CFormat::FindFormatDataByVulkan( SL2_VK_FORMAT_R5G6B5_UNORM_PACK16 ), },
		};
		const CFormat::SL2_BEST_INTERNAL_FORMAT * pkifdUseMe = nullptr;
		CFormat::SL2_BEST_INTERNAL_FORMAT bifTmp;
		if ( _oOptions.vkBmpFormatNoMask != SL2_VK_FORMAT_UNDEFINED ) {
			bifTmp.pkifdFormat = CFormat::FindFormatDataByVulkan( _oOptions.vkBmpFormatNoMask );
			pkifdUseMe = &bifTmp;
		}
		else {
			pkifdUseMe = CFormat::FindBestFormat( _iImage.Format(), bifFormats, SL2_ELEMENTS( bifFormats ) );
			if ( !pkifdUseMe ) {
				return SL2_E_BADFORMAT;
			}
		}

		std::vector<uint8_t> vConverted;
		SL2_ERRORS eError = _iImage.ConvertToFormat( pkifdUseMe->pkifdFormat, _sMip, _sArray, _sFace, vConverted, true );
		if ( eError != SL2_E_SUCCESS ) { return eError; }

		FREE_IMAGE_TYPE fitType = FIT_BITMAP;
		unsigned uRedMask = ((1 << pkifdUseMe->pkifdFormat->ui8RBits) - 1) << pkifdUseMe->pkifdFormat->ui8RShift;
		unsigned uGreenMask = ((1 << pkifdUseMe->pkifdFormat->ui8GBits) - 1) << pkifdUseMe->pkifdFormat->ui8GShift;
		unsigned uBlueMask = ((1 << pkifdUseMe->pkifdFormat->ui8BBits) - 1) << pkifdUseMe->pkifdFormat->ui8BShift;
		if ( pkifdUseMe->pkifdFormat->ui32BlockSizeInBits == 24 ) {
			uRedMask = 0xFF << (offsetof( RGBTRIPLE, rgbtRed ) * 8);
			uGreenMask = 0xFF << (offsetof( RGBTRIPLE, rgbtGreen ) * 8);
			uBlueMask = 0xFF << (offsetof( RGBTRIPLE, rgbtBlue ) * 8);
		}
		else if ( pkifdUseMe->pkifdFormat->ui32BlockSizeInBits == 32 ) {
			uRedMask = 0xFF << (offsetof( RGBQUAD, rgbRed ) * 8);
			uGreenMask = 0xFF << (offsetof( RGBQUAD, rgbGreen ) * 8);
			uBlueMask = 0xFF << (offsetof( RGBQUAD, rgbBlue ) * 8);
		}
		CImage::SL2_FREEIMAGE_ALLOCATET fiImage( fitType, _iImage.GetMipmaps()[_sMip]->Width(), _iImage.GetMipmaps()[_sMip]->Height(), pkifdUseMe->pkifdFormat->ui32BlockSizeInBits,
			uRedMask,
			uGreenMask,
			uBlueMask );
		if ( !fiImage.pbBitmap ) { return SL2_E_OUTOFMEMORY; }


		size_t sPitch = CFormat::GetRowSize( pkifdUseMe->pkifdFormat, _iImage.GetMipmaps()[_sMip]->Width() );
		uint32_t ui32Slice = uint32_t( sPitch * _iImage.GetMipmaps()[_sMip]->Height() * _sSlice );
		for ( uint32_t H = 0; H < _iImage.GetMipmaps()[_sMip]->Height(); ++H ) {
			BYTE * pui8Bits = ::FreeImage_GetScanLine( fiImage.pbBitmap, int( H ) );
			uint8_t * pui8Src = vConverted.data() + ui32Slice + sPitch * H;
			switch ( fitType ) {
				case FIT_BITMAP : {
					switch ( pkifdUseMe->pkifdFormat->ui32BlockSizeInBits ) {
						case 16 : {
							for ( uint32_t X = 0; X < _iImage.GetMipmaps()[_sMip]->Width(); ++X ) {
								uint16_t * prgbDst = reinterpret_cast<uint16_t *>(pui8Bits) + X;
								const uint16_t * prgbSrc = reinterpret_cast<uint16_t *>(pui8Src) + X;
								/*prgbDst->red = (*prgbSrc) & 0xFF;
								prgbDst->alpha = (*prgbSrc) >> 8;*/
								(*prgbDst) = (*prgbSrc);
							}
							break;
						}
						case 8 * 3 : {
							for ( uint32_t X = 0; X < _iImage.GetMipmaps()[_sMip]->Width(); ++X ) {
								RGBTRIPLE * prgbDst = reinterpret_cast<RGBTRIPLE *>(pui8Bits) + X;
								const CFormat::SL2_RGB_UNORM * prgbSrc = reinterpret_cast<CFormat::SL2_RGB_UNORM *>(pui8Src) + X;
								prgbDst->rgbtRed = prgbSrc->ui8Rgb[SL2_PC_R];
								prgbDst->rgbtGreen = prgbSrc->ui8Rgb[SL2_PC_G];
								prgbDst->rgbtBlue = prgbSrc->ui8Rgb[SL2_PC_B];
							}
							break;
						}
						case 8 * 4 : {
							for ( uint32_t X = 0; X < _iImage.GetMipmaps()[_sMip]->Width(); ++X ) {
								RGBQUAD * prgbDst = reinterpret_cast<RGBQUAD *>(pui8Bits) + X;
								const CFormat::SL2_RGBA_UNORM * prgbSrc = reinterpret_cast<CFormat::SL2_RGBA_UNORM *>(pui8Src) + X;
								prgbDst->rgbRed = prgbSrc->ui8Rgba[SL2_PC_R];
								prgbDst->rgbGreen = prgbSrc->ui8Rgba[SL2_PC_G];
								prgbDst->rgbBlue = prgbSrc->ui8Rgba[SL2_PC_B];
								prgbDst->rgbReserved = _oOptions.bBmpHasAlpha ? prgbSrc->ui8Rgba[SL2_PC_A] : 0xFF;
							}
							break;
						}
					}
					break;
				}
			}
		}


		CImage::SL2_FREE_IMAGE fiBuffer;
		if ( !fiBuffer.pmMemory ) { return SL2_E_OUTOFMEMORY; }

		if ( _oOptions.bEmbedColorProfile && _iImage.OutputColorSpace().size() && static_cast<size_t>(static_cast<long>(_iImage.OutputColorSpace().size())) == _iImage.OutputColorSpace().size() && static_cast<long>(_iImage.OutputColorSpace().size()) > 0 ) {
			if ( !::FreeImage_CreateICCProfile( fiImage.pbBitmap, static_cast<void *>(const_cast<uint8_t *>(_iImage.OutputColorSpace().data())), static_cast<long>(_iImage.OutputColorSpace().size()) ) ) { return SL2_E_OUTOFMEMORY; }
		}

		if ( !::FreeImage_SaveToMemory( FIF_BMP, fiImage.pbBitmap, fiBuffer.pmMemory, _oOptions.iBmpSaveOption ) ) {
			return SL2_E_OUTOFMEMORY;
		}
		BYTE * pbData = nullptr;
		DWORD dwSize = 0;
		if ( !::FreeImage_AcquireMemory( fiBuffer.pmMemory, &pbData, &dwSize ) ) {
			return SL2_E_INTERNALERROR;
		}
		try {
			vConverted.resize( dwSize );
		}
		catch ( ... ) {
			return SL2_E_OUTOFMEMORY;
		}
		std::memcpy( vConverted.data(), pbData, dwSize );
		{
			CStdFile sfFile;
			if ( !sfFile.Create( _sPath.c_str() ) ) {
				return SL2_E_INVALIDWRITEPERMISSIONS;
			}
			if ( !sfFile.WriteToFile( vConverted ) ) {
				return SL2_E_FILEWRITEERROR;
			}
		}

		return SL2_E_SUCCESS;
	}

    /**
	 * Exports as BMP.
	 * 
	 * \param _iImage The image to export.
	 * \param _sPath The path to which to export _iImage.
	 * \param _oOptions Export options.
	 * \param _sMip The mipmap level to export.
	 * \param _sArray The array index to export.
	 * \param _sFace The face to export.
	 * \param _sSlice The slice to export.
	 * \return Returns an error code.
	 **/
	SL2_ERRORS ExportAsBmpWithMasks( CImage &_iImage, const std::u16string &_sPath, SL2_OPTIONS &_oOptions, size_t _sMip, size_t _sArray, size_t _sFace, size_t _sSlice ) {
		if ( (_oOptions.iBmpSaveOption & 0x1) == BMP_SAVE_RLE ) { return SL2_E_FEATURENOTSUPPORTED; }


		const CFormat::SL2_BEST_INTERNAL_FORMAT bifFormats[] = {
			{ CFormat::FindFormatDataByVulkan( SL2_VK_FORMAT_R8G8B8_UNORM ), },
			{ CFormat::FindFormatDataByVulkan( SL2_VK_FORMAT_B8G8R8_UNORM ), },
			{ CFormat::FindFormatDataByVulkan( SL2_VK_FORMAT_R8G8B8_SRGB ), },

			{ CFormat::FindFormatDataByVulkan( SL2_VK_FORMAT_R8G8B8A8_UNORM ), },
			{ CFormat::FindFormatDataByVulkan( SL2_VK_FORMAT_R8G8B8A8_SRGB ), },
			{ CFormat::FindFormatDataByVulkan( SL2_VK_FORMAT_B8G8R8A8_UNORM ), },
			{ CFormat::FindFormatDataByVulkan( SL2_VK_FORMAT_B8G8R8A8_SRGB ), },
			{ CFormat::FindFormatDataByVulkan( SL2_VK_FORMAT_A8B8G8R8_UNORM_PACK32 ), },
			{ CFormat::FindFormatDataByVulkan( SL2_VK_FORMAT_A8B8G8R8_SRGB_PACK32 ), },

			{ CFormat::FindFormatDataByVulkan( SL2_VK_FORMAT_R4G4B4A4_UNORM_PACK16 ), },
			{ CFormat::FindFormatDataByVulkan( SL2_VK_FORMAT_B4G4R4A4_UNORM_PACK16 ), },
			{ CFormat::FindFormatDataByVulkan( SL2_VK_FORMAT_A4R4G4B4_UNORM_PACK16 ), },
			{ CFormat::FindFormatDataByVulkan( SL2_VK_FORMAT_A4B4G4R4_UNORM_PACK16 ), },

			{ CFormat::FindFormatDataByVulkan( SL2_VK_FORMAT_R5G6B5_UNORM_PACK16 ), },
			{ CFormat::FindFormatDataByVulkan( SL2_VK_FORMAT_B5G6R5_UNORM_PACK16 ), },
			{ CFormat::FindFormatDataByVulkan( SL2_VK_FORMAT_R5G5B5A1_UNORM_PACK16 ), },
			{ CFormat::FindFormatDataByVulkan( SL2_VK_FORMAT_A1B5G5R5_UNORM_PACK16_KHR ), },
			{ CFormat::FindFormatDataByVulkan( SL2_VK_FORMAT_A1R5G5B5_UNORM_PACK16 ), },
			{ CFormat::FindFormatDataByVulkan( SL2_VK_FORMAT_A4B4G4R4_UNORM_PACK16 ), },
		};
		const CFormat::SL2_BEST_INTERNAL_FORMAT * pkifdUseMe = nullptr;
		CFormat::SL2_BEST_INTERNAL_FORMAT bifTmp;
		if ( _oOptions.vkBmpFormat != SL2_VK_FORMAT_UNDEFINED ) {
			bifTmp.pkifdFormat = CFormat::FindFormatDataByVulkan( _oOptions.vkBmpFormat );
			pkifdUseMe = &bifTmp;
		}
		else {
			pkifdUseMe = CFormat::FindBestFormat( _iImage.Format(), bifFormats, SL2_ELEMENTS( bifFormats ) );
			if ( !pkifdUseMe ) {
				return SL2_E_BADFORMAT;
			}
		}


		std::vector<uint8_t> vFile;
		sl2::CStream sFile( vFile );

		std::vector<uint8_t> vConverted;
		SL2_ERRORS eError = _iImage.ConvertToFormat( pkifdUseMe->pkifdFormat, _sMip, _sArray, _sFace, vConverted, true );
		if ( eError != SL2_E_SUCCESS ) { return eError; }

		uint32_t ui32FormatBytes = pkifdUseMe->pkifdFormat->ui32BlockSizeInBits / 8;
		uint32_t ui32Stride = ui32FormatBytes * _iImage.GetMipmaps()[_sMip]->Width();
		if ( ui32Stride & 0x3 ) {
			ui32Stride = (ui32Stride & ~3) + 4;
		}
		uint64_t ui64SrcStride = CFormat::GetRowSize( pkifdUseMe->pkifdFormat, _iImage.GetMipmaps()[_sMip]->Width() );

		CImage::SL2_BITMAPFILEHEADER bmfhHeader = { 0x4D42 };
		bmfhHeader.ui32Offset = sizeof( CImage::SL2_BITMAPFILEHEADER ) + sizeof( CImage::SL2_BITMAPINFOHEADER ) + sizeof( CImage::SL2_BITMAPCOLORMASK );
		bmfhHeader.ui32Size = bmfhHeader.ui32Offset + ui32Stride * _iImage.GetMipmaps()[_sMip]->Height();
		CImage::SL2_BITMAPINFOHEADER bihInfo = { sizeof( CImage::SL2_BITMAPINFOHEADER ) };
		bihInfo.ui32Width = _iImage.GetMipmaps()[_sMip]->Width();
		bihInfo.ui32Height = _iImage.GetMipmaps()[_sMip]->Height();
		bihInfo.ui16Planes = 1;
		bihInfo.ui32ImageSize = ui32Stride * _iImage.GetMipmaps()[_sMip]->Height();
		bihInfo.ui32PixelsPerMeterX = static_cast<uint32_t>(std::round( 96.0 * 39.37007874015748096 ));
		bihInfo.ui32PixelsPerMeterY = static_cast<uint32_t>(std::round( 96.0 * 39.37007874015748096 ));
		bihInfo.ui32Compression = BI_BITFIELDS;
		bihInfo.ui16BitsPerPixel = uint16_t( ui32FormatBytes * 8 );

		try {
			if ( sFile.Write( reinterpret_cast<const uint8_t *>(&bmfhHeader), sizeof( bmfhHeader ) ) != sizeof( bmfhHeader ) ) { return SL2_E_OUTOFMEMORY; }
			if ( sFile.Write( reinterpret_cast<const uint8_t *>(&bihInfo), sizeof( bihInfo ) ) != sizeof( bihInfo ) ) { return SL2_E_OUTOFMEMORY; }

			CImage::SL2_BITMAPCOLORMASK bcmMask;
			bcmMask.ui32Red = ((1 << pkifdUseMe->pkifdFormat->ui8RBits) - 1) << pkifdUseMe->pkifdFormat->ui8RShift;
			bcmMask.ui32Green = ((1 << pkifdUseMe->pkifdFormat->ui8GBits) - 1) << pkifdUseMe->pkifdFormat->ui8GShift;
			bcmMask.ui32Blue = ((1 << pkifdUseMe->pkifdFormat->ui8BBits) - 1) << pkifdUseMe->pkifdFormat->ui8BShift;
			bcmMask.ui32Alpha = ((1 << pkifdUseMe->pkifdFormat->ui8ABits) - 1) << pkifdUseMe->pkifdFormat->ui8AShift;
			if ( !_oOptions.bBmpHasAlpha || !pkifdUseMe->pkifdFormat->ui8ABits ) { bcmMask.ui32Alpha = 0; }
			if ( sFile.Write( reinterpret_cast<const uint8_t *>(&bcmMask), sizeof( bcmMask ) ) != sizeof( bcmMask ) ) { return SL2_E_OUTOFMEMORY; }

			std::vector<uint8_t> vRow;
			vRow.resize( ui32Stride );

			uint64_t ui64Page = ui64SrcStride * bihInfo.ui32Height;
			// _sSlice
			for ( uint32_t Y = 0; Y < bihInfo.ui32Height; ++Y ) {
				std::memset( &vRow[0], 0, vRow.size() );
				uint8_t * pui8Dst = &vRow[0];
				const uint8_t * pui8Src = &vConverted[ui64Page*_sSlice+ui32Stride*Y];
				std::memcpy( pui8Dst, pui8Src, ui32FormatBytes * _iImage.GetMipmaps()[_sMip]->Width() );

				if ( sFile.Write( vRow.data(), vRow.size() ) != vRow.size() ) { return SL2_E_OUTOFMEMORY; }
			}


			{
				CStdFile sfFile;
				if ( !sfFile.Create( _sPath.c_str() ) ) {
					return SL2_E_INVALIDWRITEPERMISSIONS;
				}
				if ( !sfFile.WriteToFile( vFile ) ) {
					return SL2_E_FILEWRITEERROR;
				}
			}

			return SL2_E_SUCCESS;

		}
		catch ( ... ) { return SL2_E_OUTOFMEMORY; }
	}

	/**
	 * Exports as EXR.
	 * 
	 * \param _iImage The image to export.
	 * \param _sPath The path to which to export _iImage.
	 * \param _oOptions Export options.
	 * \return Returns an error code.
	 **/
	SL2_ERRORS ExportAsExr( CImage &_iImage, const std::u16string &_sPath, SL2_OPTIONS &_oOptions ) {
		if ( _iImage.Mipmaps() == 1 && _iImage.ArraySize() == 1 && _iImage.Faces() == 1 && _iImage.Depth() == 1 ) {
			return ExportAsExr( _iImage, _sPath, _oOptions, 0, 0, 0, 0 );
		}
		else {
			wchar_t szBuffer[64];
			std::u16string sRoot = CUtilities::GetFilePath( _sPath ) + CUtilities::NoExtension( _sPath );
			for ( uint32_t M = 0; M < _iImage.Mipmaps(); ++M ) {
				for ( uint32_t A = 0; A < _iImage.ArraySize(); ++A ) {
					for ( uint32_t F = 0; F < _iImage.Faces(); ++F ) {
						for ( uint32_t D = 0; D < _iImage.Depth(); ++D ) {
							wchar_t * pwcBuf = szBuffer;
							if ( _iImage.Mipmaps() > 1 ) { pwcBuf += ::wsprintfW( pwcBuf, L"_M%.2u", M ); }
							if ( _iImage.ArraySize() > 1 ) { pwcBuf += ::wsprintfW( pwcBuf, L"_A%.2u", A ); }
							if ( _iImage.Faces() > 1 ) { pwcBuf += ::wsprintfW( pwcBuf, L"_F%.2u", F ); }
							if ( _iImage.Depth() > 1 ) { pwcBuf += ::wsprintfW( pwcBuf, L"_D%.2u", D ); }
							pwcBuf += ::wsprintfW( pwcBuf, L".exr" );
							//::wsprintfW( szBuffer, L"_M%.2u_A%.2u_F%.2u_D%.2u.png", M, A, F, D );
							SL2_ERRORS eErr = ExportAsExr( _iImage, CUtilities::Append( sRoot, szBuffer ), _oOptions, M, A, F, D );
							if ( eErr != SL2_E_SUCCESS ) { return eErr; }
						}
					}
				}
			}
		}
		return SL2_E_SUCCESS;
	}

    /**
	 * Exports as EXR.
	 * 
	 * \param _iImage The image to export.
	 * \param _sPath The path to which to export _iImage.
	 * \param _oOptions Export options.
	 * \param _sMip The mipmap level to export.
	 * \param _sArray The array index to export.
	 * \param _sFace The face to export.
	 * \param _sSlice The slice to export.
	 * \return Returns an error code.
	 **/
	SL2_ERRORS ExportAsExr( CImage &_iImage, const std::u16string &_sPath, SL2_OPTIONS &_oOptions, size_t _sMip, size_t _sArray, size_t _sFace, size_t _sSlice ) {
		SL2_VKFORMAT fFormat;
		FREE_IMAGE_TYPE fitType;
		if ( CFormat::CountChannels( _iImage.Format() ) == 1 ) {
			fFormat = SL2_VK_FORMAT_R32_SFLOAT;
			fitType = FIT_FLOAT;
		}
		else if ( _iImage.Format()->ui8ABits ) {
			fFormat = SL2_VK_FORMAT_R32G32B32A32_SFLOAT;
			fitType = FIT_RGBAF;
		}
		else {
			fFormat = SL2_VK_FORMAT_R32G32B32_SFLOAT;
			fitType = FIT_RGBF;
		}

		std::vector<uint8_t> vConverted;
		SL2_ERRORS eError = _iImage.ConvertToFormat( CFormat::FindFormatDataByVulkan( fFormat ), _sMip, _sArray, _sFace, vConverted, true );
		if ( eError != SL2_E_SUCCESS ) { return eError; }


		CImage::SL2_FREEIMAGE_ALLOCATET fiImage( fitType, _iImage.GetMipmaps()[_sMip]->Width(), _iImage.GetMipmaps()[_sMip]->Height() );
		if ( !fiImage.pbBitmap ) { return SL2_E_OUTOFMEMORY; }


		size_t sPitch = CFormat::GetRowSize( CFormat::FindFormatDataByVulkan( fFormat ), _iImage.GetMipmaps()[_sMip]->Width() );
		uint32_t ui32Slice = uint32_t( sPitch * _iImage.GetMipmaps()[_sMip]->Height() * _sSlice );
		uint32_t ui32Height = _iImage.GetMipmaps()[_sMip]->Height();
		uint32_t ui32Width = _iImage.GetMipmaps()[_sMip]->Width();
		for ( uint32_t H = 0; H < ui32Height; ++H ) {
			BYTE * pui8Bits = ::FreeImage_GetScanLine( fiImage.pbBitmap, int( H ) );
			uint8_t * pui8Src = vConverted.data() + ui32Slice + sPitch * H;
			switch ( fitType ) {
				case FIT_FLOAT : {
					for ( uint32_t X = 0; X < ui32Width; ++X ) {
						float * prgbDst = reinterpret_cast<float *>(pui8Bits) + X;
						const float * pRgb = reinterpret_cast<float *>(pui8Src) + X;
						(*prgbDst) = (*pRgb);
					}
					break;	// FIT_FLOAT
				}

				case FIT_RGBF : {
					for ( uint32_t X = 0; X < ui32Width; ++X ) {
						FIRGBF * prgbDst = reinterpret_cast<FIRGBF *>(pui8Bits) + X;
						const CFormat::SL2_RGB * pRgb = reinterpret_cast<CFormat::SL2_RGB *>(pui8Src) + X;
						/*prgbDst->red = pRgb->fRgb[SL2_PC_R];
						prgbDst->green = pRgb->fRgb[SL2_PC_G];
						prgbDst->blue = pRgb->fRgb[SL2_PC_B];*/

						prgbDst->red = static_cast<float>(CUtilities::sRGBtoLinear( std::pow( pRgb->fRgb[SL2_PC_R], 2.2 ) ));
						prgbDst->green = static_cast<float>(CUtilities::sRGBtoLinear( std::pow( pRgb->fRgb[SL2_PC_G], 2.2 ) ));
						prgbDst->blue = static_cast<float>(CUtilities::sRGBtoLinear( std::pow( pRgb->fRgb[SL2_PC_B], 2.2 ) ));
					}
					break;	// FIT_RGB16
				}
				case FIT_RGBAF : {
					for ( uint32_t X = 0; X < ui32Width; ++X ) {
						FIRGBAF * prgbDst = reinterpret_cast<FIRGBAF *>(pui8Bits) + X;
						const CFormat::SL2_RGBA * pRgb = reinterpret_cast<CFormat::SL2_RGBA *>(pui8Src) + X;
						/*prgbDst->red = pRgb->fRgba[SL2_PC_R];
						prgbDst->green = pRgb->fRgba[SL2_PC_G];
						prgbDst->blue = pRgb->fRgba[SL2_PC_B];*/

						prgbDst->red = static_cast<float>(CUtilities::sRGBtoLinear( std::pow( pRgb->fRgba[SL2_PC_R], 2.2 ) ));
						prgbDst->green = static_cast<float>(CUtilities::sRGBtoLinear( std::pow( pRgb->fRgba[SL2_PC_G], 2.2 ) ));
						prgbDst->blue = static_cast<float>(CUtilities::sRGBtoLinear( std::pow( pRgb->fRgba[SL2_PC_B], 2.2 ) ));
						prgbDst->alpha = pRgb->fRgba[SL2_PC_A];
					}
					break;	// FIT_RGBA16
				}
			}
		}
		/*if ( fitType == FIT_RGBF || fitType == FIT_RGBAF ) {
			::FreeImage_AdjustGamma( fiImage.pbBitmap, 1.0 );
		}*/


		CImage::SL2_FREE_IMAGE fiBuffer;
		if ( !fiBuffer.pmMemory ) { return SL2_E_OUTOFMEMORY; }

		if ( _oOptions.bEmbedColorProfile && _iImage.OutputColorSpace().size() && static_cast<size_t>(static_cast<long>(_iImage.OutputColorSpace().size())) == _iImage.OutputColorSpace().size() && static_cast<long>(_iImage.OutputColorSpace().size()) > 0 ) {
			if ( !::FreeImage_CreateICCProfile( fiImage.pbBitmap, static_cast<void *>(const_cast<uint8_t *>(_iImage.OutputColorSpace().data())), static_cast<long>(_iImage.OutputColorSpace().size()) ) ) { return SL2_E_OUTOFMEMORY; }
		}

		if ( !::FreeImage_SaveToMemory( FIF_EXR, fiImage.pbBitmap, fiBuffer.pmMemory, _oOptions.iExrSaveOption ) ) {
			return SL2_E_OUTOFMEMORY;
		}
		BYTE * pbData = nullptr;
		DWORD dwSize = 0;
		if ( !::FreeImage_AcquireMemory( fiBuffer.pmMemory, &pbData, &dwSize ) ) {
			return SL2_E_INTERNALERROR;
		}
		try {
			vConverted.resize( dwSize );
		}
		catch ( ... ) {
			return SL2_E_OUTOFMEMORY;
		}
		std::memcpy( vConverted.data(), pbData, dwSize );
		{
			CStdFile sfFile;
			if ( !sfFile.Create( _sPath.c_str() ) ) {
				return SL2_E_INVALIDWRITEPERMISSIONS;
			}
			if ( !sfFile.WriteToFile( vConverted ) ) {
				return SL2_E_FILEWRITEERROR;
			}
		}
		return SL2_E_SUCCESS;
	}

    /**
	 * Exports as J2K.
	 * 
	 * \param _iImage The image to export.
	 * \param _sPath The path to which to export _iImage.
	 * \param _oOptions Export options.
	 * \return Returns an error code.
	 **/
	SL2_ERRORS ExportAsJ2k( CImage &_iImage, const std::u16string &_sPath, SL2_OPTIONS &_oOptions ) {
		if ( _iImage.Mipmaps() == 1 && _iImage.ArraySize() == 1 && _iImage.Faces() == 1 && _iImage.Depth() == 1 ) {
			return ExportAsJ2k( _iImage, _sPath, _oOptions, 0, 0, 0, 0 );
		}
		else {
			wchar_t szBuffer[64];
			std::u16string sRoot = CUtilities::GetFilePath( _sPath ) + CUtilities::NoExtension( _sPath );
			for ( uint32_t M = 0; M < _iImage.Mipmaps(); ++M ) {
				for ( uint32_t A = 0; A < _iImage.ArraySize(); ++A ) {
					for ( uint32_t F = 0; F < _iImage.Faces(); ++F ) {
						for ( uint32_t D = 0; D < _iImage.Depth(); ++D ) {
							wchar_t * pwcBuf = szBuffer;
							if ( _iImage.Mipmaps() > 1 ) { pwcBuf += ::wsprintfW( pwcBuf, L"_M%.2u", M ); }
							if ( _iImage.ArraySize() > 1 ) { pwcBuf += ::wsprintfW( pwcBuf, L"_A%.2u", A ); }
							if ( _iImage.Faces() > 1 ) { pwcBuf += ::wsprintfW( pwcBuf, L"_F%.2u", F ); }
							if ( _iImage.Depth() > 1 ) { pwcBuf += ::wsprintfW( pwcBuf, L"_D%.2u", D ); }
							pwcBuf += ::wsprintfW( pwcBuf, L".j2k" );
							//::wsprintfW( szBuffer, L"_M%.2u_A%.2u_F%.2u_D%.2u.png", M, A, F, D );
							SL2_ERRORS eErr = ExportAsJ2k( _iImage, CUtilities::Append( sRoot, szBuffer ), _oOptions, M, A, F, D );
							if ( eErr != SL2_E_SUCCESS ) { return eErr; }
						}
					}
				}
			}
		}
		return SL2_E_SUCCESS;
	}

	/**
	 * Exports as J2K.
	 * 
	 * \param _iImage The image to export.
	 * \param _sPath The path to which to export _iImage.
	 * \param _oOptions Export options.
	 * \param _sMip The mipmap level to export.
	 * \param _sArray The array index to export.
	 * \param _sFace The face to export.
	 * \param _sSlice The slice to export.
	 * \return Returns an error code.
	 **/
	SL2_ERRORS ExportAsJ2k( CImage &_iImage, const std::u16string &_sPath, SL2_OPTIONS &_oOptions, size_t _sMip, size_t _sArray, size_t _sFace, size_t _sSlice ) {
		const CFormat::SL2_BEST_INTERNAL_FORMAT bifFormats[] = {
			{ CFormat::FindFormatDataByVulkan( SL2_VK_FORMAT_R8G8B8_UNORM ),        FIT_BITMAP,     24 },
			{ CFormat::FindFormatDataByVulkan( SL2_VK_FORMAT_R8G8B8_SRGB ),         FIT_BITMAP,     24 },

			{ CFormat::FindFormatDataByVulkan( SL2_VK_FORMAT_R8G8B8A8_UNORM ),      FIT_BITMAP,     32 },
			{ CFormat::FindFormatDataByVulkan( SL2_VK_FORMAT_R8G8B8A8_SRGB ),       FIT_BITMAP,     32 },

			{ CFormat::FindFormatDataByVulkan( SL2_VK_FORMAT_R16_UNORM ),           FIT_UINT16,     16 },
			{ CFormat::FindFormatDataByVulkan( SL2_VK_FORMAT_R16G16B16_UNORM ),     FIT_RGB16,      16 * 3 },
			{ CFormat::FindFormatDataByVulkan( SL2_VK_FORMAT_R16G16B16A16_UNORM ),  FIT_RGBA16,     16 * 4 },
		};
		const CFormat::SL2_BEST_INTERNAL_FORMAT * pkifdUseMe = nullptr;
		if ( _oOptions.vkJ2kFormat != SL2_VK_FORMAT_UNDEFINED ) {
			pkifdUseMe = CFormat::FindBestFormat( CFormat::FindFormatDataByVulkan( _oOptions.vkJ2kFormat ), bifFormats, SL2_ELEMENTS( bifFormats ) );
			if ( !pkifdUseMe ) {
				return SL2_E_BADFORMAT;
			}
		}
		else {
			pkifdUseMe = CFormat::FindBestFormat( _iImage.Format(), bifFormats, SL2_ELEMENTS( bifFormats ) );
			if ( !pkifdUseMe ) {
				return SL2_E_BADFORMAT;
			}
		}

		std::vector<uint8_t> vConverted;
		SL2_ERRORS eError = _iImage.ConvertToFormat( pkifdUseMe->pkifdFormat, _sMip, _sArray, _sFace, vConverted, true );
		if ( eError != SL2_E_SUCCESS ) { return eError; }


		CImage::SL2_FREEIMAGE_ALLOCATET fiImage( static_cast<FREE_IMAGE_TYPE>(pkifdUseMe->sParm0), _iImage.GetMipmaps()[_sMip]->Width(), _iImage.GetMipmaps()[_sMip]->Height(), pkifdUseMe->i32Parm1 );
		if ( !fiImage.pbBitmap ) { return SL2_E_OUTOFMEMORY; }


		size_t sPitch = CFormat::GetRowSize( pkifdUseMe->pkifdFormat, _iImage.GetMipmaps()[_sMip]->Width() );
		uint32_t ui32Slice = uint32_t( sPitch * _iImage.GetMipmaps()[_sMip]->Height() * _sSlice );
		uint32_t ui32Height = _iImage.GetMipmaps()[_sMip]->Height();
		uint32_t ui32Width = _iImage.GetMipmaps()[_sMip]->Width();
		for ( uint32_t H = 0; H < ui32Height; ++H ) {
			BYTE * pui8Bits = ::FreeImage_GetScanLine( fiImage.pbBitmap, int( H ) );
			uint8_t * pui8Src = vConverted.data() + ui32Slice + sPitch * H;
			switch ( pkifdUseMe->pkifdFormat->vfVulkanFormat ) {
				case SL2_VK_FORMAT_R8G8B8_UNORM : {}
				case SL2_VK_FORMAT_R8G8B8_SRGB : {
					for ( uint32_t X = 0; X < _iImage.GetMipmaps()[_sMip]->Width(); ++X ) {
						RGBTRIPLE * prgbDst = reinterpret_cast<RGBTRIPLE *>(pui8Bits) + X;
						const CFormat::SL2_RGB_UNORM * prgbSrc = reinterpret_cast<CFormat::SL2_RGB_UNORM *>(pui8Src) + X;
						prgbDst->rgbtRed = prgbSrc->ui8Rgb[SL2_PC_R];
						prgbDst->rgbtGreen = prgbSrc->ui8Rgb[SL2_PC_G];
						prgbDst->rgbtBlue = prgbSrc->ui8Rgb[SL2_PC_B];
					}
					break;
				}
				case SL2_VK_FORMAT_R8G8B8A8_UNORM : {}
				case SL2_VK_FORMAT_R8G8B8A8_SRGB : {
					for ( uint32_t X = 0; X < _iImage.GetMipmaps()[_sMip]->Width(); ++X ) {
						RGBQUAD * prgbDst = reinterpret_cast<RGBQUAD *>(pui8Bits) + X;
						const CFormat::SL2_RGBA_UNORM * prgbSrc = reinterpret_cast<CFormat::SL2_RGBA_UNORM *>(pui8Src) + X;
						prgbDst->rgbRed = prgbSrc->ui8Rgba[SL2_PC_R];
						prgbDst->rgbGreen = prgbSrc->ui8Rgba[SL2_PC_G];
						prgbDst->rgbBlue = prgbSrc->ui8Rgba[SL2_PC_B];
						prgbDst->rgbReserved = _oOptions.bBmpHasAlpha ? prgbSrc->ui8Rgba[SL2_PC_A] : 0xFF;
					}
					break;
				}
				case SL2_VK_FORMAT_R16_UNORM : {
					for ( uint32_t X = 0; X < ui32Width; ++X ) {
						uint16_t * prgbDst = reinterpret_cast<uint16_t *>(pui8Bits) + X;
						const uint16_t * pRgb = reinterpret_cast<uint16_t *>(pui8Src) + X;
						(*prgbDst) = (*pRgb);
					}
					break;
				}
				case SL2_VK_FORMAT_R16G16B16_UNORM : {
					for ( uint32_t X = 0; X < ui32Width; ++X ) {
						FIRGB16 * prgbDst = reinterpret_cast<FIRGB16 *>(pui8Bits) + X;
						const CFormat::SL2_RGB16_UNORM * pRgb = reinterpret_cast<CFormat::SL2_RGB16_UNORM *>(pui8Src) + X;
						prgbDst->red = pRgb->ui16Rgb[SL2_PC_R];
						prgbDst->green = pRgb->ui16Rgb[SL2_PC_G];
						prgbDst->blue = pRgb->ui16Rgb[SL2_PC_B];
					}
					break;
				}
				case FIT_RGBAF : {
					for ( uint32_t X = 0; X < ui32Width; ++X ) {
						FIRGBA16 * prgbDst = reinterpret_cast<FIRGBA16 *>(pui8Bits) + X;
						const CFormat::SL2_RGBA16_UNORM * pRgb = reinterpret_cast<CFormat::SL2_RGBA16_UNORM *>(pui8Src) + X;
						prgbDst->red = pRgb->ui16Rgba[SL2_PC_R];
						prgbDst->green = pRgb->ui16Rgba[SL2_PC_G];
						prgbDst->blue = pRgb->ui16Rgba[SL2_PC_B];
						prgbDst->alpha = pRgb->ui16Rgba[SL2_PC_A];
					}
					break;
				}
			}
		}


		CImage::SL2_FREE_IMAGE fiBuffer;
		if ( !fiBuffer.pmMemory ) { return SL2_E_OUTOFMEMORY; }

		if ( _oOptions.bEmbedColorProfile && _iImage.OutputColorSpace().size() && static_cast<size_t>(static_cast<long>(_iImage.OutputColorSpace().size())) == _iImage.OutputColorSpace().size() && static_cast<long>(_iImage.OutputColorSpace().size()) > 0 ) {
			if ( !::FreeImage_CreateICCProfile( fiImage.pbBitmap, static_cast<void *>(const_cast<uint8_t *>(_iImage.OutputColorSpace().data())), static_cast<long>(_iImage.OutputColorSpace().size()) ) ) { return SL2_E_OUTOFMEMORY; }
		}

		if ( !::FreeImage_SaveToMemory( FIF_J2K, fiImage.pbBitmap, fiBuffer.pmMemory, _oOptions.iJ2kSaveOption ) ) {
			return SL2_E_OUTOFMEMORY;
		}
		BYTE * pbData = nullptr;
		DWORD dwSize = 0;
		if ( !::FreeImage_AcquireMemory( fiBuffer.pmMemory, &pbData, &dwSize ) ) {
			return SL2_E_INTERNALERROR;
		}
		try {
			vConverted.resize( dwSize );
		}
		catch ( ... ) {
			return SL2_E_OUTOFMEMORY;
		}
		std::memcpy( vConverted.data(), pbData, dwSize );
		{
			CStdFile sfFile;
			if ( !sfFile.Create( _sPath.c_str() ) ) {
				return SL2_E_INVALIDWRITEPERMISSIONS;
			}
			if ( !sfFile.WriteToFile( vConverted ) ) {
				return SL2_E_FILEWRITEERROR;
			}
		}

		return SL2_E_SUCCESS;
	}

    /**
	 * Exports as JP2.
	 * 
	 * \param _iImage The image to export.
	 * \param _sPath The path to which to export _iImage.
	 * \param _oOptions Export options.
	 * \return Returns an error code.
	 **/
	SL2_ERRORS ExportAsJp2( CImage &_iImage, const std::u16string &_sPath, SL2_OPTIONS &_oOptions ) {
		if ( _iImage.Mipmaps() == 1 && _iImage.ArraySize() == 1 && _iImage.Faces() == 1 && _iImage.Depth() == 1 ) {
			return ExportAsJp2( _iImage, _sPath, _oOptions, 0, 0, 0, 0 );
		}
		else {
			wchar_t szBuffer[64];
			std::u16string sRoot = CUtilities::GetFilePath( _sPath ) + CUtilities::NoExtension( _sPath );
			for ( uint32_t M = 0; M < _iImage.Mipmaps(); ++M ) {
				for ( uint32_t A = 0; A < _iImage.ArraySize(); ++A ) {
					for ( uint32_t F = 0; F < _iImage.Faces(); ++F ) {
						for ( uint32_t D = 0; D < _iImage.Depth(); ++D ) {
							wchar_t * pwcBuf = szBuffer;
							if ( _iImage.Mipmaps() > 1 ) { pwcBuf += ::wsprintfW( pwcBuf, L"_M%.2u", M ); }
							if ( _iImage.ArraySize() > 1 ) { pwcBuf += ::wsprintfW( pwcBuf, L"_A%.2u", A ); }
							if ( _iImage.Faces() > 1 ) { pwcBuf += ::wsprintfW( pwcBuf, L"_F%.2u", F ); }
							if ( _iImage.Depth() > 1 ) { pwcBuf += ::wsprintfW( pwcBuf, L"_D%.2u", D ); }
							pwcBuf += ::wsprintfW( pwcBuf, L".jp2" );
							//::wsprintfW( szBuffer, L"_M%.2u_A%.2u_F%.2u_D%.2u.png", M, A, F, D );
							SL2_ERRORS eErr = ExportAsJp2( _iImage, CUtilities::Append( sRoot, szBuffer ), _oOptions, M, A, F, D );
							if ( eErr != SL2_E_SUCCESS ) { return eErr; }
						}
					}
				}
			}
		}
		return SL2_E_SUCCESS;
	}

	/**
	 * Exports as JP2.
	 * 
	 * \param _iImage The image to export.
	 * \param _sPath The path to which to export _iImage.
	 * \param _oOptions Export options.
	 * \param _sMip The mipmap level to export.
	 * \param _sArray The array index to export.
	 * \param _sFace The face to export.
	 * \param _sSlice The slice to export.
	 * \return Returns an error code.
	 **/
	SL2_ERRORS ExportAsJp2( CImage &_iImage, const std::u16string &_sPath, SL2_OPTIONS &_oOptions, size_t _sMip, size_t _sArray, size_t _sFace, size_t _sSlice ) {
		const CFormat::SL2_BEST_INTERNAL_FORMAT bifFormats[] = {
			{ CFormat::FindFormatDataByVulkan( SL2_VK_FORMAT_R8G8B8_UNORM ),        FIT_BITMAP,     24 },
			{ CFormat::FindFormatDataByVulkan( SL2_VK_FORMAT_R8G8B8_SRGB ),         FIT_BITMAP,     24 },

			{ CFormat::FindFormatDataByVulkan( SL2_VK_FORMAT_R8G8B8A8_UNORM ),      FIT_BITMAP,     32 },
			{ CFormat::FindFormatDataByVulkan( SL2_VK_FORMAT_R8G8B8A8_SRGB ),       FIT_BITMAP,     32 },

			{ CFormat::FindFormatDataByVulkan( SL2_VK_FORMAT_R16_UNORM ),           FIT_UINT16,     16 },
			{ CFormat::FindFormatDataByVulkan( SL2_VK_FORMAT_R16G16B16_UNORM ),     FIT_RGB16,      16 * 3 },
			{ CFormat::FindFormatDataByVulkan( SL2_VK_FORMAT_R16G16B16A16_UNORM ),  FIT_RGBA16,     16 * 4 },
		};
		const CFormat::SL2_BEST_INTERNAL_FORMAT * pkifdUseMe = nullptr;
		if ( _oOptions.vkJ2kFormat != SL2_VK_FORMAT_UNDEFINED ) {
			pkifdUseMe = CFormat::FindBestFormat( CFormat::FindFormatDataByVulkan( _oOptions.vkJ2kFormat ), bifFormats, SL2_ELEMENTS( bifFormats ) );
			if ( !pkifdUseMe ) {
				return SL2_E_BADFORMAT;
			}
		}
		else {
			pkifdUseMe = CFormat::FindBestFormat( _iImage.Format(), bifFormats, SL2_ELEMENTS( bifFormats ) );
			if ( !pkifdUseMe ) {
				return SL2_E_BADFORMAT;
			}
		}

		std::vector<uint8_t> vConverted;
		SL2_ERRORS eError = _iImage.ConvertToFormat( pkifdUseMe->pkifdFormat, _sMip, _sArray, _sFace, vConverted, true );
		if ( eError != SL2_E_SUCCESS ) { return eError; }


		CImage::SL2_FREEIMAGE_ALLOCATET fiImage( static_cast<FREE_IMAGE_TYPE>(pkifdUseMe->sParm0), _iImage.GetMipmaps()[_sMip]->Width(), _iImage.GetMipmaps()[_sMip]->Height(), pkifdUseMe->i32Parm1 );
		if ( !fiImage.pbBitmap ) { return SL2_E_OUTOFMEMORY; }


		size_t sPitch = CFormat::GetRowSize( pkifdUseMe->pkifdFormat, _iImage.GetMipmaps()[_sMip]->Width() );
		uint32_t ui32Slice = uint32_t( sPitch * _iImage.GetMipmaps()[_sMip]->Height() * _sSlice );
		uint32_t ui32Height = _iImage.GetMipmaps()[_sMip]->Height();
		uint32_t ui32Width = _iImage.GetMipmaps()[_sMip]->Width();
		for ( uint32_t H = 0; H < ui32Height; ++H ) {
			BYTE * pui8Bits = ::FreeImage_GetScanLine( fiImage.pbBitmap, int( H ) );
			uint8_t * pui8Src = vConverted.data() + ui32Slice + sPitch * H;
			switch ( pkifdUseMe->pkifdFormat->vfVulkanFormat ) {
				case SL2_VK_FORMAT_R8G8B8_UNORM : {}
				case SL2_VK_FORMAT_R8G8B8_SRGB : {
					for ( uint32_t X = 0; X < _iImage.GetMipmaps()[_sMip]->Width(); ++X ) {
						RGBTRIPLE * prgbDst = reinterpret_cast<RGBTRIPLE *>(pui8Bits) + X;
						const CFormat::SL2_RGB_UNORM * prgbSrc = reinterpret_cast<CFormat::SL2_RGB_UNORM *>(pui8Src) + X;
						prgbDst->rgbtRed = prgbSrc->ui8Rgb[SL2_PC_R];
						prgbDst->rgbtGreen = prgbSrc->ui8Rgb[SL2_PC_G];
						prgbDst->rgbtBlue = prgbSrc->ui8Rgb[SL2_PC_B];
					}
					break;
				}
				case SL2_VK_FORMAT_R8G8B8A8_UNORM : {}
				case SL2_VK_FORMAT_R8G8B8A8_SRGB : {
					for ( uint32_t X = 0; X < _iImage.GetMipmaps()[_sMip]->Width(); ++X ) {
						RGBQUAD * prgbDst = reinterpret_cast<RGBQUAD *>(pui8Bits) + X;
						const CFormat::SL2_RGBA_UNORM * prgbSrc = reinterpret_cast<CFormat::SL2_RGBA_UNORM *>(pui8Src) + X;
						prgbDst->rgbRed = prgbSrc->ui8Rgba[SL2_PC_R];
						prgbDst->rgbGreen = prgbSrc->ui8Rgba[SL2_PC_G];
						prgbDst->rgbBlue = prgbSrc->ui8Rgba[SL2_PC_B];
						prgbDst->rgbReserved = _oOptions.bBmpHasAlpha ? prgbSrc->ui8Rgba[SL2_PC_A] : 0xFF;
					}
					break;
				}
				case SL2_VK_FORMAT_R16_UNORM : {
					for ( uint32_t X = 0; X < ui32Width; ++X ) {
						uint16_t * prgbDst = reinterpret_cast<uint16_t *>(pui8Bits) + X;
						const uint16_t * pRgb = reinterpret_cast<uint16_t *>(pui8Src) + X;
						(*prgbDst) = (*pRgb);
					}
					break;
				}
				case SL2_VK_FORMAT_R16G16B16_UNORM : {
					for ( uint32_t X = 0; X < ui32Width; ++X ) {
						FIRGB16 * prgbDst = reinterpret_cast<FIRGB16 *>(pui8Bits) + X;
						const CFormat::SL2_RGB16_UNORM * pRgb = reinterpret_cast<CFormat::SL2_RGB16_UNORM *>(pui8Src) + X;
						prgbDst->red = pRgb->ui16Rgb[SL2_PC_R];
						prgbDst->green = pRgb->ui16Rgb[SL2_PC_G];
						prgbDst->blue = pRgb->ui16Rgb[SL2_PC_B];
					}
					break;
				}
				case FIT_RGBAF : {
					for ( uint32_t X = 0; X < ui32Width; ++X ) {
						FIRGBA16 * prgbDst = reinterpret_cast<FIRGBA16 *>(pui8Bits) + X;
						const CFormat::SL2_RGBA16_UNORM * pRgb = reinterpret_cast<CFormat::SL2_RGBA16_UNORM *>(pui8Src) + X;
						prgbDst->red = pRgb->ui16Rgba[SL2_PC_R];
						prgbDst->green = pRgb->ui16Rgba[SL2_PC_G];
						prgbDst->blue = pRgb->ui16Rgba[SL2_PC_B];
						prgbDst->alpha = pRgb->ui16Rgba[SL2_PC_A];
					}
					break;
				}
			}
		}


		CImage::SL2_FREE_IMAGE fiBuffer;
		if ( !fiBuffer.pmMemory ) { return SL2_E_OUTOFMEMORY; }

		if ( _oOptions.bEmbedColorProfile && _iImage.OutputColorSpace().size() && static_cast<size_t>(static_cast<long>(_iImage.OutputColorSpace().size())) == _iImage.OutputColorSpace().size() && static_cast<long>(_iImage.OutputColorSpace().size()) > 0 ) {
			if ( !::FreeImage_CreateICCProfile( fiImage.pbBitmap, static_cast<void *>(const_cast<uint8_t *>(_iImage.OutputColorSpace().data())), static_cast<long>(_iImage.OutputColorSpace().size()) ) ) { return SL2_E_OUTOFMEMORY; }
		}

		if ( !::FreeImage_SaveToMemory( FIF_JP2, fiImage.pbBitmap, fiBuffer.pmMemory, _oOptions.iJ2kSaveOption ) ) {
			return SL2_E_OUTOFMEMORY;
		}
		BYTE * pbData = nullptr;
		DWORD dwSize = 0;
		if ( !::FreeImage_AcquireMemory( fiBuffer.pmMemory, &pbData, &dwSize ) ) {
			return SL2_E_INTERNALERROR;
		}
		try {
			vConverted.resize( dwSize );
		}
		catch ( ... ) {
			return SL2_E_OUTOFMEMORY;
		}
		std::memcpy( vConverted.data(), pbData, dwSize );
		{
			CStdFile sfFile;
			if ( !sfFile.Create( _sPath.c_str() ) ) {
				return SL2_E_INVALIDWRITEPERMISSIONS;
			}
			if ( !sfFile.WriteToFile( vConverted ) ) {
				return SL2_E_FILEWRITEERROR;
			}
		}

		return SL2_E_SUCCESS;
	}

    /**
	 * Exports as JPG.
	 * 
	 * \param _iImage The image to export.
	 * \param _sPath The path to which to export _iImage.
	 * \param _oOptions Export options.
	 * \return Returns an error code.
	 **/
	SL2_ERRORS ExportAsJpg( CImage &_iImage, const std::u16string &_sPath, SL2_OPTIONS &_oOptions ) {
		if ( _iImage.Mipmaps() == 1 && _iImage.ArraySize() == 1 && _iImage.Faces() == 1 && _iImage.Depth() == 1 ) {
			return ExportAsJpg( _iImage, _sPath, _oOptions, 0, 0, 0, 0 );
		}
		else {
			wchar_t szBuffer[64];
			std::u16string sRoot = CUtilities::GetFilePath( _sPath ) + CUtilities::NoExtension( _sPath );
			std::u16string sExt = CUtilities::GetFileExtension( _sPath );
			for ( uint32_t M = 0; M < _iImage.Mipmaps(); ++M ) {
				for ( uint32_t A = 0; A < _iImage.ArraySize(); ++A ) {
					for ( uint32_t F = 0; F < _iImage.Faces(); ++F ) {
						for ( uint32_t D = 0; D < _iImage.Depth(); ++D ) {
							wchar_t * pwcBuf = szBuffer;
							if ( _iImage.Mipmaps() > 1 ) { pwcBuf += ::wsprintfW( pwcBuf, L"_M%.2u", M ); }
							if ( _iImage.ArraySize() > 1 ) { pwcBuf += ::wsprintfW( pwcBuf, L"_A%.2u", A ); }
							if ( _iImage.Faces() > 1 ) { pwcBuf += ::wsprintfW( pwcBuf, L"_F%.2u", F ); }
							if ( _iImage.Depth() > 1 ) { pwcBuf += ::wsprintfW( pwcBuf, L"_D%.2u", D ); }
							pwcBuf += ::wsprintfW( pwcBuf, L"." );
							//::wsprintfW( szBuffer, L"_M%.2u_A%.2u_F%.2u_D%.2u.png", M, A, F, D );
							SL2_ERRORS eErr = ExportAsJpg( _iImage, CUtilities::Append( sRoot, szBuffer ) + sExt, _oOptions, M, A, F, D );
							if ( eErr != SL2_E_SUCCESS ) { return eErr; }
						}
					}
				}
			}
		}
		return SL2_E_SUCCESS;
	}

	/**
	 * Exports as JPG.
	 * 
	 * \param _iImage The image to export.
	 * \param _sPath The path to which to export _iImage.
	 * \param _oOptions Export options.
	 * \param _sMip The mipmap level to export.
	 * \param _sArray The array index to export.
	 * \param _sFace The face to export.
	 * \param _sSlice The slice to export.
	 * \return Returns an error code.
	 **/
	SL2_ERRORS ExportAsJpg( CImage &_iImage, const std::u16string &_sPath, SL2_OPTIONS &_oOptions, size_t _sMip, size_t _sArray, size_t _sFace, size_t _sSlice ) {
		const CFormat::SL2_BEST_INTERNAL_FORMAT bifFormats[] = {
			{ CFormat::FindFormatDataByVulkan( SL2_VK_FORMAT_R32G32B32_SFLOAT ),        FIT_BITMAP,     24 },
		};
		const CFormat::SL2_BEST_INTERNAL_FORMAT * pkifdUseMe = nullptr;
		pkifdUseMe = CFormat::FindBestFormat( _iImage.Format(), bifFormats, SL2_ELEMENTS( bifFormats ) );
		if ( !pkifdUseMe ) {
			return SL2_E_BADFORMAT;
		}

		std::vector<uint8_t> vConverted;
		SL2_ERRORS eError = _iImage.ConvertToFormat( pkifdUseMe->pkifdFormat, _sMip, _sArray, _sFace, vConverted, true );
		if ( eError != SL2_E_SUCCESS ) { return eError; }


		CImage::SL2_FREEIMAGE_ALLOCATET fiImage( static_cast<FREE_IMAGE_TYPE>(pkifdUseMe->sParm0), _iImage.GetMipmaps()[_sMip]->Width(), _iImage.GetMipmaps()[_sMip]->Height(), pkifdUseMe->i32Parm1 );
		if ( !fiImage.pbBitmap ) { return SL2_E_OUTOFMEMORY; }


		size_t sPitch = CFormat::GetRowSize( pkifdUseMe->pkifdFormat, _iImage.GetMipmaps()[_sMip]->Width() );
		uint32_t ui32Slice = uint32_t( sPitch * _iImage.GetMipmaps()[_sMip]->Height() * _sSlice );
		uint32_t ui32Height = _iImage.GetMipmaps()[_sMip]->Height();
		uint32_t ui32Width = _iImage.GetMipmaps()[_sMip]->Width();
		for ( uint32_t H = 0; H < ui32Height; ++H ) {
			BYTE * pui8Bits = ::FreeImage_GetScanLine( fiImage.pbBitmap, int( H ) );
			uint8_t * pui8Src = vConverted.data() + ui32Slice + sPitch * H;
			for ( uint32_t X = 0; X < _iImage.GetMipmaps()[_sMip]->Width(); ++X ) {
				RGBTRIPLE * prgbDst = reinterpret_cast<RGBTRIPLE *>(pui8Bits) + X;
				const CFormat::SL2_RGB * prgbSrc = reinterpret_cast<CFormat::SL2_RGB *>(pui8Src) + X;
				/*prgbDst->rgbtRed = static_cast<uint8_t>(std::round( CUtilities::sRGBtoLinear( CUtilities::sRGBtoLinear( prgbSrc->ui8Rgb[SL2_PC_R] / 255.0 )) * 255.0 ));
				prgbDst->rgbtGreen = static_cast<uint8_t>(std::round( CUtilities::sRGBtoLinear( CUtilities::sRGBtoLinear( prgbSrc->ui8Rgb[SL2_PC_G] / 255.0 )) * 255.0 ));
				prgbDst->rgbtBlue = static_cast<uint8_t>(std::round( CUtilities::sRGBtoLinear( CUtilities::sRGBtoLinear( prgbSrc->ui8Rgb[SL2_PC_B] / 255.0 )) * 255.0 ));*/

				/*prgbDst->rgbtRed = static_cast<uint8_t>(std::round( CUtilities::sRGBtoLinear( prgbSrc->fRgb[SL2_PC_R] ) * 255.0 ));
				prgbDst->rgbtGreen = static_cast<uint8_t>(std::round( CUtilities::sRGBtoLinear( prgbSrc->fRgb[SL2_PC_G] ) * 255.0 ));
				prgbDst->rgbtBlue = static_cast<uint8_t>(std::round( CUtilities::sRGBtoLinear( prgbSrc->fRgb[SL2_PC_B] ) * 255.0 ));*/

				/*prgbDst->rgbtRed = static_cast<uint8_t>(std::round( std::pow( prgbSrc->fRgb[SL2_PC_R], 2.2 ) * 255.0 ));
				prgbDst->rgbtGreen = static_cast<uint8_t>(std::round( std::pow( prgbSrc->fRgb[SL2_PC_G], 2.2 ) * 255.0 ));
				prgbDst->rgbtBlue = static_cast<uint8_t>(std::round( std::pow( prgbSrc->fRgb[SL2_PC_B], 2.2 ) * 255.0 ));*/

				prgbDst->rgbtRed = static_cast<uint8_t>(std::round( prgbSrc->fRgb[SL2_PC_R] * 255.0 ));
				prgbDst->rgbtGreen = static_cast<uint8_t>(std::round( prgbSrc->fRgb[SL2_PC_G] * 255.0 ));
				prgbDst->rgbtBlue = static_cast<uint8_t>(std::round( prgbSrc->fRgb[SL2_PC_B] * 255.0 ));
			}
		}
		//::FreeImage_AdjustGamma( fiImage.pbBitmap, 1.0 / 2.2 );


		CImage::SL2_FREE_IMAGE fiBuffer;
		if ( !fiBuffer.pmMemory ) { return SL2_E_OUTOFMEMORY; }

		if ( _oOptions.bEmbedColorProfile && _iImage.OutputColorSpace().size() && static_cast<size_t>(static_cast<long>(_iImage.OutputColorSpace().size())) == _iImage.OutputColorSpace().size() && static_cast<long>(_iImage.OutputColorSpace().size()) > 0 ) {
			if ( !::FreeImage_CreateICCProfile( fiImage.pbBitmap, static_cast<void *>(const_cast<uint8_t *>(_iImage.OutputColorSpace().data())), static_cast<long>(_iImage.OutputColorSpace().size()) ) ) { return SL2_E_OUTOFMEMORY; }
		}

		if ( !::FreeImage_SaveToMemory( FIF_JPEG, fiImage.pbBitmap, fiBuffer.pmMemory, _oOptions.iJpgSaveOption ) ) {
			return SL2_E_OUTOFMEMORY;
		}
		BYTE * pbData = nullptr;
		DWORD dwSize = 0;
		if ( !::FreeImage_AcquireMemory( fiBuffer.pmMemory, &pbData, &dwSize ) ) {
			return SL2_E_INTERNALERROR;
		}
		try {
			vConverted.resize( dwSize );
		}
		catch ( ... ) {
			return SL2_E_OUTOFMEMORY;
		}
		std::memcpy( vConverted.data(), pbData, dwSize );
		{
			CStdFile sfFile;
			if ( !sfFile.Create( _sPath.c_str() ) ) {
				return SL2_E_INVALIDWRITEPERMISSIONS;
			}
			if ( !sfFile.WriteToFile( vConverted ) ) {
				return SL2_E_FILEWRITEERROR;
			}
		}

		return SL2_E_SUCCESS;
	}

    /**
	 * Exports as DDS.
	 * 
	 * \param _iImage The image to export.
	 * \param _sPath The path to which to export _iImage.
	 * \param _oOptions Export options.
	 * \return Returns an error code.
	 **/
	SL2_ERRORS ExportAsDds( CImage &_iImage, const std::u16string &_sPath, SL2_OPTIONS &_oOptions ) {
		// Is the format supported?
		const sl2::CDds::SL2_FORMAT_DATA * pfdDdsData = nullptr;
        
		if ( _iImage.Format()->dfDxFormat != SL2_DXGI_FORMAT_UNKNOWN ) {
			pfdDdsData = sl2::CDds::DxgiIsSupported( _iImage.Format()->pcDxName );
		}
		if ( nullptr == pfdDdsData && _iImage.Format()->kifInternalFormat != SL2_GL_INVALID ) {
			pfdDdsData = sl2::CDds::FindByOgl( _iImage.Format()->pcOglInternalFormat, _iImage.Format()->pcOglType, _iImage.Format()->pcOglBaseInternalFormat );
		}
		if ( nullptr == pfdDdsData ) {
			return SL2_E_BADFORMAT;
		}
		std::vector<uint8_t> vBuffer;
		sl2::CStream sFile( vBuffer );


		if ( !sFile.WriteI32( 0x20534444 ) ) {       // DDS .
			return SL2_E_OUTOFMEMORY;
		}

		sl2::CDds::SL2_DDS_HEADER dhHeader = {
			.ui32Size                                           = sizeof( sl2::CDds::SL2_DDS_HEADER ),
			.ui32Flags                                          = SL2_DF_CAPS | SL2_DF_HEIGHT | SL2_DF_WIDTH | SL2_DF_PIXELFORMAT,
			.ui32Height                                         = _iImage.Height(),
			.ui32Width                                          = _iImage.Width(),
			.ui32PitchOrLinearSize                              = 0,
			.ui32Depth                                          = 0,
			.ui32MipMapCount                                    = static_cast<uint32_t>(_iImage.Mipmaps()),
			.dpPixelFormat                                      = {
				.ui32Size                                       = sizeof( sl2::CDds::SL2_DDS_PIXELFORMAT ),
				.ui32Flags                                      = 0,
				.ui32FourCC                                     = 0,
				.ui32RGBBitCount                                = 0,
				.ui32RBitMask                                   = 0,
				.ui32GBitMask                                   = 0,
				.ui32BBitMask                                   = 0,
				.ui32ABitMask                                   = 0,
			},
			.ui32Caps                                           = SL2_DDSCAPS_TEXTURE,
			.ui32Caps2                                          = 0,
			.ui32Caps3                                          = 0,
			.ui32Caps4                                          = 0,
			.ui32Reserved2                                      = 0,
		};

		sl2::SL2_TEXTURE_TYPES ttTexType = _iImage.TextureType();
		if ( _iImage.Depth() > 1 ) {
			if ( _iImage.ArraySize() != 1 ) { return SL2_E_INVALIDDATA; }
			if ( _iImage.Faces() != 1 ) { return SL2_E_INVALIDDATA; }
			ttTexType = SL2_TT_3D;
		}
		else if ( ttTexType == SL2_TT_CUBE ) {
			if ( (_iImage.Faces() % 6) != 0 ) { return SL2_E_INVALIDDATA; }
		}
		else if ( _iImage.Faces() != 1 ) { return SL2_E_INVALIDDATA; }

		// ui32PitchOrLinearSize
		// The pitch or number of bytes per scan line in an uncompressed texture; the total number of bytes in the top level texture for a compressed texture.
		if ( _iImage.Format()->bCompressed ) {
			size_t sTmp = sl2::CFormat::GetFormatSize( _iImage.Format(), _iImage.Width(), _iImage.Height(), 1 );
			if ( static_cast<size_t>(static_cast<uint32_t>(sTmp)) != sTmp ) { return SL2_E_UNSUPPORTEDSIZE; }
			dhHeader.ui32PitchOrLinearSize = static_cast<uint32_t>(sTmp);
			dhHeader.ui32Flags |= SL2_DF_LINEARSIZE;                        // Required when pitch is provided for a compressed texture.
		}
		else {
			size_t sTmp = sl2::CFormat::GetRowSize_NoPadding( _iImage.Format(), _iImage.Width() );
			if ( static_cast<size_t>(static_cast<uint32_t>(sTmp)) != sTmp ) { return SL2_E_UNSUPPORTEDSIZE; }
			dhHeader.ui32PitchOrLinearSize = static_cast<uint32_t>(sTmp);
			dhHeader.ui32Flags |= SL2_DF_PITCH;                             // Required when pitch is provided for an uncompressed texture.
		}

		// ui32Depth
		// Depth of a volume texture (in pixels), otherwise unused.
		if ( _iImage.Depth() > 1 || ttTexType == SL2_TT_3D ) {
			dhHeader.ui32Depth = _iImage.Depth();
			dhHeader.ui32Flags |= SL2_DF_DEPTH;                             // Required in a depth texture.
		}

		// DDSD_MIPMAPCOUNT	Required in a mipmapped texture.
		if ( dhHeader.ui32MipMapCount > 1 ) {
			dhHeader.ui32Flags |= SL2_DF_MIPMAPCOUNT;                       // Required in a mipmapped texture.
			dhHeader.ui32Caps |= SL2_DDSCAPS_COMPLEX;                       // Optional; must be used on any file that contains more than one surface (a mipmap, a cubic environment map, or mipmapped volume texture).
			dhHeader.ui32Caps |= SL2_DDSCAPS_MIPMAP;                        // Optional; should be used for a mipmap.
		}

		// ui32Caps
		if ( dhHeader.ui32MipMapCount > 1 || _iImage.ArraySize() > 1 || _iImage.Faces() > 1 ) {
			dhHeader.ui32Caps |= SL2_DDSCAPS_COMPLEX;                       // Optional; must be used on any file that contains more than one surface (a mipmap, a cubic environment map, or mipmapped volume texture).
		}

		// ui32Caps2
		if ( ttTexType == SL2_TT_CUBE ) {
			dhHeader.ui32Caps2 |= SL2_DDSCAPS2_CUBEMAP;                     // Required for a cube map.
			dhHeader.ui32Caps2 |= SL2_DDSCAPS2_CUBEMAP_POSITIVEX;           // Required when these surfaces are stored in a cube map.
			dhHeader.ui32Caps2 |= SL2_DDSCAPS2_CUBEMAP_NEGATIVEX;           // Required when these surfaces are stored in a cube map.
			dhHeader.ui32Caps2 |= SL2_DDSCAPS2_CUBEMAP_POSITIVEY;           // Required when these surfaces are stored in a cube map.
			dhHeader.ui32Caps2 |= SL2_DDSCAPS2_CUBEMAP_NEGATIVEY;           // Required when these surfaces are stored in a cube map.
			dhHeader.ui32Caps2 |= SL2_DDSCAPS2_CUBEMAP_POSITIVEZ;           // Required when these surfaces are stored in a cube map.
			dhHeader.ui32Caps2 |= SL2_DDSCAPS2_CUBEMAP_NEGATIVEZ;           // Required when these surfaces are stored in a cube map.
		}
		if ( _iImage.Depth() > 1 ) {
			dhHeader.ui32Caps2 |= SL2_DDSCAPS2_VOLUME;                      // Required for a volume texture.
		}
		if ( (dhHeader.ui32Caps2 & (SL2_DDSCAPS2_CUBEMAP | SL2_DDSCAPS2_VOLUME)) == (SL2_DDSCAPS2_CUBEMAP | SL2_DDSCAPS2_VOLUME) ) {
			return SL2_E_INVALIDDATA;
		}


		if ( pfdDdsData->dfFormat != CDds::SL2_DXGI_FORMAT_UNKNOWN ) {
			// Extended header.
			sl2::CDds::SL2_DDS_HEADER_DXT10 dhdHeaderEx = {
				.ui32DxgiFormat                                             = static_cast<uint32_t>(pfdDdsData->dfFormat),
				.ui32ResourceDimension                                      = 0,
				.ui32MiscFlag                                               = static_cast<uint32_t>((dhHeader.ui32Caps2 & SL2_DDSCAPS2_CUBEMAP) ? SL2_DDS_RESOURCE_MISC_TEXTURECUBE : 0),
				.ui32ArraySize                                              = static_cast<uint32_t>(_iImage.ArraySize() * _iImage.Faces()),
				.ui32MiscFlags2                                             = 0
			};

			if ( dhdHeaderEx.ui32MiscFlag == SL2_DDS_RESOURCE_MISC_TEXTURECUBE ) {
				dhdHeaderEx.ui32ArraySize /= 6;
			}

			// Extended header.
			dhHeader.dpPixelFormat.ui32FourCC = SL2_MAKEFOURCC( 'D', 'X', '1', '0' );
			dhHeader.dpPixelFormat.ui32Flags |= SL2_DPFF_FOURCC;
            
			// Resource type.
			switch ( ttTexType ) {
				case SL2_TT_1D : {
					dhdHeaderEx.ui32ResourceDimension = SL2_DDS_DIMENSION_TEXTURE1D;
					break;
				}
				case SL2_TT_2D : {
					dhdHeaderEx.ui32ResourceDimension = SL2_DDS_DIMENSION_TEXTURE2D;
					break;
				}
				case SL2_TT_3D : {
					dhdHeaderEx.ui32ResourceDimension = SL2_DDS_DIMENSION_TEXTURE3D;
					break;
				}
			}

			// Misc2 flags: Alpha.
			if ( _iImage.IsFullyOpaque() ) {
				dhdHeaderEx.ui32MiscFlags2 = (dhdHeaderEx.ui32MiscFlags2 & ~0b111) | SL2_DDS_ALPHA_MODE_OPAQUE;
			}
			else if ( _iImage.IsPremultiplied() ) {
				dhdHeaderEx.ui32MiscFlags2 = (dhdHeaderEx.ui32MiscFlags2 & ~0b111) | SL2_DDS_ALPHA_MODE_PREMULTIPLIED;
			}
			else {
				dhdHeaderEx.ui32MiscFlags2 = (dhdHeaderEx.ui32MiscFlags2 & ~0b111) | SL2_DDS_ALPHA_MODE_STRAIGHT;
			}

			if ( !sFile.Write( reinterpret_cast<uint8_t *>(&dhHeader), sizeof( dhHeader ) ) ) {
				return SL2_E_OUTOFMEMORY;
			}
			if ( !sFile.Write( reinterpret_cast<uint8_t *>(&dhdHeaderEx), sizeof( dhdHeaderEx ) ) ) {
				return SL2_E_OUTOFMEMORY;
			}
		}
		else {
			dhHeader.dpPixelFormat.ui32Flags = pfdDdsData->pfFormatFlags;
			if ( dhHeader.dpPixelFormat.ui32Flags & SL2_DPFF_FOURCC ) {
				dhHeader.dpPixelFormat.ui32FourCC = pfdDdsData->fD3dFormat;
			}
			else {
				if ( dhHeader.dpPixelFormat.ui32Flags & (SL2_DPFF_RGB | SL2_DPFF_LUMINANCE | SL2_DPFF_YUV) ) {
					dhHeader.dpPixelFormat.ui32RGBBitCount = pfdDdsData->ui8BitsPerBlock;
					dhHeader.dpPixelFormat.ui32RBitMask = ((1 << _iImage.Format()->ui8RBits) - 1) << _iImage.Format()->ui8RShift;
					dhHeader.dpPixelFormat.ui32GBitMask = ((1 << _iImage.Format()->ui8GBits) - 1) << _iImage.Format()->ui8GShift;
					dhHeader.dpPixelFormat.ui32BBitMask = ((1 << _iImage.Format()->ui8BBits) - 1) << _iImage.Format()->ui8BShift;
					dhHeader.dpPixelFormat.ui32ABitMask = ((1 << _iImage.Format()->ui8ABits) - 1) << _iImage.Format()->ui8AShift;
				}
			}
			if ( !sFile.Write( reinterpret_cast<uint8_t *>(&dhHeader), sizeof( dhHeader ) ) ) {
				return SL2_E_OUTOFMEMORY;
			}
		}

		// Add the texel data.
        
		// For each/face.
		for ( size_t A = 0; A < _iImage.ArraySize(); ++A ) {
			for ( size_t F = 0; F < _iImage.Faces(); ++F ) {
				// For each level.
				for ( size_t M = 0; M < _iImage.Mipmaps(); ++M ) {
					if ( dhHeader.ui32Flags & SL2_DF_LINEARSIZE ) {
						size_t sSrcPitch = sl2::CFormat::GetFormatSize( _iImage.Format(), _iImage.GetMipmaps()[M]->Width(), _iImage.GetMipmaps()[M]->Height(), 1 );
						// For each slice.
						for ( uint32_t D = 0; D < _iImage.GetMipmaps()[M]->Depth(); ++D ) {
							const uint8_t * pui8Src = _iImage.Data( M, D, A, F );
							if ( !sFile.Write( pui8Src, sSrcPitch ) ) { return SL2_E_OUTOFMEMORY; }
						}
					}
					else {
						size_t sSrcPitch = sl2::CFormat::GetRowSize( _iImage.Format(), _iImage.GetMipmaps()[M]->Width() );
						size_t sDstPitch = sl2::CFormat::GetRowSize_NoPadding( _iImage.Format(), _iImage.GetMipmaps()[M]->Width() );
						// For each slice.
						for ( uint32_t D = 0; D < _iImage.GetMipmaps()[M]->Depth(); ++D ) {
							const uint8_t * pui8Src = _iImage.Data( M, D, A, F );
							// For each row.
							for ( uint32_t H = 0; H < _iImage.GetMipmaps()[M]->Height(); ++H ) {
								if ( !sFile.Write( pui8Src, sDstPitch ) ) {
									return SL2_E_OUTOFMEMORY;
								}
								pui8Src += sSrcPitch;
							}
						}
					}
				}

			}
		}


		{
			CStdFile sfFile;
			if ( !sfFile.Create( _sPath.c_str() ) ) {
				return SL2_E_INVALIDWRITEPERMISSIONS;
			}
			if ( !sfFile.WriteToFile( vBuffer ) ) {
				return SL2_E_FILEWRITEERROR;
			}
		}

		return SL2_E_SUCCESS;
	}

    /**
	 * Exports as KTX 1.
	 * 
	 * \param _iImage The image to export.
	 * \param _sPath The path to which to export _iImage.
	 * \param _oOptions Export options.
	 * \return Returns an error code.
	 **/
	SL2_ERRORS ExportAsKtx1( CImage &_iImage, const std::u16string &_sPath, SL2_OPTIONS &_oOptions ) {
		//if ( _iImage.Format()->vfVulkanFormat == SL2_VK_FORMAT_UNDEFINED
		if ( _iImage.Format()->kifInternalFormat == SL2_GL_INVALID || _iImage.Format()->ktType == SL2_KT_GL_INVALID || _iImage.Format()->kbifBaseInternalFormat == SL2_KBIF_GL_INVALID ) { return SL2_E_BADFORMAT; }

		::ktxTextureCreateInfo tciCreateInfo;
		tciCreateInfo.glInternalformat = _iImage.Format()->kifInternalFormat;
		tciCreateInfo.vkFormat = _iImage.Format()->vfVulkanFormat;
		tciCreateInfo.pDfd = nullptr;
		tciCreateInfo.baseWidth = _iImage.Width();
		tciCreateInfo.baseHeight = _iImage.Height();
		tciCreateInfo.baseDepth = _iImage.Depth();
		switch ( _iImage.TextureType() ) {
			case SL2_TT_1D : {
				tciCreateInfo.numDimensions = 1;
				break;
			}
			case SL2_TT_3D : {
				tciCreateInfo.numDimensions = 3;
				break;
			}
			default : { tciCreateInfo.numDimensions = 2; }
		}
		tciCreateInfo.numLevels = static_cast<ktx_uint32_t>(_iImage.Mipmaps());
		tciCreateInfo.numLayers = static_cast<ktx_uint32_t>(_iImage.ArraySize());
		tciCreateInfo.numFaces = static_cast<ktx_uint32_t>(_iImage.Faces());
		tciCreateInfo.isArray = _iImage.ArraySize() > 1 ? KTX_TRUE : KTX_FALSE;
		tciCreateInfo.generateMipmaps = KTX_FALSE;

		sl2::CKtxTexture<ktxTexture1> kt1Tex;
		::KTX_error_code ecErr = ::ktxTexture1_Create( &tciCreateInfo, KTX_TEXTURE_CREATE_ALLOC_STORAGE, kt1Tex.HandlePointer() );
		if ( KTX_SUCCESS != ecErr || kt1Tex.Handle() == nullptr ) { return SL2_E_OUTOFMEMORY; }

		if ( _iImage.Format()->bCompressed ) {
			//(*kt1Tex).glInternalformat = _iImage.Format()->kifInternalFormat;
			//(*kt1Tex).glType = 0;
			//(*kt1Tex).glBaseInternalformat = _iImage.Format()->kbifBaseInternalFormat;
			//(*kt1Tex).glFormat = 0;
		}
		else {
			(*kt1Tex).glInternalformat = _iImage.Format()->kifInternalFormat;
			(*kt1Tex).glType = _iImage.Format()->ktType;
			(*kt1Tex).glBaseInternalformat = _iImage.Format()->kbifBaseInternalFormat;
		}

		// For each/face.
		for ( size_t A = 0; A < _iImage.ArraySize(); ++A ) {
			for ( size_t F = 0; F < _iImage.Faces(); ++F ) {
				// For each level.
				for ( size_t M = 0; M < _iImage.Mipmaps(); ++M ) {
					if ( _iImage.Format()->bCompressed ) {
						if ( ecErr != KTX_SUCCESS ) {
							size_t sPageSize = CFormat::GetFormatSize( _iImage.Format(), _iImage.GetMipmaps()[M]->Width(), _iImage.GetMipmaps()[M]->Height(), 1 );
							for ( uint32_t D = 0; D < _iImage.GetMipmaps()[M]->Depth(); ++D ) {
								ecErr = ktxTexture_SetImageFromMemory( ktxTexture( kt1Tex.Handle() ), static_cast<ktx_uint32_t>(M), static_cast<ktx_uint32_t>(A), static_cast<ktx_uint32_t>(D), _iImage.Data( M, D, A, F ), static_cast<ktx_size_t>(sPageSize) );
								if ( ecErr != KTX_SUCCESS ) { return SL2_E_OUTOFMEMORY; }
							}
						}
					}
					else {
						size_t sDstPitch = sl2::CFormat::GetRowSize_NoPadding( _iImage.Format(), _iImage.GetMipmaps()[M]->Width() );
						size_t sPitch = sl2::CFormat::GetRowSize( _iImage.Format(), _iImage.GetMipmaps()[M]->Width() );
						size_t sSrcPageSize = sPitch * _iImage.GetMipmaps()[M]->Height();
						std::vector<uint8_t> vTmp;
						try {
							vTmp.resize( sDstPitch * _iImage.GetMipmaps()[M]->Height() );
						}
						catch ( ... ) { return SL2_E_OUTOFMEMORY; }
						for ( uint32_t D = 0; D < _iImage.GetMipmaps()[M]->Depth(); ++D ) {
							uint8_t * pui8Dst = vTmp.data();
							for ( uint32_t H = 0; H < _iImage.GetMipmaps()[M]->Height(); ++H ) {
								std::memcpy( pui8Dst, _iImage.Data( M, D, A, F ) + sPitch * H, sDstPitch );
								pui8Dst += sDstPitch;
							}
							ecErr = ktxTexture_SetImageFromMemory( ktxTexture( kt1Tex.Handle() ), static_cast<ktx_uint32_t>(M), static_cast<ktx_uint32_t>(A), static_cast<ktx_uint32_t>(D), vTmp.data(), static_cast<ktx_size_t>(vTmp.size()) );
							if ( ecErr != KTX_SUCCESS ) { return SL2_E_OUTOFMEMORY; }
						}
					}
				}
			}
		}

		// Write to memory.
		ktx_uint8_t * fileData = nullptr;
		ktx_size_t fileSize = 0;
		ecErr = ktxTexture_WriteToMemory( ktxTexture( kt1Tex.Handle() ), &fileData, &fileSize );
		SL2_ERRORS eRet = SL2_E_SUCCESS;
		if ( ecErr != KTX_SUCCESS ) { eRet = SL2_E_OUTOFMEMORY; }
		else {
			CStdFile sfFile;
			if ( !sfFile.Create( _sPath.c_str() ) ) {
				// Free the memory allocated by ktxTexture_WriteToMemory.
				eRet = SL2_E_INVALIDWRITEPERMISSIONS;
			}
			else if ( !sfFile.WriteToFile( fileData, fileSize ) ) {
				// Free the memory allocated by ktxTexture_WriteToMemory.
				eRet = SL2_E_FILEWRITEERROR;
			}
			// Free the memory allocated by ktxTexture_WriteToMemory.
			std::free( fileData );
		}

		return eRet;
	}

    /**
	 * Exports as PVR.
	 * 
	 * \param _iImage The image to export.
	 * \param _sPath The path to which to export _iImage.
	 * \param _oOptions Export options.
	 * \return Returns an error code.
	 **/
	SL2_ERRORS ExportAsPvr( CImage &_iImage, const std::u16string &_sPath, SL2_OPTIONS &_oOptions ) {
		if ( _iImage.Format()->tlvtVariableType == PVRTLVT_Invalid ) { return SL2_E_BADFORMAT; }
        
		PVRTexLibChannelName tlcnChans[4];
		PVRTuint64 ui64PvrFormat = CFormat::FormatToPvrFormat( (*_iImage.Format()), tlcnChans );

		// Create texture header creation parameters.
		PVRHeader_CreateParams cpCreateParms = {};
		cpCreateParms.pixelFormat = ui64PvrFormat;
		cpCreateParms.width = _iImage.Width();
		cpCreateParms.height = _iImage.Height();
		cpCreateParms.depth = _iImage.Depth();
		cpCreateParms.numMipMaps = static_cast<PVRTuint32>(_iImage.Mipmaps());
		cpCreateParms.numArrayMembers = static_cast<PVRTuint32>(_iImage.ArraySize());
		cpCreateParms.numFaces = static_cast<PVRTuint32>(_iImage.Faces());
		if ( _iImage.OutputColorSpace().size() ) {
			cpCreateParms.colourSpace = CFormat::TransferFunc( _iImage.OutputColorSpaceType() ).tlcsPvrColorSpace;
		}
		else {
			cpCreateParms.colourSpace = (_iImage.TargetGamma() == 0.0 || _iImage.TargetGamma() == 1.0) ? PVRTLCS_Linear : PVRTLCS_sRGB;
		}
		cpCreateParms.channelType = _iImage.Format()->tlvtVariableType;
		cpCreateParms.preMultiplied = _iImage.IsPremultiplied();

		// Create the tTexture header.
		sl2::CImage::SL2_PVRTEXTUREHEADER thHeader( ::PVRTexLib_CreateTextureHeader( &cpCreateParms ) );
		if ( !thHeader.thHeader ) { return SL2_E_OUTOFMEMORY; }
		//::PVRTexLib_SetTextureVulkanFormat( thHeader.thHeader, 56 );

		// Create the tTexture object with the header.
		sl2::CImage::SL2_PVRTEXTURE tTexture( ::PVRTexLib_CreateTexture( thHeader.thHeader, nullptr ) );
        

		if ( _iImage.Format()->bCompressed ) {
			for ( PVRTuint32 M = 0; M < cpCreateParms.numMipMaps; ++M ) {
				for ( PVRTuint32 A = 0; A < cpCreateParms.numArrayMembers; ++A ) {
					for ( PVRTuint32 F = 0; F < cpCreateParms.numFaces; ++F ) {
						size_t sDstPitch = sl2::CFormat::GetRowSize_NoPadding( _iImage.Format(), _iImage.GetMipmaps()[M]->Width() );
						size_t sPitch = sl2::CFormat::GetRowSize( _iImage.Format(), _iImage.GetMipmaps()[M]->Width() );
						size_t sSrcPageSize = sPitch * _iImage.GetMipmaps()[M]->Height();
						for ( uint32_t D = 0; D < _iImage.GetMipmaps()[M]->Depth(); ++D ) {
							PVRTuint8 * pui8Data = static_cast<PVRTuint8*>(::PVRTexLib_GetTextureDataPtr( tTexture.tTexture, M, A, F, D ));
							for ( uint32_t H = 0; H < _iImage.GetMipmaps()[M]->Height(); ++H ) {
								std::memcpy( pui8Data, _iImage.Data( M, D, A, F ) + sPitch * H, sDstPitch );
								pui8Data += sDstPitch;
							}
						}
					}
				}
			}
		}
		else {
			for ( PVRTuint32 M = 0; M < cpCreateParms.numMipMaps; ++M ) {
				for ( PVRTuint32 A = 0; A < cpCreateParms.numArrayMembers; ++A ) {
					for ( PVRTuint32 F = 0; F < cpCreateParms.numFaces; ++F ) {
						size_t sPageSize = CFormat::GetFormatSize( _iImage.Format(), _iImage.GetMipmaps()[M]->Width(), _iImage.GetMipmaps()[M]->Height(), _iImage.GetMipmaps()[M]->Depth() );
						PVRTuint8 * pui8Data = static_cast<PVRTuint8*>(::PVRTexLib_GetTextureDataPtr( tTexture.tTexture, M, A, F, 0 ));
						std::memcpy( pui8Data, _iImage.Data( M, 0, A, F ), static_cast<ktx_size_t>(sPageSize) );
					}
				}
			}
		}

		SL2_ERRORS eErr = SL2_E_SUCCESS;
		// Save the tTexture as a .PVR file with mipmaps and array layers
		if ( !::PVRTexLib_SaveTextureToFile( tTexture.tTexture, sl2::CUtilities::Utf16ToUtf8( _sPath.c_str() ).c_str() ) ) {
			std::filesystem::path pAsciiPath, pAsciiFile;
			if ( !CUtilities::CreateAsciiPath( _sPath, pAsciiPath, pAsciiFile ) ) { return SL2_E_OUTOFMEMORY; }
			bool bDirCreated = false;
			try {
				std::filesystem::create_directories( pAsciiPath );
				bDirCreated = true;
				std::filesystem::path pTmpOut = pAsciiPath / pAsciiFile;

				std::u16string u16Tmp = sl2::CUtilities::XStringToU16String( pTmpOut.c_str(), pTmpOut.native().size() );
				if ( !::PVRTexLib_SaveTextureToFile( tTexture.tTexture, sl2::CUtilities::Utf16ToUtf8( u16Tmp.c_str() ).c_str() ) ) { eErr = SL2_E_FILEWRITEERROR; }
				std::filesystem::rename( pTmpOut.c_str(), _sPath.c_str() );
				std::filesystem::remove_all( pAsciiPath );
			}
			catch ( ... ) {
				if ( bDirCreated ) {
					try { std::filesystem::remove_all( pAsciiPath ); }
					catch ( ... ) {}
				}
				return SL2_E_FILEWRITEERROR;
			}
		}
		return eErr;
	}

    /**
	 * Exports as TGA.
	 * 
	 * \param _iImage The image to export.
	 * \param _sPath The path to which to export _iImage.
	 * \param _oOptions Export options.
	 * \return Returns an error code.
	 **/
	SL2_ERRORS ExportAsTga( CImage &_iImage, const std::u16string &_sPath, SL2_OPTIONS &_oOptions ) {
		if ( _iImage.Mipmaps() == 1 && _iImage.ArraySize() == 1 && _iImage.Faces() == 1 && _iImage.Depth() == 1 ) {
			return ExportAsTga( _iImage, _sPath, _oOptions, 0, 0, 0, 0 );
		}
		else {
			wchar_t szBuffer[64];
			std::u16string sRoot = CUtilities::GetFilePath( _sPath ) + CUtilities::NoExtension( _sPath );
			for ( uint32_t M = 0; M < _iImage.Mipmaps(); ++M ) {
				for ( uint32_t A = 0; A < _iImage.ArraySize(); ++A ) {
					for ( uint32_t F = 0; F < _iImage.Faces(); ++F ) {
						for ( uint32_t D = 0; D < _iImage.Depth(); ++D ) {
							wchar_t * pwcBuf = szBuffer;
							if ( _iImage.Mipmaps() > 1 ) { pwcBuf += ::wsprintfW( pwcBuf, L"_M%.2u", M ); }
							if ( _iImage.ArraySize() > 1 ) { pwcBuf += ::wsprintfW( pwcBuf, L"_A%.2u", A ); }
							if ( _iImage.Faces() > 1 ) { pwcBuf += ::wsprintfW( pwcBuf, L"_F%.2u", F ); }
							if ( _iImage.Depth() > 1 ) { pwcBuf += ::wsprintfW( pwcBuf, L"_D%.2u", D ); }
							pwcBuf += ::wsprintfW( pwcBuf, L".tga" );
							//::wsprintfW( szBuffer, L"_M%.2u_A%.2u_F%.2u_D%.2u.png", M, A, F, D );
							SL2_ERRORS eErr = ExportAsTga( _iImage, CUtilities::Append( sRoot, szBuffer ), _oOptions, M, A, F, D );
							if ( eErr != SL2_E_SUCCESS ) { return eErr; }
						}
					}
				}
			}
		}
		return SL2_E_SUCCESS;
	}

	/**
	 * Exports as TGA.
	 * 
	 * \param _iImage The image to export.
	 * \param _sPath The path to which to export _iImage.
	 * \param _oOptions Export options.
	 * \param _sMip The mipmap level to export.
	 * \param _sArray The array index to export.
	 * \param _sFace The face to export.
	 * \param _sSlice The slice to export.
	 * \return Returns an error code.
	 **/
	SL2_ERRORS ExportAsTga( CImage &_iImage, const std::u16string &_sPath, SL2_OPTIONS &_oOptions, size_t _sMip, size_t _sArray, size_t _sFace, size_t _sSlice ) {
		const CFormat::SL2_BEST_INTERNAL_FORMAT bifFormats[] = {
			{ CFormat::FindFormatDataByVulkan( SL2_VK_FORMAT_R8G8B8_UNORM ),			FIT_BITMAP, },
			{ CFormat::FindFormatDataByVulkan( SL2_VK_FORMAT_R8G8B8_SRGB ),				FIT_BITMAP, },
			{ CFormat::FindFormatDataByVulkan( SL2_VK_FORMAT_R8G8B8A8_UNORM ),			FIT_BITMAP, },
			{ CFormat::FindFormatDataByVulkan( SL2_VK_FORMAT_R8G8B8A8_SRGB ),			FIT_BITMAP, },

			{ CFormat::FindFormatDataByVulkan( SL2_VK_FORMAT_A1R5G5B5_UNORM_PACK16 ),	FIT_BITMAP, },

			{ CFormat::FindFormatDataByOgl( SL2_GL_LUMINANCE8 ),						FIT_BITMAP },
		};
		const CFormat::SL2_BEST_INTERNAL_FORMAT * pkifdUseMe = nullptr;
		if ( _oOptions.vkTgaFormat != SL2_VK_FORMAT_UNDEFINED ) {
			pkifdUseMe = CFormat::FindBestFormat( CFormat::FindFormatDataByVulkan( _oOptions.vkTgaFormat ), bifFormats, SL2_ELEMENTS( bifFormats ) );
			if ( !pkifdUseMe ) {
				return SL2_E_BADFORMAT;
			}
		}
		else {
			pkifdUseMe = CFormat::FindBestFormat( _iImage.Format(), bifFormats, SL2_ELEMENTS( bifFormats ) );
			if ( !pkifdUseMe ) {
				return SL2_E_BADFORMAT;
			}
		}

		if ( !pkifdUseMe ) { return SL2_E_BADFORMAT; }
		FREE_IMAGE_TYPE fitType = static_cast<FREE_IMAGE_TYPE>(pkifdUseMe->sParm0);
		CImage::SL2_FREEIMAGE_ALLOCATET fiImage( fitType, _iImage.GetMipmaps()[_sMip]->Width(), _iImage.GetMipmaps()[_sMip]->Height(), pkifdUseMe->pkifdFormat->ui32BlockSizeInBits );
		if ( !fiImage.pbBitmap ) { return SL2_E_OUTOFMEMORY; }

		std::vector<uint8_t> vConverted;
		SL2_ERRORS eError = _iImage.ConvertToFormat( pkifdUseMe->pkifdFormat, _sMip, _sArray, _sFace, vConverted, true );
		if ( eError != SL2_E_SUCCESS ) { return eError; }


		size_t sPitch = CFormat::GetRowSize( pkifdUseMe->pkifdFormat, _iImage.GetMipmaps()[_sMip]->Width() );
		uint32_t ui32Slice = uint32_t( sPitch * _iImage.GetMipmaps()[_sMip]->Height() * _sSlice );
		for ( uint32_t H = 0; H < _iImage.GetMipmaps()[_sMip]->Height(); ++H ) {
			BYTE * pui8Bits = ::FreeImage_GetScanLine( fiImage.pbBitmap, int( H ) );
			uint8_t * pui8Src = vConverted.data() + ui32Slice + sPitch * H;
			switch ( fitType ) {
				case FIT_BITMAP : {
					switch ( pkifdUseMe->pkifdFormat->ui32BlockSizeInBits ) {
						case 8 * 1 : {
							for ( uint32_t X = 0; X < _iImage.GetMipmaps()[_sMip]->Width(); ++X ) {
								uint8_t * prgbDst = pui8Bits + X;
								const uint8_t * pui8This = pui8Src + X;
								(*prgbDst) = (*pui8This);
							}
							break;
						}
						case 8 * 2 : {
							for ( uint32_t X = 0; X < _iImage.GetMipmaps()[_sMip]->Width(); ++X ) {
								uint16_t * prgbDst = reinterpret_cast<uint16_t *>(pui8Bits) + X;
								const uint16_t * prgbSrc = reinterpret_cast<uint16_t *>(pui8Src) + X;
								/*prgbDst->red = (*prgbSrc) & 0xFF;
								prgbDst->alpha = (*prgbSrc) >> 8;*/
								(*prgbDst) = (*prgbSrc);
							}
							break;
						}
						case 8 * 3 : {
							for ( uint32_t X = 0; X < _iImage.GetMipmaps()[_sMip]->Width(); ++X ) {
								RGBTRIPLE * prgbDst = reinterpret_cast<RGBTRIPLE *>(pui8Bits) + X;
								const CFormat::SL2_RGB_UNORM * prgbSrc = reinterpret_cast<CFormat::SL2_RGB_UNORM *>(pui8Src) + X;
								prgbDst->rgbtRed = prgbSrc->ui8Rgb[SL2_PC_R];
								prgbDst->rgbtGreen = prgbSrc->ui8Rgb[SL2_PC_G];
								prgbDst->rgbtBlue = prgbSrc->ui8Rgb[SL2_PC_B];
							}
							break;
						}
						case 8 * 4 : {
							for ( uint32_t X = 0; X < _iImage.GetMipmaps()[_sMip]->Width(); ++X ) {
								RGBQUAD * prgbDst = reinterpret_cast<RGBQUAD *>(pui8Bits) + X;
								const CFormat::SL2_RGBA_UNORM * prgbSrc = reinterpret_cast<CFormat::SL2_RGBA_UNORM *>(pui8Src) + X;
								prgbDst->rgbRed = prgbSrc->ui8Rgba[SL2_PC_R];
								prgbDst->rgbGreen = prgbSrc->ui8Rgba[SL2_PC_G];
								prgbDst->rgbBlue = prgbSrc->ui8Rgba[SL2_PC_B];
								prgbDst->rgbReserved = prgbSrc->ui8Rgba[SL2_PC_A];
							}
							break;
						}
					}
					break;
				}
			}
		}


		CImage::SL2_FREE_IMAGE fiBuffer;
		if ( !fiBuffer.pmMemory ) { return SL2_E_OUTOFMEMORY; }

		if ( _oOptions.bEmbedColorProfile && _iImage.OutputColorSpace().size() && static_cast<size_t>(static_cast<long>(_iImage.OutputColorSpace().size())) == _iImage.OutputColorSpace().size() && static_cast<long>(_iImage.OutputColorSpace().size()) > 0 ) {
			if ( !::FreeImage_CreateICCProfile( fiImage.pbBitmap, static_cast<void *>(const_cast<uint8_t *>(_iImage.OutputColorSpace().data())), static_cast<long>(_iImage.OutputColorSpace().size()) ) ) { return SL2_E_OUTOFMEMORY; }
		}

		if ( !::FreeImage_SaveToMemory( FIF_TARGA, fiImage.pbBitmap, fiBuffer.pmMemory, _oOptions.iTgaSaveOption ) ) {
			return SL2_E_OUTOFMEMORY;
		}
        

		BYTE * pbData = nullptr;
		DWORD dwSize = 0;
		if ( !::FreeImage_AcquireMemory( fiBuffer.pmMemory, &pbData, &dwSize ) ) {
			return SL2_E_INTERNALERROR;
		}
		try {
			vConverted.resize( dwSize );
		}
		catch ( ... ) {
			return SL2_E_OUTOFMEMORY;
		}
		std::memcpy( vConverted.data(), pbData, dwSize );
		{
			CStdFile sfFile;
			if ( !sfFile.Create( _sPath.c_str() ) ) {
				return SL2_E_INVALIDWRITEPERMISSIONS;
			}
			if ( !sfFile.WriteToFile( vConverted ) ) {
				return SL2_E_FILEWRITEERROR;
			}
		}

		return SL2_E_SUCCESS;
	}

    /**
	 * Exports as YUV.
	 * 
	 * \param _iImage The image to export.
	 * \param _sPath The path to which to export _iImage.
	 * \param _oOptions Export options.
	 * \return Returns an error code.
	 **/
	SL2_ERRORS ExportAsYuv( CImage &_iImage, const std::u16string &_sPath, SL2_OPTIONS &_oOptions ) {
		if ( _iImage.Mipmaps() == 1 && _iImage.ArraySize() == 1 && _iImage.Faces() == 1 ) {
			return ExportAsYuv( _iImage, _sPath, _oOptions, 0, 0, 0, 0 );
		}
		else {
			wchar_t szBuffer[64];
			std::u16string sRoot = CUtilities::GetFilePath( _sPath ) + CUtilities::NoExtension( _sPath );
			std::u16string u16Ext = CUtilities::GetFileExtension( _sPath );
			for ( uint32_t M = 0; M < _iImage.Mipmaps(); ++M ) {
				for ( uint32_t A = 0; A < _iImage.ArraySize(); ++A ) {
					for ( uint32_t F = 0; F < _iImage.Faces(); ++F ) {
						wchar_t * pwcBuf = szBuffer;
						if ( _iImage.Mipmaps() > 1 ) { pwcBuf += ::wsprintfW( pwcBuf, L"_M%.2u", M ); }
						if ( _iImage.ArraySize() > 1 ) { pwcBuf += ::wsprintfW( pwcBuf, L"_A%.2u", A ); }
						if ( _iImage.Faces() > 1 ) { pwcBuf += ::wsprintfW( pwcBuf, L"_F%.2u", F ); }
						pwcBuf += ::wsprintfW( pwcBuf, L"." );
						std::u16string u16Final = CUtilities::Append( sRoot, szBuffer );
						try { u16Final.append( u16Ext ); }
						catch ( ... ) { return SL2_E_OUTOFMEMORY; }
						//::wsprintfW( szBuffer, L"_M%.2u_A%.2u_F%.2u_D%.2u.png", M, A, F, D );
						SL2_ERRORS eErr = ExportAsYuv( _iImage, u16Final, _oOptions, M, A, F, 0 );
						if ( eErr != SL2_E_SUCCESS ) { return eErr; }
					}
				}
			}
		}
		return SL2_E_SUCCESS;
	}

	/**
	 * Exports as YUV.
	 * 
	 * \param _iImage The image to export.
	 * \param _sPath The path to which to export _iImage.
	 * \param _oOptions Export options.
	 * \param _sMip The mipmap level to export.
	 * \param _sArray The array index to export.
	 * \param _sFace The face to export.
	 * \param _sSlice The slice to export.
	 * \return Returns an error code.
	 **/
	SL2_ERRORS ExportAsYuv( CImage &_iImage, const std::u16string &_sPath, SL2_OPTIONS &_oOptions, size_t _sMip, size_t _sArray, size_t _sFace, size_t _sSlice ) {
		const CFormat::SL2_BEST_INTERNAL_FORMAT bifFormats[] = {
			{ _iImage.Format() },
			{ _oOptions.pkifdYuvFormat },
		};
		const CFormat::SL2_BEST_INTERNAL_FORMAT * pkifdUseMe = nullptr;
		if ( _oOptions.pkifdYuvFormat != nullptr ) {
			pkifdUseMe = CFormat::FindBestFormat( _oOptions.pkifdYuvFormat, bifFormats, SL2_ELEMENTS( bifFormats ) );
			if ( !pkifdUseMe ) {
				return SL2_E_BADFORMAT;
			}
		}
		else {
			pkifdUseMe = CFormat::FindBestFormat( _iImage.Format(), bifFormats, SL2_ELEMENTS( bifFormats ) );
			if ( !pkifdUseMe ) {
				return SL2_E_BADFORMAT;
			}
		}
		if ( !SL2_GET_YUV_FLAG( pkifdUseMe->pkifdFormat->ui32Flags ) ) { return SL2_E_BADFORMAT; }

		std::vector<uint8_t> vConverted;
		SL2_ERRORS eError = _iImage.ConvertToFormat( pkifdUseMe->pkifdFormat, _sMip, _sArray, _sFace, vConverted );
		if ( eError != SL2_E_SUCCESS ) { return eError; }

		uint64_t ui64Size = CFormat::GetFormatSize( pkifdUseMe->pkifdFormat, _iImage.GetMipmaps()[_sMip]->Width(), _iImage.GetMipmaps()[_sMip]->Height(), _iImage.GetMipmaps()[_sMip]->Depth() );
		if ( uint64_t( size_t( ui64Size ) ) != ui64Size ) { return SL2_E_UNSUPPORTEDSIZE; }
		try {
			vConverted.resize( size_t( ui64Size ) );
		}
		catch ( ... ) { return SL2_E_OUTOFMEMORY; }

		{
			CStdFile sfFile;
			if ( !sfFile.Create( _sPath.c_str() ) ) {
				return SL2_E_INVALIDWRITEPERMISSIONS;
			}
			if ( !sfFile.WriteToFile( vConverted ) ) {
				return SL2_E_FILEWRITEERROR;
			}
		}

		return SL2_E_SUCCESS;
	}

    /**
	 * Exports as PBM.
	 * 
	 * \param _iImage The image to export.
	 * \param _sPath The path to which to export _iImage.
	 * \param _oOptions Export options.
	 * \return Returns an error code.
	 **/
	SL2_ERRORS ExportAsPbm( CImage &_iImage, const std::u16string &_sPath, SL2_OPTIONS &_oOptions ) {
		if ( _iImage.Mipmaps() == 1 && _iImage.ArraySize() == 1 && _iImage.Faces() == 1 ) {
			return ExportAsPbm( _iImage, _sPath, _oOptions, 0, 0, 0, 0 );
		}
		else {
			wchar_t szBuffer[64];
			std::u16string sRoot = CUtilities::GetFilePath( _sPath ) + CUtilities::NoExtension( _sPath );
			std::u16string u16Ext = CUtilities::GetFileExtension( _sPath );
			for ( uint32_t M = 0; M < _iImage.Mipmaps(); ++M ) {
				for ( uint32_t A = 0; A < _iImage.ArraySize(); ++A ) {
					for ( uint32_t F = 0; F < _iImage.Faces(); ++F ) {
						for ( uint32_t D = 0; D < _iImage.Depth(); ++D ) {
							wchar_t * pwcBuf = szBuffer;
							if ( _iImage.Mipmaps() > 1 ) { pwcBuf += ::wsprintfW( pwcBuf, L"_M%.2u", M ); }
							if ( _iImage.ArraySize() > 1 ) { pwcBuf += ::wsprintfW( pwcBuf, L"_A%.2u", A ); }
							if ( _iImage.Faces() > 1 ) { pwcBuf += ::wsprintfW( pwcBuf, L"_F%.2u", F ); }
							if ( _iImage.Depth() > 1 ) { pwcBuf += ::wsprintfW( pwcBuf, L"_D%.2u", D ); }
							pwcBuf += ::wsprintfW( pwcBuf, L"." );
							std::u16string u16Final = CUtilities::Append( sRoot, szBuffer );
							try { u16Final.append( u16Ext ); }
							catch ( ... ) { return SL2_E_OUTOFMEMORY; }
							//::wsprintfW( szBuffer, L"_M%.2u_A%.2u_F%.2u_D%.2u.png", M, A, F, D );
							SL2_ERRORS eErr = ExportAsPbm( _iImage, u16Final, _oOptions, M, A, F, 0 );
							if ( eErr != SL2_E_SUCCESS ) { return eErr; }
						}
					}
				}
			}
		}
		return SL2_E_SUCCESS;
	}

	/**
	 * Exports as PBM.
	 * 
	 * \param _iImage The image to export.
	 * \param _sPath The path to which to export _iImage.
	 * \param _oOptions Export options.
	 * \param _sMip The mipmap level to export.
	 * \param _sArray The array index to export.
	 * \param _sFace The face to export.
	 * \param _sSlice The slice to export.
	 * \return Returns an error code.
	 **/
	SL2_ERRORS ExportAsPbm( CImage &_iImage, const std::u16string &_sPath, SL2_OPTIONS &_oOptions, size_t _sMip, size_t _sArray, size_t _sFace, size_t _sSlice ) {
		const CFormat::SL2_BEST_INTERNAL_FORMAT bifFormats[] = {
			{ CFormat::FindFormatDataByOgl( SL2_GL_COLOR_INDEX1_EXT ) },
		};
		const CFormat::SL2_BEST_INTERNAL_FORMAT * pbifUseMe = CFormat::FindBestFormat( _iImage.Format(), bifFormats, SL2_ELEMENTS( bifFormats ) );

		if ( !pbifUseMe ) {
			return SL2_E_BADFORMAT;
		}
		FREE_IMAGE_TYPE fitType = FIT_BITMAP;

		size_t sMax = size_t( 1ULL << pbifUseMe->pkifdFormat->ui32BlockSizeInBits );
		//_iImage.GeneratePalette( uint32_t( sMax ) );
		//if ( _iImage.Palette().Palette().size() > sMax ) { return SL2_E_UNSUPPORTEDSIZE; }
		CPalette pPalette;
		CImage::SL2_FREEIMAGE_ALLOCATET fiImage( FIT_BITMAP, _iImage.GetMipmaps()[_sMip]->Width(), _iImage.GetMipmaps()[_sMip]->Height(), pbifUseMe->pkifdFormat->ui32BlockSizeInBits );
		if ( !fiImage.pbBitmap ) { return SL2_E_OUTOFMEMORY; }

		std::vector<uint8_t> vConverted;
		SL2_ERRORS eError = _iImage.ConvertToFormat( pbifUseMe->pkifdFormat, _sMip, _sArray, _sFace, vConverted, true, true, &pPalette );
		if ( eError != SL2_E_SUCCESS ) { return eError; }

		// Set the palette.
		RGBQUAD * prgbVal = ::FreeImage_GetPalette( fiImage.pbBitmap );
		for ( size_t I = 0; I < pPalette.Palette().size(); ++I ) {
			prgbVal[I].rgbRed = static_cast<BYTE>(std::clamp( std::round( pPalette.Palette()[I].X() * 255.0 ), 0.0, 255.0 ));
			prgbVal[I].rgbGreen = static_cast<BYTE>(std::clamp( std::round( pPalette.Palette()[I].Y() * 255.0 ), 0.0, 255.0 ));
			prgbVal[I].rgbBlue = static_cast<BYTE>(std::clamp( std::round( pPalette.Palette()[I].Z() * 255.0 ), 0.0, 255.0 ));
			prgbVal[I].rgbReserved = static_cast<BYTE>(std::clamp( std::round( pPalette.Palette()[I].W() * 255.0 ), 0.0, 255.0 ));
		}

		size_t sPitch = CFormat::GetRowSize( pbifUseMe->pkifdFormat, _iImage.GetMipmaps()[_sMip]->Width() );
		uint32_t ui32Slice = uint32_t( sPitch * _iImage.GetMipmaps()[_sMip]->Height() * _sSlice );
		for ( uint32_t H = 0; H < _iImage.GetMipmaps()[_sMip]->Height(); ++H ) {
			BYTE * pui8Bits = ::FreeImage_GetScanLine( fiImage.pbBitmap, int( H ) );
			uint8_t * pui8Src = vConverted.data() + ui32Slice + sPitch * H;
			switch ( pbifUseMe->pkifdFormat->ui32BlockSizeInBits ) {
				case 1 : {
					size_t sMask = size_t( (1LL << pbifUseMe->pkifdFormat->ui32BlockSizeInBits) - 1 );
					size_t sSegs = 8 / pbifUseMe->pkifdFormat->ui32BlockSizeInBits;
					for ( uint32_t X = 0; X < _iImage.GetMipmaps()[_sMip]->Width(); ++X ) {
						size_t sOff = X * pbifUseMe->pkifdFormat->ui32BlockSizeInBits / 8L;
						size_t sShift = ((sSegs) - (X & ((sSegs) - 1)) - 1) * pbifUseMe->pkifdFormat->ui32BlockSizeInBits;
						uint8_t * prgbDst = pui8Bits + sOff;
						const uint8_t * pui8This = pui8Src + X;
						(*prgbDst) = (((*pui8This) & sMask) << sShift) | ((*prgbDst) & ~(sMask << sShift));
					}
					break;
				}
				case 8 : {
					for ( uint32_t X = 0; X < _iImage.GetMipmaps()[_sMip]->Width(); ++X ) {
						uint8_t * prgbDst = pui8Bits + X;
						const uint8_t * pui8This = pui8Src + X;
						(*prgbDst) = (*pui8This);
					}
					break;
				}
			}
		}

		CImage::SL2_FREE_IMAGE fiBuffer;
		if ( !fiBuffer.pmMemory ) { return SL2_E_OUTOFMEMORY; }


		if ( !::FreeImage_SaveToMemory( FIF_PBM, fiImage.pbBitmap, fiBuffer.pmMemory, _oOptions.iPbmSaveOption ) ) {
			return SL2_E_OUTOFMEMORY;
		}
        

		BYTE * pbData = nullptr;
		DWORD dwSize = 0;
		if ( !::FreeImage_AcquireMemory( fiBuffer.pmMemory, &pbData, &dwSize ) ) {
			return SL2_E_INTERNALERROR;
		}
		try {
			vConverted.resize( dwSize );
		}
		catch ( ... ) {
			return SL2_E_OUTOFMEMORY;
		}
		std::memcpy( vConverted.data(), pbData, dwSize );
		{
			CStdFile sfFile;
			if ( !sfFile.Create( _sPath.c_str() ) ) {
				return SL2_E_INVALIDWRITEPERMISSIONS;
			}
			if ( !sfFile.WriteToFile( vConverted ) ) {
				return SL2_E_FILEWRITEERROR;
			}
		}

		return SL2_E_SUCCESS;
	}

	/**
	 * Exports as PGM.
	 * 
	 * \param _iImage The image to export.
	 * \param _sPath The path to which to export _iImage.
	 * \param _oOptions Export options.
	 * \return Returns an error code.
	 **/
	SL2_ERRORS ExportAsPgm( CImage &_iImage, const std::u16string &_sPath, SL2_OPTIONS &_oOptions ) {
		if ( _iImage.Mipmaps() == 1 && _iImage.ArraySize() == 1 && _iImage.Faces() == 1 ) {
			return ExportAsPgm( _iImage, _sPath, _oOptions, 0, 0, 0, 0 );
		}
		else {
			wchar_t szBuffer[64];
			std::u16string sRoot = CUtilities::GetFilePath( _sPath ) + CUtilities::NoExtension( _sPath );
			std::u16string u16Ext = CUtilities::GetFileExtension( _sPath );
			for ( uint32_t M = 0; M < _iImage.Mipmaps(); ++M ) {
				for ( uint32_t A = 0; A < _iImage.ArraySize(); ++A ) {
					for ( uint32_t F = 0; F < _iImage.Faces(); ++F ) {
						for ( uint32_t D = 0; D < _iImage.Depth(); ++D ) {
							wchar_t * pwcBuf = szBuffer;
							if ( _iImage.Mipmaps() > 1 ) { pwcBuf += ::wsprintfW( pwcBuf, L"_M%.2u", M ); }
							if ( _iImage.ArraySize() > 1 ) { pwcBuf += ::wsprintfW( pwcBuf, L"_A%.2u", A ); }
							if ( _iImage.Faces() > 1 ) { pwcBuf += ::wsprintfW( pwcBuf, L"_F%.2u", F ); }
							if ( _iImage.Depth() > 1 ) { pwcBuf += ::wsprintfW( pwcBuf, L"_D%.2u", D ); }
							pwcBuf += ::wsprintfW( pwcBuf, L"." );
							std::u16string u16Final = CUtilities::Append( sRoot, szBuffer );
							try { u16Final.append( u16Ext ); }
							catch ( ... ) { return SL2_E_OUTOFMEMORY; }
							//::wsprintfW( szBuffer, L"_M%.2u_A%.2u_F%.2u_D%.2u.png", M, A, F, D );
							SL2_ERRORS eErr = ExportAsPgm( _iImage, u16Final, _oOptions, M, A, F, 0 );
							if ( eErr != SL2_E_SUCCESS ) { return eErr; }
						}
					}
				}
			}
		}
		return SL2_E_SUCCESS;
	}

	/**
	 * Exports as PGM.
	 * 
	 * \param _iImage The image to export.
	 * \param _sPath The path to which to export _iImage.
	 * \param _oOptions Export options.
	 * \param _sMip The mipmap level to export.
	 * \param _sArray The array index to export.
	 * \param _sFace The face to export.
	 * \param _sSlice The slice to export.
	 * \return Returns an error code.
	 **/
	SL2_ERRORS ExportAsPgm( CImage &_iImage, const std::u16string &_sPath, SL2_OPTIONS &_oOptions, size_t _sMip, size_t _sArray, size_t _sFace, size_t _sSlice ) {
		const CFormat::SL2_BEST_INTERNAL_FORMAT bifFormats[] = {
			{ CFormat::FindFormatDataByOgl( SL2_GL_COLOR_INDEX8_EXT ) },
		};
		const CFormat::SL2_BEST_INTERNAL_FORMAT * pbifUseMe = CFormat::FindBestFormat( _iImage.Format(), bifFormats, SL2_ELEMENTS( bifFormats ) );

		if ( !pbifUseMe ) {
			return SL2_E_BADFORMAT;
		}
		FREE_IMAGE_TYPE fitType = FIT_BITMAP;

		size_t sMax = size_t( 1ULL << pbifUseMe->pkifdFormat->ui32BlockSizeInBits );
		//_iImage.GeneratePalette( uint32_t( sMax ) );
		//if ( _iImage.Palette().Palette().size() > sMax ) { return SL2_E_UNSUPPORTEDSIZE; }
		CPalette pPalette;
		CImage::SL2_FREEIMAGE_ALLOCATET fiImage( FIT_BITMAP, _iImage.GetMipmaps()[_sMip]->Width(), _iImage.GetMipmaps()[_sMip]->Height(), pbifUseMe->pkifdFormat->ui32BlockSizeInBits );
		if ( !fiImage.pbBitmap ) { return SL2_E_OUTOFMEMORY; }

		std::vector<uint8_t> vConverted;
		SL2_ERRORS eError = _iImage.ConvertToFormat( pbifUseMe->pkifdFormat, _sMip, _sArray, _sFace, vConverted, true, true, &pPalette );
		if ( eError != SL2_E_SUCCESS ) { return eError; }

		// Set the palette.
		RGBQUAD * prgbVal = ::FreeImage_GetPalette( fiImage.pbBitmap );
		for ( size_t I = 0; I < pPalette.Palette().size(); ++I ) {
			prgbVal[I].rgbRed = static_cast<BYTE>(std::clamp( std::round( pPalette.Palette()[I].X() * 255.0 ), 0.0, 255.0 ));
			prgbVal[I].rgbGreen = static_cast<BYTE>(std::clamp( std::round( pPalette.Palette()[I].Y() * 255.0 ), 0.0, 255.0 ));
			prgbVal[I].rgbBlue = static_cast<BYTE>(std::clamp( std::round( pPalette.Palette()[I].Z() * 255.0 ), 0.0, 255.0 ));
			prgbVal[I].rgbReserved = static_cast<BYTE>(std::clamp( std::round( pPalette.Palette()[I].W() * 255.0 ), 0.0, 255.0 ));
		}

		size_t sPitch = CFormat::GetRowSize( pbifUseMe->pkifdFormat, _iImage.GetMipmaps()[_sMip]->Width() );
		uint32_t ui32Slice = uint32_t( sPitch * _iImage.GetMipmaps()[_sMip]->Height() * _sSlice );
		for ( uint32_t H = 0; H < _iImage.GetMipmaps()[_sMip]->Height(); ++H ) {
			BYTE * pui8Bits = ::FreeImage_GetScanLine( fiImage.pbBitmap, int( H ) );
			uint8_t * pui8Src = vConverted.data() + ui32Slice + sPitch * H;
			switch ( pbifUseMe->pkifdFormat->ui32BlockSizeInBits ) {
				case 1 : {
					size_t sMask = size_t( (1LL << pbifUseMe->pkifdFormat->ui32BlockSizeInBits) - 1 );
					size_t sSegs = 8 / pbifUseMe->pkifdFormat->ui32BlockSizeInBits;
					for ( uint32_t X = 0; X < _iImage.GetMipmaps()[_sMip]->Width(); ++X ) {
						size_t sOff = X * pbifUseMe->pkifdFormat->ui32BlockSizeInBits / 8L;
						size_t sShift = ((sSegs) - (X & ((sSegs) - 1)) - 1) * pbifUseMe->pkifdFormat->ui32BlockSizeInBits;
						uint8_t * prgbDst = pui8Bits + sOff;
						const uint8_t * pui8This = pui8Src + X;
						(*prgbDst) = (((*pui8This) & sMask) << sShift) | ((*prgbDst) & ~(sMask << sShift));
					}
					break;
				}
				case 8 : {
					for ( uint32_t X = 0; X < _iImage.GetMipmaps()[_sMip]->Width(); ++X ) {
						uint8_t * prgbDst = pui8Bits + X;
						const uint8_t * pui8This = pui8Src + X;
						(*prgbDst) = (*pui8This);
					}
					break;
				}
			}
		}

		CImage::SL2_FREE_IMAGE fiBuffer;
		if ( !fiBuffer.pmMemory ) { return SL2_E_OUTOFMEMORY; }


		if ( !::FreeImage_SaveToMemory( FIF_PGM, fiImage.pbBitmap, fiBuffer.pmMemory, _oOptions.iPgmSaveOption ) ) {
			return SL2_E_OUTOFMEMORY;
		}
        

		BYTE * pbData = nullptr;
		DWORD dwSize = 0;
		if ( !::FreeImage_AcquireMemory( fiBuffer.pmMemory, &pbData, &dwSize ) ) {
			return SL2_E_INTERNALERROR;
		}
		try {
			vConverted.resize( dwSize );
		}
		catch ( ... ) {
			return SL2_E_OUTOFMEMORY;
		}
		std::memcpy( vConverted.data(), pbData, dwSize );
		{
			CStdFile sfFile;
			if ( !sfFile.Create( _sPath.c_str() ) ) {
				return SL2_E_INVALIDWRITEPERMISSIONS;
			}
			if ( !sfFile.WriteToFile( vConverted ) ) {
				return SL2_E_FILEWRITEERROR;
			}
		}

		return SL2_E_SUCCESS;
	}

	/**
	 * Export as ICO.
	 * 
	 * \param _iImage The image to export.
	 * \param _sPath The path to which to export _iImage.
	 * \param _oOptions Export options.
	 * \return Returns an error code.
	 **/
	SL2_ERRORS ExportAsIco( CImage &_iImage, const std::u16string &_sPath, SL2_OPTIONS &_oOptions ) {
		if ( _iImage.Mipmaps() == 1 && _iImage.ArraySize() == 1 && _iImage.Faces() == 1 ) {
			return ExportAsIco( _iImage, _sPath, _oOptions, 0, 0, 0, 0 );
		}
		else {
			wchar_t szBuffer[64];
			std::u16string sRoot = CUtilities::GetFilePath( _sPath ) + CUtilities::NoExtension( _sPath );
			std::u16string u16Ext = CUtilities::GetFileExtension( _sPath );
			for ( uint32_t M = 0; M < _iImage.Mipmaps(); ++M ) {
				for ( uint32_t A = 0; A < _iImage.ArraySize(); ++A ) {
					for ( uint32_t F = 0; F < _iImage.Faces(); ++F ) {
						for ( uint32_t D = 0; D < _iImage.Depth(); ++D ) {
							wchar_t * pwcBuf = szBuffer;
							if ( _iImage.Mipmaps() > 1 ) { pwcBuf += ::wsprintfW( pwcBuf, L"_M%.2u", M ); }
							if ( _iImage.ArraySize() > 1 ) { pwcBuf += ::wsprintfW( pwcBuf, L"_A%.2u", A ); }
							if ( _iImage.Faces() > 1 ) { pwcBuf += ::wsprintfW( pwcBuf, L"_F%.2u", F ); }
							if ( _iImage.Depth() > 1 ) { pwcBuf += ::wsprintfW( pwcBuf, L"_D%.2u", D ); }
							pwcBuf += ::wsprintfW( pwcBuf, L"." );
							std::u16string u16Final = CUtilities::Append( sRoot, szBuffer );
							try { u16Final.append( u16Ext ); }
							catch ( ... ) { return SL2_E_OUTOFMEMORY; }
							//::wsprintfW( szBuffer, L"_M%.2u_A%.2u_F%.2u_D%.2u.png", M, A, F, D );
							SL2_ERRORS eErr = ExportAsIco( _iImage, u16Final, _oOptions, M, A, F, 0 );
							if ( eErr != SL2_E_SUCCESS ) { return eErr; }
						}
					}
				}
			}
		}
		return SL2_E_SUCCESS;
	}

	/**
	 * Export as ICO.
	 * 
	 * \param _iImage The image to export.
	 * \param _sPath The path to which to export _iImage.
	 * \param _oOptions Export options.
	 * \param _sMip The mipmap level to export.
	 * \param _sArray The array index to export.
	 * \param _sFace The face to export.
	 * \param _sSlice The slice to export.
	 * \return Returns an error code.
	 **/
	SL2_ERRORS ExportAsIco( CImage &_iImage, const std::u16string &_sPath, SL2_OPTIONS &_oOptions, size_t _sMip, size_t _sArray, size_t _sFace, size_t _sSlice ) {
		const CFormat::SL2_BEST_INTERNAL_FORMAT bifFormats[] = {
			{ CFormat::FindFormatDataByVulkan( SL2_VK_FORMAT_R8G8B8_UNORM ), },
			{ CFormat::FindFormatDataByVulkan( SL2_VK_FORMAT_R8G8B8_SRGB ), },
			{ CFormat::FindFormatDataByVulkan( SL2_VK_FORMAT_R8G8B8A8_UNORM ), },
			{ CFormat::FindFormatDataByVulkan( SL2_VK_FORMAT_R8G8B8A8_SRGB ), },

			{ CFormat::FindFormatDataByVulkan( SL2_VK_FORMAT_R5G6B5_UNORM_PACK16 ), },

			{ CFormat::FindFormatDataByOgl( SL2_GL_COLOR_INDEX1_EXT ) },
			{ CFormat::FindFormatDataByOgl( SL2_GL_COLOR_INDEX2_EXT ) },
			{ CFormat::FindFormatDataByOgl( SL2_GL_COLOR_INDEX4_EXT ) },
			{ CFormat::FindFormatDataByOgl( SL2_GL_COLOR_INDEX8_EXT ) },
		};
		const CFormat::SL2_BEST_INTERNAL_FORMAT * pkifdUseMe = nullptr;
		CFormat::SL2_BEST_INTERNAL_FORMAT bifTmp;
		if ( _oOptions.vkBmpFormatNoMask != SL2_VK_FORMAT_UNDEFINED ) {
			bifTmp.pkifdFormat = CFormat::FindFormatDataByVulkan( _oOptions.vkBmpFormatNoMask );
			pkifdUseMe = &bifTmp;
		}
		else {
			pkifdUseMe = CFormat::FindBestFormat( _iImage.Format(), bifFormats, SL2_ELEMENTS( bifFormats ) );
			if ( !pkifdUseMe ) {
				return SL2_E_BADFORMAT;
			}
		}

		FREE_IMAGE_TYPE fitType = FIT_BITMAP;

		if ( SL2_GET_IDX_FLAG( pkifdUseMe->pkifdFormat->ui32Flags ) ) {
			return ExportAsIco_Indexed( _iImage, _sPath, _oOptions, _sMip, _sArray, _sFace, _sSlice, pkifdUseMe );
		}
		CImage::SL2_FREEIMAGE_ALLOCATET fiImage( fitType, _iImage.GetMipmaps()[_sMip]->Width(), _iImage.GetMipmaps()[_sMip]->Height(), pkifdUseMe->pkifdFormat->ui32BlockSizeInBits );
		if ( !fiImage.pbBitmap ) { return SL2_E_OUTOFMEMORY; }

		std::vector<uint8_t> vConverted;
		SL2_ERRORS eError = _iImage.ConvertToFormat( pkifdUseMe->pkifdFormat, _sMip, _sArray, _sFace, vConverted, true );
		if ( eError != SL2_E_SUCCESS ) { return eError; }


		size_t sPitch = CFormat::GetRowSize( pkifdUseMe->pkifdFormat, _iImage.GetMipmaps()[_sMip]->Width() );
		uint32_t ui32Slice = uint32_t( sPitch * _iImage.GetMipmaps()[_sMip]->Height() * _sSlice );
		for ( uint32_t H = 0; H < _iImage.GetMipmaps()[_sMip]->Height(); ++H ) {
			BYTE * pui8Bits = ::FreeImage_GetScanLine( fiImage.pbBitmap, int( H ) );
			uint8_t * pui8Src = vConverted.data() + ui32Slice + sPitch * H;
			switch ( fitType ) {
				case FIT_BITMAP : {
					switch ( pkifdUseMe->pkifdFormat->ui32BlockSizeInBits ) {
						case 8 * 1 : {
							for ( uint32_t X = 0; X < _iImage.GetMipmaps()[_sMip]->Width(); ++X ) {
								uint8_t * prgbDst = pui8Bits + X;
								const uint8_t * pui8This = pui8Src + X;
								(*prgbDst) = (*pui8This);
							}
							break;
						}
						case 8 * 2 : {
							for ( uint32_t X = 0; X < _iImage.GetMipmaps()[_sMip]->Width(); ++X ) {
								uint16_t * prgbDst = reinterpret_cast<uint16_t *>(pui8Bits) + X;
								const uint16_t * prgbSrc = reinterpret_cast<uint16_t *>(pui8Src) + X;
								/*prgbDst->red = (*prgbSrc) & 0xFF;
								prgbDst->alpha = (*prgbSrc) >> 8;*/
								(*prgbDst) = (*prgbSrc);
							}
							break;
						}
						case 8 * 3 : {
							for ( uint32_t X = 0; X < _iImage.GetMipmaps()[_sMip]->Width(); ++X ) {
								RGBTRIPLE * prgbDst = reinterpret_cast<RGBTRIPLE *>(pui8Bits) + X;
								const CFormat::SL2_RGB_UNORM * prgbSrc = reinterpret_cast<CFormat::SL2_RGB_UNORM *>(pui8Src) + X;
								prgbDst->rgbtRed = prgbSrc->ui8Rgb[SL2_PC_R];
								prgbDst->rgbtGreen = prgbSrc->ui8Rgb[SL2_PC_G];
								prgbDst->rgbtBlue = prgbSrc->ui8Rgb[SL2_PC_B];
							}
							break;
						}
						case 8 * 4 : {
							for ( uint32_t X = 0; X < _iImage.GetMipmaps()[_sMip]->Width(); ++X ) {
								RGBQUAD * prgbDst = reinterpret_cast<RGBQUAD *>(pui8Bits) + X;
								const CFormat::SL2_RGBA_UNORM * prgbSrc = reinterpret_cast<CFormat::SL2_RGBA_UNORM *>(pui8Src) + X;
								prgbDst->rgbRed = prgbSrc->ui8Rgba[SL2_PC_R];
								prgbDst->rgbGreen = prgbSrc->ui8Rgba[SL2_PC_G];
								prgbDst->rgbBlue = prgbSrc->ui8Rgba[SL2_PC_B];
								prgbDst->rgbReserved = prgbSrc->ui8Rgba[SL2_PC_A];
							}
							break;
						}
					}
					break;
				}
				case FIT_UINT16 : {
					for ( uint32_t X = 0; X < _iImage.GetMipmaps()[_sMip]->Width(); ++X ) {
						uint16_t * prgbDst = reinterpret_cast<uint16_t *>(pui8Bits) + X;
						const uint16_t * prgbSrc = reinterpret_cast<uint16_t *>(pui8Src) + X;
						(*prgbDst) = (*prgbSrc);
					}
					break;
				}
				case FIT_RGB16 : {
					for ( uint32_t X = 0; X < _iImage.GetMipmaps()[_sMip]->Width(); ++X ) {
						FIRGB16 * prgbDst = reinterpret_cast<FIRGB16 *>(pui8Bits) + X;
						const CFormat::SL2_RGB16_UNORM * prgbSrc = reinterpret_cast<CFormat::SL2_RGB16_UNORM *>(pui8Src) + X;
						prgbDst->red = prgbSrc->ui16Rgb[SL2_PC_R];
						prgbDst->green = prgbSrc->ui16Rgb[SL2_PC_G];
						prgbDst->blue = prgbSrc->ui16Rgb[SL2_PC_B];
					}
					break;
				}
				case FIT_RGBA16 : {
					for ( uint32_t X = 0; X < _iImage.GetMipmaps()[_sMip]->Width(); ++X ) {
						FIRGBA16 * prgbDst = reinterpret_cast<FIRGBA16 *>(pui8Bits) + X;
						const CFormat::SL2_RGBA16_UNORM * prgbSrc = reinterpret_cast<CFormat::SL2_RGBA16_UNORM *>(pui8Src) + X;
						prgbDst->red = prgbSrc->ui16Rgba[SL2_PC_R];
						prgbDst->green = prgbSrc->ui16Rgba[SL2_PC_G];
						prgbDst->blue = prgbSrc->ui16Rgba[SL2_PC_B];
						prgbDst->alpha = prgbSrc->ui16Rgba[SL2_PC_A];
					}
					break;
				}
			}
		}


		CImage::SL2_FREE_IMAGE fiBuffer;
		if ( !fiBuffer.pmMemory ) { return SL2_E_OUTOFMEMORY; }

		if ( !::FreeImage_SaveToMemory( FIF_ICO, fiImage.pbBitmap, fiBuffer.pmMemory, _oOptions.iIcoSaveOption ) ) {
			return SL2_E_OUTOFMEMORY;
		}
        

		BYTE * pbData = nullptr;
		DWORD dwSize = 0;
		if ( !::FreeImage_AcquireMemory( fiBuffer.pmMemory, &pbData, &dwSize ) ) {
			return SL2_E_INTERNALERROR;
		}
		try {
			vConverted.resize( dwSize );
		}
		catch ( ... ) {
			return SL2_E_OUTOFMEMORY;
		}
		std::memcpy( vConverted.data(), pbData, dwSize );
		{
			CStdFile sfFile;
			if ( !sfFile.Create( _sPath.c_str() ) ) {
				return SL2_E_INVALIDWRITEPERMISSIONS;
			}
			if ( !sfFile.WriteToFile( vConverted ) ) {
				return SL2_E_FILEWRITEERROR;
			}
		}

		return SL2_E_SUCCESS;
	}

	/**
	 * Exports as ICO.
	 * 
	 * \param _iImage The image to export.
	 * \param _sPath The path to which to export _iImage.
	 * \param _oOptions Export options.
	 * \param _sMip The mipmap level to export.
	 * \param _sArray The array index to export.
	 * \param _sFace The face to export.
	 * \param _sSlice The slice to export.
	 * \param _pbifFormat The target indexed format.
	 * \return Returns an error code.
	 **/
	SL2_ERRORS ExportAsIco_Indexed( CImage &_iImage, const std::u16string &_sPath, SL2_OPTIONS &_oOptions, size_t _sMip, size_t _sArray, size_t _sFace, size_t _sSlice,
		const CFormat::SL2_BEST_INTERNAL_FORMAT * _pbifFormat ) {
		size_t sMax = size_t( 1ULL << _pbifFormat->pkifdFormat->ui32BlockSizeInBits );
		//_iImage.GeneratePalette( uint32_t( sMax ) );
		//if ( _iImage.Palette().Palette().size() > sMax ) { return SL2_E_UNSUPPORTEDSIZE; }
		CPalette pPalette;
		CImage::SL2_FREEIMAGE_ALLOCATET fiImage( FIT_BITMAP, _iImage.GetMipmaps()[_sMip]->Width(), _iImage.GetMipmaps()[_sMip]->Height(), _pbifFormat->pkifdFormat->ui32BlockSizeInBits );
		if ( !fiImage.pbBitmap ) { return SL2_E_OUTOFMEMORY; }

		std::vector<uint8_t> vConverted;
		SL2_ERRORS eError = _iImage.ConvertToFormat( _pbifFormat->pkifdFormat, _sMip, _sArray, _sFace, vConverted, true, true, &pPalette );
		if ( eError != SL2_E_SUCCESS ) { return eError; }

		// Set the palette.
		RGBQUAD * prgbVal = ::FreeImage_GetPalette( fiImage.pbBitmap );
		for ( size_t I = 0; I < pPalette.Palette().size(); ++I ) {
			prgbVal[I].rgbRed = static_cast<BYTE>(std::clamp( std::round( pPalette.Palette()[I].X() * 255.0 ), 0.0, 255.0 ));
			prgbVal[I].rgbGreen = static_cast<BYTE>(std::clamp( std::round( pPalette.Palette()[I].Y() * 255.0 ), 0.0, 255.0 ));
			prgbVal[I].rgbBlue = static_cast<BYTE>(std::clamp( std::round( pPalette.Palette()[I].Z() * 255.0 ), 0.0, 255.0 ));
			prgbVal[I].rgbReserved = static_cast<BYTE>(std::clamp( std::round( pPalette.Palette()[I].W() * 255.0 ), 0.0, 255.0 ));
		}

		size_t sPitch = CFormat::GetRowSize( _pbifFormat->pkifdFormat, _iImage.GetMipmaps()[_sMip]->Width() );
		uint32_t ui32Slice = uint32_t( sPitch * _iImage.GetMipmaps()[_sMip]->Height() * _sSlice );
		for ( uint32_t H = 0; H < _iImage.GetMipmaps()[_sMip]->Height(); ++H ) {
			BYTE * pui8Bits = ::FreeImage_GetScanLine( fiImage.pbBitmap, int( H ) );
			uint8_t * pui8Src = vConverted.data() + ui32Slice + sPitch * H;
			switch ( _pbifFormat->pkifdFormat->ui32BlockSizeInBits ) {
				case 1 : {
					size_t sMask = size_t( (1LL << _pbifFormat->pkifdFormat->ui32BlockSizeInBits) - 1 );
					size_t sSegs = 8 / _pbifFormat->pkifdFormat->ui32BlockSizeInBits;
					for ( uint32_t X = 0; X < _iImage.GetMipmaps()[_sMip]->Width(); ++X ) {
						size_t sOff = X * _pbifFormat->pkifdFormat->ui32BlockSizeInBits / 8L;
						size_t sShift = ((sSegs) - (X & ((sSegs) - 1)) - 1) * _pbifFormat->pkifdFormat->ui32BlockSizeInBits;
						uint8_t * prgbDst = pui8Bits + sOff;
						const uint8_t * pui8This = pui8Src + X;
						(*prgbDst) = (((*pui8This) & sMask) << sShift) | ((*prgbDst) & ~(sMask << sShift));
					}
					break;
				}
				case 2 : {
					size_t sMask = size_t( (1LL << _pbifFormat->pkifdFormat->ui32BlockSizeInBits) - 1 );
					for ( uint32_t X = 0; X < _iImage.GetMipmaps()[_sMip]->Width(); ++X ) {
						uint8_t * prgbDst = pui8Bits + X;
						const uint8_t * pui8This = pui8Src + X;
						(*prgbDst) = (*pui8This) & sMask;
					}
					break;
				}
				case 4 : {
					size_t sMask = size_t( (1LL << _pbifFormat->pkifdFormat->ui32BlockSizeInBits) - 1 );
					size_t sSegs = 8 / _pbifFormat->pkifdFormat->ui32BlockSizeInBits;
					for ( uint32_t X = 0; X < _iImage.GetMipmaps()[_sMip]->Width(); ++X ) {
						size_t sOff = X * _pbifFormat->pkifdFormat->ui32BlockSizeInBits / 8L;
						size_t sShift = uint8_t( !(X & 1L) ) * _pbifFormat->pkifdFormat->ui32BlockSizeInBits;
						//size_t sShift = ((sSegs) - (X & ((sSegs) - 1)) - 1) * _pbifFormat->pkifdFormat->ui32BlockSizeInBits;
						uint8_t * prgbDst = pui8Bits + sOff;
						const uint8_t * pui8This = pui8Src + X;
						(*prgbDst) = (((*pui8This) & sMask) << sShift) | ((*prgbDst) & ~(sMask << sShift));
					}
					break;
				}
				case 8 : {
					for ( uint32_t X = 0; X < _iImage.GetMipmaps()[_sMip]->Width(); ++X ) {
						uint8_t * prgbDst = pui8Bits + X;
						const uint8_t * pui8This = pui8Src + X;
						(*prgbDst) = (*pui8This);
					}
					break;
				}
			}
		}

		CImage::SL2_FREE_IMAGE fiBuffer;
		if ( !fiBuffer.pmMemory ) { return SL2_E_OUTOFMEMORY; }

		if ( _oOptions.bEmbedColorProfile && _iImage.OutputColorSpace().size() && static_cast<size_t>(static_cast<long>(_iImage.OutputColorSpace().size())) == _iImage.OutputColorSpace().size() && static_cast<long>(_iImage.OutputColorSpace().size()) > 0 ) {
			if ( !::FreeImage_CreateICCProfile( fiImage.pbBitmap, static_cast<void *>(const_cast<uint8_t *>(_iImage.OutputColorSpace().data())), static_cast<long>(_iImage.OutputColorSpace().size()) ) ) { return SL2_E_OUTOFMEMORY; }
		}

		if ( !::FreeImage_SaveToMemory( FIF_ICO, fiImage.pbBitmap, fiBuffer.pmMemory, _oOptions.iIcoSaveOption ) ) {
			return SL2_E_OUTOFMEMORY;
		}
        

		BYTE * pbData = nullptr;
		DWORD dwSize = 0;
		if ( !::FreeImage_AcquireMemory( fiBuffer.pmMemory, &pbData, &dwSize ) ) {
			return SL2_E_INTERNALERROR;
		}
		try {
			vConverted.resize( dwSize );
		}
		catch ( ... ) {
			return SL2_E_OUTOFMEMORY;
		}
		std::memcpy( vConverted.data(), pbData, dwSize );
		{
			CStdFile sfFile;
			if ( !sfFile.Create( _sPath.c_str() ) ) {
				return SL2_E_INVALIDWRITEPERMISSIONS;
			}
			if ( !sfFile.WriteToFile( vConverted ) ) {
				return SL2_E_FILEWRITEERROR;
			}
		}

		return SL2_E_SUCCESS;
	}

}   // namespace sl2

#endif  // #ifndef SL2_LIB

/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Let's convert and modify textures!
 */

#include "SL2SurfaceLevel2.h"
#include "Files/SL2StdFile.h"
#include "Image/SL2Image.h"

#include <format>
#include <iostream>

int wmain( int _iArgC, wchar_t const * _wcpArgV[] ) {
    --_iArgC;
    std::u16string sThisDir = sl2::CFileBase::GetFilePath( reinterpret_cast<const char16_t *>((*_wcpArgV++)) );
    ::FreeImage_Initialise();
    sl2::SL2_OPTIONS oOptions;

#define SL2_ERRORT( TXT, CODE )					sl2::PrintError( reinterpret_cast<const char16_t *>(TXT), (CODE) );						\
												if ( oOptions.bPause ) { ::system( "pause" ); }	                                        \
                                                ::FreeImage_DeInitialise();                                                             \
												return (CODE)
#define SL2_ERROR( CODE )						SL2_ERRORT( nullptr, (CODE) )

#define SL2_CHECK( TOTAL, NAME )                 _iArgC >= (TOTAL) && std::wcscmp( &(*_wcpArgV)[1], L ## #NAME ) == 0
#define SL2_ADV( VAL )                          _iArgC -= (VAL); _wcpArgV += (VAL); continue
    while ( _iArgC ) {
        if ( (*_wcpArgV)[0] == L'-' ) {
            if ( SL2_CHECK( 2, file ) ) {
                try {
                    oOptions.vInputs.push_back( reinterpret_cast<const char16_t *>((_wcpArgV[1])) );
                }
                catch ( ... ) { SL2_ERROR( sl2::SL2_E_OUTOFMEMORY ); }
                SL2_ADV( 2 );
            }
            if ( SL2_CHECK( 2, outfile ) ) {
                // Make sure the output list has 1 fewer entries than the input list.
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
                        sSrc += sl2::CFileBase::NoExtension( sl2::CFileBase::GetFileName( oOptions.vInputs[J] ) );
                        sSrc += u".";
                        // If the string is empty, keep the extension of the inputs.
                        if ( !sExt.size() ) {
                            sSrc += sl2::CFileBase::GetFileExtension( oOptions.vInputs[J] );
                        }
                        else {
                            sSrc += sExt;
                        }

                        oOptions.vOutputs.push_back( sSrc );
                    }
                    
                    std::u16string sSrc = sPath;
                    if ( !sName.size() ) {
                        sSrc += sl2::CFileBase::NoExtension( sl2::CFileBase::GetFileName( oOptions.vInputs[oOptions.vOutputs.size()] ) );
                    }
                    else {
                        sSrc += sl2::CFileBase::NoExtension( sl2::CFileBase::GetFileName( sThis ) );
                    }
                    sSrc += u".";
                    // If the string is empty, keep the extension of the inputs.
                    if ( !sExt.size() ) {
                        sSrc += sl2::CFileBase::GetFileExtension( oOptions.vInputs[oOptions.vOutputs.size()] );
                    }
                    else {
                        sSrc += sExt;
                    }
                    oOptions.vOutputs.push_back( sSrc );
                }
                catch ( ... ) { SL2_ERROR( sl2::SL2_E_OUTOFMEMORY ); }
                SL2_ADV( 2 );
            }
        }


        --_iArgC;
        ++_wcpArgV;
    }

#undef SL2_ADV
#undef SL2_CHECK

    if ( oOptions.vfFinalFormat == sl2::SL2_VK_FORMAT_UNDEFINED ) { oOptions.vfFinalFormat = oOptions.vfAutoFormat; }
    for ( size_t I = 0; I < oOptions.vInputs.size(); ++I ) {
        sl2::CImage iImage;
        sl2::SL2_ERRORS eError = iImage.LoadFile( oOptions.vInputs[I].c_str() );
        if ( eError != sl2::SL2_E_SUCCESS ) {
            SL2_ERRORT( std::format( L"Fail to load file: \"{}\".",
               reinterpret_cast<const wchar_t *>(oOptions.vInputs[I].c_str()) ).c_str(), eError );
        }
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
	 * Prints a given error code to the console.
	 * 
	 * \param _eError The error code to print.
	 **/
	void PrintError( SL2_ERRORS _eError ) {
        
    }

}   // namespace sl2

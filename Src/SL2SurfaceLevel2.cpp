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
												return int( CODE )
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
            if ( SL2_CHECK( 2, gamma ) || SL2_CHECK( 2, g ) ) {
                oOptions.dGamma = ::_wtof( _wcpArgV[1] );
                SL2_ADV( 2 );
            }
            if ( SL2_CHECK( 1, rgbe ) ) {
                oOptions.dGamma = 0.0;
                SL2_ADV( 1 );
            }
            if ( SL2_CHECK( 1, srgb ) ) {
                oOptions.dGamma = -2.2;
                SL2_ADV( 1 );
            }
            if ( SL2_CHECK( 1, dxt1c ) || SL2_CHECK( 1, bc1 ) ) {
                oOptions.pkifdFinalFormat = sl2::CFormat::FindFormatDataByOgl( sl2::SL2_KIF_GL_COMPRESSED_RGB_S3TC_DXT1_EXT );
                SL2_ADV( 1 );
            }
            if ( SL2_CHECK( 1, dxt1a ) || SL2_CHECK( 1, bc1a ) ) {
                oOptions.pkifdFinalFormat = sl2::CFormat::FindFormatDataByOgl( sl2::SL2_KIF_GL_COMPRESSED_RGBA_S3TC_DXT1_EXT );
                SL2_ADV( 1 );
            }
            if ( SL2_CHECK( 1, dxt2 ) ) {
                oOptions.pkifdFinalFormat = sl2::CFormat::FindFormatDataByOgl( sl2::SL2_KIF_GL_COMPRESSED_RGBA_S3TC_DXT3_EXT );
                SL2_ADV( 1 );
            }
            if ( SL2_CHECK( 1, dxt3 ) || SL2_CHECK( 1, bc2 ) ) {
                oOptions.pkifdFinalFormat = sl2::CFormat::FindFormatDataByOgl( sl2::SL2_KIF_GL_COMPRESSED_RGBA_S3TC_DXT3_EXT );
                SL2_ADV( 1 );
            }
            if ( SL2_CHECK( 1, dxt4 ) ) {
                oOptions.pkifdFinalFormat = sl2::CFormat::FindFormatDataByOgl( sl2::SL2_KIF_GL_COMPRESSED_RGBA_S3TC_DXT5_EXT );
                SL2_ADV( 1 );
            }
            if ( SL2_CHECK( 1, dxt5 ) || SL2_CHECK( 1, bc3 ) ) {
                oOptions.pkifdFinalFormat = sl2::CFormat::FindFormatDataByOgl( sl2::SL2_KIF_GL_COMPRESSED_RGBA_S3TC_DXT5_EXT );
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
                oOptions.pkifdFinalFormat = sl2::CFormat::FindFormatDataByOgl( sl2::SL2_KIF_GL_RGB5 );
                SL2_ADV( 1 );
            }
            if ( SL2_CHECK( 1, R5G6B5 ) ) {
                oOptions.pkifdFinalFormat = sl2::CFormat::FindFormatDataByVulkan( sl2::SL2_VK_FORMAT_R5G6B5_UNORM_PACK16 );
                SL2_ADV( 1 );
            }
            if ( SL2_CHECK( 1, A8 ) ) {
                oOptions.pkifdFinalFormat = sl2::CFormat::FindFormatDataByOgl( sl2::SL2_KIF_GL_ALPHA8 );
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
                oOptions.pkifdFinalFormat = sl2::CFormat::FindFormatDataByOgl( sl2::SL2_KIF_GL_LUMINANCE8_ALPHA8 );
                SL2_ADV( 1 );
            }
            if ( SL2_CHECK( 1, L16 ) ) {
                oOptions.pkifdFinalFormat = sl2::CFormat::FindFormatDataByOgl( sl2::SL2_KIF_GL_LUMINANCE16 );
                SL2_ADV( 1 );
            }
            if ( SL2_CHECK( 1, L8 ) ) {
                oOptions.pkifdFinalFormat = sl2::CFormat::FindFormatDataByOgl( sl2::SL2_KIF_GL_LUMINANCE8 );
                SL2_ADV( 1 );
            }
            if ( SL2_CHECK( 1, L4A4 ) ) {
                oOptions.pkifdFinalFormat = sl2::CFormat::FindFormatDataByOgl( sl2::SL2_KIF_GL_LUMINANCE4_ALPHA4 );
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
        }


        --_iArgC;
        ++_wcpArgV;
    }

#undef SL2_ADV
#undef SL2_CHECK

    const sl2::CFormat::SL2_KTX_INTERNAL_FORMAT_DATA * pkifdFormat = oOptions.pkifdFinalFormat;
    for ( size_t I = 0; I < oOptions.vInputs.size(); ++I ) {
        sl2::CImage iImage;
        sl2::SL2_ERRORS eError = iImage.LoadFile( oOptions.vInputs[I].c_str() );
        if ( eError != sl2::SL2_E_SUCCESS ) {
            SL2_ERRORT( std::format( L"Failed to load file: \"{}\".",
               reinterpret_cast<const wchar_t *>(oOptions.vInputs[I].c_str()) ).c_str(), eError );
        }
        iImage.SetGamma( oOptions.dGamma );
        oOptions.pkifdFinalFormat = pkifdFormat;
        if ( !oOptions.pkifdFinalFormat ) {
            oOptions.pkifdFinalFormat = iImage.Format();
        }
        sl2::CImage iConverted;
        iImage.ConvertToFormat( oOptions.pkifdFinalFormat, iConverted );


        if ( ::_wcsicmp( reinterpret_cast<const wchar_t *>(sl2::CFileBase::GetFileExtension( oOptions.vOutputs[I] ).c_str()), L"png" ) == 0 ) {
            eError = sl2::ExportAsPng( iConverted, oOptions.vOutputs[I], oOptions );
            if ( sl2::SL2_E_SUCCESS != eError ) {
                SL2_ERRORT( std::format( L"Failed to save file: \"{}\".",
                    reinterpret_cast<const wchar_t *>(oOptions.vOutputs[I].c_str()) ).c_str(), eError );
            }
        }
        //
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
        /*struct SL2_PNG_FORMAT {
            SL2_VKFORMAT                vfInternalFormat;
            FREE_IMAGE_FORMAT           ifFreeFormat;
        };*/
        const CFormat::SL2_KTX_INTERNAL_FORMAT_DATA * pkifdFormats[] = {
            CFormat::FindFormatDataByVulkan( SL2_VK_FORMAT_R8G8B8_UNORM ),
            CFormat::FindFormatDataByVulkan( SL2_VK_FORMAT_R8G8B8A8_UNORM ),
            CFormat::FindFormatDataByVulkan( SL2_VK_FORMAT_R16G16B16_UNORM ),
            CFormat::FindFormatDataByVulkan( SL2_VK_FORMAT_R16G16B16A16_UNORM ),

            CFormat::FindFormatDataByOgl( SL2_KIF_GL_LUMINANCE8 ),
            CFormat::FindFormatDataByOgl( SL2_KIF_GL_LUMINANCE16 ),
            //CFormat::FindFormatDataByOgl( SL2_KIF_GL_LUMINANCE8_ALPHA8 ),
            //CFormat::FindFormatDataByOgl( SL2_KIF_GL_LUMINANCE16_ALPHA16 ),
        };
        const CFormat::SL2_KTX_INTERNAL_FORMAT_DATA * pkifdUseMe = CFormat::FindBestFormat( _iImage.Format(), pkifdFormats, SL2_ELEMENTS( pkifdFormats ) );
        if ( !pkifdUseMe ) {
            return SL2_E_BADFORMAT;
        }
        FREE_IMAGE_TYPE fitType = FIT_BITMAP;
        if ( pkifdUseMe->kifInternalFormat == SL2_KIF_GL_LUMINANCE8 ) {
        }
        else if ( pkifdUseMe->kifInternalFormat == SL2_KIF_GL_LUMINANCE16 ) {
            fitType = FIT_UINT16;
        }
        else if ( pkifdUseMe->kifInternalFormat == SL2_KIF_GL_LUMINANCE8_ALPHA8 ) {
        }
        else if ( pkifdUseMe->kifInternalFormat == SL2_KIF_GL_LUMINANCE16_ALPHA16 ) {
            fitType = FIT_RGBA16;
        }
        else if ( pkifdUseMe->ui32BlockSizeInBits == 16 * 3 ) {
            fitType = FIT_RGB16;
        }
        else if ( pkifdUseMe->ui32BlockSizeInBits == 16 * 4 ) {
            fitType = FIT_RGBA16;
        }
        CImage::SL2_FREEIMAGE_ALLOCATET fiImage( fitType, _iImage.GetMipmaps()[_sMip]->Width(), _iImage.GetMipmaps()[_sMip]->Height(), pkifdUseMe->ui32BlockSizeInBits );
		if ( !fiImage.pbBitmap ) { return SL2_E_OUTOFMEMORY; }

        std::vector<uint8_t> vConverted;
        SL2_ERRORS eError = _iImage.ConvertToFormat( pkifdUseMe, _sMip, _sArray, _sFace, vConverted, true );
        if ( eError != SL2_E_SUCCESS ) { return eError; }


        uint32_t ui32Pitch = CFormat::GetRowSize( pkifdUseMe, _iImage.GetMipmaps()[_sMip]->Width() );
        uint32_t ui32Slice = uint32_t( ui32Pitch * _iImage.GetMipmaps()[_sMip]->Height() * _sSlice );
        for ( uint32_t H = 0; H < _iImage.GetMipmaps()[_sMip]->Height(); ++H ) {
            BYTE * pui8Bits = ::FreeImage_GetScanLine( fiImage.pbBitmap, int( H ) );
            uint8_t * pui8Src = vConverted.data() + ui32Slice + ui32Pitch * H;
            switch ( fitType ) {
                case FIT_BITMAP : {
                    switch ( pkifdUseMe->ui32BlockSizeInBits ) {
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

        if ( !::FreeImage_SaveToMemory( FIF_PNG, fiImage.pbBitmap, fiBuffer.pmMemory, 9 ) ) {
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

/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: An image is a collection of surfaces:
 *	1: Mipmaps
 *	2: Array
 *	3: 3D Slice
 *	4: Cube Face
 * 
 * This class is the primary workhorse of the tool.  Image conversion and manipulation takes place here, although the functions for conversion etc. are in SL2Formats.
 */

#include "SL2Image.h"
#include "../Files/SL2StdFile.h"
#include "../Utilities/SL2Stream.h"
#include "DDS/SL2Dds.h"
#include "SL2KtxTexture.h"

#include <basisu_transcoder.h>


namespace sl2 {

	CImage::CImage() :
		m_dGamma( 0.0 ),
		m_sArraySize( 0 ),
		m_sFaces( 0 ),
		m_pkifFormat( nullptr ),
		m_bIsPreMultiplied( false ),
		m_bNeedsPreMultiply( false ),
		m_bFlipX( false ),
		m_bFlipY( false ), 
		m_bFlipZ( false ),
		m_bSwap( false ) {
		m_sSwizzle = CFormat::DefaultSwizzle();
	}
	CImage::~CImage() {
		Reset();
	}


	// == Operators.
	/**
	 * Move operator.
	 * 
	 * \param _iOther The image to copy and destroy.
	 * \return Returns a reference to this image.
	 **/
	CImage & CImage::operator = ( CImage &&_iOther ) {
		m_vMipMaps = std::move( _iOther.m_vMipMaps );
		m_sArraySize = _iOther.m_sArraySize;
		m_sFaces = _iOther.m_sFaces;
		m_pkifFormat = _iOther.m_pkifFormat;
		m_dGamma = _iOther.m_dGamma;
		m_sSwizzle = _iOther.m_sSwizzle;
		m_bIsPreMultiplied = _iOther.m_bIsPreMultiplied;
		m_bNeedsPreMultiply = _iOther.m_bNeedsPreMultiply;
		m_bFlipX = _iOther.m_bFlipX;
		m_bFlipY = _iOther.m_bFlipZ;
		m_bFlipZ = _iOther.m_bFlipY;
		m_bSwap = _iOther.m_bSwap;
		_iOther.m_sArraySize = 0;
		_iOther.m_sFaces = 0;
		_iOther.m_pkifFormat = nullptr;
		_iOther.m_dGamma = 0.0;
		_iOther.m_sSwizzle = CFormat::DefaultSwizzle();
		_iOther.m_bIsPreMultiplied = false;
		_iOther.m_bNeedsPreMultiply = false;
		_iOther.m_bFlipX = false;
		_iOther.m_bFlipY = false;
		_iOther.m_bFlipZ = false;
		_iOther.m_bSwap = false;

		return (*this);
	}

	// == Functions.
	/**
	 * Resets the object to scratch.  It can be reused after this.
	 **/
	void CImage::Reset() {
		m_sArraySize = 0;
		m_sFaces = 0;
		for ( auto I = m_vMipMaps.size(); I--; ) {
			m_vMipMaps[I]->clear();
			m_vMipMaps[I].reset();
		}
		m_vMipMaps = std::vector<std::unique_ptr<CSurface>>();
		m_sSwizzle = CFormat::DefaultSwizzle();
		m_pkifFormat = nullptr;
		m_bIsPreMultiplied = false;
		m_bNeedsPreMultiply = false;
		m_bFlipX = false;
		m_bFlipY = false;
		m_bFlipZ = false;
		m_bSwap = false;
	}

	/**
	 * Loads an image file.  All image slices, faces, and array slices will be loaded.
	 * 
	 * \param _pcFile The name of the file to load.
	 * \return Returns an error code.
	 **/
	SL2_ERRORS CImage::LoadFile( const char16_t * _pcFile ) {
		std::vector<uint8_t> vFile;
		{
			CStdFile sfFile;
			if ( !sfFile.Open( _pcFile ) ) { return SL2_E_FILENOTFOUND; }
			if ( !sfFile.LoadToMemory( vFile ) ) { return SL2_E_OUTOFMEMORY; }
		}

		return LoadFile( vFile );
	}

	/**
	 * Loads an image file.  All image slices, faces, and array slices will be loaded.
	 * 
	 * \param _vData The image file to load.
	 * \return Returns an error code.
	 **/
	SL2_ERRORS CImage::LoadFile( const std::vector<uint8_t> &_vData ) {
		//CStream sFile( _vData );
		if ( SL2_E_SUCCESS == LoadDds( _vData ) ) { return SL2_E_SUCCESS; }
		if ( SL2_E_SUCCESS == LoadKtx1( _vData ) ) { return SL2_E_SUCCESS; }
		if ( SL2_E_SUCCESS == LoadKtx2( _vData ) ) { return SL2_E_SUCCESS; }
		return LoadFreeImage( _vData );
	}

	/**
	 * Converts to another format.  _iDst holds the converted image.
	 * 
	 * \param _pkifFormat The format to which to convert.
	 * \param _iDst Holds the converted image.
	 * \return Returns an error code.
	 **/
	SL2_ERRORS CImage::ConvertToFormat( const CFormat::SL2_KTX_INTERNAL_FORMAT_DATA * _pkifFormat, CImage &_iDst ) {
		CImage iTmp;
		if ( !_pkifFormat || !Format() ) { return SL2_E_BADFORMAT; }

		if ( (m_dGamma == 0.0 || m_dGamma == 1.0) &&
			!(!m_bIsPreMultiplied && m_bNeedsPreMultiply) &&
			!m_bFlipX && !m_bFlipY && !m_bFlipZ &&
			CFormat::SwizzleIsDefault( m_sSwizzle ) &&
			!m_bSwap &&
			((_pkifFormat->vfVulkanFormat != SL2_VK_FORMAT_UNDEFINED && _pkifFormat->vfVulkanFormat == Format()->vfVulkanFormat) ||
			(_pkifFormat->dfDxFormat != SL2_DXGI_FORMAT_UNKNOWN && _pkifFormat->dfDxFormat == Format()->dfDxFormat) ||
			(_pkifFormat->mfMetalFormat != SL2_MTLPixelFormatInvalid && _pkifFormat->mfMetalFormat == Format()->mfMetalFormat) ||
			((_pkifFormat->kifInternalFormat != SL2_KIF_GL_INVALID && _pkifFormat->kifInternalFormat == Format()->kifInternalFormat &&
				_pkifFormat->kbifBaseInternalFormat != SL2_KBIF_GL_INVALID && _pkifFormat->kbifBaseInternalFormat == Format()->kbifBaseInternalFormat &&
				_pkifFormat->ktType != SL2_KT_GL_INVALID && _pkifFormat->ktType == Format()->ktType))) ) {
			// No format conversion needed.  Just copy the buffers.
			if ( !_iDst.AllocateTexture( _pkifFormat, Width(), Height(), Depth(), Mipmaps(), ArraySize(), Faces() ) ) { return SL2_E_OUTOFMEMORY; }
			for ( size_t M = 0; M < Mipmaps(); ++M ) {
				std::memcpy( _iDst.Data( M, 0, 0, 0 ), Data( M, 0, 0, 0 ), m_vMipMaps[M]->size() );
			}
			return SL2_E_SUCCESS;
		}

		if ( !Format()->pfToRgba64F ) { return SL2_E_BADFORMAT; }
		
		if ( !iTmp.AllocateTexture( CFormat::FindFormatDataByVulkan( SL2_VK_FORMAT_R64G64B64A64_SFLOAT ), Width(), Height(), Depth(), Mipmaps(), ArraySize(), Faces() ) ) { return SL2_E_OUTOFMEMORY; }
		
		for ( size_t M = 0; M < Mipmaps(); ++M ) {
			for ( size_t A = 0; A < ArraySize(); ++A ) {
				for ( size_t F = 0; F < Faces(); ++F ) {
					if ( !Format()->pfToRgba64F( Data( M, 0, A, F ), iTmp.Data( M, 0, A, F ), m_vMipMaps[M]->Width(), m_vMipMaps[M]->Height(), m_vMipMaps[M]->Depth(), Format() ) ) {
						return SL2_E_INTERNALERROR;
					}
					BakeGamma( iTmp.Data( M, 0, A, F ), m_dGamma, m_vMipMaps[M]->Width(), m_vMipMaps[M]->Height(), m_vMipMaps[M]->Depth() );
					if ( !m_bIsPreMultiplied && m_bNeedsPreMultiply ) {
						CFormat::ApplyPreMultiply( iTmp.Data( M, 0, A, F ), m_vMipMaps[M]->Width(), m_vMipMaps[M]->Height(), m_vMipMaps[M]->Depth() );
						iTmp.m_bIsPreMultiplied = true;
					}
					if ( m_bFlipX ) {
						CFormat::FlipX( iTmp.Data( M, 0, A, F ), m_vMipMaps[M]->Width(), m_vMipMaps[M]->Height(), m_vMipMaps[M]->Depth() );
					}
					if ( m_bFlipY ) {
						CFormat::FlipY( iTmp.Data( M, 0, A, F ), m_vMipMaps[M]->Width(), m_vMipMaps[M]->Height(), m_vMipMaps[M]->Depth() );
					}
					if ( m_bFlipZ ) {
						CFormat::FlipZ( iTmp.Data( M, 0, A, F ), m_vMipMaps[M]->Width(), m_vMipMaps[M]->Height(), m_vMipMaps[M]->Depth() );
					}
					if ( m_bSwap ) {
						CFormat::Swap( iTmp.Data( M, 0, A, F ), m_vMipMaps[M]->Width(), m_vMipMaps[M]->Height(), m_vMipMaps[M]->Depth() );
					}
					if ( !CFormat::SwizzleIsDefault( m_sSwizzle ) ) {
						CFormat::ApplySwizzle( iTmp.Data( M, 0, A, F ), m_vMipMaps[M]->Width(), m_vMipMaps[M]->Height(), m_vMipMaps[M]->Depth(), m_sSwizzle );
					}
				}
			}
		}
		if ( _pkifFormat->vfVulkanFormat == SL2_VK_FORMAT_R64G64B64A64_SFLOAT ) {
			// We already did the conversion.
			_iDst = std::move( iTmp );
			return SL2_E_SUCCESS;
		}
		if ( !_pkifFormat->pfFromRgba64F ) { return SL2_E_BADFORMAT; }
		_iDst.Reset();
		if ( !_iDst.AllocateTexture( _pkifFormat, Width(), Height(), Depth(), Mipmaps(), ArraySize(), Faces() ) ) { return SL2_E_OUTOFMEMORY; }
		CFormat::SL2_KTX_INTERNAL_FORMAT_DATA ifdData = (*_pkifFormat);
		for ( size_t M = 0; M < Mipmaps(); ++M ) {
			for ( size_t A = 0; A < ArraySize(); ++A ) {
				for ( size_t F = 0; F < Faces(); ++F ) {
					if ( !_pkifFormat->pfFromRgba64F( iTmp.Data( M, 0, A, F ), _iDst.Data( M, 0, A, F ), m_vMipMaps[M]->Width(), m_vMipMaps[M]->Height(), m_vMipMaps[M]->Depth(), &ifdData ) ) {
						return SL2_E_INTERNALERROR;
					}
				}
			}
		}
		return SL2_E_SUCCESS;
	}

	/**
	 * Converts a single texture to a given format.
	 * 
	 * \param _pkifFormat The format to which to convert.
	 * \param _sMip The mipmap level to convert.
	 * \param _sArray The array to convert.
	 * \param _sFace The face to convert.
	 * \param _vDst The destination buffer.
	 * \param _bInvertY If true, the destination is inverted vertically.
	 * \return Returns an error code.
	 **/
	SL2_ERRORS CImage::ConvertToFormat( const CFormat::SL2_KTX_INTERNAL_FORMAT_DATA * _pkifFormat,
		size_t _sMip, size_t _sArray, size_t _sFace, std::vector<uint8_t> &_vDst, bool _bInvertY ) {
		if ( !_pkifFormat || !Format() ) { return SL2_E_BADFORMAT; }
		if ( _sMip >= m_vMipMaps.size() ) { return SL2_E_INVALIDCALL; }

		size_t sBaseSize = CFormat::GetFormatSize( _pkifFormat, m_vMipMaps[_sMip]->Width(), m_vMipMaps[_sMip]->Height(), m_vMipMaps[_sMip]->Depth() );
		sBaseSize = GetActualPlaneSize( sBaseSize );
		try {
			_vDst.resize( sBaseSize );
		}
		catch ( ... ) { SL2_E_OUTOFMEMORY; }
		return ConvertToFormat( _pkifFormat, _sMip, _sArray, _sFace, _vDst.data(), _bInvertY );
	}

	/**
	 * Converts a single texture to a given format.
	 * 
	 * \param _pkifFormat The format to which to convert.
	 * \param _sMip The mipmap level to convert.
	 * \param _sArray The array to convert.
	 * \param _sFace The face to convert.
	 * \param _pui8Dst The destination buffer.
	 * \param _bInvertY If true, the destination is inverted vertically.
	 * \return Returns an error code.
	 **/
	SL2_ERRORS CImage::ConvertToFormat( const CFormat::SL2_KTX_INTERNAL_FORMAT_DATA * _pkifFormat,
		size_t _sMip, size_t _sArray, size_t _sFace, uint8_t * _pui8Dst, bool _bInvertY ) {
		if ( !_pkifFormat || !Format() ) { return SL2_E_BADFORMAT; }
		if ( _sMip >= m_vMipMaps.size() ) { return SL2_E_INVALIDCALL; }

		size_t sBaseSize = CFormat::GetFormatSize( CFormat::FindFormatDataByVulkan( SL2_VK_FORMAT_R64G64B64A64_SFLOAT ), m_vMipMaps[_sMip]->Width(), m_vMipMaps[_sMip]->Height(), m_vMipMaps[_sMip]->Depth() );
		if ( (m_dGamma == 0.0 || m_dGamma == 1.0) &&
			!_bInvertY &&
			!m_bFlipX && !m_bFlipY && !m_bFlipZ &&
			CFormat::SwizzleIsDefault( m_sSwizzle ) &&
			!m_bSwap &&
			!(!m_bIsPreMultiplied && m_bNeedsPreMultiply) &&
			((_pkifFormat->vfVulkanFormat != SL2_VK_FORMAT_UNDEFINED && _pkifFormat->vfVulkanFormat == Format()->vfVulkanFormat) ||
			(_pkifFormat->dfDxFormat != SL2_DXGI_FORMAT_UNKNOWN && _pkifFormat->dfDxFormat == Format()->dfDxFormat) ||
			(_pkifFormat->mfMetalFormat != SL2_MTLPixelFormatInvalid && _pkifFormat->mfMetalFormat == Format()->mfMetalFormat) ||
			((_pkifFormat->kifInternalFormat != SL2_KIF_GL_INVALID && _pkifFormat->kifInternalFormat == Format()->kifInternalFormat &&
				_pkifFormat->kbifBaseInternalFormat != SL2_KBIF_GL_INVALID && _pkifFormat->kbifBaseInternalFormat == Format()->kbifBaseInternalFormat &&
				_pkifFormat->ktType != SL2_KT_GL_INVALID && _pkifFormat->ktType == Format()->ktType))) ) {
			// No format conversion needed.  Just copy the buffers.
			sBaseSize = CFormat::GetFormatSize( _pkifFormat, m_vMipMaps[_sMip]->Width(), m_vMipMaps[_sMip]->Height(), m_vMipMaps[_sMip]->Depth() );
			std::memcpy( _pui8Dst, Data( _sMip, 0, _sArray, _sFace ), sBaseSize );
			return SL2_E_SUCCESS;
		}

		sBaseSize = GetActualPlaneSize( sBaseSize );
		if ( !sBaseSize ) { return SL2_E_BADFORMAT; }
		std::vector<uint8_t> vTmp;
		try {
			vTmp.resize( sBaseSize );
		}
		catch ( ... ) {
			return SL2_E_OUTOFMEMORY;
		}

		if ( !Format()->pfToRgba64F( Data( _sMip, 0, _sArray, _sFace ), vTmp.data(), m_vMipMaps[_sMip]->Width(), m_vMipMaps[_sMip]->Height(), m_vMipMaps[_sMip]->Depth(), Format() ) ) {
			return SL2_E_INTERNALERROR;
		}
		BakeGamma( vTmp.data(), m_dGamma, m_vMipMaps[_sMip]->Width(), m_vMipMaps[_sMip]->Height(), m_vMipMaps[_sMip]->Depth() );
		if ( !m_bIsPreMultiplied && m_bNeedsPreMultiply ) {
			CFormat::ApplyPreMultiply( vTmp.data(), m_vMipMaps[_sMip]->Width(), m_vMipMaps[_sMip]->Height(), m_vMipMaps[_sMip]->Depth() );
		}

		if ( m_bFlipX ) {
			CFormat::FlipX( vTmp.data(), m_vMipMaps[_sMip]->Width(), m_vMipMaps[_sMip]->Height(), m_vMipMaps[_sMip]->Depth() );
		}
		if ( _bInvertY != m_bFlipY ) {
			CFormat::FlipY( vTmp.data(), m_vMipMaps[_sMip]->Width(), m_vMipMaps[_sMip]->Height(), m_vMipMaps[_sMip]->Depth() );
		}
		if ( m_bFlipZ ) {
			CFormat::FlipZ( vTmp.data(), m_vMipMaps[_sMip]->Width(), m_vMipMaps[_sMip]->Height(), m_vMipMaps[_sMip]->Depth() );
		}
		if ( m_bSwap ) {
			CFormat::Swap( vTmp.data(), m_vMipMaps[_sMip]->Width(), m_vMipMaps[_sMip]->Height(), m_vMipMaps[_sMip]->Depth() );
		}
		if ( !CFormat::SwizzleIsDefault( m_sSwizzle ) ) {
			CFormat::ApplySwizzle( vTmp.data(), m_vMipMaps[_sMip]->Width(), m_vMipMaps[_sMip]->Height(), m_vMipMaps[_sMip]->Depth(), m_sSwizzle );
		}

		CFormat::SL2_KTX_INTERNAL_FORMAT_DATA ifdData = (*_pkifFormat);
		if ( !_pkifFormat->pfFromRgba64F( vTmp.data(), _pui8Dst, m_vMipMaps[_sMip]->Width(), m_vMipMaps[_sMip]->Height(), m_vMipMaps[_sMip]->Depth(), &ifdData ) ) {
			return SL2_E_INTERNALERROR;
		}
		return SL2_E_SUCCESS;
	}

	/**
	 * Allocates a texture of a given number of mipmaps, array indices, faces, and dimensions.
	 * 
	 * \param _pkifFormat DESC
	 * \param _ui32Width Width of the image base mipmap level.
	 * \param _ui32Height Height of the image base mipmap level.
	 * \param _ui32Depth Depth of the image base mipmap level.
	 * \param _sMips Number of mipmaps.  Must be at least 1.  If 0, a full mipmap chain is allocated.
	 * \param _sArray Number of array slices.  Must be at least 1.
	 * \param _sFaces Number of faces.  Either 1 or 6.
	 * \return Returns true if all mipmaps could be allocated and the texture size is valid (non-0) and a supported format.
	 **/
	bool CImage::AllocateTexture( const CFormat::SL2_KTX_INTERNAL_FORMAT_DATA * _pkifFormat, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Depth, size_t _sMips, size_t _sArray, size_t _sFaces ) {
		if ( !_pkifFormat ) { return false; }
		if ( !_sMips ) {
			_sMips = CUtilities::Max( size_t( std::round( std::log2( _ui32Width ) ) ), size_t( std::round( std::log2( _ui32Height ) ) ) );
			_sMips = CUtilities::Max( size_t( std::round( std::log2( _ui32Depth ) ) ), _sMips ) + 1;
		}
		if ( !_sMips ) { return false; }
		

		m_sArraySize = _sArray;
		m_sFaces = _sFaces;
		m_pkifFormat = _pkifFormat;
		m_dGamma = 0.0;
		
		size_t sSrcBaseSize = CFormat::GetFormatSize( _pkifFormat, _ui32Width, _ui32Height, _ui32Depth );
		if ( !sSrcBaseSize ) { Reset(); return false; }
		try {
			m_vMipMaps.resize( _sMips );
			for ( size_t I = 0; I < _sMips; ++I ) {
				size_t sBaseSize = GetActualPlaneSize( CFormat::GetFormatSize( _pkifFormat, _ui32Width, _ui32Height, _ui32Depth ) );
				size_t sFullSize = sBaseSize * _sArray * _sFaces;
				if ( !sFullSize ) { Reset(); return false; }

				m_vMipMaps[I] = std::make_unique<CSurface>( sFullSize, sBaseSize, _ui32Width, _ui32Height, _ui32Depth );

				_ui32Width = CUtilities::Max<uint32_t>( _ui32Width >> 1, 1 );
				_ui32Height = CUtilities::Max<uint32_t>( _ui32Height >> 1, 1 );
				_ui32Depth = CUtilities::Max<uint32_t>( _ui32Depth >> 1, 1 );
			}
		}
		catch ( ... ) {
			Reset();
			return false;
		}
		
		return true;
	}

	/**
	 * Bakes the image special gamma into a given texture buffer.  The format must be RGBA64F.
	 * 
	 * \param _pui8Buffer The texture texels.
	 * \param _dGamma The gamma to apply.
	 * \param _ui32Width The width of the image.
	 * \param _ui32Height The height of the image.
	 * \param _ui32Depth The depth of the image.
	 **/
	void CImage::BakeGamma( uint8_t * _pui8Buffer, double _dGamma, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Depth ) {
		if ( !_pui8Buffer ) { return; }
		if ( _dGamma == 0.0 || _dGamma == 1.0 ) { return; }
		CFormat::SL2_RGBA64F * prDst = reinterpret_cast<CFormat::SL2_RGBA64F *>(_pui8Buffer);
		if ( _dGamma <= -1.0 ) {
			// True Linear -> sRGB conversion.
			for ( uint32_t D = 0; D < _ui32Depth; ++D ) {
				uint32_t ui32Slice = _ui32Width * _ui32Height * D;
				for ( uint32_t H = 0; H < _ui32Height; ++H ) {
					uint32_t ui32Row = _ui32Width * H;
					for ( uint32_t W = 0; W < _ui32Width; ++W ) {
						CFormat::SL2_RGBA64F * prThis = &prDst[ui32Slice+ui32Row+W];
						prThis->dRgba[SL2_PC_R] = CUtilities::LinearToSRgb( prThis->dRgba[SL2_PC_R] );
						prThis->dRgba[SL2_PC_G] = CUtilities::LinearToSRgb( prThis->dRgba[SL2_PC_G] );
						prThis->dRgba[SL2_PC_B] = CUtilities::LinearToSRgb( prThis->dRgba[SL2_PC_B] );
					}
				}
			}
		}
		else if ( _dGamma < 0.0 ) {
			// True sRGB -> Linear conversion.
			for ( uint32_t D = 0; D < _ui32Depth; ++D ) {
				uint32_t ui32Slice = _ui32Width * _ui32Height * D;
				for ( uint32_t H = 0; H < _ui32Height; ++H ) {
					uint32_t ui32Row = _ui32Width * H;
					for ( uint32_t W = 0; W < _ui32Width; ++W ) {
						CFormat::SL2_RGBA64F * prThis = &prDst[ui32Slice+ui32Row+W];
						prThis->dRgba[SL2_PC_R] = CUtilities::SRgbToLinear( prThis->dRgba[SL2_PC_R] );
						prThis->dRgba[SL2_PC_G] = CUtilities::SRgbToLinear( prThis->dRgba[SL2_PC_G] );
						prThis->dRgba[SL2_PC_B] = CUtilities::SRgbToLinear( prThis->dRgba[SL2_PC_B] );
					}
				}
			}
		}
		else {
			// Custom gamma curve.
			_dGamma = 1.0 / _dGamma;
			for ( uint32_t D = 0; D < _ui32Depth; ++D ) {
				uint32_t ui32Slice = _ui32Width * _ui32Height * D;
				for ( uint32_t H = 0; H < _ui32Height; ++H ) {
					uint32_t ui32Row = _ui32Width * H;
					for ( uint32_t W = 0; W < _ui32Width; ++W ) {
						CFormat::SL2_RGBA64F * prThis = &prDst[ui32Slice+ui32Row+W];
						prThis->dRgba[SL2_PC_R] = std::pow( prThis->dRgba[SL2_PC_R], _dGamma );
						prThis->dRgba[SL2_PC_G] = std::pow( prThis->dRgba[SL2_PC_G], _dGamma );
						prThis->dRgba[SL2_PC_B] = std::pow( prThis->dRgba[SL2_PC_B], _dGamma );
					}
				}
			}
		}
	}

	/**
	 * Loads using the FreeImage library.
	 * 
	 * \param _vData The file to load.
	 * \return Returns an error code.
	 **/
	SL2_ERRORS CImage::LoadFreeImage( const std::vector<uint8_t> &_vData ) {
		SL2_FREE_IMAGE fiImage( _vData );
		if ( !fiImage.pmMemory ) { return SL2_E_OUTOFMEMORY; }

		FREE_IMAGE_FORMAT fifFormat = ::FreeImage_GetFileTypeFromMemory( fiImage.pmMemory, 0 );
		if ( FIF_UNKNOWN == fifFormat ) { return SL2_E_INVALIDFILETYPE; }

		SL2_FREEIMAGE_LOAD_FROM_MEMORY flfmData( fiImage );
		int iTransIndex = ::FreeImage_GetTransparentIndex( flfmData.pbBitmap );

		uint32_t ui32Width = ::FreeImage_GetWidth( flfmData.pbBitmap );
		uint32_t ui32Height = ::FreeImage_GetHeight( flfmData.pbBitmap );
		constexpr uint32_t ui32Depth = 1;

#define FreeImage_GetScanLine( DATA, Y )	FreeImage_GetScanLine( DATA, ui32Height - Y - 1 )

		FREE_IMAGE_TYPE fitType = ::FreeImage_GetImageType( flfmData.pbBitmap );

		if ( (::FreeImage_GetICCProfile( flfmData.pbBitmap )->flags &
			FIICC_COLOR_IS_CMYK) == FIICC_COLOR_IS_CMYK ) {
			return SL2_E_SUCCESS;
		}

		switch ( fitType ) {
			case FIT_BITMAP : {
				switch ( ::FreeImage_GetBPP( flfmData.pbBitmap ) ) {
					case 1 : {
						if ( !AllocateTexture( CFormat::FindFormatDataByVulkan( SL2_VK_FORMAT_R8G8B8A8_UNORM ), ui32Width, ui32Height, ui32Depth ) ) { return SL2_E_OUTOFMEMORY; }
						uint8_t * pui8Dst = Data();
						const RGBQUAD * prgbqPal = ::FreeImage_GetPalette( flfmData.pbBitmap );
						uint32_t ui32Pitch = SL2_ROUND_UP( ui32Width * sizeof( CFormat::SL2_RGBA_UNORM ), 4 );
						if ( prgbqPal ) {
							for ( uint32_t Y = 0; Y < ui32Height; ++Y ) {
								const uint8_t * pui8Data = ::FreeImage_GetScanLine( flfmData.pbBitmap, Y );
								for ( uint32_t X = 0; X < ui32Width; ++X ) {
									uint8_t ui8Bit = (pui8Data[X>>3] >> (X % 8)) & 1;
									BYTE bAlpha = prgbqPal[ui8Bit].rgbReserved;
									if ( iTransIndex != -1 ) {
										bAlpha = (iTransIndex == ui8Bit) ? 0 : 255;
									}
									CFormat::SL2_RGBA_UNORM * pRgb = reinterpret_cast<CFormat::SL2_RGBA_UNORM *>(pui8Dst + ui32Pitch * Y) + X;
									pRgb->ui8Rgba[SL2_PC_R] = prgbqPal[ui8Bit].rgbRed;
									pRgb->ui8Rgba[SL2_PC_G] = prgbqPal[ui8Bit].rgbGreen;
									pRgb->ui8Rgba[SL2_PC_B] = prgbqPal[ui8Bit].rgbBlue;
									pRgb->ui8Rgba[SL2_PC_A] = bAlpha;
								}
							}
							break;
						}
						return SL2_E_INVALIDDATA;
					}
					case 4 : {
						if ( !AllocateTexture( CFormat::FindFormatDataByVulkan( SL2_VK_FORMAT_R8G8B8A8_UNORM ), ui32Width, ui32Height, ui32Depth ) ) { return SL2_E_OUTOFMEMORY; }
						uint8_t * pui8Dst = Data();
						const RGBQUAD * prgbqPal = ::FreeImage_GetPalette( flfmData.pbBitmap );
						uint32_t ui32Pitch = SL2_ROUND_UP( ui32Width * sizeof( CFormat::SL2_RGBA_UNORM ), 4 );
						if ( prgbqPal ) {
							for ( uint32_t Y = 0; Y < ui32Height; ++Y ) {
								const uint8_t * pui8Data = ::FreeImage_GetScanLine( flfmData.pbBitmap, Y );
								for ( uint32_t X = 0; X < ui32Width; ++X ) {
									uint8_t ui8Bit = (pui8Data[X>>1] >> (X % 2 << 2)) & 0xF;
									BYTE bAlpha = prgbqPal[ui8Bit].rgbReserved;
									if ( iTransIndex != -1 ) {
										bAlpha = (iTransIndex == ui8Bit) ? 0 : 255;
									}
									CFormat::SL2_RGBA_UNORM * pRgb = reinterpret_cast<CFormat::SL2_RGBA_UNORM *>(pui8Dst + ui32Pitch * Y) + X;
									pRgb->ui8Rgba[SL2_PC_R] = prgbqPal[ui8Bit].rgbRed;
									pRgb->ui8Rgba[SL2_PC_G] = prgbqPal[ui8Bit].rgbGreen;
									pRgb->ui8Rgba[SL2_PC_B] = prgbqPal[ui8Bit].rgbBlue;
									pRgb->ui8Rgba[SL2_PC_A] = bAlpha;
								}
							}
							break;
						}
						return SL2_E_INVALIDDATA;
					}
					case 8 : {
						if ( !AllocateTexture( CFormat::FindFormatDataByVulkan( SL2_VK_FORMAT_R8G8B8A8_UNORM ), ui32Width, ui32Height, ui32Depth ) ) { return SL2_E_OUTOFMEMORY; }
						uint8_t * pui8Dst = Data();
						const RGBQUAD * prgbqPal = ::FreeImage_GetPalette( flfmData.pbBitmap );
						uint32_t ui32Pitch = SL2_ROUND_UP( ui32Width * sizeof( CFormat::SL2_RGBA_UNORM ), 4 );
						if ( prgbqPal ) {
							for ( uint32_t Y = 0; Y < ui32Height; ++Y ) {
								const uint8_t * pui8Data = ::FreeImage_GetScanLine( flfmData.pbBitmap, Y );
								for ( uint32_t X = 0; X < ui32Width; ++X ) {
									uint8_t ui8Bit = pui8Data[X];
									BYTE bAlpha = prgbqPal[ui8Bit].rgbReserved;
									if ( iTransIndex != -1 ) {
										bAlpha = (iTransIndex == ui8Bit) ? 0 : 255;
									}
									CFormat::SL2_RGBA_UNORM * pRgb = reinterpret_cast<CFormat::SL2_RGBA_UNORM *>(pui8Dst + ui32Pitch * Y) + X;
									pRgb->ui8Rgba[SL2_PC_R] = prgbqPal[ui8Bit].rgbRed;
									pRgb->ui8Rgba[SL2_PC_G] = prgbqPal[ui8Bit].rgbGreen;
									pRgb->ui8Rgba[SL2_PC_B] = prgbqPal[ui8Bit].rgbBlue;
									pRgb->ui8Rgba[SL2_PC_A] = bAlpha;
								}
							}
							break;
						}
						return SL2_E_INVALIDDATA;
					}
					case 16 : {
						uint8_t * pui8Dst = Data();

						unsigned uRedMask = ::FreeImage_GetRedMask( flfmData.pbBitmap );
						unsigned uGreenMask = ::FreeImage_GetGreenMask( flfmData.pbBitmap );
						unsigned uBlueMask = ::FreeImage_GetBlueMask( flfmData.pbBitmap );
						uint32_t ui32Pitch = SL2_ROUND_UP( ui32Width * sizeof( uint16_t ), 4 );
						if ( (uRedMask == FI16_565_RED_MASK) && (uGreenMask == FI16_565_GREEN_MASK) && (uBlueMask == FI16_565_BLUE_MASK) ) {
							if ( !AllocateTexture( CFormat::FindFormatDataByVulkan( SL2_VK_FORMAT_R5G6B5_UNORM_PACK16 ), ui32Width, ui32Height, ui32Depth ) ) { return SL2_E_OUTOFMEMORY; }
							uint8_t * pui8Dst = Data();
							for ( uint32_t Y = 0; Y < ui32Height; ++Y ) {
								const uint16_t * pui16Data = reinterpret_cast<uint16_t *>(::FreeImage_GetScanLine( flfmData.pbBitmap, Y ));
								for ( uint32_t X = 0; X < ui32Width; ++X ) {
									CFormat::SL2_R5G6B5_PACKED * pRgb = reinterpret_cast<CFormat::SL2_R5G6B5_PACKED *>(pui8Dst + ui32Pitch * Y) + X;
									pRgb->ui16R = (pui16Data[X] & FI16_565_RED_MASK) >> FI16_565_RED_SHIFT;
									pRgb->ui16G = (pui16Data[X] & FI16_565_GREEN_MASK) >> FI16_565_GREEN_SHIFT;
									pRgb->ui16B = (pui16Data[X] & FI16_565_BLUE_MASK) >> FI16_565_BLUE_SHIFT;
								}
							}
						}
						else {
							if ( !AllocateTexture( CFormat::FindFormatDataByVulkan( SL2_VK_FORMAT_A1R5G5B5_UNORM_PACK16 ), ui32Width, ui32Height, ui32Depth ) ) { return SL2_E_OUTOFMEMORY; }
							uint8_t * pui8Dst = Data();
							for ( uint32_t Y = 0; Y < ui32Height; ++Y ) {
								const uint16_t * pui16Data = reinterpret_cast<uint16_t *>(::FreeImage_GetScanLine( flfmData.pbBitmap, Y ));
								for ( uint32_t X = 0; X < ui32Width; ++X ) {
									CFormat::SL2_A1R5G6B5_PACKED * pRgba = reinterpret_cast<CFormat::SL2_A1R5G6B5_PACKED *>(pui8Dst + ui32Pitch * Y) + X;
									pRgba->ui16R = (pui16Data[X] & FI16_555_RED_MASK) >> FI16_555_RED_SHIFT;
									pRgba->ui16G = (pui16Data[X] & FI16_555_GREEN_MASK) >> FI16_555_GREEN_SHIFT;
									pRgba->ui16B = (pui16Data[X] & FI16_555_BLUE_MASK) >> FI16_555_BLUE_SHIFT;
									pRgba->ui16A = (pui16Data[X] >> 7);
								}
							}
						}
						break;
					}
					case 24 : {
						if ( !AllocateTexture( CFormat::FindFormatDataByVulkan( SL2_VK_FORMAT_R8G8B8_UNORM ), ui32Width, ui32Height, ui32Depth ) ) { return SL2_E_OUTOFMEMORY; }
						uint8_t * pui8Dst = Data();
						uint32_t ui32Pitch = SL2_ROUND_UP( ui32Width * sizeof( CFormat::SL2_RGB_UNORM ), 4 );
						for ( uint32_t Y = 0; Y < ui32Height; ++Y ) {
							const RGBTRIPLE * prgbtData = reinterpret_cast<RGBTRIPLE *>(::FreeImage_GetScanLine( flfmData.pbBitmap, Y ));
							for ( uint32_t X = 0; X < ui32Width; ++X ) {
								CFormat::SL2_RGB_UNORM * pRgb = reinterpret_cast<CFormat::SL2_RGB_UNORM *>(pui8Dst + ui32Pitch * Y) + X;
								pRgb->ui8Rgb[SL2_PC_R] = prgbtData[X].rgbtRed;
								pRgb->ui8Rgb[SL2_PC_G] = prgbtData[X].rgbtGreen;
								pRgb->ui8Rgb[SL2_PC_B] = prgbtData[X].rgbtBlue;
							}
						}
						break;
					}
					case 32 : {
						if ( !AllocateTexture( CFormat::FindFormatDataByVulkan( SL2_VK_FORMAT_R8G8B8A8_UNORM ), ui32Width, ui32Height, ui32Depth ) ) { return SL2_E_OUTOFMEMORY; }
						uint8_t * pui8Dst = Data();
						uint32_t ui32Pitch = SL2_ROUND_UP( ui32Width * sizeof( CFormat::SL2_RGBA_UNORM ), 4 );
						for ( uint32_t Y = 0; Y < ui32Height; ++Y ) {
							const RGBQUAD * prgbqData = reinterpret_cast<RGBQUAD *>(::FreeImage_GetScanLine( flfmData.pbBitmap, Y ));
							for ( uint32_t X = 0; X < ui32Width; ++X ) {
								CFormat::SL2_RGBA_UNORM * pRgb = reinterpret_cast<CFormat::SL2_RGBA_UNORM *>(pui8Dst + ui32Pitch * Y) + X;
								pRgb->ui8Rgba[SL2_PC_R] = prgbqData[X].rgbRed;
								pRgb->ui8Rgba[SL2_PC_G] = prgbqData[X].rgbGreen;
								pRgb->ui8Rgba[SL2_PC_B] = prgbqData[X].rgbBlue;
								pRgb->ui8Rgba[SL2_PC_A] = prgbqData[X].rgbReserved;
							}
						}
						break;
					}
				}
				break;	// FIT_BITMAP
			}
			case FIT_UINT16 : {
				if ( !AllocateTexture( CFormat::FindFormatDataByVulkan( SL2_VK_FORMAT_R16_UNORM ), ui32Width, ui32Height, ui32Depth ) ) { return SL2_E_OUTOFMEMORY; }
				uint8_t * pui8Dst = Data();
				uint32_t ui32Pitch = SL2_ROUND_UP( ui32Width * sizeof( uint16_t ), 4 );
				for ( uint32_t Y = 0; Y < ui32Height; ++Y ) {
					const uint16_t * pui16Data = reinterpret_cast<uint16_t *>(::FreeImage_GetScanLine( flfmData.pbBitmap, Y ));
					for ( uint32_t X = 0; X < ui32Width; ++X ) {
						uint16_t * pRgb = reinterpret_cast<uint16_t *>(pui8Dst + ui32Pitch * Y) + X;
						pRgb[0] = pui16Data[X];
					}
				}
				break;	// FIT_UINT16
			}
			case FIT_INT16 : {
				if ( !AllocateTexture( CFormat::FindFormatDataByVulkan( SL2_VK_FORMAT_R16_SNORM ), ui32Width, ui32Height, ui32Depth ) ) { return SL2_E_OUTOFMEMORY; }
				uint8_t * pui8Dst = Data();
				uint32_t ui32Pitch = SL2_ROUND_UP( ui32Width * sizeof( uint16_t ), 4 );
				for ( uint32_t Y = 0; Y < ui32Height; ++Y ) {
					const int16_t * pi16Data = reinterpret_cast<int16_t *>(::FreeImage_GetScanLine( flfmData.pbBitmap, Y ));
					for ( uint32_t X = 0; X < ui32Width; ++X ) {
						int16_t * pRgb = reinterpret_cast<int16_t *>(pui8Dst + ui32Pitch * Y) + X;
						pRgb[0] = pi16Data[X];
					}
				}
				break;	// FIT_INT16
			}
			case FIT_UINT32 : {
				if ( !AllocateTexture( CFormat::FindFormatDataByVulkan( SL2_VK_FORMAT_R32_UINT ), ui32Width, ui32Height, ui32Depth ) ) { return SL2_E_OUTOFMEMORY; }
				uint8_t * pui8Dst = Data();
				uint32_t ui32Pitch = SL2_ROUND_UP( ui32Width * sizeof( uint32_t ), 4 );
				for ( uint32_t Y = 0; Y < ui32Height; ++Y ) {
					const uint32_t * pui32Data = reinterpret_cast<uint32_t *>(::FreeImage_GetScanLine( flfmData.pbBitmap, Y ));
					for ( uint32_t X = 0; X < ui32Width; ++X ) {
						uint32_t * pRgb = reinterpret_cast<uint32_t *>(pui8Dst + ui32Pitch * Y) + X;
						pRgb[0] = pui32Data[X];
					}
				}
				break;	// FIT_UINT32
			}
			case FIT_INT32 : {
				if ( !AllocateTexture( CFormat::FindFormatDataByVulkan( SL2_VK_FORMAT_R32_SINT ), ui32Width, ui32Height, ui32Depth ) ) { return SL2_E_OUTOFMEMORY; }
				uint8_t * pui8Dst = Data();
				uint32_t ui32Pitch = SL2_ROUND_UP( ui32Width * sizeof( uint32_t ), 4 );
				for ( uint32_t Y = 0; Y < ui32Height; ++Y ) {
					const int32_t * pi32Data = reinterpret_cast<int32_t *>(::FreeImage_GetScanLine( flfmData.pbBitmap, Y ));
					for ( uint32_t X = 0; X < ui32Width; ++X ) {
						int32_t * pRgb = reinterpret_cast<int32_t *>(pui8Dst + ui32Pitch * Y) + X;
						pRgb[0] = pi32Data[X];
					}
				}
				break;	// FIT_INT32
			}
			case FIT_FLOAT : {
				if ( !AllocateTexture( CFormat::FindFormatDataByVulkan( SL2_VK_FORMAT_R32_SFLOAT ), ui32Width, ui32Height, ui32Depth ) ) { return SL2_E_OUTOFMEMORY; }
				uint8_t * pui8Dst = Data();
				uint32_t ui32Pitch = SL2_ROUND_UP( ui32Width * sizeof( float ), 4 );
				for ( uint32_t Y = 0; Y < ui32Height; ++Y ) {
					const float * pfData = reinterpret_cast<float *>(::FreeImage_GetScanLine( flfmData.pbBitmap, Y ));
					for ( uint32_t X = 0; X < ui32Width; ++X ) {
						float fBit = pfData[X];
						float * pRgb = reinterpret_cast<float *>(pui8Dst + ui32Pitch * Y) + X;
						pRgb[0] = fBit;
					}
				}
				break;	// FIT_FLOAT
			}
			case FIT_DOUBLE : {
				if ( !AllocateTexture( CFormat::FindFormatDataByVulkan( SL2_VK_FORMAT_R64_SFLOAT ), ui32Width, ui32Height, ui32Depth ) ) { return SL2_E_OUTOFMEMORY; }
				uint8_t * pui8Dst = Data();
				uint32_t ui32Pitch = SL2_ROUND_UP( ui32Width * sizeof( double ), 4 );
				for ( uint32_t Y = 0; Y < ui32Height; ++Y ) {
					const double * pdData = reinterpret_cast<double *>(::FreeImage_GetScanLine( flfmData.pbBitmap, Y ));
					for ( uint32_t X = 0; X < ui32Width; ++X ) {
						double * pRgb = reinterpret_cast<double *>(pui8Dst + ui32Pitch) + X;
						pRgb[0] = pdData[X];
					}
				}
				break;	// FIT_DOUBLE
			}
			case FIT_COMPLEX : {
				if ( !AllocateTexture( CFormat::FindFormatDataByVulkan( SL2_VK_FORMAT_R64G64_SFLOAT ), ui32Width, ui32Height, ui32Depth ) ) { return SL2_E_OUTOFMEMORY; }
				uint8_t * pui8Dst = Data();
				uint32_t ui32Pitch = SL2_ROUND_UP( ui32Width * sizeof( FICOMPLEX ), 4 );
				for ( uint32_t Y = 0; Y < ui32Height; ++Y ) {
					const FICOMPLEX * pfData = reinterpret_cast<FICOMPLEX *>(::FreeImage_GetScanLine( flfmData.pbBitmap, Y ));
					for ( uint32_t X = 0; X < ui32Width; ++X ) {
						FICOMPLEX * pRgb = reinterpret_cast<FICOMPLEX *>(pui8Dst + ui32Pitch * Y) + X;
						(*pRgb) = pfData[X];
					}
				}
				break;	// FIT_COMPLEX
			}
			case FIT_RGB16 : {
				if ( !AllocateTexture( CFormat::FindFormatDataByVulkan( SL2_VK_FORMAT_R16G16B16_UNORM ), ui32Width, ui32Height, ui32Depth ) ) { return SL2_E_OUTOFMEMORY; }
				uint8_t * pui8Dst = Data();
				uint32_t ui32Pitch = SL2_ROUND_UP( ui32Width * sizeof( CFormat::SL2_RGB16_UNORM ), 4 );
				for ( uint32_t Y = 0; Y < ui32Height; ++Y ) {
					const FIRGB16 * pfrgb16Data = reinterpret_cast<FIRGB16 *>(::FreeImage_GetScanLine( flfmData.pbBitmap, Y ));
					for ( uint32_t X = 0; X < ui32Width; ++X ) {
						CFormat::SL2_RGB16_UNORM * pRgb = reinterpret_cast<CFormat::SL2_RGB16_UNORM *>(pui8Dst + ui32Pitch * Y) + X;
						pRgb->ui16Rgb[SL2_PC_R] = pfrgb16Data[X].red;
						pRgb->ui16Rgb[SL2_PC_G] = pfrgb16Data[X].green;
						pRgb->ui16Rgb[SL2_PC_B] = pfrgb16Data[X].blue;
					}
				}
				break;	// FIT_RGB16
			}
			case FIT_RGBA16 : {
				if ( !AllocateTexture( CFormat::FindFormatDataByVulkan( SL2_VK_FORMAT_R16G16B16A16_UNORM ), ui32Width, ui32Height, ui32Depth ) ) { return SL2_E_OUTOFMEMORY; }
				uint8_t * pui8Dst = Data();
				uint32_t ui32Pitch = SL2_ROUND_UP( ui32Width * sizeof( CFormat::SL2_RGB16_UNORM ), 4 );
				for ( uint32_t Y = 0; Y < ui32Height; ++Y ) {
					const FIRGBA16 * pfrgba16Data = reinterpret_cast<FIRGBA16 *>(::FreeImage_GetScanLine( flfmData.pbBitmap, Y ));
					for ( uint32_t X = 0; X < ui32Width; ++X ) {
						CFormat::SL2_RGBA16_UNORM * pRgb = reinterpret_cast<CFormat::SL2_RGBA16_UNORM *>(pui8Dst + ui32Pitch * Y) + X;
						pRgb->ui16Rgba[SL2_PC_R] = pfrgba16Data[X].red;
						pRgb->ui16Rgba[SL2_PC_G] = pfrgba16Data[X].green;
						pRgb->ui16Rgba[SL2_PC_B] = pfrgba16Data[X].blue;
						pRgb->ui16Rgba[SL2_PC_A] = pfrgba16Data[X].alpha;
					}
				}
				break;	// FIT_RGBA16
			}
			case FIT_RGBF : {
				if ( !AllocateTexture( CFormat::FindFormatDataByVulkan( SL2_VK_FORMAT_R32G32B32_SFLOAT ), ui32Width, ui32Height, ui32Depth ) ) { return SL2_E_OUTOFMEMORY; }
				uint8_t * pui8Dst = Data();
				uint32_t ui32Pitch = SL2_ROUND_UP( ui32Width * sizeof( CFormat::SL2_RGB ), 4 );
				for ( uint32_t Y = 0; Y < ui32Height; ++Y ) {
					const FIRGBF * pfData = reinterpret_cast<FIRGBF *>(::FreeImage_GetScanLine( flfmData.pbBitmap, Y ));
					for ( uint32_t X = 0; X < ui32Width; ++X ) {
						CFormat::SL2_RGB * pRgb = reinterpret_cast<CFormat::SL2_RGB *>(pui8Dst + ui32Pitch * Y) + X;
						pRgb->fRgb[SL2_PC_R] = pfData[X].red;
						pRgb->fRgb[SL2_PC_G] = pfData[X].green;
						pRgb->fRgb[SL2_PC_B] = pfData[X].blue;
					}
				}
				break;	// FIT_RGBF
			}
			case FIT_RGBAF : {
				if ( !AllocateTexture( CFormat::FindFormatDataByVulkan( SL2_VK_FORMAT_R32G32B32A32_SFLOAT ), ui32Width, ui32Height, ui32Depth ) ) { return SL2_E_OUTOFMEMORY; }
				uint8_t * pui8Dst = Data();
				uint32_t ui32Pitch = SL2_ROUND_UP( ui32Width * sizeof( CFormat::SL2_RGB ), 4 );
				for ( uint32_t Y = 0; Y < ui32Height; ++Y ) {
					const FIRGBAF * pfData = reinterpret_cast<FIRGBAF *>(::FreeImage_GetScanLine( flfmData.pbBitmap, Y ));
					for ( uint32_t X = 0; X < ui32Width; ++X ) {
						CFormat::SL2_RGBA * pRgb = reinterpret_cast<CFormat::SL2_RGBA *>(pui8Dst + ui32Pitch * Y) + X;
						pRgb->fRgba[SL2_PC_R] = pfData[X].red;
						pRgb->fRgba[SL2_PC_G] = pfData[X].green;
						pRgb->fRgba[SL2_PC_B] = pfData[X].blue;
						pRgb->fRgba[SL2_PC_A] = pfData[X].alpha;
					}
				}
				break;	// FIT_RGBAF
			}
			default : {}
		};

#undef FreeImage_GetScanLine

		return SL2_E_SUCCESS;
	}

	/**
	 * Loads a KTX1 file from memory.
	 * 
	 * \param _vData The file to load.
	 * \return Returns an error code.
	 **/
	SL2_ERRORS CImage::LoadKtx1( const std::vector<uint8_t> &_vData ) {
		CKtxTexture<ktxTexture1> kt1Tex;
		::KTX_error_code ecErr = ::ktxTexture1_CreateFromMemory( _vData.data(), _vData.size(),
			KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT,
			kt1Tex.HandlePointer() );
		if ( KTX_SUCCESS == ecErr && kt1Tex.Handle() ) {
			if ( !AllocateTexture( CFormat::FindFormatDataByOgl( static_cast<SL2_KTX_INTERNAL_FORMAT>(kt1Tex.Handle()->glInternalformat) ),
				kt1Tex.Handle()->baseWidth, kt1Tex.Handle()->baseHeight, kt1Tex.Handle()->baseDepth,
				kt1Tex.Handle()->numLevels, kt1Tex.Handle()->numLayers, kt1Tex.Handle()->numFaces ) ) { return SL2_E_OUTOFMEMORY; }

			if ( KTX_SUCCESS == ::ktxTexture_IterateLevelFaces( ktxTexture( kt1Tex.Handle() ), KtxImageLoad, this ) ) {
				return SL2_E_SUCCESS;
			}

		}
		return SL2_E_INVALIDFILETYPE;
	}

	/**
	 * Loads a KTX2 file from memory.
	 * 
	 * \param _vData The file to load.
	 * \return Returns an error code.
	 **/
	SL2_ERRORS CImage::LoadKtx2( const std::vector<uint8_t> &_vData ) {
		CKtxTexture<ktxTexture2> kt2Tex;
		::KTX_error_code ecErr = ::ktxTexture2_CreateFromMemory( _vData.data(), _vData.size(),
			KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT,
			kt2Tex.HandlePointer() );
		if ( KTX_SUCCESS == ecErr && kt2Tex.Handle() ) {
			if ( ::ktxTexture2_NeedsTranscoding( (*kt2Tex.HandlePointer()) ) ) {
				if ( KTX_SUCCESS != ::ktxTexture2_TranscodeBasis( (*kt2Tex.HandlePointer()), KTX_TTF_RGBA32, 0 ) ) {
					return SL2_E_INVALIDDATA;
				}
			}
			if ( !AllocateTexture( CFormat::FindFormatDataByVulkan( static_cast<SL2_VKFORMAT>(kt2Tex.Handle()->vkFormat) ),
				kt2Tex.Handle()->baseWidth, kt2Tex.Handle()->baseHeight, kt2Tex.Handle()->baseDepth,
				kt2Tex.Handle()->numLevels, kt2Tex.Handle()->numLayers, kt2Tex.Handle()->numFaces ) ) { return SL2_E_OUTOFMEMORY; }

			if ( KTX_SUCCESS == ::ktxTexture_IterateLevelFaces( ktxTexture( kt2Tex.Handle() ), KtxImageLoad, this ) ) {
				return SL2_E_SUCCESS;
			}
		}
		return SL2_E_INVALIDFILETYPE;
	}

	/**
	 * Loads a DDS file from memory.
	 * 
	 * \param _vData The file to load.
	 * \return Returns an error code.
	 **/
	SL2_ERRORS CImage::LoadDds( const std::vector<uint8_t> &_vData ) {
		CDds dFile;
		if ( !dFile.LoadDds( _vData ) ) { return SL2_E_INVALIDFILETYPE; }



		return SL2_E_SUCCESS;
	}

	/**
	 * Callback to load each face's data from a KTX file.
	 *
	 * \param _iMipLevel Mipmap level, starting from 0.
	 * \param _iFace Face index, usually 0, or 0-5 for cube maps.
	 * \param _iWidth Width of the image.
	 * \param _iHeight Height of the image.
	 * \param _iDepth Depth of the image.
	 * \param _ui64FaceLodSize Number of bytes to which _pvPixels points.
	 * \param _pvPixels The actual texture data.
	 * \param _pvUserdata User data passed to the callback for its own use.
	 * \return Returns an error code to indicate failure or success.
	 */
	::KTX_error_code CImage::KtxImageLoad( int _iMipLevel, int _iFace,
		int _iWidth, int _iHeight, int _iDepth,
		ktx_uint64_t _ui64FaceLodSize,
		void * _pvPixels, void * _pvUserdata ) {

		CImage * piImage = reinterpret_cast<CImage *>(_pvUserdata);
		size_t sSrcSize = CFormat::GetFormatSize( piImage->Format(), _iWidth, _iHeight, _iDepth );
		_ui64FaceLodSize /= piImage->ArraySize();
		if ( _ui64FaceLodSize < sSrcSize || 0 == sSrcSize ) {
			return ::KTX_FILE_READ_ERROR;
		}
		const uint8_t * pui8Src = reinterpret_cast<uint8_t *>(_pvPixels);
		for ( size_t I = 0; I < piImage->ArraySize(); ++I ) {
			uint8_t * pui8Dst = piImage->Data( _iMipLevel, 0, I, _iFace );
			std::memcpy( pui8Dst, pui8Src + _ui64FaceLodSize * I, sSrcSize );
		}
		return ::KTX_SUCCESS;
	}

}	// namespace sl2

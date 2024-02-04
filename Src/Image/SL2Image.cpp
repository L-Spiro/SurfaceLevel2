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
 * This class is the primary workhorse of the tool.  Image conversion and manipulation takes place here.
 */

#include "SL2Image.h"


namespace sl2 {

	CImage::CImage() :
		m_sArraySize( 0 ),
		m_ui32Depth( 0 ),
		m_sFaces( 0 ),
		m_ui32Width( 0 ),
		m_ui32Height( 0 ),
		m_pkifFormat( nullptr ) {
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
		m_ui32Width = _iOther.m_ui32Width;
		m_ui32Height = _iOther.m_ui32Height;
		m_ui32Depth = _iOther.m_ui32Depth;
		m_pkifFormat = _iOther.m_pkifFormat;
		_iOther.m_sArraySize = 0;
		_iOther.m_sFaces = 0;
		_iOther.m_ui32Width = 0;
		_iOther.m_ui32Height = 0;
		_iOther.m_ui32Depth = 0;
		_iOther.m_pkifFormat = nullptr;
		return (*this);
	}

	// == Functions.
	/**
	 * Resets the object to scratch.  It can be reused after this.
	 **/
	void CImage::Reset() {
		m_sArraySize = 0;
		m_sFaces = 0;
		m_ui32Width = 0;
		m_ui32Height = 0;
		m_ui32Depth = 0;
		m_vMipMaps = std::vector<std::unique_ptr<CSurface>>();
		m_pkifFormat = nullptr;
	}

	/**
	 * Allocates a texture of a given number of mipmaps, array indices, faces, and dimensions.
	 * 
	 * \param _pkifFormat DESC
	 * \param _ui32Width Width of the image base mipmap level.
	 * \param _ui32Height Height of the image base mipmap level.
	 * \param _ui32Depth Depth of the image base mipmap level.
	 * \param _sMips Number of mipmaps.  Must be at least 1.  If 0, a fully mipmap chain is allocated.
	 * \param _sArray Number of array slices.  Must be at least 1.
	 * \param _sFaces Number of faces.  Either 1 or 6.
	 * \return DESC
	 **/
	bool CImage::AllocateTexture( const CFormat::SL2_KTX_INTERNAL_FORMAT_DATA * _pkifFormat, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Depth, size_t _sMips, size_t _sArray, size_t _sFaces ) {
		if ( !_sMips ) {
			_sMips = CUtilities::Max( size_t( std::round( std::log2( _ui32Width ) ) ), size_t( std::round( std::log2( _ui32Height ) ) ) );
			_sMips = CUtilities::Max( size_t( std::round( std::log2( _ui32Depth ) ) ), _sMips );
		}
		if ( !_sMips ) { return false; }
		

		try {
			m_vMipMaps.resize( _sMips );
			for ( size_t I = 0; I < _sMips; ++I ) {
				size_t sBaseSize = CFormat::GetFormatSize( _pkifFormat, _ui32Width, _ui32Height, _ui32Depth );
				size_t sFullSize = sBaseSize * _sArray * _sFaces;
				if ( !sFullSize ) { return false; }

				m_vMipMaps[I] = std::make_unique<CSurface>( sFullSize );

				_ui32Width = CUtilities::Max<uint32_t>( _ui32Width >> 1, 1 );
				_ui32Height = CUtilities::Max<uint32_t>( _ui32Height >> 1, 1 );
				_ui32Depth = CUtilities::Max<uint32_t>( _ui32Depth >> 1, 1 );
			}
		}
		catch ( ... ) {
			return false;
		}
		return true;
	}

}	// namespace sl2

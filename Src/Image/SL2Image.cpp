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
		m_stArraySize( 0 ),
		m_stSlices( 0 ),
		m_stFaces( 0 ),
		m_ui32Width( 0 ),
		m_ui32Height( 0 ) {
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
		m_stArraySize = _iOther.m_stArraySize;
		m_stSlices = _iOther.m_stSlices;
		m_stFaces = _iOther.m_stFaces;
		m_ui32Width = _iOther.m_ui32Width;
		m_ui32Height = _iOther.m_ui32Height;
		_iOther.m_stArraySize = 0;
		_iOther.m_stSlices = 0;
		_iOther.m_stFaces = 0;
		_iOther.m_ui32Width = 0;
		_iOther.m_ui32Height = 0;
		return (*this);
	}

	// == Functions.
	/**
	 * Resets the object to scratch.  It can be reused after this.
	 **/
	void CImage::Reset() {
		m_stArraySize = 0;
		m_stSlices = 0;
		m_stFaces = 0;
		m_ui32Width = 0;
		m_ui32Height = 0;
		m_vMipMaps = std::vector<std::unique_ptr<std::vector<uint8_t>>>();
	}

}	// namespace sl2

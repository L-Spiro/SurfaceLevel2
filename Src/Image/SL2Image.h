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

#pragma once

#include "SL2Formats.h"
#include "SL2Surface.h"

#include <cstdint>
#include <FreeImage.h>
#include <ktx.h>
#include <memory>
#include <vector>


namespace sl2 {

	/**
	 * Class CImage
	 * \brief This class is the primary workhorse of the tool.
	 *
	 * Description: An image is a collection of surfaces:
	 *	1: Mipmaps
	 *	2: Array
	 *	3: 3D Slice
	 *	4: Cube Face
	 * 
	 * This class is the primary workhorse of the tool.  Image conversion and manipulation takes place here.
	 */
	class CImage {
	public :
		CImage();
		~CImage();


		// == Types.
		/** Wraps a FreeImage file. */
		struct SL2_FREE_IMAGE {
			SL2_FREE_IMAGE() :
				pmMemory( ::FreeImage_OpenMemory() ) {
			}
			SL2_FREE_IMAGE( const std::vector<uint8_t> &_vData ) :
				pmMemory( ::FreeImage_OpenMemory( const_cast<uint8_t *>(_vData.data()), DWORD( _vData.size() ) ) ) {
			}
			~SL2_FREE_IMAGE() {
				::FreeImage_CloseMemory( pmMemory );
				pmMemory = nullptr;
			}


			// == Members.
			FIMEMORY *										pmMemory;
		};

		/** Wraps FreeImage_LoadFromMemory(). */
		struct SL2_FREEIMAGE_LOAD_FROM_MEMORY {
			SL2_FREEIMAGE_LOAD_FROM_MEMORY( SL2_FREE_IMAGE &_fiImage ) :
				pbBitmap( ::FreeImage_LoadFromMemory( ::FreeImage_GetFileTypeFromMemory( _fiImage.pmMemory, 0 ), _fiImage.pmMemory, 0 ) ) {
			}
			~SL2_FREEIMAGE_LOAD_FROM_MEMORY() {
				::FreeImage_Unload( pbBitmap );
				pbBitmap = nullptr;
			}


			// == Members.
			FIBITMAP *										pbBitmap;
		};

		/** Wraps FreeImage_Allocate(). */
		struct SL2_FREEIMAGE_ALLOCATE {
			SL2_FREEIMAGE_ALLOCATE( int _iWidth, int _iHeight, int _iBpp, unsigned _uRedMask FI_DEFAULT(0), unsigned _uGreenMask FI_DEFAULT(0), unsigned _uBlueMask FI_DEFAULT(0) ) :
				pbBitmap( ::FreeImage_Allocate( _iWidth, _iHeight, _iBpp, _uRedMask, _uGreenMask, _uBlueMask ) ) {
			}
			~SL2_FREEIMAGE_ALLOCATE() {
				::FreeImage_Unload( pbBitmap );
				pbBitmap = nullptr;
			}


			// == Members.
			FIBITMAP *										pbBitmap;
		};

		/** Wraps FreeImage_AllocateT(). */
		struct SL2_FREEIMAGE_ALLOCATET {
			SL2_FREEIMAGE_ALLOCATET( FREE_IMAGE_TYPE _fitType, int _iWidth, int _iHeight, int _iBpp, unsigned _uRedMask FI_DEFAULT(0), unsigned _uGreenMask FI_DEFAULT(0), unsigned _uBlueMask FI_DEFAULT(0) ) :
				pbBitmap( ::FreeImage_AllocateT( _fitType, _iWidth, _iHeight, _iBpp, _uRedMask, _uGreenMask, _uBlueMask ) ) {
			}
			~SL2_FREEIMAGE_ALLOCATET() {
				::FreeImage_Unload( pbBitmap );
				pbBitmap = nullptr;
			}


			// == Members.
			FIBITMAP *										pbBitmap;
		};


		// == Operators.
		/**
		 * Move operator.
		 * 
		 * \param _iOther The image to copy and destroy.
		 * \return Returns a reference to this image.
		 **/
		CImage &											operator = ( CImage &&_iOther );


		// == Functions.
		/**
		 * Resets the object to scratch.  It can be reused after this.
		 **/
		void												Reset();

		/**
		 * Gets the width of the base texture.
		 * 
		 * \return Returns the width of mip-map 0.
		 **/
		inline uint32_t										Width() const;

		/**
		 * Gets the height of the base texture.
		 * 
		 * \return Returns the height of mip-map 0.
		 **/
		inline uint32_t										Height() const;

		/**
		 * Gets the depth of the base texture.
		 * 
		 * \return Returns the depth of mip-map 0.
		 **/
		inline uint32_t										Depth() const;

		/**
		 * Gets the number of mipmaps.
		 * 
		 * \return Returns the number of mipmaps.
		 **/
		inline size_t										Mipmaps() const;

		/**
		 * Gets the number of array elements.
		 * 
		 * \return Returns the number of array elements.
		 **/
		inline size_t										ArraySize() const;

		/**
		 * Gets the number of faces.
		 * 
		 * \return Returns the number of faces.
		 **/
		inline size_t										Faces() const;

		/**
		 * Gets the texture format.
		 * 
		 * \return Returns a constant pointer to the format data, which may be nullptr.
		 **/
		const CFormat::SL2_KTX_INTERNAL_FORMAT_DATA *		Format() const;

		/**
		 * Gets the data buffer for a given mipmap, slice, face, and array index.
		 * 
		 * \param _sMip The mipmap level to retrieve.
		 * \param _ui32Slice The 3D slice to access.
		 * \param _sArray The array index.
		 * \param _sFace The face index.
		 * \return Returns a pointer to the given 2D texture face or nullptr.
		 **/
		inline uint8_t *									Data( size_t _sMip = 0, uint32_t _ui32Slice = 0, size_t _sArray = 0, size_t _sFace = 0 );

		/**
		 * Loads an image file.  All image slices, faces, and array slices will be loaded.
		 * 
		 * \param _pcFile The name of the file to load.
		 * \return Returns an error code.
		 **/
		SL2_ERRORS											LoadFile( const char16_t * _pcFile );

		/**
		 * Loads an image file.  All image slices, faces, and array slices will be loaded.
		 * 
		 * \param _vData The image file to load.
		 * \return Returns an error code.
		 **/
		SL2_ERRORS											LoadFile( const std::vector<uint8_t> &_vData );

		/**
		 * Converts to another format.  _iDst holds the converted image.
		 * 
		 * \param _pkifFormat The format to which to convert.
		 * \param _iDst Holds the converted image.
		 * \return Returns an error code.
		 **/
		SL2_ERRORS											ConvertToFormat( const CFormat::SL2_KTX_INTERNAL_FORMAT_DATA * _pkifFormat, CImage &_iDst );

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
		SL2_ERRORS											ConvertToFormat( const CFormat::SL2_KTX_INTERNAL_FORMAT_DATA * _pkifFormat,
			size_t _sMip, size_t _sArray, size_t _sFace, std::vector<uint8_t> &_vDst, bool _bInvertY = false );

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
		SL2_ERRORS											ConvertToFormat( const CFormat::SL2_KTX_INTERNAL_FORMAT_DATA * _pkifFormat,
			size_t _sMip, size_t _sArray, size_t _sFace, uint8_t * _pui8Dst, bool _bInvertY = false );

		/**
		 * Gets a constant reference to the image's mipmaps.
		 * 
		 * \return Returns a constant reference to the image's mipmaps.
		 **/
		const std::vector<std::unique_ptr<CSurface>> &		GetMipmaps() const { return m_vMipMaps; }

		/**
		 * Gets the user-supplied gamma value.
		 * 
		 * \return Returns the user-supplied gamma value.
		 **/
		inline double										Gamma() const { return m_dGamma; }

		/**
		 * Sets the user-supplied gamma value.
		 * 
		 * \param _dGamma The user-supplied gamma.
		 **/
		inline void											SetGamma( double _dGamma ) { m_dGamma = _dGamma; }

		/**
		 * Sets whether or not alpha needs to be pre-multiplied.
		 * 
		 * \param _bPreMult If true, alpha will be premultiplied.
		 **/
		inline void											SetNeedsPreMultiply( bool _bPreMult ) { m_bNeedsPreMultiply = _bPreMult; }

		/**
		 * Sets whether or not alpha is already pre-multiplied.
		 * 
		 * \param _bPreMult If true, alpha is premultiplied.
		 **/
		inline void											SetIsPreMultiply( bool _bPreMult ) { m_bIsPreMultiplied = _bPreMult; }

		/**
		 * Sets the swizzle to apply.
		 * 
		 * \param _sSwizzle The swizzle to set.
		 **/
		inline void											SetSwizzle( const CFormat::SL2_SWIZZLE &_sSwizzle ) { m_sSwizzle = _sSwizzle; }

		/**
		 * Sets whether to swap R and B or not.
		 * 
		 * \param _bSwap If true, R and B will be swapped.
		 **/
		inline void											SetSwap( bool _bSwap ) { m_bSwap = _bSwap; }

		/**
		 * Gets the final size of a byte buffer to be used as a texture plane.  The plane will be over-allocated by 8 bytes and then rounded up to the nearest 8 bytes.
		 *	So if a 1-by-1 32-bit tecture is being allocated, 4 will be passed to _sSize, and 16 will be returned ((4+8) -> 16).
		 * 
		 * \param _sSize The base size of the byte buffer previously calculated by the texture's width, height, depth, and texel size.
		 * \return Returns the adjusted size of the texture plane to actually be allocated.
		 **/
		static inline size_t								GetActualPlaneSize( size_t _sSize );

		


	protected :
		// == Members.
		double												m_dGamma;								/**< The gamma curve.  Negative values indicate the IEC 61966-2-1:1999 sRGB curve. */
		const CFormat::SL2_KTX_INTERNAL_FORMAT_DATA *		m_pkifFormat;							/**< The texture format. */
		std::vector<std::unique_ptr<CSurface>>				m_vMipMaps;								/**< The array of mipmaps.  Index 0 is the base level. */
		size_t												m_sArraySize;							/**< Number of slices in an array.  1 for flat 1D/2D images. */
		size_t												m_sFaces;								/**< 1 for normal textures, 6 for cube textures. */
		CFormat::SL2_SWIZZLE								m_sSwizzle;								/**< Swizzle setting. */
		bool												m_bIsPreMultiplied;						/**< Is the image already pre-multiplied? */
		bool												m_bNeedsPreMultiply;					/**< Does the image need to be pre-multiplied? */
		bool												m_bFlipX;								/**< Flip horizontally? */
		bool												m_bFlipY;								/**< Flip vertically? */
		bool												m_bFlipZ;								/**< Flip depth? */
		bool												m_bSwap;								/**< Swap R and B? */


		// == Functions.
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
		bool												AllocateTexture( const CFormat::SL2_KTX_INTERNAL_FORMAT_DATA * _pkifFormat, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Depth, size_t _sMips = 1, size_t _sArray = 1, size_t _sFaces = 1 );

		/**
		 * Bakes the image special gamma into a given texture buffer.  The format must be RGBA64F.
		 * 
		 * \param _pui8Buffer The texture texels.
		 * \param _dGamma The gamma to apply.
		 * \param _ui32Width The width of the image.
		 * \param _ui32Height The height of the image.
		 * \param _ui32Depth The depth of the image.
		 **/
		void												BakeGamma( uint8_t * _pui8Buffer, double _dGamma, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Depth );

		/**
		 * Loads using the FreeImage library.
		 * 
		 * \param _vData The file to load.
		 * \return Returns an error code.
		 **/
		SL2_ERRORS											LoadFreeImage( const std::vector<uint8_t> &_vData );

		/**
		 * Loads a KTX1 file from memory.
		 * 
		 * \param _vData The file to load.
		 * \return Returns an error code.
		 **/
		SL2_ERRORS											LoadKtx1( const std::vector<uint8_t> &_vData );

		/**
		 * Loads a KTX2 file from memory.
		 * 
		 * \param _vData The file to load.
		 * \return Returns an error code.
		 **/
		SL2_ERRORS											LoadKtx2( const std::vector<uint8_t> &_vData );

		/**
		 * Loads a DDS file from memory.
		 * 
		 * \param _vData The file to load.
		 * \return Returns an error code.
		 **/
		SL2_ERRORS											LoadDds( const std::vector<uint8_t> &_vData );

		/**
		 * Loads a BMP file from memory.
		 * 
		 * \param _vData The file to load.
		 * \return Returns an error code.
		 **/
		SL2_ERRORS											LoadBmp( const std::vector<uint8_t> &_vData );

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
		static ::KTX_error_code								KtxImageLoad( int _iMipLevel, int _iFace,
			int _iWidth, int _iHeight, int _iDepth,
			ktx_uint64_t _ui64FaceLodSize,
			void * _pvPixels, void * _pvUserdata );
	};
	


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Functions.
	/**
	 * Gets the width of the base texture.
	 * 
	 * \return Returns the width of mip-map 0.
	 **/
	inline uint32_t CImage::Width() const { return m_vMipMaps.size() ? m_vMipMaps[0].get()->Width() : 0; }

	/**
	 * Gets the height of the base texture.
	 * 
	 * \return Returns the height of mip-map 0.
	 **/
	inline uint32_t CImage::Height() const { return m_vMipMaps.size() ? m_vMipMaps[0].get()->Height() : 0; }

	/**
	 * Gets the depth of the base texture.
	 * 
	 * \return Returns the depth of mip-map 0.
	 **/
	inline uint32_t CImage::Depth() const { return m_vMipMaps.size() ? m_vMipMaps[0].get()->Depth() : 0; }

	/**
	 * Gets the number of mipmaps.
	 * 
	 * \return Returns the number of mipmaps.
	 **/
	inline size_t CImage::Mipmaps() const { return m_vMipMaps.size(); }

	/**
	 * Gets the number of array elements.
	 * 
	 * \return Returns the number of array elements.
	 **/
	inline size_t CImage::ArraySize() const { return m_sArraySize; }

	/**
	 * Gets the number of faces.
	 * 
	 * \return Returns the number of faces.
	 **/
	inline size_t CImage::Faces() const { return m_sFaces; }

	/**
	 * Gets the texture format.
	 * 
	 * \return Returns a constant pointer to the format data, which may be nullptr.
	 **/
	inline const CFormat::SL2_KTX_INTERNAL_FORMAT_DATA * CImage::Format() const { return m_pkifFormat; }

	/**
	 * Gets the data buffer for a given mipmap, slice, face, and array index.
	 * 
	 * \param _sMip The mipmap level to retrieve.
	 * \param _ui32Slice The 3D slice to access.
	 * \param _sArray The array index.
	 * \param _sFace The face index.
	 * \return Returns a pointer to the given 2D texture face or nullptr.
	 **/
	inline uint8_t * CImage::Data( size_t _sMip, uint32_t _ui32Slice, size_t _sArray, size_t _sFace ) {
		if ( _sMip >= m_vMipMaps.size() || nullptr == m_pkifFormat ) { return nullptr; }
		size_t sOff = m_vMipMaps[_sMip]->BaseSize() * ((_sArray * m_sFaces) +
			_sFace) +
			CFormat::GetFormatSize( m_pkifFormat, m_vMipMaps[_sMip]->Width(), m_vMipMaps[_sMip]->Height(), _ui32Slice );
		if ( sOff >= m_vMipMaps[_sMip]->size() ) { return nullptr; }
		return m_vMipMaps[_sMip]->data() + sOff;
	}

	/**
	 * Gets the final size of a byte buffer to be used as a texture plane.  The plane will be over-allocated by 8 bytes and then rounded up to the nearest 8 bytes.
	 *	So if a 1-by-1 32-bit tecture is being allocated, 4 will be passed to _sSize, and 16 will be returned ((4+8) -> 16).
	 * 
	 * \param _sSize The base size of the byte buffer previously calculated by the texture's width, height, depth, and texel size.
	 * \return Returns the adjusted size of the texture plane to actually be allocated.
	 **/
	inline size_t CImage::GetActualPlaneSize( size_t _sSize ) {
		if ( !_sSize ) { return 0; }
		return ((_sSize + sizeof( uint64_t )) + (sizeof( uint64_t ) - 1)) & ~(sizeof( uint64_t ) - 1);
	}

}	// namespace sl2

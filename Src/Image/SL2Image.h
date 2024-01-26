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

#pragma once

#include <cstdint>
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
		 * Gets the final size of a byte buffer to be used as a texture plane.  The plane will be over-allocated by 8 bytes and then rounded up to the nearest 8 bytes.
		 *	So if a 1-by-1 32-bit tecture is being allocated, 4 will be passed to _sSize, and 16 will be returned ((4+8) -> 16).
		 * 
		 * \param _sSize The base size of the byte buffer previously calculated by the texture's width, height, and texel size.
		 * \return Returns the adjusted size of the texture plane to actually be allocated.
		 **/
		static inline size_t								GetActualPlaneSize( size_t _sSize );

		/**
		 * Allocates N mipmaps.
		 * 
		 * \param PARM DESC
		 * \param PARM DESC
		 * \return DESC
		 **/


	protected :
		// == Members.
		std::vector<std::unique_ptr<std::vector<uint8_t>>>	m_vMipMaps;								/**< The array of mipmaps.  Index 0 is the base level. */
		size_t												m_stArraySize;							/**< Number of slices in an array.  1 for flat 1D/2D images. */
		size_t												m_stSlices;								/**< The number of slices in the image. */
		size_t												m_stFaces;								/**< 1 for normal textures, 6 for cube textures. */
		uint32_t											m_ui32Width;							/**< The width of the texture. */
		uint32_t											m_ui32Height;							/**< The height of the texture. */
	};
	


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Functions.
	/**
	 * Gets the final size of a byte buffer to be used as a texture plane.  The plane will be over-allocated by 8 bytes and then rounded up to the nearest 8 bytes.
	 *	So if a 1-by-1 32-bit tecture is being allocated, 4 will be passed to _sSize, and 16 will be returned ((4+8) -> 16).
	 * 
	 * \param _sSize The base size of the byte buffer previously calculated by the texture's width, height, and texel size.
	 * \return Returns the adjusted size of the texture plane to actually be allocated.
	 **/
	inline size_t CImage::GetActualPlaneSize( size_t _sSize ) {
		return ((_sSize + sizeof( uint64_t )) + (sizeof( uint64_t ) - 1)) & ~(sizeof( uint64_t ) - 1);
	}

}	// namespace sl2

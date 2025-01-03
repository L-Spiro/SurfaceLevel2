/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A surface is a single slice of an image.  It is basically a chunk of memory that can be treated as any arrangement of texel data needed.
 */

#pragma once

#include "../Utilities/SL2AlignmentAllocator.h"

#include <vector>


namespace sl2 {

	/**
	 * Class CSurface
	 * \brief A surface is a single slice of an image.
	 *
	 * Description: A surface is a single slice of an image.  It is basically a chunk of memory that can be treated as any arrangement of texel data needed.
	 */
	//typedef std::vector<uint8_t>						CSurface;
	class CSurface : public std::vector<uint8_t, CAlignmentAllocator<uint8_t, 64>> {
	public :
		CSurface( size_t _sAllocSize, size_t _sBaseSize, uint32_t _ui32W, uint32_t _ui32H, uint32_t _ui32D ) :
			std::vector<uint8_t, CAlignmentAllocator<uint8_t, 64>>( _sAllocSize ),
			m_sBaseSize( _sBaseSize ),
			m_ui32Width( _ui32W ),
			m_ui32Height( _ui32H ),
			m_ui32Depth( _ui32D ) {
		}


		// == Functions.
		/**
		 * Gets the base size (total size of WxHxD texels)
		 * 
		 * \return Returns the size in bytes of WxHxD texels.
		 **/
		inline size_t									BaseSize() const { return m_sBaseSize; }

		/**
		 * Gets the width of the textures in this surface.
		 * 
		 * \return Returns the width of the textures in this surface.
		 **/
		inline uint32_t									Width() const { return m_ui32Width; }

		/**
		 * Gets the height of the textures in this surface.
		 * 
		 * \return Returns the height of the textures in this surface.
		 **/
		inline uint32_t									Height() const { return m_ui32Height; }

		/**
		 * Gets the depth of the textures in this surface.
		 * 
		 * \return Returns the depth of the textures in this surface.
		 **/
		inline uint32_t									Depth() const { return m_ui32Depth; }

		/**
		 * Reallocates
		 * 
		 * \param _sAllocSize The actual size to allocate, which may include some extra overflow bytes.
		 * \param _sBaseSize The actual size of the allocated surface.
		 * \param _ui32W Width of the surface.
		 * \param _ui32H Height of the surface.
		 * \param _ui32D Depth of the surface.
		 * \return Returns true if the allocation succeeds.
		 **/
		inline bool										Reallocate( size_t _sAllocSize, size_t _sBaseSize, uint32_t _ui32W, uint32_t _ui32H, uint32_t _ui32D ) {
			try {
				resize( _sAllocSize );
				m_sBaseSize = _sBaseSize;
				m_ui32Width = _ui32W;
				m_ui32Height = _ui32H;
				m_ui32Depth = _ui32D;
				return true;
			}
			catch ( ... ) { return false; }
		}


	protected :
		// == Members.
		size_t											m_sBaseSize;				/**< The size of a single WxHxD texture. */
		uint32_t										m_ui32Width;				/**< The width of each face in this surface. */
		uint32_t										m_ui32Height;				/**< The height of each face in this surface. */
		uint32_t										m_ui32Depth;				/**< The depth of each face in this surface. */
	};

}	// namespace sl2

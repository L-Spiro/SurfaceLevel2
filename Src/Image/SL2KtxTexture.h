/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Handles loading of version-1 .KTX files.  Wraps ktxTexture1 and ktxTexture2 objects for RAII purposes.
 */

#pragma once

#include <ktx.h>


namespace sl2 {

	/**
	 * Class CKtxTexture
	 * \brief Handles loading of version-1 .KTX files.
	 *
	 * Description: Handles loading of version-1 .KTX files.  Wraps ktxTexture1 and ktxTexture2 objects for RAII purposes.
	 */
	template <typename _tT>
	class CKtxTexture {
	public :
		CKtxTexture() :
			m_ptHandle( nullptr ) {
		}
		~CKtxTexture() {
			if ( m_ptHandle ) {
				ktxTexture_Destroy( ktxTexture( m_ptHandle ) );
				m_ptHandle = nullptr;
			}
		}


		// == Functions.
		/**
		 * Gets a pointer to the handle pointer for use with functions such as ::ktxTexture1_CreateFromMemory().
		 *
		 * \return Returns a pointer to the handle pointer for use with functions such as ::ktxTexture1_CreateFromMemory().
		 */
		inline _tT **					HandlePointer() { return &m_ptHandle; }

		/**
		 * Gets the texture handle pointer.
		 *
		 * \return Returns the texture handle pointer.
		 */
		inline const _tT *				Handle() const { return m_ptHandle; }

		/**
		 * Returns this object as a pointer to the handle for easier dereferencing.
		 *
		 * \return Returns this object as a pointer to the handle for easier dereferencing.
		 */
		inline							operator _tT * () const { return m_ptHandle; }


	private :
		// == Members.
		/** The texture object pointer. */
		_tT *							m_ptHandle;
	};

}	// namespace sl2

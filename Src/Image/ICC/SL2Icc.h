/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Loading and working with ICC profiles.
 */


#pragma once

#include "SL2IccDefs.h"
#include "../../OS/SL2Os.h"

#include <cstdint>
#include <vector>


namespace sl2 {

	/**
	 * Class CIcc
	 * \brief Loading and working with ICC profiles.
	 *
	 * Description: Loading and working with ICC profiles.
	 */
	class CIcc {
	public :
		CIcc() {}
		~CIcc() {}


		// == Functions.
		/**
		 * Gets the offset and size of the given tag.
		 *
		 * \param _pui8Profile The ICC profile.
		 * \param _sProfileSize The size of the ICC profile.
		 * \param _ui32Sig The tag whose data is to be found.
		 * \param _sSize Holds teh returned size of the curve data or 0 upon failure.
		 * \return Returns the offset of the curve data or 0 upon failure.
		 **/
		static size_t											GetTagDataOffset( const uint8_t * _pui8Profile, size_t _sProfileSize,
			uint32_t _ui32Sig, size_t &_sSize );

	};

}	// namespace sl2

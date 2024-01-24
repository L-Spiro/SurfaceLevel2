/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A surface is a single slice of an image.  It is basically a chunk of memory that can be treated as any arrangement of texel data needed.
 */

#pragma once

#include <vector>


namespace sl2 {

	/**
	 * Class CSurface
	 * \brief A surface is a single slice of an image.
	 *
	 * Description: A surface is a single slice of an image.  It is basically a chunk of memory that can be treated as any arrangement of texel data needed.
	 */
	typedef std::vector<uint8_t>						CSurface;
	/*class CSurface {
	public :

	};*/

}	// namespace sl2

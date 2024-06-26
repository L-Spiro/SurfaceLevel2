/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: All of the image formats, along with conversion routines and functions for working with the formats (encoding, decoding, etc.)
 */

#include "SL2TextureAddressing.h"


namespace sl2 {

	// == Members.
	/** Table of addressing functions, indexable via the SL2_TEXTURE_ADDRESSING enumeration. */
	CTextureAddressing::PfAddressingFunc CTextureAddressing::m_pfFuncs[SL2_TA_TOTAL] {
		CTextureAddressing::Wrap,
		CTextureAddressing::Mirror,
		CTextureAddressing::Clamp,
		CTextureAddressing::Border,
		CTextureAddressing::MirrorOnce,
		CTextureAddressing::NullBorder,
	};

}	// namespace sl2

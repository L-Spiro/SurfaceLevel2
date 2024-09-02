/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A palette and palette functionality.
 */


#include "SL2Palette.h"

namespace sl2 {

	CPalette::CPalette() {
	}
	CPalette::~CPalette() {
		Reset();
	}

	// == Functions.
	/**
	 * Resets the palette back to scratch.
	 **/
	void CPalette::Reset() {
		m_pPalette.clear();
		m_pPalette = CPal();
		m_pkifFormat = nullptr;
	}

}	// namespace sl2

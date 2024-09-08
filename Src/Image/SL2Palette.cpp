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

	/**
	 * Sets the size of the palette.
	 * 
	 * \param _ui32Size The new size of the palette.
	 * \return Returns true if allocation of the new palette size succeds.
	 **/
	bool CPalette::SetSize( uint32_t _ui32Size ) {
		size_t sStart = m_pPalette.size();
		try {
		}
		catch ( ... ) { return false; }
		for ( auto I = sStart; I < m_pPalette.size(); ++I ) {
			m_pPalette[I].Zero();
		}
		return true;
	}

}	// namespace sl2

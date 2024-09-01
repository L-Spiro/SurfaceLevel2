/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A palette and palette functionality.
 */

#pragma once

#include "../Utilities/SL2Vector4.h"

#include <vector>


namespace sl2 {

	/**
	 * Class CImage
	 * \brief A palette and palette functionality.
	 *
	 * Description: A palette and palette functionality.
	 */
	class CPalette {
	public :
		CPalette();
		~CPalette();


		// == Types.
		/** A color. */
		typedef CVector4<SL2_ST_AVX512>							CColor;

		/** The palette. */
		typedef std::vector<CColor>								CPal;


		// == Functions.
		/**
		 * Appends a color to the palette.
		 * 
		 * \param _cColor The color to add.
		 * \return Returns true if the color was appended successfully.
		 **/
		inline bool												Add( const CColor &_cColor );

		/**
		 * Resets the palette back to scratch.
		 **/
		void													Reset();

		/**
		 * Gets a constant pointer to the array of colors.
		 * 
		 * \return Returns a constant pointer to the array of colors.
		 **/
		inline const CPal &										Palette() const { return m_pPalette; }


	protected :
		/** The actual palette. */
		CPal													m_pPalette;
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Functions.
	/**
	 * Appends a color to the palette.
	 * 
	 * \param _cColor The color to add.
	 * \return Returns true if the color was appended successfully.
	 **/
	inline bool CPalette::Add( const CColor &_cColor ) {
		try {
			m_pPalette.push_back( _cColor );
		}
		catch ( ... ) { return false; }
	}

}	// namespace sl2

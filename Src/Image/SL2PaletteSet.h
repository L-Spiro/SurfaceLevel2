/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A set/database of palettes.
 */

#pragma once

#include "SL2Palette.h"

#include <vector>


namespace sl2 {

	/**
	 * Class CPaletteSet
	 * \brief A set/database of palettes.
	 *
	 * Description: A set/database of palettes.
	 */
	class CPaletteSet {
	public :
		CPaletteSet();
		~CPaletteSet();


		// == Functions.
		/**
		 * Resets the palette back to scratch.
		 **/
		void												Reset();

		/**
		 * Gets the active palette.
		 * 
		 * \return Returns a reference to the active palette.
		 **/
		CPalette &											Palette() { return m_vPalettes[m_sActive]; }

		/**
		 * Gets the active palette.
		 * 
		 * \return Returns a reference to the active palette.
		 **/
		const CPalette &									Palette() const { return m_vPalettes[m_sActive]; }

		/**
		 * Appends a color to the active palette.
		 * 
		 * \param _cColor The color to add.
		 * \return Returns true if the color was appended successfully.
		 **/
		inline bool											Add( const CPalette::CColor &_cColor ) { return Palette().Add( _cColor ); }

		/**
		 * Loads a palette file (.PAL or .PPL).
		 * 
		 * \param _sPath The path to the file to load.
		 * \return Returns true if the file was loaded.
		 **/
		bool												LoadPaletteFromFile( const std::u16string &_sPath );


	protected :
		// == Members.
		std::vector<CPalette>								m_vPalettes;					/**< The array of palettes.  There is always at least 1. */
		size_t												m_sActive = 0;					/**< The active palette. */


		// == Functions.
		/**
		 * Loads a palette file (.PAL or .PPL).
		 * 
		 * \param _sFile The file to load.
		 * \param _pPal Palette into which to load the file.
		 * \return Returns true if the file was loaded.
		 **/
		bool												LoadPaletteFromFile( const CStream &_sFile, CPalette &_pPal );
	};

}	// namespace sl2

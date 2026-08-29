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


		// == Types.
		/** Bitmap palette data. */
		typedef union SL2_BITMAPPALETTE {
#pragma pack( push, 1 )
			struct SL2_BM_COLOR {
				uint8_t										ui8Red,
															ui8Green,
															ui8Blue,
															ui8Alpha;
			}												s;
			uint32_t										ui32Color;
#pragma pack( pop )
		} * LPSL2_BITMAPPALETTE, * const LPCSL2_BITMAPPALETTE;

		/** A chunk header. */
		struct SL2_CHUNK {
#pragma pack( push, 1 )
			uint32_t										ui32Name;				/**< The chunk name. */
			uint32_t										ui32Size;				/**< The chunk size. */
#pragma pack( pop )
		};


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
		 * Gets the total number of palettes in the set.
		 * 
		 * \return Returns the number of palettes in the set.
		 **/
		inline size_t										Total() const { return m_vPalettes.size(); }

		/**
		 * Sets the active palette.
		 * 
		 * \param _sIdx the index of the palette to activate.
		 * \return Returns true if the given index is valid.
		 **/
		inline bool											SetActive( size_t _sIdx ) {
			if ( _sIdx >= Total() ) { return false; }
			m_sActive = _sIdx;
			return true;
		}

		/**
		 * Sets the active palette by ID.
		 * 
		 * \param _ui32Id The ID of the palette to activate.
		 * \return Returns true if the given palette ID was found.
		 **/
		inline bool											SetActiveById( uint32_t _ui32Id ) {
			for ( auto I = m_vPalettes.size(); I--; ) {
				if ( m_vPalettes[I].Id() == _ui32Id ) { m_sActive = I; return true; }
			}
			return false;
		}

		/**
		 * Loads a palette file (.PAL or .PPL).
		 * 
		 * \param _sPath The path to the file to load.
		 * \return Returns true if the file was loaded.
		 **/
		bool												LoadPaletteFromFile( const std::u16string &_sPath );

		/**
		 * Loads a palette stream.
		 * 
		 * \param _sFile The stream from which to load the file.
		 * \return Returns true if the file was loaded.
		 **/
		bool												LoadPaletteFromStream( const CStream &_sFile );


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

		/**
		 * Loads a RIFF palette.
		 * 
		 * \param _sFile The file to load.
		 * \param _pPal Palette into which to load the file.
		 * \return Returns true if the file was loaded.
		 **/
		bool												LoadRiff( const CStream &_sFile, CPalette &_pPal );

		/**
		 * Loads a PL98 palette.
		 * 
		 * \param _sFile The file to load.
		 * \param _pPal Palette into which to load the file.
		 * \return Returns true if the file was loaded.
		 **/
		bool												LoadPl98( const CStream &_sFile, CPalette &_pPal );
	};

}	// namespace sl2

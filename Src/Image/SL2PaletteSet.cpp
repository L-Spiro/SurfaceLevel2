/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A set/database of palettes.
 */


#include "SL2PaletteSet.h"
#include "../Files/SL2StdFile.h"

namespace sl2 {

	CPaletteSet::CPaletteSet() {
		m_vPalettes.push_back( CPalette() );
	}
	CPaletteSet::~CPaletteSet() {
		Reset();
	}

	// == Functions.
	/**
	 * Resets the palette back to scratch.
	 **/
	void CPaletteSet::Reset() {
		m_vPalettes.resize( 1 );
		m_vPalettes[0].Reset();
		m_sActive = 0;
	}

	/**
	 * Loads a palette file (.PAL or .PPL).
	 * 
	 * \param _sPath The path to the file to load.
	 * \return Returns true if the file was loaded.
	 **/
	bool CPaletteSet::LoadPaletteFromFile( const std::u16string &_sPath ) {
		try {
			// Open/load the file.
			std::vector<uint8_t> vFile;
			{
				CStdFile sfFile;
				if ( !sfFile.Open( _sPath.c_str() ) ) { return false; }
				if ( !sfFile.LoadToMemory( vFile ) ) { return false; }
			}

			CStream sStream( vFile );

			// Select a palette.
			size_t sIdx = 0;
			if ( !m_vPalettes.size() ) {
				m_vPalettes.push_back( CPalette() );
				m_sActive = 0;
			}
			sIdx = m_sActive;

			if ( m_vPalettes[sIdx].Palette().size() ) {
				sIdx = m_vPalettes.size();
				m_vPalettes.push_back( CPalette() );
			}


			if ( CFileBase::CmpFileExtension( _sPath, u"pal" ) ) {
				return LoadPaletteFromFile( sStream, m_vPalettes[sIdx] );
			}
			else if ( CFileBase::CmpFileExtension( _sPath, u"ppl" ) ) {
				if ( !LoadPaletteFromFile( sStream, m_vPalettes[sIdx] ) ) { return false; }
				while ( sStream.Remaining() > 256 * sizeof( CPalette::SL2_PALETTE_ENTRY ) ) {
					sIdx = m_vPalettes.size();
					m_vPalettes.push_back( CPalette() );
					if ( !LoadPaletteFromFile( sStream, m_vPalettes[sIdx] ) ) { return false; }
				}
				return true;
			}
		}
		catch ( ... ) {}
		return false;
	}

	/**
	 * Loads a palette file (.PAL or .PPL).
	 * 
	 * \param _sFile The file to load.
	 * \param _pPal Palette into which to load the file.
	 * \return Returns true if the file was loaded.
	 **/
	bool CPaletteSet::LoadPaletteFromFile( const CStream &_sFile, CPalette &_pPal ) {
		return true;
	}

}	// namespace sl2

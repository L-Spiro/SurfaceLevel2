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
			return LoadPaletteFromStream( sStream );
			// Select a palette.
			/*size_t sIdx = 0;
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
				if ( LoadPaletteFromFile( sStream, m_vPalettes[sIdx] ) ) {
					m_sActive = sIdx;
					return true;
				}
				return false;
			}
			else if ( CFileBase::CmpFileExtension( _sPath, u"ppl" ) ) {
				if ( !LoadPaletteFromFile( sStream, m_vPalettes[sIdx] ) ) { return false; }
				while ( sStream.Remaining() > 256 * sizeof( CPalette::SL2_PALETTE_ENTRY ) ) {
					sIdx = m_vPalettes.size();
					m_vPalettes.push_back( CPalette() );
					if ( !LoadPaletteFromFile( sStream, m_vPalettes[sIdx] ) ) { return false; }
				}
				m_sActive = sIdx;
				return true;
			}*/
		}
		catch ( ... ) {}
		return false;
	}

	/**
	 * Loads a palette stream.
	 * 
	 * \param _sFile The stream from which to load the file.
	 * \return Returns true if the file was loaded.
	 **/
	bool CPaletteSet::LoadPaletteFromStream( const CStream &_sFile ) {
		try {
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
			if ( LoadPaletteFromFile( _sFile, m_vPalettes[sIdx] ) ) {
				m_sActive = sIdx;
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
		uint32_t ui32Type;
		if ( !_sFile.ReadUi32( ui32Type ) ) { return false; }
		switch ( ui32Type ) {
			case 0x46464952 : {							// RIFF
				if ( !LoadRiff( _sFile, _pPal ) ) {
					return false;
				}
				// In case of multiple files packed together.
				while ( _sFile.Remaining() >= sizeof( SL2_CHUNK ) ) {
					if ( !_sFile.ReadUi32( ui32Type ) ) { return false; }
					if ( ui32Type == 0x46464952 ) {		// RIFF
						size_t sIdx = m_vPalettes.size();
						m_vPalettes.push_back( CPalette() );
						if ( !LoadRiff( _sFile, m_vPalettes[sIdx] ) ) {
							return false;
						}
					}
					else {
						// Some other kind of data.  Unread it.
						_sFile.Rewind( sizeof( ui32Type ) );
					}
				}
				return true;
			}
			case 0x38394C50 : {							// PL98
				return LoadPl98( _sFile, _pPal );
			}
			default : { return false; }
		}
	}

	/**
	 * Loads a RIFF palette.
	 * 
	 * \param _sFile The file to load.
	 * \param _pPal Palette into which to load the file.
	 * \return Returns true if the file was loaded.
	 **/
	bool CPaletteSet::LoadRiff( const CStream &_sFile, CPalette &_pPal ) {
		// The RIFF header is assumed to be consumed.  The size follows.
		uint32_t ui32Size = 0;
		if ( !_sFile.ReadUi32( ui32Size ) ) { return false; }
		if ( ui32Size > _sFile.Remaining() ) { return false; }
		// Form signature.
		uint32_t ui32Form = 0;
		if ( !_sFile.ReadUi32( ui32Form ) ) { return false; }
		if ( ui32Form != 0x204C4150 ) { return false; }					// "PAL "

		SL2_CHUNK cChunk;
		// Find the data chunk.
		while ( _sFile.Remaining() ) {
			if ( !_sFile.Read( cChunk ) ) { return false; }
			if ( cChunk.ui32Name == 0x61746164 ) {						// data
				break;
			}
			if ( !_sFile.Read( nullptr, sizeof( SL2_CHUNK ) + cChunk.ui32Size ) ) { return false; }
		}

		// Version.
		uint16_t ui16Tmp;
		if ( !_sFile.ReadUi16( ui16Tmp ) ) { return false; }
		// Number of entries.
		if ( !_sFile.ReadUi16( ui16Tmp ) ) { return false; }

		bool bAnyNonZeroAlpha = false;
		for ( uint32_t I = 0; I < ui16Tmp; ++I ) {
			SL2_BITMAPPALETTE bpEntry;
			if ( !_sFile.Read( bpEntry ) ) { return false; }
			CPalette::CColor cColor;
			cColor[0] = bpEntry.s.ui8Red / 255.0;
			cColor[1] = bpEntry.s.ui8Green / 255.0;
			cColor[2] = bpEntry.s.ui8Blue / 255.0;
			cColor[3] = bpEntry.s.ui8Alpha / 255.0;
			_pPal.Add( cColor );
			if ( bpEntry.s.ui8Alpha ) { bAnyNonZeroAlpha = true; }
		}
		_pPal.SetFormat( CFormat::FindPaletteFormatData( bAnyNonZeroAlpha ? SL2_GL_PALETTE8_RGBA8_OES : SL2_GL_PALETTE8_RGB8_OES ) );
		return true;
	}

	/**
	 * Loads a PL98 palette.
	 * 
	 * \param _sFile The file to load.
	 * \param _pPal Palette into which to load the file.
	 * \return Returns true if the file was loaded.
	 **/
	bool CPaletteSet::LoadPl98( const CStream &_sFile, CPalette &_pPal ) {
		// The PL98 header is assumed to be consumed.  The number of palettes follows.
		struct SL2_PL98_HEADER {
#pragma pack( push, 1 )
			uint32_t			ui32Total;
			uint32_t			ui32ShadeShift;
			uint32_t			ui32HazeLevels;
			uint32_t			ui32HazeColor;
			uint8_t				ui8AlloedMatched[32];
#pragma pack( pop )
		};
		SL2_PL98_HEADER phHeader;
		if ( !_sFile.Read( phHeader ) ) { return false; }
		CPalette * ppTargetPalette = &_pPal;
		std::vector<uint32_t> vTypes;
		try {
			vTypes.reserve( phHeader.ui32Total );
			for ( uint32_t I = 0; I < phHeader.ui32Total; ++I ) {
				ppTargetPalette->SetFormat( CFormat::FindPaletteFormatData( SL2_GL_PALETTE8_RGBA8_OES ) );
				// Convert the RGBA colors.
				for ( size_t J = 0; J < 256; ++J ) {
					SL2_BITMAPPALETTE bpEntry;
					if ( !_sFile.Read( bpEntry ) ) { return false; }
					CPalette::CColor cColor;
					cColor[0] = bpEntry.s.ui8Red / 255.0;
					cColor[1] = bpEntry.s.ui8Green / 255.0;
					cColor[2] = bpEntry.s.ui8Blue / 255.0;
					cColor[3] = bpEntry.s.ui8Alpha / 255.0;
					ppTargetPalette->Add( cColor );
				}
				uint32_t ui32Id;
				if ( !_sFile.Read( ui32Id ) ) { return false; }
				ppTargetPalette->SetId( ui32Id );

				uint32_t ui32Type;
				if ( !_sFile.Read( ui32Type ) ) { return false; }
				vTypes.push_back( ui32Type );

				if ( I != phHeader.ui32Total - 1 ) {
					size_t sIdx = m_vPalettes.size();
					m_vPalettes.push_back( CPalette() );
					ppTargetPalette = &m_vPalettes[sIdx];
				}
			}

			for ( size_t I = 0; I < vTypes.size(); ++I ) {
				switch ( vTypes[I] ) {
					case 1 : {		// shadehaze
						/*for each data:
							if type is shadehaze:
								uint8[256 * shadeLevels * hazeLevels] shadeMap*/
						// We don't need this; it is for run-time.
						if ( !_sFile.Read( nullptr, size_t( 256 * sizeof( uint8_t ) * (1ULL << phHeader.ui32ShadeShift) * phHeader.ui32HazeLevels ) ) ) { return false; }
						break;
					}
					case 2 : {}		// translucent
					case 6 : {}		// additive
					case 8 : {		// subtractive
						/*if type is translucent or additive or subtractive:
							uint8[256 * 256] transMap*/
						if ( !_sFile.Read( nullptr, 256 * 256 * sizeof( uint8_t )  ) ) { return false; }
						break;
					}
				}
			}

			for ( size_t I = 0; I < vTypes.size(); ++I ) {
				switch ( vTypes[I] ) {
					case 1 : {}		// shadehaze
					case 2 : {}		// translucent
					case 6 : {}		// additive
					case 8 : {		// subtractive
						/*if type is translucent, shadeHaze, additive or subtractive:
							uint8[256] colorIndex
							float[256] colorRed
							float[256] colorGreen
							float[256] colorBlue*/
						if ( !_sFile.Read( nullptr, 256 * sizeof( uint8_t ) ) ) { return false; }
						if ( !_sFile.Read( nullptr, 256 * sizeof( float ) ) ) { return false; }
						if ( !_sFile.Read( nullptr, 256 * sizeof( float ) ) ) { return false; }
						if ( !_sFile.Read( nullptr, 256 * sizeof( float ) ) ) { return false; }
						break;
					}
				}
			}

			for ( size_t I = 0; I < vTypes.size(); ++I ) {
				switch ( vTypes[I] ) {
					case 0 : {		// noremap
						/*if type is noremap:
							uint8[256] colorIndex
							float[256] colorRed
							float[256] colorGreen
							float[256] colorBlue*/
						if ( !_sFile.Read( nullptr, 256 * sizeof( uint8_t ) ) ) { return false; }
						if ( !_sFile.Read( nullptr, 256 * sizeof( float ) ) ) { return false; }
						if ( !_sFile.Read( nullptr, 256 * sizeof( float ) ) ) { return false; }
						if ( !_sFile.Read( nullptr, 256 * sizeof( float ) ) ) { return false; }
						break;
					}
				}
			}
		}
		catch ( ... ) {}
		return true;
	}

}	// namespace sl2

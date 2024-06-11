/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A DDS reader and writer.
 */

#include "SL2Dds.h"
#include "../../Utilities/SL2Stream.h"


namespace sl2 {

	/** Format data. */
	CDds::SL2_FORMAT_DATA CDds::m_fdData[] = {
		{ SL2_D3DFMT_DXT1,				SL2_DXGI_FORMAT_BC1_UNORM,						64, 0, true, false, false },
		{ SL2_D3DFMT_UNKNOWN,			SL2_DXGI_FORMAT_BC1_UNORM_SRGB,					64, 0, true, false, false },
		{ SL2_D3DFMT_DXT2,				SL2_DXGI_FORMAT_BC2_UNORM,						128, 0, true, true, false },
		{ SL2_D3DFMT_DXT3,				SL2_DXGI_FORMAT_BC2_UNORM,						128, 0, true, false, false },
		{ SL2_D3DFMT_UNKNOWN,			SL2_DXGI_FORMAT_BC2_UNORM_SRGB,					128, 0, true, false, false },
		{ SL2_D3DFMT_DXT4,				SL2_DXGI_FORMAT_BC3_UNORM,						128, 0, true, true, false },
		{ SL2_D3DFMT_DXT5,				SL2_DXGI_FORMAT_BC3_UNORM,						128, 0, true, false, false },
		{ SL2_D3DFMT_UNKNOWN,			SL2_DXGI_FORMAT_BC3_UNORM_SRGB,					128, 0, true, false, false },

		{ SL2_D3DFMT_BC4U,				SL2_DXGI_FORMAT_BC4_UNORM,						64, 0, true, false, false },
		{ SL2_D3DFMT_BC4S,				SL2_DXGI_FORMAT_BC4_SNORM,						64, 0, true, false, false },

		{ SL2_D3DFMT_BC5U,				SL2_DXGI_FORMAT_BC5_UNORM,						128, 0, true, false, false },
		{ SL2_D3DFMT_BC5S,				SL2_DXGI_FORMAT_BC5_SNORM,						128, 0, true, false, false },

		{ SL2_D3DFMT_UNKNOWN,			SL2_DXGI_FORMAT_BC6H_UF16,						128, 0, true, false, false },
		{ SL2_D3DFMT_UNKNOWN,			SL2_DXGI_FORMAT_BC6H_SF16,						128, 0, true, false, false },

		{ SL2_D3DFMT_UNKNOWN,			SL2_DXGI_FORMAT_BC7_UNORM,						128, 0, true, false, false },
		{ SL2_D3DFMT_UNKNOWN,			SL2_DXGI_FORMAT_BC7_UNORM_SRGB,					128, 0, true, false, false },


		{ SL2_D3DFMT_R8G8_B8G8,			SL2_DXGI_FORMAT_R8G8_B8G8_UNORM,				32, 0, false, false, true },
		{ SL2_D3DFMT_G8R8_G8B8,			SL2_DXGI_FORMAT_G8R8_G8B8_UNORM,				32, 0, false, false, true },

		{ SL2_D3DFMT_A16B16G16R16,		SL2_DXGI_FORMAT_R16G16B16A16_UNORM,				64, 0, false, false, false },
		{ SL2_D3DFMT_Q16W16V16U16,		SL2_DXGI_FORMAT_R16G16B16A16_SNORM,				64, 0, false, false, false },

		{ SL2_D3DFMT_R16F,				SL2_DXGI_FORMAT_R16_FLOAT,						16, 0, false, false, false },
		{ SL2_D3DFMT_G16R16F,			SL2_DXGI_FORMAT_R16G16_FLOAT,					32, 0, false, false, false },
		{ SL2_D3DFMT_A16B16G16R16F,		SL2_DXGI_FORMAT_R16G16B16A16_FLOAT,				64, 0, false, false, false },
		{ SL2_D3DFMT_R32F,				SL2_DXGI_FORMAT_R32_FLOAT,						32, 0, false, false, false },
		{ SL2_D3DFMT_G32R32F,			SL2_DXGI_FORMAT_R32G32_FLOAT,					64, 0, false, false, false },
		{ SL2_D3DFMT_A32B32G32R32F,		SL2_DXGI_FORMAT_R32G32B32A32_FLOAT,				128, 0, false, false, false },

		{ SL2_D3DFMT_UNKNOWN,			SL2_DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,			32, 0, false, false, false },
		{ SL2_D3DFMT_G16R16,			SL2_DXGI_FORMAT_R16G16_UNORM,					32, 0, false, false, false },
		{ SL2_D3DFMT_A1R5G5B5,			SL2_DXGI_FORMAT_B5G5R5A1_UNORM,					16, 0, false, false, false },
		{ SL2_D3DFMT_R5G6B5,			SL2_DXGI_FORMAT_B5G6R5_UNORM,					16, 0, false, false, false },
		{ SL2_D3DFMT_A8,				SL2_DXGI_FORMAT_A8_UNORM,						8, 0, false, false, false },
		{ SL2_D3DFMT_R8G8B8,			SL2_DXGI_FORMAT_UNKNOWN,						24, 0, false, false, false },
		{ SL2_D3DFMT_X1R5G5B5,			SL2_DXGI_FORMAT_UNKNOWN,						16, 0, false, false, false },
		{ SL2_D3DFMT_A4R4G4B4,			SL2_DXGI_FORMAT_B4G4R4A4_UNORM,					16, 0, false, false, false },
		{ SL2_D3DFMT_X4R4G4B4,			SL2_DXGI_FORMAT_UNKNOWN,						16, 0, false, false, false },
		{ SL2_D3DFMT_A8R3G3B2,			SL2_DXGI_FORMAT_UNKNOWN,						16, 0, false, false, false },
		{ SL2_D3DFMT_A8L8,				SL2_DXGI_FORMAT_UNKNOWN,						16, 0, false, false, false },
		{ SL2_D3DFMT_L16,				SL2_DXGI_FORMAT_UNKNOWN,						16, 0, false, false, false },
		{ SL2_D3DFMT_UNKNOWN,			SL2_DXGI_FORMAT_R32G32B32A32_UINT,				128, 0, false, false, false },
		{ SL2_D3DFMT_UNKNOWN,			SL2_DXGI_FORMAT_R32G32B32A32_SINT,				128, 0, false, false, false },
		{ SL2_D3DFMT_UNKNOWN,			SL2_DXGI_FORMAT_R32G32B32_FLOAT,				96, 0, false, false, false },
		{ SL2_D3DFMT_UNKNOWN,			SL2_DXGI_FORMAT_R32G32B32_UINT,					96, 0, false, false, false },
		{ SL2_D3DFMT_UNKNOWN,			SL2_DXGI_FORMAT_R32G32B32_SINT,					96, 0, false, false, false },
		{ SL2_D3DFMT_UNKNOWN,			SL2_DXGI_FORMAT_R16G16B16A16_UINT,				64, 0, false, false, false },
		{ SL2_D3DFMT_UNKNOWN,			SL2_DXGI_FORMAT_R16G16B16A16_SINT,				64, 0, false, false, false },
		{ SL2_D3DFMT_UNKNOWN,			SL2_DXGI_FORMAT_R32G32_UINT,					64, 0, false, false, false },
		{ SL2_D3DFMT_UNKNOWN,			SL2_DXGI_FORMAT_R32G32_SINT,					64, 0, false, false, false },
		
		{ SL2_D3DFMT_UNKNOWN,			SL2_DXGI_FORMAT_R10G10B10A2_UINT,				32, 0, false, false, false },
		{ SL2_D3DFMT_UNKNOWN,			SL2_DXGI_FORMAT_R11G11B10_FLOAT,				32, 0, false, false, false },
		{ SL2_D3DFMT_UNKNOWN,			SL2_DXGI_FORMAT_R8G8B8A8_UINT,					32, 0, false, false, false },
		{ SL2_D3DFMT_UNKNOWN,			SL2_DXGI_FORMAT_R8G8B8A8_SNORM,					32, 0, false, false, false },
		{ SL2_D3DFMT_UNKNOWN,			SL2_DXGI_FORMAT_R8G8B8A8_SINT,					32, 0, false, false, false },
		{ SL2_D3DFMT_UNKNOWN,			SL2_DXGI_FORMAT_R16G16_UINT,					32, 0, false, false, false },
		{ SL2_D3DFMT_UNKNOWN,			SL2_DXGI_FORMAT_R16G16_SNORM,					32, 0, false, false, false },
		{ SL2_D3DFMT_UNKNOWN,			SL2_DXGI_FORMAT_R16G16_SINT,					32, 0, false, false, false },
		{ SL2_D3DFMT_UNKNOWN,			SL2_DXGI_FORMAT_D32_FLOAT,						32, 0, false, false, false },
		{ SL2_D3DFMT_UNKNOWN,			SL2_DXGI_FORMAT_R32_UINT,						32, 0, false, false, false },
		{ SL2_D3DFMT_UNKNOWN,			SL2_DXGI_FORMAT_R32_SINT,						32, 0, false, false, false },
		{ SL2_D3DFMT_UNKNOWN,			SL2_DXGI_FORMAT_R8G8_UNORM,						16, 0, false, false, false },
		{ SL2_D3DFMT_UNKNOWN,			SL2_DXGI_FORMAT_R8G8_UINT,						16, 0, false, false, false },
		{ SL2_D3DFMT_UNKNOWN,			SL2_DXGI_FORMAT_R8G8_SNORM,						16, 0, false, false, false },
		{ SL2_D3DFMT_UNKNOWN,			SL2_DXGI_FORMAT_R8G8_SINT,						16, 0, false, false, false },
		{ SL2_D3DFMT_UNKNOWN,			SL2_DXGI_FORMAT_D16_UNORM,						16, 0, false, false, false },
		{ SL2_D3DFMT_UNKNOWN,			SL2_DXGI_FORMAT_R16_UNORM,						16, 0, false, false, false },
		{ SL2_D3DFMT_UNKNOWN,			SL2_DXGI_FORMAT_R16_UINT,						16, 0, false, false, false },
		{ SL2_D3DFMT_UNKNOWN,			SL2_DXGI_FORMAT_R16_SNORM,						16, 0, false, false, false },
		{ SL2_D3DFMT_UNKNOWN,			SL2_DXGI_FORMAT_R16_SINT,						16, 0, false, false, false },
		{ SL2_D3DFMT_UNKNOWN,			SL2_DXGI_FORMAT_R8_UNORM,						8, 0, false, false, false },
		{ SL2_D3DFMT_UNKNOWN,			SL2_DXGI_FORMAT_R8_UINT,						8, 0, false, false, false },
		{ SL2_D3DFMT_UNKNOWN,			SL2_DXGI_FORMAT_R8_SNORM,						8, 0, false, false, false },
		{ SL2_D3DFMT_UNKNOWN,			SL2_DXGI_FORMAT_R8_SINT,						8, 0, false, false, false },
		{ SL2_D3DFMT_UNKNOWN,			SL2_DXGI_FORMAT_R1_UNORM,						1, 0, false, false, false },
		{ SL2_D3DFMT_UNKNOWN,			SL2_DXGI_FORMAT_R9G9B9E5_SHAREDEXP,				32, 0, false, false, false },
		{ SL2_D3DFMT_UNKNOWN,			SL2_DXGI_FORMAT_B5G5R5A1_UNORM,					16, 0, false, false, false },
		{ SL2_D3DFMT_UNKNOWN,			SL2_DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM,		32, 0, false, false, false },
		{ SL2_D3DFMT_UNKNOWN,			SL2_DXGI_FORMAT_B8G8R8A8_UNORM_SRGB,			32, 0, false, false, false },
		{ SL2_D3DFMT_UNKNOWN,			SL2_DXGI_FORMAT_B8G8R8X8_UNORM_SRGB,			32, 0, false, false, false },

		{ SL2_D3DFMT_UNKNOWN,			SL2_DXGI_FORMAT_X32_TYPELESS_G8X24_UINT,		64, 0, false, false, false },
		{ SL2_D3DFMT_UNKNOWN,			SL2_DXGI_FORMAT_D32_FLOAT_S8X24_UINT,			64, 0, false, false, false },
		{ SL2_D3DFMT_D24S8,				SL2_DXGI_FORMAT_D24_UNORM_S8_UINT,				32, 0, false, false, false },
		{ SL2_D3DFMT_UNKNOWN,			SL2_DXGI_FORMAT_X24_TYPELESS_G8_UINT,			32, 0, false, false, false },

		// Order matters here.
		{ SL2_D3DFMT_UNKNOWN,			SL2_DXGI_FORMAT_R8G8B8A8_UNORM,					32, 0, false, false, false },
		{ SL2_D3DFMT_UNKNOWN,			SL2_DXGI_FORMAT_B8G8R8A8_UNORM,					32, 0, false, false, false },
		{ SL2_D3DFMT_UNKNOWN,			SL2_DXGI_FORMAT_B8G8R8X8_UNORM,					32, 0, false, false, false },

		{ SL2_D3DFMT_R8G8B8,			SL2_DXGI_FORMAT_R8G8B8A8_UNORM,					24, 32, false, false, false, Convert_RGB24_to_RGBA32 },
		{ SL2_D3DFMT_A8R8G8B8,			SL2_DXGI_FORMAT_R8G8B8A8_UNORM,					32, 0, false, false, false, Convert_RGBA32_to_RGBA32<SL2_TEXEL_RGBA8, true> },
		{ SL2_D3DFMT_X8R8G8B8,			SL2_DXGI_FORMAT_R8G8B8A8_UNORM,					32, 0, false, false, false, Convert_RGBA32_to_RGBA32<SL2_TEXEL_RGBA8, false> },
		{ SL2_D3DFMT_A8B8G8R8,			SL2_DXGI_FORMAT_B8G8R8A8_UNORM,					32, 0, false, false, false, Convert_RGBA32_to_RGBA32<SL2_TEXEL_BGRA8, true> },
		{ SL2_D3DFMT_X8B8G8R8,			SL2_DXGI_FORMAT_B8G8R8X8_UNORM,					32, 0, false, false, false, Convert_RGBA32_to_RGBA32<SL2_TEXEL_BGRA8, false> },


		{ SL2_D3DFMT_UNKNOWN,			SL2_DXGI_FORMAT_R10G10B10A2_UNORM,				32, 0, false, false, false },

		{ SL2_D3DFMT_A2B10G10R10,		SL2_DXGI_FORMAT_R10G10B10A2_UNORM,				32, 0, false, false, false, Convert_RGBA32_to_RGBA32<SL2_TEXEL_RGB10A2, true> },
		{ SL2_D3DFMT_A2R10G10B10,		SL2_DXGI_FORMAT_R10G10B10A2_UNORM,				32, 0, false, false, false, Convert_RGBA32_to_RGBA32<SL2_TEXEL_RGB10A2, true> },


		{ SL2_D3DFMT_UNKNOWN,			SL2_DXGI_FORMAT_YUY2,							32, 0, false, false, true },
		{ SL2_D3DFMT_YUY2,				SL2_DXGI_FORMAT_YUY2,							32, 0, false, false, true },
		{ SL2_D3DFMT_UNKNOWN,			SL2_DXGI_FORMAT_V208,							24, 0, false, false, true },

		{ SL2_D3DFMT_UYVY,				SL2_DXGI_FORMAT_YUY2,							32, 0, false, false, true, Convert_UYVY_to_YUY2 },
		{ SL2_D3DFMT_CxV8U8,			SL2_DXGI_FORMAT_V208,							16, 24, false, false, true },		// Add conversion.

		
	};


	CDds::CDds() {
	}
	CDds::~CDds() {
	}


	// == Functions.
	/**
	 * Loads a DDS file from memory.
	 *
	 * \param _vFileData The in-memory image of the file.
	 * \return Returns true if the file was successfully loaded.  False indicates an invalid file or lack of RAM.
	 */
	bool CDds::LoadDds( const std::vector<uint8_t> &_vFileData ) {
		CStream sStream( _vFileData );
		uint32_t ui32Magic;
		if ( !sStream.Read( ui32Magic ) ) { return false; }
		if ( ui32Magic != 0x20534444 ) { return false; }

		if ( !sStream.Read( m_dhHeader ) ) { return false; }
		
		if ( m_dhHeader.dpPixelFormat.ui32Size != sizeof( m_dhHeader.dpPixelFormat ) ) { return false; }

		if ( (m_dhHeader.ui32Flags & (LSI_DF_CAPS | LSI_DF_HEIGHT | LSI_DF_WIDTH | LSI_DF_PIXELFORMAT)) !=
			(LSI_DF_CAPS | LSI_DF_HEIGHT | LSI_DF_WIDTH | LSI_DF_PIXELFORMAT) ) {
			if ( (m_dhHeader.ui32Flags & (LSI_DF_HEIGHT | LSI_DF_WIDTH)) !=
				(LSI_DF_HEIGHT | LSI_DF_WIDTH) ) {
				return false;
			}
		}


		m_pfdFormat = FormatByD3DFORMAT( static_cast<SL2_D3DFORMAT>(m_dhHeader.dpPixelFormat.ui32FourCC) );
		m_ui32ArraySize = 1;

		bool bUseDx10 = false;
		if ( m_dhHeader.dpPixelFormat.ui32Flags & LSI_DPFF_FOURCC ) {
			switch ( m_dhHeader.dpPixelFormat.ui32FourCC ) {
				case SL2_MAKEFOURCC( 'D', 'X', '1', '0' ) : {
					bUseDx10 = true;
					if ( !sStream.Read( m_dh10Header10 ) ) { return false; }
					m_pfdFormat = FormatByDXGI_FORMAT( static_cast<SL2_DXGI_FORMAT>(m_dh10Header10.ui32DxgiFormat) );
					m_ui32ArraySize = m_dh10Header10.ui32ArraySize;
					break;
				}
			}
		}

		uint32_t ui32Array = m_ui32ArraySize;
		if ( m_dhHeader.ui32Caps2 & SL2_DDSCAPS2_CUBEMAP ) {
			ui32Array = m_ui32Faces = 6;
		}

		if ( nullptr == m_pfdFormat ) { return false; }

		uint32_t ui32Pitch = m_dhHeader.ui32PitchOrLinearSize;
		uint32_t ui32PitchAfter = m_dhHeader.ui32PitchOrLinearSize;
		size_t sSrcSize = 0;
		if ( m_pfdFormat->bIsCompressed ) {
			ui32Pitch = (m_dhHeader.ui32Width + 3) / 4 * m_pfdFormat->ui8BitsPerBlock / 8;
			ui32PitchAfter = (m_dhHeader.ui32Width + 3) / 4 * m_pfdFormat->ui8BitsAfterConvert / 8;
			
		}
		else if ( m_pfdFormat->bPacked ) {
			ui32Pitch = ((m_dhHeader.ui32Width + 1) >> 1) * 4;
			ui32PitchAfter = ((m_dhHeader.ui32Width + 1) >> 1) * 4;
		}
		else {
			ui32Pitch = (m_dhHeader.ui32Width * m_pfdFormat->ui8BitsPerBlock + 7) / 8;
			ui32PitchAfter = (m_dhHeader.ui32Width * m_pfdFormat->ui8BitsAfterConvert + 7) / 8;
		}
		if ( !m_pfdFormat->ui8BitsAfterConvert ) {
			ui32PitchAfter = ui32Pitch;
		}

		size_t iIndex = 0;
		uint8_t * pui8Src = sStream.Data();
		for ( uint32_t J = 0; J < ui32Array; J++ ) {
			uint32_t ui32W = std::max( m_dhHeader.ui32Width, static_cast<uint32_t>(1) );
			uint32_t ui32H = std::max( m_dhHeader.ui32Height, static_cast<uint32_t>(1) );
			uint32_t ui32D = std::max( m_dhHeader.ui32Depth, static_cast<uint32_t>(1) );
			for ( uint32_t I = 0; I < m_dhHeader.ui32MipMapCount; I++ ) {
				size_t sSrcSize = ui32D * ui32H * ui32Pitch;
				if ( m_pfdFormat->bIsCompressed ) {
					sSrcSize = GetCompressedSizeBc( ui32W,
						ui32H,
						ui32D, m_pfdFormat->ui8BitsPerBlock );
				}

				SL2_TEX tTexture;
				tTexture.ui32Pitch = ui32PitchAfter;
				try {
					tTexture.vTexture.resize( sSrcSize );
				}
				catch( ... ) {}
				if ( m_pfdFormat->pfConverter ) {
					m_pfdFormat->pfConverter( pui8Src, tTexture.vTexture.data(), ui32W, ui32H, ui32D, ui32Pitch, m_dhHeader.dpPixelFormat );
					sStream.Read( nullptr, sSrcSize );
				}
				else {
					sStream.Read( tTexture.vTexture.data(), tTexture.vTexture.size() );
				}

				m_vTextures.push_back( std::move( tTexture ) );

				ui32W = std::max( ui32W >> 1, static_cast<uint32_t>(1) );
				ui32H = std::max( ui32H >> 1, static_cast<uint32_t>(1) );
				ui32D = std::max( ui32D >> 1, static_cast<uint32_t>(1) );
			}
		}
		m_dhHeader.ui32PitchOrLinearSize = ui32Pitch;
		return true;
	}

	/**
	 * Converts D3DFMT_UYVY to DXGI_FORMAT_YUY2.
	 * 
	 * \param _pui8Src The data to convert.
	 * \param _pui8Dst The data to convert.
	 * \param _ui32Width The width of the given data.
	 * \param _ui32Height The height of the given data.
	 * \param _ui32Depth The depth of the given data.
	 * \param _ui32Pitch The row width in bytes of the given data.
	 * \param _dpfPixelFormat The pixel format data.
	 **/
	void CDds::Convert_UYVY_to_YUY2( uint8_t * _pui8Src, uint8_t * _pui8Dst, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Depth, uint32_t _ui32Pitch, const LSI_DDS_PIXELFORMAT &/*_dpfPixelFormat*/ ) {
		struct SL2_TEXELS {
			uint8_t				ui8U;
			uint8_t				ui8Y0;
			uint8_t				ui8V;
			uint8_t				ui8Y1;
		};

		size_t sSliceSize = _ui32Pitch * _ui32Height;		
		for ( uint32_t D = 0; D < _ui32Depth; ++D ) {
			for ( uint32_t H = 0; H < _ui32Height; ++H ) {
				for ( uint32_t W = 0; W < _ui32Width; ++W ) {
					SL2_TEXELS * ptTexelsSrc = reinterpret_cast<SL2_TEXELS *>(_pui8Src + _ui32Pitch * H + sSliceSize * D) + W;
					SL2_TEXELS * ptTexelsDst = reinterpret_cast<SL2_TEXELS *>(_pui8Dst + _ui32Pitch * H + sSliceSize * D) + W;
					ptTexelsDst->ui8U = ptTexelsSrc->ui8Y0;
					ptTexelsDst->ui8Y0 = ptTexelsSrc->ui8U;
					ptTexelsDst->ui8V = ptTexelsSrc->ui8Y1;
					ptTexelsDst->ui8Y1 = ptTexelsSrc->ui8V;
					/*std::swap( ptTexels->ui8U, ptTexels->ui8Y0 );
					std::swap( ptTexels->ui8V, ptTexels->ui8Y1 );*/
				}
			}
		}
	}

	/**
	 * Converts a maskd 24-bit RGB to a 32-bit SL2_DXGI_FORMAT_R8G8B8A8_UNORM or SL2_DXGI_FORMAT_R8G8B8A8_UNORM_SRGB.
	 * 
	 * \param _pui8Src The data to convert.
	 * \param _pui8Dst The data to convert.
	 * \param _ui32Width The width of the given data.
	 * \param _ui32Height The height of the given data.
	 * \param _ui32Depth The depth of the given data.
	 * \param _ui32Pitch The row width in bytes of the given data.
	 * \param _dpfPixelFormat The pixel format data.
	 **/
	void CDds::Convert_RGB24_to_RGBA32( uint8_t * _pui8Src, uint8_t * _pui8Dst, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Depth, uint32_t _ui32Pitch, const LSI_DDS_PIXELFORMAT &_dpfPixelFormat ) {
		struct SL2_TEXELS {
			uint8_t				ui8R;
			uint8_t				ui8G;
			uint8_t				ui8B;
			uint8_t				ui8A;
		};
		double dR, dG, dB;
		size_t sR = CUtilities::BitMaskToShift( _dpfPixelFormat.ui32RBitMask, dR );
		size_t sG = CUtilities::BitMaskToShift( _dpfPixelFormat.ui32GBitMask, dG );
		size_t sB = CUtilities::BitMaskToShift( _dpfPixelFormat.ui32BBitMask, dB );
		size_t sSliceSize = _ui32Pitch * _ui32Height;		
		for ( uint32_t D = 0; D < _ui32Depth; ++D ) {
			for ( uint32_t H = 0; H < _ui32Height; ++H ) {
				for ( uint32_t W = 0; W < _ui32Width; ++W ) {
					uint32_t * pui32TexelsSrc = reinterpret_cast<uint32_t *>(_pui8Src + _ui32Pitch * H + sSliceSize * D + (W * 3));
					SL2_TEXELS * ptTexelsDst = reinterpret_cast<SL2_TEXELS *>(_pui8Dst + _ui32Pitch * H + sSliceSize * D) + W;
					ptTexelsDst->ui8R = (*pui32TexelsSrc) >> sR;
					ptTexelsDst->ui8G = (*pui32TexelsSrc) >> sG;
					ptTexelsDst->ui8B = (*pui32TexelsSrc) >> sB;
					ptTexelsDst->ui8A = 0xFF;
				}
			}
		}
	}

	/**
	 * Gets format data given a SL2_D3DFORMAT format.
	 * 
	 * \param _dfFormat The format to find.
	 * \return Returns a matching SL2_FORMAT_DATA pointer from m_fdData or nullptr.
	 **/
	CDds::SL2_FORMAT_DATA * CDds::FormatByD3DFORMAT( SL2_D3DFORMAT _dfFormat ) {
		if ( _dfFormat != SL2_D3DFMT_UNKNOWN ) {
			for ( size_t I = 0; I < SL2_ELEMENTS( m_fdData ); ++I ) {
				if ( m_fdData[I].fD3dFormat == _dfFormat ) { return &m_fdData[I]; }
			}
		}
		return nullptr;
	}

	/**
	 * Gets format data given a SL2_DXGI_FORMAT format.
	 * 
	 * \param _dfFormat The format to find.
	 * \return Returns a matching SL2_FORMAT_DATA pointer from m_fdData or nullptr.
	 **/
	CDds::SL2_FORMAT_DATA * CDds::FormatByDXGI_FORMAT( SL2_DXGI_FORMAT _dfFormat ) {
		if ( _dfFormat != SL2_DXGI_FORMAT_UNKNOWN ) {
			for ( size_t I = 0; I < SL2_ELEMENTS( m_fdData ); ++I ) {
				if ( m_fdData[I].dfFormat == _dfFormat ) { return &m_fdData[I]; }
			}
		}
		return nullptr;
	}

}	// namespace sl2

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
		{ SL2_D3DFMT_DXT1,				SL2_DXGI_FORMAT_BC1_UNORM,						64, true, false },
		{ SL2_D3DFMT_UNKNOWN,			SL2_DXGI_FORMAT_BC1_UNORM_SRGB,					64, true, false },
		{ SL2_D3DFMT_DXT2,				SL2_DXGI_FORMAT_BC2_UNORM,						128, true, true },
		{ SL2_D3DFMT_DXT3,				SL2_DXGI_FORMAT_BC2_UNORM,						128, true, false },
		{ SL2_D3DFMT_UNKNOWN,			SL2_DXGI_FORMAT_BC2_UNORM_SRGB,					128, true, false },
		{ SL2_D3DFMT_DXT4,				SL2_DXGI_FORMAT_BC3_UNORM,						128, true, true },
		{ SL2_D3DFMT_DXT5,				SL2_DXGI_FORMAT_BC3_UNORM,						128, true, false },
		{ SL2_D3DFMT_UNKNOWN,			SL2_DXGI_FORMAT_BC3_UNORM_SRGB,					128, true, false },

		{ SL2_D3DFMT_BC4U,				SL2_DXGI_FORMAT_BC4_UNORM,						64, true, false },
		{ SL2_D3DFMT_BC4S,				SL2_DXGI_FORMAT_BC4_SNORM,						64, true, false },

		{ SL2_D3DFMT_BC5U,				SL2_DXGI_FORMAT_BC5_UNORM,						128, true, false },
		{ SL2_D3DFMT_BC5S,				SL2_DXGI_FORMAT_BC5_SNORM,						128, true, false },

		{ SL2_D3DFMT_UNKNOWN,			SL2_DXGI_FORMAT_BC6H_UF16,						128, true, false },
		{ SL2_D3DFMT_UNKNOWN,			SL2_DXGI_FORMAT_BC6H_SF16,						128, true, false },

		{ SL2_D3DFMT_UNKNOWN,			SL2_DXGI_FORMAT_BC7_UNORM,						128, true, false },
		{ SL2_D3DFMT_UNKNOWN,			SL2_DXGI_FORMAT_BC7_UNORM_SRGB,					128, true, false },


		{ SL2_D3DFMT_R8G8_B8G8,			SL2_DXGI_FORMAT_R8G8_B8G8_UNORM,				32, false, false },
		{ SL2_D3DFMT_G8R8_G8B8,			SL2_DXGI_FORMAT_G8R8_G8B8_UNORM,				32, false, false },

		{ SL2_D3DFMT_A16B16G16R16,		SL2_DXGI_FORMAT_R16G16B16A16_UNORM,				64, false, false },
		{ SL2_D3DFMT_Q16W16V16U16,		SL2_DXGI_FORMAT_R16G16B16A16_SNORM,				64, false, false },

		{ SL2_D3DFMT_R16F,				SL2_DXGI_FORMAT_R16_FLOAT,						16, false, false },
		{ SL2_D3DFMT_G16R16F,			SL2_DXGI_FORMAT_R16G16_FLOAT,					32, false, false },
		{ SL2_D3DFMT_A16B16G16R16F,		SL2_DXGI_FORMAT_R16G16B16A16_FLOAT,				64, false, false },
		{ SL2_D3DFMT_R32F,				SL2_DXGI_FORMAT_R32_FLOAT,						32, false, false },
		{ SL2_D3DFMT_G32R32F,			SL2_DXGI_FORMAT_R32G32_FLOAT,					64, false, false },
		{ SL2_D3DFMT_A32B32G32R32F,		SL2_DXGI_FORMAT_R32G32B32A32_FLOAT,				128, false, false },

		{ SL2_D3DFMT_UYVY,				SL2_DXGI_FORMAT_YUY2,							32, false, false,	Convert_UYVY_to_YUY2 },
		
		{ SL2_D3DFMT_YUY2,				SL2_DXGI_FORMAT_YUY2,							32, false, false },
		{ SL2_D3DFMT_CxV8U8,			SL2_DXGI_FORMAT_V208,							16, false, false },		// Add conversion.

		{ SL2_D3DFMT_A8B8G8R8,			SL2_DXGI_FORMAT_R8G8B8A8_UNORM,					32, false, false },
		{ SL2_D3DFMT_UNKNOWN,			SL2_DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,			32, false, false },
		{ SL2_D3DFMT_G16R16,			SL2_DXGI_FORMAT_R16G16_UNORM,					32, false, false },
		{ SL2_D3DFMT_A2B10G10R10,		SL2_DXGI_FORMAT_R10G10B10A2_UNORM,				32, false, false },
		{ SL2_D3DFMT_A1R5G5B5,			SL2_DXGI_FORMAT_B5G5R5A1_UNORM,					16, false, false },
		{ SL2_D3DFMT_R5G6B5,			SL2_DXGI_FORMAT_B5G6R5_UNORM,					16, false, false },
		{ SL2_D3DFMT_A8,				SL2_DXGI_FORMAT_A8_UNORM,						8, false, false },
		{ SL2_D3DFMT_A8R8G8B8,			SL2_DXGI_FORMAT_B8G8R8A8_UNORM,					32, false, false },
		{ SL2_D3DFMT_X8R8G8B8,			SL2_DXGI_FORMAT_B8G8R8X8_UNORM,					32, false, false },
		{ SL2_D3DFMT_X8B8G8R8,			SL2_DXGI_FORMAT_UNKNOWN,						32, false, false },
		{ SL2_D3DFMT_A2R10G10B10,		SL2_DXGI_FORMAT_R10G10B10A2_UNORM,				32, false, false },		// Add conversion.
		{ SL2_D3DFMT_R8G8B8,			SL2_DXGI_FORMAT_UNKNOWN,						24, false, false },
		{ SL2_D3DFMT_X1R5G5B5,			SL2_DXGI_FORMAT_UNKNOWN,						16, false, false },
		{ SL2_D3DFMT_A4R4G4B4,			SL2_DXGI_FORMAT_B4G4R4A4_UNORM,					16, false, false },

		{ SL2_D3DFMT_X4R4G4B4,			SL2_DXGI_FORMAT_UNKNOWN,						16, false, false },
		{ SL2_D3DFMT_A8R3G3B2,			SL2_DXGI_FORMAT_UNKNOWN,						16, false, false },
		{ SL2_D3DFMT_A8L8,				SL2_DXGI_FORMAT_UNKNOWN,						16, false, false },
		{ SL2_D3DFMT_L16,				SL2_DXGI_FORMAT_UNKNOWN,						16, false, false },

		{ SL2_D3DFMT_UNKNOWN,			SL2_DXGI_FORMAT_R32G32B32A32_UINT,				128, false, false },
		{ SL2_D3DFMT_UNKNOWN,			SL2_DXGI_FORMAT_R32G32B32A32_SINT,				128, false, false },
		{ SL2_D3DFMT_UNKNOWN,			SL2_DXGI_FORMAT_R32G32B32_FLOAT,				96, false, false },
		{ SL2_D3DFMT_UNKNOWN,			SL2_DXGI_FORMAT_R32G32B32_UINT,					96, false, false },
		{ SL2_D3DFMT_UNKNOWN,			SL2_DXGI_FORMAT_R32G32B32_SINT,					96, false, false },
		{ SL2_D3DFMT_UNKNOWN,			SL2_DXGI_FORMAT_R16G16B16A16_UINT,				128, false, false },
		{ SL2_D3DFMT_UNKNOWN,			SL2_DXGI_FORMAT_R16G16B16A16_SINT,				128, false, false },
		{ SL2_D3DFMT_UNKNOWN,			SL2_DXGI_FORMAT_R32G32_UINT,					64, false, false },
		{ SL2_D3DFMT_UNKNOWN,			SL2_DXGI_FORMAT_R32G32_SINT,					64, false, false },
		{ SL2_D3DFMT_UNKNOWN,			SL2_DXGI_FORMAT_R10G10B10A2_UNORM,				32, false, false },
		{ SL2_D3DFMT_UNKNOWN,			SL2_DXGI_FORMAT_R10G10B10A2_UINT,				32, false, false },
		{ SL2_D3DFMT_UNKNOWN,			SL2_DXGI_FORMAT_R11G11B10_FLOAT,				32, false, false },
		{ SL2_D3DFMT_UNKNOWN,			SL2_DXGI_FORMAT_R8G8B8A8_UINT,					32, false, false },
		{ SL2_D3DFMT_UNKNOWN,			SL2_DXGI_FORMAT_R8G8B8A8_SNORM,					32, false, false },
		{ SL2_D3DFMT_UNKNOWN,			SL2_DXGI_FORMAT_R8G8B8A8_SINT,					32, false, false },
		{ SL2_D3DFMT_UNKNOWN,			SL2_DXGI_FORMAT_R16G16_UINT,					32, false, false },
		{ SL2_D3DFMT_UNKNOWN,			SL2_DXGI_FORMAT_R16G16_SNORM,					32, false, false },
		{ SL2_D3DFMT_UNKNOWN,			SL2_DXGI_FORMAT_R16G16_SINT,					32, false, false },
		{ SL2_D3DFMT_UNKNOWN,			SL2_DXGI_FORMAT_D32_FLOAT,						32, false, false },
		{ SL2_D3DFMT_UNKNOWN,			SL2_DXGI_FORMAT_R32_UINT,						32, false, false },
		{ SL2_D3DFMT_UNKNOWN,			SL2_DXGI_FORMAT_R32_SINT,						32, false, false },
		{ SL2_D3DFMT_UNKNOWN,			SL2_DXGI_FORMAT_R8G8_UNORM,						16, false, false },
		{ SL2_D3DFMT_UNKNOWN,			SL2_DXGI_FORMAT_R8G8_UINT,						16, false, false },
		{ SL2_D3DFMT_UNKNOWN,			SL2_DXGI_FORMAT_R8G8_SNORM,						16, false, false },
		{ SL2_D3DFMT_UNKNOWN,			SL2_DXGI_FORMAT_R8G8_SINT,						16, false, false },
		{ SL2_D3DFMT_UNKNOWN,			SL2_DXGI_FORMAT_D16_UNORM,						16, false, false },
		{ SL2_D3DFMT_UNKNOWN,			SL2_DXGI_FORMAT_R16_UNORM,						16, false, false },
		{ SL2_D3DFMT_UNKNOWN,			SL2_DXGI_FORMAT_R16_UINT,						16, false, false },
		{ SL2_D3DFMT_UNKNOWN,			SL2_DXGI_FORMAT_R16_SNORM,						16, false, false },
		{ SL2_D3DFMT_UNKNOWN,			SL2_DXGI_FORMAT_R16_SINT,						16, false, false },
		{ SL2_D3DFMT_UNKNOWN,			SL2_DXGI_FORMAT_R8_UNORM,						8, false, false },
		{ SL2_D3DFMT_UNKNOWN,			SL2_DXGI_FORMAT_R8_UINT,						8, false, false },
		{ SL2_D3DFMT_UNKNOWN,			SL2_DXGI_FORMAT_R8_SNORM,						8, false, false },
		{ SL2_D3DFMT_UNKNOWN,			SL2_DXGI_FORMAT_R8_SINT,						8, false, false },
		{ SL2_D3DFMT_UNKNOWN,			SL2_DXGI_FORMAT_R1_UNORM,						1, false, false },
		{ SL2_D3DFMT_UNKNOWN,			SL2_DXGI_FORMAT_R9G9B9E5_SHAREDEXP,				32, false, false },
		{ SL2_D3DFMT_UNKNOWN,			SL2_DXGI_FORMAT_B5G5R5A1_UNORM,					16, false, false },
		{ SL2_D3DFMT_UNKNOWN,			SL2_DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM,		32, false, false },
		{ SL2_D3DFMT_UNKNOWN,			SL2_DXGI_FORMAT_B8G8R8A8_UNORM_SRGB,			32, false, false },
		{ SL2_D3DFMT_UNKNOWN,			SL2_DXGI_FORMAT_B8G8R8X8_UNORM_SRGB,			32, false, false },
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

		LSI_DDS_HEADER dhHeader;
		if ( !sStream.Read( dhHeader ) ) { return false; }
		
		if ( dhHeader.dpPixelFormat.ui32Size != sizeof( dhHeader.dpPixelFormat ) ) { return false; }

		if ( (dhHeader.ui32Flags & (LSI_DF_CAPS | LSI_DF_HEIGHT | LSI_DF_WIDTH | LSI_DF_PIXELFORMAT)) !=
			(LSI_DF_CAPS | LSI_DF_HEIGHT | LSI_DF_WIDTH | LSI_DF_PIXELFORMAT) ) {
			if ( (dhHeader.ui32Flags & (LSI_DF_HEIGHT | LSI_DF_WIDTH)) !=
				(LSI_DF_HEIGHT | LSI_DF_WIDTH) ) {
				return false;
			}
		}

		LSI_DDS_HEADER_DXT10 dhdDx10Header;
		bool bUseDx10 = false;
		if ( dhHeader.dpPixelFormat.ui32Flags & LSI_DPFF_FOURCC ) {
			switch ( dhHeader.dpPixelFormat.ui32FourCC ) {
				case SL2_MAKEFOURCC( 'D', 'X', '1', '0' ) : {
					bUseDx10 = true;
					if ( !sStream.Read( dhdDx10Header ) ) { return false; }
					break;
				}
			}
		}

		return true;
	}

	/**
	 * Converts D3DFMT_UYVY to DXGI_FORMAT_YUY2 in-place.
	 * 
	 * \param _pui8SrcDst The data to convert.
	 * \param _ui32Width The width of the given data.
	 * \param _ui32Height The height of the given data.
	 * \param _ui32Pitch The row width in bytes of the given data.
	 **/
	void CDds::Convert_UYVY_to_YUY2( uint8_t * _pui8SrcDst, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Pitch ) {
		struct SL2_TEXELS {
			uint8_t				ui8U;
			uint8_t				ui8Y0;
			uint8_t				ui8V;
			uint8_t				ui8Y1;
		};

		
		for ( uint32_t H = 0; H < _ui32Height; ++H ) {
			for ( uint32_t W = 0; W < _ui32Width; ++W ) {
				SL2_TEXELS * ptTexels = reinterpret_cast<SL2_TEXELS *>(_pui8SrcDst + _ui32Pitch * H) + W;
				std::swap( ptTexels->ui8U, ptTexels->ui8Y0 );
				std::swap( ptTexels->ui8V, ptTexels->ui8Y1 );
			}
		}
	}

}	// namespace sl2

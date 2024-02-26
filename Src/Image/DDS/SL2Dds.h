/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A DDS reader and writer.
 */

#pragma once

#include "../SL2Surface.h"
#include <cstdint>
#include <memory>
#include <vector>


namespace sl2 {

	// == Enumerations.
	/**
	 * Pixel format flags.
	 */
	enum LSI_DDS_PIXEL_FORMAT_FLAGS {
		LSI_DPFF_ALPHAPIXELS			= 0x1,						/**< Texture contains alpha data; ui32RGBAlphaBitMask contains valid data. */
		LSI_DPFF_ALPHA					= 0x2,						/**< Used in some older DDS files for alpha channel only uncompressed data (ui32RGBBitCount contains the alpha channel bitcount; ui32ABitMask contains valid data). */
		LSI_DPFF_FOURCC					= 0x4,						/**< Texture contains compressed RGB data; ui32FourCC contains valid data. */
		LSI_DPFF_RGB					= 0x40,						/**< Texture contains uncompressed RGB data; dwRGBBitCount and the RGB masks (ui32RBitMask, ui32RBitMask, ui32RBitMask) contain valid data. */
		LSI_DPFF_RGBA					= LSI_DPFF_RGB |
											LSI_DPFF_ALPHAPIXELS,	/**< Texture contains uncompressed RGB datawith alpha. */
		LSI_DPFF_YUV					= 0x200,					/**< Used in some older DDS files for YUV uncompressed data (ui32RGBBitCount contains the YUV bit count; ui32RBitMask contains the Y mask, ui32GBitMask contains the U mask, ui32BBitMask contains the V mask). */
		LSI_DPFF_LUMINANCE				= 0x20000,					/**< Used in some older DDS files for single channel color uncompressed data (ui32RGBBitCount contains the luminance channel bit count; ui32RBitMask contains the channel mask). Can be combined with LSI_DPFF_ALPHAPIXELS for a two-channel DDS file. */
	};

	/**
	 * DDS flags.
	 */
	enum LSI_DDS_FLAGS {
		LSI_DF_CAPS						= 0x1,						/**< Required in every .dds file. */
		LSI_DF_HEIGHT					= 0x2,						/**< Required in every .dds file. */
		LSI_DF_WIDTH					= 0x4,						/**< Required in every .dds file. */
		LSI_DF_PITCH					= 0x8,						/**< Required when pitch is provided for an uncompressed texture. */
		LSI_DF_PIXELFORMAT				= 0x1000,					/**< Required in every .dds file. */
		LSI_DF_MIPMAPCOUNT				= 0x20000,					/**< Required in a mipmapped texture. */
		LSI_DF_LINEARSIZE				= 0x80000,					/**< Required when pitch is provided for a compressed texture. */
		LSI_DF_DEPTH					= 0x800000,					/**< Required in a depth texture. */
	};

	/**
	 * Class CDds
	 * \brief A DDS reader and writer.
	 *
	 * Description: A DDS reader and writer.
	 */
	class CDds {
	public :
		CDds();
		~CDds();


		// == Types.
		/** The DDS header pixel format structure. */
#pragma pack( push, 1 )
		typedef struct LSI_DDS_PIXELFORMAT {
			uint32_t										ui32Size;
			uint32_t										ui32Flags;
			uint32_t										ui32FourCC;
			uint32_t										ui32RGBBitCount;
			uint32_t										ui32RBitMask;
			uint32_t										ui32GBitMask;
			uint32_t										ui32BBitMask;
			uint32_t										ui32ABitMask;
		} * LPLSI_DDS_PIXELFORMAT, * const LPCLSI_DDS_PIXELFORMAT;
#pragma pack( pop )

		/** The DDS header. */
#pragma pack( push, 1 )
		typedef struct LSI_DDS_HEADER {
			uint32_t										ui32Size;
			uint32_t										ui32Flags;
			uint32_t										ui32Height;
			uint32_t										ui32Width;
			uint32_t										ui32PitchOrLinearSize;
			uint32_t										ui32Depth;
			uint32_t										ui32MipMapCount;
			uint32_t										ui32Reserved1[11];
			LSI_DDS_PIXELFORMAT								dpPixelFormat;
			uint32_t										ui32Caps;
			uint32_t										ui32Caps2;
			uint32_t										ui32Caps3;
			uint32_t										ui32Caps4;
			uint32_t										ui32Reserved2;
		} * LPLSI_DDS_HEADER, * const LPCLSI_DDS_HEADER;

		/** Extended header. */
		typedef struct LSI_DDS_HEADER_DXT10 {
			uint32_t										ui32DxgiFormat;
			uint32_t										ui32ResourceDimension;
			uint32_t										ui32MiscFlag;
			uint32_t										ui32ArraySize;
			uint32_t										ui32MiscFlags2;
		} * LPLSI_DDS_HEADER_DXT10, * const LPCLSI_DDS_HEADER_DXT10;
#pragma pack( pop )


		/**
		 * Loads a DDS file from memory.
		 *
		 * \param _vFileData The in-memory image of the file.
		 * \return Returns true if the file was successfully loaded.  False indicates an invalid file or lack of RAM.
		 */
		static bool											LoadDds( const std::vector<uint8_t>(_vFileData) );


	protected :
		// == Members.
		std::vector<std::unique_ptr<CSurface>>				m_vMipMaps;								/**< The array of mipmaps.  Index 0 is the base level. */
	};

}	// namespace sl2

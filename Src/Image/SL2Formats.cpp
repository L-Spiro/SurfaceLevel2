/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: All of the image formats, along with conversion routines and functions for working with the formats (encoding, decoding, etc.)
 */

#include "SL2Formats.h"


namespace sl2 {

	// == Members.
	/** The data for internal formats. */
	const CFormat::SL2_KTX_INTERNAL_FORMAT_DATA CFormat::m_kifdInternalFormats[] = {
#define SL2_TBITS( R, G, B, A )													R, G, B, A	// Just helps to label template parameters.
#define SL2_TSHIFTS( R, G, B, A )												R, G, B, A	// Just helps to label template parameters.
#define SL2_LBITS( L, A )														L, A		// Just helps to label template parameters.
#define SL2_LSHIFTS( L, A )														L, A		// Just helps to label template parameters.
#define SL2_IBITS( I )															I			// Just helps to label template parameters.
#define SL2_CONV_I( TTO, TFROM, TBITS, TSHIFTS, TSIZE, SIGNED, NORM, TSRGB )	TBITS, TSHIFTS, (TTO<TBITS, TSHIFTS, TSIZE, SIGNED, NORM, TSRGB>), (TFROM<TBITS, TSHIFTS, TSIZE, SIGNED, NORM, TSRGB>)
#define SL2_CONV_F( TTO, TFROM, TBITS, TSHIFTS, TSIZE )							(TTO<TBITS, TSHIFTS, TSIZE>), (TFROM<TBITS, TSHIFTS, TSIZE>)
#define SL2_CONV_L( TTO, TFROM, TBITS, TSHIFTS, TSIZE, SIGNED, NORM, TSRGB )	(CFormat::LumAlphaToRgba64F<TBITS, TSHIFTS, TSIZE, SIGNED, NORM, TSRGB>), (CFormat::LumAlphaFromRgba64F<TBITS, TSHIFTS, TSIZE, SIGNED, NORM, TSRGB>)
#define SL2_CONV_IN( TTO, TFROM, TBITS, TSIZE, SIGNED, NORM, TFLOAT )			TBITS, SL2_TSHIFTS( 0, 0, 0, 0 ), (TTO<TBITS, TSIZE, SIGNED, NORM, TFLOAT>), (TFROM<TBITS, TSIZE, SIGNED, NORM, TFLOAT>)
#define SL2_GEN_INT( TBITS, TSHIFTS, TSIZE, SIGNED, NORM, TSRGB )				SL2_CONV_I( CFormat::StdIntToRgba64F, CFormat::StdIntFromRgba64F, TBITS, TSHIFTS, TSIZE, SIGNED, NORM, TSRGB )
#define SL2_ID( VULKAN, DX, METAL, OGLINTERNAL, OGLTYPE, OGLBASE )				#VULKAN, #DX, #METAL, #OGLINTERNAL, #OGLTYPE, #OGLBASE, SL2_ ## VULKAN, SL2_ ## DX, SL2_ ## METAL, SL2_KIF_ ## OGLINTERNAL, SL2_KT_ ## OGLTYPE, SL2_KBIF_ ## OGLBASE
		//		  vfVulkanFormat		dfDxFormat		mfMetalFormat		kifInternalFormat		ktType		kbifBaseInternalFormat						ui32Flags
		//																																						  ui32PaletteSizeInBits
		//																																						  |  ui32BlockSizeInBits
		//																																						  |		ui32BlockWidth
		//																																						  |		|  ui32BlockHeight
		//																																						  |		|	  ui32BlockDepth
		//																																						  |		|		 ui32MinBlocksX
		//																																						  |		|			ui32MinBlocksY
		//																																						  |		|			   bSrgb  bCompressed
		//																																						  |		|							 bPalette
		//																																						  |		|									bFloatFormat
		//																																						  |		|										   pfCompSizeFunc
		{ SL2_ID( VK_FORMAT_R8_UNORM, DXGI_FORMAT_R8_UNORM, MTLPixelFormatR8Unorm, GL_R8, GL_UNSIGNED_BYTE, GL_RED ),										0x00, 0, 8, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_GEN_INT( SL2_TBITS(8, 0, 0, 0), SL2_TSHIFTS(0, 0, 0, 0), 1, false, true, false ) },
		{ SL2_ID( VK_FORMAT_R8G8_UNORM, DXGI_FORMAT_R8G8_UNORM, MTLPixelFormatRG8Unorm, GL_RG8, GL_UNSIGNED_BYTE, GL_RG ),									0x00, 0, 16, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_GEN_INT( SL2_TBITS(8, 8, 0, 0), SL2_TSHIFTS(0, 8, 0, 0), 2, false, true, false ) },
		{ SL2_ID( VK_FORMAT_R8G8B8_UNORM, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_RGB8, GL_UNSIGNED_BYTE, GL_RGB ),									0x00, 0, 24, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_GEN_INT( SL2_TBITS(8, 8, 8, 0), SL2_TSHIFTS(0, 8, 16, 0), 3, false, true, false ) },
		{ SL2_ID( VK_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R8G8B8A8_UNORM, MTLPixelFormatRGBA8Unorm, GL_RGBA8, GL_UNSIGNED_BYTE, GL_RGBA ),					0x00, 0, 32, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_GEN_INT( SL2_TBITS(8, 8, 8, 8), SL2_TSHIFTS(0, 8, 16, 24), 4, false, true, false ) },

		{ SL2_ID( VK_FORMAT_R8_SNORM, DXGI_FORMAT_R8_SNORM, MTLPixelFormatR8Snorm, GL_R8_SNORM, GL_BYTE, GL_RED ),											0x00, 0, 8, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_GEN_INT( SL2_TBITS(8, 0, 0, 0), SL2_TSHIFTS(0, 0, 0, 0), 1, true, true, false ) },
		{ SL2_ID( VK_FORMAT_R8G8_SNORM, DXGI_FORMAT_R8G8_SNORM, MTLPixelFormatRG8Snorm, GL_RG8_SNORM, GL_BYTE, GL_RG ),										0x00, 0, 16, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_GEN_INT( SL2_TBITS(8, 8, 0, 0), SL2_TSHIFTS(0, 8, 0, 0), 2, true, true, false ) },
		{ SL2_ID( VK_FORMAT_R8G8B8_SNORM, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_RGB8_SNORM, GL_BYTE, GL_RGB ),										0x00, 0, 24, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_GEN_INT( SL2_TBITS(8, 8, 8, 0), SL2_TSHIFTS(0, 8, 16, 0), 3, true, true, false ) },
		{ SL2_ID( VK_FORMAT_R8G8B8A8_SNORM, DXGI_FORMAT_R8G8B8A8_SNORM, MTLPixelFormatRGBA8Snorm, GL_RGBA8_SNORM, GL_BYTE, GL_RGBA ),						0x00, 0, 32, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_GEN_INT( SL2_TBITS(8, 8, 8, 8), SL2_TSHIFTS(0, 8, 16, 24), 4, true, true, false ) },

		{ SL2_ID( VK_FORMAT_R8_UINT, DXGI_FORMAT_R8_UINT, MTLPixelFormatR8Uint, GL_R8UI, GL_UNSIGNED_BYTE, GL_RED ),										0x00, 0, 8, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_GEN_INT( SL2_TBITS(8, 0, 0, 0), SL2_TSHIFTS(0, 0, 0, 0), 1, false, false, false ) },
		{ SL2_ID( VK_FORMAT_R8G8_UINT, DXGI_FORMAT_R8G8_UINT, MTLPixelFormatRG8Uint, GL_RG8UI, GL_UNSIGNED_BYTE, GL_RG ),									0x00, 0, 16, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_GEN_INT( SL2_TBITS(8, 8, 0, 0), SL2_TSHIFTS(0, 8, 0, 0), 2, false, false, false ) },
		{ SL2_ID( VK_FORMAT_R8G8B8_UINT, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_RGB8UI, GL_UNSIGNED_BYTE, GL_RGB ),									0x00, 0, 24, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_GEN_INT( SL2_TBITS(8, 8, 8, 0), SL2_TSHIFTS(0, 8, 16, 0), 3, false, false, false ) },
		{ SL2_ID( VK_FORMAT_R8G8B8A8_UINT, DXGI_FORMAT_R8G8B8A8_UINT, MTLPixelFormatRGBA8Uint, GL_RGBA8UI, GL_UNSIGNED_BYTE, GL_RGBA ),						0x00, 0, 32, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_GEN_INT( SL2_TBITS(8, 8, 8, 8), SL2_TSHIFTS(0, 8, 16, 24), 4, false, false, false ) },

		{ SL2_ID( VK_FORMAT_R8_SINT, DXGI_FORMAT_R8_SINT, MTLPixelFormatR8Sint, GL_R8I, GL_BYTE, GL_RED ),													0x00, 0, 8, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_GEN_INT( SL2_TBITS(8, 0, 0, 0), SL2_TSHIFTS(0, 0, 0, 0), 1, true, false, false ) },
		{ SL2_ID( VK_FORMAT_R8G8_SINT, DXGI_FORMAT_R8G8_SINT, MTLPixelFormatRG8Sint, GL_RG8I, GL_BYTE, GL_RG ),												0x00, 0, 16, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_GEN_INT( SL2_TBITS(8, 8, 0, 0), SL2_TSHIFTS(0, 8, 0, 0), 2, true, false, false ) },
		{ SL2_ID( VK_FORMAT_R8G8B8_SINT, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_RGB8I, GL_BYTE, GL_RGB ),											0x00, 0, 24, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_GEN_INT( SL2_TBITS(8, 8, 8, 0), SL2_TSHIFTS(0, 8, 16, 0), 3, true, false, false ) },
		{ SL2_ID( VK_FORMAT_R8G8B8A8_SINT, DXGI_FORMAT_R8G8B8A8_SINT, MTLPixelFormatRGBA8Sint, GL_RGBA8I, GL_BYTE, GL_RGBA ),								0x00, 0, 32, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_GEN_INT( SL2_TBITS(8, 8, 8, 8), SL2_TSHIFTS(0, 8, 16, 24), 4, true, false, false ) },

		{ SL2_ID( VK_FORMAT_R8_SRGB, DXGI_FORMAT_UNKNOWN, MTLPixelFormatR8Unorm_sRGB, GL_SR8, GL_UNSIGNED_BYTE, GL_RED ),									0x00, 0, 8, 1, 1, 1, 1, 1, true, false, false, false, nullptr, SL2_GEN_INT( SL2_TBITS(8, 0, 0, 0), SL2_TSHIFTS(0, 0, 0, 0), 1, false, true, true ) },
		{ SL2_ID( VK_FORMAT_R8G8_SRGB, DXGI_FORMAT_UNKNOWN, MTLPixelFormatRG8Unorm_sRGB, GL_SRG8, GL_UNSIGNED_BYTE, GL_RG ),								0x00, 0, 16, 1, 1, 1, 1, 1, true, false, false, false, nullptr, SL2_GEN_INT( SL2_TBITS(8, 8, 0, 0), SL2_TSHIFTS(0, 8, 0, 0), 2, false, true, true ) },
		{ SL2_ID( VK_FORMAT_R8G8B8_SRGB, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_SRGB8, GL_UNSIGNED_BYTE, GL_RGB ),									0x00, 0, 24, 1, 1, 1, 1, 1, true, false, false, false, nullptr, SL2_GEN_INT( SL2_TBITS(8, 8, 8, 0), SL2_TSHIFTS(0, 8, 16, 0), 3, false, true, true ) },
		{ SL2_ID( VK_FORMAT_R8G8B8A8_SRGB, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, MTLPixelFormatRGBA8Unorm_sRGB, GL_SRGB8_ALPHA8, GL_UNSIGNED_BYTE, GL_RGBA ),	0x00, 0, 32, 1, 1, 1, 1, 1, true, false, false, false, nullptr, SL2_GEN_INT( SL2_TBITS(8, 8, 8, 8), SL2_TSHIFTS(0, 8, 16, 24), 4, false, true, true ) },

		{ SL2_ID( VK_FORMAT_R16_UNORM, DXGI_FORMAT_R16_UNORM, MTLPixelFormatR16Unorm, GL_R16, GL_UNSIGNED_SHORT, GL_RED ),									0x00, 0, 16, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_GEN_INT( SL2_TBITS(16, 0, 0, 0), SL2_TSHIFTS(0, 0, 0, 0), 2, false, true, false ) },
		{ SL2_ID( VK_FORMAT_R16G16_UNORM, DXGI_FORMAT_R16G16_UNORM, MTLPixelFormatRG16Unorm, GL_RG16, GL_UNSIGNED_SHORT, GL_RG ),							0x00, 0, 32, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_GEN_INT( SL2_TBITS(16, 16, 0, 0), SL2_TSHIFTS(0, 16, 0, 0), 4, false, true, false ) },
		{ SL2_ID( VK_FORMAT_R16G16B16_UNORM, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_RGB16, GL_UNSIGNED_SHORT, GL_RGB ),								0x00, 0, 48, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_GEN_INT( SL2_TBITS(16, 16, 16, 0), SL2_TSHIFTS(0, 16, 32, 0), 6, false, true, false ) },
		{ SL2_ID( VK_FORMAT_R16G16B16A16_UNORM, DXGI_FORMAT_R16G16B16A16_UNORM, MTLPixelFormatRGBA16Unorm, GL_RGBA16, GL_UNSIGNED_SHORT, GL_RGBA ),			0x00, 0, 64, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_GEN_INT( SL2_TBITS(16, 16, 16, 16), SL2_TSHIFTS(0, 16, 32, 48), 8, false, true, false ) },

		{ SL2_ID( VK_FORMAT_R16_SNORM, DXGI_FORMAT_R16_SNORM, MTLPixelFormatR16Snorm, GL_R16_SNORM, GL_SHORT, GL_RED ),										0x00, 0, 16, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_GEN_INT( SL2_TBITS(16, 0, 0, 0), SL2_TSHIFTS(0, 0, 0, 0), 2, true, true, false ) },
		{ SL2_ID( VK_FORMAT_R16G16_SNORM, DXGI_FORMAT_R16G16_SNORM, MTLPixelFormatRG16Snorm, GL_RG16_SNORM, GL_SHORT, GL_RG ),								0x00, 0, 32, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_GEN_INT( SL2_TBITS(16, 16, 0, 0), SL2_TSHIFTS(0, 16, 0, 0), 4, true, true, false ) },
		{ SL2_ID( VK_FORMAT_R16G16B16_SNORM, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_RGB16_SNORM, GL_SHORT, GL_RGB ),								0x00, 0, 48, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_GEN_INT( SL2_TBITS(16, 16, 16, 0), SL2_TSHIFTS(0, 16, 32, 0), 6, true, true, false ) },
		{ SL2_ID( VK_FORMAT_R16G16B16A16_SNORM, DXGI_FORMAT_R16G16B16A16_SNORM, MTLPixelFormatRGBA16Snorm, GL_RGBA16_SNORM, GL_SHORT, GL_RGBA ),			0x00, 0, 64, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_GEN_INT( SL2_TBITS(16, 16, 16, 16), SL2_TSHIFTS(0, 16, 32, 48), 8, true, true, false ) },

		{ SL2_ID( VK_FORMAT_R16_UINT, DXGI_FORMAT_R16_UINT, MTLPixelFormatR16Uint, GL_R16UI, GL_UNSIGNED_SHORT, GL_RED ),									0x00, 0, 16, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_GEN_INT( SL2_TBITS(16, 0, 0, 0), SL2_TSHIFTS(0, 0, 0, 0), 2, false, false, false ) },
		{ SL2_ID( VK_FORMAT_R16G16_UINT, DXGI_FORMAT_R16G16_UINT, MTLPixelFormatRG16Uint, GL_RG16UI, GL_UNSIGNED_SHORT, GL_RG ),							0x00, 0, 32, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_GEN_INT( SL2_TBITS(16, 16, 0, 0), SL2_TSHIFTS(0, 16, 0, 0), 4, false, false, false ) },
		{ SL2_ID( VK_FORMAT_R16G16B16_UINT, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_RGB16UI, GL_UNSIGNED_SHORT, GL_RGB ),							0x00, 0, 48, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_GEN_INT( SL2_TBITS(16, 16, 16, 0), SL2_TSHIFTS(0, 16, 32, 0), 6, false, false, false ) },
		{ SL2_ID( VK_FORMAT_R16G16B16A16_UINT, DXGI_FORMAT_R16G16B16A16_UINT, MTLPixelFormatRGBA16Uint, GL_RGBA16UI, GL_UNSIGNED_SHORT, GL_RGBA ),			0x00, 0, 64, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_GEN_INT( SL2_TBITS(16, 16, 16, 16), SL2_TSHIFTS(0, 16, 32, 48), 8, false, false, false ) },

		{ SL2_ID( VK_FORMAT_R16_SINT, DXGI_FORMAT_R16_SINT, MTLPixelFormatR16Sint, GL_R16I, GL_SHORT, GL_RED ),												0x00, 0, 16, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_GEN_INT( SL2_TBITS(16, 0, 0, 0), SL2_TSHIFTS(0, 0, 0, 0), 2, true, false, false ) },
		{ SL2_ID( VK_FORMAT_R16G16_SINT, DXGI_FORMAT_R16G16_SINT, MTLPixelFormatRG16Sint, GL_RG16I, GL_SHORT, GL_RG ),										0x00, 0, 32, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_GEN_INT( SL2_TBITS(16, 16, 0, 0), SL2_TSHIFTS(0, 16, 0, 0), 4, true, false, false ) },
		{ SL2_ID( VK_FORMAT_R16G16B16_SINT, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_RGB16I, GL_SHORT, GL_RGB ),										0x00, 0, 48, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_GEN_INT( SL2_TBITS(16, 16, 16, 0), SL2_TSHIFTS(0, 16, 32, 0), 6, true, false, false ) },
		{ SL2_ID( VK_FORMAT_R16G16B16A16_SINT, DXGI_FORMAT_R16G16B16A16_SINT, MTLPixelFormatRGBA16Sint, GL_RGBA16I, GL_SHORT, GL_RGBA ),					0x00, 0, 64, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_GEN_INT( SL2_TBITS(16, 16, 16, 16), SL2_TSHIFTS(0, 16, 32, 48), 8, true, false, false ) },
		
		{ SL2_ID( VK_FORMAT_R16_SFLOAT, DXGI_FORMAT_R16_FLOAT, MTLPixelFormatR16Float, GL_R16F, GL_HALF_FLOAT, GL_RED ),									0x00, 0, 16, 1, 1, 1, 1, 1, false, false, false, true, nullptr, SL2_TBITS(16, 0, 0, 0), SL2_TSHIFTS(0, 0, 0, 0), SL2_CONV_F( CFormat::F16ToRgba64F, CFormat::F16FromRgba64F, SL2_TBITS(16, 0, 0, 0), SL2_TSHIFTS(0, 0, 0, 0), 2 ) },
		{ SL2_ID( VK_FORMAT_R16G16_SFLOAT, DXGI_FORMAT_R16G16_FLOAT, MTLPixelFormatRG16Float, GL_RG16F, GL_HALF_FLOAT, GL_RG ),								0x00, 0, 32, 1, 1, 1, 1, 1, false, false, false, true, nullptr, SL2_TBITS(16, 16, 0, 0), SL2_TSHIFTS(0, 16, 0, 0), SL2_CONV_F( CFormat::F16ToRgba64F, CFormat::F16FromRgba64F, SL2_TBITS(16, 16, 0, 0), SL2_TSHIFTS(0, 16, 0, 0), 4 ) },
		{ SL2_ID( VK_FORMAT_R16G16B16_SFLOAT, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_RGB16F, GL_HALF_FLOAT, GL_RGB ),								0x00, 0, 48, 1, 1, 1, 1, 1, false, false, false, true, nullptr, SL2_TBITS(16, 16, 16, 0), SL2_TSHIFTS(0, 16, 32, 0), SL2_CONV_F( CFormat::F16ToRgba64F, CFormat::F16FromRgba64F, SL2_TBITS(16, 16, 16, 0), SL2_TSHIFTS(0, 16, 32, 0), 6 ) },
		{ SL2_ID( VK_FORMAT_R16G16B16A16_SFLOAT, DXGI_FORMAT_R16G16B16A16_FLOAT, MTLPixelFormatRGBA16Float, GL_RGBA16F, GL_HALF_FLOAT, GL_RGBA ),			0x00, 0, 64, 1, 1, 1, 1, 1, false, false, false, true, nullptr, SL2_TBITS(16, 16, 16, 16), SL2_TSHIFTS(0, 16, 32, 48), SL2_CONV_F( CFormat::F16ToRgba64F, CFormat::F16FromRgba64F, SL2_TBITS(16, 16, 16, 16), SL2_TSHIFTS(0, 16, 32, 48), 8 ) },
		
		{ SL2_ID( VK_FORMAT_R32_UINT, DXGI_FORMAT_R32_UINT, MTLPixelFormatR32Uint, GL_R32UI, GL_UNSIGNED_INT, GL_RED ),										0x00, 0, 32, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_CONV_I( CFormat::Int128ToRgba64F, CFormat::Int128FromRgba64F, SL2_TBITS(32, 0, 0, 0), SL2_TSHIFTS(0, 0, 0, 0), 4, false, false, false ) },
		{ SL2_ID( VK_FORMAT_R32G32_UINT, DXGI_FORMAT_R32G32_UINT, MTLPixelFormatRG32Uint, GL_RG32UI, GL_UNSIGNED_INT, GL_RG ),								0x00, 0, 64, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_CONV_I( CFormat::Int128ToRgba64F, CFormat::Int128FromRgba64F, SL2_TBITS(32, 32, 0, 0), SL2_TSHIFTS(0, 32, 0, 0), 8, false, false, false ) },
		{ SL2_ID( VK_FORMAT_R32G32B32_UINT, DXGI_FORMAT_R32G32B32_UINT, MTLPixelFormatInvalid, GL_RGB32UI, GL_UNSIGNED_INT, GL_RGB ),						0x00, 0, 96, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_CONV_I( CFormat::Int128ToRgba64F, CFormat::Int128FromRgba64F, SL2_TBITS(32, 32, 32, 0), SL2_TSHIFTS(0, 32, 64, 0), 12, false, false, false ) },
		{ SL2_ID( VK_FORMAT_R32G32B32A32_UINT, DXGI_FORMAT_R32G32B32A32_UINT, MTLPixelFormatRGBA32Uint, GL_RGBA32UI, GL_UNSIGNED_INT, GL_RGBA ),			0x00, 0, 128, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_CONV_I( CFormat::Int128ToRgba64F, CFormat::Int128FromRgba64F, SL2_TBITS(32, 32, 32, 32), SL2_TSHIFTS(0, 32, 64, 96), 16, false, false, false ) },
		
		{ SL2_ID( VK_FORMAT_R32_SINT, DXGI_FORMAT_R32_SINT, MTLPixelFormatR32Sint, GL_R32I, GL_INT, GL_RED ),												0x00, 0, 32, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_CONV_I( CFormat::Int128ToRgba64F, CFormat::Int128FromRgba64F, SL2_TBITS(32, 0, 0, 0), SL2_TSHIFTS(0, 0, 0, 0), 4, true, false, false ) },
		{ SL2_ID( VK_FORMAT_R32G32_SINT, DXGI_FORMAT_R32G32_SINT, MTLPixelFormatRG32Sint, GL_RG32I, GL_INT, GL_RG ),										0x00, 0, 64, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_CONV_I( CFormat::Int128ToRgba64F, CFormat::Int128FromRgba64F, SL2_TBITS(32, 32, 0, 0), SL2_TSHIFTS(0, 32, 0, 0), 8, true, false, false ) },
		{ SL2_ID( VK_FORMAT_R32G32B32_SINT, DXGI_FORMAT_R32G32B32_SINT, MTLPixelFormatInvalid, GL_RGB32I, GL_INT, GL_RGB ),									0x00, 0, 96, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_CONV_I( CFormat::Int128ToRgba64F, CFormat::Int128FromRgba64F, SL2_TBITS(32, 32, 32, 0), SL2_TSHIFTS(0, 32, 64, 0), 12, true, false, false ) },
		{ SL2_ID( VK_FORMAT_R32G32B32A32_SINT, DXGI_FORMAT_R32G32B32A32_SINT, MTLPixelFormatRGBA32Sint, GL_RGBA32I, GL_INT, GL_RGBA ),						0x00, 0, 128, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_CONV_I( CFormat::Int128ToRgba64F, CFormat::Int128FromRgba64F, SL2_TBITS(32, 32, 32, 32), SL2_TSHIFTS(0, 32, 64, 96), 16, true, false, false ) },
		
		{ SL2_ID( VK_FORMAT_R32_SFLOAT, DXGI_FORMAT_R32_FLOAT, MTLPixelFormatR32Float, GL_R32F, GL_FLOAT, GL_RED ),											0x00, 0, 32, 1, 1, 1, 1, 1, false, false, false, true, nullptr, SL2_TBITS(32, 0, 0, 0), SL2_TSHIFTS(0, 0, 0, 0), SL2_CONV_F( CFormat::F32ToRgba64F, CFormat::F32FromRgba64F, SL2_TBITS(32, 0, 0, 0), SL2_TSHIFTS(0, 0, 0, 0), 4 ) },
		{ SL2_ID( VK_FORMAT_R32G32_SFLOAT, DXGI_FORMAT_R32G32_FLOAT, MTLPixelFormatRG32Float, GL_RG32F, GL_FLOAT, GL_RG ),									0x00, 0, 64, 1, 1, 1, 1, 1, false, false, false, true, nullptr, SL2_TBITS(32, 32, 0, 0), SL2_TSHIFTS(0, 32, 0, 0), SL2_CONV_F( CFormat::F32ToRgba64F, CFormat::F32FromRgba64F, SL2_TBITS(32, 32, 0, 0), SL2_TSHIFTS(0, 32, 0, 0), 8 ) },
		{ SL2_ID( VK_FORMAT_R32G32B32_SFLOAT, DXGI_FORMAT_R32G32B32_FLOAT, MTLPixelFormatInvalid, GL_RGB32F, GL_FLOAT, GL_RGB ),							0x00, 0, 96, 1, 1, 1, 1, 1, false, false, false, true, nullptr, SL2_TBITS(32, 32, 32, 0), SL2_TSHIFTS(0, 32, 64, 0), SL2_CONV_F( CFormat::F32ToRgba64F, CFormat::F32FromRgba64F, SL2_TBITS(32, 32, 32, 0), SL2_TSHIFTS(0, 32, 64, 0), 12 ) },
		{ SL2_ID( VK_FORMAT_R32G32B32A32_SFLOAT, DXGI_FORMAT_R32G32B32A32_FLOAT, MTLPixelFormatRGBA32Float, GL_RGBA32F, GL_FLOAT, GL_RGBA ),				0x00, 0, 128, 1, 1, 1, 1, 1, false, false, false, true, nullptr, SL2_TBITS(32, 32, 32, 32), SL2_TSHIFTS(0, 32, 64, 96), SL2_CONV_F( CFormat::F32ToRgba64F, CFormat::F32FromRgba64F, SL2_TBITS(32, 32, 32, 32), SL2_TSHIFTS(0, 32, 64, 96), 16 ) },

		{ SL2_ID( VK_FORMAT_R64_UINT, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_INVALID, GL_INVALID, GL_INVALID ),										0x00, 0, 64, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_CONV_I( CFormat::Int256ToRgba64F, CFormat::Int256FromRgba64F, SL2_TBITS(64, 0, 0, 0), SL2_TSHIFTS(0, 0, 0, 0), 8, false, false, false ) },
		{ SL2_ID( VK_FORMAT_R64G64_UINT, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_INVALID, GL_INVALID, GL_INVALID ),									0x00, 0, 128, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_CONV_I( CFormat::Int256ToRgba64F, CFormat::Int256FromRgba64F, SL2_TBITS(64, 64, 0, 0), SL2_TSHIFTS(0, 64, 0, 0), 16, false, false, false ) },
		{ SL2_ID( VK_FORMAT_R64G64B64_UINT, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_INVALID, GL_INVALID, GL_INVALID ),								0x00, 0, 192, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_CONV_I( CFormat::Int256ToRgba64F, CFormat::Int256FromRgba64F, SL2_TBITS(64, 64, 64, 0), SL2_TSHIFTS(0, 64, 128, 0), 24, false, false, false ) },
		{ SL2_ID( VK_FORMAT_R64G64B64A64_UINT, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_INVALID, GL_INVALID, GL_INVALID ),							0x00, 0, 256, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_CONV_I( CFormat::Int256ToRgba64F, CFormat::Int256FromRgba64F, SL2_TBITS(64, 64, 64, 64), SL2_TSHIFTS(0, 64, 128, 192), 32, false, false, false ) },

		{ SL2_ID( VK_FORMAT_R64_SINT, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_INVALID, GL_INVALID, GL_INVALID ),										0x00, 0, 64, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_CONV_I( CFormat::Int256ToRgba64F, CFormat::Int256FromRgba64F, SL2_TBITS(64, 0, 0, 0), SL2_TSHIFTS(0, 0, 0, 0), 8, true, false, false ) },
		{ SL2_ID( VK_FORMAT_R64G64_SINT, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_INVALID, GL_INVALID, GL_INVALID ),									0x00, 0, 128, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_CONV_I( CFormat::Int256ToRgba64F, CFormat::Int256FromRgba64F, SL2_TBITS(64, 64, 0, 0), SL2_TSHIFTS(0, 64, 0, 0), 16, true, false, false ) },
		{ SL2_ID( VK_FORMAT_R64G64B64_SINT, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_INVALID, GL_INVALID, GL_INVALID ),								0x00, 0, 192, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_CONV_I( CFormat::Int256ToRgba64F, CFormat::Int256FromRgba64F, SL2_TBITS(64, 64, 64, 0), SL2_TSHIFTS(0, 64, 128, 0), 24, true, false, false ) },
		{ SL2_ID( VK_FORMAT_R64G64B64A64_SINT, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_INVALID, GL_INVALID, GL_INVALID ),							0x00, 0, 256, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_CONV_I( CFormat::Int256ToRgba64F, CFormat::Int256FromRgba64F, SL2_TBITS(64, 64, 64, 64), SL2_TSHIFTS(0, 64, 128, 192), 32, true, false, false ) },

		{ SL2_ID( VK_FORMAT_R64_SFLOAT, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_INVALID, GL_INVALID, GL_INVALID ),									0x00, 0, 64, 1, 1, 1, 1, 1, false, false, false, true, nullptr, SL2_TBITS(64, 0, 0, 0), SL2_TSHIFTS(0, 0, 0, 0), SL2_CONV_F( CFormat::F64ToRgba64F, CFormat::F64FromRgba64F, SL2_TBITS(64, 0, 0, 0), SL2_TSHIFTS(0, 0, 0, 0), 8 ) },
		{ SL2_ID( VK_FORMAT_R64G64_SFLOAT, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_INVALID, GL_INVALID, GL_INVALID ),								0x00, 0, 128, 1, 1, 1, 1, 1, false, false, false, true, nullptr, SL2_TBITS(64, 64, 0, 0), SL2_TSHIFTS(0, 64, 0, 0), SL2_CONV_F( CFormat::F64ToRgba64F, CFormat::F64FromRgba64F, SL2_TBITS(64, 64, 0, 0), SL2_TSHIFTS(0, 64, 0, 0), 16 ) },
		{ SL2_ID( VK_FORMAT_R64G64B64_SFLOAT, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_INVALID, GL_INVALID, GL_INVALID ),								0x00, 0, 192, 1, 1, 1, 1, 1, false, false, false, true, nullptr, SL2_TBITS(64, 64, 64, 0), SL2_TSHIFTS(0, 64, 128, 0), SL2_CONV_F( CFormat::F64ToRgba64F, CFormat::F64FromRgba64F, SL2_TBITS(64, 64, 64, 0), SL2_TSHIFTS(0, 64, 128, 0), 24 ) },
		{ SL2_ID( VK_FORMAT_R64G64B64A64_SFLOAT, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_INVALID, GL_INVALID, GL_INVALID ),							0x00, 0, 256, 1, 1, 1, 1, 1, false, false, false, true, nullptr, SL2_TBITS(64, 64, 64, 64), SL2_TSHIFTS(0, 64, 128, 192), SL2_CONV_F( CFormat::F64ToRgba64F, CFormat::F64FromRgba64F, SL2_TBITS(64, 64, 64, 64), SL2_TSHIFTS(0, 64, 128, 192), 32 ) },
		
		{ SL2_ID( VK_FORMAT_UNDEFINED, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_R3_G3_B2, GL_UNSIGNED_BYTE_2_3_3_REV, GL_RGB ),						0x01, 0, 8, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_GEN_INT( SL2_TBITS(3, 3, 2, 0), SL2_TSHIFTS(0, 3, 6, 0), 1, false, true, false ) },
		{ SL2_ID( VK_FORMAT_UNDEFINED, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_RGB4, GL_UNSIGNED_BYTE, GL_RGB ),										0x01, 0, 24, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_GEN_INT( SL2_TBITS(8, 8, 8, 0), SL2_TSHIFTS(0, 8, 16, 0), 3, false, true, false ) },
		{ SL2_ID( VK_FORMAT_UNDEFINED, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_RGB5, GL_UNSIGNED_BYTE, GL_RGB ),										0x01, 0, 24, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_GEN_INT( SL2_TBITS(8, 8, 8, 0), SL2_TSHIFTS(0, 8, 16, 0), 3, false, true, false ) },
		{ SL2_ID( VK_FORMAT_R5G6B5_UNORM_PACK16, DXGI_FORMAT_B5G6R5_UNORM, MTLPixelFormatB5G6R5Unorm, GL_RGB565, GL_UNSIGNED_SHORT_5_6_5, GL_RGB ),			0x01, 0, 16, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_GEN_INT( SL2_TBITS(5, 6, 5, 0), SL2_TSHIFTS(11, 5, 0, 0), 2, false, true, false ) },
		{ SL2_ID( VK_FORMAT_UNDEFINED, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_RGB10, GL_UNSIGNED_SHORT, GL_RGB ),									0x01, 0, 48, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_GEN_INT( SL2_TBITS(16, 16, 16, 0), SL2_TSHIFTS(0, 16, 32, 0), 6, false, true, false ) },
		{ SL2_ID( VK_FORMAT_UNDEFINED, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_RGB12, GL_UNSIGNED_SHORT, GL_RGB ),									0x01, 0, 48, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_GEN_INT( SL2_TBITS(16, 16, 16, 0), SL2_TSHIFTS(0, 16, 32, 0), 6, false, true, false ) },
		{ SL2_ID( VK_FORMAT_UNDEFINED, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_RGBA2, GL_UNSIGNED_SHORT, GL_RGBA ),									0x01, 0, 32, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_GEN_INT( SL2_TBITS(8, 8, 8, 8), SL2_TSHIFTS(0, 8, 16, 24), 4, false, true, false ) },
		{ SL2_ID( VK_FORMAT_A4R4G4B4_UNORM_PACK16, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_RGBA4, GL_UNSIGNED_SHORT_4_4_4_4, GL_RGBA ),				0x01, 0, 16, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_GEN_INT( SL2_TBITS(4, 4, 4, 4), SL2_TSHIFTS(12, 8, 4, 0), 2, false, true, false ) },
		{ SL2_ID( VK_FORMAT_UNDEFINED, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_RGBA12, GL_UNSIGNED_SHORT, GL_RGBA ),									0x01, 0, 64, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_GEN_INT( SL2_TBITS(16, 16, 16, 1), SL2_TSHIFTS(0, 16, 32, 48), 8, false, true, false ) },
		{ SL2_ID( VK_FORMAT_R5G5B5A1_UNORM_PACK16, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_RGB5_A1, GL_UNSIGNED_SHORT_5_5_5_1, GL_RGBA ),			0x01, 0, 32, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_GEN_INT( SL2_TBITS(5, 5, 5, 1), SL2_TSHIFTS(11, 6, 1, 0), 2, false, true, false ) },
		{ SL2_ID( VK_FORMAT_A2R10G10B10_UNORM_PACK32, DXGI_FORMAT_R10G10B10A2_UNORM, MTLPixelFormatRGB10A2Unorm, GL_RGB10_A2, GL_UNSIGNED_INT_2_10_10_10_REV, GL_RGBA ),
																																							0x01, 0, 32, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_GEN_INT( SL2_TBITS(10, 10, 10, 2), SL2_TSHIFTS(0, 10, 20, 30), 4, false, true, false ) },
		{ SL2_ID( VK_FORMAT_A2B10G10R10_UINT_PACK32, DXGI_FORMAT_R10G10B10A2_UINT, MTLPixelFormatRGB10A2Uint, GL_RGB10_A2UI, GL_UNSIGNED_INT_2_10_10_10_REV, GL_RGBA ),
																																							0x01, 0, 32, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_GEN_INT( SL2_TBITS(10, 10, 10, 2), SL2_TSHIFTS(0, 10, 20, 30), 4, false, false, false ) },
		{ SL2_ID( VK_FORMAT_B10G11R11_UFLOAT_PACK32, DXGI_FORMAT_R11G11B10_FLOAT, MTLPixelFormatRG11B10Float, GL_R11F_G11F_B10F, GL_UNSIGNED_INT_10F_11F_11F_REV, GL_RGB ),
																																							0x01, 0, 32, 1, 1, 1, 1, 1, false, false, false, true, nullptr, SL2_TBITS(11, 11, 10, 0), SL2_TSHIFTS(0, 11, 22, 0), CFormat::R11G11B10FToRgba64F, CFormat::R11G11B10FFromRgba64F },
		{ SL2_ID( VK_FORMAT_E5B9G9R9_UFLOAT_PACK32, DXGI_FORMAT_R9G9B9E5_SHAREDEXP, MTLPixelFormatRGB9E5Float, GL_RGB9_E5, GL_UNSIGNED_INT_5_9_9_9_REV, GL_RGB ),
																																							0x01, 0, 32, 1, 1, 1, 1, 1, false, false, false, true, nullptr, SL2_TBITS(9, 9, 9, 0), SL2_TSHIFTS(9, 18, 27, 0), CFormat::RGB9E5ToRgba64F, CFormat::RGB9E5FromRgba64F },
		
		{ SL2_ID( VK_FORMAT_UNDEFINED, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_ALPHA4, GL_UNSIGNED_BYTE, GL_ALPHA ),									0x00, 0, 8, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_TBITS(0, 0, 0, 4), SL2_TSHIFTS(0, 0, 0, 0), SL2_CONV_L( CFormat::LumAlphaToRgba64F, CFormat::LumAlphaFromRgba64F, SL2_LBITS(0, 4), SL2_LSHIFTS(0, 0), 1, false, true, false ) },
		{ SL2_ID( VK_FORMAT_UNDEFINED, DXGI_FORMAT_A8_UNORM, MTLPixelFormatA8Unorm, GL_ALPHA8, GL_UNSIGNED_BYTE, GL_ALPHA ),								0x00, 0, 8, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_TBITS(0, 0, 0, 8), SL2_TSHIFTS(0, 0, 0, 0), SL2_CONV_L( CFormat::LumAlphaToRgba64F, CFormat::LumAlphaFromRgba64F, SL2_LBITS(0, 8), SL2_LSHIFTS(0, 0), 1, false, true, false ) },
		{ SL2_ID( VK_FORMAT_UNDEFINED, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_ALPHA8_SNORM, GL_BYTE, GL_ALPHA ),									0x00, 0, 8, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_TBITS(0, 0, 0, 8), SL2_TSHIFTS(0, 0, 0, 0), SL2_CONV_L( CFormat::LumAlphaToRgba64F, CFormat::LumAlphaFromRgba64F, SL2_LBITS(0, 8), SL2_LSHIFTS(0, 0), 1, true, true, false ) },
		{ SL2_ID( VK_FORMAT_UNDEFINED, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_ALPHA8UI_EXT, GL_UNSIGNED_BYTE, GL_ALPHA_INTEGER ),					0x00, 0, 8, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_TBITS(0, 0, 0, 8), SL2_TSHIFTS(0, 0, 0, 0), SL2_CONV_L( CFormat::LumAlphaToRgba64F, CFormat::LumAlphaFromRgba64F, SL2_LBITS(0, 8), SL2_LSHIFTS(0, 0), 1, false, false, false ) },
		{ SL2_ID( VK_FORMAT_UNDEFINED, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_ALPHA8I_EXT, GL_BYTE, GL_ALPHA_INTEGER ),								0x00, 0, 8, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_TBITS(0, 0, 0, 8), SL2_TSHIFTS(0, 0, 0, 0), SL2_CONV_L( CFormat::LumAlphaToRgba64F, CFormat::LumAlphaFromRgba64F, SL2_LBITS(0, 8), SL2_LSHIFTS(0, 0), 1, true, false, false ) },
		{ SL2_ID( VK_FORMAT_UNDEFINED, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_ALPHA12, GL_UNSIGNED_SHORT, GL_ALPHA ),								0x00, 0, 16, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_TBITS(0, 0, 0, 12), SL2_TSHIFTS(0, 0, 0, 0), SL2_CONV_L( CFormat::LumAlphaToRgba64F, CFormat::LumAlphaFromRgba64F, SL2_LBITS(0, 12), SL2_LSHIFTS(0, 0), 2, false, true, false ) },
		{ SL2_ID( VK_FORMAT_UNDEFINED, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_ALPHA16, GL_UNSIGNED_SHORT, GL_ALPHA ),								0x00, 0, 16, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_TBITS(0, 0, 0, 16), SL2_TSHIFTS(0, 0, 0, 0), SL2_CONV_L( CFormat::LumAlphaToRgba64F, CFormat::LumAlphaFromRgba64F, SL2_LBITS(0, 16), SL2_LSHIFTS(0, 0), 2, false, true, false ) },
		{ SL2_ID( VK_FORMAT_UNDEFINED, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_ALPHA16_SNORM, GL_SHORT, GL_ALPHA ),									0x00, 0, 16, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_TBITS(0, 0, 0, 16), SL2_TSHIFTS(0, 0, 0, 0), SL2_CONV_L( CFormat::LumAlphaToRgba64F, CFormat::LumAlphaFromRgba64F, SL2_LBITS(0, 16), SL2_LSHIFTS(0, 0), 2, true, true, false ) },
		{ SL2_ID( VK_FORMAT_UNDEFINED, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_ALPHA16UI_EXT, GL_UNSIGNED_SHORT, GL_ALPHA_INTEGER ),					0x00, 0, 16, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_TBITS(0, 0, 0, 16), SL2_TSHIFTS(0, 0, 0, 0), SL2_CONV_L( CFormat::LumAlphaToRgba64F, CFormat::LumAlphaFromRgba64F, SL2_LBITS(0, 16), SL2_LSHIFTS(0, 0), 2, false, false, false ) },
		{ SL2_ID( VK_FORMAT_UNDEFINED, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_ALPHA16I_EXT, GL_SHORT, GL_ALPHA_INTEGER ),							0x00, 0, 16, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_TBITS(0, 0, 0, 16), SL2_TSHIFTS(0, 0, 0, 0), SL2_CONV_L( CFormat::LumAlphaToRgba64F, CFormat::LumAlphaFromRgba64F, SL2_LBITS(0, 16), SL2_LSHIFTS(0, 0), 2, true, false, false ) },
		{ SL2_ID( VK_FORMAT_UNDEFINED, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_ALPHA16F_ARB, GL_HALF_FLOAT, GL_ALPHA ),								0x00, 0, 16, 1, 1, 1, 1, 1, false, false, false, true, nullptr, SL2_TBITS(0, 0, 0, 16), SL2_TSHIFTS(0, 0, 0, 0), SL2_CONV_F( CFormat::LumAlphaFToRgba64F, CFormat::LumAlphaFFromRgba64F, SL2_LBITS(0, 16), SL2_LSHIFTS(0, 0), 2 ) },
		{ SL2_ID( VK_FORMAT_UNDEFINED, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_ALPHA32UI_EXT, GL_UNSIGNED_INT, GL_ALPHA_INTEGER ),					0x00, 0, 32, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_TBITS(0, 0, 0, 32), SL2_TSHIFTS(0, 0, 0, 0), SL2_CONV_L( CFormat::LumAlphaToRgba64F, CFormat::LumAlphaFromRgba64F, SL2_LBITS(0, 32), SL2_LSHIFTS(0, 0), 4, false, false, false ) },
		{ SL2_ID( VK_FORMAT_UNDEFINED, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_ALPHA32I_EXT, GL_INT, GL_ALPHA_INTEGER ),								0x00, 0, 32, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_TBITS(0, 0, 0, 32), SL2_TSHIFTS(0, 0, 0, 0), SL2_CONV_L( CFormat::LumAlphaToRgba64F, CFormat::LumAlphaFromRgba64F, SL2_LBITS(0, 32), SL2_LSHIFTS(0, 0), 4, true, false, false ) },
		{ SL2_ID( VK_FORMAT_UNDEFINED, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_ALPHA32F_ARB, GL_FLOAT, GL_ALPHA ),									0x00, 0, 32, 1, 1, 1, 1, 1, false, false, false, true, nullptr, SL2_TBITS(0, 0, 0, 32), SL2_TSHIFTS(0, 0, 0, 0), SL2_CONV_F( CFormat::LumAlphaFToRgba64F, CFormat::LumAlphaFFromRgba64F, SL2_LBITS(0, 32), SL2_LSHIFTS(0, 0), 4 ) },
		
		{ SL2_ID( VK_FORMAT_UNDEFINED, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_LUMINANCE4, GL_UNSIGNED_BYTE, GL_LUMINANCE ),							0x00, 0, 8, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_TBITS(4, 0, 0, 0), SL2_TSHIFTS(0, 0, 0, 0), SL2_CONV_L( CFormat::LumAlphaToRgba64F, CFormat::LumAlphaFromRgba64F, SL2_LBITS(4, 0), SL2_LSHIFTS(0, 0), 1, false, true, false ) },
		{ SL2_ID( VK_FORMAT_UNDEFINED, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_LUMINANCE8, GL_UNSIGNED_BYTE, GL_LUMINANCE ),							0x00, 0, 8, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_TBITS(8, 0, 0, 0), SL2_TSHIFTS(0, 0, 0, 0), SL2_CONV_L( CFormat::LumAlphaToRgba64F, CFormat::LumAlphaFromRgba64F, SL2_LBITS(8, 0), SL2_LSHIFTS(0, 0), 1, false, true, false ) },
		{ SL2_ID( VK_FORMAT_UNDEFINED, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_LUMINANCE8_SNORM, GL_BYTE, GL_LUMINANCE ),							0x00, 0, 8, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_TBITS(8, 0, 0, 0), SL2_TSHIFTS(0, 0, 0, 0), SL2_CONV_L( CFormat::LumAlphaToRgba64F, CFormat::LumAlphaFromRgba64F, SL2_LBITS(8, 0), SL2_LSHIFTS(0, 0), 1, true, true, false ) },
		{ SL2_ID( VK_FORMAT_UNDEFINED, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_SLUMINANCE8, GL_UNSIGNED_BYTE, GL_LUMINANCE ),						0x00, 0, 8, 1, 1, 1, 1, 1, true, false, false, false, nullptr, SL2_TBITS(8, 0, 0, 0), SL2_TSHIFTS(0, 0, 0, 0), SL2_CONV_L( CFormat::LumAlphaToRgba64F, CFormat::LumAlphaFromRgba64F, SL2_LBITS(8, 0), SL2_LSHIFTS(0, 0), 1, false, true, true ) },
		{ SL2_ID( VK_FORMAT_UNDEFINED, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_LUMINANCE8UI_EXT, GL_UNSIGNED_BYTE, GL_LUMINANCE_INTEGER ),			0x00, 0, 8, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_TBITS(8, 0, 0, 0), SL2_TSHIFTS(0, 0, 0, 0), SL2_CONV_L( CFormat::LumAlphaToRgba64F, CFormat::LumAlphaFromRgba64F, SL2_LBITS(8, 0), SL2_LSHIFTS(0, 0), 1, false, false, false ) },
		{ SL2_ID( VK_FORMAT_UNDEFINED, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_LUMINANCE8I_EXT, GL_BYTE, GL_LUMINANCE_INTEGER ),						0x00, 0, 8, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_TBITS(8, 0, 0, 0), SL2_TSHIFTS(0, 0, 0, 0), SL2_CONV_L( CFormat::LumAlphaToRgba64F, CFormat::LumAlphaFromRgba64F, SL2_LBITS(8, 0), SL2_LSHIFTS(0, 0), 1, true, false, false ) },
		{ SL2_ID( VK_FORMAT_UNDEFINED, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_LUMINANCE12, GL_UNSIGNED_SHORT, GL_LUMINANCE ),						0x00, 0, 16, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_TBITS(12, 0, 0, 0), SL2_TSHIFTS(0, 0, 0, 0), SL2_CONV_L( CFormat::LumAlphaToRgba64F, CFormat::LumAlphaFromRgba64F, SL2_LBITS(12, 0), SL2_LSHIFTS(0, 0), 2, false, true, false ) },
		{ SL2_ID( VK_FORMAT_UNDEFINED, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_LUMINANCE16, GL_UNSIGNED_SHORT, GL_LUMINANCE ),						0x00, 0, 16, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_TBITS(16, 0, 0, 0), SL2_TSHIFTS(0, 0, 0, 0), SL2_CONV_L( CFormat::LumAlphaToRgba64F, CFormat::LumAlphaFromRgba64F, SL2_LBITS(16, 0), SL2_LSHIFTS(0, 0), 2, false, true, false ) },
		{ SL2_ID( VK_FORMAT_UNDEFINED, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_LUMINANCE16_SNORM, GL_SHORT, GL_LUMINANCE ),							0x00, 0, 16, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_TBITS(16, 0, 0, 0), SL2_TSHIFTS(0, 0, 0, 0), SL2_CONV_L( CFormat::LumAlphaToRgba64F, CFormat::LumAlphaFromRgba64F, SL2_LBITS(16, 0), SL2_LSHIFTS(0, 0), 2, true, true, false ) },
		{ SL2_ID( VK_FORMAT_UNDEFINED, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_LUMINANCE16UI_EXT, GL_UNSIGNED_SHORT, GL_LUMINANCE_INTEGER ),			0x00, 0, 16, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_TBITS(16, 0, 0, 0), SL2_TSHIFTS(0, 0, 0, 0), SL2_CONV_L( CFormat::LumAlphaToRgba64F, CFormat::LumAlphaFromRgba64F, SL2_LBITS(16, 0), SL2_LSHIFTS(0, 0), 2, false, false, false ) },
		{ SL2_ID( VK_FORMAT_UNDEFINED, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_LUMINANCE16I_EXT, GL_SHORT, GL_LUMINANCE_INTEGER ),					0x00, 0, 16, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_TBITS(16, 0, 0, 0), SL2_TSHIFTS(0, 0, 0, 0), SL2_CONV_L( CFormat::LumAlphaToRgba64F, CFormat::LumAlphaFromRgba64F, SL2_LBITS(16, 0), SL2_LSHIFTS(0, 0), 2, true, false, false ) },
		{ SL2_ID( VK_FORMAT_UNDEFINED, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_LUMINANCE16F_ARB, GL_HALF_FLOAT, GL_LUMINANCE ),						0x00, 0, 16, 1, 1, 1, 1, 1, false, false, false, true, nullptr, SL2_TBITS(16, 0, 0, 0), SL2_TSHIFTS(0, 0, 0, 0), SL2_CONV_F( CFormat::LumAlphaFToRgba64F, CFormat::LumAlphaFFromRgba64F, SL2_LBITS(16, 0), SL2_LSHIFTS(0, 0), 2 ) },
		{ SL2_ID( VK_FORMAT_UNDEFINED, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_LUMINANCE32UI_EXT, GL_UNSIGNED_INT, GL_LUMINANCE_INTEGER ),			0x00, 0, 32, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_TBITS(32, 0, 0, 0), SL2_TSHIFTS(0, 0, 0, 0), SL2_CONV_L( CFormat::LumAlphaToRgba64F, CFormat::LumAlphaFromRgba64F, SL2_LBITS(32, 0), SL2_LSHIFTS(0, 0), 4, false, false, false ) },
		{ SL2_ID( VK_FORMAT_UNDEFINED, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_LUMINANCE32I_EXT, GL_INT, GL_LUMINANCE_INTEGER ),						0x00, 0, 32, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_TBITS(32, 0, 0, 0), SL2_TSHIFTS(0, 0, 0, 0), SL2_CONV_L( CFormat::LumAlphaToRgba64F, CFormat::LumAlphaFromRgba64F, SL2_LBITS(32, 0), SL2_LSHIFTS(0, 0), 4, true, false, false ) },
		{ SL2_ID( VK_FORMAT_UNDEFINED, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_LUMINANCE32F_ARB, GL_FLOAT, GL_LUMINANCE ),							0x00, 0, 32, 1, 1, 1, 1, 1, false, false, false, true, nullptr, SL2_TBITS(32, 0, 0, 0), SL2_TSHIFTS(0, 0, 0, 0), SL2_CONV_F( CFormat::LumAlphaFToRgba64F, CFormat::LumAlphaFFromRgba64F, SL2_LBITS(32, 0), SL2_LSHIFTS(0, 0), 4 ) },
		
		{ SL2_ID( VK_FORMAT_UNDEFINED, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_LUMINANCE4_ALPHA4, GL_UNSIGNED_BYTE, GL_LUMINANCE_ALPHA ),			0x00, 0, 8, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_TBITS(4, 0, 0, 4), SL2_TSHIFTS(0, 0, 0, 4), SL2_CONV_L( CFormat::LumAlphaToRgba64F, CFormat::LumAlphaFromRgba64F, SL2_LBITS(4, 4), SL2_LSHIFTS(0, 4), 1, false, true, false ) },
		{ SL2_ID( VK_FORMAT_UNDEFINED, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_LUMINANCE6_ALPHA2, GL_UNSIGNED_BYTE, GL_LUMINANCE_ALPHA ),			0x00, 0, 8, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_TBITS(6, 0, 0, 2), SL2_TSHIFTS(0, 0, 0, 6), SL2_CONV_L( CFormat::LumAlphaToRgba64F, CFormat::LumAlphaFromRgba64F, SL2_LBITS(6, 2), SL2_LSHIFTS(0, 6), 1, false, true, false ) },	// Weird result.
		{ SL2_ID( VK_FORMAT_UNDEFINED, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_LUMINANCE8_ALPHA8, GL_UNSIGNED_BYTE, GL_LUMINANCE_ALPHA ),			0x00, 0, 16, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_TBITS(8, 0, 0, 8), SL2_TSHIFTS(0, 0, 0, 8), SL2_CONV_L( CFormat::LumAlphaToRgba64F, CFormat::LumAlphaFromRgba64F, SL2_LBITS(8, 8), SL2_LSHIFTS(0, 8), 2, false, true, false ) },
		{ SL2_ID( VK_FORMAT_UNDEFINED, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_LUMINANCE8_ALPHA8_SNORM, GL_BYTE, GL_LUMINANCE_ALPHA ),				0x00, 0, 16, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_TBITS(8, 0, 0, 8), SL2_TSHIFTS(0, 0, 0, 8), SL2_CONV_L( CFormat::LumAlphaToRgba64F, CFormat::LumAlphaFromRgba64F, SL2_LBITS(8, 8), SL2_LSHIFTS(0, 8), 2, true, true, false ) },
		{ SL2_ID( VK_FORMAT_UNDEFINED, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_SLUMINANCE8_ALPHA8, GL_UNSIGNED_BYTE, GL_LUMINANCE_ALPHA ),			0x00, 0, 16, 1, 1, 1, 1, 1, true, false, false, false, nullptr, SL2_TBITS(8, 0, 0, 8), SL2_TSHIFTS(0, 0, 0, 8), SL2_CONV_L( CFormat::LumAlphaToRgba64F, CFormat::LumAlphaFromRgba64F, SL2_LBITS(8, 8), SL2_LSHIFTS(0, 8), 2, false, true, true ) },
		{ SL2_ID( VK_FORMAT_UNDEFINED, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_LUMINANCE_ALPHA8UI_EXT, GL_UNSIGNED_BYTE, GL_LUMINANCE_ALPHA_INTEGER ),
																																							0x00, 0, 16, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_TBITS(8, 0, 0, 8), SL2_TSHIFTS(0, 0, 0, 8), SL2_CONV_L( CFormat::LumAlphaToRgba64F, CFormat::LumAlphaFromRgba64F, SL2_LBITS(8, 8), SL2_LSHIFTS(0, 8), 2, false, false, false ) },
		{ SL2_ID( VK_FORMAT_UNDEFINED, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_LUMINANCE_ALPHA8I_EXT, GL_BYTE, GL_LUMINANCE_ALPHA_INTEGER ),			0x00, 0, 16, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_TBITS(12, 0, 0, 4), SL2_TSHIFTS(0, 0, 0, 12), SL2_CONV_L( CFormat::LumAlphaToRgba64F, CFormat::LumAlphaFromRgba64F, SL2_LBITS(12, 4), SL2_LSHIFTS(0, 12), 2, true, false, false ) },
		{ SL2_ID( VK_FORMAT_UNDEFINED, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_LUMINANCE12_ALPHA4, GL_UNSIGNED_SHORT, GL_LUMINANCE_ALPHA ),			0x00, 0, 32, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_TBITS(12, 0, 0, 12), SL2_TSHIFTS(0, 0, 0, 16), SL2_CONV_L( CFormat::LumAlphaToRgba64F, CFormat::LumAlphaFromRgba64F, SL2_LBITS(12, 12), SL2_LSHIFTS(0, 16), 4, false, true, false ) },
		{ SL2_ID( VK_FORMAT_UNDEFINED, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_LUMINANCE12_ALPHA12, GL_UNSIGNED_SHORT, GL_LUMINANCE_ALPHA ),			0x00, 0, 32, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_TBITS(16, 0, 0, 16), SL2_TSHIFTS(0, 0, 0, 16), SL2_CONV_L( CFormat::LumAlphaToRgba64F, CFormat::LumAlphaFromRgba64F, SL2_LBITS(16, 16), SL2_LSHIFTS(0, 16), 4, false, true, false ) },
		{ SL2_ID( VK_FORMAT_UNDEFINED, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_LUMINANCE16_ALPHA16, GL_UNSIGNED_SHORT, GL_LUMINANCE_ALPHA ),			0x00, 0, 32, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_TBITS(16, 0, 0, 16), SL2_TSHIFTS(0, 0, 0, 16), SL2_CONV_L( CFormat::LumAlphaToRgba64F, CFormat::LumAlphaFromRgba64F, SL2_LBITS(16, 16), SL2_LSHIFTS(0, 16), 4, false, true, false ) },
		{ SL2_ID( VK_FORMAT_UNDEFINED, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_LUMINANCE16_ALPHA16_SNORM, GL_SHORT, GL_LUMINANCE_ALPHA ),			0x00, 0, 32, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_TBITS(16, 0, 0, 16), SL2_TSHIFTS(0, 0, 0, 16), SL2_CONV_L( CFormat::LumAlphaToRgba64F, CFormat::LumAlphaFromRgba64F, SL2_LBITS(16, 16), SL2_LSHIFTS(0, 16), 4, true, true, false ) },
		{ SL2_ID( VK_FORMAT_UNDEFINED, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_LUMINANCE_ALPHA16UI_EXT, GL_UNSIGNED_SHORT, GL_LUMINANCE_ALPHA_INTEGER ),
																																							0x00, 0, 32, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_TBITS(16, 0, 0, 16), SL2_TSHIFTS(0, 0, 0, 16), SL2_CONV_L( CFormat::LumAlphaToRgba64F, CFormat::LumAlphaFromRgba64F, SL2_LBITS(16, 16), SL2_LSHIFTS(0, 16), 4, false, false, false ) },
		{ SL2_ID( VK_FORMAT_UNDEFINED, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_LUMINANCE_ALPHA16I_EXT, GL_SHORT, GL_LUMINANCE_ALPHA_INTEGER ),		0x00, 0, 32, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_TBITS(16, 0, 0, 16), SL2_TSHIFTS(0, 0, 0, 16), SL2_CONV_L( CFormat::LumAlphaToRgba64F, CFormat::LumAlphaFromRgba64F, SL2_LBITS(16, 16), SL2_LSHIFTS(0, 16), 4, true, false, false ) },
		{ SL2_ID( VK_FORMAT_UNDEFINED, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_LUMINANCE_ALPHA16F_ARB, GL_HALF_FLOAT, GL_LUMINANCE_ALPHA ),			0x00, 0, 32, 1, 1, 1, 1, 1, false, false, false, true, nullptr, SL2_TBITS(16, 0, 0, 16), SL2_TSHIFTS(0, 0, 0, 16), SL2_CONV_F( CFormat::LumAlphaFToRgba64F, CFormat::LumAlphaFFromRgba64F, SL2_LBITS(16, 16), SL2_LSHIFTS(0, 16), 4 ) },
		{ SL2_ID( VK_FORMAT_UNDEFINED, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_LUMINANCE_ALPHA32UI_EXT, GL_UNSIGNED_INT, GL_LUMINANCE_ALPHA_INTEGER ),
																																							0x00, 0, 64, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_TBITS(32, 0, 0, 32), SL2_TSHIFTS(0, 0, 0, 32), SL2_CONV_L( CFormat::LumAlphaToRgba64F, CFormat::LumAlphaFromRgba64F, SL2_LBITS(32, 32), SL2_LSHIFTS(0, 32), 8, false, false, false ) },
		{ SL2_ID( VK_FORMAT_UNDEFINED, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_LUMINANCE_ALPHA32I_EXT, GL_INT, GL_LUMINANCE_ALPHA_INTEGER ),			0x00, 0, 64, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_TBITS(32, 0, 0, 32), SL2_TSHIFTS(0, 0, 0, 32), SL2_CONV_L( CFormat::LumAlphaToRgba64F, CFormat::LumAlphaFromRgba64F, SL2_LBITS(32, 32), SL2_LSHIFTS(0, 32), 8, true, false, false ) },
		{ SL2_ID( VK_FORMAT_UNDEFINED, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_LUMINANCE_ALPHA32F_ARB, GL_FLOAT, GL_LUMINANCE_ALPHA ),				0x00, 0, 64, 1, 1, 1, 1, 1, false, false, false, true, nullptr, SL2_TBITS(32, 0, 0, 32), SL2_TSHIFTS(0, 0, 0, 32), SL2_CONV_F( CFormat::LumAlphaFToRgba64F, CFormat::LumAlphaFFromRgba64F, SL2_LBITS(32, 32), SL2_LSHIFTS(0, 32), 8 ) },
		
		{ SL2_ID( VK_FORMAT_UNDEFINED, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_INTENSITY4, GL_UNSIGNED_BYTE, GL_LUMINANCE ),							0x00, 0, 8, 1, 1, 1, 1, 1, false, false, false, false, nullptr, 0, 0, 0, SL2_CONV_IN( CFormat::IntensityToRgba64F, CFormat::IntensityFromRgba64F, SL2_IBITS(4), 1, false, true, false ) },
		{ SL2_ID( VK_FORMAT_UNDEFINED, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_INTENSITY8, GL_UNSIGNED_BYTE, GL_LUMINANCE ),							0x00, 0, 8, 1, 1, 1, 1, 1, false, false, false, false, nullptr, 0, 0, 0, SL2_CONV_IN( CFormat::IntensityToRgba64F, CFormat::IntensityFromRgba64F, SL2_IBITS(8), 1, false, true, false ) },
		{ SL2_ID( VK_FORMAT_UNDEFINED, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_INTENSITY8_SNORM, GL_BYTE, GL_LUMINANCE ),							0x00, 0, 8, 1, 1, 1, 1, 1, false, false, false, false, nullptr, 0, 0, 0, SL2_CONV_IN( CFormat::IntensityToRgba64F, CFormat::IntensityFromRgba64F, SL2_IBITS(8), 1, true, true, false ) },
		{ SL2_ID( VK_FORMAT_UNDEFINED, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_INTENSITY8UI_EXT, GL_UNSIGNED_BYTE, GL_LUMINANCE_INTEGER ),			0x00, 0, 8, 1, 1, 1, 1, 1, false, false, false, false, nullptr, 0, 0, 0, SL2_CONV_IN( CFormat::IntensityToRgba64F, CFormat::IntensityFromRgba64F, SL2_IBITS(8), 1, false, false, false ) },
		{ SL2_ID( VK_FORMAT_UNDEFINED, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_INTENSITY8I_EXT, GL_BYTE, GL_LUMINANCE_INTEGER ),						0x00, 0, 8, 1, 1, 1, 1, 1, false, false, false, false, nullptr, 0, 0, 0, SL2_CONV_IN( CFormat::IntensityToRgba64F, CFormat::IntensityFromRgba64F, SL2_IBITS(8), 1, true, false, false ) },
		{ SL2_ID( VK_FORMAT_UNDEFINED, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_INTENSITY12, GL_UNSIGNED_SHORT, GL_LUMINANCE ),						0x00, 0, 16, 1, 1, 1, 1, 1, false, false, false, false, nullptr, 0, 0, 0, SL2_CONV_IN( CFormat::IntensityToRgba64F, CFormat::IntensityFromRgba64F, SL2_IBITS(12), 2, false, true, false ) },
		{ SL2_ID( VK_FORMAT_UNDEFINED, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_INTENSITY16, GL_UNSIGNED_SHORT, GL_LUMINANCE ),						0x00, 0, 16, 1, 1, 1, 1, 1, false, false, false, false, nullptr, 0, 0, 0, SL2_CONV_IN( CFormat::IntensityToRgba64F, CFormat::IntensityFromRgba64F, SL2_IBITS(16), 2, false, true, false ) },
		{ SL2_ID( VK_FORMAT_UNDEFINED, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_INTENSITY16_SNORM, GL_SHORT, GL_LUMINANCE ),							0x00, 0, 16, 1, 1, 1, 1, 1, false, false, false, false, nullptr, 0, 0, 0, SL2_CONV_IN( CFormat::IntensityToRgba64F, CFormat::IntensityFromRgba64F, SL2_IBITS(16), 2, true, true, false ) },
		{ SL2_ID( VK_FORMAT_UNDEFINED, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_INTENSITY16UI_EXT, GL_UNSIGNED_SHORT, GL_LUMINANCE_INTEGER ),			0x00, 0, 16, 1, 1, 1, 1, 1, false, false, false, false, nullptr, 0, 0, 0, SL2_CONV_IN( CFormat::IntensityToRgba64F, CFormat::IntensityFromRgba64F, SL2_IBITS(16), 2, false, false, false ) },
		{ SL2_ID( VK_FORMAT_UNDEFINED, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_INTENSITY16I_EXT, GL_SHORT, GL_LUMINANCE_INTEGER ),					0x00, 0, 16, 1, 1, 1, 1, 1, false, false, false, false, nullptr, 0, 0, 0, SL2_CONV_IN( CFormat::IntensityToRgba64F, CFormat::IntensityFromRgba64F, SL2_IBITS(16), 2, true, false, false ) },
		{ SL2_ID( VK_FORMAT_UNDEFINED, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_INTENSITY16F_ARB, GL_HALF_FLOAT, GL_LUMINANCE ),						0x00, 0, 16, 1, 1, 1, 1, 1, false, false, false, false, nullptr, 0, 0, 0, SL2_CONV_IN( CFormat::IntensityToRgba64F, CFormat::IntensityFromRgba64F, SL2_IBITS(16), 2, false, true, true ) },
		{ SL2_ID( VK_FORMAT_UNDEFINED, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_INTENSITY32UI_EXT, GL_UNSIGNED_INT, GL_LUMINANCE_INTEGER ),			0x00, 0, 32, 1, 1, 1, 1, 1, false, false, false, false, nullptr, 0, 0, 0, SL2_CONV_IN( CFormat::IntensityToRgba64F, CFormat::IntensityFromRgba64F, SL2_IBITS(32), 4, false, false, false ) },
		{ SL2_ID( VK_FORMAT_UNDEFINED, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_INTENSITY32I_EXT, GL_INT, GL_LUMINANCE_INTEGER ),						0x00, 0, 32, 1, 1, 1, 1, 1, false, false, false, false, nullptr, 0, 0, 0, SL2_CONV_IN( CFormat::IntensityToRgba64F, CFormat::IntensityFromRgba64F, SL2_IBITS(32), 4, true, false, false ) },
		{ SL2_ID( VK_FORMAT_UNDEFINED, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_INTENSITY32F_ARB, GL_FLOAT, GL_LUMINANCE ),							0x00, 0, 32, 1, 1, 1, 1, 1, false, false, false, false, nullptr, 0, 0, 0, SL2_CONV_IN( CFormat::IntensityToRgba64F, CFormat::IntensityFromRgba64F, SL2_IBITS(32), 4, false, true, true ) },
		
		{ SL2_ID( VK_FORMAT_UNDEFINED, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_COMPRESSED_RED, GL_UNSIGNED_BYTE, GL_RED ),							0x00, 0, 8, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_GEN_INT( SL2_TBITS(8, 0, 0, 0), SL2_TSHIFTS(0, 0, 0, 0), 1, false, true, false ) },
		{ SL2_ID( VK_FORMAT_UNDEFINED, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_COMPRESSED_ALPHA, GL_UNSIGNED_BYTE, GL_ALPHA ),						0x00, 0, 8, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_GEN_INT( SL2_TBITS(0, 0, 0, 8), SL2_TSHIFTS(0, 0, 0, 0), 1, false, true, false ) },
		{ SL2_ID( VK_FORMAT_UNDEFINED, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_COMPRESSED_LUMINANCE, GL_UNSIGNED_BYTE, GL_LUMINANCE ),				0x00, 0, 8, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_TBITS(8, 0, 0, 0), SL2_TSHIFTS(0, 0, 0, 0), SL2_CONV_L( CFormat::LumAlphaToRgba64F, CFormat::LumAlphaFromRgba64F, SL2_LBITS(8, 0), SL2_LSHIFTS(0, 0), 1, false, true, false ) },
		{ SL2_ID( VK_FORMAT_UNDEFINED, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_COMPRESSED_SLUMINANCE, GL_UNSIGNED_BYTE, GL_LUMINANCE ),				0x00, 0, 8, 1, 1, 1, 1, 1, true, false, false, false, nullptr, SL2_TBITS(8, 0, 0, 0), SL2_TSHIFTS(0, 0, 0, 0), SL2_CONV_L( CFormat::LumAlphaToRgba64F, CFormat::LumAlphaFromRgba64F, SL2_LBITS(8, 0), SL2_LSHIFTS(0, 0), 1, false, true, true ) },
		{ SL2_ID( VK_FORMAT_UNDEFINED, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_COMPRESSED_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, GL_LUMINANCE_ALPHA ),	0x00, 0, 16, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_TBITS(8, 0, 0, 8), SL2_TSHIFTS(0, 0, 0, 8), SL2_CONV_L( CFormat::LumAlphaToRgba64F, CFormat::LumAlphaFromRgba64F, SL2_LBITS(8, 8), SL2_LSHIFTS(0, 8), 2, false, true, false ) },
		{ SL2_ID( VK_FORMAT_UNDEFINED, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_COMPRESSED_SLUMINANCE_ALPHA, GL_UNSIGNED_BYTE, GL_LUMINANCE_ALPHA ),	0x00, 0, 16, 1, 1, 1, 1, 1, true, false, false, false, nullptr, SL2_TBITS(8, 0, 0, 8), SL2_TSHIFTS(0, 0, 0, 8), SL2_CONV_L( CFormat::LumAlphaToRgba64F, CFormat::LumAlphaFromRgba64F, SL2_LBITS(8, 8), SL2_LSHIFTS(0, 8), 2, false, true, true ) },
		{ SL2_ID( VK_FORMAT_UNDEFINED, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_COMPRESSED_INTENSITY, GL_UNSIGNED_BYTE, GL_LUMINANCE ),				0x00, 0, 8, 1, 1, 1, 1, 1, false, false, false, false, nullptr, 0, 0, 0, SL2_CONV_IN( CFormat::IntensityToRgba64F, CFormat::IntensityFromRgba64F, SL2_IBITS(8), 1, false, true, false ) },
		{ SL2_ID( VK_FORMAT_UNDEFINED, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_COMPRESSED_RG, GL_UNSIGNED_BYTE, GL_RG ),								0x00, 0, 16, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_GEN_INT( SL2_TBITS(8, 8, 0, 0), SL2_TSHIFTS(0, 8, 0, 0), 2, false, true, false ) },
		{ SL2_ID( VK_FORMAT_UNDEFINED, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_COMPRESSED_RGB, GL_UNSIGNED_BYTE, GL_RGB ),							0x00, 0, 24, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_GEN_INT( SL2_TBITS(8, 8, 8, 0), SL2_TSHIFTS(0, 8, 16, 0), 3, false, true, false ) },
		{ SL2_ID( VK_FORMAT_UNDEFINED, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_COMPRESSED_RGBA, GL_UNSIGNED_BYTE, GL_RGBA ),							0x00, 0, 32, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_GEN_INT( SL2_TBITS(8, 8, 8, 8), SL2_TSHIFTS(0, 8, 16, 24), 4, false, true, false ) },
		{ SL2_ID( VK_FORMAT_UNDEFINED, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_COMPRESSED_SRGB, GL_UNSIGNED_BYTE, GL_RGB ),							0x00, 0, 24, 1, 1, 1, 1, 1, true, false, false, false, nullptr, SL2_GEN_INT( SL2_TBITS(8, 8, 8, 0), SL2_TSHIFTS(0, 8, 16, 0), 3, false, true, true ) },
		{ SL2_ID( VK_FORMAT_UNDEFINED, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_COMPRESSED_SRGB_ALPHA, GL_UNSIGNED_BYTE, GL_RGBA ),					0x00, 0, 32, 1, 1, 1, 1, 1, true, false, false, false, nullptr, SL2_GEN_INT( SL2_TBITS(8, 8, 8, 8), SL2_TSHIFTS(0, 8, 16, 24), 4, false, true, true ) },
		
		//{ SL2_KIF_GL_COMPRESSED_RGB_FXT1_3DFX, SL2_KT_0, SL2_KBIF_0, 0x00, 0, 0, 1, 1, 1, 1, 1, false, false, false, false, nullptr },
		//{ SL2_KIF_GL_COMPRESSED_RGBA_FXT1_3DFX, SL2_KT_0, SL2_KBIF_0, 0x00, 0, 0, 1, 1, 1, 1, 1, false, false, false, false, nullptr },
		
		{ SL2_ID( VK_FORMAT_BC1_RGB_UNORM_BLOCK, DXGI_FORMAT_BC1_UNORM, MTLPixelFormatInvalid, GL_COMPRESSED_RGB_S3TC_DXT1_EXT, GL_UNSIGNED_BYTE, GL_RGB ),	0x02 | SL2_MAKE_COMP_FLAG( SL2_CS_BC )
																																								, 0, 64, 4, 4, 1, 1, 1, false, true, false, false, CFormat::GetCompressedSizeBc, SL2_TBITS(5, 6, 5, 0), SL2_TSHIFTS(0, 0, 0, 0), CFormat::Dxt1ToRgba64F<false>, CFormat::Dxt1FromRgba64F<0, false> },
		//{ SL2_KIF_GL_COMPRESSED_RGBA_S3TC_DXT1_EXT, SL2_KT_GL_UNSIGNED_BYTE, SL2_KBIF_GL_RGBA, 0x02 | SL2_MAKE_COMP_FLAG( SL2_CS_BC ), 0, 64, 4, 4, 1, 1, 1, false, true, false, false, nullptr, CFormat::Dxt1ToRgba64F<false>, CFormat::Dxt1FromRgba64F<128, false> },
		//{ SL2_KIF_GL_COMPRESSED_RGBA_S3TC_DXT3_EXT, SL2_KT_GL_UNSIGNED_BYTE, SL2_KBIF_GL_RGBA, 0x02 | SL2_MAKE_COMP_FLAG( SL2_CS_BC ), 0, 128, 4, 4, 1, 1, 1, false, true, false, false, nullptr, CFormat::Dxt3ToRgba64F<false>, CFormat::Dxt3FromRgba32F<false> },
		//{ SL2_KIF_GL_COMPRESSED_RGBA_S3TC_DXT5_EXT, SL2_KT_GL_UNSIGNED_BYTE, SL2_KBIF_GL_RGBA, 0x02 | SL2_MAKE_COMP_FLAG( SL2_CS_BC ), 0, 128, 4, 4, 1, 1, 1, false, true, false, false, nullptr, CFormat::Dxt5ToRgba32F<false>, CFormat::Dxt5FromRgba32F<false> },
		//{ SL2_KIF_GL_COMPRESSED_SRGB_S3TC_DXT1_EXT, SL2_KT_GL_UNSIGNED_BYTE, SL2_KBIF_GL_RGB, 0x02 | SL2_MAKE_COMP_FLAG( SL2_CS_BC ), 0, 64, 4, 4, 1, 1, 1, true, true, false, false, nullptr, CFormat::Dxt1ToRgba64F<true>, CFormat::Dxt1FromRgba64F<0, true> },
		//{ SL2_KIF_GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT, SL2_KT_GL_UNSIGNED_BYTE, SL2_KBIF_GL_RGBA, 0x02 | SL2_MAKE_COMP_FLAG( SL2_CS_BC ), 0, 64, 4, 4, 1, 1, 1, true, true, false, false, nullptr, CFormat::Dxt1ToRgba64F<true>, CFormat::Dxt1FromRgba64F<128, true> },
		//{ SL2_KIF_GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT, SL2_KT_GL_UNSIGNED_BYTE, SL2_KBIF_GL_RGBA, 0x02 | SL2_MAKE_COMP_FLAG( SL2_CS_BC ), 0, 128, 4, 4, 1, 1, 1, true, true, false, false, nullptr, CFormat::Dxt3ToRgba64F<true>, CFormat::Dxt3FromRgba32F<true> },
		//{ SL2_KIF_GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT, SL2_KT_GL_UNSIGNED_BYTE, SL2_KBIF_GL_RGBA, 0x02 | SL2_MAKE_COMP_FLAG( SL2_CS_BC ), 0, 128, 4, 4, 1, 1, 1, true, true, false, false, nullptr, CFormat::Dxt5ToRgba32F<true>, CFormat::Dxt5FromRgba32F<true> },
		// 
		//{ SL2_KIF_GL_COMPRESSED_LUMINANCE_LATC1_EXT, SL2_KT_GL_UNSIGNED_BYTE, SL2_KBIF_GL_RED, 0x02, 0, 64, 4, 4, 1, 1, 1, false, true, false, nullptr, CFormat::Bc4uToRgba32F<false, true>, CFormat::Bc4uFromRgba32F<false, true> },
		//{ SL2_KIF_GL_COMPRESSED_LUMINANCE_ALPHA_LATC2_EXT, SL2_KT_GL_UNSIGNED_BYTE, SL2_KBIF_GL_RG, 0x02, 0, 128, 4, 4, 1, 1, 1, false, true, false, nullptr, CFormat::Bc5uToRgba32F<false, true>, CFormat::Bc5uFromRgba32F<false, true> },
		//{ SL2_KIF_GL_COMPRESSED_SIGNED_LUMINANCE_LATC1_EXT, SL2_KT_GL_UNSIGNED_BYTE, SL2_KBIF_GL_RED, 0x02, 0, 64, 4, 4, 1, 1, 1, false, true, false, nullptr, CFormat::Bc4sToRgba32F<false, true>, CFormat::Bc4sFromRgba32F<false, true> },
		//{ SL2_KIF_GL_COMPRESSED_SIGNED_LUMINANCE_ALPHA_LATC2_EXT, SL2_KT_GL_UNSIGNED_BYTE, SL2_KBIF_GL_RG, 0x02, 0, 128, 4, 4, 1, 1, 1, false, true, false, nullptr, CFormat::Bc5sToRgba32F<false, true>, CFormat::Bc5sFromRgba32F<false, true> },
		//{ SL2_KIF_GL_COMPRESSED_RED_RGTC1, SL2_KT_GL_UNSIGNED_BYTE, SL2_KBIF_GL_RED, 0x02, 0, 64, 4, 4, 1, 1, 1, false, true, false, nullptr, CFormat::Bc4uToRgba32F<false, false>, CFormat::Bc4uFromRgba32F<false, false> },
		//{ SL2_KIF_GL_COMPRESSED_RG_RGTC2, SL2_KT_GL_UNSIGNED_BYTE, SL2_KBIF_GL_RG, 0x02, 0, 128, 4, 4, 1, 1, 1, false, true, false, nullptr, CFormat::Bc5uToRgba32F<false, false>, CFormat::Bc5uFromRgba32F<false, false> },
		//{ SL2_KIF_GL_COMPRESSED_SIGNED_RED_RGTC1, SL2_KT_GL_UNSIGNED_BYTE, SL2_KBIF_GL_RED, 0x02, 0, 64, 4, 4, 1, 1, 1, false, true, false, nullptr, CFormat::Bc4sToRgba32F<false, false>, CFormat::Bc4sFromRgba32F<false, false> },
		//{ SL2_KIF_GL_COMPRESSED_SIGNED_RG_RGTC2, SL2_KT_GL_UNSIGNED_BYTE, SL2_KBIF_GL_RG, 0x02, 0, 128, 4, 4, 1, 1, 1, false, true, false, nullptr, CFormat::Bc5sToRgba32F<false, false>, CFormat::Bc5sFromRgba32F<false, false> },
		//{ SL2_KIF_GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT, SL2_KT_GL_FLOAT, SL2_KBIF_GL_RGB, 0x02, 0, 128, 4, 4, 1, 1, 1, false, true, false, nullptr, /**/nullptr, nullptr /*CFormat::Bc6hToRgba32F<false>, CFormat::Bc6hFromRgba32F<false>*/ },
		//{ SL2_KIF_GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT, SL2_KT_GL_FLOAT, SL2_KBIF_GL_RGB, 0x02, 0, 128, 4, 4, 1, 1, 1, false, true, false, nullptr, CFormat::Bc6hToRgba32F<false>, CFormat::Bc6hFromRgba32F<false> },
		//{ SL2_KIF_GL_COMPRESSED_RGBA_BPTC_UNORM, SL2_KT_GL_UNSIGNED_BYTE, SL2_KBIF_GL_RGBA, 0x02, 0, 128, 4, 4, 1, 1, 1, false, true, false, nullptr, CFormat::Bc7uToRgba32F<false>, CFormat::Bc7uFromRgba32F<false> },
		//{ SL2_KIF_GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM, SL2_KT_GL_UNSIGNED_BYTE, SL2_KBIF_GL_RGBA, 0x02, 0, 128, 4, 4, 1, 1, 1, true, true, false, nullptr, CFormat::Bc7uToRgba32F<true>, CFormat::Bc7uFromRgba32F<true> },
		//{ SL2_KIF_GL_ETC1_RGB8_OES, SL2_KT_GL_UNSIGNED_BYTE, SL2_KBIF_GL_RGB, 0x02, 0, 64, 4, 4, 1, 1, 1, false, true, false, nullptr, CEtc::Etc1ToRgba32F, CEtc::Etc1FromRgba32F },
		//{ SL2_KIF_GL_COMPRESSED_RGB8_ETC2, SL2_KT_GL_UNSIGNED_BYTE, SL2_KBIF_GL_RGB, 0x02, 0, 64, 4, 4, 1, 1, 1, false, true, false, nullptr, CEtc::Etc2ToRgba32F<false>, CEtc::Etc2FromRgba32F<false> },
		//{ SL2_KIF_GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2, SL2_KT_GL_UNSIGNED_BYTE, SL2_KBIF_GL_RGBA, 0x02, 0, 64, 4, 4, 1, 1, 1, false, true, false, nullptr, CEtc::Etc2A1ToRgba32F<false>, CEtc::Etc2A1FromRgba32F<false> },
		//{ SL2_KIF_GL_COMPRESSED_RGBA8_ETC2_EAC, SL2_KT_GL_UNSIGNED_BYTE, SL2_KBIF_GL_RGBA, 0x02, 0, 128, 4, 4, 1, 1, 1, false, true, false, nullptr, CEtc::Etc2EacToRgba32F<false>, CEtc::Etc2EacFromRgba32F<false> },
		//{ SL2_KIF_GL_COMPRESSED_SRGB8_ETC2, SL2_KT_GL_UNSIGNED_BYTE, SL2_KBIF_GL_RGB, 0x02, 0, 64, 4, 4, 1, 1, 1, true, true, false, nullptr, CEtc::Etc2ToRgba32F<true>, CEtc::Etc2FromRgba32F<true> },
		//{ SL2_KIF_GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2, SL2_KT_GL_UNSIGNED_BYTE, SL2_KBIF_GL_RGBA, 0x02, 0, 64, 4, 4, 1, 1, 1, true, true, false, nullptr, CEtc::Etc2A1ToRgba32F<true>, CEtc::Etc2A1FromRgba32F<true> },
		//{ SL2_KIF_GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC, SL2_KT_GL_UNSIGNED_BYTE, SL2_KBIF_GL_RGBA, 0x02, 0, 128, 4, 4, 1, 1, 1, true, true, false, nullptr, CEtc::Etc2EacToRgba32F<true>, CEtc::Etc2EacFromRgba32F<true> },
		//{ SL2_KIF_GL_COMPRESSED_R11_EAC, SL2_KT_GL_UNSIGNED_BYTE, SL2_KBIF_GL_RED, 0x02, 0, 64, 4, 4, 1, 1, 1, false, true, false, nullptr, CEtc::R11EacToRgba32F<false, false>, CEtc::R11EacFromRgba32F<false, false> },
		//{ SL2_KIF_GL_COMPRESSED_RG11_EAC, SL2_KT_GL_UNSIGNED_BYTE, SL2_KBIF_GL_RG, 0x02, 0, 128, 4, 4, 1, 1, 1, false, true, false, nullptr, CEtc::RG11EacToRgba32F<false, false>, CEtc::RG11EacFromRgba32F<false, false> },
		//{ SL2_KIF_GL_COMPRESSED_SIGNED_R11_EAC, SL2_KT_GL_UNSIGNED_BYTE, SL2_KBIF_GL_RED, 0x02, 0, 64, 4, 4, 1, 1, 1, false, true, false, nullptr, CEtc::R11EacToRgba32F<false, true>, CEtc::R11EacFromRgba32F<false, true> },
		//{ SL2_KIF_GL_COMPRESSED_SIGNED_RG11_EAC, SL2_KT_GL_UNSIGNED_BYTE, SL2_KBIF_GL_RG, 0x02, 0, 128, 4, 4, 1, 1, 1, false, true, false, nullptr, CEtc::RG11EacToRgba32F<false, true>, CEtc::RG11EacFromRgba32F<false, true> },
		//{ SL2_KIF_GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG, SL2_KT_GL_UNSIGNED_BYTE, SL2_KBIF_GL_RGB, 0x02, 0, 64, 8, 4, 1, 2, 2, false, true, false, &(GetCompressedSizePvrtc<8, 4>), CPvrtc::PvrtcToRgba32F<SL2_PF_PCRTC_2_RGB_V1, false>, CPvrtc::PvrtcFromRgba32F<SL2_PF_PCRTC_2_RGB_V1, false> },
		//{ SL2_KIF_GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG, SL2_KT_GL_UNSIGNED_BYTE, SL2_KBIF_GL_RGB, 0x02, 0, 64, 4, 4, 1, 2, 2, false, true, false, &(GetCompressedSizePvrtc<4, 4>), CPvrtc::PvrtcToRgba32F<SL2_PF_PCRTC_4_RGB_V1, false>, CPvrtc::PvrtcFromRgba32F<SL2_PF_PCRTC_4_RGB_V1, false> },
		//{ SL2_KIF_GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG, SL2_KT_GL_UNSIGNED_BYTE, SL2_KBIF_GL_RGBA, 0x02, 0, 64, 8, 4, 1, 2, 2, false, true, false, &(GetCompressedSizePvrtc<8, 4>), CPvrtc::PvrtcToRgba32F<SL2_PF_PCRTC_2_RGBA_V1, false>, CPvrtc::PvrtcFromRgba32F<SL2_PF_PCRTC_2_RGBA_V1, false> },
		//{ SL2_KIF_GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG, SL2_KT_GL_UNSIGNED_BYTE, SL2_KBIF_GL_RGBA, 0x02, 0, 64, 4, 4, 1, 2, 2, false, true, false, &(GetCompressedSizePvrtc<4, 4>), CPvrtc::PvrtcToRgba32F<SL2_PF_PCRTC_4_RGBA_V1, false>, CPvrtc::PvrtcFromRgba32F<SL2_PF_PCRTC_4_RGBA_V1, false> },
		//{ SL2_KIF_GL_COMPRESSED_RGBA_PVRTC_2BPPV2_IMG, SL2_KT_GL_UNSIGNED_BYTE, SL2_KBIF_GL_RGBA, 0x02, 0, 64, 8, 4, 1, 1, 1, false, true, false, &(GetCompressedSizePvrtc<8, 4>), CPvrtc::PvrtcToRgba32F<SL2_PF_PCRTC_2_RGBA_V2, false>, CPvrtc::PvrtcFromRgba32F<SL2_PF_PCRTC_2_RGBA_V2, false> },
		//{ SL2_KIF_GL_COMPRESSED_RGBA_PVRTC_4BPPV2_IMG, SL2_KT_GL_UNSIGNED_BYTE, SL2_KBIF_GL_RGBA, 0x02, 0, 64, 4, 4, 1, 1, 1, false, true, false, &(GetCompressedSizePvrtc<4, 4>), CPvrtc::PvrtcToRgba32F<SL2_PF_PCRTC_4_RGBA_V2, false>, CPvrtc::PvrtcFromRgba32F<SL2_PF_PCRTC_4_RGBA_V2, false> },
		//{ SL2_KIF_GL_COMPRESSED_SRGB_PVRTC_2BPPV1_EXT, SL2_KT_GL_UNSIGNED_BYTE, SL2_KBIF_GL_RGB, 0x02, 0, 64, 8, 4, 1, 2, 2, true, true, false, &(GetCompressedSizePvrtc<8, 4>), CPvrtc::PvrtcToRgba32F<SL2_PF_PCRTC_2_RGB_V1, true>, CPvrtc::PvrtcFromRgba32F<SL2_PF_PCRTC_2_RGB_V1, true> },
		//{ SL2_KIF_GL_COMPRESSED_SRGB_PVRTC_4BPPV1_EXT, SL2_KT_GL_UNSIGNED_BYTE, SL2_KBIF_GL_RGB, 0x02, 0, 64, 4, 4, 1, 2, 2, true, true, false, &(GetCompressedSizePvrtc<4, 4>), CPvrtc::PvrtcToRgba32F<SL2_PF_PCRTC_4_RGB_V1, true>, CPvrtc::PvrtcFromRgba32F<SL2_PF_PCRTC_4_RGB_V1, true> },
		//{ SL2_KIF_GL_COMPRESSED_SRGB_ALPHA_PVRTC_2BPPV1_EXT, SL2_KT_GL_UNSIGNED_BYTE, SL2_KBIF_GL_RGBA, 0x02, 0, 64, 8, 4, 1, 2, 2, true, true, false, &(GetCompressedSizePvrtc<8, 4>), CPvrtc::PvrtcToRgba32F<SL2_PF_PCRTC_2_RGBA_V1, true>, CPvrtc::PvrtcFromRgba32F<SL2_PF_PCRTC_2_RGBA_V1, true> },
		//{ SL2_KIF_GL_COMPRESSED_SRGB_ALPHA_PVRTC_4BPPV1_EXT, SL2_KT_GL_UNSIGNED_BYTE, SL2_KBIF_GL_RGBA, 0x02, 0, 64, 4, 4, 1, 2, 2, true, true, false, &(GetCompressedSizePvrtc<4, 4>), CPvrtc::PvrtcToRgba32F<SL2_PF_PCRTC_4_RGBA_V1, true>, CPvrtc::PvrtcFromRgba32F<SL2_PF_PCRTC_4_RGBA_V1, true> },
		//{ SL2_KIF_GL_COMPRESSED_SRGB_ALPHA_PVRTC_2BPPV2_IMG, SL2_KT_GL_UNSIGNED_BYTE, SL2_KBIF_GL_RGBA, 0x02, 0, 64, 8, 4, 1, 1, 1, true, true, false, &(GetCompressedSizePvrtc<8, 4>), CPvrtc::PvrtcToRgba32F<SL2_PF_PCRTC_2_RGBA_V2, true>, CPvrtc::PvrtcFromRgba32F<SL2_PF_PCRTC_2_RGBA_V2, true> },
		//{ SL2_KIF_GL_COMPRESSED_SRGB_ALPHA_PVRTC_4BPPV2_IMG, SL2_KT_GL_UNSIGNED_BYTE, SL2_KBIF_GL_RGBA, 0x02, 0, 64, 4, 4, 1, 1, 1, true, true, false, &(GetCompressedSizePvrtc<4, 4>), CPvrtc::PvrtcToRgba32F<SL2_PF_PCRTC_4_RGBA_V2, true>, CPvrtc::PvrtcFromRgba32F<SL2_PF_PCRTC_4_RGBA_V2, true> },
		//{ SL2_KIF_GL_COMPRESSED_RGBA_ASTC_4x4_KHR, SL2_KT_GL_UNSIGNED_BYTE, SL2_KBIF_GL_RGBA, 0x02, 0, 128, 4, 4, 1, 1, 1, false, true, false, &(CFormat::GetCompressedSizeAstc<4, 4>), CAstc::AstcToRgba32F<SL2_PF_ASTC_4_4, 4, 4, false>, CAstc::AstcFromRgba32F<SL2_PF_ASTC_4_4, 4, 4, false> },
		//{ SL2_KIF_GL_COMPRESSED_RGBA_ASTC_5x4_KHR, SL2_KT_GL_UNSIGNED_BYTE, SL2_KBIF_GL_RGBA, 0x02, 0, 128, 5, 4, 1, 1, 1, false, true, false, &(CFormat::GetCompressedSizeAstc<5, 4>), CAstc::AstcToRgba32F<SL2_PF_ASTC_5_4, 5, 4, false>, CAstc::AstcFromRgba32F<SL2_PF_ASTC_5_4, 5, 4, false> },
		//{ SL2_KIF_GL_COMPRESSED_RGBA_ASTC_5x5_KHR, SL2_KT_GL_UNSIGNED_BYTE, SL2_KBIF_GL_RGBA, 0x02, 0, 128, 5, 5, 1, 1, 1, false, true, false, &(CFormat::GetCompressedSizeAstc<5, 5>), CAstc::AstcToRgba32F<SL2_PF_ASTC_5_5, 5, 5, false>, CAstc::AstcFromRgba32F<SL2_PF_ASTC_5_5, 5, 5, false> },
		//{ SL2_KIF_GL_COMPRESSED_RGBA_ASTC_6x5_KHR, SL2_KT_GL_UNSIGNED_BYTE, SL2_KBIF_GL_RGBA, 0x02, 0, 128, 6, 5, 1, 1, 1, false, true, false, &(CFormat::GetCompressedSizeAstc<6, 5>), CAstc::AstcToRgba32F<SL2_PF_ASTC_6_5, 6, 5, false>, CAstc::AstcFromRgba32F<SL2_PF_ASTC_6_5, 6, 5, false> },
		//{ SL2_KIF_GL_COMPRESSED_RGBA_ASTC_6x6_KHR, SL2_KT_GL_UNSIGNED_BYTE, SL2_KBIF_GL_RGBA, 0x02, 0, 128, 6, 6, 1, 1, 1, false, true, false, &(CFormat::GetCompressedSizeAstc<6, 6>), CAstc::AstcToRgba32F<SL2_PF_ASTC_6_6, 6, 6, false>, CAstc::AstcFromRgba32F<SL2_PF_ASTC_6_6, 6, 6, false> },
		//{ SL2_KIF_GL_COMPRESSED_RGBA_ASTC_8x5_KHR, SL2_KT_GL_UNSIGNED_BYTE, SL2_KBIF_GL_RGBA, 0x02, 0, 128, 8, 5, 1, 1, 1, false, true, false, &(CFormat::GetCompressedSizeAstc<8, 5>), CAstc::AstcToRgba32F<SL2_PF_ASTC_8_5, 8, 5, false>, CAstc::AstcFromRgba32F<SL2_PF_ASTC_8_5, 8, 5, false> },
		//{ SL2_KIF_GL_COMPRESSED_RGBA_ASTC_8x6_KHR, SL2_KT_GL_UNSIGNED_BYTE, SL2_KBIF_GL_RGBA, 0x02, 0, 128, 8, 6, 1, 1, 1, false, true, false, &(CFormat::GetCompressedSizeAstc<8, 6>), CAstc::AstcToRgba32F<SL2_PF_ASTC_8_6, 8, 6, false>, CAstc::AstcFromRgba32F<SL2_PF_ASTC_8_6, 8, 6, false> },
		//{ SL2_KIF_GL_COMPRESSED_RGBA_ASTC_8x8_KHR, SL2_KT_GL_UNSIGNED_BYTE, SL2_KBIF_GL_RGBA, 0x02, 0, 128, 8, 8, 1, 1, 1, false, true, false, &(CFormat::GetCompressedSizeAstc<8, 8>), CAstc::AstcToRgba32F<SL2_PF_ASTC_8_8, 8, 8, false>, CAstc::AstcFromRgba32F<SL2_PF_ASTC_8_8, 8, 8, false> },
		//{ SL2_KIF_GL_COMPRESSED_RGBA_ASTC_10x5_KHR, SL2_KT_GL_UNSIGNED_BYTE, SL2_KBIF_GL_RGBA, 0x02, 0, 128, 10, 5, 1, 1, 1, false, true, false, &(CFormat::GetCompressedSizeAstc<10, 5>), CAstc::AstcToRgba32F<SL2_PF_ASTC_10_5, 10, 5, false>, CAstc::Astc2FromRgba32F<SL2_PF_ASTC_10_5, 10, 5, false> },
		//{ SL2_KIF_GL_COMPRESSED_RGBA_ASTC_10x6_KHR, SL2_KT_GL_UNSIGNED_BYTE, SL2_KBIF_GL_RGBA, 0x02, 0, 128, 10, 6, 1, 1, 1, false, true, false, &(CFormat::GetCompressedSizeAstc<10, 6>), CAstc::AstcToRgba32F<SL2_PF_ASTC_10_6, 10, 6, false>, CAstc::Astc2FromRgba32F<SL2_PF_ASTC_10_6, 10, 6, false> },
		//{ SL2_KIF_GL_COMPRESSED_RGBA_ASTC_10x8_KHR, SL2_KT_GL_UNSIGNED_BYTE, SL2_KBIF_GL_RGBA, 0x02, 0, 128, 10, 8, 1, 1, 1, false, true, false, &(CFormat::GetCompressedSizeAstc<10, 8>), CAstc::AstcToRgba32F<SL2_PF_ASTC_10_8, 10, 8, false>, CAstc::Astc2FromRgba32F<SL2_PF_ASTC_10_8, 10, 8, false> },
		//{ SL2_KIF_GL_COMPRESSED_RGBA_ASTC_10x10_KHR, SL2_KT_GL_UNSIGNED_BYTE, SL2_KBIF_GL_RGBA, 0x02, 0, 128, 10, 10, 1, 1, 1, false, true, false, &(CFormat::GetCompressedSizeAstc<10, 10>), CAstc::AstcToRgba32F<SL2_PF_ASTC_10_10, 10, 10, false>, CAstc::Astc2FromRgba32F<SL2_PF_ASTC_10_10, 10, 10, false> },
		//{ SL2_KIF_GL_COMPRESSED_RGBA_ASTC_12x10_KHR, SL2_KT_GL_UNSIGNED_BYTE, SL2_KBIF_GL_RGBA, 0x02, 0, 128, 12, 10, 1, 1, 1, false, true, false, &(CFormat::GetCompressedSizeAstc<12, 10>), CAstc::AstcToRgba32F<SL2_PF_ASTC_12_10, 12, 10, false>, CAstc::Astc2FromRgba32F<SL2_PF_ASTC_12_10, 12, 10, false> },
		//{ SL2_KIF_GL_COMPRESSED_RGBA_ASTC_12x12_KHR, SL2_KT_GL_UNSIGNED_BYTE, SL2_KBIF_GL_RGBA, 0x02, 0, 128, 12, 12, 1, 1, 1, false, true, false, &(CFormat::GetCompressedSizeAstc<12, 12>), CAstc::AstcToRgba32F<SL2_PF_ASTC_12_12, 12, 12, false>, CAstc::Astc2FromRgba32F<SL2_PF_ASTC_12_12, 12, 12, false> },
		//{ SL2_KIF_GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR, SL2_KT_GL_UNSIGNED_BYTE, SL2_KBIF_GL_RGBA, 0x02, 0, 128, 4, 4, 1, 1, 1, true, true, false, &(CFormat::GetCompressedSizeAstc<4, 4>), CAstc::AstcToRgba32F<SL2_PF_ASTC_4_4, 4, 4, true>, CAstc::AstcFromRgba32F<SL2_PF_ASTC_4_4, 4, 4, true> },
		//{ SL2_KIF_GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR, SL2_KT_GL_UNSIGNED_BYTE, SL2_KBIF_GL_RGBA, 0x02, 0, 128, 5, 4, 1, 1, 1, true, true, false, &(CFormat::GetCompressedSizeAstc<5, 4>), CAstc::AstcToRgba32F<SL2_PF_ASTC_5_4, 5, 4, true>, CAstc::AstcFromRgba32F<SL2_PF_ASTC_5_4, 5, 4, true> },
		//{ SL2_KIF_GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR, SL2_KT_GL_UNSIGNED_BYTE, SL2_KBIF_GL_RGBA, 0x02, 0, 128, 5, 5, 1, 1, 1, true, true, false, &(CFormat::GetCompressedSizeAstc<5, 5>), CAstc::AstcToRgba32F<SL2_PF_ASTC_5_5, 5, 5, true>, CAstc::AstcFromRgba32F<SL2_PF_ASTC_5_5, 5, 5, true> },
		//{ SL2_KIF_GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR, SL2_KT_GL_UNSIGNED_BYTE, SL2_KBIF_GL_RGBA, 0x02, 0, 128, 6, 5, 1, 1, 1, true, true, false, &(CFormat::GetCompressedSizeAstc<6, 5>), CAstc::AstcToRgba32F<SL2_PF_ASTC_6_5, 6, 5, true>, CAstc::AstcFromRgba32F<SL2_PF_ASTC_6_5, 6, 5, true> },
		//{ SL2_KIF_GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR, SL2_KT_GL_UNSIGNED_BYTE, SL2_KBIF_GL_RGBA, 0x02, 0, 128, 6, 6, 1, 1, 1, true, true, false, &(CFormat::GetCompressedSizeAstc<6, 6>), CAstc::AstcToRgba32F<SL2_PF_ASTC_6_6, 6, 6, true>, CAstc::AstcFromRgba32F<SL2_PF_ASTC_6_6, 6, 6, true> },
		//{ SL2_KIF_GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR, SL2_KT_GL_UNSIGNED_BYTE, SL2_KBIF_GL_RGBA, 0x02, 0, 128, 8, 5, 1, 1, 1, true, true, false, &(CFormat::GetCompressedSizeAstc<8, 5>), CAstc::AstcToRgba32F<SL2_PF_ASTC_8_5, 8, 5, true>, CAstc::AstcFromRgba32F<SL2_PF_ASTC_8_5, 8, 5, true> },
		//{ SL2_KIF_GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR, SL2_KT_GL_UNSIGNED_BYTE, SL2_KBIF_GL_RGBA, 0x02, 0, 128, 8, 6, 1, 1, 1, true, true, false, &(CFormat::GetCompressedSizeAstc<8, 6>), CAstc::AstcToRgba32F<SL2_PF_ASTC_8_6, 8, 6, true>, CAstc::AstcFromRgba32F<SL2_PF_ASTC_8_6, 8, 6, true> },
		//{ SL2_KIF_GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR, SL2_KT_GL_UNSIGNED_BYTE, SL2_KBIF_GL_RGBA, 0x02, 0, 128, 8, 8, 1, 1, 1, true, true, false, &(CFormat::GetCompressedSizeAstc<8, 8>), CAstc::AstcToRgba32F<SL2_PF_ASTC_8_8, 8, 8, true>, CAstc::AstcFromRgba32F<SL2_PF_ASTC_8_8, 8, 8, true> },
		//{ SL2_KIF_GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR, SL2_KT_GL_UNSIGNED_BYTE, SL2_KBIF_GL_RGBA, 0x02, 0, 128, 10, 5, 1, 1, 1, true, true, false, &(CFormat::GetCompressedSizeAstc<10, 5>), CAstc::AstcToRgba32F<SL2_PF_ASTC_10_5, 10, 5, true>, CAstc::Astc2FromRgba32F<SL2_PF_ASTC_10_5, 10, 5, true> },
		//{ SL2_KIF_GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR, SL2_KT_GL_UNSIGNED_BYTE, SL2_KBIF_GL_RGBA, 0x02, 0, 128, 10, 6, 1, 1, 1, true, true, false, &(CFormat::GetCompressedSizeAstc<10, 6>), CAstc::AstcToRgba32F<SL2_PF_ASTC_10_6, 10, 6, true>, CAstc::Astc2FromRgba32F<SL2_PF_ASTC_10_6, 10, 6, true> },
		//{ SL2_KIF_GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR, SL2_KT_GL_UNSIGNED_BYTE, SL2_KBIF_GL_RGBA, 0x02, 0, 128, 10, 8, 1, 1, 1, true, true, false, &(CFormat::GetCompressedSizeAstc<10, 8>), CAstc::AstcToRgba32F<SL2_PF_ASTC_10_8, 10, 8, true>, CAstc::Astc2FromRgba32F<SL2_PF_ASTC_10_8, 10, 8, true> },
		//{ SL2_KIF_GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR, SL2_KT_GL_UNSIGNED_BYTE, SL2_KBIF_GL_RGBA, 0x02, 0, 128, 10, 10, 1, 1, 1, true, true, false, &(CFormat::GetCompressedSizeAstc<10, 10>), CAstc::AstcToRgba32F<SL2_PF_ASTC_10_10, 10, 10, true>, CAstc::Astc2FromRgba32F<SL2_PF_ASTC_10_10, 10, 10, true> },
		//{ SL2_KIF_GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR, SL2_KT_GL_UNSIGNED_BYTE, SL2_KBIF_GL_RGBA, 0x02, 0, 128, 12, 10, 1, 1, 1, true, true, false, &(CFormat::GetCompressedSizeAstc<12, 10>), CAstc::AstcToRgba32F<SL2_PF_ASTC_12_10, 12, 10, true>, CAstc::Astc2FromRgba32F<SL2_PF_ASTC_12_10, 12, 10, true> },
		//{ SL2_KIF_GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR, SL2_KT_GL_UNSIGNED_BYTE, SL2_KBIF_GL_RGBA, 0x02, 0, 128, 12, 12, 1, 1, 1, true, true, false, &(CFormat::GetCompressedSizeAstc<12, 12>), CAstc::AstcToRgba32F<SL2_PF_ASTC_12_12, 12, 12, true>, CAstc::Astc2FromRgba32F<SL2_PF_ASTC_12_12, 12, 12, true> },
		//{ SL2_KIF_GL_COMPRESSED_RGBA_ASTC_3x3x3_OES, SL2_KT_GL_UNSIGNED_BYTE, SL2_KBIF_GL_RGBA, 0x02, 0, 128, 3, 3, 3, 1, 1, false, true, false, nullptr },
		//{ SL2_KIF_GL_COMPRESSED_RGBA_ASTC_4x3x3_OES, SL2_KT_GL_UNSIGNED_BYTE, SL2_KBIF_GL_RGBA, 0x02, 0, 128, 4, 3, 3, 1, 1, false, true, false, nullptr },
		//{ SL2_KIF_GL_COMPRESSED_RGBA_ASTC_4x4x3_OES, SL2_KT_GL_UNSIGNED_BYTE, SL2_KBIF_GL_RGBA, 0x02, 0, 128, 4, 4, 3, 1, 1, false, true, false, nullptr },
		//{ SL2_KIF_GL_COMPRESSED_RGBA_ASTC_4x4x4_OES, SL2_KT_GL_UNSIGNED_BYTE, SL2_KBIF_GL_RGBA, 0x02, 0, 128, 4, 4, 4, 1, 1, false, true, false, nullptr },
		//{ SL2_KIF_GL_COMPRESSED_RGBA_ASTC_5x4x4_OES, SL2_KT_GL_UNSIGNED_BYTE, SL2_KBIF_GL_RGBA, 0x02, 0, 128, 5, 4, 4, 1, 1, false, true, false, nullptr },
		//{ SL2_KIF_GL_COMPRESSED_RGBA_ASTC_5x5x4_OES, SL2_KT_GL_UNSIGNED_BYTE, SL2_KBIF_GL_RGBA, 0x02, 0, 128, 5, 5, 4, 1, 1, false, true, false, nullptr },
		//{ SL2_KIF_GL_COMPRESSED_RGBA_ASTC_5x5x5_OES, SL2_KT_GL_UNSIGNED_BYTE, SL2_KBIF_GL_RGBA, 0x02, 0, 128, 5, 5, 5, 1, 1, false, true, false, nullptr },
		//{ SL2_KIF_GL_COMPRESSED_RGBA_ASTC_6x5x5_OES, SL2_KT_GL_UNSIGNED_BYTE, SL2_KBIF_GL_RGBA, 0x02, 0, 128, 6, 5, 5, 1, 1, false, true, false, nullptr },
		//{ SL2_KIF_GL_COMPRESSED_RGBA_ASTC_6x6x5_OES, SL2_KT_GL_UNSIGNED_BYTE, SL2_KBIF_GL_RGBA, 0x02, 0, 128, 6, 6, 5, 1, 1, false, true, false, nullptr },
		//{ SL2_KIF_GL_COMPRESSED_RGBA_ASTC_6x6x6_OES, SL2_KT_GL_UNSIGNED_BYTE, SL2_KBIF_GL_RGBA, 0x02, 0, 128, 6, 6, 6, 1, 1, false, true, false, nullptr },
		//{ SL2_KIF_GL_COMPRESSED_SRGB8_ALPHA8_ASTC_3x3x3_OES, SL2_KT_GL_UNSIGNED_BYTE, SL2_KBIF_GL_RGBA, 0x02, 0, 128, 3, 3, 3, 1, 1, true, true, false, nullptr },
		//{ SL2_KIF_GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x3x3_OES, SL2_KT_GL_UNSIGNED_BYTE, SL2_KBIF_GL_RGBA, 0x02, 0, 128, 4, 3, 3, 1, 1, true, true, false, nullptr },
		//{ SL2_KIF_GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4x3_OES, SL2_KT_GL_UNSIGNED_BYTE, SL2_KBIF_GL_RGBA, 0x02, 0, 128, 4, 4, 3, 1, 1, true, true, false, nullptr },
		//{ SL2_KIF_GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4x4_OES, SL2_KT_GL_UNSIGNED_BYTE, SL2_KBIF_GL_RGBA, 0x02, 0, 128, 4, 4, 4, 1, 1, true, true, false, nullptr },
		//{ SL2_KIF_GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4x4_OES, SL2_KT_GL_UNSIGNED_BYTE, SL2_KBIF_GL_RGBA, 0x02, 0, 128, 5, 4, 4, 1, 1, true, true, false, nullptr },
		//{ SL2_KIF_GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5x4_OES, SL2_KT_GL_UNSIGNED_BYTE, SL2_KBIF_GL_RGBA, 0x02, 0, 128, 5, 5, 4, 1, 1, true, true, false, nullptr },
		//{ SL2_KIF_GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5x5_OES, SL2_KT_GL_UNSIGNED_BYTE, SL2_KBIF_GL_RGBA, 0x02, 0, 128, 5, 5, 5, 1, 1, true, true, false, nullptr },
		//{ SL2_KIF_GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5x5_OES, SL2_KT_GL_UNSIGNED_BYTE, SL2_KBIF_GL_RGBA, 0x02, 0, 128, 6, 5, 5, 1, 1, true, true, false, nullptr },
		//{ SL2_KIF_GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6x5_OES, SL2_KT_GL_UNSIGNED_BYTE, SL2_KBIF_GL_RGBA, 0x02, 0, 128, 6, 6, 5, 1, 1, true, true, false, nullptr },
		//{ SL2_KIF_GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6x6_OES, SL2_KT_GL_UNSIGNED_BYTE, SL2_KBIF_GL_RGBA, 0x02, 0, 128, 6, 6, 6, 1, 1, true, true, false, nullptr },
		//{ SL2_KIF_GL_ATC_RGB_AMD, SL2_KT_GL_UNSIGNED_BYTE, SL2_KBIF_GL_RGB, 0x02, 0, 64, 4, 4, 1, 1, 1, false, true, false, nullptr },
		//{ SL2_KIF_GL_ATC_RGBA_EXPLICIT_ALPHA_AMD, SL2_KT_GL_UNSIGNED_BYTE, SL2_KBIF_GL_RGBA, 0x02, 0, 128, 4, 4, 1, 1, 1, false, true, false, nullptr },
		//{ SL2_KIF_GL_ATC_RGBA_INTERPOLATED_ALPHA_AMD, SL2_KT_GL_UNSIGNED_BYTE, SL2_KBIF_GL_RGBA, 0x02, 0, 128, 4, 4, 1, 1, 1, false, true, false, nullptr },
		//{ SL2_KIF_GL_PALETTE4_RGB8_OES, SL2_KT_GL_UNSIGNED_BYTE, SL2_KBIF_GL_RGB, 0x04, 384, 4, 1, 1, 1, 1, 1, false, false, true, nullptr },
		//{ SL2_KIF_GL_PALETTE4_RGBA8_OES, SL2_KT_GL_UNSIGNED_BYTE, SL2_KBIF_GL_RGBA, 0x04, 512, 4, 1, 1, 1, 1, 1, false, false, true, nullptr },
		//{ SL2_KIF_GL_PALETTE4_R5_G6_B5_OES, SL2_KT_GL_UNSIGNED_SHORT_5_6_5, SL2_KBIF_GL_RGB, 0x04, 256, 4, 1, 1, 1, 1, 1, false, false, true, nullptr },
		//{ SL2_KIF_GL_PALETTE4_RGBA4_OES, SL2_KT_GL_UNSIGNED_SHORT_4_4_4_4, SL2_KBIF_GL_RGBA, 0x04, 256, 4, 1, 1, 1, 1, 1, false, false, true, nullptr },
		//{ SL2_KIF_GL_PALETTE4_RGB5_A1_OES, SL2_KT_GL_UNSIGNED_SHORT_5_5_5_1, SL2_KBIF_GL_RGBA, 0x04, 256, 4, 1, 1, 1, 1, 1, false, false, true, nullptr },
		//{ SL2_KIF_GL_PALETTE8_RGB8_OES, SL2_KT_GL_UNSIGNED_BYTE, SL2_KBIF_GL_RGB, 0x04, 6144, 8, 1, 1, 1, 1, 1, false, false, true, nullptr },
		//{ SL2_KIF_GL_PALETTE8_RGBA8_OES, SL2_KT_GL_UNSIGNED_BYTE, SL2_KBIF_GL_RGBA, 0x04, 8192, 8, 1, 1, 1, 1, 1, false, false, true, nullptr },
		//{ SL2_KIF_GL_PALETTE8_R5_G6_B5_OES, SL2_KT_GL_UNSIGNED_SHORT_5_6_5, SL2_KBIF_GL_RGB, 0x04, 4096, 8, 1, 1, 1, 1, 1, false, false, true, nullptr },
		//{ SL2_KIF_GL_PALETTE8_RGBA4_OES, SL2_KT_GL_UNSIGNED_SHORT_4_4_4_4, SL2_KBIF_GL_RGBA, 0x04, 4096, 8, 1, 1, 1, 1, 1, false, false, true, nullptr },
		//{ SL2_KIF_GL_PALETTE8_RGB5_A1_OES, SL2_KT_GL_UNSIGNED_SHORT_5_5_5_1, SL2_KBIF_GL_RGBA, 0x04, 4096, 8, 1, 1, 1, 1, 1, false, false, true, nullptr },
		//{ SL2_KIF_GL_COLOR_INDEX1_EXT, SL2_KT_0, SL2_KBIF_0, 0x00, 0, 0, 1, 1, 1, 1, 1, false, false, false, nullptr },
		//{ SL2_KIF_GL_COLOR_INDEX2_EXT, SL2_KT_0, SL2_KBIF_0, 0x00, 0, 0, 1, 1, 1, 1, 1, false, false, false, nullptr },
		//{ SL2_KIF_GL_COLOR_INDEX4_EXT, SL2_KT_0, SL2_KBIF_0, 0x00, 0, 0, 1, 1, 1, 1, 1, false, false, false, nullptr },
		//{ SL2_KIF_GL_COLOR_INDEX8_EXT, SL2_KT_0, SL2_KBIF_0, 0x00, 0, 0, 1, 1, 1, 1, 1, false, false, false, nullptr },
		//{ SL2_KIF_GL_COLOR_INDEX12_EXT, SL2_KT_0, SL2_KBIF_0, 0x00, 0, 0, 1, 1, 1, 1, 1, false, false, false, nullptr },
		//{ SL2_KIF_GL_COLOR_INDEX16_EXT, SL2_KT_0, SL2_KBIF_0, 0x00, 0, 0, 1, 1, 1, 1, 1, false, false, false, nullptr },
		//{ SL2_KIF_GL_DEPTH_COMPONENT16, SL2_KT_GL_UNSIGNED_SHORT, SL2_KBIF_GL_DEPTH_COMPONENT, 0x08, 0, 16, 1, 1, 1, 1, 1, false, false, false, nullptr, CFormat::Depth16ToRgba32F, CFormat::Depth16FromRgba32F },
		//{ SL2_KIF_GL_DEPTH_COMPONENT24, SL2_KT_GL_UNSIGNED_INT, SL2_KBIF_GL_DEPTH_COMPONENT, 0x08, 0, 32, 1, 1, 1, 1, 1, false, false, false, nullptr, CFormat::Depth24ToRgba32F, CFormat::Depth24FromRgba32F },
		//{ SL2_KIF_GL_DEPTH_COMPONENT32, SL2_KT_GL_UNSIGNED_INT, SL2_KBIF_GL_DEPTH_COMPONENT, 0x08, 0, 32, 1, 1, 1, 1, 1, false, false, false, nullptr, CFormat::Depth32ToRgba32F, CFormat::Depth32FromRgba32F },
		//{ SL2_KIF_GL_DEPTH_COMPONENT32F, SL2_KT_GL_FLOAT, SL2_KBIF_GL_DEPTH_COMPONENT, 0x08, 0, 32, 1, 1, 1, 1, 1, false, false, false, nullptr, CFormat::Depth32FToRgba32F, CFormat::Depth32FFromRgba32F },
		//{ SL2_KIF_GL_DEPTH_COMPONENT32F_NV, SL2_KT_GL_FLOAT, SL2_KBIF_GL_DEPTH_COMPONENT, 0x08, 0, 32, 1, 1, 1, 1, 1, false, false, false, nullptr, CFormat::Depth32FToRgba32F, CFormat::Depth32FFromRgba32F },
		//{ SL2_KIF_GL_STENCIL_INDEX1, SL2_KT_GL_UNSIGNED_BYTE, SL2_KBIF_GL_STENCIL_INDEX, 0x10, 0, 8, 1, 1, 1, 1, 1, false, false, false, nullptr, CFormat::StencilXToRgba64F<uint8_t, 1>, CFormat::StencilXFromRgba64F<uint8_t, 1> },
		//{ SL2_KIF_GL_STENCIL_INDEX4, SL2_KT_GL_UNSIGNED_BYTE, SL2_KBIF_GL_STENCIL_INDEX, 0x10, 0, 8, 1, 1, 1, 1, 1, false, false, false, nullptr, CFormat::StencilXToRgba64F<uint8_t, 4>, CFormat::StencilXFromRgba64F<uint8_t, 4> },
		//{ SL2_KIF_GL_STENCIL_INDEX8, SL2_KT_GL_UNSIGNED_BYTE, SL2_KBIF_GL_STENCIL_INDEX, 0x10, 0, 8, 1, 1, 1, 1, 1, false, false, false, nullptr, CFormat::StencilXToRgba64F<uint8_t, 8>, CFormat::StencilXFromRgba64F<uint8_t, 8> },
		//{ SL2_KIF_GL_STENCIL_INDEX16, SL2_KT_GL_UNSIGNED_SHORT, SL2_KBIF_GL_STENCIL_INDEX, 0x10, 0, 16, 1, 1, 1, 1, 1, false, false, false, nullptr, CFormat::StencilXToRgba64F<uint16_t, 16>, CFormat::StencilXFromRgba64F<uint16_t, 16> },
		//{ SL2_KIF_GL_DEPTH24_STENCIL8, SL2_KT_GL_UNSIGNED_INT_24_8, SL2_KBIF_GL_DEPTH_STENCIL, 0x18, 0, 32, 1, 1, 1, 1, 1, false, false, false, nullptr, CFormat::Depth24S8ToRgba32F, CFormat::Depth24S8FromRgba32F },
		//{ SL2_KIF_GL_DEPTH32F_STENCIL8, SL2_KT_GL_FLOAT_32_UNSIGNED_INT_24_8_REV, SL2_KBIF_GL_DEPTH_STENCIL, 0x18, 0, 64, 1, 1, 1, 1, 1, false, false, false, nullptr, CFormat::Depth32FS8ToRgba32F, CFormat::Depth32FS8FromRgba32F },
		//{ SL2_KIF_GL_DEPTH32F_STENCIL8_NV, SL2_KT_GL_FLOAT_32_UNSIGNED_INT_24_8_REV, SL2_KBIF_GL_DEPTH_STENCIL, 0x18, 0, 64, 1, 1, 1, 1, 1, false, false, false, nullptr, CFormat::Depth32FS8ToRgba32F, CFormat::Depth32FS8FromRgba32F },
		
		
#undef SL2_ID
#undef SL2_GEN_INT
#undef SL2_CONV_IN
#undef SL2_CONV_F
#undef SL2_CONV_I
#undef SL2_IBITS
#undef SL2_LBITS
#undef SL2_LSHIFTS
#undef SL2_TSHIFTS
#undef SL2_TBITS
	};

	/** Whether to use NVIDA's decoding of block formats or not. */
	bool CFormat::m_bUseNVidiaDecode = false;

	/** The luma coefficients for each standard. */
	const CFormat::SL2_LUMA CFormat::m_lLumaCoeffs[SL2_LS_TOTAL] = {
		{ 0.212639005871510,	0.715168678767756,	0.072192315360734 },							// SL2_LS_REC_709
		{ 0.2627,				0.678,				0.0593 },										// SL2_LS_REC_2020
		{ 0.212,				0.701,				0.087 },										// SL2_LS_SMPTC
		{ 0.299,				0.587,				0.114 },										// SL2_LS_REC_601
		{ 0.3086,				0.6094,				0.0820 },										// SL2_LS_CIE_1931
		{ 0.30,					0.59,				0.11 },											// SL2_LS_NTSC_1953
		{ 0.299,				0.587,				0.114 },										// SL2_LS_EBU_TECH_3213
	};

	/** The current luma coefficients. */
	SL2_LUMA_STANDARDS CFormat::m_lsCurStandard = SL2_LS_REC_709;

	/** The current luma coefficients. */
	CFormat::SL2_LUMA CFormat::m_lCurCoeffs = {
		m_lLumaCoeffs[SL2_LS_REC_709]
	};


	// == Functions.
	/**
	 * Finds format data given its Vulkan format identifier.
	 * 
	 * \param _vfFormat The format to find.
	 * \return Returns the matching SL2_KTX_INTERNAL_FORMAT_DATA data, or nullptr.
	 **/
	const CFormat::SL2_KTX_INTERNAL_FORMAT_DATA * CFormat::FindFormatDataByVulkan( SL2_VKFORMAT _vfFormat ) {
		for ( size_t I = 0; I < SL2_ELEMENTS( m_kifdInternalFormats ); ++I ) {
			if ( m_kifdInternalFormats[I].vfVulkanFormat == _vfFormat ) { return &m_kifdInternalFormats[I]; }
		}
		return nullptr;
	}

	/**
	 * Finds format data given its Vulkan format string.
	 * 
	 * \param _pcFormat The format to find by name.
	 * \return Returns the matching SL2_KTX_INTERNAL_FORMAT_DATA data, or nullptr.
	 **/
	const CFormat::SL2_KTX_INTERNAL_FORMAT_DATA * CFormat::FindFormatDataByVulkan( const char * _pcFormat ) {
		for ( size_t I = 0; I < SL2_ELEMENTS( m_kifdInternalFormats ); ++I ) {
			if ( std::strcmp( m_kifdInternalFormats[I].pcVulkanName, _pcFormat ) == 0 ) { return &m_kifdInternalFormats[I]; }
		}
		return nullptr;
	}

	/**
	 * Finds format data given its Direct3D 12 format identifier.
	 * 
	 * \param _dfFormat The format to find.
	 * \return Returns the matching SL2_KTX_INTERNAL_FORMAT_DATA data, or nullptr.
	 **/
	const CFormat::SL2_KTX_INTERNAL_FORMAT_DATA * CFormat::FindFormatDataByDx( SL2_DXGI_FORMAT _dfFormat ) {
		for ( size_t I = 0; I < SL2_ELEMENTS( m_kifdInternalFormats ); ++I ) {
			if ( m_kifdInternalFormats[I].dfDxFormat == _dfFormat ) { return &m_kifdInternalFormats[I]; }
		}
		return nullptr;
	}

	/**
	 * Finds format data given its Direct3D 12 format string.
	 * 
	 * \param _pcFormat The format to find by name.
	 * \return Returns the matching SL2_KTX_INTERNAL_FORMAT_DATA data, or nullptr.
	 **/
	const CFormat::SL2_KTX_INTERNAL_FORMAT_DATA * CFormat::FindFormatDataByDx( const char * _pcFormat ) {
		for ( size_t I = 0; I < SL2_ELEMENTS( m_kifdInternalFormats ); ++I ) {
			if ( std::strcmp( m_kifdInternalFormats[I].pcDxName, _pcFormat ) == 0 ) { return &m_kifdInternalFormats[I]; }
		}
		return nullptr;
	}

	/**
	 * Finds format data given its OpenGL format identifier.
	 * 
	 * \param _kifFormat The format to find.
	 * \return Returns the matching SL2_KTX_INTERNAL_FORMAT_DATA data, or nullptr.
	 **/
	const CFormat::SL2_KTX_INTERNAL_FORMAT_DATA * CFormat::FindFormatDataByOgl( SL2_KTX_INTERNAL_FORMAT _kifFormat ) {
		for ( size_t I = 0; I < SL2_ELEMENTS( m_kifdInternalFormats ); ++I ) {
			if ( m_kifdInternalFormats[I].kifInternalFormat == _kifFormat ) { return &m_kifdInternalFormats[I]; }
		}
		return nullptr;
	}

	/**
	 * Finds format data given its OpenGL format string.
	 * 
	 * \param _pcFormat The format to find by name.
	 * \return Returns the matching SL2_KTX_INTERNAL_FORMAT_DATA data, or nullptr.
	 **/
	const CFormat::SL2_KTX_INTERNAL_FORMAT_DATA * CFormat::FindFormatDataByOgl( const char * _pcFormat ) {
		for ( size_t I = 0; I < SL2_ELEMENTS( m_kifdInternalFormats ); ++I ) {
			if ( std::strcmp( m_kifdInternalFormats[I].pcOglInternalFormat, _pcFormat ) == 0 ) { return &m_kifdInternalFormats[I]; }
		}
		return nullptr;
	}

	/**
	 * Finds format data given its Metal format identifier.
	 * 
	 * \param _mpfFormat The format to find.
	 * \return Returns the matching SL2_KTX_INTERNAL_FORMAT_DATA data, or nullptr.
	 **/
	const CFormat::SL2_KTX_INTERNAL_FORMAT_DATA * CFormat::FindFormatDataByMetal( SL2_MTLPIXELFORMAT _mpfFormat ) {
		for ( size_t I = 0; I < SL2_ELEMENTS( m_kifdInternalFormats ); ++I ) {
			if ( m_kifdInternalFormats[I].mfMetalFormat == _mpfFormat ) { return &m_kifdInternalFormats[I]; }
		}
		return nullptr;
	}

	/**
	 * Finds format data given its Metal format string.
	 * 
	 * \param _pcFormat The format to find by name.
	 * \return Returns the matching SL2_KTX_INTERNAL_FORMAT_DATA data, or nullptr.
	 **/
	const CFormat::SL2_KTX_INTERNAL_FORMAT_DATA * CFormat::FindFormatDataByMetal( const char * _pcFormat ) {
		for ( size_t I = 0; I < SL2_ELEMENTS( m_kifdInternalFormats ); ++I ) {
			if ( std::strcmp( m_kifdInternalFormats[I].pcMetalName, _pcFormat ) == 0 ) { return &m_kifdInternalFormats[I]; }
		}
		return nullptr;
	}

	/**
	 * Given a set of formats, finds the one among them that is the best fit for the given format.
	 * 
	 * \param _pkifFormat The input format.
	 * \param _ppkifFormats The array of formats.
	 * \param _sTotal The total number of formats to which _pkifFormats points.
	 * \param _pfScore AN optional pointer to a float that can receive the score for the return format.
	 * \return Returns the format among _pkifFormats that best fits _pkifFormat.  Can return nullptr in case of error.
	 **/
	const CFormat::SL2_KTX_INTERNAL_FORMAT_DATA * CFormat::FindBestFormat( const CFormat::SL2_KTX_INTERNAL_FORMAT_DATA * _pkifFormat,
		const CFormat::SL2_KTX_INTERNAL_FORMAT_DATA ** _ppkifFormats, size_t _sTotal, float * _pfScore ) {
		/*struct SL2_SCORE {
			SL2_SCORE( const CFormat::SL2_KTX_INTERNAL_FORMAT_DATA * _ppkifdFormat ) :
				pkifdFormat( _ppkifdFormat ),
				fScore( 0.0f ) {
			}
			const SL2_KTX_INTERNAL_FORMAT_DATA *	pkifdFormat = nullptr;;
			float									fScore = 0.0f;
		};
		std::vector<SL2_SCORE> vScores;*/
		const CFormat::SL2_KTX_INTERNAL_FORMAT_DATA * pkifdWinner = nullptr;
		float fScore = -INFINITY;
		try {
			for ( auto I = _sTotal; I--; ) {
				if ( _pkifFormat == _ppkifFormats[I] ||
					((_pkifFormat->vfVulkanFormat != SL2_VK_FORMAT_UNDEFINED && _pkifFormat->vfVulkanFormat == _ppkifFormats[I]->vfVulkanFormat) ||
					(_pkifFormat->dfDxFormat != SL2_DXGI_FORMAT_UNKNOWN && _pkifFormat->dfDxFormat == _ppkifFormats[I]->dfDxFormat) ||
					(_pkifFormat->mfMetalFormat != SL2_MTLPixelFormatInvalid && _pkifFormat->mfMetalFormat == _ppkifFormats[I]->mfMetalFormat) ||
					(_pkifFormat->kifInternalFormat != SL2_KIF_GL_INVALID && _pkifFormat->kifInternalFormat == _ppkifFormats[I]->kifInternalFormat)) ) {
					// Exact match with the input.
					if ( _pfScore ) { (*_pfScore) = 100.0f; }
					return _pkifFormat;
				}
				//vScores.push_back( _ppkifFormats[I] );
				float fThisScore = ScoreFormat( _pkifFormat, _ppkifFormats[I] );
				if ( fThisScore > fScore ) {
					fScore = fThisScore;
					pkifdWinner = _ppkifFormats[I];
				}
			}
		}
		catch ( ... ) { return nullptr; }

		return pkifdWinner;
	}

	/**
	 * Gets the score for how well _pkifTest matches _pkifFormat as a possible conversion target format.  IE how well _pkifFormat can be converted to _pkifTest.
	 * 
	 * \param _pkifFormat The source format.
	 * \param _pkifTest The potential destination format.
	 * \return Returns a score indicating how well _pkifFormat can be converted to _pkifTest.  Higher is better.
	 **/
	float CFormat::ScoreFormat( const SL2_KTX_INTERNAL_FORMAT_DATA * _pkifFormat,
		const SL2_KTX_INTERNAL_FORMAT_DATA * _pkifTest ) {
		if ( !_pkifFormat || !_pkifTest ) { return 0.0f; }
		float fScore = 0.0f;

		if ( _pkifFormat->ui8RBits == _pkifTest->ui8RBits ) { fScore += 1.0f; }
		if ( _pkifFormat->ui8GBits == _pkifTest->ui8GBits ) { fScore += 1.0f; }
		if ( _pkifFormat->ui8BBits == _pkifTest->ui8BBits ) { fScore += 1.0f; }
		if ( _pkifFormat->ui8ABits == _pkifTest->ui8ABits ) { fScore += 1.0f; }

		if ( _pkifFormat->ui8RBits < _pkifTest->ui8RBits ) { fScore += 0.25f; }
		if ( _pkifFormat->ui8GBits < _pkifTest->ui8GBits ) { fScore += 0.25f; }
		if ( _pkifFormat->ui8BBits < _pkifTest->ui8BBits ) { fScore += 0.25f; }
		if ( _pkifFormat->ui8ABits < _pkifTest->ui8ABits ) { fScore += 0.25f; }

		if ( !_pkifFormat->bCompressed ) {
			if ( _pkifFormat->ui8RShift == _pkifTest->ui8RShift ) { fScore += 1.0f; }
			if ( _pkifFormat->ui8GShift == _pkifTest->ui8GShift ) { fScore += 1.0f; }
			if ( _pkifFormat->ui8BShift == _pkifTest->ui8BShift ) { fScore += 1.0f; }
			if ( _pkifFormat->ui8AShift == _pkifTest->ui8AShift ) { fScore += 1.0f; }
		}

		if ( CountChannels( _pkifFormat ) == CountChannels( _pkifTest ) ) { fScore += 1.0f; }
		if ( SL2_GET_COMP_FLAG( _pkifFormat->ui32Flags ) == SL2_GET_COMP_FLAG( _pkifTest->ui32Flags ) ) { fScore += 1.0f; }

		return fScore;
	}

	/**
	 * Gets the number of channels on the given format.
	 * 
	 * \param _pkifFormat The format whose channel count is to be obtained.
	 * \return Returns the number of channels on _pkifFormat.
	 **/
	uint32_t CFormat::CountChannels( const CFormat::SL2_KTX_INTERNAL_FORMAT_DATA * _pkifFormat ) {
		if ( !_pkifFormat ) { return 0; }
		uint32_t ui32Count = 0;
		
		if ( _pkifFormat->ui8RBits ) { ++ui32Count; }
		if ( _pkifFormat->ui8GBits ) { ++ui32Count; }
		if ( _pkifFormat->ui8BBits ) { ++ui32Count; }
		if ( _pkifFormat->ui8ABits ) { ++ui32Count; }

		return ui32Count;
	}

	/**
	 * Returns the size, in pixels, of a block of a DDS file.
	 *
	 * \param _ifdFormat The DXT format whose block size is to be obtained.
	 * \return Returns the size, in pixels, of a block of a DDS file.
	 */
	uint32_t CFormat::DxtBlockSize( const SL2_KTX_INTERNAL_FORMAT_DATA &_ifdFormat ) {
		return _ifdFormat.ui32Flags >> 3;
	}

	/**
	 * Decodes a single block of DXT3 alpha.
	 *
	 * \param _ui64Block The block to decode.
	 * \param _pfPalette The created palette (contains 16 entries).
	 */
	void CFormat::DecodeDXT3_Alpha( uint64_t _ui64Block, float * _pfPalette ) {
		for ( uint32_t I = 0; I < 16; ++I ) {
			(*_pfPalette++) = (_ui64Block & 0xF) / 15.0f;
			_ui64Block >>= 4;
		}
	}

	/**
	 * Decodes a single block of BC4U.
	 *
	 * \param _ui64Block The block to decode.
	 * \param _pfPalette The created palette (contains 8 entries).
	 */
	void CFormat::DecodeBC4U( uint64_t _ui64Block, float * _pfPalette ) {
		_pfPalette[0] = ((_ui64Block >> 0) & 0xFF) / 255.0f;
		_pfPalette[1] = ((_ui64Block >> 8) & 0xFF) / 255.0f;
		if ( _pfPalette[0] > _pfPalette[1] ) {
			// 6 interpolated color values.
			_pfPalette[2] = (6.0f * _pfPalette[0] + 1.0f * _pfPalette[1]) / 7.0f;	// Bit code 010.
			_pfPalette[3] = (5.0f * _pfPalette[0] + 2.0f * _pfPalette[1]) / 7.0f;	// Bit code 011.
			_pfPalette[4] = (4.0f * _pfPalette[0] + 3.0f * _pfPalette[1]) / 7.0f;	// Bit code 100.
			_pfPalette[5] = (3.0f * _pfPalette[0] + 4.0f * _pfPalette[1]) / 7.0f;	// Bit code 101.
			_pfPalette[6] = (2.0f * _pfPalette[0] + 5.0f * _pfPalette[1]) / 7.0f;	// Bit code 110.
			_pfPalette[7] = (1.0f * _pfPalette[0] + 6.0f * _pfPalette[1]) / 7.0f;	// Bit code 111.
		}
		else {
			// 4 interpolated color values.
			_pfPalette[2] = (4.0f * _pfPalette[0] + 1.0f * _pfPalette[1]) / 5.0f;	// Bit code 010.
			_pfPalette[3] = (3.0f * _pfPalette[0] + 2.0f * _pfPalette[1]) / 5.0f;	// Bit code 011.
			_pfPalette[4] = (2.0f * _pfPalette[0] + 3.0f * _pfPalette[1]) / 5.0f;	// Bit code 100.
			_pfPalette[5] = (1.0f * _pfPalette[0] + 4.0f * _pfPalette[1]) / 5.0f;	// Bit code 101.
			_pfPalette[6] = 0.0f;													// Bit code 110.
			_pfPalette[7] = 1.0f;													// Bit code 111.
		}
	}

	/**
	 * Decodes a single block of BC4S.
	 *
	 * \param _ui64Block The block to decode.
	 * \param _pfPalette The created palette (contains 8 entries).
	 */
	void CFormat::DecodeBC4S( uint64_t _ui64Block, float * _pfPalette ) {
		int8_t i8C0 = static_cast<int8_t>(_ui64Block >> 0);
		int8_t i8C1 = static_cast<int8_t>(_ui64Block >> 8);
		_pfPalette[0] = i8C0 == -128 ? -1.0f : i8C0 / 127.0f;
		_pfPalette[1] = i8C1 == -128 ? -1.0f : i8C1 / 127.0f;
		if ( _pfPalette[0] > _pfPalette[1] ) {
			// 6 interpolated color values.
			_pfPalette[2] = (6.0f * _pfPalette[0] + 1.0f * _pfPalette[1]) / 7.0f;	// Bit code 010.
			_pfPalette[3] = (5.0f * _pfPalette[0] + 2.0f * _pfPalette[1]) / 7.0f;	// Bit code 011.
			_pfPalette[4] = (4.0f * _pfPalette[0] + 3.0f * _pfPalette[1]) / 7.0f;	// Bit code 100.
			_pfPalette[5] = (3.0f * _pfPalette[0] + 4.0f * _pfPalette[1]) / 7.0f;	// Bit code 101.
			_pfPalette[6] = (2.0f * _pfPalette[0] + 5.0f * _pfPalette[1]) / 7.0f;	// Bit code 110.
			_pfPalette[7] = (1.0f * _pfPalette[0] + 6.0f * _pfPalette[1]) / 7.0f;	// Bit code 111.
		}
		else {
			// 4 interpolated color values.
			_pfPalette[2] = (4.0f * _pfPalette[0] + 1.0f * _pfPalette[1]) / 5.0f;	// Bit code 010.
			_pfPalette[3] = (3.0f * _pfPalette[0] + 2.0f * _pfPalette[1]) / 5.0f;	// Bit code 011.
			_pfPalette[4] = (2.0f * _pfPalette[0] + 3.0f * _pfPalette[1]) / 5.0f;	// Bit code 100.
			_pfPalette[5] = (1.0f * _pfPalette[0] + 4.0f * _pfPalette[1]) / 5.0f;	// Bit code 101.
			_pfPalette[6] = -1.0f;													// Bit code 110.
			_pfPalette[7] = 1.0f;													// Bit code 111.
		}
	}

	/**
	 * Gets the indices from a DXT1 block. 
	 *
	 * \param _ui64Block The block to decode.
	 * \param _pui8Indices The 16 indices as extracted from the block.
	 */
	void CFormat::Dxt1Indices( uint64_t _ui64Block, uint8_t * _pui8Indices ) {
		_ui64Block >>= 32;
		for ( uint32_t I = 0; I < 16; ++I ) {
			(*_pui8Indices++) = _ui64Block & 0x3;
			_ui64Block >>= 2;
		}
	}

	/**
	 * Gets the indices from a BC4 block. 
	 *
	 * \param _ui64Block The block to decode.
	 * \param _pui8Indices The 16 indices as extracted from the block.
	 */
	void CFormat::Bc4Indices( uint64_t _ui64Block, uint8_t * _pui8Indices ) {
		_ui64Block >>= 16;
		for ( uint32_t I = 0; I < 16; ++I ) {
			(*_pui8Indices++) = _ui64Block & 0x7;
			_ui64Block >>= 3;
		}
	}

}	// namespace sl2

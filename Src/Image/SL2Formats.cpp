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
		{ SL2_ID( VK_FORMAT_B8G8R8_UNORM, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_SRGB8, GL_UNSIGNED_BYTE, GL_BGR ),									0x00, 0, 24, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_GEN_INT( SL2_TBITS(8, 8, 8, 0), SL2_TSHIFTS(16, 8, 0, 0), 3, false, true, false ) },
		{ SL2_ID( VK_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R8G8B8A8_UNORM, MTLPixelFormatRGBA8Unorm, GL_RGBA8, GL_UNSIGNED_BYTE, GL_RGBA ),					0x00, 0, 32, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_GEN_INT( SL2_TBITS(8, 8, 8, 8), SL2_TSHIFTS(0, 8, 16, 24), 4, false, true, false ) },
		{ SL2_ID( VK_FORMAT_B8G8R8A8_UNORM, DXGI_FORMAT_B8G8R8A8_UNORM, MTLPixelFormatBGRA8Unorm, GL_RGBA8, GL_UNSIGNED_BYTE, GL_BGRA ),					0x00, 0, 32, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_GEN_INT( SL2_TBITS(8, 8, 8, 8), SL2_TSHIFTS(16, 8, 0, 24), 4, false, true, false ) },

		{ SL2_ID( VK_FORMAT_R8_SNORM, DXGI_FORMAT_R8_SNORM, MTLPixelFormatR8Snorm, GL_R8_SNORM, GL_BYTE, GL_RED ),											0x00, 0, 8, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_GEN_INT( SL2_TBITS(8, 0, 0, 0), SL2_TSHIFTS(0, 0, 0, 0), 1, true, true, false ) },
		{ SL2_ID( VK_FORMAT_R8G8_SNORM, DXGI_FORMAT_R8G8_SNORM, MTLPixelFormatRG8Snorm, GL_RG8_SNORM, GL_BYTE, GL_RG ),										0x00, 0, 16, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_GEN_INT( SL2_TBITS(8, 8, 0, 0), SL2_TSHIFTS(0, 8, 0, 0), 2, true, true, false ) },
		{ SL2_ID( VK_FORMAT_R8G8B8_SNORM, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_RGB8_SNORM, GL_BYTE, GL_RGB ),										0x00, 0, 24, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_GEN_INT( SL2_TBITS(8, 8, 8, 0), SL2_TSHIFTS(0, 8, 16, 0), 3, true, true, false ) },
		{ SL2_ID( VK_FORMAT_B8G8R8_SNORM, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_RGB8_SNORM, GL_BYTE, GL_BGR ),										0x00, 0, 24, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_GEN_INT( SL2_TBITS(8, 8, 8, 0), SL2_TSHIFTS(16, 8, 0, 0), 3, true, true, false ) },
		{ SL2_ID( VK_FORMAT_R8G8B8A8_SNORM, DXGI_FORMAT_R8G8B8A8_SNORM, MTLPixelFormatRGBA8Snorm, GL_RGBA8_SNORM, GL_BYTE, GL_RGBA ),						0x00, 0, 32, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_GEN_INT( SL2_TBITS(8, 8, 8, 8), SL2_TSHIFTS(0, 8, 16, 24), 4, true, true, false ) },
		{ SL2_ID( VK_FORMAT_B8G8R8A8_SNORM, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_RGBA8_SNORM, GL_BYTE, GL_BGRA ),									0x00, 0, 32, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_GEN_INT( SL2_TBITS(8, 8, 8, 8), SL2_TSHIFTS(16, 8, 0, 24), 4, true, true, false ) },

		{ SL2_ID( VK_FORMAT_R8_UINT, DXGI_FORMAT_R8_UINT, MTLPixelFormatR8Uint, GL_R8UI, GL_UNSIGNED_BYTE, GL_RED_INTEGER ),								0x00, 0, 8, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_GEN_INT( SL2_TBITS(8, 0, 0, 0), SL2_TSHIFTS(0, 0, 0, 0), 1, false, false, false ) },
		{ SL2_ID( VK_FORMAT_R8G8_UINT, DXGI_FORMAT_R8G8_UINT, MTLPixelFormatRG8Uint, GL_RG8UI, GL_UNSIGNED_BYTE, GL_RG_INTEGER ),							0x00, 0, 16, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_GEN_INT( SL2_TBITS(8, 8, 0, 0), SL2_TSHIFTS(0, 8, 0, 0), 2, false, false, false ) },
		{ SL2_ID( VK_FORMAT_R8G8B8_UINT, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_RGB8UI, GL_UNSIGNED_BYTE, GL_RGB_INTEGER ),							0x00, 0, 24, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_GEN_INT( SL2_TBITS(8, 8, 8, 0), SL2_TSHIFTS(0, 8, 16, 0), 3, false, false, false ) },
		{ SL2_ID( VK_FORMAT_B8G8R8_UINT, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_RGB8UI, GL_UNSIGNED_BYTE, GL_BGR_INTEGER ),							0x00, 0, 24, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_GEN_INT( SL2_TBITS(8, 8, 8, 0), SL2_TSHIFTS(16, 8, 0, 0), 3, false, false, false ) },
		{ SL2_ID( VK_FORMAT_R8G8B8A8_UINT, DXGI_FORMAT_R8G8B8A8_UINT, MTLPixelFormatRGBA8Uint, GL_RGBA8UI, GL_UNSIGNED_BYTE, GL_RGBA_INTEGER ),				0x00, 0, 32, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_GEN_INT( SL2_TBITS(8, 8, 8, 8), SL2_TSHIFTS(0, 8, 16, 24), 4, false, false, false ) },
		{ SL2_ID( VK_FORMAT_B8G8R8A8_UINT, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_RGBA8UI, GL_UNSIGNED_BYTE, GL_BGRA_INTEGER ),						0x00, 0, 32, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_GEN_INT( SL2_TBITS(8, 8, 8, 8), SL2_TSHIFTS(16, 8, 0, 24), 4, false, false, false ) },

		{ SL2_ID( VK_FORMAT_R8_SINT, DXGI_FORMAT_R8_SINT, MTLPixelFormatR8Sint, GL_R8I, GL_BYTE, GL_RED_INTEGER ),											0x00, 0, 8, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_GEN_INT( SL2_TBITS(8, 0, 0, 0), SL2_TSHIFTS(0, 0, 0, 0), 1, true, false, false ) },
		{ SL2_ID( VK_FORMAT_R8G8_SINT, DXGI_FORMAT_R8G8_SINT, MTLPixelFormatRG8Sint, GL_RG8I, GL_BYTE, GL_RG_INTEGER ),										0x00, 0, 16, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_GEN_INT( SL2_TBITS(8, 8, 0, 0), SL2_TSHIFTS(0, 8, 0, 0), 2, true, false, false ) },
		{ SL2_ID( VK_FORMAT_R8G8B8_SINT, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_RGB8I, GL_BYTE, GL_RGB_INTEGER ),									0x00, 0, 24, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_GEN_INT( SL2_TBITS(8, 8, 8, 0), SL2_TSHIFTS(0, 8, 16, 0), 3, true, false, false ) },
		{ SL2_ID( VK_FORMAT_B8G8R8_SINT, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_RGB8I, GL_BYTE, GL_BGR_INTEGER ),									0x00, 0, 24, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_GEN_INT( SL2_TBITS(8, 8, 8, 0), SL2_TSHIFTS(16, 8, 0, 0), 3, true, false, false ) },
		{ SL2_ID( VK_FORMAT_R8G8B8A8_SINT, DXGI_FORMAT_R8G8B8A8_SINT, MTLPixelFormatRGBA8Sint, GL_RGBA8I, GL_UNSIGNED_BYTE, GL_RGBA_INTEGER ),				0x00, 0, 32, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_GEN_INT( SL2_TBITS(8, 8, 8, 8), SL2_TSHIFTS(0, 8, 16, 24), 4, true, false, false ) },
		{ SL2_ID( VK_FORMAT_B8G8R8A8_SINT, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_RGBA8I, GL_UNSIGNED_BYTE, GL_BGRA_INTEGER ),						0x00, 0, 32, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_GEN_INT( SL2_TBITS(8, 8, 8, 8), SL2_TSHIFTS(16, 8, 0, 24), 4, true, false, false ) },

		{ SL2_ID( VK_FORMAT_R8_SRGB, DXGI_FORMAT_UNKNOWN, MTLPixelFormatR8Unorm_sRGB, GL_SR8, GL_UNSIGNED_BYTE, GL_RED ),									0x00, 0, 8, 1, 1, 1, 1, 1, true, false, false, false, nullptr, SL2_GEN_INT( SL2_TBITS(8, 0, 0, 0), SL2_TSHIFTS(0, 0, 0, 0), 1, false, true, true ) },
		{ SL2_ID( VK_FORMAT_R8G8_SRGB, DXGI_FORMAT_UNKNOWN, MTLPixelFormatRG8Unorm_sRGB, GL_SRG8, GL_UNSIGNED_BYTE, GL_RG ),								0x00, 0, 16, 1, 1, 1, 1, 1, true, false, false, false, nullptr, SL2_GEN_INT( SL2_TBITS(8, 8, 0, 0), SL2_TSHIFTS(0, 8, 0, 0), 2, false, true, true ) },
		{ SL2_ID( VK_FORMAT_R8G8B8_SRGB, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_SRGB8, GL_UNSIGNED_BYTE, GL_RGB ),									0x00, 0, 24, 1, 1, 1, 1, 1, true, false, false, false, nullptr, SL2_GEN_INT( SL2_TBITS(8, 8, 8, 0), SL2_TSHIFTS(0, 8, 16, 0), 3, false, true, true ) },
		{ SL2_ID( VK_FORMAT_B8G8R8_SRGB, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_SRGB8, GL_UNSIGNED_BYTE, GL_BGR ),									0x00, 0, 24, 1, 1, 1, 1, 1, true, false, false, false, nullptr, SL2_GEN_INT( SL2_TBITS(8, 8, 8, 0), SL2_TSHIFTS(16, 8, 0, 0), 3, false, true, true ) },
		{ SL2_ID( VK_FORMAT_R8G8B8A8_SRGB, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, MTLPixelFormatRGBA8Unorm_sRGB, GL_SRGB8_ALPHA8, GL_UNSIGNED_BYTE, GL_RGBA ),	0x00, 0, 32, 1, 1, 1, 1, 1, true, false, false, false, nullptr, SL2_GEN_INT( SL2_TBITS(8, 8, 8, 8), SL2_TSHIFTS(0, 8, 16, 24), 4, false, true, true ) },
		{ SL2_ID( VK_FORMAT_B8G8R8A8_SRGB, DXGI_FORMAT_B8G8R8A8_UNORM_SRGB, MTLPixelFormatBGRA8Unorm_sRGB, GL_SRGB8_ALPHA8, GL_UNSIGNED_BYTE, GL_BGRA ),	0x00, 0, 32, 1, 1, 1, 1, 1, true, false, false, false, nullptr, SL2_GEN_INT( SL2_TBITS(8, 8, 8, 8), SL2_TSHIFTS(16, 8, 0, 24), 4, false, true, true ) },

		{ SL2_ID( VK_FORMAT_R16_UNORM, DXGI_FORMAT_R16_UNORM, MTLPixelFormatR16Unorm, GL_R16, GL_UNSIGNED_SHORT, GL_RED ),									0x00, 0, 16, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_GEN_INT( SL2_TBITS(16, 0, 0, 0), SL2_TSHIFTS(0, 0, 0, 0), 2, false, true, false ) },
		{ SL2_ID( VK_FORMAT_R16G16_UNORM, DXGI_FORMAT_R16G16_UNORM, MTLPixelFormatRG16Unorm, GL_RG16, GL_UNSIGNED_SHORT, GL_RG ),							0x00, 0, 32, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_GEN_INT( SL2_TBITS(16, 16, 0, 0), SL2_TSHIFTS(0, 16, 0, 0), 4, false, true, false ) },
		{ SL2_ID( VK_FORMAT_R16G16B16_UNORM, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_RGB16, GL_UNSIGNED_SHORT, GL_RGB ),								0x00, 0, 48, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_GEN_INT( SL2_TBITS(16, 16, 16, 0), SL2_TSHIFTS(0, 16, 32, 0), 6, false, true, false ) },
		{ SL2_ID( VK_FORMAT_R16G16B16A16_UNORM, DXGI_FORMAT_R16G16B16A16_UNORM, MTLPixelFormatRGBA16Unorm, GL_RGBA16, GL_UNSIGNED_SHORT, GL_RGBA ),			0x00, 0, 64, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_GEN_INT( SL2_TBITS(16, 16, 16, 16), SL2_TSHIFTS(0, 16, 32, 48), 8, false, true, false ) },

		{ SL2_ID( VK_FORMAT_R16_SNORM, DXGI_FORMAT_R16_SNORM, MTLPixelFormatR16Snorm, GL_R16_SNORM, GL_SHORT, GL_RED ),										0x00, 0, 16, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_GEN_INT( SL2_TBITS(16, 0, 0, 0), SL2_TSHIFTS(0, 0, 0, 0), 2, true, true, false ) },
		{ SL2_ID( VK_FORMAT_R16G16_SNORM, DXGI_FORMAT_R16G16_SNORM, MTLPixelFormatRG16Snorm, GL_RG16_SNORM, GL_SHORT, GL_RG ),								0x00, 0, 32, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_GEN_INT( SL2_TBITS(16, 16, 0, 0), SL2_TSHIFTS(0, 16, 0, 0), 4, true, true, false ) },
		{ SL2_ID( VK_FORMAT_R16G16B16_SNORM, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_RGB16_SNORM, GL_SHORT, GL_RGB ),								0x00, 0, 48, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_GEN_INT( SL2_TBITS(16, 16, 16, 0), SL2_TSHIFTS(0, 16, 32, 0), 6, true, true, false ) },
		{ SL2_ID( VK_FORMAT_R16G16B16A16_SNORM, DXGI_FORMAT_R16G16B16A16_SNORM, MTLPixelFormatRGBA16Snorm, GL_RGBA16_SNORM, GL_SHORT, GL_RGBA ),			0x00, 0, 64, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_GEN_INT( SL2_TBITS(16, 16, 16, 16), SL2_TSHIFTS(0, 16, 32, 48), 8, true, true, false ) },

		{ SL2_ID( VK_FORMAT_R16_UINT, DXGI_FORMAT_R16_UINT, MTLPixelFormatR16Uint, GL_R16UI, GL_UNSIGNED_SHORT, GL_RED_INTEGER ),							0x00, 0, 16, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_GEN_INT( SL2_TBITS(16, 0, 0, 0), SL2_TSHIFTS(0, 0, 0, 0), 2, false, false, false ) },
		{ SL2_ID( VK_FORMAT_R16G16_UINT, DXGI_FORMAT_R16G16_UINT, MTLPixelFormatRG16Uint, GL_RG16UI, GL_UNSIGNED_SHORT, GL_RG_INTEGER ),					0x00, 0, 32, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_GEN_INT( SL2_TBITS(16, 16, 0, 0), SL2_TSHIFTS(0, 16, 0, 0), 4, false, false, false ) },
		{ SL2_ID( VK_FORMAT_R16G16B16_UINT, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_RGB16UI, GL_UNSIGNED_SHORT, GL_RGB_INTEGER ),					0x00, 0, 48, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_GEN_INT( SL2_TBITS(16, 16, 16, 0), SL2_TSHIFTS(0, 16, 32, 0), 6, false, false, false ) },
		{ SL2_ID( VK_FORMAT_R16G16B16A16_UINT, DXGI_FORMAT_R16G16B16A16_UINT, MTLPixelFormatRGBA16Uint, GL_RGBA16UI, GL_UNSIGNED_SHORT, GL_RGBA_INTEGER ),	0x00, 0, 64, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_GEN_INT( SL2_TBITS(16, 16, 16, 16), SL2_TSHIFTS(0, 16, 32, 48), 8, false, false, false ) },

		{ SL2_ID( VK_FORMAT_R16_SINT, DXGI_FORMAT_R16_SINT, MTLPixelFormatR16Sint, GL_R16I, GL_SHORT, GL_RED_INTEGER ),										0x00, 0, 16, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_GEN_INT( SL2_TBITS(16, 0, 0, 0), SL2_TSHIFTS(0, 0, 0, 0), 2, true, false, false ) },
		{ SL2_ID( VK_FORMAT_R16G16_SINT, DXGI_FORMAT_R16G16_SINT, MTLPixelFormatRG16Sint, GL_RG16I, GL_SHORT, GL_RG_INTEGER ),								0x00, 0, 32, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_GEN_INT( SL2_TBITS(16, 16, 0, 0), SL2_TSHIFTS(0, 16, 0, 0), 4, true, false, false ) },
		{ SL2_ID( VK_FORMAT_R16G16B16_SINT, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_RGB16I, GL_SHORT, GL_RGB_INTEGER ),								0x00, 0, 48, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_GEN_INT( SL2_TBITS(16, 16, 16, 0), SL2_TSHIFTS(0, 16, 32, 0), 6, true, false, false ) },
		{ SL2_ID( VK_FORMAT_R16G16B16A16_SINT, DXGI_FORMAT_R16G16B16A16_SINT, MTLPixelFormatRGBA16Sint, GL_RGBA16I, GL_SHORT, GL_RGBA_INTEGER ),			0x00, 0, 64, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_GEN_INT( SL2_TBITS(16, 16, 16, 16), SL2_TSHIFTS(0, 16, 32, 48), 8, true, false, false ) },
		
		{ SL2_ID( VK_FORMAT_R16_SFLOAT, DXGI_FORMAT_R16_FLOAT, MTLPixelFormatR16Float, GL_R16F, GL_HALF_FLOAT, GL_RED ),									0x00, 0, 16, 1, 1, 1, 1, 1, false, false, false, true, nullptr, SL2_TBITS(16, 0, 0, 0), SL2_TSHIFTS(0, 0, 0, 0), SL2_CONV_F( CFormat::F16ToRgba64F, CFormat::F16FromRgba64F, SL2_TBITS(16, 0, 0, 0), SL2_TSHIFTS(0, 0, 0, 0), 2 ) },
		{ SL2_ID( VK_FORMAT_R16G16_SFLOAT, DXGI_FORMAT_R16G16_FLOAT, MTLPixelFormatRG16Float, GL_RG16F, GL_HALF_FLOAT, GL_RG ),								0x00, 0, 32, 1, 1, 1, 1, 1, false, false, false, true, nullptr, SL2_TBITS(16, 16, 0, 0), SL2_TSHIFTS(0, 16, 0, 0), SL2_CONV_F( CFormat::F16ToRgba64F, CFormat::F16FromRgba64F, SL2_TBITS(16, 16, 0, 0), SL2_TSHIFTS(0, 16, 0, 0), 4 ) },
		{ SL2_ID( VK_FORMAT_R16G16B16_SFLOAT, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_RGB16F, GL_HALF_FLOAT, GL_RGB ),								0x00, 0, 48, 1, 1, 1, 1, 1, false, false, false, true, nullptr, SL2_TBITS(16, 16, 16, 0), SL2_TSHIFTS(0, 16, 32, 0), SL2_CONV_F( CFormat::F16ToRgba64F, CFormat::F16FromRgba64F, SL2_TBITS(16, 16, 16, 0), SL2_TSHIFTS(0, 16, 32, 0), 6 ) },
		{ SL2_ID( VK_FORMAT_R16G16B16A16_SFLOAT, DXGI_FORMAT_R16G16B16A16_FLOAT, MTLPixelFormatRGBA16Float, GL_RGBA16F, GL_HALF_FLOAT, GL_RGBA ),			0x00, 0, 64, 1, 1, 1, 1, 1, false, false, false, true, nullptr, SL2_TBITS(16, 16, 16, 16), SL2_TSHIFTS(0, 16, 32, 48), SL2_CONV_F( CFormat::F16ToRgba64F, CFormat::F16FromRgba64F, SL2_TBITS(16, 16, 16, 16), SL2_TSHIFTS(0, 16, 32, 48), 8 ) },
		
		{ SL2_ID( VK_FORMAT_R32_UINT, DXGI_FORMAT_R32_UINT, MTLPixelFormatR32Uint, GL_R32UI, GL_UNSIGNED_INT, GL_RED_INTEGER ),								0x00, 0, 32, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_CONV_I( CFormat::Int128ToRgba64F, CFormat::Int128FromRgba64F, SL2_TBITS(32, 0, 0, 0), SL2_TSHIFTS(0, 0, 0, 0), 4, false, false, false ) },
		{ SL2_ID( VK_FORMAT_R32G32_UINT, DXGI_FORMAT_R32G32_UINT, MTLPixelFormatRG32Uint, GL_RG32UI, GL_UNSIGNED_INT, GL_RG_INTEGER ),						0x00, 0, 64, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_CONV_I( CFormat::Int128ToRgba64F, CFormat::Int128FromRgba64F, SL2_TBITS(32, 32, 0, 0), SL2_TSHIFTS(0, 32, 0, 0), 8, false, false, false ) },
		{ SL2_ID( VK_FORMAT_R32G32B32_UINT, DXGI_FORMAT_R32G32B32_UINT, MTLPixelFormatInvalid, GL_RGB32UI, GL_UNSIGNED_INT, GL_RGB_INTEGER ),				0x00, 0, 96, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_CONV_I( CFormat::Int128ToRgba64F, CFormat::Int128FromRgba64F, SL2_TBITS(32, 32, 32, 0), SL2_TSHIFTS(0, 32, 64, 0), 12, false, false, false ) },
		{ SL2_ID( VK_FORMAT_R32G32B32A32_UINT, DXGI_FORMAT_R32G32B32A32_UINT, MTLPixelFormatRGBA32Uint, GL_RGBA32UI, GL_UNSIGNED_INT, GL_RGBA_INTEGER ),	0x00, 0, 128, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_CONV_I( CFormat::Int128ToRgba64F, CFormat::Int128FromRgba64F, SL2_TBITS(32, 32, 32, 32), SL2_TSHIFTS(0, 32, 64, 96), 16, false, false, false ) },
		
		{ SL2_ID( VK_FORMAT_R32_SINT, DXGI_FORMAT_R32_SINT, MTLPixelFormatR32Sint, GL_R32I, GL_INT, GL_RED_INTEGER ),										0x00, 0, 32, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_CONV_I( CFormat::Int128ToRgba64F, CFormat::Int128FromRgba64F, SL2_TBITS(32, 0, 0, 0), SL2_TSHIFTS(0, 0, 0, 0), 4, true, false, false ) },
		{ SL2_ID( VK_FORMAT_R32G32_SINT, DXGI_FORMAT_R32G32_SINT, MTLPixelFormatRG32Sint, GL_RG32I, GL_INT, GL_RG_INTEGER ),								0x00, 0, 64, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_CONV_I( CFormat::Int128ToRgba64F, CFormat::Int128FromRgba64F, SL2_TBITS(32, 32, 0, 0), SL2_TSHIFTS(0, 32, 0, 0), 8, true, false, false ) },
		{ SL2_ID( VK_FORMAT_R32G32B32_SINT, DXGI_FORMAT_R32G32B32_SINT, MTLPixelFormatInvalid, GL_RGB32I, GL_INT, GL_RGB_INTEGER ),							0x00, 0, 96, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_CONV_I( CFormat::Int128ToRgba64F, CFormat::Int128FromRgba64F, SL2_TBITS(32, 32, 32, 0), SL2_TSHIFTS(0, 32, 64, 0), 12, true, false, false ) },
		{ SL2_ID( VK_FORMAT_R32G32B32A32_SINT, DXGI_FORMAT_R32G32B32A32_SINT, MTLPixelFormatRGBA32Sint, GL_RGBA32I, GL_INT, GL_RGBA_INTEGER ),				0x00, 0, 128, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_CONV_I( CFormat::Int128ToRgba64F, CFormat::Int128FromRgba64F, SL2_TBITS(32, 32, 32, 32), SL2_TSHIFTS(0, 32, 64, 96), 16, true, false, false ) },
		
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
		{ SL2_ID( VK_FORMAT_R4G4_UNORM_PACK8, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_INVALID, GL_INVALID, GL_INVALID ),								0x01, 0, 8, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_GEN_INT( SL2_TBITS(4, 4, 0, 0), SL2_TSHIFTS(4, 0, 0, 0), 2, false, true, false ) },
		{ SL2_ID( VK_FORMAT_UNDEFINED, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_RGB4, GL_UNSIGNED_SHORT_4_4_4_4, GL_RGB ),							0x01, 0, 16, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_GEN_INT( SL2_TBITS(4, 4, 4, 0), SL2_TSHIFTS(0, 4, 8, 0), 3, false, true, false ) },
		{ SL2_ID( VK_FORMAT_R4G4B4A4_UNORM_PACK16, DXGI_FORMAT_UNKNOWN, MTLPixelFormatABGR4Unorm, GL_RGBA4, GL_UNSIGNED_SHORT_4_4_4_4, GL_RGBA ),			0x01, 0, 16, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_GEN_INT( SL2_TBITS(4, 4, 4, 4), SL2_TSHIFTS(12, 8, 4, 0), 2, false, true, false ) },
		{ SL2_ID( VK_FORMAT_B4G4R4A4_UNORM_PACK16, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_RGBA4, GL_UNSIGNED_SHORT_4_4_4_4, GL_BGRA ),				0x01, 0, 16, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_GEN_INT( SL2_TBITS(4, 4, 4, 4), SL2_TSHIFTS(12, 8, 4, 0), 2, false, true, false ) },
		{ SL2_ID( VK_FORMAT_UNDEFINED, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_RGB5, GL_UNSIGNED_SHORT_5_5_5_1, GL_RGB ),							0x01, 0, 16, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_GEN_INT( SL2_TBITS(5, 5, 5, 0), SL2_TSHIFTS(1, 6, 11, 0), 2, false, true, false ) },
		{ SL2_ID( VK_FORMAT_R5G6B5_UNORM_PACK16, DXGI_FORMAT_B5G6R5_UNORM, MTLPixelFormatB5G6R5Unorm, GL_RGB565, GL_UNSIGNED_SHORT_5_6_5, GL_RGB ),			0x01, 0, 16, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_GEN_INT( SL2_TBITS(5, 6, 5, 0), SL2_TSHIFTS(11, 5, 0, 0), 2, false, true, false ) },
		{ SL2_ID( VK_FORMAT_B5G6R5_UNORM_PACK16, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_RGB565, GL_UNSIGNED_SHORT_5_6_5_REV, GL_RGB ),				0x01, 0, 16, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_GEN_INT( SL2_TBITS(5, 6, 5, 0), SL2_TSHIFTS(0, 5, 11, 0), 2, false, true, false ) },
		{ SL2_ID( VK_FORMAT_UNDEFINED, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_RGB10, GL_UNSIGNED_SHORT, GL_RGB ),									0x01, 0, 48, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_GEN_INT( SL2_TBITS(16, 16, 16, 0), SL2_TSHIFTS(0, 16, 32, 0), 6, false, true, false ) },
		{ SL2_ID( VK_FORMAT_UNDEFINED, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_RGB12, GL_UNSIGNED_SHORT, GL_RGB ),									0x01, 0, 48, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_GEN_INT( SL2_TBITS(16, 16, 16, 0), SL2_TSHIFTS(0, 16, 32, 0), 6, false, true, false ) },
		{ SL2_ID( VK_FORMAT_UNDEFINED, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_RGBA2, GL_UNSIGNED_BYTE, GL_RGBA ),									0x01, 0, 32, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_GEN_INT( SL2_TBITS(8, 8, 8, 8), SL2_TSHIFTS(0, 8, 16, 24), 4, false, true, false ) },
		{ SL2_ID( VK_FORMAT_UNDEFINED, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_RGBA12, GL_UNSIGNED_SHORT, GL_RGBA ),									0x01, 0, 64, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_GEN_INT( SL2_TBITS(16, 16, 16, 1), SL2_TSHIFTS(0, 16, 32, 48), 8, false, true, false ) },
		{ SL2_ID( VK_FORMAT_R5G5B5A1_UNORM_PACK16, DXGI_FORMAT_UNKNOWN, MTLPixelFormatA1BGR5Unorm, GL_RGB5_A1, GL_UNSIGNED_SHORT_5_5_5_1, GL_RGBA ),		0x01, 0, 16, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_GEN_INT( SL2_TBITS(5, 5, 5, 1), SL2_TSHIFTS(11, 6, 1, 0), 2, false, true, false ) },
		{ SL2_ID( VK_FORMAT_UNDEFINED, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_RGB5_A1, GL_UNSIGNED_SHORT_5_5_5_1, GL_BGRA ),						0x01, 0, 16, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_GEN_INT( SL2_TBITS(5, 5, 5, 1), SL2_TSHIFTS(1, 6, 11, 0), 2, false, true, false ) },
		{ SL2_ID( VK_FORMAT_A1B5G5R5_UNORM_PACK16_KHR, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_RGB5_A1, GL_UNSIGNED_SHORT_1_5_5_5_REV, GL_RGBA ),	0x01, 0, 16, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_GEN_INT( SL2_TBITS(5, 5, 5, 1), SL2_TSHIFTS(1, 6, 11, 0), 2, false, true, false ) },
		{ SL2_ID( VK_FORMAT_A1R5G5B5_UNORM_PACK16, DXGI_FORMAT_B5G5R5A1_UNORM, MTLPixelFormatBGR5A1Unorm, GL_RGB5_A1, GL_UNSIGNED_SHORT_1_5_5_5_REV, GL_BGRA ),
																																							0x01, 0, 16, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_GEN_INT( SL2_TBITS(5, 5, 5, 1), SL2_TSHIFTS(10, 5, 0, 15), 2, false, true, false ) },
			
		{ SL2_ID( VK_FORMAT_A8B8G8R8_UNORM_PACK32, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_INVALID, GL_INVALID, GL_INVALID ),						0x01, 0, 32, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_GEN_INT( SL2_TBITS(8, 8, 8, 8), SL2_TSHIFTS(0, 8, 16, 24), 4, false, true, false ) },
		{ SL2_ID( VK_FORMAT_A8B8G8R8_SNORM_PACK32, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_INVALID, GL_INVALID, GL_INVALID ),						0x01, 0, 32, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_GEN_INT( SL2_TBITS(8, 8, 8, 8), SL2_TSHIFTS(0, 8, 16, 24), 4, true, true, false ) },
		{ SL2_ID( VK_FORMAT_A8B8G8R8_UINT_PACK32, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_INVALID, GL_INVALID, GL_INVALID ),							0x01, 0, 32, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_GEN_INT( SL2_TBITS(8, 8, 8, 8), SL2_TSHIFTS(0, 8, 16, 24), 4, false, false, false ) },
		{ SL2_ID( VK_FORMAT_A8B8G8R8_SINT_PACK32, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_INVALID, GL_INVALID, GL_INVALID ),							0x01, 0, 32, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_GEN_INT( SL2_TBITS(8, 8, 8, 8), SL2_TSHIFTS(0, 8, 16, 24), 4, true, false, false ) },
		{ SL2_ID( VK_FORMAT_A8B8G8R8_SRGB_PACK32, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_INVALID, GL_INVALID, GL_INVALID ),							0x00, 0, 32, 1, 1, 1, 1, 1, true, false, false, false, nullptr, SL2_GEN_INT( SL2_TBITS(8, 8, 8, 8), SL2_TSHIFTS(0, 8, 16, 24), 4, false, true, true ) },

		{ SL2_ID( VK_FORMAT_A2R10G10B10_UNORM_PACK32, DXGI_FORMAT_UNKNOWN, MTLPixelFormatBGR10A2Unorm, GL_RGB10_A2, GL_UNSIGNED_INT_2_10_10_10_REV, GL_BGRA ),
																																							0x01, 0, 32, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_GEN_INT( SL2_TBITS(10, 10, 10, 2), SL2_TSHIFTS(20, 10, 0, 30), 4, false, true, false ) },
		{ SL2_ID( VK_FORMAT_A2B10G10R10_UNORM_PACK32, DXGI_FORMAT_R10G10B10A2_UNORM, MTLPixelFormatRGB10A2Unorm, GL_RGB10_A2, GL_UNSIGNED_INT_2_10_10_10_REV, GL_RGBA ),
																																							0x01, 0, 32, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_GEN_INT( SL2_TBITS(10, 10, 10, 2), SL2_TSHIFTS(0, 10, 20, 30), 4, false, true, false ) },
		{ SL2_ID( VK_FORMAT_A2R10G10B10_SNORM_PACK32, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_INVALID, GL_INVALID, GL_INVALID ),						0x01, 0, 32, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_GEN_INT( SL2_TBITS(10, 10, 10, 2), SL2_TSHIFTS(20, 10, 0, 30), 4, true, true, false ) },
		{ SL2_ID( VK_FORMAT_A2B10G10R10_SNORM_PACK32, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_INVALID, GL_INVALID, GL_INVALID ),						0x01, 0, 32, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_GEN_INT( SL2_TBITS(10, 10, 10, 2), SL2_TSHIFTS(0, 10, 20, 30), 4, true, true, false ) },


		{ SL2_ID( VK_FORMAT_A2R10G10B10_UINT_PACK32, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_RGB10_A2UI, GL_UNSIGNED_INT_2_10_10_10_REV, GL_BGRA_INTEGER ),
																																							0x01, 0, 32, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_GEN_INT( SL2_TBITS(10, 10, 10, 2), SL2_TSHIFTS(20, 10, 0, 30), 4, false, false, false ) },
		{ SL2_ID( VK_FORMAT_A2B10G10R10_UINT_PACK32, DXGI_FORMAT_R10G10B10A2_UINT, MTLPixelFormatRGB10A2Uint, GL_RGB10_A2UI, GL_UNSIGNED_INT_2_10_10_10_REV, GL_RGBA_INTEGER ),
																																							0x01, 0, 32, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_GEN_INT( SL2_TBITS(10, 10, 10, 2), SL2_TSHIFTS(0, 10, 20, 30), 4, false, false, false ) },
		{ SL2_ID( VK_FORMAT_A2R10G10B10_SINT_PACK32, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_INVALID, GL_INVALID, GL_INVALID ),						0x01, 0, 32, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_GEN_INT( SL2_TBITS(10, 10, 10, 2), SL2_TSHIFTS(20, 10, 0, 30), 4, true, false, false ) },
		{ SL2_ID( VK_FORMAT_A2B10G10R10_SINT_PACK32, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_INVALID, GL_INVALID, GL_INVALID ),						0x01, 0, 32, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_GEN_INT( SL2_TBITS(10, 10, 10, 2), SL2_TSHIFTS(0, 10, 20, 30), 4, true, false, false ) },




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
		
		{ SL2_ID( VK_FORMAT_BC1_RGB_UNORM_BLOCK, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_COMPRESSED_RGB_S3TC_DXT1_EXT, GL_UNSIGNED_BYTE, GL_RGB ),	0x02 | SL2_MAKE_COMP_FLAG( SL2_CS_BC ),
																																							0, 64, 4, 4, 1, 1, 1, false, true, false, false, CFormat::GetCompressedSizeBc, SL2_TBITS(5, 6, 5, 0), SL2_TSHIFTS(0, 0, 0, 0), CFormat::Dxt1ToRgba64F<false>, CFormat::Dxt1FromRgba64F<0, false> },
		{ SL2_ID( VK_FORMAT_BC1_RGBA_UNORM_BLOCK, DXGI_FORMAT_BC1_UNORM, MTLPixelFormatBC1_RGBA, GL_COMPRESSED_RGBA_S3TC_DXT1_EXT, GL_UNSIGNED_BYTE, GL_RGBA ),	0x02 | SL2_MAKE_COMP_FLAG( SL2_CS_BC ),
																																							0, 64, 4, 4, 1, 1, 1, false, true, false, false, CFormat::GetCompressedSizeBc, SL2_TBITS(5, 6, 5, 1), SL2_TSHIFTS(0, 0, 0, 0), CFormat::Dxt1ToRgba64F<false>, CFormat::Dxt1FromRgba64F<128, false> },

		{ SL2_ID( VK_FORMAT_BC2_UNORM_BLOCK, DXGI_FORMAT_BC2_UNORM, MTLPixelFormatBC2_RGBA, GL_COMPRESSED_RGBA_S3TC_DXT3_EXT, GL_UNSIGNED_BYTE, GL_RGBA ),	0x02 | SL2_MAKE_COMP_FLAG( SL2_CS_BC ),
																																							0, 128, 4, 4, 1, 1, 1, false, true, false, false, CFormat::GetCompressedSizeBc, SL2_TBITS(5, 6, 5, 4), SL2_TSHIFTS(0, 0, 0, 0), CFormat::Dxt3ToRgba64F<false>, CFormat::Dxt3FromRgba64F<false> },
		{ SL2_ID( VK_FORMAT_BC3_UNORM_BLOCK, DXGI_FORMAT_BC3_UNORM, MTLPixelFormatBC3_RGBA, GL_COMPRESSED_RGBA_S3TC_DXT5_EXT, GL_UNSIGNED_BYTE, GL_RGBA ),	0x02 | SL2_MAKE_COMP_FLAG( SL2_CS_BC ),
																																							0, 128, 4, 4, 1, 1, 1, false, true, false, false, CFormat::GetCompressedSizeBc, SL2_TBITS(5, 6, 5, 4), SL2_TSHIFTS(0, 0, 0, 0), CFormat::Dxt5ToRgba64F<false>, CFormat::Dxt5FromRgba64F<false> },
		{ SL2_ID( VK_FORMAT_BC1_RGB_SRGB_BLOCK, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_COMPRESSED_RGB_S3TC_DXT1_EXT, GL_UNSIGNED_BYTE, GL_RGB ),	0x02 | SL2_MAKE_COMP_FLAG( SL2_CS_BC ),
																																							0, 64, 4, 4, 1, 1, 1, true, true, false, false, CFormat::GetCompressedSizeBc, SL2_TBITS(5, 6, 5, 0), SL2_TSHIFTS(0, 0, 0, 0), CFormat::Dxt1ToRgba64F<true>, CFormat::Dxt1FromRgba64F<0, true> },
		{ SL2_ID( VK_FORMAT_BC1_RGBA_SRGB_BLOCK, DXGI_FORMAT_BC1_UNORM_SRGB, MTLPixelFormatBC1_RGBA_sRGB, GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT, GL_UNSIGNED_BYTE, GL_RGBA ), 0x02 | SL2_MAKE_COMP_FLAG( SL2_CS_BC ),
																																							0, 64, 4, 4, 1, 1, 1, true, true, false, false, CFormat::GetCompressedSizeBc, SL2_TBITS(5, 6, 5, 1), SL2_TSHIFTS(0, 0, 0, 0), CFormat::Dxt1ToRgba64F<true>, CFormat::Dxt1FromRgba64F<128, true> },
		{ SL2_ID( VK_FORMAT_BC2_SRGB_BLOCK, DXGI_FORMAT_BC2_UNORM_SRGB, MTLPixelFormatBC2_RGBA_sRGB, GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT, GL_UNSIGNED_BYTE, GL_RGBA ),	0x02 | SL2_MAKE_COMP_FLAG( SL2_CS_BC ),
																																							0, 128, 4, 4, 1, 1, 1, true, true, false, false, CFormat::GetCompressedSizeBc, SL2_TBITS(5, 6, 5, 4), SL2_TSHIFTS(0, 0, 0, 0), CFormat::Dxt3ToRgba64F<true>, CFormat::Dxt3FromRgba64F<true> },
		{ SL2_ID( VK_FORMAT_BC3_SRGB_BLOCK, DXGI_FORMAT_BC3_UNORM_SRGB, MTLPixelFormatBC3_RGBA_sRGB, GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT, GL_UNSIGNED_BYTE, GL_RGBA ),	0x02 | SL2_MAKE_COMP_FLAG( SL2_CS_BC ),
																																							0, 128, 4, 4, 1, 1, 1, true, true, false, false, CFormat::GetCompressedSizeBc, SL2_TBITS(5, 6, 5, 4), SL2_TSHIFTS(0, 0, 0, 0), CFormat::Dxt5ToRgba64F<true>, CFormat::Dxt5FromRgba64F<true> },
		
		{ SL2_ID( VK_FORMAT_UNDEFINED, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_COMPRESSED_LUMINANCE_LATC1_EXT, GL_UNSIGNED_BYTE, GL_RED ),	0x02 | SL2_MAKE_COMP_FLAG( SL2_CS_BC ),
																																							0, 64, 4, 4, 1, 1, 1, false, true, false, false, CFormat::GetCompressedSizeBc, SL2_TBITS(8, 0, 0, 0), SL2_TSHIFTS(0, 0, 0, 0), CFormat::Bc4uToRgba64F<false, true>, CFormat::Bc4uFromRgba64F<false, true> },
		{ SL2_ID( VK_FORMAT_UNDEFINED, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_COMPRESSED_LUMINANCE_ALPHA_LATC2_EXT, GL_UNSIGNED_BYTE, GL_RG ),	0x02 | SL2_MAKE_COMP_FLAG( SL2_CS_BC ),
																																							0, 128, 4, 4, 1, 1, 1, false, true, false, false, CFormat::GetCompressedSizeBc, SL2_TBITS(8, 0, 0, 8), SL2_TSHIFTS(0, 0, 0, 8), CFormat::Bc5uToRgba64F<false, true>, CFormat::Bc5uFromRgba64F<false, true> },
		{ SL2_ID( VK_FORMAT_UNDEFINED, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_COMPRESSED_SIGNED_LUMINANCE_LATC1_EXT, GL_UNSIGNED_BYTE, GL_RED ),	0x02 | SL2_MAKE_COMP_FLAG( SL2_CS_BC ),
																																							0, 64, 4, 4, 1, 1, 1, false, true, false, false, CFormat::GetCompressedSizeBc, SL2_TBITS(8, 0, 0, 0), SL2_TSHIFTS(0, 0, 0, 0), CFormat::Bc4sToRgba64F<false, true>, CFormat::Bc4sFromRgba64F<false, true> },
		{ SL2_ID( VK_FORMAT_UNDEFINED, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_COMPRESSED_SIGNED_LUMINANCE_ALPHA_LATC2_EXT, GL_UNSIGNED_BYTE, GL_RG ),	0x02 | SL2_MAKE_COMP_FLAG( SL2_CS_BC ),
																																							0, 128, 4, 4, 1, 1, 1, false, true, false, false, CFormat::GetCompressedSizeBc, SL2_TBITS(8, 0, 0, 8), SL2_TSHIFTS(0, 0, 0, 8), CFormat::Bc5sToRgba64F<false, true>, CFormat::Bc5sFromRgba64F<false, true> },
		{ SL2_ID( VK_FORMAT_BC4_UNORM_BLOCK, DXGI_FORMAT_BC4_UNORM, MTLPixelFormatBC4_RUnorm, GL_COMPRESSED_RED_RGTC1, GL_UNSIGNED_BYTE, GL_RED ),	0x02 | SL2_MAKE_COMP_FLAG( SL2_CS_BC ),
																																							0, 64, 4, 4, 1, 1, 1, false, true, false, false, CFormat::GetCompressedSizeBc, SL2_TBITS(8, 0, 0, 0), SL2_TSHIFTS(0, 0, 0, 0), CFormat::Bc4uToRgba64F<false, false>, CFormat::Bc4uFromRgba64F<false, false> },
		{ SL2_ID( VK_FORMAT_BC5_UNORM_BLOCK, DXGI_FORMAT_BC5_UNORM, MTLPixelFormatBC5_RGUnorm, GL_COMPRESSED_RG_RGTC2, GL_UNSIGNED_BYTE, GL_RG ),	0x02 | SL2_MAKE_COMP_FLAG( SL2_CS_BC ),
																																							0, 128, 4, 4, 1, 1, 1, false, true, false, false, CFormat::GetCompressedSizeBc, SL2_TBITS(8, 8, 0, 0), SL2_TSHIFTS(0, 8, 0, 0), CFormat::Bc5uToRgba64F<false, false>, CFormat::Bc5uFromRgba64F<false, false> },
		{ SL2_ID( VK_FORMAT_BC4_SNORM_BLOCK, DXGI_FORMAT_BC4_SNORM, MTLPixelFormatBC4_RSnorm, GL_COMPRESSED_SIGNED_RED_RGTC1, GL_UNSIGNED_BYTE, GL_RED ),	0x02 | SL2_MAKE_COMP_FLAG( SL2_CS_BC ),
																																							0, 64, 4, 4, 1, 1, 1, false, true, false, false, CFormat::GetCompressedSizeBc, SL2_TBITS(8, 0, 0, 0), SL2_TSHIFTS(0, 0, 0, 0), CFormat::Bc4sToRgba64F<false, false>, CFormat::Bc4sFromRgba64F<false, false> },
		{ SL2_ID( VK_FORMAT_BC5_SNORM_BLOCK, DXGI_FORMAT_BC5_SNORM, MTLPixelFormatBC5_RGSnorm, GL_COMPRESSED_SIGNED_RG_RGTC2, GL_UNSIGNED_BYTE, GL_RG ),	0x02 | SL2_MAKE_COMP_FLAG( SL2_CS_BC ),
																																							0, 128, 4, 4, 1, 1, 1, false, true, false, false, CFormat::GetCompressedSizeBc, SL2_TBITS(8, 8, 0, 0), SL2_TSHIFTS(0, 8, 0, 0), CFormat::Bc5sToRgba64F<false, false>, CFormat::Bc5sFromRgba64F<false, false> },
		
		//{ SL2_KIF_GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT, SL2_KT_GL_FLOAT, SL2_KBIF_GL_RGB, 0x02, 0, 128, 4, 4, 1, 1, 1, false, true, false, nullptr, /**/nullptr, nullptr /*CFormat::Bc6hToRgba64F<false>, CFormat::Bc6hFromRgba64F<false>*/ },
		{ SL2_ID( VK_FORMAT_BC6H_UFLOAT_BLOCK, DXGI_FORMAT_BC6H_UF16, MTLPixelFormatBC6H_RGBUfloat, GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT, GL_FLOAT, GL_RGB ),	0x02 | SL2_MAKE_COMP_FLAG( SL2_CS_BC ),
																																							0, 128, 4, 4, 1, 1, 1, false, true, false, false, CFormat::GetCompressedSizeBc, SL2_TBITS(16, 16, 16, 0), SL2_TSHIFTS(0, 0, 0, 0), CFormat::Bc6hToRgba64F<false>, CFormat::Bc6hFromRgba64F<false> },

		{ SL2_ID( VK_FORMAT_BC7_UNORM_BLOCK, DXGI_FORMAT_BC7_UNORM, MTLPixelFormatBC7_RGBAUnorm, GL_COMPRESSED_RGBA_BPTC_UNORM, GL_UNSIGNED_BYTE, GL_RGBA ),	0x02 | SL2_MAKE_COMP_FLAG( SL2_CS_BC ),
																																							0, 128, 4, 4, 1, 1, 1, false, true, false, false, CFormat::GetCompressedSizeBc, SL2_TBITS(8, 8, 8, 8), SL2_TSHIFTS(0, 0, 0, 0), CFormat::Bc7uToRgba64F<false>, CFormat::Bc7uFromRgba64F<false> },
		{ SL2_ID( VK_FORMAT_BC7_SRGB_BLOCK, DXGI_FORMAT_BC7_UNORM_SRGB, MTLPixelFormatBC7_RGBAUnorm_sRGB, GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM, GL_UNSIGNED_BYTE, GL_RGBA ),	0x02 | SL2_MAKE_COMP_FLAG( SL2_CS_BC ),
																																							0, 128, 4, 4, 1, 1, 1, false, true, false, false, CFormat::GetCompressedSizeBc, SL2_TBITS(8, 8, 8, 8), SL2_TSHIFTS(0, 0, 0, 0), CFormat::Bc7uToRgba64F<true>, CFormat::Bc7uFromRgba64F<true> },
		{ SL2_ID( VK_FORMAT_UNDEFINED, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_ETC1_RGB8_OES, GL_UNSIGNED_BYTE, GL_RGB ),	0x02 | SL2_MAKE_COMP_FLAG( SL2_CS_ETC ),
																																							0, 64, 4, 4, 1, 1, 1, false, true, false, false, CFormat::GetCompressedSizeEtc, SL2_TBITS(8, 8, 8, 8), SL2_TSHIFTS(0, 0, 0, 0), CFormat::Etc1ToRgba64F, CFormat::Etc1FromRgba64F },
		{ SL2_ID( VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK, DXGI_FORMAT_UNKNOWN, MTLPixelFormatETC2_RGB8, GL_COMPRESSED_RGB8_ETC2, GL_UNSIGNED_BYTE, GL_RGB ),	0x02 | SL2_MAKE_COMP_FLAG( SL2_CS_ETC ),
																																							0, 64, 4, 4, 1, 1, 1, false, true, false, false, CFormat::GetCompressedSizeEtc, SL2_TBITS(8, 8, 8, 0), SL2_TSHIFTS(0, 0, 0, 0), CFormat::Etc2ToRgba64F<false>, CFormat::Etc2FromRgba64F<false> },
		{ SL2_ID( VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK, DXGI_FORMAT_UNKNOWN, MTLPixelFormatETC2_RGB8A1, GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2, GL_UNSIGNED_BYTE, GL_RGBA ),	0x02 | SL2_MAKE_COMP_FLAG( SL2_CS_ETC ),
																																							0, 64, 4, 4, 1, 1, 1, false, true, false, false, CFormat::GetCompressedSizeEtc, SL2_TBITS(8, 8, 8, 1), SL2_TSHIFTS(0, 0, 0, 0), CFormat::Etc2A1ToRgba64F<false>, CFormat::Etc2A1FromRgba64F<false> },
		{ SL2_ID( VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK, DXGI_FORMAT_UNKNOWN, MTLPixelFormatEAC_RGBA8, GL_COMPRESSED_RGBA8_ETC2_EAC, GL_UNSIGNED_BYTE, GL_RGBA ),	0x02 | SL2_MAKE_COMP_FLAG( SL2_CS_ETC ),
																																							0, 128, 4, 4, 1, 1, 1, false, true, false, false, CFormat::GetCompressedSizeEtc, SL2_TBITS(8, 8, 8, 8), SL2_TSHIFTS(0, 0, 0, 0), CFormat::Etc2EacToRgba64F<false>, CFormat::Etc2EacFromRgba64F<false> },
		{ SL2_ID( VK_FORMAT_ETC2_R8G8B8_SRGB_BLOCK, DXGI_FORMAT_UNKNOWN, MTLPixelFormatETC2_RGB8_sRGB, GL_COMPRESSED_SRGB8_ETC2, GL_UNSIGNED_BYTE, GL_RGB ),	0x02 | SL2_MAKE_COMP_FLAG( SL2_CS_ETC ),
																																							0, 64, 4, 4, 1, 1, 1, true, true, false, false, CFormat::GetCompressedSizeEtc, SL2_TBITS(8, 8, 8, 0), SL2_TSHIFTS(0, 0, 0, 0), CFormat::Etc2ToRgba64F<true>, CFormat::Etc2FromRgba64F<true> },
		{ SL2_ID( VK_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK, DXGI_FORMAT_UNKNOWN, MTLPixelFormatETC2_RGB8A1_sRGB, GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2, GL_UNSIGNED_BYTE, GL_RGBA ),	0x02 | SL2_MAKE_COMP_FLAG( SL2_CS_ETC ),
																																							0, 64, 4, 4, 1, 1, 1, true, true, false, false, CFormat::GetCompressedSizeEtc, SL2_TBITS(8, 8, 8, 1), SL2_TSHIFTS(0, 0, 0, 0), CFormat::Etc2A1ToRgba64F<true>, CFormat::Etc2A1FromRgba64F<true> },
		{ SL2_ID( VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK, DXGI_FORMAT_UNKNOWN, MTLPixelFormatEAC_RGBA8_sRGB, GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC, GL_UNSIGNED_BYTE, GL_RGBA ),	0x02 | SL2_MAKE_COMP_FLAG( SL2_CS_ETC ),
																																							0, 128, 4, 4, 1, 1, 1, true, true, false, false, CFormat::GetCompressedSizeEtc, SL2_TBITS(8, 8, 8, 8), SL2_TSHIFTS(0, 0, 0, 0), CFormat::Etc2EacToRgba64F<true>, CFormat::Etc2EacFromRgba64F<true> },
																																								
		{ SL2_ID( VK_FORMAT_EAC_R11_UNORM_BLOCK, DXGI_FORMAT_UNKNOWN, MTLPixelFormatEAC_R11Unorm, GL_COMPRESSED_R11_EAC, GL_UNSIGNED_BYTE, GL_RED ),	0x02 | SL2_MAKE_COMP_FLAG( SL2_CS_ETC ),
																																							0, 64, 4, 4, 1, 1, 1, false, true, false, false, CFormat::GetCompressedSizeEtc, SL2_TBITS(11, 0, 0, 0), SL2_TSHIFTS(0, 0, 0, 0), CFormat::R11EacToRgba64F<false>, CFormat::R11EacFromRgba64F<false> },
		{ SL2_ID( VK_FORMAT_EAC_R11G11_UNORM_BLOCK, DXGI_FORMAT_UNKNOWN, MTLPixelFormatEAC_RG11Unorm, GL_COMPRESSED_RG11_EAC, GL_UNSIGNED_BYTE, GL_RG ),	0x02 | SL2_MAKE_COMP_FLAG( SL2_CS_ETC ),
																																							0, 128, 4, 4, 1, 1, 1, false, true, false, false, CFormat::GetCompressedSizeEtc, SL2_TBITS(11, 11, 0, 0), SL2_TSHIFTS(0, 0, 0, 0), CFormat::RG11EacToRgba64F<false>, CFormat::RG11EacFromRgba64F<false> },
		{ SL2_ID( VK_FORMAT_EAC_R11_SNORM_BLOCK, DXGI_FORMAT_UNKNOWN, MTLPixelFormatEAC_R11Snorm, GL_COMPRESSED_SIGNED_R11_EAC, GL_UNSIGNED_BYTE, GL_RED ),	0x02 | SL2_MAKE_COMP_FLAG( SL2_CS_ETC ),
																																							0, 64, 4, 4, 1, 1, 1, true, true, false, false, CFormat::GetCompressedSizeEtc, SL2_TBITS(11, 0, 0, 0), SL2_TSHIFTS(0, 0, 0, 0), CFormat::R11EacToRgba64F<true>, CFormat::R11EacFromRgba64F<true> },
		{ SL2_ID( VK_FORMAT_EAC_R11G11_SNORM_BLOCK, DXGI_FORMAT_UNKNOWN, MTLPixelFormatEAC_RG11Snorm, GL_COMPRESSED_SIGNED_RG11_EAC, GL_UNSIGNED_BYTE, GL_RG ),	0x02 | SL2_MAKE_COMP_FLAG( SL2_CS_ETC ),
																																							0, 128, 4, 4, 1, 1, 1, false, true, false, false, CFormat::GetCompressedSizeEtc, SL2_TBITS(11, 11, 0, 0), SL2_TSHIFTS(0, 0, 0, 0), CFormat::RG11EacToRgba64F<true>, CFormat::RG11EacFromRgba64F<true> },

		{ SL2_ID( VK_FORMAT_UNDEFINED, DXGI_FORMAT_UNKNOWN, MTLPixelFormatPVRTC_RGB_2BPP, GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG, GL_UNSIGNED_BYTE, GL_RGB ),	0x02 | SL2_MAKE_COMP_FLAG( SL2_CS_PVRTC ),
																																							0, 64, 8, 4, 1, 1, 1, false, true, false, false, &(GetCompressedSizePvrtc<8, 4>), SL2_TBITS(8, 8, 8, 0), SL2_TSHIFTS(0, 0, 0, 0), CFormat::PvrtcToRgba64F<PVRTLPF_PVRTCI_2bpp_RGB, false>, CFormat::PvrtcFromRgba64F<PVRTLPF_PVRTCI_2bpp_RGB, false> },
		{ SL2_ID( VK_FORMAT_UNDEFINED, DXGI_FORMAT_UNKNOWN, MTLPixelFormatPVRTC_RGB_4BPP, GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG, GL_UNSIGNED_BYTE, GL_RGB ),	0x02 | SL2_MAKE_COMP_FLAG( SL2_CS_PVRTC ),
																																							0, 64, 4, 4, 1, 1, 1, false, true, false, false, &(GetCompressedSizePvrtc<4, 4>), SL2_TBITS(8, 8, 8, 0), SL2_TSHIFTS(0, 0, 0, 0), CFormat::PvrtcToRgba64F<PVRTLPF_PVRTCI_4bpp_RGB, false>, CFormat::PvrtcFromRgba64F<PVRTLPF_PVRTCI_4bpp_RGB, false> },
		{ SL2_ID( VK_FORMAT_PVRTC1_2BPP_UNORM_BLOCK_IMG, DXGI_FORMAT_UNKNOWN, MTLPixelFormatPVRTC_RGBA_2BPP, GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG, GL_UNSIGNED_BYTE, GL_RGBA ),	0x02 | SL2_MAKE_COMP_FLAG( SL2_CS_PVRTC ),
																																							0, 64, 8, 4, 1, 1, 1, false, true, false, false, &(GetCompressedSizePvrtc<8, 4>), SL2_TBITS(8, 8, 8, 8), SL2_TSHIFTS(0, 0, 0, 0), CFormat::PvrtcToRgba64F<PVRTLPF_PVRTCI_2bpp_RGBA, false>, CFormat::PvrtcFromRgba64F<PVRTLPF_PVRTCI_2bpp_RGBA, false> },
		{ SL2_ID( VK_FORMAT_PVRTC1_4BPP_UNORM_BLOCK_IMG, DXGI_FORMAT_UNKNOWN, MTLPixelFormatPVRTC_RGBA_4BPP, GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG, GL_UNSIGNED_BYTE, GL_RGBA ),	0x02 | SL2_MAKE_COMP_FLAG( SL2_CS_PVRTC ),
																																							0, 64, 4, 4, 1, 1, 1, false, true, false, false, &(GetCompressedSizePvrtc<4, 4>), SL2_TBITS(8, 8, 8, 8), SL2_TSHIFTS(0, 0, 0, 0), CFormat::PvrtcToRgba64F<PVRTLPF_PVRTCI_4bpp_RGBA, false>, CFormat::PvrtcFromRgba64F<PVRTLPF_PVRTCI_4bpp_RGBA, false> },
		{ SL2_ID( VK_FORMAT_PVRTC2_2BPP_UNORM_BLOCK_IMG, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_COMPRESSED_RGBA_PVRTC_2BPPV2_IMG, GL_UNSIGNED_BYTE, GL_RGBA ),	0x02 | SL2_MAKE_COMP_FLAG( SL2_CS_PVRTC ),
																																							0, 64, 8, 4, 1, 1, 1, false, true, false, false, &(GetCompressedSizePvrtc<8, 4>), SL2_TBITS(8, 8, 8, 8), SL2_TSHIFTS(0, 0, 0, 0), CFormat::PvrtcToRgba64F<PVRTLPF_PVRTCII_2bpp, false>, CFormat::PvrtcFromRgba64F<PVRTLPF_PVRTCII_2bpp, false> },
		{ SL2_ID( VK_FORMAT_PVRTC2_4BPP_UNORM_BLOCK_IMG, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_COMPRESSED_RGBA_PVRTC_4BPPV2_IMG, GL_UNSIGNED_BYTE, GL_RGBA ),	0x02 | SL2_MAKE_COMP_FLAG( SL2_CS_PVRTC ),
																																							0, 64, 4, 4, 1, 1, 1, false, true, false, false, &(GetCompressedSizePvrtc<4, 4>), SL2_TBITS(8, 8, 8, 8), SL2_TSHIFTS(0, 0, 0, 0), CFormat::PvrtcToRgba64F<PVRTLPF_PVRTCII_4bpp, false>, CFormat::PvrtcFromRgba64F<PVRTLPF_PVRTCII_4bpp, false> },
		{ SL2_ID( VK_FORMAT_UNDEFINED, DXGI_FORMAT_UNKNOWN, MTLPixelFormatPVRTC_RGB_2BPP_sRGB, GL_COMPRESSED_SRGB_PVRTC_2BPPV1_EXT, GL_UNSIGNED_BYTE, GL_RGB ),	0x02 | SL2_MAKE_COMP_FLAG( SL2_CS_PVRTC ),
																																							0, 64, 8, 4, 1, 1, 1, true, true, false, false, &(GetCompressedSizePvrtc<8, 4>), SL2_TBITS(8, 8, 8, 0), SL2_TSHIFTS(0, 0, 0, 0), CFormat::PvrtcToRgba64F<PVRTLPF_PVRTCI_2bpp_RGB, true>, CFormat::PvrtcFromRgba64F<PVRTLPF_PVRTCI_2bpp_RGB, true> },
		{ SL2_ID( VK_FORMAT_UNDEFINED, DXGI_FORMAT_UNKNOWN, MTLPixelFormatPVRTC_RGB_4BPP_sRGB, GL_COMPRESSED_SRGB_PVRTC_4BPPV1_EXT, GL_UNSIGNED_BYTE, GL_RGB ),	0x02 | SL2_MAKE_COMP_FLAG( SL2_CS_PVRTC ),
																																							0, 64, 4, 4, 1, 1, 1, true, true, false, false, &(GetCompressedSizePvrtc<4, 4>), SL2_TBITS(8, 8, 8, 0), SL2_TSHIFTS(0, 0, 0, 0), CFormat::PvrtcToRgba64F<PVRTLPF_PVRTCI_4bpp_RGB, true>, CFormat::PvrtcFromRgba64F<PVRTLPF_PVRTCI_4bpp_RGB, true> },
		{ SL2_ID( VK_FORMAT_PVRTC1_2BPP_SRGB_BLOCK_IMG, DXGI_FORMAT_UNKNOWN, MTLPixelFormatPVRTC_RGBA_2BPP_sRGB, GL_COMPRESSED_SRGB_ALPHA_PVRTC_2BPPV1_EXT, GL_UNSIGNED_BYTE, GL_RGBA ),	0x02 | SL2_MAKE_COMP_FLAG( SL2_CS_PVRTC ),
																																							0, 64, 8, 4, 1, 1, 1, true, true, false, false, &(GetCompressedSizePvrtc<8, 4>), SL2_TBITS(8, 8, 8, 8), SL2_TSHIFTS(0, 0, 0, 0), CFormat::PvrtcToRgba64F<PVRTLPF_PVRTCI_2bpp_RGBA, true>, CFormat::PvrtcFromRgba64F<PVRTLPF_PVRTCI_2bpp_RGBA, true> },
		{ SL2_ID( VK_FORMAT_PVRTC1_4BPP_SRGB_BLOCK_IMG, DXGI_FORMAT_UNKNOWN, MTLPixelFormatPVRTC_RGBA_4BPP_sRGB, GL_COMPRESSED_SRGB_ALPHA_PVRTC_4BPPV1_EXT, GL_UNSIGNED_BYTE, GL_RGBA ),	0x02 | SL2_MAKE_COMP_FLAG( SL2_CS_PVRTC ),
																																							0, 64, 4, 4, 1, 1, 1, true, true, false, false, &(GetCompressedSizePvrtc<4, 4>), SL2_TBITS(8, 8, 8, 8), SL2_TSHIFTS(0, 0, 0, 0), CFormat::PvrtcToRgba64F<PVRTLPF_PVRTCI_4bpp_RGBA, true>, CFormat::PvrtcFromRgba64F<PVRTLPF_PVRTCI_4bpp_RGBA, true> },
		{ SL2_ID( VK_FORMAT_PVRTC2_2BPP_SRGB_BLOCK_IMG, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_COMPRESSED_SRGB_ALPHA_PVRTC_2BPPV2_IMG, GL_UNSIGNED_BYTE, GL_RGBA ),	0x02 | SL2_MAKE_COMP_FLAG( SL2_CS_PVRTC ),
																																							0, 64, 8, 4, 1, 1, 1, true, true, false, false, &(GetCompressedSizePvrtc<8, 4>), SL2_TBITS(8, 8, 8, 8), SL2_TSHIFTS(0, 0, 0, 0), CFormat::PvrtcToRgba64F<PVRTLPF_PVRTCII_2bpp, true>, CFormat::PvrtcFromRgba64F<PVRTLPF_PVRTCII_2bpp, true> },
		{ SL2_ID( VK_FORMAT_PVRTC2_4BPP_SRGB_BLOCK_IMG, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_COMPRESSED_SRGB_ALPHA_PVRTC_4BPPV2_IMG, GL_UNSIGNED_BYTE, GL_RGBA ),	0x02 | SL2_MAKE_COMP_FLAG( SL2_CS_PVRTC ),
																																							0, 64, 4, 4, 1, 1, 1, true, true, false, false, &(GetCompressedSizePvrtc<4, 4>), SL2_TBITS(8, 8, 8, 8), SL2_TSHIFTS(0, 0, 0, 0), CFormat::PvrtcToRgba64F<PVRTLPF_PVRTCII_4bpp, true>, CFormat::PvrtcFromRgba64F<PVRTLPF_PVRTCII_4bpp, true> },


		{ SL2_ID( VK_FORMAT_ASTC_4x4_UNORM_BLOCK, DXGI_FORMAT_UNKNOWN, MTLPixelFormatASTC_4x4_LDR, GL_COMPRESSED_RGBA_ASTC_4x4_KHR, GL_UNSIGNED_BYTE, GL_RGBA ),	0x02 | SL2_MAKE_COMP_FLAG( SL2_CS_ASTC ),
																																							0, 128, 4, 4, 1, 1, 1, false, true, false, false, &(GetCompressedSizeAstc<4, 4>), SL2_TBITS(8, 8, 8, 8), SL2_TSHIFTS(0, 0, 0, 0), CFormat::AstcToRgba64F<4, 4, false>, CFormat::AstcFromRgba64F<4, 4, false> },
		{ SL2_ID( VK_FORMAT_ASTC_5x4_UNORM_BLOCK, DXGI_FORMAT_UNKNOWN, MTLPixelFormatASTC_5x4_LDR, GL_COMPRESSED_RGBA_ASTC_5x4_KHR, GL_UNSIGNED_BYTE, GL_RGBA ),	0x02 | SL2_MAKE_COMP_FLAG( SL2_CS_ASTC ),
																																							0, 128, 5, 4, 1, 1, 1, false, true, false, false, &(GetCompressedSizeAstc<5, 4>), SL2_TBITS(8, 8, 8, 8), SL2_TSHIFTS(0, 0, 0, 0), CFormat::AstcToRgba64F<5, 4, false>, CFormat::AstcFromRgba64F<5, 4, false> },
		{ SL2_ID( VK_FORMAT_ASTC_5x5_UNORM_BLOCK, DXGI_FORMAT_UNKNOWN, MTLPixelFormatASTC_5x5_LDR, GL_COMPRESSED_RGBA_ASTC_5x5_KHR, GL_UNSIGNED_BYTE, GL_RGBA ),	0x02 | SL2_MAKE_COMP_FLAG( SL2_CS_ASTC ),
																																							0, 128, 5, 5, 1, 1, 1, false, true, false, false, &(GetCompressedSizeAstc<5, 5>), SL2_TBITS(8, 8, 8, 8), SL2_TSHIFTS(0, 0, 0, 0), CFormat::AstcToRgba64F<5, 5, false>, CFormat::AstcFromRgba64F<5, 5, false> },
		{ SL2_ID( VK_FORMAT_ASTC_6x5_UNORM_BLOCK, DXGI_FORMAT_UNKNOWN, MTLPixelFormatASTC_6x5_LDR, GL_COMPRESSED_RGBA_ASTC_6x5_KHR, GL_UNSIGNED_BYTE, GL_RGBA ),	0x02 | SL2_MAKE_COMP_FLAG( SL2_CS_ASTC ),
																																							0, 128, 6, 5, 1, 1, 1, false, true, false, false, &(GetCompressedSizeAstc<6, 5>), SL2_TBITS(8, 8, 8, 8), SL2_TSHIFTS(0, 0, 0, 0), CFormat::AstcToRgba64F<6, 5, false>, CFormat::AstcFromRgba64F<6, 5, false> },
		{ SL2_ID( VK_FORMAT_ASTC_6x6_UNORM_BLOCK, DXGI_FORMAT_UNKNOWN, MTLPixelFormatASTC_6x6_LDR, GL_COMPRESSED_RGBA_ASTC_6x6_KHR, GL_UNSIGNED_BYTE, GL_RGBA ),	0x02 | SL2_MAKE_COMP_FLAG( SL2_CS_ASTC ),
																																							0, 128, 6, 6, 1, 1, 1, false, true, false, false, &(GetCompressedSizeAstc<6, 6>), SL2_TBITS(8, 8, 8, 8), SL2_TSHIFTS(0, 0, 0, 0), CFormat::AstcToRgba64F<6, 6, false>, CFormat::AstcFromRgba64F<6, 6, false> },
		{ SL2_ID( VK_FORMAT_ASTC_8x5_UNORM_BLOCK, DXGI_FORMAT_UNKNOWN, MTLPixelFormatASTC_8x5_LDR, GL_COMPRESSED_RGBA_ASTC_8x5_KHR, GL_UNSIGNED_BYTE, GL_RGBA ),	0x02 | SL2_MAKE_COMP_FLAG( SL2_CS_ASTC ),
																																							0, 128, 8, 5, 1, 1, 1, false, true, false, false, &(GetCompressedSizeAstc<8, 5>), SL2_TBITS(8, 8, 8, 8), SL2_TSHIFTS(0, 0, 0, 0), CFormat::AstcToRgba64F<8, 5, false>, CFormat::AstcFromRgba64F<8, 5, false> },
		{ SL2_ID( VK_FORMAT_ASTC_8x6_UNORM_BLOCK, DXGI_FORMAT_UNKNOWN, MTLPixelFormatASTC_8x6_LDR, GL_COMPRESSED_RGBA_ASTC_8x6_KHR, GL_UNSIGNED_BYTE, GL_RGBA ),	0x02 | SL2_MAKE_COMP_FLAG( SL2_CS_ASTC ),
																																							0, 128, 8, 6, 1, 1, 1, false, true, false, false, &(GetCompressedSizeAstc<8, 6>), SL2_TBITS(8, 8, 8, 8), SL2_TSHIFTS(0, 0, 0, 0), CFormat::AstcToRgba64F<8, 6, false>, CFormat::AstcFromRgba64F<8, 6, false> },
		{ SL2_ID( VK_FORMAT_ASTC_8x8_UNORM_BLOCK, DXGI_FORMAT_UNKNOWN, MTLPixelFormatASTC_8x8_LDR, GL_COMPRESSED_RGBA_ASTC_8x8_KHR, GL_UNSIGNED_BYTE, GL_RGBA ),	0x02 | SL2_MAKE_COMP_FLAG( SL2_CS_ASTC ),
																																							0, 128, 8, 8, 1, 1, 1, false, true, false, false, &(GetCompressedSizeAstc<8, 8>), SL2_TBITS(8, 8, 8, 8), SL2_TSHIFTS(0, 0, 0, 0), CFormat::AstcToRgba64F<8, 8, false>, CFormat::AstcFromRgba64F<8, 8, false> },
		{ SL2_ID( VK_FORMAT_ASTC_10x5_UNORM_BLOCK, DXGI_FORMAT_UNKNOWN, MTLPixelFormatASTC_10x5_LDR, GL_COMPRESSED_RGBA_ASTC_10x5_KHR, GL_UNSIGNED_BYTE, GL_RGBA ),	0x02 | SL2_MAKE_COMP_FLAG( SL2_CS_ASTC ),
																																							0, 128, 10, 5, 1, 1, 1, false, true, false, false, &(GetCompressedSizeAstc<10, 5>), SL2_TBITS(8, 8, 8, 8), SL2_TSHIFTS(0, 0, 0, 0), CFormat::AstcToRgba64F<10, 5, false>, CFormat::AstcFromRgba64F<10, 5, false> },
		{ SL2_ID( VK_FORMAT_ASTC_10x6_UNORM_BLOCK, DXGI_FORMAT_UNKNOWN, MTLPixelFormatASTC_10x6_LDR, GL_COMPRESSED_RGBA_ASTC_10x6_KHR, GL_UNSIGNED_BYTE, GL_RGBA ),	0x02 | SL2_MAKE_COMP_FLAG( SL2_CS_ASTC ),
																																							0, 128, 10, 6, 1, 1, 1, false, true, false, false, &(GetCompressedSizeAstc<10, 6>), SL2_TBITS(8, 8, 8, 8), SL2_TSHIFTS(0, 0, 0, 0), CFormat::AstcToRgba64F<10, 6, false>, CFormat::AstcFromRgba64F<10, 6, false> },
		{ SL2_ID( VK_FORMAT_ASTC_10x8_UNORM_BLOCK, DXGI_FORMAT_UNKNOWN, MTLPixelFormatASTC_10x8_LDR, GL_COMPRESSED_RGBA_ASTC_10x8_KHR, GL_UNSIGNED_BYTE, GL_RGBA ),	0x02 | SL2_MAKE_COMP_FLAG( SL2_CS_ASTC ),
																																							0, 128, 10, 8, 1, 1, 1, false, true, false, false, &(GetCompressedSizeAstc<10, 8>), SL2_TBITS(8, 8, 8, 8), SL2_TSHIFTS(0, 0, 0, 0), CFormat::AstcToRgba64F<10, 8, false>, CFormat::AstcFromRgba64F<10, 8, false> },
		{ SL2_ID( VK_FORMAT_ASTC_10x10_UNORM_BLOCK, DXGI_FORMAT_UNKNOWN, MTLPixelFormatASTC_10x10_LDR, GL_COMPRESSED_RGBA_ASTC_10x10_KHR, GL_UNSIGNED_BYTE, GL_RGBA ),	0x02 | SL2_MAKE_COMP_FLAG( SL2_CS_ASTC ),
																																							0, 128, 10, 10, 1, 1, 1, false, true, false, false, &(GetCompressedSizeAstc<10, 10>), SL2_TBITS(8, 8, 8, 8), SL2_TSHIFTS(0, 0, 0, 0), CFormat::AstcToRgba64F<10, 10, false>, CFormat::AstcFromRgba64F<10, 10, false> },
		{ SL2_ID( VK_FORMAT_ASTC_12x10_UNORM_BLOCK, DXGI_FORMAT_UNKNOWN, MTLPixelFormatASTC_12x10_LDR, GL_COMPRESSED_RGBA_ASTC_12x10_KHR, GL_UNSIGNED_BYTE, GL_RGBA ),	0x02 | SL2_MAKE_COMP_FLAG( SL2_CS_ASTC ),
																																							0, 128, 12, 10, 1, 1, 1, false, true, false, false, &(GetCompressedSizeAstc<12, 10>), SL2_TBITS(8, 8, 8, 8), SL2_TSHIFTS(0, 0, 0, 0), CFormat::AstcToRgba64F<12, 10, false>, CFormat::AstcFromRgba64F<12, 10, false> },
		{ SL2_ID( VK_FORMAT_ASTC_12x12_UNORM_BLOCK, DXGI_FORMAT_UNKNOWN, MTLPixelFormatASTC_12x12_LDR, GL_COMPRESSED_RGBA_ASTC_12x12_KHR, GL_UNSIGNED_BYTE, GL_RGBA ),	0x02 | SL2_MAKE_COMP_FLAG( SL2_CS_ASTC ),
																																							0, 128, 12, 12, 1, 1, 1, false, true, false, false, &(GetCompressedSizeAstc<12, 12>), SL2_TBITS(8, 8, 8, 8), SL2_TSHIFTS(0, 0, 0, 0), CFormat::AstcToRgba64F<12, 12, false>, CFormat::AstcFromRgba64F<12, 12, false> },


		{ SL2_ID( VK_FORMAT_ASTC_4x4_SRGB_BLOCK, DXGI_FORMAT_UNKNOWN, MTLPixelFormatASTC_4x4_sRGB, GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR, GL_UNSIGNED_BYTE, GL_RGBA ),	0x02 | SL2_MAKE_COMP_FLAG( SL2_CS_ASTC ),
																																							0, 128, 4, 4, 1, 1, 1, true, true, false, false, &(GetCompressedSizeAstc<4, 4>), SL2_TBITS(8, 8, 8, 8), SL2_TSHIFTS(0, 0, 0, 0), CFormat::AstcToRgba64F<4, 4, true>, CFormat::AstcFromRgba64F<4, 4, true> },
		{ SL2_ID( VK_FORMAT_ASTC_5x4_SRGB_BLOCK, DXGI_FORMAT_UNKNOWN, MTLPixelFormatASTC_5x4_sRGB, GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR, GL_UNSIGNED_BYTE, GL_RGBA ),	0x02 | SL2_MAKE_COMP_FLAG( SL2_CS_ASTC ),
																																							0, 128, 5, 4, 1, 1, 1, false, true, false, false, &(GetCompressedSizeAstc<5, 4>), SL2_TBITS(8, 8, 8, 8), SL2_TSHIFTS(0, 0, 0, 0), CFormat::AstcToRgba64F<5, 4, true>, CFormat::AstcFromRgba64F<5, 4, true> },
		{ SL2_ID( VK_FORMAT_ASTC_5x5_SRGB_BLOCK, DXGI_FORMAT_UNKNOWN, MTLPixelFormatASTC_5x5_sRGB, GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR, GL_UNSIGNED_BYTE, GL_RGBA ),	0x02 | SL2_MAKE_COMP_FLAG( SL2_CS_ASTC ),
																																							0, 128, 5, 5, 1, 1, 1, false, true, false, false, &(GetCompressedSizeAstc<5, 5>), SL2_TBITS(8, 8, 8, 8), SL2_TSHIFTS(0, 0, 0, 0), CFormat::AstcToRgba64F<5, 5, true>, CFormat::AstcFromRgba64F<5, 5, true> },
		{ SL2_ID( VK_FORMAT_ASTC_6x5_SRGB_BLOCK, DXGI_FORMAT_UNKNOWN, MTLPixelFormatASTC_6x5_sRGB, GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR, GL_UNSIGNED_BYTE, GL_RGBA ),	0x02 | SL2_MAKE_COMP_FLAG( SL2_CS_ASTC ),
																																							0, 128, 6, 5, 1, 1, 1, false, true, false, false, &(GetCompressedSizeAstc<6, 5>), SL2_TBITS(8, 8, 8, 8), SL2_TSHIFTS(0, 0, 0, 0), CFormat::AstcToRgba64F<6, 5, true>, CFormat::AstcFromRgba64F<6, 5, true> },
		{ SL2_ID( VK_FORMAT_ASTC_6x6_SRGB_BLOCK, DXGI_FORMAT_UNKNOWN, MTLPixelFormatASTC_6x6_sRGB, GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR, GL_UNSIGNED_BYTE, GL_RGBA ),	0x02 | SL2_MAKE_COMP_FLAG( SL2_CS_ASTC ),
																																							0, 128, 6, 6, 1, 1, 1, false, true, false, false, &(GetCompressedSizeAstc<6, 6>), SL2_TBITS(8, 8, 8, 8), SL2_TSHIFTS(0, 0, 0, 0), CFormat::AstcToRgba64F<6, 6, true>, CFormat::AstcFromRgba64F<6, 6, true> },
		{ SL2_ID( VK_FORMAT_ASTC_8x5_SRGB_BLOCK, DXGI_FORMAT_UNKNOWN, MTLPixelFormatASTC_8x5_sRGB, GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR, GL_UNSIGNED_BYTE, GL_RGBA ),	0x02 | SL2_MAKE_COMP_FLAG( SL2_CS_ASTC ),
																																							0, 128, 8, 5, 1, 1, 1, false, true, false, false, &(GetCompressedSizeAstc<8, 5>), SL2_TBITS(8, 8, 8, 8), SL2_TSHIFTS(0, 0, 0, 0), CFormat::AstcToRgba64F<8, 5, true>, CFormat::AstcFromRgba64F<8, 5, true> },
		{ SL2_ID( VK_FORMAT_ASTC_8x6_SRGB_BLOCK, DXGI_FORMAT_UNKNOWN, MTLPixelFormatASTC_8x6_sRGB, GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR, GL_UNSIGNED_BYTE, GL_RGBA ),	0x02 | SL2_MAKE_COMP_FLAG( SL2_CS_ASTC ),
																																							0, 128, 8, 6, 1, 1, 1, false, true, false, false, &(GetCompressedSizeAstc<8, 6>), SL2_TBITS(8, 8, 8, 8), SL2_TSHIFTS(0, 0, 0, 0), CFormat::AstcToRgba64F<8, 6, true>, CFormat::AstcFromRgba64F<8, 6, true> },
		{ SL2_ID( VK_FORMAT_ASTC_8x8_SRGB_BLOCK, DXGI_FORMAT_UNKNOWN, MTLPixelFormatASTC_8x8_sRGB, GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR, GL_UNSIGNED_BYTE, GL_RGBA ),	0x02 | SL2_MAKE_COMP_FLAG( SL2_CS_ASTC ),
																																							0, 128, 8, 8, 1, 1, 1, false, true, false, false, &(GetCompressedSizeAstc<8, 8>), SL2_TBITS(8, 8, 8, 8), SL2_TSHIFTS(0, 0, 0, 0), CFormat::AstcToRgba64F<8, 8, true>, CFormat::AstcFromRgba64F<8, 8, true> },
		{ SL2_ID( VK_FORMAT_ASTC_10x5_SRGB_BLOCK, DXGI_FORMAT_UNKNOWN, MTLPixelFormatASTC_10x5_sRGB, GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR, GL_UNSIGNED_BYTE, GL_RGBA ),	0x02 | SL2_MAKE_COMP_FLAG( SL2_CS_ASTC ),
																																							0, 128, 10, 5, 1, 1, 1, false, true, false, false, &(GetCompressedSizeAstc<10, 5>), SL2_TBITS(8, 8, 8, 8), SL2_TSHIFTS(0, 0, 0, 0), CFormat::AstcToRgba64F<10, 5, true>, CFormat::AstcFromRgba64F<10, 5, true> },
		{ SL2_ID( VK_FORMAT_ASTC_10x6_SRGB_BLOCK, DXGI_FORMAT_UNKNOWN, MTLPixelFormatASTC_10x6_sRGB, GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR, GL_UNSIGNED_BYTE, GL_RGBA ),	0x02 | SL2_MAKE_COMP_FLAG( SL2_CS_ASTC ),
																																							0, 128, 10, 6, 1, 1, 1, false, true, false, false, &(GetCompressedSizeAstc<10, 6>), SL2_TBITS(8, 8, 8, 8), SL2_TSHIFTS(0, 0, 0, 0), CFormat::AstcToRgba64F<10, 6, true>, CFormat::AstcFromRgba64F<10, 6, true> },
		{ SL2_ID( VK_FORMAT_ASTC_10x8_SRGB_BLOCK, DXGI_FORMAT_UNKNOWN, MTLPixelFormatASTC_10x8_sRGB, GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR, GL_UNSIGNED_BYTE, GL_RGBA ),	0x02 | SL2_MAKE_COMP_FLAG( SL2_CS_ASTC ),
																																							0, 128, 10, 8, 1, 1, 1, false, true, false, false, &(GetCompressedSizeAstc<10, 8>), SL2_TBITS(8, 8, 8, 8), SL2_TSHIFTS(0, 0, 0, 0), CFormat::AstcToRgba64F<10, 8, true>, CFormat::AstcFromRgba64F<10, 8, true> },
		{ SL2_ID( VK_FORMAT_ASTC_10x10_SRGB_BLOCK, DXGI_FORMAT_UNKNOWN, MTLPixelFormatASTC_10x10_sRGB, GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR, GL_UNSIGNED_BYTE, GL_RGBA ),	0x02 | SL2_MAKE_COMP_FLAG( SL2_CS_ASTC ),
																																							0, 128, 10, 10, 1, 1, 1, false, true, false, false, &(GetCompressedSizeAstc<10, 10>), SL2_TBITS(8, 8, 8, 8), SL2_TSHIFTS(0, 0, 0, 0), CFormat::AstcToRgba64F<10, 10, true>, CFormat::AstcFromRgba64F<10, 10, true> },
		{ SL2_ID( VK_FORMAT_ASTC_12x10_SRGB_BLOCK, DXGI_FORMAT_UNKNOWN, MTLPixelFormatASTC_12x10_sRGB, GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR, GL_UNSIGNED_BYTE, GL_RGBA ),	0x02 | SL2_MAKE_COMP_FLAG( SL2_CS_ASTC ),
																																							0, 128, 12, 10, 1, 1, 1, false, true, false, false, &(GetCompressedSizeAstc<12, 10>), SL2_TBITS(8, 8, 8, 8), SL2_TSHIFTS(0, 0, 0, 0), CFormat::AstcToRgba64F<12, 10, true>, CFormat::AstcFromRgba64F<12, 10, true> },
		{ SL2_ID( VK_FORMAT_ASTC_12x12_SRGB_BLOCK, DXGI_FORMAT_UNKNOWN, MTLPixelFormatASTC_12x12_sRGB, GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR, GL_UNSIGNED_BYTE, GL_RGBA ),	0x02 | SL2_MAKE_COMP_FLAG( SL2_CS_ASTC ),
																																							0, 128, 12, 12, 1, 1, 1, false, true, false, false, &(GetCompressedSizeAstc<12, 12>), SL2_TBITS(8, 8, 8, 8), SL2_TSHIFTS(0, 0, 0, 0), CFormat::AstcToRgba64F<12, 12, true>, CFormat::AstcFromRgba64F<12, 12, true> },
				


		{ SL2_ID( VK_FORMAT_ASTC_4x4_SFLOAT_BLOCK, DXGI_FORMAT_UNKNOWN, MTLPixelFormatASTC_4x4_HDR, GL_INVALID, GL_INVALID, GL_INVALID ),	0x02 | SL2_MAKE_COMP_FLAG( SL2_CS_ASTC ),
																																							0, 128, 4, 4, 1, 1, 1, false, true, false, true, &(GetCompressedSizeAstc<4, 4>), SL2_TBITS(16, 16, 16, 16), SL2_TSHIFTS(0, 0, 0, 0), CFormat::AstcToRgba64F<4, 4, false, true>, CFormat::AstcFromRgba64F<4, 4, false, true> },
		{ SL2_ID( VK_FORMAT_ASTC_5x4_SFLOAT_BLOCK, DXGI_FORMAT_UNKNOWN, MTLPixelFormatASTC_5x4_HDR, GL_INVALID, GL_INVALID, GL_INVALID ),	0x02 | SL2_MAKE_COMP_FLAG( SL2_CS_ASTC ),
																																							0, 128, 5, 4, 1, 1, 1, false, true, false, true, &(GetCompressedSizeAstc<5, 4>), SL2_TBITS(16, 16, 16, 16), SL2_TSHIFTS(0, 0, 0, 0), CFormat::AstcToRgba64F<5, 4, false, true>, CFormat::AstcFromRgba64F<5, 4, false, true> },
		{ SL2_ID( VK_FORMAT_ASTC_5x5_SFLOAT_BLOCK, DXGI_FORMAT_UNKNOWN, MTLPixelFormatASTC_5x5_HDR, GL_INVALID, GL_INVALID, GL_INVALID ),	0x02 | SL2_MAKE_COMP_FLAG( SL2_CS_ASTC ),
																																							0, 128, 5, 5, 1, 1, 1, false, true, false, true, &(GetCompressedSizeAstc<5, 5>), SL2_TBITS(16, 16, 16, 16), SL2_TSHIFTS(0, 0, 0, 0), CFormat::AstcToRgba64F<5, 5, false, true>, CFormat::AstcFromRgba64F<5, 5, false, true> },
		{ SL2_ID( VK_FORMAT_ASTC_6x5_SFLOAT_BLOCK, DXGI_FORMAT_UNKNOWN, MTLPixelFormatASTC_6x5_HDR, GL_INVALID, GL_INVALID, GL_INVALID ),	0x02 | SL2_MAKE_COMP_FLAG( SL2_CS_ASTC ),
																																							0, 128, 6, 5, 1, 1, 1, false, true, false, true, &(GetCompressedSizeAstc<6, 5>), SL2_TBITS(16, 16, 16, 16), SL2_TSHIFTS(0, 0, 0, 0), CFormat::AstcToRgba64F<6, 5, false, true>, CFormat::AstcFromRgba64F<6, 5, false, true> },
		{ SL2_ID( VK_FORMAT_ASTC_6x6_SFLOAT_BLOCK, DXGI_FORMAT_UNKNOWN, MTLPixelFormatASTC_6x6_HDR, GL_INVALID, GL_INVALID, GL_INVALID ),	0x02 | SL2_MAKE_COMP_FLAG( SL2_CS_ASTC ),
																																							0, 128, 6, 6, 1, 1, 1, false, true, false, true, &(GetCompressedSizeAstc<6, 6>), SL2_TBITS(16, 16, 16, 16), SL2_TSHIFTS(0, 0, 0, 0), CFormat::AstcToRgba64F<6, 6, false, true>, CFormat::AstcFromRgba64F<6, 6, false, true> },
		{ SL2_ID( VK_FORMAT_ASTC_8x5_SFLOAT_BLOCK, DXGI_FORMAT_UNKNOWN, MTLPixelFormatASTC_8x5_HDR, GL_INVALID, GL_INVALID, GL_INVALID ),	0x02 | SL2_MAKE_COMP_FLAG( SL2_CS_ASTC ),
																																							0, 128, 8, 5, 1, 1, 1, false, true, false, true, &(GetCompressedSizeAstc<8, 5>), SL2_TBITS(16, 16, 16, 16), SL2_TSHIFTS(0, 0, 0, 0), CFormat::AstcToRgba64F<8, 5, false, true>, CFormat::AstcFromRgba64F<8, 5, false, true> },
		{ SL2_ID( VK_FORMAT_ASTC_8x6_SFLOAT_BLOCK, DXGI_FORMAT_UNKNOWN, MTLPixelFormatASTC_8x6_HDR, GL_INVALID, GL_INVALID, GL_INVALID ),	0x02 | SL2_MAKE_COMP_FLAG( SL2_CS_ASTC ),
																																							0, 128, 8, 6, 1, 1, 1, false, true, false, true, &(GetCompressedSizeAstc<8, 6>), SL2_TBITS(16, 16, 16, 16), SL2_TSHIFTS(0, 0, 0, 0), CFormat::AstcToRgba64F<8, 6, false, true>, CFormat::AstcFromRgba64F<8, 6, false, true> },
		{ SL2_ID( VK_FORMAT_ASTC_8x8_SFLOAT_BLOCK, DXGI_FORMAT_UNKNOWN, MTLPixelFormatASTC_8x8_HDR, GL_INVALID, GL_INVALID, GL_INVALID ),	0x02 | SL2_MAKE_COMP_FLAG( SL2_CS_ASTC ),
																																							0, 128, 8, 8, 1, 1, 1, false, true, false, true, &(GetCompressedSizeAstc<8, 8>), SL2_TBITS(16, 16, 16, 16), SL2_TSHIFTS(0, 0, 0, 0), CFormat::AstcToRgba64F<8, 8, false, true>, CFormat::AstcFromRgba64F<8, 8, false, true> },
		{ SL2_ID( VK_FORMAT_ASTC_10x5_SFLOAT_BLOCK, DXGI_FORMAT_UNKNOWN, MTLPixelFormatASTC_10x5_HDR, GL_INVALID, GL_INVALID, GL_INVALID ),	0x02 | SL2_MAKE_COMP_FLAG( SL2_CS_ASTC ),
																																							0, 128, 10, 5, 1, 1, 1, false, true, false, true, &(GetCompressedSizeAstc<10, 5>), SL2_TBITS(16, 16, 16, 16), SL2_TSHIFTS(0, 0, 0, 0), CFormat::AstcToRgba64F<10, 5, false, true>, CFormat::AstcFromRgba64F<10, 5, false, true> },
		{ SL2_ID( VK_FORMAT_ASTC_10x6_SFLOAT_BLOCK, DXGI_FORMAT_UNKNOWN, MTLPixelFormatASTC_10x6_HDR, GL_INVALID, GL_INVALID, GL_INVALID ),	0x02 | SL2_MAKE_COMP_FLAG( SL2_CS_ASTC ),
																																							0, 128, 10, 6, 1, 1, 1, false, true, false, true, &(GetCompressedSizeAstc<10, 6>), SL2_TBITS(16, 16, 16, 16), SL2_TSHIFTS(0, 0, 0, 0), CFormat::AstcToRgba64F<10, 6, false, true>, CFormat::AstcFromRgba64F<10, 6, false, true> },
		{ SL2_ID( VK_FORMAT_ASTC_10x8_SFLOAT_BLOCK, DXGI_FORMAT_UNKNOWN, MTLPixelFormatASTC_10x8_HDR, GL_INVALID, GL_INVALID, GL_INVALID ),	0x02 | SL2_MAKE_COMP_FLAG( SL2_CS_ASTC ),
																																							0, 128, 10, 8, 1, 1, 1, false, true, false, true, &(GetCompressedSizeAstc<10, 8>), SL2_TBITS(16, 16, 16, 16), SL2_TSHIFTS(0, 0, 0, 0), CFormat::AstcToRgba64F<10, 8, false, true>, CFormat::AstcFromRgba64F<10, 8, false, true> },
		{ SL2_ID( VK_FORMAT_ASTC_10x10_SFLOAT_BLOCK, DXGI_FORMAT_UNKNOWN, MTLPixelFormatASTC_10x10_HDR, GL_INVALID, GL_INVALID, GL_INVALID ),	0x02 | SL2_MAKE_COMP_FLAG( SL2_CS_ASTC ),
																																							0, 128, 10, 10, 1, 1, 1, false, true, false, true, &(GetCompressedSizeAstc<10, 10>), SL2_TBITS(16, 16, 16, 16), SL2_TSHIFTS(0, 0, 0, 0), CFormat::AstcToRgba64F<10, 10, false, true>, CFormat::AstcFromRgba64F<10, 10, false, true> },
		{ SL2_ID( VK_FORMAT_ASTC_12x10_SFLOAT_BLOCK, DXGI_FORMAT_UNKNOWN, MTLPixelFormatASTC_12x10_HDR, GL_INVALID, GL_INVALID, GL_INVALID ),	0x02 | SL2_MAKE_COMP_FLAG( SL2_CS_ASTC ),
																																							0, 128, 12, 10, 1, 1, 1, false, true, false, true, &(GetCompressedSizeAstc<12, 10>), SL2_TBITS(16, 16, 16, 16), SL2_TSHIFTS(0, 0, 0, 0), CFormat::AstcToRgba64F<12, 10, false, true>, CFormat::AstcFromRgba64F<12, 10, false, true> },
		{ SL2_ID( VK_FORMAT_ASTC_12x12_SFLOAT_BLOCK, DXGI_FORMAT_UNKNOWN, MTLPixelFormatASTC_12x12_HDR, GL_INVALID, GL_INVALID, GL_INVALID ),	0x02 | SL2_MAKE_COMP_FLAG( SL2_CS_ASTC ),
																																							0, 128, 12, 12, 1, 1, 1, false, true, false, true, &(GetCompressedSizeAstc<12, 12>), SL2_TBITS(16, 16, 16, 16), SL2_TSHIFTS(0, 0, 0, 0), CFormat::AstcToRgba64F<12, 12, false, true>, CFormat::AstcFromRgba64F<12, 12, false, true> },


		{ SL2_ID( VK_FORMAT_ASTC_3x3x3_UNORM_BLOCK_EXT, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_COMPRESSED_RGBA_ASTC_3x3x3_OES, GL_UNSIGNED_BYTE, GL_RGBA ),	0x02 | SL2_MAKE_COMP_FLAG( SL2_CS_ASTC ),
																																							0, 128, 3, 3, 3, 1, 1, false, true, false, false, &(GetCompressedSizeAstc<3, 3, 3>), SL2_TBITS(8, 8, 8, 8), SL2_TSHIFTS(0, 0, 0, 0), CFormat::AstcToRgba64F<3, 3, false, false, 3>, CFormat::AstcFromRgba64F_2<3, 3, false, 3, false> },
		{ SL2_ID( VK_FORMAT_ASTC_4x3x3_UNORM_BLOCK_EXT, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_COMPRESSED_RGBA_ASTC_4x3x3_OES, GL_UNSIGNED_BYTE, GL_RGBA ),	0x02 | SL2_MAKE_COMP_FLAG( SL2_CS_ASTC ),
																																							0, 128, 4, 3, 3, 1, 1, false, true, false, false, &(GetCompressedSizeAstc<4, 3, 3>), SL2_TBITS(8, 8, 8, 8), SL2_TSHIFTS(0, 0, 0, 0), CFormat::AstcToRgba64F<4, 3, false, false, 3>, CFormat::AstcFromRgba64F_2<4, 3, false, 3, false> },
		{ SL2_ID( VK_FORMAT_ASTC_4x4x3_UNORM_BLOCK_EXT, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_COMPRESSED_RGBA_ASTC_4x4x3_OES, GL_UNSIGNED_BYTE, GL_RGBA ),	0x02 | SL2_MAKE_COMP_FLAG( SL2_CS_ASTC ),
																																							0, 128, 4, 4, 3, 1, 1, false, true, false, false, &(GetCompressedSizeAstc<4, 4, 3>), SL2_TBITS(8, 8, 8, 8), SL2_TSHIFTS(0, 0, 0, 0), CFormat::AstcToRgba64F<4, 4, false, false, 3>, CFormat::AstcFromRgba64F_2<4, 4, false, 3, false> },
		{ SL2_ID( VK_FORMAT_ASTC_4x4x4_UNORM_BLOCK_EXT, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_COMPRESSED_RGBA_ASTC_4x4x4_OES, GL_UNSIGNED_BYTE, GL_RGBA ),	0x02 | SL2_MAKE_COMP_FLAG( SL2_CS_ASTC ),
																																							0, 128, 4, 4, 4, 1, 1, false, true, false, false, &(GetCompressedSizeAstc<4, 4, 4>), SL2_TBITS(8, 8, 8, 8), SL2_TSHIFTS(0, 0, 0, 0), CFormat::AstcToRgba64F<4, 4, false, false, 4>, CFormat::AstcFromRgba64F_2<4, 4, false, 4, false> },
		{ SL2_ID( VK_FORMAT_ASTC_5x4x4_UNORM_BLOCK_EXT, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_COMPRESSED_RGBA_ASTC_5x4x4_OES, GL_UNSIGNED_BYTE, GL_RGBA ),	0x02 | SL2_MAKE_COMP_FLAG( SL2_CS_ASTC ),
																																							0, 128, 5, 4, 4, 1, 1, false, true, false, false, &(GetCompressedSizeAstc<5, 4, 4>), SL2_TBITS(8, 8, 8, 8), SL2_TSHIFTS(0, 0, 0, 0), CFormat::AstcToRgba64F<5, 4, false, false, 4>, CFormat::AstcFromRgba64F_2<5, 4, false, 4, false> },
		{ SL2_ID( VK_FORMAT_ASTC_5x5x4_UNORM_BLOCK_EXT, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_COMPRESSED_RGBA_ASTC_5x5x4_OES, GL_UNSIGNED_BYTE, GL_RGBA ),	0x02 | SL2_MAKE_COMP_FLAG( SL2_CS_ASTC ),
																																							0, 128, 5, 5, 4, 1, 1, false, true, false, false, &(GetCompressedSizeAstc<5, 5, 4>), SL2_TBITS(8, 8, 8, 8), SL2_TSHIFTS(0, 0, 0, 0), CFormat::AstcToRgba64F<5, 5, false, false, 4>, CFormat::AstcFromRgba64F_2<5, 5, false, 4, false> },
		{ SL2_ID( VK_FORMAT_ASTC_5x5x5_UNORM_BLOCK_EXT, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_COMPRESSED_RGBA_ASTC_5x5x5_OES, GL_UNSIGNED_BYTE, GL_RGBA ),	0x02 | SL2_MAKE_COMP_FLAG( SL2_CS_ASTC ),
																																							0, 128, 5, 5, 5, 1, 1, false, true, false, false, &(GetCompressedSizeAstc<5, 5, 5>), SL2_TBITS(8, 8, 8, 8), SL2_TSHIFTS(0, 0, 0, 0), CFormat::AstcToRgba64F<5, 5, false, false, 5>, CFormat::AstcFromRgba64F_2<5, 5, false, 5, false> },
		{ SL2_ID( VK_FORMAT_ASTC_6x5x5_UNORM_BLOCK_EXT, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_COMPRESSED_RGBA_ASTC_6x5x5_OES, GL_UNSIGNED_BYTE, GL_RGBA ),	0x02 | SL2_MAKE_COMP_FLAG( SL2_CS_ASTC ),
																																							0, 128, 6, 5, 5, 1, 1, false, true, false, false, &(GetCompressedSizeAstc<6, 5, 5>), SL2_TBITS(8, 8, 8, 8), SL2_TSHIFTS(0, 0, 0, 0), CFormat::AstcToRgba64F<6, 5, false, false, 5>, CFormat::AstcFromRgba64F_2<6, 5, false, 5, false> },
		{ SL2_ID( VK_FORMAT_ASTC_6x6x5_UNORM_BLOCK_EXT, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_COMPRESSED_RGBA_ASTC_6x6x5_OES, GL_UNSIGNED_BYTE, GL_RGBA ),	0x02 | SL2_MAKE_COMP_FLAG( SL2_CS_ASTC ),
																																							0, 128, 6, 6, 5, 1, 1, false, true, false, false, &(GetCompressedSizeAstc<6, 6, 5>), SL2_TBITS(8, 8, 8, 8), SL2_TSHIFTS(0, 0, 0, 0), CFormat::AstcToRgba64F<6, 6, false, false, 5>, CFormat::AstcFromRgba64F_2<6, 6, false, 5, false> },
		{ SL2_ID( VK_FORMAT_ASTC_6x6x6_UNORM_BLOCK_EXT, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_COMPRESSED_RGBA_ASTC_6x6x6_OES, GL_UNSIGNED_BYTE, GL_RGBA ),	0x02 | SL2_MAKE_COMP_FLAG( SL2_CS_ASTC ),
																																							0, 128, 6, 6, 6, 1, 1, false, true, false, false, &(GetCompressedSizeAstc<6, 6, 6>), SL2_TBITS(8, 8, 8, 8), SL2_TSHIFTS(0, 0, 0, 0), CFormat::AstcToRgba64F<6, 6, false, false, 6>, CFormat::AstcFromRgba64F_2<6, 6, false, 6, false> },


		{ SL2_ID( VK_FORMAT_ASTC_3x3x3_SRGB_BLOCK_EXT, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_COMPRESSED_SRGB8_ALPHA8_ASTC_3x3x3_OES, GL_UNSIGNED_BYTE, GL_RGBA ),	0x02 | SL2_MAKE_COMP_FLAG( SL2_CS_ASTC ),
																																							0, 128, 3, 3, 3, 1, 1, true, true, false, false, &(GetCompressedSizeAstc<3, 3, 3>), SL2_TBITS(8, 8, 8, 8), SL2_TSHIFTS(0, 0, 0, 0), CFormat::AstcToRgba64F<3, 3, true, false, 3>, CFormat::AstcFromRgba64F_2<3, 3, true, 3, false> },
		{ SL2_ID( VK_FORMAT_ASTC_4x3x3_SRGB_BLOCK_EXT, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x3x3_OES, GL_UNSIGNED_BYTE, GL_RGBA ),	0x02 | SL2_MAKE_COMP_FLAG( SL2_CS_ASTC ),
																																							0, 128, 4, 3, 3, 1, 1, true, true, false, false, &(GetCompressedSizeAstc<4, 3, 3>), SL2_TBITS(8, 8, 8, 8), SL2_TSHIFTS(0, 0, 0, 0), CFormat::AstcToRgba64F<4, 3, true, false, 3>, CFormat::AstcFromRgba64F_2<4, 3, true, 3, false> },
		{ SL2_ID( VK_FORMAT_ASTC_4x4x3_SRGB_BLOCK_EXT, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4x3_OES, GL_UNSIGNED_BYTE, GL_RGBA ),	0x02 | SL2_MAKE_COMP_FLAG( SL2_CS_ASTC ),
																																							0, 128, 4, 4, 3, 1, 1, true, true, false, false, &(GetCompressedSizeAstc<4, 4, 3>), SL2_TBITS(8, 8, 8, 8), SL2_TSHIFTS(0, 0, 0, 0), CFormat::AstcToRgba64F<4, 4, true, false, 3>, CFormat::AstcFromRgba64F_2<4, 4, true, 3, false> },
		{ SL2_ID( VK_FORMAT_ASTC_4x4x4_SRGB_BLOCK_EXT, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4x4_OES, GL_UNSIGNED_BYTE, GL_RGBA ),	0x02 | SL2_MAKE_COMP_FLAG( SL2_CS_ASTC ),
																																							0, 128, 4, 4, 4, 1, 1, true, true, false, false, &(GetCompressedSizeAstc<4, 4, 4>), SL2_TBITS(8, 8, 8, 8), SL2_TSHIFTS(0, 0, 0, 0), CFormat::AstcToRgba64F<4, 4, true, false, 4>, CFormat::AstcFromRgba64F_2<4, 4, true, 4, false> },
		{ SL2_ID( VK_FORMAT_ASTC_5x4x4_SRGB_BLOCK_EXT, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4x4_OES, GL_UNSIGNED_BYTE, GL_RGBA ),	0x02 | SL2_MAKE_COMP_FLAG( SL2_CS_ASTC ),
																																							0, 128, 5, 4, 4, 1, 1, true, true, false, false, &(GetCompressedSizeAstc<5, 4, 4>), SL2_TBITS(8, 8, 8, 8), SL2_TSHIFTS(0, 0, 0, 0), CFormat::AstcToRgba64F<5, 4, true, false, 4>, CFormat::AstcFromRgba64F_2<5, 4, true, 4, false> },
		{ SL2_ID( VK_FORMAT_ASTC_5x5x4_SRGB_BLOCK_EXT, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5x4_OES, GL_UNSIGNED_BYTE, GL_RGBA ),	0x02 | SL2_MAKE_COMP_FLAG( SL2_CS_ASTC ),
																																							0, 128, 5, 5, 4, 1, 1, true, true, false, false, &(GetCompressedSizeAstc<5, 5, 4>), SL2_TBITS(8, 8, 8, 8), SL2_TSHIFTS(0, 0, 0, 0), CFormat::AstcToRgba64F<5, 5, true, false, 4>, CFormat::AstcFromRgba64F_2<5, 5, true, 4, false> },
		{ SL2_ID( VK_FORMAT_ASTC_5x5x5_SRGB_BLOCK_EXT, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5x5_OES, GL_UNSIGNED_BYTE, GL_RGBA ),	0x02 | SL2_MAKE_COMP_FLAG( SL2_CS_ASTC ),
																																							0, 128, 5, 5, 5, 1, 1, true, true, false, false, &(GetCompressedSizeAstc<5, 5, 5>), SL2_TBITS(8, 8, 8, 8), SL2_TSHIFTS(0, 0, 0, 0), CFormat::AstcToRgba64F<5, 5, true, false, 5>, CFormat::AstcFromRgba64F_2<5, 5, true, 5, false> },
		{ SL2_ID( VK_FORMAT_ASTC_6x5x5_SRGB_BLOCK_EXT, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5x5_OES, GL_UNSIGNED_BYTE, GL_RGBA ),	0x02 | SL2_MAKE_COMP_FLAG( SL2_CS_ASTC ),
																																							0, 128, 6, 5, 5, 1, 1, true, true, false, false, &(GetCompressedSizeAstc<6, 5, 5>), SL2_TBITS(8, 8, 8, 8), SL2_TSHIFTS(0, 0, 0, 0), CFormat::AstcToRgba64F<6, 5, true, false, 5>, CFormat::AstcFromRgba64F_2<6, 5, true, 5, false> },
		{ SL2_ID( VK_FORMAT_ASTC_6x6x5_SRGB_BLOCK_EXT, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6x5_OES, GL_UNSIGNED_BYTE, GL_RGBA ),	0x02 | SL2_MAKE_COMP_FLAG( SL2_CS_ASTC ),
																																							0, 128, 6, 6, 5, 1, 1, true, true, false, false, &(GetCompressedSizeAstc<6, 6, 5>), SL2_TBITS(8, 8, 8, 8), SL2_TSHIFTS(0, 0, 0, 0), CFormat::AstcToRgba64F<6, 6, true, false, 5>, CFormat::AstcFromRgba64F_2<6, 6, true, 5, false> },
		{ SL2_ID( VK_FORMAT_ASTC_6x6x6_SRGB_BLOCK_EXT, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6x6_OES, GL_UNSIGNED_BYTE, GL_RGBA ),	0x02 | SL2_MAKE_COMP_FLAG( SL2_CS_ASTC ),
																																							0, 128, 6, 6, 6, 1, 1, true, true, false, false, &(GetCompressedSizeAstc<6, 6, 6>), SL2_TBITS(8, 8, 8, 8), SL2_TSHIFTS(0, 0, 0, 0), CFormat::AstcToRgba64F<6, 6, true, false, 6>, CFormat::AstcFromRgba64F_2<6, 6, true, 6, false> },

		{ SL2_ID( VK_FORMAT_ASTC_3x3x3_SFLOAT_BLOCK_EXT, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_INVALID, GL_INVALID, GL_INVALID ),	0x02 | SL2_MAKE_COMP_FLAG( SL2_CS_ASTC ),
																																							0, 128, 3, 3, 3, 1, 1, false, true, false, true, &(GetCompressedSizeAstc<3, 3, 3>), SL2_TBITS(16, 16, 16, 16), SL2_TSHIFTS(0, 0, 0, 0), CFormat::AstcToRgba64F<3, 3, false, true, 3>, CFormat::AstcFromRgba64F_2<3, 3, false, 3, true> },
		{ SL2_ID( VK_FORMAT_ASTC_4x3x3_SFLOAT_BLOCK_EXT, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_INVALID, GL_INVALID, GL_INVALID ),	0x02 | SL2_MAKE_COMP_FLAG( SL2_CS_ASTC ),
																																							0, 128, 4, 3, 3, 1, 1, false, true, false, true, &(GetCompressedSizeAstc<4, 3, 3>), SL2_TBITS(16, 16, 16, 16), SL2_TSHIFTS(0, 0, 0, 0), CFormat::AstcToRgba64F<4, 3, false, true, 3>, CFormat::AstcFromRgba64F_2<4, 3, false, 3, true> },
		{ SL2_ID( VK_FORMAT_ASTC_4x4x3_SFLOAT_BLOCK_EXT, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_INVALID, GL_INVALID, GL_INVALID ),	0x02 | SL2_MAKE_COMP_FLAG( SL2_CS_ASTC ),
																																							0, 128, 4, 4, 3, 1, 1, false, true, false, true, &(GetCompressedSizeAstc<4, 4, 3>), SL2_TBITS(16, 16, 16, 16), SL2_TSHIFTS(0, 0, 0, 0), CFormat::AstcToRgba64F<4, 4, false, true, 3>, CFormat::AstcFromRgba64F_2<4, 4, false, 3, true> },
		{ SL2_ID( VK_FORMAT_ASTC_4x4x4_SFLOAT_BLOCK_EXT, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_INVALID, GL_INVALID, GL_INVALID ),	0x02 | SL2_MAKE_COMP_FLAG( SL2_CS_ASTC ),
																																							0, 128, 4, 4, 4, 1, 1, false, true, false, true, &(GetCompressedSizeAstc<4, 4, 4>), SL2_TBITS(16, 16, 16, 16), SL2_TSHIFTS(0, 0, 0, 0), CFormat::AstcToRgba64F<4, 4, false, true, 4>, CFormat::AstcFromRgba64F_2<4, 4, false, 4, true> },
		{ SL2_ID( VK_FORMAT_ASTC_5x4x4_SFLOAT_BLOCK_EXT, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_INVALID, GL_INVALID, GL_INVALID ),	0x02 | SL2_MAKE_COMP_FLAG( SL2_CS_ASTC ),
																																							0, 128, 5, 4, 4, 1, 1, false, true, false, true, &(GetCompressedSizeAstc<5, 4, 4>), SL2_TBITS(16, 16, 16, 16), SL2_TSHIFTS(0, 0, 0, 0), CFormat::AstcToRgba64F<5, 4, false, true, 4>, CFormat::AstcFromRgba64F_2<5, 4, false, 4, true> },
		{ SL2_ID( VK_FORMAT_ASTC_5x5x4_SFLOAT_BLOCK_EXT, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_INVALID, GL_INVALID, GL_INVALID ),	0x02 | SL2_MAKE_COMP_FLAG( SL2_CS_ASTC ),
																																							0, 128, 5, 5, 4, 1, 1, false, true, false, true, &(GetCompressedSizeAstc<5, 5, 4>), SL2_TBITS(16, 16, 16, 16), SL2_TSHIFTS(0, 0, 0, 0), CFormat::AstcToRgba64F<5, 5, false, true, 4>, CFormat::AstcFromRgba64F_2<5, 5, false, 4, true> },
		{ SL2_ID( VK_FORMAT_ASTC_5x5x5_SFLOAT_BLOCK_EXT, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_INVALID, GL_INVALID, GL_INVALID ),	0x02 | SL2_MAKE_COMP_FLAG( SL2_CS_ASTC ),
																																							0, 128, 5, 5, 5, 1, 1, false, true, false, true, &(GetCompressedSizeAstc<5, 5, 5>), SL2_TBITS(16, 16, 16, 16), SL2_TSHIFTS(0, 0, 0, 0), CFormat::AstcToRgba64F<5, 5, false, true, 5>, CFormat::AstcFromRgba64F_2<5, 5, false, 5, true> },
		{ SL2_ID( VK_FORMAT_ASTC_6x5x5_SFLOAT_BLOCK_EXT, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_INVALID, GL_INVALID, GL_INVALID ),	0x02 | SL2_MAKE_COMP_FLAG( SL2_CS_ASTC ),
																																							0, 128, 6, 5, 5, 1, 1, false, true, false, true, &(GetCompressedSizeAstc<6, 5, 5>), SL2_TBITS(16, 16, 16, 16), SL2_TSHIFTS(0, 0, 0, 0), CFormat::AstcToRgba64F<6, 5, false, true, 5>, CFormat::AstcFromRgba64F_2<6, 5, false, 5, true> },
		{ SL2_ID( VK_FORMAT_ASTC_6x6x5_SFLOAT_BLOCK_EXT, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_INVALID, GL_INVALID, GL_INVALID ),	0x02 | SL2_MAKE_COMP_FLAG( SL2_CS_ASTC ),
																																							0, 128, 6, 6, 5, 1, 1, false, true, false, true, &(GetCompressedSizeAstc<6, 6, 5>), SL2_TBITS(16, 16, 16, 16), SL2_TSHIFTS(0, 0, 0, 0), CFormat::AstcToRgba64F<6, 6, false, true, 5>, CFormat::AstcFromRgba64F_2<6, 6, false, 5, true> },
		{ SL2_ID( VK_FORMAT_ASTC_6x6x6_SFLOAT_BLOCK_EXT, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_INVALID, GL_INVALID, GL_INVALID ),	0x02 | SL2_MAKE_COMP_FLAG( SL2_CS_ASTC ),
																																							0, 128, 6, 6, 6, 1, 1, false, true, false, true, &(GetCompressedSizeAstc<6, 6, 6>), SL2_TBITS(16, 16, 16, 16), SL2_TSHIFTS(0, 0, 0, 0), CFormat::AstcToRgba64F<6, 6, false, true, 6>, CFormat::AstcFromRgba64F_2<6, 6, false, 6, true> },

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
		
		{ SL2_ID( VK_FORMAT_D16_UNORM, DXGI_FORMAT_D16_UNORM, MTLPixelFormatDepth16Unorm, GL_DEPTH_COMPONENT16, GL_UNSIGNED_SHORT, GL_DEPTH_COMPONENT ),	0x08, 0, 16, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_TBITS(16, 0, 0, 0), SL2_TSHIFTS(0, 0, 0, 0), CFormat::Depth16ToRgba64F, CFormat::Depth16FromRgba64F },
		{ SL2_ID( VK_FORMAT_UNDEFINED, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_DEPTH_COMPONENT24, GL_UNSIGNED_INT, GL_DEPTH_COMPONENT ),				0x08, 0, 24, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_TBITS(24, 0, 0, 0), SL2_TSHIFTS(0, 0, 0, 0), CFormat::Depth24ToRgba64F, CFormat::Depth24FromRgba64F },
		{ SL2_ID( VK_FORMAT_UNDEFINED, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_DEPTH_COMPONENT32, GL_UNSIGNED_INT, GL_DEPTH_COMPONENT ),				0x08, 0, 32, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_TBITS(32, 0, 0, 0), SL2_TSHIFTS(0, 0, 0, 0), CFormat::Depth32ToRgba64F, CFormat::Depth32FromRgba64F },
		{ SL2_ID( VK_FORMAT_D32_SFLOAT, DXGI_FORMAT_D32_FLOAT, MTLPixelFormatDepth32Float, GL_DEPTH_COMPONENT32F, GL_FLOAT, GL_DEPTH_COMPONENT ),			0x08, 0, 32, 1, 1, 1, 1, 1, false, false, false, true, nullptr, SL2_TBITS(32, 0, 0, 0), SL2_TSHIFTS(0, 0, 0, 0), CFormat::Depth32FToRgba64F, CFormat::Depth32FFromRgba64F },
		{ SL2_ID( VK_FORMAT_UNDEFINED, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_DEPTH_COMPONENT32F_NV, GL_FLOAT, GL_DEPTH_COMPONENT ),				0x08, 0, 32, 1, 1, 1, 1, 1, false, false, false, true, nullptr, SL2_TBITS(32, 0, 0, 0), SL2_TSHIFTS(0, 0, 0, 0), CFormat::Depth32FToRgba64F, CFormat::Depth32FFromRgba64F },

		{ SL2_ID( VK_FORMAT_UNDEFINED, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_STENCIL_INDEX1, GL_UNSIGNED_BYTE, GL_STENCIL_INDEX ),					0x10, 0, 1, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_TBITS(0, 0, 0, 1), SL2_TSHIFTS(0, 0, 0, 0), CFormat::StencilXToRgba64F<uint8_t, 1>, CFormat::StencilXFromRgba64F<uint8_t, 1> },
		{ SL2_ID( VK_FORMAT_UNDEFINED, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_STENCIL_INDEX4, GL_UNSIGNED_BYTE, GL_STENCIL_INDEX ),					0x10, 0, 4, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_TBITS(0, 0, 0, 4), SL2_TSHIFTS(0, 0, 0, 0), CFormat::StencilXToRgba64F<uint8_t, 4>, CFormat::StencilXFromRgba64F<uint8_t, 4> },
		{ SL2_ID( VK_FORMAT_S8_UINT, DXGI_FORMAT_UNKNOWN, MTLPixelFormatStencil8, GL_STENCIL_INDEX8, GL_UNSIGNED_BYTE, GL_STENCIL_INDEX ),					0x10, 0, 8, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_TBITS(0, 0, 0, 8), SL2_TSHIFTS(0, 0, 0, 0), CFormat::StencilXToRgba64F<uint8_t, 8>, CFormat::StencilXFromRgba64F<uint8_t, 8> },
		{ SL2_ID( VK_FORMAT_UNDEFINED, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_STENCIL_INDEX16, GL_UNSIGNED_BYTE, GL_STENCIL_INDEX ),				0x10, 0, 16, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_TBITS(0, 0, 0, 16), SL2_TSHIFTS(0, 0, 0, 0), CFormat::StencilXToRgba64F<uint16_t, 16>, CFormat::StencilXFromRgba64F<uint16_t, 16> },

		{ SL2_ID( VK_FORMAT_D16_UNORM_S8_UINT, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_INVALID, GL_INVALID, GL_INVALID ),							0x18, 0, 24, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_TBITS(16, 0, 0, 8), SL2_TSHIFTS(0, 0, 0, 16), CFormat::Depth16S8ToRgba64F, CFormat::Depth16S8FromRgba64F },
		{ SL2_ID( VK_FORMAT_D24_UNORM_S8_UINT, DXGI_FORMAT_D24_UNORM_S8_UINT, MTLPixelFormatDepth24Unorm_Stencil8, GL_DEPTH24_STENCIL8, GL_UNSIGNED_INT_24_8, GL_DEPTH_STENCIL ),
																																							0x18, 0, 32, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_TBITS(24, 0, 0, 8), SL2_TSHIFTS(8, 0, 0, 0), CFormat::Depth24S8ToRgba64F, CFormat::Depth24S8FromRgba64F },
		{ SL2_ID( VK_FORMAT_X8_D24_UNORM_PACK32, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_INVALID, GL_INVALID, GL_INVALID ),							0x18, 0, 32, 1, 1, 1, 1, 1, false, false, false, false, nullptr, SL2_TBITS(24, 0, 0, 8), SL2_TSHIFTS(8, 0, 0, 0), CFormat::Depth24X8ToRgba64F, CFormat::Depth24X8FromRgba64F },
		{ SL2_ID( VK_FORMAT_D32_SFLOAT_S8_UINT, DXGI_FORMAT_D32_FLOAT_S8X24_UINT, MTLPixelFormatDepth32Float_Stencil8, GL_DEPTH32F_STENCIL8, GL_FLOAT_32_UNSIGNED_INT_24_8_REV, GL_DEPTH_STENCIL ),
																																							0x18, 0, 64, 1, 1, 1, 1, 1, false, false, false, true, nullptr, SL2_TBITS(32, 0, 0, 8), SL2_TSHIFTS(0, 0, 0, 32), CFormat::Depth32FS8ToRgba64F, CFormat::Depth32FS8FromRgba64F },
		{ SL2_ID( VK_FORMAT_UNDEFINED, DXGI_FORMAT_UNKNOWN, MTLPixelFormatInvalid, GL_DEPTH32F_STENCIL8_NV, GL_FLOAT_32_UNSIGNED_INT_24_8_REV, GL_DEPTH_STENCIL ),
																																							0x18, 0, 64, 1, 1, 1, 1, 1, false, false, false, true, nullptr, SL2_TBITS(32, 0, 0, 8), SL2_TSHIFTS(0, 0, 0, 32), CFormat::Depth32FS8ToRgba64F, CFormat::Depth32FS8FromRgba64F },
		
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
	bool CFormat::m_bUseNVidiaDecode = true;

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

	/** BC7 settings. */
	::bc7_enc_settings CFormat::m_besBc7Settings;
	
	/** BC6H settings. */
	::bc6h_enc_settings CFormat::m_besBc6hSettings;
	
	/** ETC settings. */
	::etc_enc_settings CFormat::m_eesEtsSettings;
	
	/** ASTC settings. */
	::astc_enc_settings CFormat::m_aesAstcSettings;
	
	/** Squish settings. */
	uint32_t CFormat::m_ui32SquishFlags = squish::kColorMetricPerceptual | squish::kColorIterativeClusterFit | squish::kWeightColorByAlpha;

	/** Performance value. 0 = Very Slow, 1 = Slow, 2 = Basic, 3 = Fast, 4 = Very Fast. 5 = Ultra Fast. */
	uint32_t CFormat::m_ui32Perf = 3;

	/** Alpha cut-off. */
	uint8_t CFormat::m_ui8AlphaThresh = 128;


	// == Functions.
	/**
	 * Initializes settings.
	 **/
	void CFormat::Init() {
		::GetProfile_alpha_basic( &m_besBc7Settings );
		::GetProfile_bc6h_basic( &m_besBc6hSettings );
		::GetProfile_etc_slow( &m_eesEtsSettings );
		//::GetProfile_astc_alpha_fast( &m_aesAstcSettings );
	}

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
	 * Applies settings based on the current value of m_ui32Perf.
	 * 
	 * \param _bAlpha Determines whether the current texture has an alpha channel.
	 * \param _ui32AstcBlockX The ASTC X block size.
	 * \param _ui32AstcBlockY The ASTC Y block size.
	 **/
	void CFormat::ApplySettings( bool _bAlpha, uint32_t _ui32AstcBlockX, uint32_t _ui32AstcBlockY ) {
		switch ( m_ui32Perf ) {
			case 0 : {					// Very slow.
				if ( _bAlpha ) {
					::GetProfile_alpha_slow( &m_besBc7Settings );
					::GetProfile_astc_alpha_slow( &m_aesAstcSettings, _ui32AstcBlockX, _ui32AstcBlockY );
				}
				else {
					::GetProfile_slow( &m_besBc7Settings );
					::GetProfile_astc_fast( &m_aesAstcSettings, _ui32AstcBlockX, _ui32AstcBlockY );
				}
				::GetProfile_bc6h_veryslow( &m_besBc6hSettings );
				::GetProfile_etc_slow( &m_eesEtsSettings );
				
				m_ui32SquishFlags = m_ui32SquishFlags & ~(squish::kColorIterativeClusterFit | squish::kColorClusterFit | squish::kColorRangeFit);
				m_ui32SquishFlags |= squish::kColorIterativeClusterFit;
				break;
			}
			case 1 : {					// Slow.
				if ( _bAlpha ) {
					::GetProfile_alpha_slow( &m_besBc7Settings );
					::GetProfile_astc_alpha_slow( &m_aesAstcSettings, _ui32AstcBlockX, _ui32AstcBlockY );
				}
				else {
					::GetProfile_slow( &m_besBc7Settings );
					::GetProfile_astc_fast( &m_aesAstcSettings, _ui32AstcBlockX, _ui32AstcBlockY );
				}
				::GetProfile_bc6h_slow( &m_besBc6hSettings );
				::GetProfile_etc_slow( &m_eesEtsSettings );
				
				m_ui32SquishFlags = m_ui32SquishFlags & ~(squish::kColorIterativeClusterFit | squish::kColorClusterFit | squish::kColorRangeFit);
				m_ui32SquishFlags |= squish::kColorIterativeClusterFit;
				break;
			}
			case 2 : {					// Basic.
				if ( _bAlpha ) {
					::GetProfile_alpha_basic( &m_besBc7Settings );
					::GetProfile_astc_alpha_slow( &m_aesAstcSettings, _ui32AstcBlockX, _ui32AstcBlockY );
				}
				else {
					::GetProfile_basic( &m_besBc7Settings );
					::GetProfile_astc_fast( &m_aesAstcSettings, _ui32AstcBlockX, _ui32AstcBlockY );
				}
				::GetProfile_bc6h_basic( &m_besBc6hSettings );
				::GetProfile_etc_slow( &m_eesEtsSettings );
				
				m_ui32SquishFlags = m_ui32SquishFlags & ~(squish::kColorIterativeClusterFit | squish::kColorClusterFit | squish::kColorRangeFit);
				m_ui32SquishFlags |= squish::kColorClusterFit;
				break;
			}
			case 3 : {					// Fast.
				if ( _bAlpha ) {
					::GetProfile_alpha_fast( &m_besBc7Settings );
					::GetProfile_astc_alpha_slow( &m_aesAstcSettings, _ui32AstcBlockX, _ui32AstcBlockY );
				}
				else {
					::GetProfile_fast( &m_besBc7Settings );
					::GetProfile_astc_fast( &m_aesAstcSettings, _ui32AstcBlockX, _ui32AstcBlockY );
				}
				::GetProfile_bc6h_fast( &m_besBc6hSettings );
				::GetProfile_etc_slow( &m_eesEtsSettings );
				
				m_ui32SquishFlags = m_ui32SquishFlags & ~(squish::kColorIterativeClusterFit | squish::kColorClusterFit | squish::kColorRangeFit);
				m_ui32SquishFlags |= squish::kColorRangeFit;
				break;
			}
			case 4 : {					// Very Fast.
				if ( _bAlpha ) {
					::GetProfile_alpha_veryfast( &m_besBc7Settings );
					::GetProfile_astc_alpha_fast( &m_aesAstcSettings, _ui32AstcBlockX, _ui32AstcBlockY );
				}
				else {
					::GetProfile_veryfast( &m_besBc7Settings );
					::GetProfile_astc_fast( &m_aesAstcSettings, _ui32AstcBlockX, _ui32AstcBlockY );
				}
				::GetProfile_bc6h_veryfast( &m_besBc6hSettings );
				::GetProfile_etc_slow( &m_eesEtsSettings );
				
				m_ui32SquishFlags = m_ui32SquishFlags & ~(squish::kColorIterativeClusterFit | squish::kColorClusterFit | squish::kColorRangeFit);
				m_ui32SquishFlags |= squish::kColorRangeFit;
				break;
			}
			case 5 : {					// Ultra Fast.
				if ( _bAlpha ) {
					::GetProfile_alpha_ultrafast( &m_besBc7Settings );
					::GetProfile_astc_alpha_fast( &m_aesAstcSettings, _ui32AstcBlockX, _ui32AstcBlockY );
				}
				else {
					::GetProfile_ultrafast( &m_besBc7Settings );
					::GetProfile_astc_fast( &m_aesAstcSettings, _ui32AstcBlockX, _ui32AstcBlockY );
				}
				::GetProfile_bc6h_veryfast( &m_besBc6hSettings );
				::GetProfile_etc_slow( &m_eesEtsSettings );
				
				m_ui32SquishFlags = m_ui32SquishFlags & ~(squish::kColorIterativeClusterFit | squish::kColorClusterFit | squish::kColorRangeFit);
				m_ui32SquishFlags |= squish::kColorRangeFit;
				break;
			}
		}
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

		if ( _pkifFormat->ui8RBits && _pkifFormat->ui8RBits == _pkifTest->ui8RBits ) { fScore += 1.0f; }
		if ( _pkifFormat->ui8GBits && _pkifFormat->ui8GBits == _pkifTest->ui8GBits ) { fScore += 1.0f; }
		if ( _pkifFormat->ui8BBits && _pkifFormat->ui8BBits == _pkifTest->ui8BBits ) { fScore += 1.0f; }
		if ( _pkifFormat->ui8ABits && _pkifFormat->ui8ABits == _pkifTest->ui8ABits ) { fScore += 1.0f; }

		uint8_t ui8Chans = CountChannels( _pkifFormat );
		uint8_t ui8TheirChans = CountChannels( _pkifTest );
		uint8_t ui32ChanMix = 0;
		if ( _pkifFormat->ui8RBits && _pkifTest->ui8RBits ) { ++ui32ChanMix; }
		if ( _pkifFormat->ui8GBits && _pkifTest->ui8GBits ) { ++ui32ChanMix; }
		if ( _pkifFormat->ui8BBits && _pkifTest->ui8BBits ) { ++ui32ChanMix; }
		if ( _pkifFormat->ui8ABits && _pkifTest->ui8ABits ) { ++ui32ChanMix; }
		if ( ui32ChanMix == ui8Chans ) {
			// Same channels active.
			fScore += 1.0f;
		}
		else if ( ui8Chans < ui8TheirChans ) {
			// All the channels we need and more.
			fScore += 0.25f;
		}

		if ( _pkifFormat->ui8RBits && _pkifTest->ui8RBits && _pkifFormat->ui8RBits < _pkifTest->ui8RBits ) { fScore += float( _pkifFormat->ui8RBits ) / _pkifTest->ui8RBits * 0.25f; }
		if ( _pkifFormat->ui8GBits && _pkifTest->ui8GBits && _pkifFormat->ui8GBits < _pkifTest->ui8GBits ) { fScore += float( _pkifFormat->ui8GBits ) / _pkifTest->ui8GBits * 0.25f; }
		if ( _pkifFormat->ui8BBits && _pkifTest->ui8BBits && _pkifFormat->ui8BBits < _pkifTest->ui8BBits ) { fScore += float( _pkifFormat->ui8BBits ) / _pkifTest->ui8BBits * 0.25f; }
		if ( _pkifFormat->ui8ABits && _pkifTest->ui8ABits && _pkifFormat->ui8ABits < _pkifTest->ui8ABits ) { fScore += float( _pkifFormat->ui8ABits ) / _pkifTest->ui8ABits * 0.25f; }

		if ( !_pkifFormat->bCompressed ) {
			if ( _pkifFormat->ui8RShift == _pkifTest->ui8RShift ) { fScore += 1.0f; }
			if ( _pkifFormat->ui8GShift == _pkifTest->ui8GShift ) { fScore += 1.0f; }
			if ( _pkifFormat->ui8BShift == _pkifTest->ui8BShift ) { fScore += 1.0f; }
			if ( _pkifFormat->ui8AShift == _pkifTest->ui8AShift ) { fScore += 1.0f; }
		}

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
	 * Decodes a swizzle string.
	 * 
	 * \param _pwcString The swizzle string to decode.
	 * \param _sReturn HOlds the return swizzle if true is returned.
	 * \return Returns true if the string represents a valid swizzle.
	 **/
	bool CFormat::CreateSwizzleFromString( const wchar_t * _pwcString, SL2_SWIZZLE &_sReturn ) {
		for ( size_t I = 0; I < 4; ++I ) {
			switch ( _pwcString[I] ) {
				case L'r' : {}
				case L'R' : {}
				case L'x' : {}
				case L'X' : {
					_sReturn.pcComp[I] = SL2_PC_R;
					break;
				}
				case L'g' : {}
				case L'G' : {}
				case L'y' : {}
				case L'Y' : {
					_sReturn.pcComp[I] = SL2_PC_G;
					break;
				}
				case L'b' : {}
				case L'B' : {}
				case L'z' : {}
				case L'Z' : {
					_sReturn.pcComp[I] = SL2_PC_B;
					break;
				}
				case L'a' : {}
				case L'A' : {}
				case L'w' : {}
				case L'W' : {
					_sReturn.pcComp[I] = SL2_PC_A;
					break;
				}
				case L'0' : {
					_sReturn.pcComp[I] = SL2_PC_ZERO;
					break;
				}
				case L'1' : {
					_sReturn.pcComp[I] = SL2_PC_ONE;
					break;
				}
				default : { return false; }
			}
		}
		return true;
	}

	/**
	 * Is the given swizzle a default swizzle?
	 * 
	 * \param _sSwizzle The swizzle to test.
	 * \return Returns true if the swizzle represents no change to the components.
	 **/
	bool CFormat::SwizzleIsDefault( const SL2_SWIZZLE &_sSwizzle ) {
		for ( size_t I = 0; I < 4; ++I ) {
			if ( size_t( _sSwizzle.pcComp[I] ) != I ) { return false; }
		}
		return true;
	}

	/**
	 * Expands an RGBA64F texture to a size divisible by some number of pixels.
	 * 
	 * \param _pui8Src The source texture to expand.
	 * \param _ui32W The width of the source texture.  Contains the width of the final texture upon return.
	 * \param _ui32H The height of the source texture.  Contains the width of the final texture upon return.
	 * \param _ui32D The depth of the source texture.  Contains the width of the final texture upon return.
	 * \param _vOutput The output vector to contain the expanded image, with border colors replicated to the added edges.
	 * \param _ui32X The X multiplicant.
	 * \param _ui32Y The Y multiplicant.
	 * \param _ui32Z The Z multiplicant.
	 * \return Returns true if the new texture could be allocated.
	 **/
	bool CFormat::ExpandTexture( const uint8_t * _pui8Src, uint32_t &_ui32W, uint32_t &_ui32H, uint32_t &_ui32D,
		std::vector<uint8_t> &_vOutput, uint32_t _ui32X, uint32_t _ui32Y, uint32_t _ui32Z ) {
		uint32_t ui32NewW = (_ui32W + (_ui32X - 1)) / _ui32X * _ui32X;
		uint32_t ui32NewH = (_ui32H + (_ui32Y - 1)) / _ui32Y * _ui32Y;
		uint32_t ui32NewD = (_ui32D + (_ui32Z - 1)) / _ui32Z * _ui32Z;

		
		uint32_t ui32SrcPitch = _ui32W;
		uint32_t ui32SrcSlice = ui32SrcPitch * _ui32H;

		uint32_t ui32DstPitch = ui32NewW;
		uint32_t ui32DstSlice = ui32DstPitch * ui32NewH;

		try {
			_vOutput.resize( ui32DstSlice * ui32NewD * sizeof( SL2_RGBA64F ) );
		}
		catch ( ... ) { return false; }
		SL2_RGBA64F * prgbDst = reinterpret_cast<SL2_RGBA64F *>(_vOutput.data());
		const SL2_RGBA64F * prgbSrc = reinterpret_cast<const SL2_RGBA64F *>(_pui8Src);
		for ( uint32_t D = 0; D < ui32NewD; ++D ) {
			for ( uint32_t H = 0; H < ui32NewH; ++H ) {
				for ( uint32_t W = 0; W < ui32NewW; ++W ) {
					uint32_t ui32ThisX = CUtilities::Min( W, _ui32W - 1 );
					uint32_t ui32ThisY = CUtilities::Min( H, _ui32H - 1 );
					uint32_t ui32ThisZ = CUtilities::Min( D, _ui32D - 1 );

					prgbDst[ui32DstSlice*D+ui32DstPitch*H+W] = prgbSrc[ui32SrcSlice*ui32ThisZ+ui32SrcPitch*ui32ThisY+ui32ThisX];
				}
			}
		}
		_ui32W = ui32NewW;
		_ui32H = ui32NewH;
		_ui32D = ui32NewD;
		return true;
	}

	/**
	 * Copies an RGBA64F texture.
	 * 
	 * \param _pui8Src The source texture to copy.
	 * \param _ui32W The width of the source texture.
	 * \param _ui32H The height of the source texture.
	 * \param _ui32D The depth of the source texture.
	 * \param _vOutput The output vector to contain the copied image.
	 * \return Returns true if the new texture could be allocated.
	 **/
	bool CFormat::CopyTexture( const uint8_t * _pui8Src, uint32_t _ui32W, uint32_t _ui32H, uint32_t _ui32D,
		std::vector<uint8_t> &_vOutput ) {
		uint32_t ui32Pitch = _ui32W;
		uint32_t ui32Slice = ui32Pitch * _ui32H;

		try {
			_vOutput.resize( ui32Slice * _ui32D * sizeof( SL2_RGBA64F ) );
		}
		catch ( ... ) {
			return false;
		}
		SL2_RGBA64F * prgbDst = reinterpret_cast<SL2_RGBA64F *>(_vOutput.data());
		const SL2_RGBA64F * prgbSrc = reinterpret_cast<const SL2_RGBA64F *>(_pui8Src);
		for ( uint32_t D = 0; D < _ui32D; ++D ) {
			for ( uint32_t H = 0; H < _ui32H; ++H ) {
				for ( uint32_t W = 0; W < _ui32W; ++W ) {
					prgbDst[ui32Slice*D+ui32Pitch*H+W] = prgbSrc[ui32Slice*D+ui32Pitch*H+W];
				}
			}
		}
		return true;
	}

	/**
	 * Applies a given gamma curve an RGBA64F texture.
	 * 
	 * \param _pui8Src The source texture to expand.
	 * \param _ui32W The width of the source texture.
	 * \param _ui32H The height of the source texture.
	 * \param _ui32D The depth of the source texture.
	 * \param _dGamma The gamma to apply to the source texture.
	 **/
	void CFormat::ApplyGamma( uint8_t * _pui8Src, uint32_t _ui32W, uint32_t _ui32H, uint32_t _ui32D,
		double _dGamma ) {
		if ( _dGamma == 0.0 || _dGamma == 1.0 ) { return; }
		uint32_t ui32Pitch = _ui32W;
		uint32_t ui32Slice = ui32Pitch * _ui32H;
		SL2_RGBA64F * prgbDst = reinterpret_cast<SL2_RGBA64F *>(_pui8Src);
		for ( uint32_t D = 0; D < _ui32D; ++D ) {
			for ( uint32_t H = 0; H < _ui32H; ++H ) {
				if ( _dGamma < -1.0 ) {
					for ( uint32_t W = 0; W < _ui32W; ++W ) {
						size_t sIdx = ui32Slice * D + ui32Pitch * H + W;
						prgbDst[sIdx].dRgba[SL2_PC_R] = CUtilities::LinearToSRgb( prgbDst[sIdx].dRgba[SL2_PC_R] );
						prgbDst[sIdx].dRgba[SL2_PC_G] = CUtilities::LinearToSRgb( prgbDst[sIdx].dRgba[SL2_PC_G] );
						prgbDst[sIdx].dRgba[SL2_PC_B] = CUtilities::LinearToSRgb( prgbDst[sIdx].dRgba[SL2_PC_B] );
					}
				}
				else if ( _dGamma < -0.0 ) {
					for ( uint32_t W = 0; W < _ui32W; ++W ) {
						size_t sIdx = ui32Slice * D + ui32Pitch * H + W;
						prgbDst[sIdx].dRgba[SL2_PC_R] = CUtilities::SRgbToLinear( prgbDst[sIdx].dRgba[SL2_PC_R] );
						prgbDst[sIdx].dRgba[SL2_PC_G] = CUtilities::SRgbToLinear( prgbDst[sIdx].dRgba[SL2_PC_G] );
						prgbDst[sIdx].dRgba[SL2_PC_B] = CUtilities::SRgbToLinear( prgbDst[sIdx].dRgba[SL2_PC_B] );
					}
				}
				else {
					for ( uint32_t W = 0; W < _ui32W; ++W ) {
						size_t sIdx = ui32Slice * D + ui32Pitch * H + W;
						prgbDst[sIdx].dRgba[SL2_PC_R] = std::pow( prgbDst[sIdx].dRgba[SL2_PC_R], _dGamma );
						prgbDst[sIdx].dRgba[SL2_PC_G] = std::pow( prgbDst[sIdx].dRgba[SL2_PC_G], _dGamma );
						prgbDst[sIdx].dRgba[SL2_PC_B] = std::pow( prgbDst[sIdx].dRgba[SL2_PC_B], _dGamma );
					}
				}
			}
		}
	}

	/**
	 * Applies a swizzle to a given texture.
	 * 
	 * \param _pui8Src The source texture to expand.
	 * \param _ui32W The width of the source texture.
	 * \param _ui32H The height of the source texture.
	 * \param _ui32D The depth of the source texture.
	 * \param _sSwizzle The swizzle to apply.
	 **/
	void CFormat::ApplySwizzle( uint8_t * _pui8Src, uint32_t _ui32W, uint32_t _ui32H, uint32_t _ui32D,
		const SL2_SWIZZLE &_sSwizzle ) {
		uint32_t ui32Pitch = _ui32W;
		uint32_t ui32Slice = ui32Pitch * _ui32H;
		SL2_RGBA64F * prgbDst = reinterpret_cast<SL2_RGBA64F *>(_pui8Src);
		for ( uint32_t D = 0; D < _ui32D; ++D ) {
			for ( uint32_t H = 0; H < _ui32H; ++H ) {
				for ( uint32_t W = 0; W < _ui32W; ++W ) {
					size_t sIdx = ui32Slice * D + ui32Pitch * H + W;
					SL2_RGBA64F rgbCopy = prgbDst[sIdx];
					for ( size_t I = 0; I < 4; ++I ) {
						switch ( _sSwizzle.pcComp[I] ) {
							case SL2_PC_ZERO : {
								prgbDst[sIdx].dRgba[I] = 0.0;
								break;
							}
							case SL2_PC_ONE : {
								prgbDst[sIdx].dRgba[I] = 1.0;
								break;
							}
							default : {
								prgbDst[sIdx].dRgba[I] = rgbCopy.dRgba[_sSwizzle.pcComp[I]];
							}
						}
					}
				}
			}
		}
	}

	/**
	 * Pre-multiply the alpha values in the given RGBA64F texture.
	 * 
	 * \param _pui8Src The source texture.
	 * \param _ui32W The width of the source texture.
	 * \param _ui32H The height of the source texture.
	 * \param _ui32D The depth of the source texture.
	 **/
	void CFormat::ApplyPreMultiply( uint8_t * _pui8Src, uint32_t _ui32W, uint32_t _ui32H, uint32_t _ui32D ) {
		uint32_t ui32Pitch = _ui32W;
		uint32_t ui32Slice = ui32Pitch * _ui32H;
		SL2_RGBA64F * prgbDst = reinterpret_cast<SL2_RGBA64F *>(_pui8Src);
		for ( uint32_t D = 0; D < _ui32D; ++D ) {
			for ( uint32_t H = 0; H < _ui32H; ++H ) {
				for ( uint32_t W = 0; W < _ui32W; ++W ) {
					size_t sIdx = ui32Slice * D + ui32Pitch * H + W;
					prgbDst[sIdx].dRgba[SL2_PC_R] = prgbDst[sIdx].dRgba[SL2_PC_R] * prgbDst[sIdx].dRgba[SL2_PC_A];
					prgbDst[sIdx].dRgba[SL2_PC_G] = prgbDst[sIdx].dRgba[SL2_PC_G] * prgbDst[sIdx].dRgba[SL2_PC_A];
					prgbDst[sIdx].dRgba[SL2_PC_B] = prgbDst[sIdx].dRgba[SL2_PC_B] * prgbDst[sIdx].dRgba[SL2_PC_A];
				}
			}
		}
	}

	/**
	 * Flips an RGBA64F texture vertically.
	 * 
	 * \param _pui8Src The source texture.
	 * \param _ui32W The width of the source texture.
	 * \param _ui32H The height of the source texture.
	 * \param _ui32D The depth of the source texture.
	 **/
	void CFormat::FlipY( uint8_t * _pui8Src, uint32_t _ui32W, uint32_t _ui32H, uint32_t _ui32D ) {
		uint32_t ui32Pitch = _ui32W;
		uint32_t ui32Slice = ui32Pitch * _ui32H;
		SL2_RGBA64F * prgbDst = reinterpret_cast<SL2_RGBA64F *>(_pui8Src);
		uint32_t ui32HalfH = _ui32H >> 1;
		for ( uint32_t D = 0; D < _ui32D; ++D ) {
			for ( uint32_t H = 0; H < ui32HalfH; ++H ) {
				for ( uint32_t W = 0; W < _ui32W; ++W ) {
					size_t sIdx0 = ui32Slice * D + ui32Pitch * H + W;
					size_t sIdx1 = ui32Slice * D + ui32Pitch * (_ui32H - 1 - H) + W;
					std::swap( prgbDst[sIdx0], prgbDst[sIdx1] );
				}
			}
		}
	}

	/**
	 * Flips an RGBA64F texture horizontally.
	 * 
	 * \param _pui8Src The source texture.
	 * \param _ui32W The width of the source texture.
	 * \param _ui32H The height of the source texture.
	 * \param _ui32D The depth of the source texture.
	 **/
	void CFormat::FlipX( uint8_t * _pui8Src, uint32_t _ui32W, uint32_t _ui32H, uint32_t _ui32D ) {
		uint32_t ui32Pitch = _ui32W;
		uint32_t ui32Slice = ui32Pitch * _ui32H;
		SL2_RGBA64F * prgbDst = reinterpret_cast<SL2_RGBA64F *>(_pui8Src);
		uint32_t ui32HalfW = _ui32W >> 1;
		for ( uint32_t D = 0; D < _ui32D; ++D ) {
			for ( uint32_t H = 0; H < _ui32H; ++H ) {
				for ( uint32_t W = 0; W < ui32HalfW; ++W ) {
					size_t sIdx0 = ui32Slice * D + ui32Pitch * H + W;
					size_t sIdx1 = ui32Slice * D + ui32Pitch * H + (_ui32W - 1 - W);
					std::swap( prgbDst[sIdx0], prgbDst[sIdx1] );
				}
			}
		}
	}

	/**
	 * Flips an RGBA64F texture depth.
	 * 
	 * \param _pui8Src The source texture.
	 * \param _ui32W The width of the source texture.
	 * \param _ui32H The height of the source texture.
	 * \param _ui32D The depth of the source texture.
	 **/
	void CFormat::FlipZ( uint8_t * _pui8Src, uint32_t _ui32W, uint32_t _ui32H, uint32_t _ui32D ) {
		uint32_t ui32Pitch = _ui32W;
		uint32_t ui32Slice = ui32Pitch * _ui32H;
		SL2_RGBA64F * prgbDst = reinterpret_cast<SL2_RGBA64F *>(_pui8Src);
		uint32_t ui32HalfD = _ui32D >> 1;
		for ( uint32_t D = 0; D < ui32HalfD; ++D ) {
			for ( uint32_t H = 0; H < _ui32H; ++H ) {
				for ( uint32_t W = 0; W < _ui32W; ++W ) {
					size_t sIdx0 = ui32Slice * D + ui32Pitch * H + W;
					size_t sIdx1 = ui32Slice * (_ui32D - 1 - D) + ui32Pitch * H + W;
					std::swap( prgbDst[sIdx0], prgbDst[sIdx1] );
				}
			}
		}
	}

	/**
	 * Swaps the R and B channels in a givem RGBA64F texture.
	 * 
	 * \param param _pui8Src The source texture.
	 * \param _ui32W The width of the source texture.
	 * \param _ui32H The height of the source texture.
	 * \param _ui32D The depth of the source texture.
	 **/
	void CFormat::Swap( uint8_t * _pui8Src, uint32_t _ui32W, uint32_t _ui32H, uint32_t _ui32D ) {
		uint32_t ui32Pitch = _ui32W;
		uint32_t ui32Slice = ui32Pitch * _ui32H;
		SL2_RGBA64F * prgbDst = reinterpret_cast<SL2_RGBA64F *>(_pui8Src);
		uint32_t ui32HalfD = _ui32D >> 1;
		for ( uint32_t D = 0; D < ui32HalfD; ++D ) {
			for ( uint32_t H = 0; H < _ui32H; ++H ) {
				for ( uint32_t W = 0; W < _ui32W; ++W ) {
					size_t sIdx = ui32Slice * D + ui32Pitch * H + W;
					std::swap( prgbDst[sIdx].dRgba[SL2_PC_R], prgbDst[sIdx].dRgba[SL2_PC_B] );
				}
			}
		}
	}

	/**
	 * Converts an RGBA64F texture from CYMK to RGB.
	 * 
	 * \param _pui8Src The source texture.
	 * \param _ui32W The width of the source texture.
	 * \param _ui32H The height of the source texture.
	 * \param _ui32D The depth of the source texture.
	 **/
	void CFormat::CymkToRgb( uint8_t * _pui8Src, uint32_t _ui32W, uint32_t _ui32H, uint32_t _ui32D ) {
		uint32_t ui32Pitch = _ui32W;
		uint32_t ui32Slice = ui32Pitch * _ui32H;
		SL2_RGBA64F * prgbDst = reinterpret_cast<SL2_RGBA64F *>(_pui8Src);
		for ( uint32_t D = 0; D < _ui32D; ++D ) {
			for ( uint32_t H = 0; H < _ui32H; ++H ) {
				for ( uint32_t W = 0; W < _ui32W; ++W ) {
					size_t sIdx = ui32Slice * D + ui32Pitch * H + W;
					double dK = 1.0 - prgbDst[sIdx].dRgba[SL2_PC_A] * 2.55;
					prgbDst[sIdx].dRgba[SL2_PC_R] = (1.0 - prgbDst[sIdx].dRgba[SL2_PC_R] * 2.55) * dK;
					prgbDst[sIdx].dRgba[SL2_PC_G] = (1.0 - prgbDst[sIdx].dRgba[SL2_PC_G] * 2.55) * dK;
					prgbDst[sIdx].dRgba[SL2_PC_B] = (1.0 - prgbDst[sIdx].dRgba[SL2_PC_B] * 2.55) * dK;
					prgbDst[sIdx].dRgba[SL2_PC_A] = 1.0;
				}
			}
		}
	}

	/**
	 * Converts a single RGBA64F value to Lab.  Value is already in linear space and normalized.
	 * 
	 * \param _rgbVal The value to convert.
	 * \return Returns the converted Lab color.
	 **/
	CFormat::SL2_RGBA64F CFormat::ToLab( const SL2_RGBA64F &_rgbVal ) {
		SL2_RGBA64F rgbTmp;
		rgbTmp.dRgba[0] = _rgbVal.dRgba[0] * 100.0;
		rgbTmp.dRgba[1] = _rgbVal.dRgba[1] * 100.0;
		rgbTmp.dRgba[2] = _rgbVal.dRgba[2] * 100.0;
		rgbTmp.dRgba[3] = _rgbVal.dRgba[3];

		SL2_RGBA64F rgbXyz;
		rgbXyz.dRgba[0] = rgbTmp.dRgba[0] * 0.4124 + rgbTmp.dRgba[1] * 0.3576 + rgbTmp.dRgba[2] * 0.1805;
		rgbXyz.dRgba[1] = rgbTmp.dRgba[0] * 0.2126 + rgbTmp.dRgba[1] * 0.7152 + rgbTmp.dRgba[2] * 0.0722;
		rgbXyz.dRgba[2] = rgbTmp.dRgba[0] * 0.0193 + rgbTmp.dRgba[1] * 0.1192 + rgbTmp.dRgba[2] * 0.9505;

		rgbXyz.dRgba[0] = rgbXyz.dRgba[0] / 95.047;
		rgbXyz.dRgba[1] = rgbXyz.dRgba[1] / 100.0;
		rgbXyz.dRgba[2] = rgbXyz.dRgba[2] / 108.883;

		for ( size_t I = 0; I < 3; ++I ) {
			if ( rgbXyz.dRgba[I] > 0.008856 ) {
				rgbXyz.dRgba[I] = std::pow( rgbXyz.dRgba[I], 1.0 / 3.0 );
			}
			else {
				rgbXyz.dRgba[I] = (7.787 * rgbXyz.dRgba[I]) + (16.0 / 116.0);
			}
		}

		rgbTmp.dRgba[0] = (116.0 * rgbXyz.dRgba[1]) - 16.0;
		rgbTmp.dRgba[1] = 500.0 * (rgbXyz.dRgba[0] - rgbXyz.dRgba[1]);
		rgbTmp.dRgba[2] = 200.0 * (rgbXyz.dRgba[1] - rgbXyz.dRgba[2]);
		return rgbTmp;
	}

	/**
	 * Converts an RGBA64F texture to Lab colors.
	 * 
	 * \param _pui8Src The source texture.
	 * \param _ui32W The width of the source texture.
	 * \param _ui32H The height of the source texture.
	 * \param _ui32D The depth of the source texture.
	 **/
	void CFormat::ToLab( uint8_t * _pui8Src, uint32_t _ui32W, uint32_t _ui32H, uint32_t _ui32D ) {
		uint32_t ui32Pitch = _ui32W;
		uint32_t ui32Slice = ui32Pitch * _ui32H;
		SL2_RGBA64F * prgbDst = reinterpret_cast<SL2_RGBA64F *>(_pui8Src);
		for ( uint32_t D = 0; D < _ui32D; ++D ) {
			for ( uint32_t H = 0; H < _ui32H; ++H ) {
				for ( uint32_t W = 0; W < _ui32W; ++W ) {
					size_t sIdx = ui32Slice * D + ui32Pitch * H + W;
					prgbDst[sIdx] = ToLab( prgbDst[sIdx] );
				}
			}
		}
	}

	/**
	 * Gathers luminance into the R channel of the given RGBA64F texture.
	 * 
	 * \param _pui8Src The source texture.
	 * \param _ui32W The width of the source texture.
	 * \param _ui32H The height of the source texture.
	 * \param _ui32D The depth of the source texture.
	 **/
	void CFormat::LumaToR( uint8_t * _pui8Src, uint32_t _ui32W, uint32_t _ui32H, uint32_t _ui32D ) {
		uint32_t ui32Pitch = _ui32W;
		uint32_t ui32Slice = ui32Pitch * _ui32H;
		SL2_RGBA64F * prgbDst = reinterpret_cast<SL2_RGBA64F *>(_pui8Src);
		for ( uint32_t D = 0; D < _ui32D; ++D ) {
			for ( uint32_t H = 0; H < _ui32H; ++H ) {
				for ( uint32_t W = 0; W < _ui32W; ++W ) {
					size_t sIdx = ui32Slice * D + ui32Pitch * H + W;
					prgbDst[sIdx].dRgba[SL2_PC_R] = m_lCurCoeffs.dRgb[0] * prgbDst[sIdx].dRgba[SL2_PC_R] +
						m_lCurCoeffs.dRgb[1] * prgbDst[sIdx].dRgba[SL2_PC_G] +
						m_lCurCoeffs.dRgb[2] * prgbDst[sIdx].dRgba[SL2_PC_B];
				}
			}
		}
	}

	/**
	 * Converts an RGBA64F texture to RGBA16F in-place.
	 * 
	 * \param _pui8Src The source texture.
	 * \param _ui32W The width of the source texture.
	 * \param _ui32H The height of the source texture.
	 * \param _ui32D The depth of the source texture.
	 **/
	void CFormat::ToF16( uint8_t * _pui8Src, uint32_t _ui32W, uint32_t _ui32H, uint32_t _ui32D ) {
		uint32_t ui32Pitch = _ui32W;
		uint32_t ui32Slice = ui32Pitch * _ui32H;
		struct SL2_RGBAF16 {
			CFloat16 fRgba[4];
		};
		SL2_RGBAF16 * prgbDst = reinterpret_cast<SL2_RGBAF16 *>(_pui8Src);
		SL2_RGBA64F * prgbSrc = reinterpret_cast<SL2_RGBA64F *>(_pui8Src);
		CFloat16 f16One = CFloat16( 1.0 );
		for ( uint32_t D = 0; D < _ui32D; ++D ) {
			for ( uint32_t H = 0; H < _ui32H; ++H ) {
				for ( uint32_t W = 0; W < _ui32W; ++W ) {
					size_t sIdx = ui32Slice * D + ui32Pitch * H + W;
					prgbDst[sIdx].fRgba[SL2_PC_R] = CFloat16( prgbSrc[sIdx].dRgba[SL2_PC_R] );
					prgbDst[sIdx].fRgba[SL2_PC_G] = CFloat16( prgbSrc[sIdx].dRgba[SL2_PC_G] );
					prgbDst[sIdx].fRgba[SL2_PC_B] = CFloat16( prgbSrc[sIdx].dRgba[SL2_PC_B] );
					prgbDst[sIdx].fRgba[SL2_PC_A] = f16One;
				}
			}
		}
	}

	/**
	 * Converts an RGBA64F texture to RGB8 in-place.
	 * 
	 * \param _pui8Src The source texture.
	 * \param _ui32W The width of the source texture.
	 * \param _ui32H The height of the source texture.
	 * \param _ui32D The depth of the source texture.
	 **/
	void CFormat::ToRGB8( uint8_t * _pui8Src, uint32_t _ui32W, uint32_t _ui32H, uint32_t _ui32D ) {
		uint32_t ui32Pitch = _ui32W;
		uint32_t ui32Slice = ui32Pitch * _ui32H;
		SL2_RGB_UNORM * prgbDst = reinterpret_cast<SL2_RGB_UNORM *>(_pui8Src);
		SL2_RGBA64F * prgbSrc = reinterpret_cast<SL2_RGBA64F *>(_pui8Src);
		for ( uint32_t D = 0; D < _ui32D; ++D ) {
			for ( uint32_t H = 0; H < _ui32H; ++H ) {
				for ( uint32_t W = 0; W < _ui32W; ++W ) {
					size_t sIdx = ui32Slice * D + ui32Pitch * H + W;
					prgbDst[sIdx].ui8Rgb[SL2_PC_R] = uint8_t( std::round( CUtilities::Clamp<double>( prgbSrc[sIdx].dRgba[SL2_PC_R], 0.0, 1.0 ) * 255.0 ) );
					prgbDst[sIdx].ui8Rgb[SL2_PC_G] = uint8_t( std::round( CUtilities::Clamp<double>( prgbSrc[sIdx].dRgba[SL2_PC_G], 0.0, 1.0 ) * 255.0 ) );
					prgbDst[sIdx].ui8Rgb[SL2_PC_B] = uint8_t( std::round( CUtilities::Clamp<double>( prgbSrc[sIdx].dRgba[SL2_PC_B], 0.0, 1.0 ) * 255.0 ) );
				}
			}
		}
	}

	/**
	 * Converts an RGBA64F texture to RGBA8 in-place.
	 * 
	 * \param _pui8Src The source texture.
	 * \param _ui32W The width of the source texture.
	 * \param _ui32H The height of the source texture.
	 * \param _ui32D The depth of the source texture.
	 **/
	void CFormat::ToRGBA8( uint8_t * _pui8Src, uint32_t _ui32W, uint32_t _ui32H, uint32_t _ui32D ) {
		uint32_t ui32Pitch = _ui32W;
		uint32_t ui32Slice = ui32Pitch * _ui32H;
		SL2_RGBA_UNORM * prgbDst = reinterpret_cast<SL2_RGBA_UNORM *>(_pui8Src);
		SL2_RGBA64F * prgbSrc = reinterpret_cast<SL2_RGBA64F *>(_pui8Src);
		for ( uint32_t D = 0; D < _ui32D; ++D ) {
			for ( uint32_t H = 0; H < _ui32H; ++H ) {
				for ( uint32_t W = 0; W < _ui32W; ++W ) {
					size_t sIdx = ui32Slice * D + ui32Pitch * H + W;
					prgbDst[sIdx].ui8Rgba[SL2_PC_R] = uint8_t( std::round( CUtilities::Clamp<double>( prgbSrc[sIdx].dRgba[SL2_PC_R], 0.0, 1.0 ) * 255.0 ) );
					prgbDst[sIdx].ui8Rgba[SL2_PC_G] = uint8_t( std::round( CUtilities::Clamp<double>( prgbSrc[sIdx].dRgba[SL2_PC_G], 0.0, 1.0 ) * 255.0 ) );
					prgbDst[sIdx].ui8Rgba[SL2_PC_B] = uint8_t( std::round( CUtilities::Clamp<double>( prgbSrc[sIdx].dRgba[SL2_PC_B], 0.0, 1.0 ) * 255.0 ) );
					prgbDst[sIdx].ui8Rgba[SL2_PC_A] = uint8_t( std::round( CUtilities::Clamp<double>( prgbSrc[sIdx].dRgba[SL2_PC_A], 0.0, 1.0 ) * 255.0 ) );
				}
			}
		}
	}

	/**
	 * Converts an RGBA64F texture to RGB8 and A8 in-place (RGB8) and as a copy (A8).
	 * 
	 * \param _pui8Src The source texture.
	 * \param _ui32W The width of the source texture.
	 * \param _ui32H The height of the source texture.
	 * \param _ui32D The depth of the source texture.
	 * \param _vAlpha Holds the alpha channel on output.
	 * \param _i32Thresh The alpha threshhold for punch-through formats.
	 * \return Returns true if the alpha channel was able to be allocated.
	 **/
	bool CFormat::ToRGB8A8( uint8_t * _pui8Src, uint32_t _ui32W, uint32_t _ui32H, uint32_t _ui32D,
		std::vector<uint8_t> &_vAlpha, int32_t _i32Thresh ) {
		try {
			_vAlpha.resize( _ui32H * _ui32W * _ui32D );
		}
		catch ( ... ) { return false; }
		uint32_t ui32Pitch = _ui32W;
		uint32_t ui32Slice = ui32Pitch * _ui32H;
		SL2_RGB_UNORM * prgbDst = reinterpret_cast<SL2_RGB_UNORM *>(_pui8Src);
		uint8_t * pui8DstA = reinterpret_cast<uint8_t *>(_vAlpha.data());
		SL2_RGBA64F * prgbSrc = reinterpret_cast<SL2_RGBA64F *>(_pui8Src);
		for ( uint32_t D = 0; D < _ui32D; ++D ) {
			for ( uint32_t H = 0; H < _ui32H; ++H ) {
				for ( uint32_t W = 0; W < _ui32W; ++W ) {
					size_t sIdx = ui32Slice * D + ui32Pitch * H + W;
					prgbDst[sIdx].ui8Rgb[SL2_PC_R] = uint8_t( std::round( CUtilities::Clamp<double>( prgbSrc[sIdx].dRgba[SL2_PC_R], 0.0, 1.0 ) * 255.0 ) );
					prgbDst[sIdx].ui8Rgb[SL2_PC_G] = uint8_t( std::round( CUtilities::Clamp<double>( prgbSrc[sIdx].dRgba[SL2_PC_G], 0.0, 1.0 ) * 255.0 ) );
					prgbDst[sIdx].ui8Rgb[SL2_PC_B] = uint8_t( std::round( CUtilities::Clamp<double>( prgbSrc[sIdx].dRgba[SL2_PC_B], 0.0, 1.0 ) * 255.0 ) );
					if ( _i32Thresh < 0 ) {
						pui8DstA[sIdx] = uint8_t( std::round( CUtilities::Clamp<double>( prgbSrc[sIdx].dRgba[SL2_PC_A], 0.0, 1.0 ) * 255.0 ) );
					}
					else {
						uint8_t ui8Alpha = uint8_t( std::round( CUtilities::Clamp<double>( prgbSrc[sIdx].dRgba[SL2_PC_A], 0.0, 1.0 ) * 255.0 ) );
						pui8DstA[sIdx] = ui8Alpha < _i32Thresh ? 0 : 255;
					}
				}
			}
		}
		return true;
	}

	/**
	 * Converts an RGBA64F texture to 11-bit R represented as 16-bit values in-place.
	 * 
	 * \param _pui8Src The source texture.
	 * \param _ui32W The width of the source texture.
	 * \param _ui32H The height of the source texture.
	 * \param _ui32D The depth of the source texture.
	 * \param _bSigned Whether to translate the values to a signed format.
	 **/
	void CFormat::ToR11( uint8_t * _pui8Src, uint32_t _ui32W, uint32_t _ui32H, uint32_t _ui32D, bool _bSigned ) {
		uint32_t ui32Pitch = _ui32W;
		uint32_t ui32Slice = ui32Pitch * _ui32H;
		uint16_t * pui16Dst = reinterpret_cast<uint16_t *>(_pui8Src);
		SL2_RGBA64F * prgbSrc = reinterpret_cast<SL2_RGBA64F *>(_pui8Src);
		for ( uint32_t D = 0; D < _ui32D; ++D ) {
			for ( uint32_t H = 0; H < _ui32H; ++H ) {
				for ( uint32_t W = 0; W < _ui32W; ++W ) {
					size_t sIdx = ui32Slice * D + ui32Pitch * H + W;
					if ( _bSigned ) {
						pui16Dst[sIdx] = uint16_t( std::round( (CUtilities::Clamp<double>( prgbSrc[sIdx].dRgba[SL2_PC_R], -1.0, 1.0 ) / 2.0 + 0.5) * 65534.0 ) ) + 1;
					}
					else {
						pui16Dst[sIdx] = uint16_t( std::round( CUtilities::Clamp<double>( prgbSrc[sIdx].dRgba[SL2_PC_R], 0.0, 1.0 ) * 65535.0 ) );
					}
					pui16Dst[sIdx] = ::_byteswap_ushort( pui16Dst[sIdx] );
				}
			}
		}
	}

	/**
	 * Converts an RGBA64F texture to 11-bit R represented as 16-bit values in-place.
	 * 
	 * \param _pui8Src The source texture.
	 * \param _ui32W The width of the source texture.
	 * \param _ui32H The height of the source texture.
	 * \param _ui32D The depth of the source texture.
	 * \param _bSigned Whether to translate the values to a signed format.
	 * \param _vGreen Holds the green values.
	 * \return Returns true if the green buffer was allocated.
	 **/
	bool CFormat::ToRG11( uint8_t * _pui8Src, uint32_t _ui32W, uint32_t _ui32H, uint32_t _ui32D,
		std::vector<uint8_t> &_vGreen, bool _bSigned ) {
		uint32_t ui32Pitch = _ui32W;
		uint32_t ui32Slice = ui32Pitch * _ui32H;
		try {
			_vGreen.resize( ui32Slice * _ui32D * sizeof( uint16_t ) );
		}
		catch ( ... ) { return false; }

		uint16_t * pui16DstR = reinterpret_cast<uint16_t *>(_pui8Src);
		uint16_t * pui16DstG = reinterpret_cast<uint16_t *>(_vGreen.data());
		SL2_RGBA64F * prgbSrc = reinterpret_cast<SL2_RGBA64F *>(_pui8Src);
		for ( uint32_t D = 0; D < _ui32D; ++D ) {
			for ( uint32_t H = 0; H < _ui32H; ++H ) {
				for ( uint32_t W = 0; W < _ui32W; ++W ) {
					size_t sIdx = ui32Slice * D + ui32Pitch * H + W;
					if ( _bSigned ) {
						pui16DstG[sIdx] = uint16_t( std::round( (CUtilities::Clamp<double>( prgbSrc[sIdx].dRgba[SL2_PC_G], -1.0, 1.0 ) / 2.0 + 0.5) * 65534.0 ) ) + 1;
						pui16DstR[sIdx] = uint16_t( std::round( (CUtilities::Clamp<double>( prgbSrc[sIdx].dRgba[SL2_PC_R], -1.0, 1.0 ) / 2.0 + 0.5) * 65534.0 ) ) + 1;
					}
					else {
						pui16DstG[sIdx] = uint16_t( std::round( CUtilities::Clamp<double>( prgbSrc[sIdx].dRgba[SL2_PC_G], 0.0, 1.0 ) * 65535.0 ) );
						pui16DstR[sIdx] = uint16_t( std::round( CUtilities::Clamp<double>( prgbSrc[sIdx].dRgba[SL2_PC_R], 0.0, 1.0 ) * 65535.0 ) );
					}
					pui16DstR[sIdx] = ::_byteswap_ushort( pui16DstR[sIdx] );
					pui16DstG[sIdx] = ::_byteswap_ushort( pui16DstG[sIdx] );
				}
			}
		}
		return true;
	}

	/**
	 * Converts an RGBA64F texture to RGBAF32 in-place.
	 * 
	 * \param _pui8Src The source texture.
	 * \param _ui32W The width of the source texture.
	 * \param _ui32H The height of the source texture.
	 * \param _ui32D The depth of the source texture.
	 **/
	void CFormat::ToRGBA32F( uint8_t * _pui8Src, uint32_t _ui32W, uint32_t _ui32H, uint32_t _ui32D ) {
		uint32_t ui32Pitch = _ui32W;
		uint32_t ui32Slice = ui32Pitch * _ui32H;
		SL2_RGBA * prgbDst = reinterpret_cast<SL2_RGBA *>(_pui8Src);
		SL2_RGBA64F * prgbSrc = reinterpret_cast<SL2_RGBA64F *>(_pui8Src);
		for ( uint32_t D = 0; D < _ui32D; ++D ) {
			for ( uint32_t H = 0; H < _ui32H; ++H ) {
				for ( uint32_t W = 0; W < _ui32W; ++W ) {
					size_t sIdx = ui32Slice * D + ui32Pitch * H + W;
					prgbDst[sIdx].fRgba[SL2_PC_R] = float( prgbSrc[sIdx].dRgba[SL2_PC_R] );
					prgbDst[sIdx].fRgba[SL2_PC_G] = float( prgbSrc[sIdx].dRgba[SL2_PC_G] );
					prgbDst[sIdx].fRgba[SL2_PC_B] = float( prgbSrc[sIdx].dRgba[SL2_PC_B] );
					prgbDst[sIdx].fRgba[SL2_PC_A] = float( prgbSrc[sIdx].dRgba[SL2_PC_A] );
				}
			}
		}
	}

	/**
	 * Prints the formats in a single long list.
	 **/
	void CFormat::PrintFormats_List() {
		std::string sTmp;
		for ( size_t I = 0; I < SL2_ELEMENTS( m_kifdInternalFormats ); ++I ) {
			if ( m_kifdInternalFormats[I].vfVulkanFormat != SL2_VK_FORMAT_UNDEFINED ) {
				sTmp += m_kifdInternalFormats[I].pcVulkanName;
				sTmp += "\r\n";
			}
		}

		for ( size_t I = 0; I < SL2_ELEMENTS( m_kifdInternalFormats ); ++I ) {
			if ( m_kifdInternalFormats[I].dfDxFormat != SL2_DXGI_FORMAT_UNKNOWN ) {
				sTmp += m_kifdInternalFormats[I].pcDxName;
				sTmp += "\r\n";
			}
		}

		for ( size_t I = 0; I < SL2_ELEMENTS( m_kifdInternalFormats ); ++I ) {
			if ( m_kifdInternalFormats[I].mfMetalFormat != SL2_MTLPixelFormatInvalid ) {
				sTmp += m_kifdInternalFormats[I].pcMetalName;
				sTmp += "\r\n";
			}
		}

		for ( size_t I = 0; I < SL2_ELEMENTS( m_kifdInternalFormats ); ++I ) {
			if ( m_kifdInternalFormats[I].kifInternalFormat != SL2_KIF_GL_INVALID ) {
				sTmp += m_kifdInternalFormats[I].pcOglInternalFormat;
				sTmp += "\t| ";
				sTmp += m_kifdInternalFormats[I].pcOglType;
				sTmp += "\t| ";
				sTmp += m_kifdInternalFormats[I].pcOglBaseInternalFormat;
				sTmp += "\r\n";
			}
		}

		::printf( "%s", sTmp.c_str() );
		::OutputDebugStringA( sTmp.c_str() );
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
	 * \param _pdPalette The created palette (contains 16 entries).
	 */
	void CFormat::DecodeDXT3_Alpha( uint64_t _ui64Block, double * _pdPalette ) {
		for ( uint32_t I = 0; I < 16; ++I ) {
			(*_pdPalette++) = (_ui64Block & 0xF) / 15.0;
			_ui64Block >>= 4;
		}
	}

	/**
	 * Decodes a single block of BC4U.
	 *
	 * \param _ui64Block The block to decode.
	 * \param _pdPalette The created palette (contains 8 entries).
	 */
	void CFormat::DecodeBC4U( uint64_t _ui64Block, double * _pdPalette ) {
		_pdPalette[0] = ((_ui64Block >> 0) & 0xFF) / 255.0;
		_pdPalette[1] = ((_ui64Block >> 8) & 0xFF) / 255.0;
		if ( _pdPalette[0] > _pdPalette[1] ) {
			// 6 interpolated color values.
			_pdPalette[2] = (6.0 * _pdPalette[0] + 1.0 * _pdPalette[1]) / 7.0;		// Bit code 010.
			_pdPalette[3] = (5.0 * _pdPalette[0] + 2.0 * _pdPalette[1]) / 7.0;		// Bit code 011.
			_pdPalette[4] = (4.0 * _pdPalette[0] + 3.0 * _pdPalette[1]) / 7.0;		// Bit code 100.
			_pdPalette[5] = (3.0 * _pdPalette[0] + 4.0 * _pdPalette[1]) / 7.0;		// Bit code 101.
			_pdPalette[6] = (2.0 * _pdPalette[0] + 5.0 * _pdPalette[1]) / 7.0;		// Bit code 110.
			_pdPalette[7] = (1.0 * _pdPalette[0] + 6.0 * _pdPalette[1]) / 7.0;		// Bit code 111.
		}
		else {
			// 4 interpolated color values.
			_pdPalette[2] = (4.0 * _pdPalette[0] + 1.0 * _pdPalette[1]) / 5.0;		// Bit code 010.
			_pdPalette[3] = (3.0 * _pdPalette[0] + 2.0 * _pdPalette[1]) / 5.0;		// Bit code 011.
			_pdPalette[4] = (2.0 * _pdPalette[0] + 3.0 * _pdPalette[1]) / 5.0;		// Bit code 100.
			_pdPalette[5] = (1.0 * _pdPalette[0] + 4.0 * _pdPalette[1]) / 5.0;		// Bit code 101.
			_pdPalette[6] = 0.0;													// Bit code 110.
			_pdPalette[7] = 1.0;													// Bit code 111.
		}
	}

	/**
	 * Decodes a single block of BC4S.
	 *
	 * \param _ui64Block The block to decode.
	 * \param _pdPalette The created palette (contains 8 entries).
	 */
	void CFormat::DecodeBC4S( uint64_t _ui64Block, double * _pdPalette ) {
		int8_t i8C0 = static_cast<int8_t>(_ui64Block >> 0);
		int8_t i8C1 = static_cast<int8_t>(_ui64Block >> 8);
		_pdPalette[0] = i8C0 == -128 ? -1.0 : i8C0 / 127.0;
		_pdPalette[1] = i8C1 == -128 ? -1.0 : i8C1 / 127.0;
		if ( _pdPalette[0] > _pdPalette[1] ) {
			// 6 interpolated color values.
			_pdPalette[2] = (6.0 * _pdPalette[0] + 1.0 * _pdPalette[1]) / 7.0;		// Bit code 010.
			_pdPalette[3] = (5.0 * _pdPalette[0] + 2.0 * _pdPalette[1]) / 7.0;		// Bit code 011.
			_pdPalette[4] = (4.0 * _pdPalette[0] + 3.0 * _pdPalette[1]) / 7.0;		// Bit code 100.
			_pdPalette[5] = (3.0 * _pdPalette[0] + 4.0 * _pdPalette[1]) / 7.0;		// Bit code 101.
			_pdPalette[6] = (2.0 * _pdPalette[0] + 5.0 * _pdPalette[1]) / 7.0;		// Bit code 110.
			_pdPalette[7] = (1.0 * _pdPalette[0] + 6.0 * _pdPalette[1]) / 7.0;		// Bit code 111.
		}
		else {
			// 4 interpolated color values.
			_pdPalette[2] = (4.0 * _pdPalette[0] + 1.0 * _pdPalette[1]) / 5.0;		// Bit code 010.
			_pdPalette[3] = (3.0 * _pdPalette[0] + 2.0 * _pdPalette[1]) / 5.0;		// Bit code 011.
			_pdPalette[4] = (2.0 * _pdPalette[0] + 3.0 * _pdPalette[1]) / 5.0;		// Bit code 100.
			_pdPalette[5] = (1.0 * _pdPalette[0] + 4.0 * _pdPalette[1]) / 5.0;		// Bit code 101.
			_pdPalette[6] = -1.0;													// Bit code 110.
			_pdPalette[7] = 1.0;													// Bit code 111.
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

	/**
	 * Depth-16 -> RGBA64F conversion.
	 *
	 * \param _pui8Src Source texels.
	 * \param _pui8Dst Destination texels known to be in RGBA64F format.
	 * \param _ui32Width Width of the image.
	 * \param _ui32Height Height of the image.
	 * \param _ui32Depth Depth of the image.
	 * \param _pvParms Optional parameters for the conversion.
	 */
	bool CFormat::Depth16ToRgba64F( const uint8_t * _pui8Src, uint8_t * _pui8Dst, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Depth, const void * _pvParms ) {
		struct SL2_D16 {
			uint16_t ui16Val;
		};
		const uint32_t ui32RowSize = sizeof( SL2_RGBA64F ) * _ui32Width;
		const uint32_t ui32PlaneSize = ui32RowSize * _ui32Height;
		const uint32_t ui32SrcRowSize = SL2_ROUND_UP( sizeof( SL2_D16 ) * _ui32Width, 4 );
		const uint32_t ui32SrcPlaneSize = ui32SrcRowSize * _ui32Height;
		
		for ( uint32_t Z = 0; Z < _ui32Depth; ++Z ) {
			for ( uint32_t Y = 0; Y < _ui32Height; ++Y ) {
				for ( uint32_t X = 0; X < _ui32Width; ++X ) {
					SL2_RGBA64F & rgbaThis = reinterpret_cast<SL2_RGBA64F &>(_pui8Dst[Z*ui32PlaneSize+Y*ui32RowSize+X*sizeof(SL2_RGBA64F)]);
					const SL2_D16 * prSrc = reinterpret_cast<const SL2_D16 *>(&_pui8Src[Z*ui32SrcPlaneSize+Y*ui32SrcRowSize+X*sizeof(SL2_D16)]);
					rgbaThis.dRgba[SL2_PC_R] = prSrc->ui16Val / 65535.0;
					rgbaThis.dRgba[SL2_PC_G] = rgbaThis.dRgba[SL2_PC_R];
					rgbaThis.dRgba[SL2_PC_B] = rgbaThis.dRgba[SL2_PC_R];
					rgbaThis.dRgba[SL2_PC_A] = 1.0;
				}
			}
		}
		return true;
	}

	/**
	 * RGBA64F -> Depth-16 conversion.
	 *
	 * \param _pui8Src Source texels known to be in RGBA64F format.
	 * \param _pui8Dst Destination texels.
	 * \param _ui32Width Width of the image.
	 * \param _ui32Height Height of the image.
	 * \param _ui32Depth Depth of the image.
	 * \param _pvParms Optional parameters for the conversion.
	 */
	bool CFormat::Depth16FromRgba64F( const uint8_t * _pui8Src, uint8_t * _pui8Dst, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Depth, const void * _pvParms ) {
		struct SL2_D16 {
			uint16_t ui16Val;
		};
		const uint32_t ui32SrcRowSize = sizeof( SL2_RGBA64F ) * _ui32Width;
		const uint32_t ui32SrcPlaneSize = ui32SrcRowSize * _ui32Height;
		const uint32_t ui32RowSize = SL2_ROUND_UP( sizeof( SL2_D16 ) * _ui32Width, 4 );
		const uint32_t ui32PlaneSize = ui32RowSize * _ui32Height;

		for ( uint32_t Z = 0; Z < _ui32Depth; ++Z ) {
			for ( uint32_t Y = 0; Y < _ui32Height; ++Y ) {
				for ( uint32_t X = 0; X < _ui32Width; ++X ) {
					const SL2_RGBA64F & rgbaThis = reinterpret_cast<const SL2_RGBA64F &>(_pui8Src[Z*ui32SrcPlaneSize+Y*ui32SrcRowSize+X*sizeof(SL2_RGBA64F)]);
					SL2_D16 * prDst = reinterpret_cast<SL2_D16 *>(&_pui8Dst[Z*ui32PlaneSize+Y*ui32RowSize+X*sizeof(SL2_D16)]);
					double dLum = (rgbaThis.dRgba[SL2_PC_R] +
						rgbaThis.dRgba[SL2_PC_G] +
						rgbaThis.dRgba[SL2_PC_B]) / 3.0;
					prDst->ui16Val = static_cast<uint16_t>(dLum * 65535.5);
				}
			}
		}
		return true;
	}

	/**
	 * Depth-24 -> RGBA64F conversion.
	 *
	 * \param _pui8Src Source texels.
	 * \param _pui8Dst Destination texels known to be in RGBA64F format.
	 * \param _ui32Width Width of the image.
	 * \param _ui32Height Height of the image.
	 * \param _ui32Depth Depth of the image.
	 * \param _pvParms Optional parameters for the conversion.
	 */
	bool CFormat::Depth24ToRgba64F( const uint8_t * _pui8Src, uint8_t * _pui8Dst, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Depth, const void * _pvParms ) {
		struct SL2_D24 {
			uint32_t ui32Val;
		};
		const uint32_t ui32RowSize = sizeof( SL2_RGBA64F ) * _ui32Width;
		const uint32_t ui32PlaneSize = ui32RowSize * _ui32Height;
		const uint32_t ui32SrcRowSize = SL2_ROUND_UP( sizeof( SL2_D24 ) * _ui32Width, 4 );
		const uint32_t ui32SrcPlaneSize = ui32SrcRowSize * _ui32Height;
		
		for ( uint32_t Z = 0; Z < _ui32Depth; ++Z ) {
			for ( uint32_t Y = 0; Y < _ui32Height; ++Y ) {
				for ( uint32_t X = 0; X < _ui32Width; ++X ) {
					SL2_RGBA64F & rgbaThis = reinterpret_cast<SL2_RGBA64F &>(_pui8Dst[Z*ui32PlaneSize+Y*ui32RowSize+X*sizeof(SL2_RGBA64F)]);
					const SL2_D24 * prSrc = reinterpret_cast<const SL2_D24 *>(&_pui8Src[Z*ui32SrcPlaneSize+Y*ui32SrcRowSize+X*sizeof(SL2_D24)]);
					rgbaThis.dRgba[SL2_PC_R] = (prSrc->ui32Val >> 8) / 16777215.0;
					rgbaThis.dRgba[SL2_PC_G] = rgbaThis.dRgba[SL2_PC_R];
					rgbaThis.dRgba[SL2_PC_B] = rgbaThis.dRgba[SL2_PC_R];
					rgbaThis.dRgba[SL2_PC_A] = 1.0;
				}
			}
		}
		return true;
	}

	/**
	 * RGBA64F -> Depth-24 conversion.
	 *
	 * \param _pui8Src Source texels known to be in RGBA64F format.
	 * \param _pui8Dst Destination texels.
	 * \param _ui32Width Width of the image.
	 * \param _ui32Height Height of the image.
	 * \param _ui32Depth Depth of the image.
	 * \param _pvParms Optional parameters for the conversion.
	 */
	bool CFormat::Depth24FromRgba64F( const uint8_t * _pui8Src, uint8_t * _pui8Dst, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Depth, const void * _pvParms ) {
		struct SL2_D24 {
			uint32_t ui32Val;
		};
		const uint32_t ui32SrcRowSize = sizeof( SL2_RGBA64F ) * _ui32Width;
		const uint32_t ui32SrcPlaneSize = ui32SrcRowSize * _ui32Height;
		const uint32_t ui32RowSize = SL2_ROUND_UP( sizeof( SL2_D24 ) * _ui32Width, 4 );
		const uint32_t ui32PlaneSize = ui32RowSize * _ui32Height;
		for ( uint32_t Z = 0; Z < _ui32Depth; ++Z ) {
			for ( uint32_t Y = 0; Y < _ui32Height; ++Y ) {
				for ( uint32_t X = 0; X < _ui32Width; ++X ) {
					const SL2_RGBA64F & rgbaThis = reinterpret_cast<const SL2_RGBA64F &>(_pui8Src[Z*ui32SrcPlaneSize+Y*ui32SrcRowSize+X*sizeof(SL2_RGBA64F)]);
					SL2_D24 * prDst = reinterpret_cast<SL2_D24 *>(&_pui8Dst[Z*ui32PlaneSize+Y*ui32RowSize+X*sizeof(SL2_D24)]);
					double dLum = (rgbaThis.dRgba[SL2_PC_R] +
						rgbaThis.dRgba[SL2_PC_G] +
						rgbaThis.dRgba[SL2_PC_B]) / 3.0;
					prDst->ui32Val = static_cast<uint32_t>(dLum * 16777215.5) << 8;
				}
			}
		}
		return true;
	}

	/**
	 * Depth-32 -> RGBA64F conversion.
	 *
	 * \param _pui8Src Source texels.
	 * \param _pui8Dst Destination texels known to be in RGBA64F format.
	 * \param _ui32Width Width of the image.
	 * \param _ui32Height Height of the image.
	 * \param _ui32Depth Depth of the image.
	 * \param _pvParms Optional parameters for the conversion.
	 */
	bool CFormat::Depth32ToRgba64F( const uint8_t * _pui8Src, uint8_t * _pui8Dst, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Depth, const void * _pvParms ) {
		struct SL2_D32 {
			uint32_t ui32Val;
		};
		const uint32_t ui32RowSize = sizeof( SL2_RGBA64F ) * _ui32Width;
		const uint32_t ui32PlaneSize = ui32RowSize * _ui32Height;
		const uint32_t ui32SrcRowSize = SL2_ROUND_UP( sizeof( SL2_D32 ) * _ui32Width, 4 );
		const uint32_t ui32SrcPlaneSize = ui32SrcRowSize * _ui32Height;
		
		for ( uint32_t Z = 0; Z < _ui32Depth; ++Z ) {
			for ( uint32_t Y = 0; Y < _ui32Height; ++Y ) {
				for ( uint32_t X = 0; X < _ui32Width; ++X ) {
					SL2_RGBA64F & rgbaThis = reinterpret_cast<SL2_RGBA64F &>(_pui8Dst[Z*ui32PlaneSize+Y*ui32RowSize+X*sizeof(SL2_RGBA64F)]);
					const SL2_D32 * prSrc = reinterpret_cast<const SL2_D32 *>(&_pui8Src[Z*ui32SrcPlaneSize+Y*ui32SrcRowSize+X*sizeof(SL2_D32)]);
					rgbaThis.dRgba[SL2_PC_R] = prSrc->ui32Val / 4294967295.0;
					rgbaThis.dRgba[SL2_PC_G] = rgbaThis.dRgba[SL2_PC_R];
					rgbaThis.dRgba[SL2_PC_B] = rgbaThis.dRgba[SL2_PC_R];
					rgbaThis.dRgba[SL2_PC_A] = 1.0;
				}
			}
		}
		return true;
	}

	/**
	 * RGBA64F -> Depth-32 conversion.
	 *
	 * \param _pui8Src Source texels known to be in RGBA64F format.
	 * \param _pui8Dst Destination texels.
	 * \param _ui32Width Width of the image.
	 * \param _ui32Height Height of the image.
	 * \param _ui32Depth Depth of the image.
	 * \param _pvParms Optional parameters for the conversion.
	 */
	bool CFormat::Depth32FromRgba64F( const uint8_t * _pui8Src, uint8_t * _pui8Dst, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Depth, const void * _pvParms ) {
		struct SL2_D32 {
			uint32_t ui32Val;
		};
		const uint32_t ui32SrcRowSize = sizeof( SL2_RGBA64F ) * _ui32Width;
		const uint32_t ui32SrcPlaneSize = ui32SrcRowSize * _ui32Height;
		const uint32_t ui32RowSize = SL2_ROUND_UP( sizeof( SL2_D32 ) * _ui32Width, 4 );
		const uint32_t ui32PlaneSize = ui32RowSize * _ui32Height;
		for ( uint32_t Z = 0; Z < _ui32Depth; ++Z ) {
			for ( uint32_t Y = 0; Y < _ui32Height; ++Y ) {
				for ( uint32_t X = 0; X < _ui32Width; ++X ) {
					const SL2_RGBA64F & rgbaThis = reinterpret_cast<const SL2_RGBA64F &>(_pui8Src[Z*ui32SrcPlaneSize+Y*ui32SrcRowSize+X*sizeof(SL2_RGBA64F)]);
					SL2_D32 * prDst = reinterpret_cast<SL2_D32 *>(&_pui8Dst[Z*ui32PlaneSize+Y*ui32RowSize+X*sizeof(SL2_D32)]);
					double dLum = (rgbaThis.dRgba[SL2_PC_R] +
						rgbaThis.dRgba[SL2_PC_G] +
						rgbaThis.dRgba[SL2_PC_B]) / 3.0;
					prDst->ui32Val = static_cast<uint32_t>(dLum * 4294967295.5);
				}
			}
		}
		return true;
	}

	/**
	 * Depth-32F -> RGBA64F conversion.
	 *
	 * \param _pui8Src Source texels.
	 * \param _pui8Dst Destination texels known to be in RGBA64F format.
	 * \param _ui32Width Width of the image.
	 * \param _ui32Height Height of the image.
	 * \param _ui32Depth Depth of the image.
	 * \param _pvParms Optional parameters for the conversion.
	 */
	bool CFormat::Depth32FToRgba64F( const uint8_t * _pui8Src, uint8_t * _pui8Dst, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Depth, const void * _pvParms ) {
		struct SL2_D32F {
			float fVal;
		};
		const uint32_t ui32RowSize = sizeof( SL2_RGBA64F ) * _ui32Width;
		const uint32_t ui32PlaneSize = ui32RowSize * _ui32Height;
		const uint32_t ui32SrcRowSize = SL2_ROUND_UP( sizeof( SL2_D32F ) * _ui32Width, 4 );
		const uint32_t ui32SrcPlaneSize = ui32SrcRowSize * _ui32Height;
		
		for ( uint32_t Z = 0; Z < _ui32Depth; ++Z ) {
			for ( uint32_t Y = 0; Y < _ui32Height; ++Y ) {
				for ( uint32_t X = 0; X < _ui32Width; ++X ) {
					SL2_RGBA64F & rgbaThis = reinterpret_cast<SL2_RGBA64F &>(_pui8Dst[Z*ui32PlaneSize+Y*ui32RowSize+X*sizeof(SL2_RGBA64F)]);
					const SL2_D32F * prSrc = reinterpret_cast<const SL2_D32F *>(&_pui8Src[Z*ui32SrcPlaneSize+Y*ui32SrcRowSize+X*sizeof(SL2_D32F)]);
					rgbaThis.dRgba[SL2_PC_R] = prSrc->fVal;
					rgbaThis.dRgba[SL2_PC_G] = prSrc->fVal;
					rgbaThis.dRgba[SL2_PC_B] = prSrc->fVal;
					rgbaThis.dRgba[SL2_PC_A] = 1.0;
				}
			}
		}
		return true;
	}

	/**
	 * RGBA64F -> Depth-32F conversion.
	 *
	 * \param _pui8Src Source texels known to be in RGBA64F format.
	 * \param _pui8Dst Destination texels.
	 * \param _ui32Width Width of the image.
	 * \param _ui32Height Height of the image.
	 * \param _ui32Depth Depth of the image.
	 * \param _pvParms Optional parameters for the conversion.
	 */
	bool CFormat::Depth32FFromRgba64F( const uint8_t * _pui8Src, uint8_t * _pui8Dst, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Depth, const void * _pvParms ) {
		struct SL2_D32F {
			float fVal;
		};
		const uint32_t ui32SrcRowSize = sizeof( SL2_RGBA64F ) * _ui32Width;
		const uint32_t ui32SrcPlaneSize = ui32SrcRowSize * _ui32Height;
		const uint32_t ui32RowSize = SL2_ROUND_UP( sizeof( SL2_D32F ) * _ui32Width, 4 );
		const uint32_t ui32PlaneSize = ui32RowSize * _ui32Height;
		for ( uint32_t Z = 0; Z < _ui32Depth; ++Z ) {
			for ( uint32_t Y = 0; Y < _ui32Height; ++Y ) {
				for ( uint32_t X = 0; X < _ui32Width; ++X ) {
					const SL2_RGBA64F & rgbaThis = reinterpret_cast<const SL2_RGBA64F &>(_pui8Src[Z*ui32SrcPlaneSize+Y*ui32SrcRowSize+X*sizeof(SL2_RGBA64F)]);
					SL2_D32F * prDst = reinterpret_cast<SL2_D32F *>(&_pui8Dst[Z*ui32PlaneSize+Y*ui32RowSize+X*sizeof(SL2_D32F)]);
					prDst->fVal = float( (rgbaThis.dRgba[SL2_PC_R] +
						rgbaThis.dRgba[SL2_PC_G] +
						rgbaThis.dRgba[SL2_PC_B]) / 3.0 );
				}
			}
		}
		return true;
	}

	/**
	 * Depth-16/Stencil-8 -> RGBA64F conversion.
	 *
	 * \param _pui8Src Source texels.
	 * \param _pui8Dst Destination texels known to be in RGBA64F format.
	 * \param _ui32Width Width of the image.
	 * \param _ui32Height Height of the image.
	 * \param _ui32Depth Depth of the image.
	 * \param _pvParms Optional parameters for the conversion.
	 */
	bool CFormat::Depth16S8ToRgba64F( const uint8_t * _pui8Src, uint8_t * _pui8Dst, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Depth, const void * _pvParms ) {
		struct SL2_D16S8 {
			uint16_t ui16Val;
			uint16_t ui8S;
		};
		const uint32_t ui32RowSize = sizeof( SL2_RGBA64F ) * _ui32Width;
		const uint32_t ui32PlaneSize = ui32RowSize * _ui32Height;
		const uint32_t ui32SrcRowSize = SL2_ROUND_UP( sizeof( SL2_D16S8 ) * _ui32Width, 4 );
		const uint32_t ui32SrcPlaneSize = ui32SrcRowSize * _ui32Height;
		
		for ( uint32_t Z = 0; Z < _ui32Depth; ++Z ) {
			for ( uint32_t Y = 0; Y < _ui32Height; ++Y ) {
				for ( uint32_t X = 0; X < _ui32Width; ++X ) {
					SL2_RGBA64F & rgbaThis = reinterpret_cast<SL2_RGBA64F &>(_pui8Dst[Z*ui32PlaneSize+Y*ui32RowSize+X*sizeof(SL2_RGBA64F)]);
					const SL2_D16S8 * prSrc = reinterpret_cast<const SL2_D16S8 *>(&_pui8Src[Z*ui32SrcPlaneSize+Y*ui32SrcRowSize+X*sizeof(SL2_D16S8)]);
					rgbaThis.dRgba[SL2_PC_R] = prSrc->ui16Val / 65535.0;
					rgbaThis.dRgba[SL2_PC_G] = rgbaThis.dRgba[SL2_PC_R];
					rgbaThis.dRgba[SL2_PC_B] = rgbaThis.dRgba[SL2_PC_R];
					rgbaThis.dRgba[SL2_PC_A] = prSrc->ui8S / 255.0;
				}
			}
		}
		return true;
	}

	/**
	 * RGBA64F -> Depth-16/Stencil-8 conversion.
	 *
	 * \param _pui8Src Source texels known to be in RGBA64F format.
	 * \param _pui8Dst Destination texels.
	 * \param _ui32Width Width of the image.
	 * \param _ui32Height Height of the image.
	 * \param _ui32Depth Depth of the image.
	 * \param _pvParms Optional parameters for the conversion.
	 */
	bool CFormat::Depth16S8FromRgba64F( const uint8_t * _pui8Src, uint8_t * _pui8Dst, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Depth, const void * _pvParms ) {
		struct SL2_D16S8 {
			uint16_t ui16Val;
			uint16_t ui8S;
		};
		const uint32_t ui32SrcRowSize = sizeof( SL2_RGBA64F ) * _ui32Width;
		const uint32_t ui32SrcPlaneSize = ui32SrcRowSize * _ui32Height;
		const uint32_t ui32RowSize = SL2_ROUND_UP( sizeof( SL2_D16S8 ) * _ui32Width, 4 );
		const uint32_t ui32PlaneSize = ui32RowSize * _ui32Height;
		for ( uint32_t Z = 0; Z < _ui32Depth; ++Z ) {
			for ( uint32_t Y = 0; Y < _ui32Height; ++Y ) {
				for ( uint32_t X = 0; X < _ui32Width; ++X ) {
					const SL2_RGBA64F & rgbaThis = reinterpret_cast<const SL2_RGBA64F &>(_pui8Src[Z*ui32SrcPlaneSize+Y*ui32SrcRowSize+X*sizeof(SL2_RGBA64F)]);
					SL2_D16S8 * prDst = reinterpret_cast<SL2_D16S8 *>(&_pui8Dst[Z*ui32PlaneSize+Y*ui32RowSize+X*sizeof(SL2_D16S8)]);
					double dLum = (rgbaThis.dRgba[SL2_PC_R] +
						rgbaThis.dRgba[SL2_PC_G] +
						rgbaThis.dRgba[SL2_PC_B]) / 3.0;
					prDst->ui16Val = static_cast<uint32_t>(dLum * 65535.5);
					prDst->ui8S = static_cast<uint8_t>(rgbaThis.dRgba[SL2_PC_A] * 255.5);
				}
			}
		}
		return true;
	}

	/**
	 * Depth-24/Stencil-8 -> RGBA64F conversion.
	 *
	 * \param _pui8Src Source texels.
	 * \param _pui8Dst Destination texels known to be in RGBA64F format.
	 * \param _ui32Width Width of the image.
	 * \param _ui32Height Height of the image.
	 * \param _ui32Depth Depth of the image.
	 * \param _pvParms Optional parameters for the conversion.
	 */
	bool CFormat::Depth24S8ToRgba64F( const uint8_t * _pui8Src, uint8_t * _pui8Dst, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Depth, const void * _pvParms ) {
		struct SL2_D24 {
			uint32_t ui32Val;
		};
		const uint32_t ui32RowSize = sizeof( SL2_RGBA64F ) * _ui32Width;
		const uint32_t ui32PlaneSize = ui32RowSize * _ui32Height;
		const uint32_t ui32SrcRowSize = SL2_ROUND_UP( sizeof( SL2_D24 ) * _ui32Width, 4 );
		const uint32_t ui32SrcPlaneSize = ui32SrcRowSize * _ui32Height;
		
		for ( uint32_t Z = 0; Z < _ui32Depth; ++Z ) {
			for ( uint32_t Y = 0; Y < _ui32Height; ++Y ) {
				for ( uint32_t X = 0; X < _ui32Width; ++X ) {
					SL2_RGBA64F & rgbaThis = reinterpret_cast<SL2_RGBA64F &>(_pui8Dst[Z*ui32PlaneSize+Y*ui32RowSize+X*sizeof(SL2_RGBA64F)]);
					const SL2_D24 * prSrc = reinterpret_cast<const SL2_D24 *>(&_pui8Src[Z*ui32SrcPlaneSize+Y*ui32SrcRowSize+X*sizeof(SL2_D24)]);
					rgbaThis.dRgba[SL2_PC_R] = (prSrc->ui32Val >> 8) / 16777215.0;
					rgbaThis.dRgba[SL2_PC_G] = rgbaThis.dRgba[SL2_PC_R];
					rgbaThis.dRgba[SL2_PC_B] = rgbaThis.dRgba[SL2_PC_R];
					rgbaThis.dRgba[SL2_PC_A] = static_cast<uint8_t>(prSrc->ui32Val) / 255.0;
				}
			}
		}
		return true;
	}

	/**
	 * RGBA64F -> Depth-24/Stencil-8 conversion.
	 *
	 * \param _pui8Src Source texels known to be in RGBA64F format.
	 * \param _pui8Dst Destination texels.
	 * \param _ui32Width Width of the image.
	 * \param _ui32Height Height of the image.
	 * \param _ui32Depth Depth of the image.
	 * \param _pvParms Optional parameters for the conversion.
	 */
	bool CFormat::Depth24S8FromRgba64F( const uint8_t * _pui8Src, uint8_t * _pui8Dst, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Depth, const void * _pvParms ) {
		struct SL2_D24 {
			uint32_t ui32Val;
		};
		const uint32_t ui32SrcRowSize = sizeof( SL2_RGBA64F ) * _ui32Width;
		const uint32_t ui32SrcPlaneSize = ui32SrcRowSize * _ui32Height;
		const uint32_t ui32RowSize = SL2_ROUND_UP( sizeof( SL2_D24 ) * _ui32Width, 4 );
		const uint32_t ui32PlaneSize = ui32RowSize * _ui32Height;

		for ( uint32_t Z = 0; Z < _ui32Depth; ++Z ) {
			for ( uint32_t Y = 0; Y < _ui32Height; ++Y ) {
				for ( uint32_t X = 0; X < _ui32Width; ++X ) {
					const SL2_RGBA64F & rgbaThis = reinterpret_cast<const SL2_RGBA64F &>(_pui8Src[Z*ui32SrcPlaneSize+Y*ui32SrcRowSize+X*sizeof(SL2_RGBA64F)]);
					SL2_D24 * prDst = reinterpret_cast<SL2_D24 *>(&_pui8Dst[Z*ui32PlaneSize+Y*ui32RowSize+X*sizeof(SL2_D24)]);
					double dLum = (rgbaThis.dRgba[SL2_PC_R] +
						rgbaThis.dRgba[SL2_PC_G] +
						rgbaThis.dRgba[SL2_PC_B]) / 3.0;
					prDst->ui32Val = static_cast<uint32_t>(dLum * 16777215.5) << 8;
					prDst->ui32Val |= static_cast<uint8_t>(rgbaThis.dRgba[SL2_PC_A] * 255.5);
				}
			}
		}
		return true;
	}

	/**
	 * Depth-24/X-8 -> RGBA64F conversion.
	 *
	 * \param _pui8Src Source texels.
	 * \param _pui8Dst Destination texels known to be in RGBA64F format.
	 * \param _ui32Width Width of the image.
	 * \param _ui32Height Height of the image.
	 * \param _ui32Depth Depth of the image.
	 * \param _pvParms Optional parameters for the conversion.
	 */
	bool CFormat::Depth24X8ToRgba64F( const uint8_t * _pui8Src, uint8_t * _pui8Dst, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Depth, const void * _pvParms ) {
		struct SL2_D24 {
			uint32_t ui32Val;
		};
		const uint32_t ui32RowSize = sizeof( SL2_RGBA64F ) * _ui32Width;
		const uint32_t ui32PlaneSize = ui32RowSize * _ui32Height;
		const uint32_t ui32SrcRowSize = SL2_ROUND_UP( sizeof( SL2_D24 ) * _ui32Width, 4 );
		const uint32_t ui32SrcPlaneSize = ui32SrcRowSize * _ui32Height;
		
		for ( uint32_t Z = 0; Z < _ui32Depth; ++Z ) {
			for ( uint32_t Y = 0; Y < _ui32Height; ++Y ) {
				for ( uint32_t X = 0; X < _ui32Width; ++X ) {
					SL2_RGBA64F & rgbaThis = reinterpret_cast<SL2_RGBA64F &>(_pui8Dst[Z*ui32PlaneSize+Y*ui32RowSize+X*sizeof(SL2_RGBA64F)]);
					const SL2_D24 * prSrc = reinterpret_cast<const SL2_D24 *>(&_pui8Src[Z*ui32SrcPlaneSize+Y*ui32SrcRowSize+X*sizeof(SL2_D24)]);
					rgbaThis.dRgba[SL2_PC_R] = (prSrc->ui32Val >> 8) / 16777215.0;
					rgbaThis.dRgba[SL2_PC_G] = rgbaThis.dRgba[SL2_PC_R];
					rgbaThis.dRgba[SL2_PC_B] = rgbaThis.dRgba[SL2_PC_R];
					rgbaThis.dRgba[SL2_PC_A] = 0.0;
				}
			}
		}
		return true;
	}

	/**
	 * RGBA64F -> Depth-24/X-8 conversion.
	 *
	 * \param _pui8Src Source texels known to be in RGBA64F format.
	 * \param _pui8Dst Destination texels.
	 * \param _ui32Width Width of the image.
	 * \param _ui32Height Height of the image.
	 * \param _ui32Depth Depth of the image.
	 * \param _pvParms Optional parameters for the conversion.
	 */
	bool CFormat::Depth24X8FromRgba64F( const uint8_t * _pui8Src, uint8_t * _pui8Dst, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Depth, const void * _pvParms ) {
		struct SL2_D24 {
			uint32_t ui32Val;
		};
		const uint32_t ui32SrcRowSize = sizeof( SL2_RGBA64F ) * _ui32Width;
		const uint32_t ui32SrcPlaneSize = ui32SrcRowSize * _ui32Height;
		const uint32_t ui32RowSize = SL2_ROUND_UP( sizeof( SL2_D24 ) * _ui32Width, 4 );
		const uint32_t ui32PlaneSize = ui32RowSize * _ui32Height;

		for ( uint32_t Z = 0; Z < _ui32Depth; ++Z ) {
			for ( uint32_t Y = 0; Y < _ui32Height; ++Y ) {
				for ( uint32_t X = 0; X < _ui32Width; ++X ) {
					const SL2_RGBA64F & rgbaThis = reinterpret_cast<const SL2_RGBA64F &>(_pui8Src[Z*ui32SrcPlaneSize+Y*ui32SrcRowSize+X*sizeof(SL2_RGBA64F)]);
					SL2_D24 * prDst = reinterpret_cast<SL2_D24 *>(&_pui8Dst[Z*ui32PlaneSize+Y*ui32RowSize+X*sizeof(SL2_D24)]);
					double dLum = (rgbaThis.dRgba[SL2_PC_R] +
						rgbaThis.dRgba[SL2_PC_G] +
						rgbaThis.dRgba[SL2_PC_B]) / 3.0;
					prDst->ui32Val = static_cast<uint32_t>(dLum * 16777215.5) << 8;
				}
			}
		}
		return true;
	}

	/**
	 * Depth-32F/Stencil-8 -> RGBA64F conversion.
	 *
	 * \param _pui8Src Source texels.
	 * \param _pui8Dst Destination texels known to be in RGBA64F format.
	 * \param _ui32Width Width of the image.
	 * \param _ui32Height Height of the image.
	 * \param _ui32Depth Depth of the image.
	 * \param _pvParms Optional parameters for the conversion.
	 */
	bool CFormat::Depth32FS8ToRgba64F( const uint8_t * _pui8Src, uint8_t * _pui8Dst, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Depth, const void * _pvParms ) {
		struct SL2_D32FS8 {
			float fVal;
			uint32_t ui32Stencil;
		};
		const uint64_t ui64RowSize = sizeof( SL2_RGBA64F ) * _ui32Width;
		const uint64_t ui64PlaneSize = ui64RowSize * _ui32Height;
		const uint64_t ui64SrcRowSize = SL2_ROUND_UP( sizeof( SL2_D32FS8 ) * _ui32Width, 4 );
		const uint64_t ui64SrcPlaneSize = ui64SrcRowSize * _ui32Height;
		
		for ( uint32_t Z = 0; Z < _ui32Depth; ++Z ) {
			for ( uint32_t Y = 0; Y < _ui32Height; ++Y ) {
				for ( uint32_t X = 0; X < _ui32Width; ++X ) {
					SL2_RGBA64F & rgbaThis = reinterpret_cast<SL2_RGBA64F &>(_pui8Dst[Z*ui64PlaneSize+Y*ui64RowSize+X*sizeof(SL2_RGBA64F)]);
					const SL2_D32FS8 * prSrc = reinterpret_cast<const SL2_D32FS8 *>(&_pui8Src[Z*ui64SrcPlaneSize+Y*ui64SrcRowSize+X*sizeof(SL2_D32FS8)]);
					rgbaThis.dRgba[SL2_PC_R] = prSrc->fVal;
					rgbaThis.dRgba[SL2_PC_G] = prSrc->fVal;
					rgbaThis.dRgba[SL2_PC_B] = prSrc->fVal;
					rgbaThis.dRgba[SL2_PC_A] = static_cast<uint8_t>(prSrc->ui32Stencil) / 255.0;
				}
			}
		}
		return true;
	}

	/**
	 * RGBA64F -> Depth-32F/Stencil-8 conversion.
	 *
	 * \param _pui8Src Source texels known to be in RGBA64F format.
	 * \param _pui8Dst Destination texels.
	 * \param _ui32Width Width of the image.
	 * \param _ui32Height Height of the image.
	 * \param _ui32Depth Depth of the image.
	 * \param _pvParms Optional parameters for the conversion.
	 */
	bool CFormat::Depth32FS8FromRgba64F( const uint8_t * _pui8Src, uint8_t * _pui8Dst, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Depth, const void * _pvParms ) {
		struct SL2_D32FS8 {
			float fVal;
			uint32_t ui32Stencil;
		};
		const uint32_t ui32SrcRowSize = sizeof( SL2_RGBA64F ) * _ui32Width;
		const uint32_t ui32SrcPlaneSize = ui32SrcRowSize * _ui32Height;
		const uint32_t ui32RowSize = SL2_ROUND_UP( sizeof( SL2_D32FS8 ) * _ui32Width, 4 );
		const uint32_t ui32PlaneSize = ui32RowSize * _ui32Height;
		for ( uint32_t Z = 0; Z < _ui32Depth; ++Z ) {
			for ( uint32_t Y = 0; Y < _ui32Height; ++Y ) {
				for ( uint32_t X = 0; X < _ui32Width; ++X ) {
					const SL2_RGBA64F & rgbaThis = reinterpret_cast<const SL2_RGBA64F &>(_pui8Src[Z*ui32SrcPlaneSize+Y*ui32SrcRowSize+X*sizeof(SL2_RGBA64F)]);
					SL2_D32FS8 * prDst = reinterpret_cast<SL2_D32FS8 *>(&_pui8Dst[Z*ui32PlaneSize+Y*ui32RowSize+X*sizeof(SL2_D32FS8)]);
					prDst->fVal = float( (rgbaThis.dRgba[SL2_PC_R] +
						rgbaThis.dRgba[SL2_PC_G] +
						rgbaThis.dRgba[SL2_PC_B]) / 3.0 );
					prDst->ui32Stencil = static_cast<uint8_t>(rgbaThis.dRgba[SL2_PC_A] * 255.5);
				}
			}
		}
		return true;
	}

	/**
	 * ETC1 -> RGBA64F conversion.
	 *
	 * \param _pui8Src Source texels.
	 * \param _pui8Dst Destination texels known to be in RGBA64F format.
	 * \param _ui32Width Width of the image.
	 * \param _ui32Height Height of the image.
	 * \param _ui32Depth Depth of the image.
	 * \param _pvParms Optional parameters for the conversion.
	 */
	bool CFormat::Etc1ToRgba64F( const uint8_t * _pui8Src, uint8_t * _pui8Dst, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Depth, const void * _pvParms ) {
		struct SL2_ETC1_BLOCK {
			uint64_t ui64Block0;
		};
		const SL2_ETC1_BLOCK * pbbBlocks = reinterpret_cast<const SL2_ETC1_BLOCK *>(_pui8Src);
		uint32_t ui32BlocksW = (_ui32Width + 3) / 4;
		uint32_t ui32BlocksH = (_ui32Height + 3) / 4;
		uint32_t ui32SliceSize = ui32BlocksW * ui32BlocksH * sizeof( SL2_ETC1_BLOCK );

		uint32_t ui32DstSliceSize = _ui32Width * _ui32Height;
		SL2_RGBA64F * prgbaTexels = reinterpret_cast<SL2_RGBA64F *>(_pui8Dst);
		uint8_t fPaletteRgbUi8[16*4];
		for ( uint32_t Z = 0; Z < _ui32Depth; ++Z ) {
			for ( uint32_t Y = 0; Y < ui32BlocksH; ++Y ) {
				for ( uint32_t X = 0; X < ui32BlocksW; ++X ) {
					::detexDecompressBlockETC1( reinterpret_cast<const uint8_t *>(&pbbBlocks[Z*ui32SliceSize+Y*ui32BlocksW+X]), DETEX_MODE_MASK_ALL_MODES_ETC1,
						0, reinterpret_cast<uint8_t *>(fPaletteRgbUi8) );

					for ( uint32_t I = 0; I < 16; ++I ) {
						uint32_t ui32ThisX = X * 4 + I % 4;
						uint32_t ui32ThisY = Y * 4 + I / 4;
						if ( ui32ThisX < _ui32Width && ui32ThisY < _ui32Height ) {
							SL2_RGBA64F * prgbaRow0 = &prgbaTexels[Z*ui32DstSliceSize+ui32ThisY*_ui32Width+ui32ThisX];
							(*prgbaRow0).dRgba[SL2_PC_R] = fPaletteRgbUi8[I*4+SL2_PC_R] / 255.0;
							(*prgbaRow0).dRgba[SL2_PC_G] = fPaletteRgbUi8[I*4+SL2_PC_G] / 255.0;
							(*prgbaRow0).dRgba[SL2_PC_B] = fPaletteRgbUi8[I*4+SL2_PC_B] / 255.0;
							(*prgbaRow0).dRgba[SL2_PC_A] = fPaletteRgbUi8[I*4+SL2_PC_A] / 255.0;
						}
					}
				}
			}
		}
		return true;
	}

	/**
	 * Generic RGBA64F -> ETC1 conversion.
	 *
	 * \param _pui8Src Source texels known to be in RGBA64F format.
	 * \param _pui8Dst Destination texels.
	 * \param _ui32Width Width of the image.
	 * \param _ui32Height Height of the image.
	 * \param _ui32Depth Depth of the image.
	 * \param _pvParms Optional parameters for the conversion.
	 */
	bool CFormat::Etc1FromRgba64F( const uint8_t * _pui8Src, uint8_t * _pui8Dst, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Depth, const void * _pvParms ) {
		struct SL2_ETC1_BLOCK {
			uint64_t ui64Block0;
		};
		uint32_t ui32BlocksW = (_ui32Width + 3) / 4;
		uint32_t ui32BlocksH = (_ui32Height + 3) / 4;
		uint32_t ui32SliceSize = ui32BlocksW * ui32BlocksH * sizeof( SL2_ETC1_BLOCK );
		uint32_t ui32SrcPitch = SL2_ROUND_UP( _ui32Width * sizeof( SL2_RGBA64F ), 4 );
		uint32_t ui32SrcSlice = ui32SrcPitch * _ui32Height;

		std::vector<uint8_t> vResized;
		for ( uint32_t Z = 0; Z < _ui32Depth; ++Z ) {
			const uint8_t * pui8Src = _pui8Src;
			uint32_t ui32X = _ui32Width;
			uint32_t ui32Y = _ui32Height;
			uint32_t ui32Z = 1;
			if ( (_ui32Width % 4) || (_ui32Height % 4) ) {
				if ( !ExpandTexture( _pui8Src, ui32X, ui32Y, ui32Z,
					vResized, 4, 4, 1 ) ) { return false; }
				pui8Src = vResized.data();
			}

			::rgba_surface rsSurface;
			rsSurface.ptr = const_cast<uint8_t *>(pui8Src);
			rsSurface.width = ui32X;
			rsSurface.height = ui32Y;
			rsSurface.stride = ui32X * sizeof( SL2_RGBA64F );
			::CompressBlocksETC1( &rsSurface, _pui8Dst, &m_eesEtsSettings );

			_pui8Dst += ui32SliceSize;
			_pui8Src += ui32SrcSlice;
		}
		return true;
	}

}	// namespace sl2

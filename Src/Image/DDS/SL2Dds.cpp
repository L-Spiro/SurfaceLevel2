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
#define SL2_TYPE( D3D, DXGI, OGL_IF, OGL_TYPE, OGL_BIF )		SL2_ ## D3D, SL2_ ## DXGI, #D3D, #DXGI, #OGL_IF, #OGL_TYPE, #OGL_BIF
	/** Format data. */
	CDds::SL2_FORMAT_DATA CDds::m_fdData[] = {
		{ SL2_TYPE( D3DFMT_DXT1,			DXGI_FORMAT_BC1_UNORM,							GL_COMPRESSED_RGBA_S3TC_DXT1_EXT, GL_UNSIGNED_BYTE, GL_RGBA ),							64, 0,	SL2_DPFF_FOURCC,	true, false, false },
		{ SL2_TYPE( D3DFMT_UNKNOWN,			DXGI_FORMAT_BC1_UNORM_SRGB,						GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT, GL_UNSIGNED_BYTE, GL_RGBA ),					64, 0,	SL2_DPFF_FOURCC,	true, false, false },
		{ SL2_TYPE( D3DFMT_DXT2,			DXGI_FORMAT_BC2_UNORM,							GL_COMPRESSED_RGBA_S3TC_DXT3_EXT, GL_UNSIGNED_BYTE, GL_RGBA ),							128, 0,	SL2_DPFF_FOURCC,	true, true, false },
		{ SL2_TYPE( D3DFMT_DXT3,			DXGI_FORMAT_BC2_UNORM,							GL_COMPRESSED_RGBA_S3TC_DXT3_EXT, GL_UNSIGNED_BYTE, GL_RGBA ),							128, 0, SL2_DPFF_FOURCC,	true, false, false },
		{ SL2_TYPE( D3DFMT_UNKNOWN,			DXGI_FORMAT_BC2_UNORM_SRGB,						GL_INVALID, GL_INVALID, GL_INVALID ),													128, 0, SL2_DPFF_FOURCC,	true, false, false },
		{ SL2_TYPE( D3DFMT_DXT4,			DXGI_FORMAT_BC3_UNORM,							GL_COMPRESSED_RGBA_S3TC_DXT5_EXT, GL_UNSIGNED_BYTE, GL_RGBA ),							128, 0, SL2_DPFF_FOURCC,	true, true, false },
		{ SL2_TYPE( D3DFMT_DXT5,			DXGI_FORMAT_BC3_UNORM,							GL_COMPRESSED_RGBA_S3TC_DXT5_EXT, GL_UNSIGNED_BYTE, GL_RGBA ),							128, 0, SL2_DPFF_FOURCC,	true, false, false },
		{ SL2_TYPE( D3DFMT_UNKNOWN,			DXGI_FORMAT_BC3_UNORM_SRGB,						GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT, GL_UNSIGNED_BYTE, GL_RGBA ),					128, 0, SL2_DPFF_FOURCC,	true, false, false },

		{ SL2_TYPE( D3DFMT_BC4U,			DXGI_FORMAT_BC4_UNORM,							GL_COMPRESSED_RED_RGTC1, GL_UNSIGNED_BYTE, GL_RED ),									64, 0,	SL2_DPFF_FOURCC,	true, false, false },
		{ SL2_TYPE( D3DFMT_BC4S,			DXGI_FORMAT_BC4_SNORM,							GL_COMPRESSED_SIGNED_RED_RGTC1, GL_UNSIGNED_BYTE, GL_RED ),								64, 0,	SL2_DPFF_FOURCC,	true, false, false },

		{ SL2_TYPE( D3DFMT_BC5U,			DXGI_FORMAT_BC5_UNORM,							GL_COMPRESSED_RG_RGTC2, GL_UNSIGNED_BYTE, GL_RG ),										128, 0, SL2_DPFF_FOURCC,	true, false, false },
		{ SL2_TYPE( D3DFMT_BC5S,			DXGI_FORMAT_BC5_SNORM,							GL_COMPRESSED_SIGNED_RG_RGTC2, GL_UNSIGNED_BYTE, GL_RG ),								128, 0, SL2_DPFF_FOURCC,	true, false, false },

		{ SL2_TYPE( D3DFMT_UNKNOWN,			DXGI_FORMAT_BC6H_UF16,							GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT, GL_FLOAT, GL_RGB ),								128, 0, SL2_DPFF_FOURCC,	true, false, false },
		{ SL2_TYPE( D3DFMT_UNKNOWN,			DXGI_FORMAT_BC6H_SF16,							GL_INVALID, GL_INVALID, GL_INVALID /** TODO. */ ),										128, 0, SL2_DPFF_FOURCC,	true, false, false },

		{ SL2_TYPE( D3DFMT_UNKNOWN,			DXGI_FORMAT_BC7_UNORM,							GL_COMPRESSED_RGBA_BPTC_UNORM, GL_UNSIGNED_BYTE, GL_RGBA ),								128, 0, SL2_DPFF_FOURCC,	true, false, false },
		{ SL2_TYPE( D3DFMT_UNKNOWN,			DXGI_FORMAT_BC7_UNORM_SRGB,						GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM, GL_UNSIGNED_BYTE, GL_RGBA ),						128, 0, SL2_DPFF_FOURCC,	true, false, false },


		{ SL2_TYPE( D3DFMT_R8G8_B8G8,		DXGI_FORMAT_R8G8_B8G8_UNORM,					GL_INVALID, GL_INVALID, GL_INVALID /** TODO. */ ),										32, 0,	SL2_DPFF_FOURCC,	false, false, true },
		{ SL2_TYPE( D3DFMT_G8R8_G8B8,		DXGI_FORMAT_G8R8_G8B8_UNORM,					GL_INVALID, GL_INVALID, GL_INVALID /** TODO. */ ),										32, 0,	SL2_DPFF_FOURCC,	false, false, true },

		{ SL2_TYPE( D3DFMT_A16B16G16R16,	DXGI_FORMAT_R16G16B16A16_UNORM,					GL_RGBA16, GL_UNSIGNED_SHORT, GL_RGBA ),												64, 0,	SL2_DPFF_FOURCC,	false, false, false },
		{ SL2_TYPE( D3DFMT_Q16W16V16U16,	DXGI_FORMAT_R16G16B16A16_SNORM,					GL_RGBA16_SNORM, GL_SHORT, GL_RGBA ),													64, 0,	SL2_DPFF_FOURCC,	false, false, false },

		{ SL2_TYPE( D3DFMT_R16F,			DXGI_FORMAT_R16_FLOAT,							GL_R16F, GL_HALF_FLOAT, GL_RED ),														16, 0,	SL2_DPFF_FOURCC,	false, false, false },
		{ SL2_TYPE( D3DFMT_G16R16F,			DXGI_FORMAT_R16G16_FLOAT,						GL_RG16F, GL_HALF_FLOAT, GL_RG ),														32, 0,	SL2_DPFF_FOURCC,	false, false, false },
		{ SL2_TYPE( D3DFMT_A16B16G16R16F,	DXGI_FORMAT_R16G16B16A16_FLOAT,					GL_RGBA16F, GL_HALF_FLOAT, GL_RGBA ),													64, 0,	SL2_DPFF_FOURCC,	false, false, false },
		{ SL2_TYPE( D3DFMT_R32F,			DXGI_FORMAT_R32_FLOAT,							GL_R32F, GL_FLOAT, GL_RED ),															32, 0,	SL2_DPFF_FOURCC,	false, false, false },
		{ SL2_TYPE( D3DFMT_G32R32F,			DXGI_FORMAT_R32G32_FLOAT,						GL_RG32F, GL_FLOAT, GL_RG ),															64, 0,	SL2_DPFF_FOURCC,	false, false, false },
		{ SL2_TYPE( D3DFMT_A32B32G32R32F,	DXGI_FORMAT_R32G32B32A32_FLOAT,					GL_RGBA32F, GL_FLOAT, GL_RGBA ),														128, 0, SL2_DPFF_FOURCC,	false, false, false },

		{ SL2_TYPE( D3DFMT_UNKNOWN,			DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,				GL_SRGB8_ALPHA8, GL_UNSIGNED_BYTE, GL_RGBA ),											32, 0,	SL2_DPFF_RGBA,		false, false, false },
		{ SL2_TYPE( D3DFMT_G16R16,			DXGI_FORMAT_R16G16_UNORM,						GL_RG16, GL_UNSIGNED_SHORT, GL_RG ),													32, 0,	SL2_DPFF_FOURCC,	false, false, false },
		{ SL2_TYPE( D3DFMT_R8G8B8,			DXGI_FORMAT_UNKNOWN,							GL_RGB8, GL_UNSIGNED_BYTE, GL_RGB ),													24, 0,	SL2_DPFF_RGB,		false, false, false },
		{ SL2_TYPE( D3DFMT_A1R5G5B5,		DXGI_FORMAT_B5G5R5A1_UNORM,						GL_RGB5_A1, GL_UNSIGNED_SHORT_1_5_5_5_REV, GL_BGRA ),									16, 0,	SL2_DPFF_RGBA,		false, false, false },
		{ SL2_TYPE( D3DFMT_R5G6B5,			DXGI_FORMAT_B5G6R5_UNORM,						GL_RGB565, GL_UNSIGNED_SHORT_5_6_5, GL_RGB ),											16, 0,	SL2_DPFF_RGB,		false, false, false },
		{ SL2_TYPE( D3DFMT_A8,				DXGI_FORMAT_A8_UNORM,							GL_ALPHA8, GL_UNSIGNED_BYTE, GL_ALPHA ),												8, 0,	SL2_DPFF_ALPHA,		false, false, false },
		
		{ SL2_TYPE( D3DFMT_X1R5G5B5,		DXGI_FORMAT_UNKNOWN,							GL_RGB5, GL_UNSIGNED_SHORT_1_5_5_5_REV, GL_RGB ),										16, 0,	SL2_DPFF_RGB,		false, false, false },
		{ SL2_TYPE( D3DFMT_A4R4G4B4,		DXGI_FORMAT_B4G4R4A4_UNORM,						GL_RGBA4, GL_UNSIGNED_SHORT_4_4_4_4_REV, GL_BGRA ),										16, 0,	SL2_DPFF_RGBA,		false, false, false },
		{ SL2_TYPE( D3DFMT_X4R4G4B4,		DXGI_FORMAT_UNKNOWN,							GL_RGB4, GL_UNSIGNED_SHORT_4_4_4_4_REV, GL_RGB ),										16, 0,	SL2_DPFF_RGB,		false, false, false },
		{ SL2_TYPE( D3DFMT_A8R3G3B2,		DXGI_FORMAT_UNKNOWN,							GL_RGBA8, GL_UNSIGNED_BYTE_3_3_2, GL_BGRA ),											16, 0,	SL2_DPFF_RGBA,		false, false, false },
		
		{ SL2_TYPE( D3DFMT_A8L8,			DXGI_FORMAT_UNKNOWN,							GL_LUMINANCE8_ALPHA8, GL_UNSIGNED_BYTE, GL_LUMINANCE_ALPHA ),							16, 0,	SL2_DPFF_LUMINANCE,	false, false, false },
		{ SL2_TYPE( D3DFMT_L16,				DXGI_FORMAT_UNKNOWN,							GL_LUMINANCE16, GL_UNSIGNED_SHORT, GL_LUMINANCE ),										16, 0,	SL2_DPFF_LUMINANCE,	false, false, false },
		{ SL2_TYPE( D3DFMT_A4L4,			DXGI_FORMAT_UNKNOWN,							GL_LUMINANCE4_ALPHA4, GL_UNSIGNED_BYTE, GL_LUMINANCE_ALPHA ),							8, 0,	SL2_DPFF_LUMINANCE,	false, false, false },
		{ SL2_TYPE( D3DFMT_L8,				DXGI_FORMAT_UNKNOWN,							GL_LUMINANCE8, GL_UNSIGNED_BYTE, GL_LUMINANCE ),										8, 0,	SL2_DPFF_LUMINANCE,	false, false, false },
		
		{ SL2_TYPE( D3DFMT_UNKNOWN,			DXGI_FORMAT_R32G32B32A32_UINT,					GL_RGBA32UI, GL_UNSIGNED_INT, GL_RGBA_INTEGER ),										128, 0, SL2_DPFF_RGBA,		false, false, false },
		{ SL2_TYPE( D3DFMT_UNKNOWN,			DXGI_FORMAT_R32G32B32A32_SINT,					GL_RGBA32I, GL_INT, GL_RGBA_INTEGER ),													128, 0, SL2_DPFF_RGBA,		false, false, false },
		{ SL2_TYPE( D3DFMT_UNKNOWN,			DXGI_FORMAT_R32G32B32_FLOAT,					GL_RGB32F, GL_FLOAT, GL_RGB ),															96, 0,	SL2_DPFF_RGB,		false, false, false },
		{ SL2_TYPE( D3DFMT_UNKNOWN,			DXGI_FORMAT_R32G32B32_UINT,						GL_RGB32UI, GL_UNSIGNED_INT, GL_RGB_INTEGER ),											96, 0,	SL2_DPFF_RGB,		false, false, false },
		{ SL2_TYPE( D3DFMT_UNKNOWN,			DXGI_FORMAT_R32G32B32_SINT,						GL_RGB32I, GL_INT, GL_RGB_INTEGER ),													96, 0,	SL2_DPFF_RGB,		false, false, false },
		{ SL2_TYPE( D3DFMT_UNKNOWN,			DXGI_FORMAT_R16G16B16A16_UINT,					GL_RGBA16UI, GL_UNSIGNED_SHORT, GL_RGBA_INTEGER ),										64, 0,	SL2_DPFF_RGBA,		false, false, false },
		{ SL2_TYPE( D3DFMT_UNKNOWN,			DXGI_FORMAT_R16G16B16A16_SINT,					GL_RGBA16I, GL_SHORT, GL_RGBA_INTEGER ),												64, 0,	SL2_DPFF_RGBA,		false, false, false },
		{ SL2_TYPE( D3DFMT_UNKNOWN,			DXGI_FORMAT_R32G32_UINT,						GL_RG32UI, GL_UNSIGNED_INT, GL_RG_INTEGER ),											64, 0,	SL2_DPFF_NULL,		false, false, false },
		{ SL2_TYPE( D3DFMT_UNKNOWN,			DXGI_FORMAT_R32G32_SINT,						GL_RG32I, GL_INT, GL_RG_INTEGER ),														64, 0,	SL2_DPFF_NULL,		false, false, false },
		
		{ SL2_TYPE( D3DFMT_UNKNOWN,			DXGI_FORMAT_R10G10B10A2_UINT,					GL_RGB10_A2UI, GL_UNSIGNED_INT_2_10_10_10_REV, GL_RGBA_INTEGER ),						32, 0,	SL2_DPFF_RGBA,		false, false, false },
		{ SL2_TYPE( D3DFMT_UNKNOWN,			DXGI_FORMAT_R11G11B10_FLOAT,					GL_R11F_G11F_B10F, GL_UNSIGNED_INT_10F_11F_11F_REV, GL_RGB ),							32, 0,	SL2_DPFF_RGB,		false, false, false },
		{ SL2_TYPE( D3DFMT_UNKNOWN,			DXGI_FORMAT_R8G8B8A8_UINT,						GL_RGBA8UI, GL_UNSIGNED_BYTE, GL_RGBA_INTEGER ),										32, 0,	SL2_DPFF_RGBA,		false, false, false },
		{ SL2_TYPE( D3DFMT_UNKNOWN,			DXGI_FORMAT_R8G8B8A8_SNORM,						GL_RGBA8_SNORM, GL_BYTE, GL_RGBA ),														32, 0,	SL2_DPFF_RGBA,		false, false, false },
		{ SL2_TYPE( D3DFMT_UNKNOWN,			DXGI_FORMAT_R8G8B8A8_SINT,						GL_RGBA8I, GL_BYTE, GL_RGBA_INTEGER ),													32, 0,	SL2_DPFF_RGBA,		false, false, false },
		{ SL2_TYPE( D3DFMT_UNKNOWN,			DXGI_FORMAT_R16G16_UINT,						GL_RG16UI, GL_UNSIGNED_SHORT, GL_RG_INTEGER ),											32, 0,	SL2_DPFF_NULL,		false, false, false },
		{ SL2_TYPE( D3DFMT_UNKNOWN,			DXGI_FORMAT_R16G16_SNORM,						GL_RG16_SNORM, GL_SHORT, GL_RG ),														32, 0,	SL2_DPFF_NULL,		false, false, false },
		{ SL2_TYPE( D3DFMT_UNKNOWN,			DXGI_FORMAT_R16G16_SINT,						GL_RG16I, GL_SHORT, GL_RG_INTEGER ),													32, 0,	SL2_DPFF_NULL,		false, false, false },
		{ SL2_TYPE( D3DFMT_UNKNOWN,			DXGI_FORMAT_D32_FLOAT,							GL_DEPTH_COMPONENT32F, GL_FLOAT, GL_DEPTH_COMPONENT ),									32, 0,	SL2_DPFF_NULL,		false, false, false },
		{ SL2_TYPE( D3DFMT_UNKNOWN,			DXGI_FORMAT_R32_UINT,							GL_R32UI, GL_UNSIGNED_INT, GL_RED_INTEGER ),											32, 0,	SL2_DPFF_NULL,		false, false, false },
		{ SL2_TYPE( D3DFMT_UNKNOWN,			DXGI_FORMAT_R32_SINT,							GL_R32I, GL_INT, GL_RED_INTEGER ),														32, 0,	SL2_DPFF_NULL,		false, false, false },
		{ SL2_TYPE( D3DFMT_UNKNOWN,			DXGI_FORMAT_R8G8_UNORM,							GL_RG8, GL_UNSIGNED_BYTE, GL_RG ),														16, 0,	SL2_DPFF_NULL,		false, false, false },
		{ SL2_TYPE( D3DFMT_UNKNOWN,			DXGI_FORMAT_R8G8_UINT,							GL_RG8UI, GL_UNSIGNED_BYTE, GL_RG_INTEGER ),											16, 0,	SL2_DPFF_NULL,		false, false, false },
		{ SL2_TYPE( D3DFMT_UNKNOWN,			DXGI_FORMAT_R8G8_SNORM,							GL_RG8_SNORM, GL_BYTE, GL_RG ),															16, 0,	SL2_DPFF_NULL,		false, false, false },
		{ SL2_TYPE( D3DFMT_UNKNOWN,			DXGI_FORMAT_R8G8_SINT,							GL_RG8I, GL_BYTE, GL_RG_INTEGER ),														16, 0,	SL2_DPFF_NULL,		false, false, false },
		{ SL2_TYPE( D3DFMT_UNKNOWN,			DXGI_FORMAT_D16_UNORM,							GL_DEPTH_COMPONENT16, GL_UNSIGNED_SHORT, GL_DEPTH_COMPONENT ),							16, 0,	SL2_DPFF_NULL,		false, false, false },
		{ SL2_TYPE( D3DFMT_UNKNOWN,			DXGI_FORMAT_R16_UNORM,							GL_R16, GL_UNSIGNED_SHORT, GL_RED ),													16, 0,	SL2_DPFF_NULL,		false, false, false },
		{ SL2_TYPE( D3DFMT_UNKNOWN,			DXGI_FORMAT_R16_UINT,							GL_R16UI, GL_UNSIGNED_SHORT, GL_RED_INTEGER ),											16, 0,	SL2_DPFF_NULL,		false, false, false },
		{ SL2_TYPE( D3DFMT_UNKNOWN,			DXGI_FORMAT_R16_SNORM,							GL_R16_SNORM, GL_SHORT, GL_RED ),														16, 0,	SL2_DPFF_NULL,		false, false, false },
		{ SL2_TYPE( D3DFMT_UNKNOWN,			DXGI_FORMAT_R16_SINT,							GL_R16I, GL_SHORT, GL_RED_INTEGER ),													16, 0,	SL2_DPFF_NULL,		false, false, false },
		{ SL2_TYPE( D3DFMT_UNKNOWN,			DXGI_FORMAT_R8_UNORM,							GL_R8, GL_UNSIGNED_BYTE, GL_RED ),														8, 0,	SL2_DPFF_NULL,		false, false, false },
		{ SL2_TYPE( D3DFMT_UNKNOWN,			DXGI_FORMAT_R8_UINT,							GL_R8UI, GL_UNSIGNED_BYTE, GL_RED_INTEGER ),											8, 0,	SL2_DPFF_NULL,		false, false, false },
		{ SL2_TYPE( D3DFMT_UNKNOWN,			DXGI_FORMAT_R8_SNORM,							GL_R8_SNORM, GL_BYTE, GL_RED ),															8, 0,	SL2_DPFF_NULL,		false, false, false },
		{ SL2_TYPE( D3DFMT_UNKNOWN,			DXGI_FORMAT_R8_SINT,							GL_R8I, GL_BYTE, GL_RED_INTEGER ),														8, 0,	SL2_DPFF_NULL,		false, false, false },
		{ SL2_TYPE( D3DFMT_UNKNOWN,			DXGI_FORMAT_R1_UNORM,							GL_INVALID, GL_INVALID, GL_INVALID /** TODO. */ ),										1, 0,	SL2_DPFF_NULL,		false, false, false },
		{ SL2_TYPE( D3DFMT_UNKNOWN,			DXGI_FORMAT_R9G9B9E5_SHAREDEXP,					GL_RGB9_E5, GL_UNSIGNED_INT_5_9_9_9_REV, GL_RGB ),										32, 0,	SL2_DPFF_RGB,		false, false, false },
		{ SL2_TYPE( D3DFMT_UNKNOWN,			DXGI_FORMAT_B5G5R5A1_UNORM,						GL_RGB5_A1, GL_UNSIGNED_SHORT_1_5_5_5_REV, GL_BGRA ),									16, 0,	SL2_DPFF_RGBA,		false, false, false },
		{ SL2_TYPE( D3DFMT_UNKNOWN,			DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM,			GL_RGB10_A2, GL_UNSIGNED_INT_2_10_10_10_REV, GL_BGRA ),									32, 0,	SL2_DPFF_RGBA,		false, false, false },
		{ SL2_TYPE( D3DFMT_UNKNOWN,			DXGI_FORMAT_B8G8R8A8_UNORM_SRGB,				GL_SRGB8_ALPHA8, GL_UNSIGNED_BYTE, GL_BGRA ),											32, 0,	SL2_DPFF_RGBA,		false, false, false },
		{ SL2_TYPE( D3DFMT_UNKNOWN,			DXGI_FORMAT_B8G8R8X8_UNORM_SRGB,				GL_INVALID, GL_INVALID, GL_INVALID ),													32, 0,	SL2_DPFF_RGB,		false, false, false },

		{ SL2_TYPE( D3DFMT_UNKNOWN,			DXGI_FORMAT_X32_TYPELESS_G8X24_UINT,			GL_INVALID, GL_INVALID, GL_INVALID ),													64, 0,	SL2_DPFF_NULL,		false, false, false },
		{ SL2_TYPE( D3DFMT_UNKNOWN,			DXGI_FORMAT_D32_FLOAT_S8X24_UINT,				GL_DEPTH32F_STENCIL8, GL_FLOAT_32_UNSIGNED_INT_24_8_REV, GL_DEPTH_STENCIL ),			64, 0,	SL2_DPFF_NULL,		false, false, false },
		{ SL2_TYPE( D3DFMT_D24S8,			DXGI_FORMAT_D24_UNORM_S8_UINT,					GL_DEPTH24_STENCIL8, GL_UNSIGNED_INT_24_8, GL_DEPTH_STENCIL ),							32, 0,	SL2_DPFF_NULL,		false, false, false },
		{ SL2_TYPE( D3DFMT_UNKNOWN,			DXGI_FORMAT_X24_TYPELESS_G8_UINT,				GL_INVALID, GL_INVALID, GL_INVALID ),													32, 0,	SL2_DPFF_NULL,		false, false, false },

		// Order matters here.
		{ SL2_TYPE( D3DFMT_UNKNOWN,			DXGI_FORMAT_R8G8B8A8_UNORM,						GL_RGBA8, GL_UNSIGNED_BYTE, GL_RGBA ),													32, 0,	SL2_DPFF_RGBA,		false, false, false },
		{ SL2_TYPE( D3DFMT_UNKNOWN,			DXGI_FORMAT_B8G8R8A8_UNORM,						GL_RGBA8, GL_UNSIGNED_BYTE, GL_BGRA ),													32, 0,	SL2_DPFF_RGBA,		false, false, false },
		{ SL2_TYPE( D3DFMT_UNKNOWN,			DXGI_FORMAT_B8G8R8X8_UNORM,						GL_INVALID, GL_INVALID, GL_INVALID ),													32, 0,	SL2_DPFF_RGB,		false, false, false },
		
		{ SL2_TYPE( D3DFMT_A8R8G8B8,		DXGI_FORMAT_R8G8B8A8_UNORM,						GL_RGBA8, GL_UNSIGNED_BYTE, GL_RGBA ),													32, 0,	SL2_DPFF_RGBA,		false, false, false, Convert_RGBA32_to_RGBA32<SL2_TEXEL_RGBA8, true> },
		{ SL2_TYPE( D3DFMT_X8R8G8B8,		DXGI_FORMAT_R8G8B8A8_UNORM,						GL_RGBA8, GL_UNSIGNED_BYTE, GL_RGBA ),													32, 0,	SL2_DPFF_RGB,		false, false, false, Convert_RGBA32_to_RGBA32<SL2_TEXEL_RGBA8, false> },
		{ SL2_TYPE( D3DFMT_A8B8G8R8,		DXGI_FORMAT_B8G8R8A8_UNORM,						GL_RGBA8, GL_UNSIGNED_BYTE, GL_BGRA ),													32, 0,	SL2_DPFF_RGBA,		false, false, false, Convert_RGBA32_to_RGBA32<SL2_TEXEL_BGRA8, true> },
		{ SL2_TYPE( D3DFMT_X8B8G8R8,		DXGI_FORMAT_B8G8R8X8_UNORM,						GL_INVALID, GL_INVALID, GL_INVALID ),													32, 0,	SL2_DPFF_RGB,		false, false, false, Convert_RGBA32_to_RGBA32<SL2_TEXEL_BGRA8, false> },

		// Order matters here.
		{ SL2_TYPE( D3DFMT_UNKNOWN,			DXGI_FORMAT_R10G10B10A2_UNORM,					GL_RGB10_A2, GL_UNSIGNED_INT_2_10_10_10_REV, GL_RGBA ),									32, 0,	SL2_DPFF_RGBA,		false, false, false },

		{ SL2_TYPE( D3DFMT_A2B10G10R10,		DXGI_FORMAT_R10G10B10A2_UNORM,					GL_RGB10_A2, GL_UNSIGNED_INT_2_10_10_10_REV, GL_RGBA ),									32, 0,	SL2_DPFF_RGBA,		false, false, false, Convert_RGBA32_to_RGBA32<SL2_TEXEL_RGB10A2, true> },
		{ SL2_TYPE( D3DFMT_A2R10G10B10,		DXGI_FORMAT_R10G10B10A2_UNORM,					GL_RGB10_A2, GL_UNSIGNED_INT_2_10_10_10_REV, GL_RGBA ),									32, 0,	SL2_DPFF_RGBA,		false, false, false, Convert_RGBA32_to_RGBA32<SL2_TEXEL_RGB10A2, true> },

		// Order matters here.
		{ SL2_TYPE( D3DFMT_UNKNOWN,			DXGI_FORMAT_YUY2,								GL_RGB, GL_UNSIGNED_SHORT_8_8_APPLE, GL_YCBCR_422_APPLE ),								32, 0,	SL2_DPFF_YUV,		false, false, true },
		{ SL2_TYPE( D3DFMT_YUY2,			DXGI_FORMAT_YUY2,								GL_RGB, GL_UNSIGNED_SHORT_8_8_APPLE, GL_YCBCR_422_APPLE ),								32, 0,	SL2_DPFF_FOURCC,	false, false, true },
		{ SL2_TYPE( D3DFMT_UNKNOWN,			DXGI_FORMAT_V208,								GL_INVALID, GL_INVALID, GL_INVALID ),													24, 0,	SL2_DPFF_YUV,		false, false, true },

		{ SL2_TYPE( D3DFMT_UYVY,			DXGI_FORMAT_YUY2,								GL_RGB, GL_UNSIGNED_SHORT_8_8_APPLE, GL_YCBCR_422_APPLE ),								32, 0,	SL2_DPFF_FOURCC,	false, false, true, Convert_UYVY_to_YUY2 },
		{ SL2_TYPE( D3DFMT_CxV8U8,			DXGI_FORMAT_V208,								GL_INVALID, GL_INVALID, GL_INVALID ),													16, 24, SL2_DPFF_FOURCC,	false, false, true },		// Add conversion.

		
	};
#undef SL2_TYPE


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
		
		if ( m_dhHeader.ui32Size != sizeof( m_dhHeader ) ) { return false; }
		if ( m_dhHeader.dpPixelFormat.ui32Size != sizeof( m_dhHeader.dpPixelFormat ) ) { return false; }

		if ( (m_dhHeader.ui32Flags & (SL2_DF_CAPS | SL2_DF_HEIGHT | SL2_DF_WIDTH | SL2_DF_PIXELFORMAT)) !=
			(SL2_DF_CAPS | SL2_DF_HEIGHT | SL2_DF_WIDTH | SL2_DF_PIXELFORMAT) ) {
			if ( (m_dhHeader.ui32Flags & (SL2_DF_HEIGHT | SL2_DF_WIDTH)) !=
				(SL2_DF_HEIGHT | SL2_DF_WIDTH) ) {
				return false;
			}
		}


		
		m_ui32ArraySize = 1;

		bool bUseDx10 = false;
		if ( m_dhHeader.dpPixelFormat.ui32Flags & SL2_DPFF_FOURCC ) {
			switch ( m_dhHeader.dpPixelFormat.ui32FourCC ) {
				case SL2_MAKEFOURCC( 'D', 'X', '1', '0' ) : {
					bUseDx10 = true;
					if ( !sStream.Read( m_dh10Header10 ) ) { return false; }
					m_pfdFormat = FormatByDXGI_FORMAT( static_cast<SL2_DXGI_FORMAT>(m_dh10Header10.ui32DxgiFormat) );
					m_ui32ArraySize = m_dh10Header10.ui32ArraySize;
					break;
				}
				default : {
					m_pfdFormat = FormatByD3DFORMAT( static_cast<SL2_D3DFORMAT>(m_dhHeader.dpPixelFormat.ui32FourCC) );
				}
			}
			if ( nullptr == m_pfdFormat ) { return false; }
		}
		else {
			// Have to figure out the format based off m_dhHeader.dpPixelFormat.ui32Flags, masks, and bit counts.
		}

		uint32_t ui32Array = m_ui32ArraySize;
		if ( m_dhHeader.ui32Caps2 & SL2_DDSCAPS2_CUBEMAP ) {
			ui32Array = m_ui32Faces = 6;
		}

		if ( m_pfdFormat ) {
			// We have known metadata regarding the format.  This might require conversions.
			//	Convert (optionally) and load the data.
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
					catch ( ... ) { return false; }
					if ( m_pfdFormat->pfConverter ) {
						m_pfdFormat->pfConverter( pui8Src, tTexture.vTexture.data(), ui32W, ui32H, ui32D, ui32Pitch, m_dhHeader.dpPixelFormat );
						if ( sStream.Read( nullptr, sSrcSize ) != sSrcSize ) { return false; };
					}
					else {
						if ( sStream.Read( tTexture.vTexture.data(), tTexture.vTexture.size() ) != tTexture.vTexture.size() ) { return false; }
					}
					tTexture.ui32W = ui32W;
					tTexture.ui32H = ui32H;
					tTexture.ui32D = ui32D;
					m_vTextures.push_back( std::move( tTexture ) );

					ui32W = std::max( ui32W >> 1, static_cast<uint32_t>(1) );
					ui32H = std::max( ui32H >> 1, static_cast<uint32_t>(1) );
					ui32D = std::max( ui32D >> 1, static_cast<uint32_t>(1) );
				}
			}
			m_dhHeader.ui32PitchOrLinearSize = ui32Pitch;
		}
		else {
			// Going based on the pixel format data using bit sizes and assuming tight packing.
			size_t iIndex = 0;
			//uint32_t ui32Pitch = m_dhHeader.ui32PitchOrLinearSize;
			uint8_t * pui8Src = sStream.Data();
			for ( uint32_t J = 0; J < ui32Array; J++ ) {
				uint32_t ui32W = std::max( m_dhHeader.ui32Width, static_cast<uint32_t>(1) );
				uint32_t ui32H = std::max( m_dhHeader.ui32Height, static_cast<uint32_t>(1) );
				uint32_t ui32D = std::max( m_dhHeader.ui32Depth, static_cast<uint32_t>(1) );
				
				for ( uint32_t I = 0; I < m_dhHeader.ui32MipMapCount; I++ ) {
					size_t sPitch = static_cast<size_t>(static_cast<uint64_t>(m_dhHeader.dpPixelFormat.ui32RGBBitCount) * ui32W);
					size_t sSrcSize = static_cast<size_t>(static_cast<uint64_t>(ui32D) * ui32H * sPitch / 8ULL);
					SL2_TEX tTexture;
					tTexture.ui32Pitch = static_cast<uint32_t>(sPitch);
					try {
						tTexture.vTexture.resize( sSrcSize );
					}
					catch ( ... ) { return false; }

					if ( sStream.Read( tTexture.vTexture.data(), tTexture.vTexture.size() ) != tTexture.vTexture.size() ) { return false; }
					tTexture.ui32W = ui32W;
					tTexture.ui32H = ui32H;
					tTexture.ui32D = ui32D;
					m_vTextures.push_back( std::move( tTexture ) );

					ui32W = std::max( ui32W >> 1, static_cast<uint32_t>(1) );
					ui32H = std::max( ui32H >> 1, static_cast<uint32_t>(1) );
					ui32D = std::max( ui32D >> 1, static_cast<uint32_t>(1) );
				}
			}
		}
		return true;
	}

	/**
	 * Checks if the given DXGI format is supported in DDS.
	 * 
	 * \param _pcDxgiFormat The name of the DXGI format to verify.
	 * \return Returns a pointer to the given format by DXGI if available.
	 **/
	const CDds::SL2_FORMAT_DATA * CDds::DxgiIsSupported( const char * _pcDxgiFormat ) {
		for ( size_t I = 0; I < SL2_ELEMENTS( m_fdData ); ++I ) {
			if ( ::_stricmp( m_fdData[I].pcDxgiName, _pcDxgiFormat ) == 0 ) { return &m_fdData[I]; }
		}
		return nullptr;
	}

	/**
	 * Finds a fall-back using OpenGL formats.
	 * 
	 * \param _pcFormat The internal format to find by name.
	 * \param _pcType The type to find by name.
	 * \param _pcBaseFormat The base internal format to find by name.
	 * \return Returns a pointer to the given format by OpenGL if available.
	 **/
	const CDds::SL2_FORMAT_DATA * CDds::FindByOgl( const char * _pcFormat, const char * _pcType, const char * _pcBaseFormat ) {
		for ( size_t I = 0; I < SL2_ELEMENTS( m_fdData ); ++I ) {
			if ( ::_stricmp( m_fdData[I].pcOglInternalFormat, _pcFormat ) == 0 &&
				::_stricmp( m_fdData[I].pcOglType, _pcType ) == 0 &&
				::_stricmp( m_fdData[I].pcOglBaseInternalFormat, _pcBaseFormat ) == 0 ) { return &m_fdData[I]; }
		}
		return nullptr;
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
	void CDds::Convert_UYVY_to_YUY2( uint8_t * _pui8Src, uint8_t * _pui8Dst, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Depth, uint32_t _ui32Pitch, const SL2_DDS_PIXELFORMAT &/*_dpfPixelFormat*/ ) {
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
	void CDds::Convert_RGB24_to_RGBA32( uint8_t * _pui8Src, uint8_t * _pui8Dst, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Depth, uint32_t _ui32Pitch, const SL2_DDS_PIXELFORMAT &_dpfPixelFormat ) {
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

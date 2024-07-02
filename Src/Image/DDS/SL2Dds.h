/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A DDS reader and writer.
 */

#pragma once

#include "../SL2Surface.h"
#include "../../Utilities/SL2Utilities.h"

#include <cstdint>
#include <memory>
#include <vector>

#define SL2_MAKEFOURCC( A, B, C, D )													(static_cast<uint32_t>(static_cast<uint8_t>(A)) |			\
																						(static_cast<uint32_t>(static_cast<uint8_t>(B)) << 8) |		\
																						(static_cast<uint32_t>(static_cast<uint8_t>(C)) << 16) |	\
																						(static_cast<uint32_t>(static_cast<uint8_t>(D)) << 24))

#define SL2_DDS_FOURCC										0x00000004					// DDPF_FOURCC
#define SL2_DDS_RGB											0x00000040					// DDPF_RGB
#define SL2_DDS_RGBA										0x00000041					// DDPF_RGB | DDPF_ALPHAPIXELS
#define SL2_DDS_LUMINANCE									0x00020000					// DDPF_LUMINANCE
#define SL2_DDS_LUMINANCEA									0x00020001					// DDPF_LUMINANCE | DDPF_ALPHAPIXELS
#define SL2_DDS_ALPHAPIXELS									0x00000001					// DDPF_ALPHAPIXELS
#define SL2_DDS_ALPHA										0x00000002					// DDPF_ALPHA
#define SL2_DDS_PAL8										0x00000020					// DDPF_PALETTEINDEXED8
#define SL2_DDS_PAL8A										0x00000021					// DDPF_PALETTEINDEXED8 | DDPF_ALPHAPIXELS
#define SL2_DDS_BUMPDUDV									0x00080000					// DDPF_BUMPDUDV
#define SL2_DDS_BUMPLUMINANCE								0x00040000


namespace sl2 {

	// == Enumerations.
	/**
	 * Pixel format flags.
	 */
	enum SL2_DDS_PIXEL_FORMAT_FLAGS {
		SL2_DPFF_NULL										= 0x0,
		SL2_DPFF_ALPHAPIXELS								= 0x1,						/**< Texture contains alpha data; ui32RGBAlphaBitMask contains valid data. */
		SL2_DPFF_ALPHA										= 0x2,						/**< Used in some older DDS files for alpha channel only uncompressed data (ui32RGBBitCount contains the alpha channel bitcount; ui32ABitMask contains valid data). */
		SL2_DPFF_FOURCC										= 0x4,						/**< Texture contains compressed RGB data; ui32FourCC contains valid data. */
		SL2_DPFF_RGB										= 0x40,						/**< Texture contains uncompressed RGB data; dwRGBBitCount and the RGB masks (ui32RBitMask, ui32RBitMask, ui32RBitMask) contain valid data. */
		SL2_DPFF_RGBA										= SL2_DPFF_RGB |
																SL2_DPFF_ALPHAPIXELS,	/**< Texture contains uncompressed RGB datawith alpha. */
		SL2_DPFF_YUV										= 0x200,					/**< Used in some older DDS files for YUV uncompressed data (ui32RGBBitCount contains the YUV bit count; ui32RBitMask contains the Y mask, ui32GBitMask contains the U mask, ui32BBitMask contains the V mask). */
		SL2_DPFF_LUMINANCE									= 0x20000,					/**< Used in some older DDS files for single channel color uncompressed data (ui32RGBBitCount contains the luminance channel bit count; ui32RBitMask contains the channel mask). Can be combined with SL2_DPFF_ALPHAPIXELS for a two-channel DDS file. */
	};

	/** DDS flags. */
	enum SL2_DDS_FLAGS {
		SL2_DF_CAPS											= 0x1,						/**< Required in every .dds file. */
		SL2_DF_HEIGHT										= 0x2,						/**< Required in every .dds file. */
		SL2_DF_WIDTH										= 0x4,						/**< Required in every .dds file. */
		SL2_DF_PITCH										= 0x8,						/**< Required when pitch is provided for an uncompressed texture. */
		SL2_DF_PIXELFORMAT									= 0x1000,					/**< Required in every .dds file. */
		SL2_DF_MIPMAPCOUNT									= 0x20000,					/**< Required in a mipmapped texture. */
		SL2_DF_LINEARSIZE									= 0x80000,					/**< Required when pitch is provided for a compressed texture. */
		SL2_DF_DEPTH										= 0x800000,					/**< Required in a depth texture. */
	};

	/** Caps1 flags. */
	enum SL2_CAPS1 {
		SL2_DDSCAPS_COMPLEX									= 0x8,
		SL2_DDSCAPS_MIPMAP									= 0x400000,
		SL2_DDSCAPS_TEXTURE									= 0x1000,
	};

	/** Cap2 flags. */
	enum SL2_CAPS2 {
		SL2_DDSCAPS2_CUBEMAP								= 0x200,
		SL2_DDSCAPS2_CUBEMAP_POSITIVEX						= 0x400,
		SL2_DDSCAPS2_CUBEMAP_NEGATIVEX						= 0x800,
		SL2_DDSCAPS2_CUBEMAP_POSITIVEY						= 0x1000,
		SL2_DDSCAPS2_CUBEMAP_NEGATIVEY						= 0x2000,
		SL2_DDSCAPS2_CUBEMAP_POSITIVEZ						= 0x4000,
		SL2_DDSCAPS2_CUBEMAP_NEGATIVEZ						= 0x8000,
		SL2_DDSCAPS2_VOLUME									= 0x200000,
	};

	/** Resource dimensions. */
	enum SL2_RESOURCE_DIMENSIONS {
		SL2_DDS_DIMENSION_TEXTURE1D							= 2,
		SL2_DDS_DIMENSION_TEXTURE2D							= 3,
		SL2_DDS_DIMENSION_TEXTURE3D							= 4,
	};

	/** Resource extra. */
	enum SL2_RESOURCE_MISC {
		SL2_DDS_RESOURCE_MISC_TEXTURECUBE					= 0x4,
	};

	/** Alpha mode. */
	enum SL2_ALPHA_MODE {
		SL2_DDS_ALPHA_MODE_UNKNOWN							= 0x0,
		SL2_DDS_ALPHA_MODE_STRAIGHT							= 0x1,
		SL2_DDS_ALPHA_MODE_PREMULTIPLIED					= 0x2,
		SL2_DDS_ALPHA_MODE_OPAQUE							= 0x3,
		SL2_DDS_ALPHA_MODE_CUSTOM							= 0x4,
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


		// == Enumerations.
		/** DXGI formats.*/
		enum SL2_DXGI_FORMAT {
			SL2_DXGI_FORMAT_UNKNOWN = 0,
			SL2_DXGI_FORMAT_R32G32B32A32_TYPELESS = 1,
			SL2_DXGI_FORMAT_R32G32B32A32_FLOAT = 2,
			SL2_DXGI_FORMAT_R32G32B32A32_UINT = 3,
			SL2_DXGI_FORMAT_R32G32B32A32_SINT = 4,
			SL2_DXGI_FORMAT_R32G32B32_TYPELESS = 5,
			SL2_DXGI_FORMAT_R32G32B32_FLOAT = 6,
			SL2_DXGI_FORMAT_R32G32B32_UINT = 7,
			SL2_DXGI_FORMAT_R32G32B32_SINT = 8,
			SL2_DXGI_FORMAT_R16G16B16A16_TYPELESS = 9,
			SL2_DXGI_FORMAT_R16G16B16A16_FLOAT = 10,
			SL2_DXGI_FORMAT_R16G16B16A16_UNORM = 11,
			SL2_DXGI_FORMAT_R16G16B16A16_UINT = 12,
			SL2_DXGI_FORMAT_R16G16B16A16_SNORM = 13,
			SL2_DXGI_FORMAT_R16G16B16A16_SINT = 14,
			SL2_DXGI_FORMAT_R32G32_TYPELESS = 15,
			SL2_DXGI_FORMAT_R32G32_FLOAT = 16,
			SL2_DXGI_FORMAT_R32G32_UINT = 17,
			SL2_DXGI_FORMAT_R32G32_SINT = 18,
			SL2_DXGI_FORMAT_R32G8X24_TYPELESS = 19,
			SL2_DXGI_FORMAT_D32_FLOAT_S8X24_UINT = 20,
			SL2_DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS = 21,
			SL2_DXGI_FORMAT_X32_TYPELESS_G8X24_UINT = 22,
			SL2_DXGI_FORMAT_R10G10B10A2_TYPELESS = 23,
			SL2_DXGI_FORMAT_R10G10B10A2_UNORM = 24,
			SL2_DXGI_FORMAT_R10G10B10A2_UINT = 25,
			SL2_DXGI_FORMAT_R11G11B10_FLOAT = 26,
			SL2_DXGI_FORMAT_R8G8B8A8_TYPELESS = 27,
			SL2_DXGI_FORMAT_R8G8B8A8_UNORM = 28,
			SL2_DXGI_FORMAT_R8G8B8A8_UNORM_SRGB = 29,
			SL2_DXGI_FORMAT_R8G8B8A8_UINT = 30,
			SL2_DXGI_FORMAT_R8G8B8A8_SNORM = 31,
			SL2_DXGI_FORMAT_R8G8B8A8_SINT = 32,
			SL2_DXGI_FORMAT_R16G16_TYPELESS = 33,
			SL2_DXGI_FORMAT_R16G16_FLOAT = 34,
			SL2_DXGI_FORMAT_R16G16_UNORM = 35,
			SL2_DXGI_FORMAT_R16G16_UINT = 36,
			SL2_DXGI_FORMAT_R16G16_SNORM = 37,
			SL2_DXGI_FORMAT_R16G16_SINT = 38,
			SL2_DXGI_FORMAT_R32_TYPELESS = 39,
			SL2_DXGI_FORMAT_D32_FLOAT = 40,
			SL2_DXGI_FORMAT_R32_FLOAT = 41,
			SL2_DXGI_FORMAT_R32_UINT = 42,
			SL2_DXGI_FORMAT_R32_SINT = 43,
			SL2_DXGI_FORMAT_R24G8_TYPELESS = 44,
			SL2_DXGI_FORMAT_D24_UNORM_S8_UINT = 45,
			SL2_DXGI_FORMAT_R24_UNORM_X8_TYPELESS = 46,
			SL2_DXGI_FORMAT_X24_TYPELESS_G8_UINT = 47,
			SL2_DXGI_FORMAT_R8G8_TYPELESS = 48,
			SL2_DXGI_FORMAT_R8G8_UNORM = 49,
			SL2_DXGI_FORMAT_R8G8_UINT = 50,
			SL2_DXGI_FORMAT_R8G8_SNORM = 51,
			SL2_DXGI_FORMAT_R8G8_SINT = 52,
			SL2_DXGI_FORMAT_R16_TYPELESS = 53,
			SL2_DXGI_FORMAT_R16_FLOAT = 54,
			SL2_DXGI_FORMAT_D16_UNORM = 55,
			SL2_DXGI_FORMAT_R16_UNORM = 56,
			SL2_DXGI_FORMAT_R16_UINT = 57,
			SL2_DXGI_FORMAT_R16_SNORM = 58,
			SL2_DXGI_FORMAT_R16_SINT = 59,
			SL2_DXGI_FORMAT_R8_TYPELESS = 60,
			SL2_DXGI_FORMAT_R8_UNORM = 61,
			SL2_DXGI_FORMAT_R8_UINT = 62,
			SL2_DXGI_FORMAT_R8_SNORM = 63,
			SL2_DXGI_FORMAT_R8_SINT = 64,
			SL2_DXGI_FORMAT_A8_UNORM = 65,
			SL2_DXGI_FORMAT_R1_UNORM = 66,
			SL2_DXGI_FORMAT_R9G9B9E5_SHAREDEXP = 67,
			SL2_DXGI_FORMAT_R8G8_B8G8_UNORM = 68,
			SL2_DXGI_FORMAT_G8R8_G8B8_UNORM = 69,
			SL2_DXGI_FORMAT_BC1_TYPELESS = 70,
			SL2_DXGI_FORMAT_BC1_UNORM = 71,
			SL2_DXGI_FORMAT_BC1_UNORM_SRGB = 72,
			SL2_DXGI_FORMAT_BC2_TYPELESS = 73,
			SL2_DXGI_FORMAT_BC2_UNORM = 74,
			SL2_DXGI_FORMAT_BC2_UNORM_SRGB = 75,
			SL2_DXGI_FORMAT_BC3_TYPELESS = 76,
			SL2_DXGI_FORMAT_BC3_UNORM = 77,
			SL2_DXGI_FORMAT_BC3_UNORM_SRGB = 78,
			SL2_DXGI_FORMAT_BC4_TYPELESS = 79,
			SL2_DXGI_FORMAT_BC4_UNORM = 80,
			SL2_DXGI_FORMAT_BC4_SNORM = 81,
			SL2_DXGI_FORMAT_BC5_TYPELESS = 82,
			SL2_DXGI_FORMAT_BC5_UNORM = 83,
			SL2_DXGI_FORMAT_BC5_SNORM = 84,
			SL2_DXGI_FORMAT_B5G6R5_UNORM = 85,
			SL2_DXGI_FORMAT_B5G5R5A1_UNORM = 86,
			SL2_DXGI_FORMAT_B8G8R8A8_UNORM = 87,
			SL2_DXGI_FORMAT_B8G8R8X8_UNORM = 88,
			SL2_DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM = 89,
			SL2_DXGI_FORMAT_B8G8R8A8_TYPELESS = 90,
			SL2_DXGI_FORMAT_B8G8R8A8_UNORM_SRGB = 91,
			SL2_DXGI_FORMAT_B8G8R8X8_TYPELESS = 92,
			SL2_DXGI_FORMAT_B8G8R8X8_UNORM_SRGB = 93,
			SL2_DXGI_FORMAT_BC6H_TYPELESS = 94,
			SL2_DXGI_FORMAT_BC6H_UF16 = 95,
			SL2_DXGI_FORMAT_BC6H_SF16 = 96,
			SL2_DXGI_FORMAT_BC7_TYPELESS = 97,
			SL2_DXGI_FORMAT_BC7_UNORM = 98,
			SL2_DXGI_FORMAT_BC7_UNORM_SRGB = 99,
			SL2_DXGI_FORMAT_AYUV = 100,
			SL2_DXGI_FORMAT_Y410 = 101,
			SL2_DXGI_FORMAT_Y416 = 102,
			SL2_DXGI_FORMAT_NV12 = 103,
			SL2_DXGI_FORMAT_P010 = 104,
			SL2_DXGI_FORMAT_P016 = 105,
			SL2_DXGI_FORMAT_420_OPAQUE = 106,
			SL2_DXGI_FORMAT_YUY2 = 107,
			SL2_DXGI_FORMAT_Y210 = 108,
			SL2_DXGI_FORMAT_Y216 = 109,
			SL2_DXGI_FORMAT_NV11 = 110,
			SL2_DXGI_FORMAT_AI44 = 111,
			SL2_DXGI_FORMAT_IA44 = 112,
			SL2_DXGI_FORMAT_P8 = 113,
			SL2_DXGI_FORMAT_A8P8 = 114,
			SL2_DXGI_FORMAT_B4G4R4A4_UNORM = 115,
			SL2_DXGI_FORMAT_P208 = 130,
			SL2_DXGI_FORMAT_V208 = 131,
			SL2_DXGI_FORMAT_V408 = 132,
			SL2_DXGI_FORMAT_SAMPLER_FEEDBACK_MIN_MIP_OPAQUE,
			SL2_DXGI_FORMAT_SAMPLER_FEEDBACK_MIP_REGION_USED_OPAQUE,
			SL2_DXGI_FORMAT_FORCE_UINT = 0xFFFFFFFF
		} ;

		/** D3DFMT formats. */
		enum SL2_D3DFORMAT {
			SL2_D3DFMT_UNKNOWN								=  0,

			SL2_D3DFMT_R8G8B8								= 20,
			SL2_D3DFMT_A8R8G8B8								= 21,
			SL2_D3DFMT_X8R8G8B8								= 22,
			SL2_D3DFMT_R5G6B5								= 23,
			SL2_D3DFMT_X1R5G5B5								= 24,
			SL2_D3DFMT_A1R5G5B5								= 25,
			SL2_D3DFMT_A4R4G4B4								= 26,
			SL2_D3DFMT_R3G3B2								= 27,
			SL2_D3DFMT_A8									= 28,
			SL2_D3DFMT_A8R3G3B2								= 29,
			SL2_D3DFMT_X4R4G4B4								= 30,
			SL2_D3DFMT_A2B10G10R10							= 31,
			SL2_D3DFMT_A8B8G8R8								= 32,
			SL2_D3DFMT_X8B8G8R8								= 33,
			SL2_D3DFMT_G16R16								= 34,
			SL2_D3DFMT_A2R10G10B10							= 35,
			SL2_D3DFMT_A16B16G16R16							= 36,

			SL2_D3DFMT_A8P8									= 40,
			SL2_D3DFMT_P8									= 41,

			SL2_D3DFMT_L8									= 50,
			SL2_D3DFMT_A8L8									= 51,
			SL2_D3DFMT_A4L4									= 52,

			SL2_D3DFMT_V8U8									= 60,
			SL2_D3DFMT_L6V5U5								= 61,
			SL2_D3DFMT_X8L8V8U8								= 62,
			SL2_D3DFMT_Q8W8V8U8								= 63,
			SL2_D3DFMT_V16U16								= 64,
			SL2_D3DFMT_A2W10V10U10							= 67,

			SL2_D3DFMT_UYVY									= SL2_MAKEFOURCC( 'U', 'Y', 'V', 'Y' ),
			SL2_D3DFMT_R8G8_B8G8							= SL2_MAKEFOURCC( 'R', 'G', 'B', 'G' ),
			SL2_D3DFMT_YUY2									= SL2_MAKEFOURCC( 'Y', 'U', 'Y', '2' ),
			SL2_D3DFMT_G8R8_G8B8							= SL2_MAKEFOURCC( 'G', 'R', 'G', 'B' ),
			SL2_D3DFMT_DXT1									= SL2_MAKEFOURCC( 'D', 'X', 'T', '1' ),
			SL2_D3DFMT_DXT2									= SL2_MAKEFOURCC( 'D', 'X', 'T', '2' ),
			SL2_D3DFMT_DXT3									= SL2_MAKEFOURCC( 'D', 'X', 'T', '3' ),
			SL2_D3DFMT_DXT4									= SL2_MAKEFOURCC( 'D', 'X', 'T', '4' ),
			SL2_D3DFMT_DXT5									= SL2_MAKEFOURCC( 'D', 'X', 'T', '5' ),

			SL2_D3DFMT_D16_LOCKABLE							= 70,
			SL2_D3DFMT_D32									= 71,
			SL2_D3DFMT_D15S1								= 73,
			SL2_D3DFMT_D24S8								= 75,
			SL2_D3DFMT_D24X8								= 77,
			SL2_D3DFMT_D24X4S4								= 79,
			SL2_D3DFMT_D16									= 80,

			SL2_D3DFMT_D32F_LOCKABLE						= 82,
			SL2_D3DFMT_D24FS8								= 83,

			SL2_D3DFMT_D32_LOCKABLE							= 84,
			SL2_D3DFMT_S8_LOCKABLE							= 85,

			SL2_D3DFMT_L16									= 81,

			SL2_D3DFMT_VERTEXDATA							= 100,
			SL2_D3DFMT_INDEX16								= 101,
			SL2_D3DFMT_INDEX32								= 102,

			SL2_D3DFMT_Q16W16V16U16							= 110,

			SL2_D3DFMT_MULTI2_ARGB8							= SL2_MAKEFOURCC( 'M','E','T','1' ),

			SL2_D3DFMT_R16F									= 111,
			SL2_D3DFMT_G16R16F								= 112,
			SL2_D3DFMT_A16B16G16R16F						= 113,

			SL2_D3DFMT_R32F									= 114,
			SL2_D3DFMT_G32R32F								= 115,
			SL2_D3DFMT_A32B32G32R32F						= 116,

			SL2_D3DFMT_CxV8U8								= 117,
			
			SL2_D3DFMT_A1									= 118,
			SL2_D3DFMT_A2B10G10R10_XR_BIAS					= 119,
			SL2_D3DFMT_BINARYBUFFER							= 199,


			// == Special Formats == //
			SL2_D3DFMT_BC4U									= SL2_MAKEFOURCC( 'B', 'C', '4', 'U' ),
			SL2_D3DFMT_BC4S									= SL2_MAKEFOURCC( 'B', 'C', '4', 'S' ),
			SL2_D3DFMT_BC5U									= SL2_MAKEFOURCC( 'A', 'T', 'I', '2' ),
			SL2_D3DFMT_BC5S									= SL2_MAKEFOURCC( 'B', 'C', '5', 'S' ),

			SL2_D3DFMT_FORCE_DWORD							= 0x7FFFFFFF
		};


		// == Types.
		/** The DDS header pixel format structure. */
#pragma pack( push, 1 )
		typedef struct SL2_DDS_PIXELFORMAT {
			uint32_t										ui32Size;
			uint32_t										ui32Flags;
			uint32_t										ui32FourCC;
			uint32_t										ui32RGBBitCount;
			uint32_t										ui32RBitMask;
			uint32_t										ui32GBitMask;
			uint32_t										ui32BBitMask;
			uint32_t										ui32ABitMask;
		} * LPSL2_DDS_PIXELFORMAT, * const LPCSL2_DDS_PIXELFORMAT;

		/** The DDS header. */
		typedef struct SL2_DDS_HEADER {
			uint32_t										ui32Size;
			uint32_t										ui32Flags;
			uint32_t										ui32Height;
			uint32_t										ui32Width;
			uint32_t										ui32PitchOrLinearSize;
			uint32_t										ui32Depth;
			uint32_t										ui32MipMapCount;
			uint32_t										ui32Reserved1[11];
			SL2_DDS_PIXELFORMAT								dpPixelFormat;
			uint32_t										ui32Caps;
			uint32_t										ui32Caps2;
			uint32_t										ui32Caps3;
			uint32_t										ui32Caps4;
			uint32_t										ui32Reserved2;
		} * LPSL2_DDS_HEADER, * const LPCSL2_DDS_HEADER;

		/** Extended header. */
		typedef struct SL2_DDS_HEADER_DXT10 {
			uint32_t										ui32DxgiFormat;
			uint32_t										ui32ResourceDimension;
			uint32_t										ui32MiscFlag;
			uint32_t										ui32ArraySize;
			uint32_t										ui32MiscFlags2;
		} * LPSL2_DDS_HEADER_DXT10, * const LPCSL2_DDS_HEADER_DXT10;
#pragma pack( pop )

		/** A conversion function. */
		typedef void (*										PfConversion)( uint8_t * _pui8Src, uint8_t * _pui8Dst, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Depth, uint32_t _ui32Pitch, const SL2_DDS_PIXELFORMAT &_dpfPixelFormat );

		/** A single texture. */
		struct SL2_TEX {
			SL2_TEX() = default;
			SL2_TEX( SL2_TEX &&_tOther ) noexcept :
				vTexture( std::move( _tOther.vTexture ) ),
				ui32Pitch( _tOther.ui32Pitch ),
				ui32W( _tOther.ui32W ),
				ui32H( _tOther.ui32H ),
				ui32D( _tOther.ui32D ) {}
			SL2_TEX( const SL2_TEX & ) = delete;
			SL2_TEX &										operator = ( const SL2_TEX & ) = delete;

			std::vector<uint8_t>							vTexture;
			uint32_t										ui32Pitch;
			uint32_t										ui32W;
			uint32_t										ui32H;
			uint32_t										ui32D;


			SL2_TEX &										operator = ( SL2_TEX &&_tOther ) noexcept {
				if ( this != &_tOther ) {
					vTexture = std::move( _tOther.vTexture );
					ui32Pitch = _tOther.ui32Pitch;
					ui32W = _tOther.ui32W;
					ui32H = _tOther.ui32H;
					ui32D = _tOther.ui32D;
				}
				return (*this);
			}

			
		};

		/** Data associated with each supported format. */
		struct SL2_FORMAT_DATA {
			SL2_D3DFORMAT									fD3dFormat;								/**< The D3D format. */
			SL2_DXGI_FORMAT									dfFormat;								/**< The DXGI format. */
			const char *									pcD3dName;								/**< Name of the D3D format. */
			const char *									pcDxgiName;								/**< Name of the DXGI format. */
			const char *									pcOglInternalFormat;					/**< The OpenGL internal format. */
			const char *									pcOglType;								/**< The OpenGL type. */
			const char *									pcOglBaseInternalFormat;				/**< The OpenGL base internal format. */
			uint8_t											ui8BitsPerBlock;						/**< Bits-per-block for compressed textures or format bit size per-texel. */
			uint8_t											ui8BitsAfterConvert;					/**< Bits-per-block after pfConverter runs. */
			SL2_DDS_PIXEL_FORMAT_FLAGS						pfFormatFlags;							/**< Pixel format flags. */
			bool											bIsCompressed;							/**< Is this a compressed format? */
			bool											bIsPremultiplied;						/**< Is alpha pre-multiplied? */
			bool											bPacked;								/**< Is it a packed or YUV format? */
			PfConversion									pfConverter;							/**< An optional conversion function. */
		};


		/**
		 * Loads a DDS file from memory.
		 *
		 * \param _vFileData The in-memory image of the file.
		 * \return Returns true if the file was successfully loaded.  False indicates an invalid file or lack of RAM.
		 */
		bool												LoadDds( const std::vector<uint8_t> &_vFileData );

		/**
		 * Returns the total size of a compressed image given a factor and its width and height.
		 *
		 * \param _ui32Width Width in pixels.
		 * \param _ui32Height Height in pixels.
		 * \param _ui32Depth Unused.
		 * \param _ui32Bits Bits in the block size.
		 * \param _pvParms Unused.
		 * \return Returns the size of the compressed data.
		 */
		inline uint32_t										GetCompressedSizeBc( uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Depth, uint32_t _ui32Bits ) {
			return ((((_ui32Width + 3) >> 2) * ((_ui32Height + 3) >> 2) * _ui32Bits) >> 3) * _ui32Depth;
		}

		/**
		 * Gets the format.
		 *
		 * \return Returns the DXGI format of the loaded data.
		 **/
		inline SL2_DXGI_FORMAT								Format() const { return m_pfdFormat ? m_pfdFormat->dfFormat : SL2_DXGI_FORMAT_UNKNOWN; }

		/**
		 * Gets the file width.
		 *
		 * \return Returns the file width.
		 **/
		inline uint32_t										Width() const { return std::max( m_dhHeader.ui32Width, static_cast<uint32_t>(1) ); }

		/**
		 * Gets the file height.
		 *
		 * \return Returns the file height.
		 **/
		inline uint32_t										Height() const { return std::max( m_dhHeader.ui32Height, static_cast<uint32_t>(1) ); }

		/**
		 * Gets the file depth.
		 *
		 * \return Returns the file depth.
		 **/
		inline uint32_t										Depth() const { return std::max( m_dhHeader.ui32Depth, static_cast<uint32_t>(1) ); }

		/**
		 * Gets the number of mipmaps.
		 *
		 * \return Returns the number of mipmaps in the file.
		 **/
		inline uint32_t										Mips() const { return std::max( m_dhHeader.ui32MipMapCount, static_cast<uint32_t>(1) ); }

		/**
		 * Gets the array size.
		 *
		 * \return Returns the array size.
		 **/
		inline uint32_t										Array() const { return std::max( m_ui32ArraySize, static_cast<uint32_t>(1) ); }

		/**
		 * Gets the face size.
		 *
		 * \return Returns the face size.
		 **/
		inline uint32_t										Faces() const { return std::max( m_ui32Faces, static_cast<uint32_t>(1) ); }

		/**
		 * Gets the pitch.
		 *
		 * \return Returns the pitch.
		 **/
		inline uint32_t										Pitch() const { return m_dhHeader.ui32PitchOrLinearSize; }

		/**
		 * Gets the texture buffers.
		 *
		 * \return Returns a constant reference to the buffered texture data.
		 **/
		inline const std::vector<SL2_TEX> &					Buffers() const { return m_vTextures; }

		/**
		 * Is the extended header used?
		 *
		 * \return Returns true if the SL2_DDS_HEADER_DXT10 structure is used.
		 **/
		inline bool											UsesExtHeader() const { return m_dhHeader.dpPixelFormat.ui32FourCC == SL2_MAKEFOURCC( 'D', 'X', '1', '0' ); }

		/**
		 * Gets a constant reference to the DDS header.
		 *
		 * \return Returns a constant reference to the DDS header.
		 **/
		inline const SL2_DDS_HEADER &						Header() const { return m_dhHeader; }

		/**
		 * Gets a constant reference to the extended header.
		 *
		 * \return Returns a constant reference to the extended header.
		 **/
		inline const SL2_DDS_HEADER_DXT10 &					Header10() const { return m_dh10Header10; }

		/**
		 * Checks if the given DXGI format is supported in DDS.
		 * 
		 * \param _pcDxgiFormat The name of the DXGI format to verify.
		 * \return Returns a pointer to the given format by DXGI if available.
		 **/
		static const SL2_FORMAT_DATA *						DxgiIsSupported( const char * _pcDxgiFormat );

		/**
		 * Finds a fall-back using OpenGL formats.
		 * 
		 * \param _pcFormat The internal format to find by name.
		 * \param _pcType The type to find by name.
		 * \param _pcBaseFormat The base internal format to find by name.
		 * \return Returns a pointer to the given format by OpenGL if available.
		 **/
		static const SL2_FORMAT_DATA *						FindByOgl( const char * _pcFormat, const char * _pcType, const char * _pcBaseFormat );

	protected :
		// == Types.
		/** RGBA8. */
		struct SL2_TEXEL_RGBA8 {
			uint8_t											ui8R;
			uint8_t											ui8G;
			uint8_t											ui8B;
			uint8_t											ui8A;
		};

		/** RGBA8. */
		struct SL2_TEXEL_BGRA8 {
			uint8_t											ui8B;
			uint8_t											ui8G;
			uint8_t											ui8R;
			uint8_t											ui8A;
		};

		/** RGB10A2. */
		struct SL2_TEXEL_RGB10A2 {
			uint32_t										ui8R : 10;
			uint32_t										ui8G : 10;
			uint32_t										ui8B : 10;
			uint32_t										ui8A : 2;
		};


		// == Members.
		size_t												m_sPitch;								/**< Row size in bytes. */
		SL2_FORMAT_DATA *									m_pfdFormat;							/**< The format data. */
		std::vector<SL2_TEX>								m_vTextures;							/**< Each of the textures in the file. */

		uint32_t											m_ui32ArraySize;						/**< Array size. */
		uint32_t											m_ui32Faces;							/**< Number of faces. */

		SL2_DDS_HEADER										m_dhHeader;								/**< The header. */
		SL2_DDS_HEADER_DXT10								m_dh10Header10;							/**< Extended header. */

		/** Format data. */
		static SL2_FORMAT_DATA								m_fdData[];


		// == Functions.
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
		static void											Convert_UYVY_to_YUY2( uint8_t * _pui8Src, uint8_t * _pui8Dst, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Depth, uint32_t _ui32Pitch, const SL2_DDS_PIXELFORMAT &_dpfPixelFormat );

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
		static void											Convert_RGB24_to_RGBA32( uint8_t * _pui8Src, uint8_t * _pui8Dst, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Depth, uint32_t _ui32Pitch, const SL2_DDS_PIXELFORMAT &_dpfPixelFormat );

		/**
		 * Converts a maskd 32-bit RGBA to a 32-bit SL2_DXGI_FORMAT_R8G8B8A8_UNORM or SL2_DXGI_FORMAT_R8G8B8A8_UNORM_SRGB.
		 * 
		 * \param _pui8Src The data to convert.
		 * \param _pui8Dst The data to convert.
		 * \param _ui32Width The width of the given data.
		 * \param _ui32Height The height of the given data.
		 * \param _ui32Depth The depth of the given data.
		 * \param _ui32Pitch The row width in bytes of the given data.
		 * \param _dpfPixelFormat The pixel format data.
		 **/
		template <typename _tDstType, bool _bHasAlpha>
			static void										Convert_RGBA32_to_RGBA32( uint8_t * _pui8Src, uint8_t * _pui8Dst, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Depth, uint32_t _ui32Pitch, const SL2_DDS_PIXELFORMAT &_dpfPixelFormat ) {
			double dR, dG, dB, dA;
			size_t sR = CUtilities::BitMaskToShift( _dpfPixelFormat.ui32RBitMask, dR );
			size_t sG = CUtilities::BitMaskToShift( _dpfPixelFormat.ui32GBitMask, dG );
			size_t sB = CUtilities::BitMaskToShift( _dpfPixelFormat.ui32BBitMask, dB );
			size_t sA;
			if constexpr ( _bHasAlpha ) {
				sA = CUtilities::BitMaskToShift( _dpfPixelFormat.ui32BBitMask, dA );
			}
			else {
				sA = 255;
			}
			
			size_t sSliceSize = _ui32Pitch * _ui32Height;		
			for ( uint32_t D = 0; D < _ui32Depth; ++D ) {
				for ( uint32_t H = 0; H < _ui32Height; ++H ) {
					for ( uint32_t W = 0; W < _ui32Width; ++W ) {
						uint32_t * pui32TexelsSrc = reinterpret_cast<uint32_t *>(_pui8Src + _ui32Pitch * H + sSliceSize * D) + W;
						_tDstType * ptTexelsDst = reinterpret_cast<_tDstType *>(_pui8Dst + _ui32Pitch * H + sSliceSize * D) + W;
						ptTexelsDst->ui8R = (*pui32TexelsSrc) >> sR;
						ptTexelsDst->ui8G = (*pui32TexelsSrc) >> sG;
						ptTexelsDst->ui8B = (*pui32TexelsSrc) >> sB;
						if constexpr ( _bHasAlpha ) {
							ptTexelsDst->ui8A = (*pui32TexelsSrc) >> sA;
						}
						else {
							ptTexelsDst->ui8A = static_cast<uint8_t>(sA);
						}
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
		static SL2_FORMAT_DATA *							FormatByD3DFORMAT( SL2_D3DFORMAT _dfFormat );

		/**
		 * Gets format data given a SL2_DXGI_FORMAT format.
		 * 
		 * \param _dfFormat The format to find.
		 * \return Returns a matching SL2_FORMAT_DATA pointer from m_fdData or nullptr.
		 **/
		static SL2_FORMAT_DATA *							FormatByDXGI_FORMAT( SL2_DXGI_FORMAT _dfFormat );
	};

}	// namespace sl2

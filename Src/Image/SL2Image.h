﻿/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: An image is a collection of surfaces:
 *	1: Mipmaps
 *	2: Array
 *	3: 3D Slice
 *	4: Cube Face
 * 
 * This class is the primary workhorse of the tool.  Image conversion and manipulation takes place here, although the functions for conversion etc. are in SL2Formats.
 */

#pragma once

#include "../Utilities/SL2Resampler.h"
#include "ICC/SL2Icc.h"
#include "ISPC/cielab_ispc.h"
#include "PVRTexTool/PVRTexLib.hpp"
#include "SL2Formats.h"
#include "SL2Kernel.h"
#include "SL2Palette.h"
#include "SL2PaletteSet.h"
#include "SL2Surface.h"

#include <cstdint>
#include <FreeImage.h>
#include <ktx.h>
#include <memory>
#include <vector>


namespace sl2 {

	// == Enumerations.
	/** Nearest-to resizing. */
	enum SL2_RESAMPLE_TO {
		SL2_RT_NONE,
		SL2_RT_NEAREST,
		SL2_RT_NEXT_HI,
		SL2_RT_NEXT_LO,
		SL2_RT_FIT,
	};

	/** Quick rotations. */
	enum SL2_QUICK_ROTATION {
		SL2_QR_ROT_0,
		SL2_QR_ROT_90,
		SL2_QR_ROT_180,
		SL2_QR_ROT_270
	};

	/**
	 * Class CImage
	 * \brief This class is the primary workhorse of the tool.
	 *
	 * Description: An image is a collection of surfaces:
	 *	1: Mipmaps
	 *	2: Array
	 *	3: 3D Slice
	 *	4: Cube Face
	 * 
	 * This class is the primary workhorse of the tool.  Image conversion and manipulation takes place here.
	 */
	class CImage {
	public :
		CImage();
		~CImage();


		// == Types.
		/** Wraps a FreeImage file. */
		struct SL2_FREE_IMAGE {
			SL2_FREE_IMAGE() :
				pmMemory( ::FreeImage_OpenMemory() ) {
			}
			SL2_FREE_IMAGE( const std::vector<uint8_t> &_vData ) :
				pmMemory( ::FreeImage_OpenMemory( const_cast<uint8_t *>(_vData.data()), DWORD( _vData.size() ) ) ) {
			}
			~SL2_FREE_IMAGE() {
				::FreeImage_CloseMemory( pmMemory );
				pmMemory = nullptr;
			}


			// == Members.
			FIMEMORY *										pmMemory;
		};

		/** Wraps FreeImage_LoadFromMemory(). */
		struct SL2_FREEIMAGE_LOAD_FROM_MEMORY {
			SL2_FREEIMAGE_LOAD_FROM_MEMORY( SL2_FREE_IMAGE &_fiImage ) :
				pbBitmap( ::FreeImage_LoadFromMemory( ::FreeImage_GetFileTypeFromMemory( _fiImage.pmMemory, 0 ), _fiImage.pmMemory, 0 ) ) {
			}
			~SL2_FREEIMAGE_LOAD_FROM_MEMORY() {
				::FreeImage_Unload( pbBitmap );
				pbBitmap = nullptr;
			}


			// == Members.
			FIBITMAP *										pbBitmap;
		};

		/** Wraps FreeImage_LoadMultiBitmapFromMemory(). */
		struct SL2_FREEIMAGE_LOAD_MULTI_BIPMAP_FROM_MEMORY {
			SL2_FREEIMAGE_LOAD_MULTI_BIPMAP_FROM_MEMORY( SL2_FREE_IMAGE &_fiImage ) :
				pbBitmap( ::FreeImage_LoadMultiBitmapFromMemory( ::FreeImage_GetFileTypeFromMemory( _fiImage.pmMemory, 0 ), _fiImage.pmMemory, 0 ) ) {
			}
			~SL2_FREEIMAGE_LOAD_MULTI_BIPMAP_FROM_MEMORY() {
				::FreeImage_CloseMultiBitmap( pbBitmap );
				pbBitmap = nullptr;
			}


			// == Members.
			FIMULTIBITMAP *									pbBitmap;
		};

		/** Wraps FreeImage_OpenMultiBitmapFromHandle(). */
		struct SL2_FREEIMAGE_OPEN_MULTI_BIPMAP_FROM_HANDLE {
			SL2_FREEIMAGE_OPEN_MULTI_BIPMAP_FROM_HANDLE( FREE_IMAGE_FORMAT _fiF, FreeImageIO * _pfiIo, fi_handle _hHandle, int _iFlags FI_DEFAULT( 0 ) ) :
				pbBitmap( ::FreeImage_OpenMultiBitmapFromHandle( _fiF, _pfiIo, _hHandle,  _iFlags ) ) {
			}
			~SL2_FREEIMAGE_OPEN_MULTI_BIPMAP_FROM_HANDLE() {
				::FreeImage_CloseMultiBitmap( pbBitmap );
				pbBitmap = nullptr;
			}


			// == Members.
			FIMULTIBITMAP *									pbBitmap;
		};

		/** Wraps FreeImage_Allocate(). */
		struct SL2_FREEIMAGE_ALLOCATE {
			SL2_FREEIMAGE_ALLOCATE( int _iWidth, int _iHeight, int _iBpp, unsigned _uRedMask FI_DEFAULT(0), unsigned _uGreenMask FI_DEFAULT(0), unsigned _uBlueMask FI_DEFAULT(0) ) :
				pbBitmap( ::FreeImage_Allocate( _iWidth, _iHeight, _iBpp, _uRedMask, _uGreenMask, _uBlueMask ) ) {
			}
			~SL2_FREEIMAGE_ALLOCATE() {
				::FreeImage_Unload( pbBitmap );
				pbBitmap = nullptr;
			}


			// == Members.
			FIBITMAP *										pbBitmap;
		};

		/** Wraps FreeImage_AllocateT(). */
		struct SL2_FREEIMAGE_ALLOCATET {
			SL2_FREEIMAGE_ALLOCATET( FREE_IMAGE_TYPE _fitType, int _iWidth, int _iHeight, int _iBpp = 8, unsigned _uRedMask FI_DEFAULT(0), unsigned _uGreenMask FI_DEFAULT(0), unsigned _uBlueMask FI_DEFAULT(0) ) :
				pbBitmap( ::FreeImage_AllocateT( _fitType, _iWidth, _iHeight, _iBpp, _uRedMask, _uGreenMask, _uBlueMask ) ) {
			}
			~SL2_FREEIMAGE_ALLOCATET() {
				::FreeImage_Unload( pbBitmap );
				pbBitmap = nullptr;
			}


			// == Members.
			FIBITMAP *										pbBitmap;
		};

		/** Wraps FreeImage_LockPage(). */
		struct SL2_FREEIMAGE_LOCK_PAGE {
			SL2_FREEIMAGE_LOCK_PAGE( FIMULTIBITMAP * _pfmpbBitmap, int _iPage ) :
				pfmpbBitmap( _pfmpbBitmap ),
				pbBitmap( ::FreeImage_LockPage( _pfmpbBitmap, _iPage ) ) {
			}
			~SL2_FREEIMAGE_LOCK_PAGE() {
				if ( pfmpbBitmap ) {
					FreeImage_UnlockPage( pfmpbBitmap, pbBitmap, bChanged );
				}
				pbBitmap = nullptr;
				pfmpbBitmap = nullptr;
			}


			// == Members.
			FIBITMAP *										pbBitmap;
			FIMULTIBITMAP *									pfmpbBitmap;
			BOOL											bChanged = FALSE;
		};

		/** Wraps FreeImage_Clone(). */
		struct SL2_FREEIMAGE_CLONE {
			SL2_FREEIMAGE_CLONE() :
				pbBitmap( nullptr ) {}
			SL2_FREEIMAGE_CLONE( FIBITMAP * _pbBitmap ) :
				pbBitmap( ::FreeImage_Clone( _pbBitmap ) ) {
			}
			~SL2_FREEIMAGE_CLONE() {
				::FreeImage_Unload( pbBitmap );
				pbBitmap = nullptr;
			}


			// == Operators.
			SL2_FREEIMAGE_CLONE &							operator = ( FIBITMAP * _pbBitmap ) {
				::FreeImage_Unload( pbBitmap );
				pbBitmap = ::FreeImage_Clone( _pbBitmap );
				return (*this);
			}


			// == Functions.
			FIBITMAP *										Bitmap() { return pbBitmap; }

			
			// == Members.
		protected :
			FIBITMAP *										pbBitmap;
		};

		/** Wraps FIBITMAP(). */
		struct SL2_FREEIMAGE_FIBITMAP {
			SL2_FREEIMAGE_FIBITMAP() :
				pbBitmap( nullptr ) {}
			SL2_FREEIMAGE_FIBITMAP( FIBITMAP * _pbBitmap ) :
				pbBitmap( _pbBitmap ) {
			}
			~SL2_FREEIMAGE_FIBITMAP() {
				::FreeImage_Unload( pbBitmap );
				pbBitmap = nullptr;
			}


			// == Operators.
			SL2_FREEIMAGE_FIBITMAP &						operator = ( FIBITMAP * _pbBitmap ) {
				::FreeImage_Unload( pbBitmap );
				pbBitmap = _pbBitmap;
				return (*this);
			}


			// == Functions.
			FIBITMAP *										Bitmap() { return pbBitmap; }

			
			// == Members.
		protected :
			FIBITMAP *										pbBitmap;
		};

		/** Wraps PVRTexLib_PVRTextureHeader. */
		struct SL2_PVRTEXTUREHEADER {
			SL2_PVRTEXTUREHEADER( PVRTexLib_PVRTextureHeader _thHeader ) :
				thHeader( _thHeader ) {
			}
			~SL2_PVRTEXTUREHEADER() {
				if ( thHeader ) {
					::PVRTexLib_DestroyTextureHeader( thHeader );
					thHeader = nullptr;
				}
			}


			// == Members.
			PVRTexLib_PVRTextureHeader						thHeader;
		};

		/** Wraps PVRTexLib_PVRTexture. */
		struct SL2_PVRTEXTURE {
			SL2_PVRTEXTURE( PVRTexLib_PVRTexture _tTexture ) :
				tTexture( _tTexture ) {
			}
			~SL2_PVRTEXTURE() {
				if ( tTexture ) {
					::PVRTexLib_DestroyTexture( tTexture );
					tTexture = nullptr;
				}
			}


			// == Members.
			PVRTexLib_PVRTexture							tTexture;
		};

#pragma pack( push, 1 )
		/** The bitmap file header. */
		typedef struct SL2_BITMAPFILEHEADER {
			uint16_t										ui16Header;
			uint32_t										ui32Size;
			uint16_t										ui16Reserved1;
			uint16_t										ui16Reserved2;
			uint32_t										ui32Offset;
		} * LPSL2_BITMAPFILEHEADER, * const LPCSL2_BITMAPFILEHEADER;

		/** The bitmap info header. */
		typedef struct SL2_BITMAPINFOHEADER {
			uint32_t										ui32InfoSize;
			uint32_t										ui32Width;
			uint32_t										ui32Height;
			uint16_t										ui16Planes;
			uint16_t										ui16BitsPerPixel;
			uint32_t										ui32Compression;
			uint32_t										ui32ImageSize;
			uint32_t										ui32PixelsPerMeterX;
			uint32_t										ui32PixelsPerMeterY;
			uint32_t										ui32ColorsInPalette;
			uint32_t										ui32ImportantColors;
		} * LPSL2_BITMAPINFOHEADER, * const LPCSL2_BITMAPINFOHEADER;

		/** Bitmap color mask. */
		typedef struct SL2_BITMAPCOLORMASK {
			uint32_t										ui32Red,
															ui32Green,
															ui32Blue,
															ui32Alpha;
		} * LPSL2_BITMAPCOLORMASK, * const LPCSL2_BITMAPCOLORMASK;

		/** Bitmap palette data. */
		typedef union SL2_BITMAPPALETTE {
			struct SL2_BM_COLOR {
				uint8_t										ui8Red,
															ui8Green,
															ui8Blue,
															ui8Alpha;
			};
			uint32_t										ui32Color;
		} * LPSL2_BITMAPPALETTE, * const LPCSL2_BITMAPPALETTE;
#pragma pack( pop )

		/** A chunk header. */
		struct SL2_CHUNK {
#pragma pack( push, 1 )
			uint32_t										ui32Name;				/**< The chunk name. */
			uint32_t										ui32Size;				/**< The chunk size. */
#pragma pack( pop )
		};

		/** A chunk and its data. */
		struct SL2_CHUNK_EX : public SL2_CHUNK {
#pragma pack( push, 1 )
			uint8_t											ui8Data[1];				/**< The beginning of the chunk data. */
#pragma pack( pop )
		};

		/** A "head" chunk. */
		struct SL2_HEAD_CHUNK : public SL2_CHUNK {
#pragma pack( push, 1 )
			uint32_t										ui32Version;			/**< The file version. */
			uint32_t										ui32Width;				/**< The image (mip 0) width. */
			uint32_t										ui32Height;				/**< The image height (mip 0). */
			uint32_t										ui32BitDepth;			/**< The image bit depth (should always be 8). */
			uint32_t										ui32Attribute;			/**< Image attributes. */
#pragma pack( pop )
		};

		/** A "DETL" chunk. */
		struct SL2_DETL_CHUNK : public SL2_CHUNK {
#pragma pack( push, 1 )
			uint32_t										ui32MipLevels;			/**< The number of mip levels, including the base level mip 0. */
#pragma pack( pop )
		};

		/** A "PiDX" chunk. */
		struct SL2_PIDX_CHUNK : public SL2_CHUNK {
#pragma pack( push, 1 )
			uint32_t										ui32PalIdx;				/**< If there is no RIFF chunk, this must exist to locate a palette within the palette database. */
#pragma pack( pop )
		};

		/** A cropping window. */
		struct SL2_WINDOW {
			int32_t											i32X = 0, i32Y = 0, i32Z = 0;		/**< The starting position of the window. */
			uint32_t										ui32W = 0, ui32H = 0, ui32D = 0;	/**< The dimensions of the window. */


			// == Functions.
			/**
			 * Gets the left coordinate.
			 * 
			 * \return Returns the left coordinate.
			 **/
			inline int64_t									Left() const { return i32X; }

			/**
			 * Gets the right coordinate.
			 * 
			 * \return Returns the right coordinate.
			 **/
			inline int64_t									Right() const { return i32X + ui32W; }

			/**
			 * Gets the top coordinate.
			 * 
			 * \return Returns the top coordinate.
			 **/
			inline int64_t									Top() const { return i32Y; }

			/**
			 * Gets the bottom coordinate.
			 * 
			 * \return Returns the bottom coordinate.
			 **/
			inline int64_t									Bottom() const { return i32Y + ui32H; }

			/**
			 * Gets the near coordinate.
			 * 
			 * \return Returns the near coordinate.
			 **/
			inline int64_t									Front() const { return i32Z; }

			/**
			 * Gets the far coordinate.
			 * 
			 * \return Returns the far coordinate.
			 **/
			inline int64_t									Back() const { return i32Z + ui32D; }
		};


		// == Operators.
		/**
		 * Move operator.
		 * 
		 * \param _iOther The image to copy and destroy.
		 * \return Returns a reference to this image.
		 **/
		CImage &											operator = ( CImage &&_iOther ) noexcept;


		// == Functions.
		/**
		 * Resets the object to scratch.  It can be reused after this.
		 **/
		void												Reset();

		/**
		 * Gets the width of the base texture.
		 * 
		 * \return Returns the width of mip-map 0.
		 **/
		inline uint32_t										Width() const;

		/**
		 * Gets the height of the base texture.
		 * 
		 * \return Returns the height of mip-map 0.
		 **/
		inline uint32_t										Height() const;

		/**
		 * Gets the depth of the base texture.
		 * 
		 * \return Returns the depth of mip-map 0.
		 **/
		inline uint32_t										Depth() const;

		/**
		 * Gets the number of mipmaps.
		 * 
		 * \return Returns the number of mipmaps.
		 **/
		inline size_t										Mipmaps() const;

		/**
		 * Gets the number of array elements.
		 * 
		 * \return Returns the number of array elements.
		 **/
		inline size_t										ArraySize() const;

		/**
		 * Gets the number of faces.
		 * 
		 * \return Returns the number of faces.
		 **/
		inline size_t										Faces() const;

		/**
		 * Gets the texture format.
		 * 
		 * \return Returns a constant pointer to the format data, which may be nullptr.
		 **/
		const CFormat::SL2_KTX_INTERNAL_FORMAT_DATA *		Format() const;

		/**
		 * Gets the data buffer for a given mipmap, slice, face, and array index.
		 * 
		 * \param _sMip The mipmap level to retrieve.
		 * \param _ui32Slice The 3D slice to access.
		 * \param _sArray The array index.
		 * \param _sFace The face index.
		 * \return Returns a pointer to the given 2D texture face or nullptr.
		 **/
		inline uint8_t *									Data( size_t _sMip = 0, uint32_t _ui32Slice = 0, size_t _sArray = 0, size_t _sFace = 0 );

		/**
		 * Loads an image file.  All image slices, faces, and array slices will be loaded.
		 * 
		 * \param _pcFile The name of the file to load.
		 * \return Returns an error code.
		 **/
		SL2_ERRORS											LoadFile( const char16_t * _pcFile );

		/**
		 * Loads an image file.  All image slices, faces, and array slices will be loaded.
		 * 
		 * \param _vData The image file to load.
		 * \return Returns an error code.
		 **/
		SL2_ERRORS											LoadFile( const std::vector<uint8_t> &_vData );

		/**
		 * Loads an image format from the clipboard.  PNG is attempted first, the standard bitmap formats.
		 * 
		 * \return Returns an error code.
		 **/
		SL2_ERRORS											LoadFromClipboard();

		/**
		 * Loads a basic YUV image file.  All image slices, faces, and array slices will be loaded.
		 * 
		 * \param _vData The image file to load.
		 * \return Returns an error code.
		 **/
		template <unsigned _uFormat>
		SL2_ERRORS											LoadYuv_Dgxi_Basic( const std::vector<uint8_t> &_vData );

		/**
		 * Loads a basic YUV image file.  All image slices, faces, and array slices will be loaded.
		 * 
		 * \param _vData The image file to load.
		 * \return Returns an error code.
		 **/
		template <unsigned _uFormat>
		SL2_ERRORS											LoadYuv_Vulkan_Basic( const std::vector<uint8_t> &_vData );

		/**
		 * Converts to another format.  _iDst holds the converted image.
		 * 
		 * \param _pkifFormat The format to which to convert.
		 * \param _iDst Holds the converted image.
		 * \return Returns an error code.
		 **/
		SL2_ERRORS											ConvertToFormat( const CFormat::SL2_KTX_INTERNAL_FORMAT_DATA * _pkifFormat, CImage &_iDst );

		/**
		 * Converts a single texture to a given format.
		 * 
		 * \param _pkifFormat The format to which to convert.
		 * \param _sMip The mipmap level to convert.
		 * \param _sArray The array to convert.
		 * \param _sFace The face to convert.
		 * \param _vDst The destination buffer.
		 * \param _bInvertY If true, the destination is inverted vertically.
		 * \return Returns an error code.
		 **/
		SL2_ERRORS											ConvertToFormat( const CFormat::SL2_KTX_INTERNAL_FORMAT_DATA * _pkifFormat,
			size_t _sMip, size_t _sArray, size_t _sFace, std::vector<uint8_t> &_vDst, bool _bInvertY = false, bool _bGenPalette = false, CPalette * _pNewPalette = nullptr );

		/**
		 * Converts a single texture to a given format.
		 * 
		 * \param _pkifFormat The format to which to convert.
		 * \param _sMip The mipmap level to convert.
		 * \param _sArray The array to convert.
		 * \param _sFace The face to convert.
		 * \param _pui8Dst The destination buffer.
		 * \param _bInvertY If true, the destination is inverted vertically.
		 * \return Returns an error code.
		 **/
		SL2_ERRORS											ConvertToFormat( const CFormat::SL2_KTX_INTERNAL_FORMAT_DATA * _pkifFormat,
			size_t _sMip, size_t _sArray, size_t _sFace, uint8_t * _pui8Dst, bool _bInvertY = false, bool _bGenPalette = false, CPalette * _pNewPalette = nullptr );

		/**
		 * Gets a constant reference to the image's mipmaps.
		 * 
		 * \return Returns a constant reference to the image's mipmaps.
		 **/
		const std::vector<std::unique_ptr<CSurface>> &		GetMipmaps() const { return m_vMipMaps; }

		/**
		 * Gets the user-supplied gamma value.
		 * 
		 * \return Returns the user-supplied gamma value.
		 **/
		inline double										Gamma() const { return m_dGamma; }

		/**
		 * Gets the user-supplied target gamma value.
		 * 
		 * \return Returns the user-supplied target gamma value.
		 **/
		inline double										TargetGamma() const { return m_dTargetGamma; }

		/**
		 * Sets the user-supplied gamma value.
		 * 
		 * \param _dGamma The user-supplied gamma.
		 **/
		inline void											SetGamma( double _dGamma ) { m_dGamma = _dGamma; m_bManuallySetGamma = true; }

		/**
		 * Sets the target gamma.
		 * 
		 * \param _dGamma The user-supplied gamma.
		 **/
		inline void											SetTargetGamma( double _dGamma ) { m_dTargetGamma = _dGamma; m_bManuallySetTargetGamma = true; }

		/**
		 * Stes the input and output gamma transfer functions.
		 * 
		 * \param _cgcInput The input gamma curve.
		 * \param _cgcOutput The output gamma curve.
		 **/
		inline void											SetColorSpace( SL2_COLORSPACE_GAMMA_CURVES _cgcInput, SL2_COLORSPACE_GAMMA_CURVES _cgcOutput ) {
			m_cgcInputCurve = _cgcInput;
			m_cgcOutputCurve = _cgcOutput;
		}

		/**
		 * Sets the input ICC file.
		 * 
		 * \param _vFile The input file.
		 * \return Returns false if no profile is provided and allocation of a new default profile fails.
		 **/
		inline bool											SetInputColorSpace( std::vector<uint8_t> &_vFile ) {
			if ( !_vFile.size() && m_vIccProfile.size() ) { return true; }
			m_vIccProfile = std::move( _vFile );
			return true;
		}

		/**
		 * Sets the output ICC file.
		 * 
		 * \param _vFile The output file.
		 * \return Returns false if no profile is provided and allocation of a new default profile fails.
		 **/
		inline bool											SetOutputColorSpace( std::vector<uint8_t> &_vFile ) {
			m_vOutIccProfile = std::move( _vFile );
			return true;
		}

		/**
		 * Sets the input and output rendering intents.
		 * 
		 * \param _i32In The input rendering intent.
		 * \param _i32Out The output rendering intent.
		 **/
		inline void											SetRenderingIntents( int32_t _i32In, int32_t _i32Out ) {
			m_i32InRenderingIntent = _i32In;
			m_i32OutRenderingIntent = _i32Out;
		}

		/**
		 * Gets the output ICC file.
		 *
		 * \return Returns a constant reference to the in-memory output ICC file.
		 **/
		inline const std::vector<uint8_t> &					OutputColorSpace() const { return m_vOutIccProfile; }

		/**
		 * Gets the output gamma curve.
		 *
		 * \return Returns the output gamma curve.
		 **/
		inline SL2_COLORSPACE_GAMMA_CURVES					OutputColorSpaceType() const { return m_cgcOutputCurve; }

		/**
		 * Whether to ignore ICC colorspace gamma.
		 *
		 * \param _bIgnoreSourceColorspaceGamma To ignore or not to ignore.
		 **/
		inline void											SetIgnoreColorspaceGamma( bool _bIgnoreSourceColorspaceGamma ) { m_bIgnoreSourceColorspaceGamma = _bIgnoreSourceColorspaceGamma; }

		/**
		 * Sets the crop window.  Any dimensions set to 0 will be treated as the full image dimension.
		 * 
		 * \param _wWindow The cropping window.
		 **/
		inline void											SetCrop( const SL2_WINDOW &_wWindow ) { m_wCroppingWindow = _wWindow; }

		/**
		 * Sets the quick-rotation angle.
		 * 
		 * \param _qrRot The rotation to set.
		 **/
		inline void											SetQuickRotate( SL2_QUICK_ROTATION _qrRot ) {
			m_qrQuickRotation = _qrRot;
		}

		/**
		 * Sets whether or not alpha needs to be pre-multiplied.
		 * 
		 * \param _bPreMult If true, alpha will be premultiplied.
		 **/
		inline void											SetNeedsPreMultiply( bool _bPreMult ) { m_bNeedsPreMultiply = _bPreMult; }

		/**
		 * Sets whether or not alpha is already pre-multiplied.
		 * 
		 * \param _bPreMult If true, alpha is premultiplied.
		 **/
		inline void											SetIsPreMultiply( bool _bPreMult ) { m_bIsPreMultiplied = _bPreMult; }

		/**
		 * Is the texture alpha-pre-multiplied?
		 *
		 * \return Returns true if the texutre is pre-multiplied.
		 **/
		inline bool											IsPremultiplied() const { return m_bIsPreMultiplied; }

		/**
		 * Sets the swizzle to apply.
		 * 
		 * \param _sSwizzle The swizzle to set.
		 **/
		inline void											SetSwizzle( const CFormat::SL2_SWIZZLE &_sSwizzle ) { m_sSwizzle = _sSwizzle; }

		/**
		 * Sets whether to swap R and B or not.
		 * 
		 * \param _bSwap If true, R and B will be swapped.
		 **/
		inline void											SetSwap( bool _bSwap ) { m_bSwap = _bSwap; }

		/**
		 * Sets whether to vertically flip or not.
		 * 
		 * \param _bFlipX If true, the image will be horizontally flipped.
		 * \param _bFlipY If true, the image will be vertically flipped.
		 * \param _bFlipZ If true, the image will be depth flipped.
		 **/
		inline void											SetFlip( bool _bFlipX, bool _bFlipY, bool _bFlipZ ) { m_bFlipX = _bFlipY; m_bFlipX = _bFlipY; m_bFlipZ = _bFlipZ; }

		/**
		 * Sets whether to ignore alpha or not.
		 * 
		 * \param _bIgnore If true, alpha will be ignored.
		 **/
		inline void											SetIgnoreAlpha( bool _bIgnore ) { m_bIgnoreAlpha = _bIgnore; }

		/**
		 * Gets the texture type.
		 *
		 * \return Returns the the texture type.
		 **/
		inline SL2_TEXTURE_TYPES							TextureType() const { return Depth() > 1 ? SL2_TT_3D : m_ttType; }

		/**
		 * Is this texture fully opaque?
		 *
		 * \return Returns true if all alpha values across all faces, arrays, and mipmaps are set to 1.0.
		 **/
		inline bool											IsFullyOpaque() const { return m_bFullyOpaque; }

		/**
		 * Sets mipmap parameters.
		 * 
		 * \param _mhMipHandling How to handle mipmaps.
		 * \param _sTotal The number of mipmaps to generate or keep.
		 **/
		inline void											SetMipParms( SL2_MIPMAP_HANDLING _mhMipHandling, size_t _sTotal ) {
			m_mhMipHandling = _mhMipHandling;
			m_sTotalMips = _sTotal;
		}

		/**
		 * Gets a read/write reference to the resampling parameters.
		 * 
		 * \return Returns a read/write reference to the resampling parameters.
		 **/
		inline CResampler::SL2_RESAMPLE &					Resampling() { return m_rResample; }

		/**
		 * Gets a read/write reference to the mipmapping resampling parameters.
		 * 
		 * \return Returns a read/write reference to the mipmapping resampling parameters.
		 **/
		inline CResampler::SL2_RESAMPLE &					MipResampling() { return m_rMipResample; }

		/**
		 * Sets normal-map parameters.
		 * 
		 * \param _kKernel The kernel to use.
		 * \param _dScale The normal-map scale.
		 * \param _caNormalChannel The channel to use.
		 * \param _dY The Y axis multiplier.
		 * \return Returns true if the kernel copied successfully.  False indicates a memory failure.
		 **/
		bool												SetNormalMapParms( const CKernel &_kKernel, double _dScale, SL2_CHANNEL_ACCESS _caNormalChannel, double _dY );

		/**
		 * Sets the YUV file width and height.
		 * 
		 * \param _pkifdYuvFormat The YUV file format.
		 * \param _ui32W Width of the YUV file.
		 * \param _ui32H Height of the YUV file.
		 **/
		void												SetYuvSize( const CFormat::SL2_KTX_INTERNAL_FORMAT_DATA * _pkifdYuvFormat, uint32_t _ui32W, uint32_t _ui32H ) {
			m_pkifdYuvFormat = _pkifdYuvFormat;
			m_ui32YuvW = _ui32W;
			m_ui32YuvH = _ui32H;
		}

		/**
		 * Gets a reference to the palette.
		 * 
		 * \return Returns a reference to the palette.
		 **/
		inline CPalette &									Palette() { return m_pPalette.Palette(); }

		/**
		 * By default the old palette is kept if available.  A new palette is always generated if there is no old palette.
		 *	Palettes are generated using only the top-level mipmap.
		 * 
		 * \param _bGenNew Whether to generate a new palette or not.
		 **/
		inline void											SetGenPalette( bool _bGenNew ) {
			m_bGenPalette = _bGenNew;
		}

		/**
		 * Creates a CMYK verion of the given texture slice.
		 * 
		 * \param _ui32Slice The slice to convert.
		 * \param _sMip The mip level to convert.
		 * \param _sArray The array index to convert.
		 * \param _sFace The face to convert.
		 * \param _vResult Holds the resulting CMYK values as 4 doubles per texel.
		 * \return Returns true if allocation of the target buffer and an internal buffer succeed.
		 **/
		bool												ToCmyk( uint32_t _ui32Slice, size_t _sMip, size_t _sArray, size_t _sFace, std::vector<uint8_t> &_vResult );

		/**
		 * Gets the final size of a byte buffer to be used as a texture plane.  The plane will be over-allocated by 8 bytes and then rounded up to the nearest 8 bytes.
		 *	So if a 1-by-1 32-bit tecture is being allocated, 4 will be passed to _sSize, and 16 will be returned ((4+8) -> 16).
		 * 
		 * \param _sSize The base size of the byte buffer previously calculated by the texture's width, height, depth, and texel size.
		 * \return Returns the adjusted size of the texture plane to actually be allocated.
		 **/
		static inline uint64_t								GetActualPlaneSize( uint64_t _sSize );

		/**
		 * Applies a kernel to the given double image.
		 * 
		 * \param _pdImage The buffer on which to operate.
		 * \param _ui32X X coordinate of the texel to which to apply the kernel.
		 * \param _ui32Y Y coordinate of the texel to which to apply the kernel.
		 * \param _ui32W Width of the image buffer.
		 * \param _ui32H Height of the image buffer.
		 * \param _ui32D Depth slice of the image buffer.
		 * \param _kKernel the kernel to apply.
		 * \param _taAddressW Texture-addressing for width.
		 * \param _taAddressH Texture-addressing for height.
		 * \param _dBorder The borner color.
		 * \return Returns true if allocating a temporary buffer succeeded.
		 **/
		static double										ApplyKernel( double * _pdImage, uint32_t _ui32X, uint32_t _ui32Y, uint32_t _ui32W, uint32_t _ui32H, uint32_t _ui32D, const CKernel &_kKernel,
			SL2_TEXTURE_ADDRESSING _taAddressW, SL2_TEXTURE_ADDRESSING _taAddressH, double _dBorder );

		/**
		 * FreeImage read handler.
		 * 
		 * \param _pvBuffer Pointer to the array where the read objects are stored.
		 * \param _uSize Size of each object in bytes.
		 * \param _uCount The number of the objects to be read.
		 * \param _hHandle The stream to read.
		 * \return Number of objects read successfully, which may be less than _uCount if an error or end-of-file condition occurs.  If _uSize or _uCount is zero, fread returns zero and performs no other action.
		 **/
		static unsigned DLL_CALLCONV						FI_Read( void * _pvBuffer, unsigned _uSize, unsigned _uCount, fi_handle _hHandle ) {
			return static_cast<unsigned>(::fread( _pvBuffer, _uSize, _uCount, reinterpret_cast<FILE *>(_hHandle)));
		}

		/**
		 * FreeImage write handler.
		 * 
		 * \param _pvBuffer Pointer to the first object in the array to be written.
		 * \param _uSize Size of each object.
		 * \param _uCount The number of the objects to be written.
		 * \param _hHandle Pointer to the output stream.
		 * \return The number of objects written successfully, which may be less than _uCount if an error occurs.  If _uSize or _uCount is zero, fwrite returns zero and performs no other action.
		 **/
		static unsigned DLL_CALLCONV						FI_Write( void * _pvBuffer, unsigned _uSize, unsigned _uCount, fi_handle _hHandle ) {
			return static_cast<unsigned>(::fwrite( _pvBuffer, _uSize, _uCount, reinterpret_cast<FILE *>(_hHandle)));
		}

		/**
		 * Sets the file position indicator for the file stream stream to the value pointed to by _uOffset.
		 * 
		 * \param _hHandle File stream to modify
		 * \param _uOffset Number of characters to shift the position relative to origin.
		 * \param _uOrigin Position to which offset is added. It can have one of the following values: SEEK_SET, SEEK_CUR, SEEK_END.
		 * \return ​0​ upon success, nonzero value otherwise.
		 **/
		static int DLL_CALLCONV								FI_Seek( fi_handle _hHandle, long _uOffset, int _uOrigin ) {
			return static_cast<int>(::fseek( reinterpret_cast<FILE *>(_hHandle), _uOffset, _uOrigin));
		}

		/**
		 * Returns the file position indicator for the file stream _hHandle.
		 * 
		 * \param _hHandle File stream to examine.
		 * \return File position indicator on success or -1L if failure occurs.
		 **/
		static long DLL_CALLCONV							FI_Tell( fi_handle _hHandle ) {
			return static_cast<long>(::ftell( reinterpret_cast<FILE *>(_hHandle) ));
		}		


	protected :
		// == Members.
		double												m_dGamma;								/**< The gamma curve.  Negative values indicate the IEC 61966-2-1:1999 sRGB curve. */
		double												m_dTargetGamma;							/**< The target gamma curve. */
		SL2_COLORSPACE_GAMMA_CURVES							m_cgcInputCurve;						/**< The input gamma curve. */
		SL2_COLORSPACE_GAMMA_CURVES							m_cgcOutputCurve;						/**< The output gamma curve. */
		CIcc::SL2_TRANSFER_FUNC								m_tfInColorSpaceTransferFunc[4];		/**< The transfer functions for inputs. */
		CIcc::SL2_TRANSFER_FUNC								m_tfOutColorSpaceTransferFunc[4];		/**< The transfer functions for outputs. */
		int32_t												m_i32InRenderingIntent;					/**< Input -> linear rendering intent. */
		int32_t												m_i32OutRenderingIntent;				
		bool												m_bManuallySetGamma;					/**< Was the input gamma manually set? */
		bool												m_bManuallySetTargetGamma;				/**< Was the output gamma manually set? */

		CKernel												m_kKernel;								/**< The kernel to apply. */
		double												m_dKernelScale;							/**< Kernel scale. */
		double												m_dKernelYAxis;							/**< Kernel Y axis. */
		const CFormat::SL2_KTX_INTERNAL_FORMAT_DATA *		m_pkifFormat;							/**< The texture format. */
		std::vector<std::unique_ptr<CSurface>>				m_vMipMaps;								/**< The array of mipmaps.  Index 0 is the base level. */
		size_t												m_sArraySize;							/**< Number of slices in an array.  1 for flat 1D/2D images. */
		size_t												m_sFaces;								/**< 1 for normal textures, 6 for cube textures. */
		CFormat::SL2_SWIZZLE								m_sSwizzle;								/**< Swizzle setting. */
		SL2_CHANNEL_ACCESS									m_caKernelChannal;						/**< The channel for the kernel to access. */
		bool												m_bIsPreMultiplied;						/**< Is the image already pre-multiplied? */
		bool												m_bIgnoreAlpha;							/**< Ignore alpha?  Sets alpha channel to 1.0. */
		bool												m_bNeedsPreMultiply;					/**< Does the image need to be pre-multiplied? */
		bool												m_bFlipX;								/**< Flip horizontally? */
		bool												m_bFlipY;								/**< Flip vertically? */
		bool												m_bFlipZ;								/**< Flip depth? */
		bool												m_bSwap;								/**< Swap R and B? */

		CResampler::SL2_RESAMPLE							m_rResample;							/**< Resample parameters. */
		CResampler::SL2_RESAMPLE							m_rMipResample;							/**< Resample parameters. */

		SL2_MIPMAP_HANDLING									m_mhMipHandling;						/**< How to handle mimaps. */
		size_t												m_sTotalMips;							/**< How many mipmaps to put into the final result, or 0 to keep existing mipmaps or to generate a full set. */

		SL2_TEXTURE_TYPES									m_ttType;								/**< The type of texture. */
		bool												m_bFullyOpaque;							/**< Is the alpha channel just 1.0's? */

		std::vector<uint8_t>								m_vIccProfile;							/**< The ICC profile. */
		std::vector<uint8_t>								m_vOutIccProfile;						/**< The output ICC profile. */
		bool												m_bApplyInputColorSpaceTransfer;		/**< If true, any ICC-profile transfer functions are applied during the X -> Linear conversion. */
		bool												m_bApplyOutputColorSpaceTransfer;		/**< If true, any ICC-profile transfer functions are applied during the Linear -> X conversion. */
		bool												m_bIgnoreSourceColorspaceGamma;			/**< Ignores the gamma curve on embedded or selected colrospace profiles. */

		const CFormat::SL2_KTX_INTERNAL_FORMAT_DATA *		m_pkifdYuvFormat;						/**< The YUV file format. */
		uint32_t											m_ui32YuvW;								/**< The width of a YUV image. */
		uint32_t											m_ui32YuvH;								/**< The height of a YUV image. */

		CPaletteSet											m_pPalette;								/**< The palette. */
		bool												m_bGenPalette;							/**< Generate a new palette? */

		SL2_WINDOW											m_wCroppingWindow;						/**< The cropping window. */
		SL2_QUICK_ROTATION									m_qrQuickRotation;						/**< Quick rotation. */

		std::vector<long>									m_vFrameTimes;							/**< Frame times, in milliseconds. */


		// == Functions.
		/**
		 * Allocates a texture of a given number of mipmaps, array indices, faces, and dimensions.
		 * 
		 * \param _pkifFormat DESC
		 * \param _ui32Width Width of the image base mipmap level.
		 * \param _ui32Height Height of the image base mipmap level.
		 * \param _ui32Depth Depth of the image base mipmap level.
		 * \param _sMips Number of mipmaps.  Must be at least 1.  If 0, a full mipmap chain is allocated.
		 * \param _sArray Number of array slices.  Must be at least 1.
		 * \param _sFaces Number of faces.  Either 1 or 6.
		 * \return Returns true if all mipmaps could be allocated and the texture size is valid (non-0) and a supported format.
		 **/
		bool												AllocateTexture( const CFormat::SL2_KTX_INTERNAL_FORMAT_DATA * _pkifFormat, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Depth, size_t _sMips = 1, size_t _sArray = 1, size_t _sFaces = 1 );

		/**
		 * Determines if any of the parameters change between this image and the given new image format.
		 * 
		 * \param _pkifFormat The new format to which to convert.
		 * \param _bFlip The incoming flip flag.
		 * \param _ui32Width The width of the slice being copied.
		 * \param _ui32Height The height of the slice being copied.
		 * \param _ui32Depth The depth of the slice being copied.
		 * \return Returns true if no changes need to be made when copying this image to a new image given the target format, flip flag, resample paramaters, etc.
		 **/
		bool												ParametersAreUnchanged( const CFormat::SL2_KTX_INTERNAL_FORMAT_DATA * _pkifFormat, bool _bFlip, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Depth );

		/**
		 * Crops an image from one buffer into another.  Coordinates can extend outside of the image, at which time the border color will be used.
		 * 
		 * \param _pui8Src The source buffer (RGBA64F).  On output, points to the location of the cropped buffer.  if there was no cropping operation, this is unchanged, otherwise it will point to _pdDst.
		 * \param _vDst The destination buffer (RGBA64F).
		 * \param _ui32Width Width of the source image on input, width of the new image on output.
		 * \param _ui32Height Height of the source image on input, width of the new image on output.
		 * \param _ui32Depth Depth of the source image on input, width of the new image on output.
		 * \param _taX Texture-addressing mode for sampling outside of the image horizontally.
		 * \param _taY Texture-addressing mode for sampling outside of the image vertically.
		 * \param _taZ Texture-addressing mode for sampling outside of the image depthedly.
		 * \param _wWindow the cropping window and size of the output image.
		 * \param _pdBorderColor The border color for sampling outside the source image.
		 **/
		void												Crop( uint8_t * &_pui8Src, std::vector<uint8_t> &_vDst,
			uint32_t &_ui32Width, uint32_t &_ui32Height, uint32_t &_ui32Depth,
			SL2_TEXTURE_ADDRESSING _taX, SL2_TEXTURE_ADDRESSING _taY, SL2_TEXTURE_ADDRESSING _taZ,
			const SL2_WINDOW &_wWindow, const double * _pdBorderColor );

		/**
		 * Determines if the given window contains exactly the full image.
		 * 
		 * \param _ui32Width The image width.
		 * \param _ui32Height The image height.
		 * \param _ui32Depth The image depth.
		 * \param _wWindow The cropping window to test
		 * \return Returns true if the cropping window exactly matches the full image.
		 **/
		bool												CropIsFullSize( uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Depth, const SL2_WINDOW &_wWindow );

		/**
		 * Applies a quick rotation on a buffer in-place.
		 * 
		 * \param _pui8Src The buffer to rotate in-place.
		 * \param _ui32Width On input, the width of the incoming buffer.  On output, the width of the rotated image.
		 * \param _ui32Height On input, the height of the incoming buffer.  On output, the height of the rotated image.
		 * \param _ui32Depth The depth of the buffer.
		 * \param _qrRot The amount to quick-rotate.
		 * \return Returns true if a temporary buffer was able to be created.  False always means memory failure.
		 **/
		bool												QuickRotate( uint8_t * _pui8Src,
			uint32_t &_ui32Width, uint32_t &_ui32Height, uint32_t _ui32Depth,
			SL2_QUICK_ROTATION _qrRot );

		/**
		 * Bakes the image special gamma into a given texture buffer.  The format must be RGBA64F.
		 * 
		 * \param _pui8Buffer The texture texels.
		 * \param _dGamma The gamma to apply.
		 * \param _ui32Width The width of the image.
		 * \param _ui32Height The height of the image.
		 * \param _ui32Depth The depth of the image.
		 * \param _ptfGamma The gamma transfer functions.
		 **/
		void												BakeGamma( uint8_t * _pui8Buffer, double _dGamma, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Depth, CFormat::SL2_TRANSFER_FUNCS _ptfGamma );

		/**
		 * Clamps the given texture between the high and low values.
		 * 
		 * \param _pdBuffer The pointer to the RGBA64F buffer to clamp.
		 * \param _sTotal The number of texels in the given buffer.
		 * \param _dLow The low clamp value.
		 * \param _dHigh The high clamp value.
		 **/
		static bool											Clamp( double * _pdBuffer, size_t _sTotal, double _dLow = 0.0, double _dHigh = 1.0 );

		/**
		 * Applies an ICC colorspace transfer function to a given RGBA64F buffer.
		 * 
		 * \param _pui8Buffer The texture texels.
		 * \param _ui32Width The width of the image.
		 * \param _ui32Height The height of the image.
		 * \param _ui32Depth The depth of the image.
		 * \param _pfGammaFuncR The gamma transfer function.
		 * \param _pvGammaFuncParmR The gamma transfer function parameter.
		 * \param _pfGammaFuncG The gamma transfer function.
		 * \param _pvGammaFuncParmG The gamma transfer function parameter.
		 * \param _pfGammaFuncB The gamma transfer function.
		 * \param _pvGammaFuncParmB The gamma transfer function parameter.
		 **/
		void												ApplyColorSpaceTransferFunction( uint8_t * _pui8Buffer, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Depth,
			CIcc::PfTransferFunc _pfGammaFuncR, const void * _pvGammaFuncParmR,
			CIcc::PfTransferFunc _pfGammaFuncG, const void * _pvGammaFuncParmG,
			CIcc::PfTransferFunc _pfGammaFuncB, const void * _pvGammaFuncParmB );

		/**
		 * Applies the source colorspace profile.  The source color profile is whatever came with the loaded image file.  The destination is a linear color space based on m_cgcOutputCurve
		 * 
		 * \param _pui8Buffer The texture texels.
		 * \param _ui32Width The width of the image.
		 * \param _ui32Height The height of the image.
		 * \param _ui32Depth The depth of the image.
		 * \return Returns true if the profile was applied.
		 **/
		bool												ApplySrcColorSpace( uint8_t * _pui8Buffer, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Depth );

		/**
		 * Applies the destination colorspace profile.  It actually only applies the gamma curve from the output ICC profile.
		 * 
		 * \param _pui8Buffer The texture texels.
		 * \param _ui32Width The width of the image.
		 * \param _ui32Height The height of the image.
		 * \param _ui32Depth The depth of the image.
		 * \return Returns true if the profile was applied.
		 **/
		bool												ApplyDstColorSpace( uint8_t * _pui8Buffer, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Depth );

		/**
		 * Generates a palette with the given number of entries.  The palette format is used to determine the color format.
		 * 
		 * \param _pui8Buffer The pointer to the buffer of colors.
		 * \param _ui64Total The total number of RGBA64F colors to which _pui8Buffer points.
		 * \param _ui32PalTotal The total number of entries to generate in the palette.
		 * \return Returns true if all necessary allocations succeed.
		 **/
		bool												GeneratePalette( const uint8_t * _pui8Buffer, uint64_t _ui64Total, uint32_t _ui32PalTotal );

		/**
		 * Sets alpha to _dValue.
		 * 
		 * \param _pui8Buffer The texture texels.
		 * \param _dValue The value to apply.
		 * \param _ui32Width The width of the image.
		 * \param _ui32Height The height of the image.
		 * \param _ui32Depth The depth of the image.
		 **/
		void												SetAlpha( uint8_t * _pui8Buffer, double _dValue, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Depth );

		/**
		 * Tests alpha for being entirely of a given value.
		 *
		 * \param _pui8Buffer The texture texels.
		 * \param _dValue The value to check.
		 * \param _ui32Width The width of the image.
		 * \param _ui32Height The height of the image.
		 * \param _ui32Depth The depth of the image.
		 * \return Returns true if all values in the alpha channel are euqal to _dValue.
		 **/
		bool												AlphaIsFullyEqualTo( const uint8_t * _pui8Buffer, double _dValue, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Depth );

		/**
		 * Tests alpha for being entirely of a given value.
		 *
		 * \param _pui8Buffer The texture texels.
		 * \param _dValue The value to check.
		 * \param _ui64Total The total number of texels to which _pui8Buffer points.
		 * \return Returns true if all values in the alpha channel are euqal to _dValue.
		 **/
		bool												AlphaIsFullyEqualTo( const uint8_t * _pui8Buffer, double _dValue, uint64_t _ui64Total );

		/**
		 * Converts a given RGBA64F buffer to a normal map.
		 * 
		 * \param _prgbaData The RGBA buffer to convert to a normal map.
		 * \param _ui32W The width of the input texture.
		 * \param _ui32H The height of the input texture.
		 * \param _ui32D The depth slice to convert.
		 * \return Returns true if the temporary buffer was allocated.
		 **/
		bool												ConvertToNormalMap( CFormat::SL2_RGBA64F * _prgbaData, uint32_t _ui32W, uint32_t _ui32H, uint32_t _ui32D );

		/**
		 * Loads using the FreeImage library.
		 * 
		 * \param _vData The file to load.
		 * \return Returns an error code.
		 **/
		SL2_ERRORS											LoadFreeImage( const std::vector<uint8_t> &_vData );

		/**
		 * Loads a page given a FreeImage FIBITMAP pointer and the page index.
		 * 
		 * \param _pbBitmap The page to load.
		 * \param _sIdx The index of the image to load (image array).
		 * \param _sTotalIdx The total number of images to load.
		 * \return Returns an error code.
		 **/
		SL2_ERRORS											LoadFreeImagePage( FIBITMAP * _pbBitmap, size_t _sIdx = 0, size_t _sTotalIdx = 1 );

		/**
		 * \brief Converts a FreeImage bitmap to 32-bit RGBA.
		 *
		 * \param _pbBitmap The input bitmap to convert.
		 * \return FIBITMAP* A 32-bit RGBA bitmap. The caller is responsible for freeing the returned bitmap.
		 */
		FIBITMAP *											ConvertToRGBA32( FIBITMAP * _pbBitmap );

		/**
		 * \brief Converts a 32-bit RGBA bitmap to another bit depth supported by FreeImage.
		 *
		 * \param _pbBitmap The 32-bit RGBA bitmap to convert.
		 * \param _uTargetBpp The target bit depth (1, 2, 4, 8, 24, or 32).
		 * \param _bIs565 Whether the target 16-bit format should use 5-6-5 instead of 5-5-5.
		 * \return FIBITMAP* A bitmap with the specified bit depth. The caller is responsible for freeing the returned bitmap.
		 */
		//FIBITMAP *											ConvertFromRGBA32( FIBITMAP * _pbBitmap, unsigned _uTargetBpp, bool _bIs565 = false );

		/**
		 * \brief Blends one image onto another with transparency support.
		 *
		 * \param _pbBaseCanvas The destination image.
		 * \param _pbBitmapFrame The source image to blend onto the destination.
		 * \param _iFrameLeft The left offset for the frame.
		 * \param _iFrameTop The top offset for the frame.
		 */
		void												AlphaBlend( FIBITMAP * _pbBaseCanvas, FIBITMAP * _pbBitmapFrame, uint16_t _iFrameLeft, uint16_t _iFrameTop );

		/**
		 * Loads a KTX1 file from memory.
		 * 
		 * \param _vData The file to load.
		 * \return Returns an error code.
		 **/
		SL2_ERRORS											LoadKtx1( const std::vector<uint8_t> &_vData );

		/**
		 * Loads a KTX2 file from memory.
		 * 
		 * \param _vData The file to load.
		 * \return Returns an error code.
		 **/
		SL2_ERRORS											LoadKtx2( const std::vector<uint8_t> &_vData );

		/**
		 * Loads a DDS file from memory.
		 * 
		 * \param _vData The file to load.
		 * \return Returns an error code.
		 **/
		SL2_ERRORS											LoadDds( const std::vector<uint8_t> &_vData );

		/**
		 * Loads a BMP file from memory.
		 * 
		 * \param _vData The file to load.
		 * \return Returns an error code.
		 **/
		SL2_ERRORS											LoadBmp( const std::vector<uint8_t> &_vData );

		/**
		 * Loads a Phoenix BMP file from memory.
		 * 
		 * \param _vData The file to load.
		 * \return Returns an error code.
		 **/
		SL2_ERRORS											LoadPBmp( const std::vector<uint8_t> &_vData );

		/**
		 * Callback to load each face's data from a KTX file.
		 *
		 * \param _iMipLevel Mipmap level, starting from 0.
		 * \param _iFace Face index, usually 0, or 0-5 for cube maps.
		 * \param _iWidth Width of the image.
		 * \param _iHeight Height of the image.
		 * \param _iDepth Depth of the image.
		 * \param _ui64FaceLodSize Number of bytes to which _pvPixels points.
		 * \param _pvPixels The actual texture data.
		 * \param _pvUserdata User data passed to the callback for its own use.
		 * \return Returns an error code to indicate failure or success.
		 */
		static ::KTX_error_code								KtxImageLoad( int _iMipLevel, int _iFace,
			int _iWidth, int _iHeight, int _iDepth,
			ktx_uint64_t _ui64FaceLodSize,
			void * _pvPixels, void * _pvUserdata );
	};
	


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Functions.
	/**
	 * Gets the width of the base texture.
	 * 
	 * \return Returns the width of mip-map 0.
	 **/
	inline uint32_t CImage::Width() const { return m_vMipMaps.size() ? m_vMipMaps[0].get()->Width() : 0; }

	/**
	 * Gets the height of the base texture.
	 * 
	 * \return Returns the height of mip-map 0.
	 **/
	inline uint32_t CImage::Height() const { return m_vMipMaps.size() ? m_vMipMaps[0].get()->Height() : 0; }

	/**
	 * Gets the depth of the base texture.
	 * 
	 * \return Returns the depth of mip-map 0.
	 **/
	inline uint32_t CImage::Depth() const { return m_vMipMaps.size() ? m_vMipMaps[0].get()->Depth() : 0; }

	/**
	 * Gets the number of mipmaps.
	 * 
	 * \return Returns the number of mipmaps.
	 **/
	inline size_t CImage::Mipmaps() const { return m_vMipMaps.size(); }

	/**
	 * Gets the number of array elements.
	 * 
	 * \return Returns the number of array elements.
	 **/
	inline size_t CImage::ArraySize() const { return m_sArraySize; }

	/**
	 * Gets the number of faces.
	 * 
	 * \return Returns the number of faces.
	 **/
	inline size_t CImage::Faces() const { return m_sFaces; }

	/**
	 * Gets the texture format.
	 * 
	 * \return Returns a constant pointer to the format data, which may be nullptr.
	 **/
	inline const CFormat::SL2_KTX_INTERNAL_FORMAT_DATA * CImage::Format() const { return m_pkifFormat; }

	/**
	 * Gets the data buffer for a given mipmap, slice, face, and array index.
	 * 
	 * \param _sMip The mipmap level to retrieve.
	 * \param _ui32Slice The 3D slice to access.
	 * \param _sArray The array index.
	 * \param _sFace The face index.
	 * \return Returns a pointer to the given 2D texture face or nullptr.
	 **/
	inline uint8_t * CImage::Data( size_t _sMip, uint32_t _ui32Slice, size_t _sArray, size_t _sFace ) {
		if ( _sMip >= m_vMipMaps.size() || nullptr == m_pkifFormat ) { return nullptr; }
		uint64_t ui64Off = m_vMipMaps[_sMip]->BaseSize() * ((_sArray * m_sFaces) +
			_sFace) +
			CFormat::GetFormatSize( m_pkifFormat, m_vMipMaps[_sMip]->Width(), m_vMipMaps[_sMip]->Height(), _ui32Slice );
		if ( ui64Off >= m_vMipMaps[_sMip]->size() ) { return nullptr; }
		return m_vMipMaps[_sMip]->data() + ui64Off;
	}

	/**
	 * Gets the final size of a byte buffer to be used as a texture plane.  The plane will be over-allocated by 8 bytes and then rounded up to the nearest 8 bytes.
	 *	So if a 1-by-1 32-bit tecture is being allocated, 4 will be passed to _sSize, and 16 will be returned ((4+8) -> 16).
	 * 
	 * \param _sSize The base size of the byte buffer previously calculated by the texture's width, height, depth, and texel size.
	 * \return Returns the adjusted size of the texture plane to actually be allocated.
	 **/
	inline uint64_t CImage::GetActualPlaneSize( uint64_t _sSize ) {
		if ( !_sSize ) { return 0; }
		return ((_sSize + sizeof( uint64_t )) + (sizeof( uint64_t ) - 1ULL)) & ~(sizeof( uint64_t ) - 1ULL);
	}

	/**
	 * Loads a basic YUV image file.  All image slices, faces, and array slices will be loaded.
	 * 
	 * \param _vData The image file to load.
	 * \return Returns an error code.
	 **/
	template <unsigned _uFormat>
	SL2_ERRORS CImage::LoadYuv_Dgxi_Basic( const std::vector<uint8_t> &_vData ) {
		if ( !m_ui32YuvW || !m_ui32YuvH ) { return SL2_E_UNSUPPORTEDSIZE; }
		if ( !m_pkifdYuvFormat ) {
			m_pkifdYuvFormat = CFormat::FindFormatDataByDx( static_cast<SL2_DXGI_FORMAT>(_uFormat) );
			if ( !m_pkifdYuvFormat ) { return SL2_E_INVALIDFILETYPE; }
		}
		uint64_t ui64SrcBaseSize = CFormat::GetFormatSize( m_pkifdYuvFormat, m_ui32YuvW, m_ui32YuvH, 1 );
		uint64_t ui64Depth = uint64_t( _vData.size() / ui64SrcBaseSize );
		if ( uint64_t( uint32_t( ui64Depth ) ) != ui64Depth || ui64Depth == 0 ) { return SL2_E_UNSUPPORTEDSIZE; }
		if ( !AllocateTexture( m_pkifdYuvFormat, m_ui32YuvW, m_ui32YuvH, uint32_t( ui64Depth ) ) ) { return SL2_E_OUTOFMEMORY; }

		ui64SrcBaseSize = CFormat::GetFormatSize( m_pkifdYuvFormat, m_ui32YuvW, m_ui32YuvH, uint32_t( ui64Depth ) );
		if ( uint64_t( size_t( ui64SrcBaseSize ) ) != ui64SrcBaseSize ) { return SL2_E_UNSUPPORTEDSIZE; }
		if ( ui64SrcBaseSize != _vData.size() ) { return SL2_E_BADFORMAT; }
		// Convert to NV12.
		/*uint32_t ui32ChromaW = std::max( (m_ui32YuvW + 1U) / 2U, 1U );
		uint32_t ui32ChromaH = std::max( (m_ui32YuvH + 1U) / 2U, 1U );
		uint64_t ui64YSize = uint64_t( m_ui32YuvW ) * m_ui32YuvH;
		uint64_t ui64Off2 = (ui32ChromaW * ui32ChromaH);
		if ( uint64_t( size_t( ui64YSize ) ) != ui64YSize ) { return SL2_E_UNSUPPORTEDSIZE; }
		const uint8_t * pui8Src = _vData.data();
		uint8_t * pui8Dst = Data();
		for ( uint32_t D = 0; D < uint32_t( ui64Depth ); ++D ) {
			std::memcpy( pui8Dst, pui8Src, size_t( m_ui32YuvW * m_ui32YuvH ) );
			pui8Dst += m_ui32YuvW * m_ui32YuvH;
			pui8Src += m_ui32YuvW * m_ui32YuvH;
			for ( uint32_t H = 0; H < ui32ChromaH; ++H ) {
				for ( uint32_t W = 0; W < ui32ChromaW; ++W ) {
					pui8Dst[(H*ui32ChromaW+W)*2+0] = pui8Src[(H*ui32ChromaW+W)];
					pui8Dst[(H*ui32ChromaW+W)*2+1] = pui8Src[(H*ui32ChromaW+W)+ui64Off2];
				}
			}
			pui8Dst += ui64Off2 * 2;
			pui8Src += ui64Off2 * 2;
		}*/

		std::memcpy( Data(), _vData.data(), _vData.size() );
		return SL2_E_SUCCESS;
	}

	/**
	 * Loads a basic YUV image file.  All image slices, faces, and array slices will be loaded.
	 * 
	 * \param _vData The image file to load.
	 * \return Returns an error code.
	 **/
	template <unsigned _uFormat>
	SL2_ERRORS CImage::LoadYuv_Vulkan_Basic( const std::vector<uint8_t> &_vData ) {
		if ( !m_ui32YuvW || !m_ui32YuvH ) { return SL2_E_UNSUPPORTEDSIZE; }
		if ( !m_pkifdYuvFormat ) {
			m_pkifdYuvFormat = CFormat::FindFormatDataByVulkan( static_cast<SL2_VKFORMAT>(_uFormat) );
			if ( !m_pkifdYuvFormat ) { return SL2_E_INVALIDFILETYPE; }
		}
		uint64_t ui64SrcBaseSize = CFormat::GetFormatSize( m_pkifdYuvFormat, m_ui32YuvW, m_ui32YuvH, 1 );
		uint64_t ui64Depth = uint64_t( _vData.size() / ui64SrcBaseSize );
		if ( uint64_t( uint32_t( ui64Depth ) ) != ui64Depth || ui64Depth == 0 ) { return SL2_E_UNSUPPORTEDSIZE; }
		if ( !AllocateTexture( m_pkifdYuvFormat, m_ui32YuvW, m_ui32YuvH, uint32_t( ui64Depth ) ) ) { return SL2_E_OUTOFMEMORY; }

		ui64SrcBaseSize = CFormat::GetFormatSize( m_pkifdYuvFormat, m_ui32YuvW, m_ui32YuvH, uint32_t( ui64Depth ) );
		if ( uint64_t( size_t( ui64SrcBaseSize ) ) != ui64SrcBaseSize ) { return SL2_E_UNSUPPORTEDSIZE; }
		if ( ui64SrcBaseSize != _vData.size() ) { return SL2_E_BADFORMAT; }

		std::memcpy( Data(), _vData.data(), _vData.size() );
		return SL2_E_SUCCESS;
	}

}	// namespace sl2

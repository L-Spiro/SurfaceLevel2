/**
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

#include "SL2Image.h"
#include "../Files/SL2StdFile.h"
#include "../Utilities/SL2Stream.h"
#include "../Utilities/SL2Vector4.h"
#include "DDS/SL2Dds.h"
#include "SL2KtxTexture.h"

#include <basisu_transcoder.h>


namespace sl2 {

	CImage::CImage() :
		m_dGamma( -2.2 ),
		m_dTargetGamma( -2.2 ),
		m_cgcInputCurve( SL2_CGC_sRGB_PRECISE ),
		m_cgcOutputCurve( SL2_CGC_sRGB_PRECISE ),
		m_i32InRenderingIntent( INTENT_RELATIVE_COLORIMETRIC ),
		m_i32OutRenderingIntent( INTENT_RELATIVE_COLORIMETRIC ),
		m_bManuallySetGamma( false ),
		m_bManuallySetTargetGamma( false ),
		m_sArraySize( 0 ),
		m_sFaces( 0 ),
		m_pkifFormat( nullptr ),
		m_pkifdYuvFormat( nullptr ),
		m_caKernelChannal( SL2_CA_R ),
		m_dKernelScale( 1.0 ),
		m_dKernelYAxis( 1.0 ),
		m_bIsPreMultiplied( false ),
		m_bIgnoreAlpha( false ),
		m_bNeedsPreMultiply( false ),
		m_bFlipX( false ),
		m_bFlipY( false ), 
		m_bFlipZ( false ),
		m_bSwap( false ),
		m_mhMipHandling( SL2_MH_GENERATE_NEW ),
		m_sTotalMips( 0 ),
		m_ttType( SL2_TT_2D ),
		m_bFullyOpaque( false ),
		m_bApplyInputColorSpaceTransfer( true ),
		m_bApplyOutputColorSpaceTransfer( false ),
		m_bIgnoreSourceColorspaceGamma( false ),
		m_ui32YuvW( 0 ),
		m_ui32YuvH( 0 ),
		m_bGenPalette( false ) {
		m_sSwizzle = CFormat::DefaultSwizzle();
	}
	CImage::~CImage() {
		Reset();
	}


	// == Operators.
	/**
	 * Move operator.
	 * 
	 * \param _iOther The image to copy and destroy.
	 * \return Returns a reference to this image.
	 **/
	CImage & CImage::operator = ( CImage &&_iOther ) noexcept {
		if ( this != &_iOther ) {
			m_vMipMaps = std::move( _iOther.m_vMipMaps );
			m_sArraySize = _iOther.m_sArraySize;
			m_kKernel = _iOther.m_kKernel;
			m_sFaces = _iOther.m_sFaces;
			m_pkifFormat = _iOther.m_pkifFormat;
			m_dGamma = _iOther.m_dGamma;
			m_dTargetGamma = _iOther.m_dTargetGamma;
			m_bManuallySetGamma = _iOther.m_bManuallySetGamma;
			m_bManuallySetTargetGamma = _iOther.m_bManuallySetTargetGamma;
			m_cgcInputCurve = _iOther.m_cgcInputCurve;
			m_cgcOutputCurve = _iOther.m_cgcOutputCurve;
			m_i32InRenderingIntent = _iOther.m_i32InRenderingIntent,
			m_i32OutRenderingIntent = _iOther.m_i32OutRenderingIntent;
			for ( size_t I = SL2_ELEMENTS( m_tfInColorSpaceTransferFunc ); I--; ) {
				m_tfInColorSpaceTransferFunc[I] = _iOther.m_tfInColorSpaceTransferFunc[I];
			}
			for ( size_t I = SL2_ELEMENTS( m_tfOutColorSpaceTransferFunc ); I--; ) {
				m_tfOutColorSpaceTransferFunc[I] = _iOther.m_tfOutColorSpaceTransferFunc[I];
			}
			m_sSwizzle = _iOther.m_sSwizzle;
			m_caKernelChannal = _iOther.m_caKernelChannal;
			m_dKernelScale = _iOther.m_dKernelScale;
			m_dKernelYAxis = _iOther.m_dKernelYAxis;
			m_bIsPreMultiplied = _iOther.m_bIsPreMultiplied;
			m_bIgnoreAlpha = _iOther.m_bIgnoreAlpha;
			m_bNeedsPreMultiply = _iOther.m_bNeedsPreMultiply;
			m_bFlipX = _iOther.m_bFlipX;
			m_bFlipY = _iOther.m_bFlipZ;
			m_bFlipZ = _iOther.m_bFlipY;
			m_bSwap = _iOther.m_bSwap;
			m_mhMipHandling = _iOther.m_mhMipHandling;
			m_sTotalMips = _iOther.m_sTotalMips;
			m_rResample = _iOther.m_rResample;
			m_rMipResample = _iOther.m_rMipResample;
			m_ttType = _iOther.m_ttType;
			m_bFullyOpaque = _iOther.m_bFullyOpaque;
			m_vIccProfile = std::move( _iOther.m_vIccProfile );
			m_vOutIccProfile = std::move( _iOther.m_vOutIccProfile );
			m_bApplyInputColorSpaceTransfer = _iOther.m_bApplyInputColorSpaceTransfer;
			m_bApplyOutputColorSpaceTransfer = _iOther.m_bApplyOutputColorSpaceTransfer;
			m_bIgnoreSourceColorspaceGamma = _iOther.m_bIgnoreSourceColorspaceGamma;
			m_pkifdYuvFormat = _iOther.m_pkifdYuvFormat;
			m_ui32YuvW = _iOther.m_ui32YuvW;
			m_ui32YuvH = _iOther.m_ui32YuvH;
			m_pPalette = _iOther.m_pPalette;
			m_bGenPalette = _iOther.m_bGenPalette;
			m_wCroppingWindow = _iOther.m_wCroppingWindow;
			
			_iOther.m_sArraySize = 0;
			_iOther.m_kKernel.SetSize( 0 );
			_iOther.m_sFaces = 0;
			_iOther.m_pkifFormat = nullptr;
			_iOther.m_dGamma = -2.2;
			_iOther.m_dTargetGamma = -2.2;
			_iOther.m_bManuallySetGamma = _iOther.m_bManuallySetTargetGamma = false;
			_iOther.m_cgcInputCurve = SL2_CGC_sRGB_PRECISE;
			_iOther.m_cgcOutputCurve = SL2_CGC_sRGB_PRECISE;
			_iOther.m_i32InRenderingIntent = INTENT_RELATIVE_COLORIMETRIC,
			_iOther.m_i32OutRenderingIntent = INTENT_RELATIVE_COLORIMETRIC;
			for ( size_t I = SL2_ELEMENTS( m_tfInColorSpaceTransferFunc ); I--; ) {
				_iOther.m_tfInColorSpaceTransferFunc[I] = CIcc::SL2_TRANSFER_FUNC();
			}
			for ( size_t I = SL2_ELEMENTS( m_tfOutColorSpaceTransferFunc ); I--; ) {
				_iOther.m_tfOutColorSpaceTransferFunc[I] = CIcc::SL2_TRANSFER_FUNC();
			}
			_iOther.m_sSwizzle = CFormat::DefaultSwizzle();
			_iOther.m_bIsPreMultiplied = false;
			_iOther.m_bIgnoreAlpha = false;
			_iOther.m_bNeedsPreMultiply = false;
			_iOther.m_bFlipX = false;
			_iOther.m_bFlipY = false;
			_iOther.m_bFlipZ = false;
			_iOther.m_bSwap = false;
			_iOther.m_caKernelChannal = SL2_CA_R;
			_iOther.m_dKernelScale = 1.0;
			_iOther.m_dKernelYAxis = 1.0;
			_iOther.m_rResample = _iOther.m_rMipResample = CResampler::SL2_RESAMPLE();
			_iOther.m_mhMipHandling = SL2_MH_GENERATE_NEW;
			_iOther.m_sTotalMips = 0;
			_iOther.m_ttType = SL2_TT_2D;
			_iOther.m_bFullyOpaque = false;
			_iOther.m_bApplyInputColorSpaceTransfer = true;
			_iOther.m_bApplyOutputColorSpaceTransfer = false;
			_iOther.m_bIgnoreSourceColorspaceGamma = false;
			_iOther.m_pkifdYuvFormat = nullptr;
			_iOther.m_ui32YuvW = _iOther.m_ui32YuvH = 0;
			_iOther.m_pPalette.Reset();
			_iOther.m_bGenPalette = false;
			_iOther.m_wCroppingWindow.i32X = _iOther.m_wCroppingWindow.i32Y = _iOther.m_wCroppingWindow.i32Z = 0;
			_iOther.m_wCroppingWindow.ui32W = _iOther.m_wCroppingWindow.ui32H = _iOther.m_wCroppingWindow.ui32D = 0;
		}

		return (*this);
	}


	// == Functions.
	/**
	 * Resets the object to scratch.  It can be reused after this.
	 **/
	void CImage::Reset() {
		m_dGamma = -2.2,
		m_dTargetGamma = -2.2;
		m_cgcInputCurve = SL2_CGC_sRGB_PRECISE;
		m_cgcOutputCurve = SL2_CGC_sRGB_PRECISE;
		m_i32InRenderingIntent = INTENT_RELATIVE_COLORIMETRIC,
		m_i32OutRenderingIntent = INTENT_RELATIVE_COLORIMETRIC;
		m_bManuallySetGamma = m_bManuallySetTargetGamma = false;
		m_sArraySize = 0;
		m_sFaces = 0;
		for ( auto I = m_vMipMaps.size(); I--; ) {
			m_vMipMaps[I]->clear();
			m_vMipMaps[I].reset();
		}
		m_vMipMaps = std::vector<std::unique_ptr<CSurface>>();
		for ( size_t I = SL2_ELEMENTS( m_tfInColorSpaceTransferFunc ); I--; ) {
			m_tfInColorSpaceTransferFunc[I] = CIcc::SL2_TRANSFER_FUNC();
		}
		for ( size_t I = SL2_ELEMENTS( m_tfOutColorSpaceTransferFunc ); I--; ) {
			m_tfOutColorSpaceTransferFunc[I] = CIcc::SL2_TRANSFER_FUNC();
		}
		m_sSwizzle = CFormat::DefaultSwizzle();
		m_caKernelChannal = SL2_CA_R;
		m_dKernelScale = 1.0;
		m_dKernelYAxis = 1.0;
		m_kKernel.SetSize( 0 );
		m_pkifFormat = nullptr;
		m_bIsPreMultiplied = false;
		m_bIgnoreAlpha = false;
		m_bNeedsPreMultiply = false;
		m_bFlipX = false;
		m_bFlipY = false;
		m_bFlipZ = false;
		m_bSwap = false;

		m_mhMipHandling = SL2_MH_GENERATE_NEW;
		m_sTotalMips = 0;

		m_ttType = SL2_TT_2D;
		m_bFullyOpaque = false;
		m_vIccProfile.clear();
		m_vIccProfile = std::vector<uint8_t>();
		m_vOutIccProfile.clear();
		m_vOutIccProfile = std::vector<uint8_t>();
		m_bApplyInputColorSpaceTransfer = true;
		m_bApplyOutputColorSpaceTransfer = false;
		m_bIgnoreSourceColorspaceGamma = false;
		m_pkifdYuvFormat = nullptr;
		m_ui32YuvW = m_ui32YuvH = 0;
		m_pPalette.Reset();
		m_bGenPalette = false;
		m_wCroppingWindow.i32X = m_wCroppingWindow.i32Y = m_wCroppingWindow.i32Z = 0;
		m_wCroppingWindow.ui32W = m_wCroppingWindow.ui32H = m_wCroppingWindow.ui32D = 0;
	}

	/**
	 * Loads an image file.  All image slices, faces, and array slices will be loaded.
	 * 
	 * \param _pcFile The name of the file to load.
	 * \return Returns an error code.
	 **/
	SL2_ERRORS CImage::LoadFile( const char16_t * _pcFile ) {
		std::vector<uint8_t> vFile;
		{
			CStdFile sfFile;
			if ( !sfFile.Open( _pcFile ) ) { return SL2_E_FILENOTFOUND; }
			if ( !sfFile.LoadToMemory( vFile ) ) { return SL2_E_OUTOFMEMORY; }
		}

#define SL2_YUV_CHECK( FMT, EXT )	(m_pkifdYuvFormat && m_pkifdYuvFormat->vfVulkanFormat == SL2_ ## FMT) || ::_wcsicmp( reinterpret_cast<const wchar_t *>(sl2::CFileBase::GetFileExtension( _pcFile ).c_str()), L ## #EXT ) == 0
#define SL2_VUL_YUV( FMT, EXT )									\
	if ( SL2_YUV_CHECK( FMT, EXT ) ) {							\
		return LoadYuv_Vulkan_Basic<SL2_ ## FMT>( vFile );		\
	}
#define SL2_DX_CHECK( FMT, EXT )	(m_pkifdYuvFormat && m_pkifdYuvFormat->dfDxFormat == SL2_ ## FMT) || ::_wcsicmp( reinterpret_cast<const wchar_t *>(sl2::CFileBase::GetFileExtension( _pcFile ).c_str()), L ## #EXT ) == 0
#define SL2_DX_YUV( FMT, EXT )									\
	if ( SL2_DX_CHECK( FMT, EXT ) ) {							\
		return LoadYuv_Dgxi_Basic<SL2_ ## FMT>( vFile );		\
	}

		
		// 4:4:4.
		SL2_VUL_YUV( VK_FORMAT_G16_B16_R16_3PLANE_444_UNORM, yuv444p16 )
		SL2_VUL_YUV( VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_444_UNORM_3PACK16, yuv444p12le )
		SL2_VUL_YUV( VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_444_UNORM_3PACK16, yuv444p10le )
		SL2_VUL_YUV( VK_FORMAT_G8_B8_R8_3PLANE_444_UNORM, yuv444p )

		SL2_VUL_YUV( VK_FORMAT_G16_B16R16_2PLANE_444_UNORM, yuv444y16 )
		SL2_VUL_YUV( VK_FORMAT_G12X4_B12X4R12X4_2PLANE_444_UNORM_3PACK16, yuv444y12le )
		SL2_VUL_YUV( VK_FORMAT_G10X6_B10X6R10X6_2PLANE_444_UNORM_3PACK16, yuv444y10le )
		SL2_VUL_YUV( VK_FORMAT_G8_B8R8_2PLANE_444_UNORM, yuv444y )


		// 4:2:2.
		SL2_VUL_YUV( VK_FORMAT_G16_B16_R16_3PLANE_422_UNORM, yuv422p16 )
		SL2_VUL_YUV( VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_422_UNORM_3PACK16, yuv422p12le )
		SL2_VUL_YUV( VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_422_UNORM_3PACK16, yuv422p10le )
		SL2_VUL_YUV( VK_FORMAT_G8_B8_R8_3PLANE_422_UNORM, yuv422p )

		SL2_VUL_YUV( VK_FORMAT_G16_B16R16_2PLANE_422_UNORM, yuv422y16 )
		SL2_DX_YUV( DXGI_FORMAT_P216, p216 );
		SL2_VUL_YUV( VK_FORMAT_G12X4_B12X4R12X4_2PLANE_422_UNORM_3PACK16, yuv422y12le )
		SL2_VUL_YUV( VK_FORMAT_G10X6_B10X6R10X6_2PLANE_422_UNORM_3PACK16, yuv422y10le )
		SL2_DX_YUV( DXGI_FORMAT_P210, p210 );
		SL2_VUL_YUV( VK_FORMAT_G8_B8R8_2PLANE_422_UNORM, yuv422y )
		SL2_DX_YUV( DXGI_FORMAT_P208, p208 );

		if ( SL2_DX_CHECK( DXGI_FORMAT_Y216, y216 ) || SL2_YUV_CHECK( VK_FORMAT_G16B16G16R16_422_UNORM, yuyv16 ) ) {
			return LoadYuv_Dgxi_Basic<SL2_DXGI_FORMAT_Y216>( vFile );
		}
		SL2_VUL_YUV( VK_FORMAT_G12X4B12X4G12X4R12X4_422_UNORM_4PACK16, yuyv12le )
		if ( SL2_DX_CHECK( DXGI_FORMAT_Y210, y210 ) || SL2_YUV_CHECK( VK_FORMAT_G10X6B10X6G10X6R10X6_422_UNORM_4PACK16, yuyv10le ) ) {
			return LoadYuv_Dgxi_Basic<SL2_DXGI_FORMAT_Y210>( vFile );
		}
		if ( SL2_DX_CHECK( DXGI_FORMAT_YUY2, yuy2 ) || SL2_YUV_CHECK( VK_FORMAT_G8B8G8R8_422_UNORM, yuyv ) ) {
			return LoadYuv_Dgxi_Basic<SL2_DXGI_FORMAT_YUY2>( vFile );
		}

		SL2_VUL_YUV( VK_FORMAT_B16G16R16G16_422_UNORM, uyvy16 );
		SL2_VUL_YUV( VK_FORMAT_B12X4G12X4R12X4G12X4_422_UNORM_4PACK16, uyvy12le );
		SL2_VUL_YUV( VK_FORMAT_B10X6G10X6R10X6G10X6_422_UNORM_4PACK16, uyvy10le );
		if ( SL2_DX_CHECK( VK_FORMAT_B8G8R8G8_422_UNORM, uyv2 ) || SL2_YUV_CHECK( DXGI_FORMAT_R8G8_B8G8_UNORM, uyvy ) ) {
			return LoadYuv_Vulkan_Basic<SL2_VK_FORMAT_B8G8R8G8_422_UNORM>( vFile );
		}

		


		// 4:2:0.
		SL2_VUL_YUV( VK_FORMAT_G16_B16_R16_3PLANE_420_UNORM, yuv420p16 )
		SL2_VUL_YUV( VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_420_UNORM_3PACK16, yuv420p12le )
		SL2_VUL_YUV( VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_420_UNORM_3PACK16, yuv420p10le )
		SL2_VUL_YUV( VK_FORMAT_G8_B8_R8_3PLANE_420_UNORM, yuv420p )
		SL2_DX_YUV( DXGI_FORMAT_420_OPAQUE, yv12 )
		SL2_DX_YUV( DXGI_FORMAT_YV12, yv12 )

		SL2_VUL_YUV( VK_FORMAT_G16_B16R16_2PLANE_420_UNORM, yuv420y16 )
		SL2_DX_YUV( DXGI_FORMAT_P016, p016 );
		SL2_DX_YUV( DXGI_FORMAT_P010, p010 );

		if ( SL2_YUV_CHECK( VK_FORMAT_G12X4_B12X4R12X4_2PLANE_420_UNORM_3PACK16, yuv420y12le ) || ::_wcsicmp( reinterpret_cast<const wchar_t *>(sl2::CFileBase::GetFileExtension( _pcFile ).c_str()), L"p012" ) == 0 ) {
			return LoadYuv_Vulkan_Basic<SL2_VK_FORMAT_G12X4_B12X4R12X4_2PLANE_420_UNORM_3PACK16>( vFile );
		}
		if ( SL2_YUV_CHECK( VK_FORMAT_G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16, yuv420y10le ) || ::_wcsicmp( reinterpret_cast<const wchar_t *>(sl2::CFileBase::GetFileExtension( _pcFile ).c_str()), L"p010" ) == 0 ) {
			return LoadYuv_Vulkan_Basic<SL2_VK_FORMAT_G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16>( vFile );
		}
		if ( SL2_YUV_CHECK( VK_FORMAT_G8_B8R8_2PLANE_420_UNORM, yuv420y ) || SL2_YUV_CHECK( DXGI_FORMAT_NV12, nv12 ) || ::_wcsicmp( reinterpret_cast<const wchar_t *>(sl2::CFileBase::GetFileExtension( _pcFile ).c_str()), L"yuv420y" ) == 0 ) {
			return LoadYuv_Vulkan_Basic<SL2_VK_FORMAT_G8_B8R8_2PLANE_420_UNORM>( vFile );
		}

		SL2_DX_YUV( DXGI_FORMAT_NV21, nv21 )

		// Alpha.
		SL2_DX_YUV( DXGI_FORMAT_Y416, y416 )
		SL2_VUL_YUV( VK_FORMAT_R12X4G12X4B12X4A12X4_UNORM_4PACK16, yuva12le )
		SL2_VUL_YUV( VK_FORMAT_R10X6G10X6B10X6A10X6_UNORM_4PACK16, yuva10le )
		SL2_DX_YUV( DXGI_FORMAT_Y410, y410 )
		SL2_DX_YUV( DXGI_FORMAT_AYUV, ayuv )
		

		return LoadFile( vFile );
#undef SL2_DX_YUV
#undef SL2_DX_CHECK
#undef SL2_YUV_CHECK
#undef SL2_VUL_YUV
	}

	/**
	 * Loads an image file.  All image slices, faces, and array slices will be loaded.
	 * 
	 * \param _vData The image file to load.
	 * \return Returns an error code.
	 **/
	SL2_ERRORS CImage::LoadFile( const std::vector<uint8_t> &_vData ) {
		if ( SL2_E_SUCCESS == LoadBmp( _vData ) ) { return SL2_E_SUCCESS; }
		if ( SL2_E_SUCCESS == LoadDds( _vData ) ) { return SL2_E_SUCCESS; }
		if ( SL2_E_SUCCESS == LoadKtx1( _vData ) ) { return SL2_E_SUCCESS; }
		if ( SL2_E_SUCCESS == LoadKtx2( _vData ) ) { return SL2_E_SUCCESS; }
		return LoadFreeImage( _vData );
	}

	/**
	 * Converts to another format.  _iDst holds the converted image.
	 * 
	 * \param _pkifFormat The format to which to convert.
	 * \param _iDst Holds the converted image.
	 * \return Returns an error code.
	 **/
	SL2_ERRORS CImage::ConvertToFormat( const CFormat::SL2_KTX_INTERNAL_FORMAT_DATA * _pkifFormat, CImage &_iDst ) {
		CImage iTmp;
		if ( !_pkifFormat || !Format() ) { return SL2_E_BADFORMAT; }

		//if ( ParametersAreUnchanged( _pkifFormat, false, 0, 0, 0 ) ) {
		//	// No format conversion needed.  Just copy the buffers.
		//	if ( !_iDst.AllocateTexture( _pkifFormat, Width(), Height(), Depth(), Mipmaps(), ArraySize(), Faces() ) ) { return SL2_E_OUTOFMEMORY; }
		//	for ( size_t M = 0; M < Mipmaps(); ++M ) {
		//		std::memcpy( _iDst.Data( M, 0, 0, 0 ), Data( M, 0, 0, 0 ), m_vMipMaps[M]->size() );
		//	}
		//	return SL2_E_SUCCESS;
		//}

		if ( !Format()->pfToRgba64F ) { return SL2_E_BADFORMAT; }
		
		if ( !m_wCroppingWindow.ui32W ) {
			m_wCroppingWindow.i32X = 0;
			m_wCroppingWindow.ui32W = Width();
		}
		if ( !m_wCroppingWindow.ui32H ) {
			m_wCroppingWindow.i32Y = 0;
			m_wCroppingWindow.ui32H = Height();
		}
		if ( !m_wCroppingWindow.ui32D ) {
			m_wCroppingWindow.i32Z = 0;
			m_wCroppingWindow.ui32D = Depth();
		}
		

		m_rResample.ui32W = m_wCroppingWindow.ui32W;
		m_rResample.ui32H = m_wCroppingWindow.ui32H;
		m_rResample.ui32D = m_wCroppingWindow.ui32D;
		m_rResample.bAlpha = Format()->ui8ABits && _pkifFormat->ui8ABits;
		uint32_t ui32NewW = m_rResample.ui32NewW ? m_rResample.ui32NewW : m_wCroppingWindow.ui32W;
		uint32_t ui32NewH = m_rResample.ui32NewH ? m_rResample.ui32NewH : m_wCroppingWindow.ui32H;
		uint32_t ui32NewD = m_rResample.ui32NewD ? m_rResample.ui32NewD : m_wCroppingWindow.ui32D;
		bool bResize = m_rResample.ui32W != ui32NewW || m_rResample.ui32H != ui32NewH || m_rResample.ui32D != ui32NewD;
		bool bUseTmpBuffer = false;
		if ( bResize ) {
			size_t sOldSize = m_rResample.ui32W * m_rResample.ui32H * m_rResample.ui32D;
			size_t sNewSize = ui32NewW * ui32NewH * ui32NewD;
			if ( sNewSize < sOldSize ) { bUseTmpBuffer = true; }
		}
		m_rMipResample.bAlpha = m_rResample.bAlpha;
		
		

		size_t sSrcMips = Mipmaps();
		size_t sDstMips = Mipmaps();
		switch ( m_mhMipHandling ) {
			case SL2_MH_REMOVE_EXISTING : {
				sSrcMips = sDstMips = 1;
				break;
			}
			case SL2_MH_KEEP_EXISTING : {
				sSrcMips = std::max( std::min( Mipmaps(), m_sTotalMips + 1 ), size_t( 1 ) );
				sDstMips = std::max( m_sTotalMips + 1, Mipmaps() );
				if ( !m_sTotalMips ) {
					sDstMips = CUtilities::Max( size_t( std::floor( std::log2( ui32NewW ) ) ), size_t( std::floor( std::log2( ui32NewH ) ) ) );
					sDstMips = CUtilities::Max( size_t( std::floor( std::log2( ui32NewD ) ) ), sDstMips ) + 1;
				}
				break;
			}
			case SL2_MH_GENERATE_NEW : {
				sSrcMips = 1;
				if ( !m_sTotalMips ) {
					sDstMips = CUtilities::Max( size_t( std::floor( std::log2( ui32NewW ) ) ), size_t( std::floor( std::log2( ui32NewH ) ) ) );
					sDstMips = CUtilities::Max( size_t( std::floor( std::log2( ui32NewD ) ) ), sDstMips ) + 1;
				}
				else {
					sDstMips = m_sTotalMips + 1;
				}
				break;
			}
		}
		if ( sDstMips > sSrcMips ) { bUseTmpBuffer = true; }
		if ( !CropIsFullSize( Width(), Height(), Depth(), m_wCroppingWindow ) ) { bUseTmpBuffer = true; }

		if ( !iTmp.AllocateTexture( CFormat::FindFormatDataByVulkan( SL2_VK_FORMAT_R64G64B64A64_SFLOAT ), ui32NewW, ui32NewH, ui32NewD, sDstMips, ArraySize(), Faces() ) ) { return SL2_E_OUTOFMEMORY; }

		std::vector<double> vTmp;
		if ( bUseTmpBuffer ) {
			try { vTmp.resize( Width() * Height() * Depth() * 4 ); }
			catch ( ... ) { return SL2_E_OUTOFMEMORY; }
		}
		bool bTargetIsPremulAlpha = m_bIsPreMultiplied;
		bool bOpaque = true;
		CFormat::SL2_KTX_INTERNAL_FORMAT_DATA ifdData = (*Format());
		ifdData.pvCustom = this;

		std::vector<uint8_t> vCrop;
		for ( size_t M = 0; M < sSrcMips; ++M ) {
			for ( size_t A = 0; A < ArraySize(); ++A ) {
				for ( size_t F = 0; F < Faces(); ++F ) {
					uint8_t * pui8Dest = iTmp.Data( M, 0, A, F );
					if ( bUseTmpBuffer ) {
						pui8Dest = reinterpret_cast<uint8_t *>(vTmp.data());
					}
					uint32_t ui32W, ui32H, ui32D;
					ui32W = m_vMipMaps[M]->Width();
					ui32H = m_vMipMaps[M]->Height();
					ui32D = m_vMipMaps[M]->Depth();

					if ( !Format()->pfToRgba64F( Data( M, 0, A, F ), pui8Dest, ui32W, ui32H, ui32D, &ifdData ) ) { return SL2_E_INTERNALERROR; }
					try {
						Crop( pui8Dest, vCrop, ui32W, ui32H, ui32D,
							m_rResample.taColorW, m_rResample.taColorH, m_rResample.taColorD,
							m_wCroppingWindow, m_rResample.dBorderColor );
					}
					catch ( ... ) { return SL2_E_OUTOFMEMORY; }
					if ( m_dGamma ) {
						BakeGamma( pui8Dest, 1.0 / m_dGamma, ui32W, ui32H, ui32D, CFormat::TransferFunc( m_cgcInputCurve ) );
					}
					if ( m_bApplyInputColorSpaceTransfer ) {
						ApplySrcColorSpace( pui8Dest, ui32W, ui32H, ui32D );
					}
					if ( m_bIgnoreAlpha ) {
						m_bIsPreMultiplied = m_bNeedsPreMultiply = false;
						SetAlpha( pui8Dest, 1.0, ui32W, ui32H, ui32D );
					}
					if ( m_bFlipX && ui32W > 1 ) {
						CFormat::FlipX( pui8Dest, ui32W, ui32H, ui32D );
					}
					if ( m_bFlipY && ui32H > 1 ) {
						CFormat::FlipY( pui8Dest, ui32W, ui32H, ui32D );
					}
					if ( m_bFlipZ && ui32D > 1 ) {
						CFormat::FlipZ( pui8Dest, ui32W, ui32H, ui32D );
					}
					if ( m_bSwap ) {
						CFormat::Swap( pui8Dest, ui32W, ui32H, ui32D );
					}
					if ( !CFormat::SwizzleIsDefault( m_sSwizzle ) ) {
						CFormat::ApplySwizzle( pui8Dest, ui32W, ui32H, ui32D, m_sSwizzle );
					}

					bool bThisIsOpuaqe = AlphaIsFullyEqualTo( pui8Dest, 1.0, ui32W, ui32H, ui32D );
					bOpaque = bOpaque && bThisIsOpuaqe;
					if ( bResize ) {
						CResampler rResampleMe;
						CResampler::SL2_RESAMPLE rResampleCopy = m_rResample;
						rResampleCopy.ui32W = ui32W;
						rResampleCopy.ui32H = ui32H;
						rResampleCopy.ui32D = ui32D;
						rResampleCopy.ui32NewW = std::max( m_rResample.ui32NewW >> M, 1U );
						rResampleCopy.ui32NewH = std::max( m_rResample.ui32NewH >> M, 1U );
						rResampleCopy.ui32NewD = std::max( m_rResample.ui32NewD >> M, 1U );
						rResampleCopy.bAlpha = rResampleCopy.bAlpha && !bThisIsOpuaqe;
						if ( !rResampleMe.Resample( reinterpret_cast<double *>(pui8Dest), reinterpret_cast<double *>(iTmp.Data( M, 0, A, F )), rResampleCopy ) ) { return SL2_E_OUTOFMEMORY; }
					}
					else if ( bUseTmpBuffer ) {
						// Using the temporary buffer but not resizing?  Copy it over.
						std::memcpy( iTmp.Data( M, 0, A, F ), pui8Dest, ui32W * ui32H * ui32D * 4 * sizeof( double ) );
					}
					if ( M == 0 ) {
						// Generate mipmaps using the original full-sized non-scaled image.
						for ( size_t N = sSrcMips; N < sDstMips; ++N ) {
							CResampler rResampleMe;
							CResampler::SL2_RESAMPLE rResampleCopy = m_rMipResample;
							rResampleCopy.ui32W = ui32W;
							rResampleCopy.ui32H = ui32H;
							rResampleCopy.ui32D = ui32D;
							rResampleCopy.ui32NewW = std::max( m_rResample.ui32NewW >> N, 1U );
							rResampleCopy.ui32NewH = std::max( m_rResample.ui32NewH >> N, 1U );
							rResampleCopy.ui32NewD = std::max( m_rResample.ui32NewD >> N, 1U );
							rResampleCopy.bAlpha = rResampleCopy.bAlpha && !bThisIsOpuaqe;
							if ( !rResampleMe.Resample( reinterpret_cast<double *>(pui8Dest), reinterpret_cast<double *>(iTmp.Data( N, 0, A, F )), rResampleCopy ) ) { return SL2_E_OUTOFMEMORY; }
						}
					}
					if ( !m_bIsPreMultiplied && m_bNeedsPreMultiply ) {
						CFormat::ApplyPreMultiply( iTmp.Data( M, 0, A, F ),
							iTmp.m_vMipMaps[M]->Width(), iTmp.m_vMipMaps[M]->Height(), iTmp.m_vMipMaps[M]->Depth() );
						bTargetIsPremulAlpha = true;
					}
					
				}
			}
		}
		
		for ( size_t M = 0; M < iTmp.Mipmaps(); ++M ) {
			for ( size_t A = 0; A < iTmp.ArraySize(); ++A ) {
				for ( size_t F = 0; F < iTmp.Faces(); ++F ) {
					if ( m_kKernel.Size() ) {
						if ( !ConvertToNormalMap( reinterpret_cast<CFormat::SL2_RGBA64F *>(iTmp.Data( M, 0, A, F )), iTmp.m_vMipMaps[M]->Width(), iTmp.m_vMipMaps[M]->Height(), iTmp.m_vMipMaps[M]->Depth() ) ) { return SL2_E_OUTOFMEMORY; }
					}
					else {
						if ( m_dTargetGamma ) {
							BakeGamma( iTmp.Data( M, 0, A, F ), m_dTargetGamma, iTmp.m_vMipMaps[M]->Width(), iTmp.m_vMipMaps[M]->Height(), iTmp.m_vMipMaps[M]->Depth(), CFormat::TransferFunc( m_cgcOutputCurve ) );
						}
						ApplyDstColorSpace( iTmp.Data( M, 0, A, F ), iTmp.m_vMipMaps[M]->Width(), iTmp.m_vMipMaps[M]->Height(), iTmp.m_vMipMaps[M]->Depth() );
					}
				}
			}
		}

		if ( SL2_GET_IDX_FLAG( _pkifFormat->ui32Flags ) ) {
			// Converting to an indexed format?  May need to generate a palette.
			size_t sMax = size_t( 1ULL << _pkifFormat->ui32BlockSizeInBits );
			if ( m_bGenPalette || Palette().Palette().size() == 0 || Palette().Palette().size() > sMax ) {
				uint64_t ui64Total = uint64_t( iTmp.Width() ) * iTmp.Height() * iTmp.Depth() * iTmp.Faces() * iTmp.ArraySize();
				if ( ui64Total ) {
					if ( !GeneratePalette( iTmp.Data( 0, 0, 0, 0 ), ui64Total, uint32_t( sMax ) ) ) { return SL2_E_OUTOFMEMORY; }
				}
			}
		}

		if ( _pkifFormat->vfVulkanFormat == SL2_VK_FORMAT_R64G64B64A64_SFLOAT ) {
			// We already did the conversion.
			_iDst = std::move( iTmp );
			_iDst.m_bNeedsPreMultiply = _iDst.m_bIsPreMultiplied = bTargetIsPremulAlpha;
			_iDst.m_ttType = m_ttType;
			_iDst.m_bFullyOpaque = bOpaque;
			_iDst.m_dGamma = _iDst.m_dTargetGamma = m_dTargetGamma;
			_iDst.m_cgcInputCurve = _iDst.m_cgcOutputCurve = m_cgcOutputCurve;
			_iDst.m_vIccProfile = _iDst.m_vOutIccProfile = m_vOutIccProfile;
			_iDst.m_bApplyInputColorSpaceTransfer = m_bApplyInputColorSpaceTransfer;
			_iDst.m_pPalette = m_pPalette;
			_iDst.m_bGenPalette = false;
			if ( !_iDst.m_vOutIccProfile.size() ) {
				_iDst.m_bApplyInputColorSpaceTransfer = false;
			}
			for ( size_t I = SL2_ELEMENTS( m_tfOutColorSpaceTransferFunc ); I--; ) {
				_iDst.m_tfInColorSpaceTransferFunc[I] = m_tfOutColorSpaceTransferFunc[I];	// Not a bug.
				_iDst.m_tfOutColorSpaceTransferFunc[I] = m_tfOutColorSpaceTransferFunc[I];
			}
			return SL2_E_SUCCESS;
		}
		if ( !_pkifFormat->pfFromRgba64F ) { return SL2_E_BADFORMAT; }
		_iDst.Reset();
		if ( !_iDst.AllocateTexture( _pkifFormat, ui32NewW, ui32NewH, ui32NewD, iTmp.Mipmaps(), iTmp.ArraySize(), iTmp.Faces() ) ) { return SL2_E_OUTOFMEMORY; }
		ifdData = (*_pkifFormat);
		ifdData.pvCustom = this;
		for ( size_t M = 0; M < iTmp.Mipmaps(); ++M ) {
			for ( size_t A = 0; A < iTmp.ArraySize(); ++A ) {
				for ( size_t F = 0; F < iTmp.Faces(); ++F ) {
					if ( !_pkifFormat->pfFromRgba64F( iTmp.Data( M, 0, A, F ), _iDst.Data( M, 0, A, F ), iTmp.m_vMipMaps[M]->Width(), iTmp.m_vMipMaps[M]->Height(), iTmp.m_vMipMaps[M]->Depth(), &ifdData ) ) {
						return SL2_E_INTERNALERROR;
					}
				}
			}
		}
		_iDst.m_bNeedsPreMultiply = _iDst.m_bIsPreMultiplied = bTargetIsPremulAlpha;
		_iDst.m_ttType = m_ttType;
		_iDst.m_bFullyOpaque = bOpaque;
		_iDst.m_dGamma = _iDst.m_dTargetGamma = m_dTargetGamma;
		_iDst.m_cgcInputCurve = _iDst.m_cgcOutputCurve = m_cgcOutputCurve;
		_iDst.m_vIccProfile = _iDst.m_vOutIccProfile = m_vOutIccProfile;
		_iDst.m_bApplyInputColorSpaceTransfer = m_bApplyInputColorSpaceTransfer;
		_iDst.m_pPalette = m_pPalette;
		_iDst.m_bGenPalette = false;
		if ( !_iDst.m_vOutIccProfile.size() ) {
			_iDst.m_bApplyInputColorSpaceTransfer = false;
		}
		for ( size_t I = SL2_ELEMENTS( m_tfOutColorSpaceTransferFunc ); I--; ) {
			_iDst.m_tfInColorSpaceTransferFunc[I] = m_tfOutColorSpaceTransferFunc[I];	// Not a bug.
			_iDst.m_tfOutColorSpaceTransferFunc[I] = m_tfOutColorSpaceTransferFunc[I];
		}
		return SL2_E_SUCCESS;
	}

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
	SL2_ERRORS CImage::ConvertToFormat( const CFormat::SL2_KTX_INTERNAL_FORMAT_DATA * _pkifFormat,
		size_t _sMip, size_t _sArray, size_t _sFace, std::vector<uint8_t> &_vDst, bool _bInvertY, bool _bGenPalette, CPalette * _pNewPalette ) {
		if ( !_pkifFormat || !Format() ) { return SL2_E_BADFORMAT; }
		if ( _sMip >= m_vMipMaps.size() ) { return SL2_E_INVALIDCALL; }

		uint64_t ui64BaseSize = CFormat::GetFormatSize( _pkifFormat, m_vMipMaps[_sMip]->Width(), m_vMipMaps[_sMip]->Height(), m_vMipMaps[_sMip]->Depth() );
		ui64BaseSize = GetActualPlaneSize( ui64BaseSize );
		if ( uint64_t( size_t( ui64BaseSize ) ) != ui64BaseSize ) { return SL2_E_UNSUPPORTEDSIZE; }
		try {
			_vDst.resize( size_t( ui64BaseSize ) );
		}
		catch ( ... ) { SL2_E_OUTOFMEMORY; }
		return ConvertToFormat( _pkifFormat, _sMip, _sArray, _sFace, _vDst.data(), _bInvertY, _bGenPalette, _pNewPalette );
	}

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
	SL2_ERRORS CImage::ConvertToFormat( const CFormat::SL2_KTX_INTERNAL_FORMAT_DATA * _pkifFormat,
		size_t _sMip, size_t _sArray, size_t _sFace, uint8_t * _pui8Dst, bool _bInvertY, bool _bGenPalette, CPalette * _pNewPalette ) {
		if ( !_pkifFormat || !Format() ) { return SL2_E_BADFORMAT; }
		if ( _sMip >= m_vMipMaps.size() ) { return SL2_E_INVALIDCALL; }

		uint64_t ui64BaseSize = CFormat::GetFormatSize( CFormat::FindFormatDataByVulkan( SL2_VK_FORMAT_R64G64B64A64_SFLOAT ), m_vMipMaps[_sMip]->Width(), m_vMipMaps[_sMip]->Height(), m_vMipMaps[_sMip]->Depth() );
		if ( uint64_t( size_t( ui64BaseSize ) ) != ui64BaseSize ) { return SL2_E_UNSUPPORTEDSIZE; }
		//if ( ParametersAreUnchanged( _pkifFormat, _bInvertY, m_vMipMaps[_sMip]->Width(), m_vMipMaps[_sMip]->Height(), m_vMipMaps[_sMip]->Depth() ) ) {
		//	// No format conversion needed.  Just copy the buffers.
		//	ui64BaseSize = CFormat::GetFormatSize( _pkifFormat, m_vMipMaps[_sMip]->Width(), m_vMipMaps[_sMip]->Height(), m_vMipMaps[_sMip]->Depth() );
		//	std::memcpy( _pui8Dst, Data( _sMip, 0, _sArray, _sFace ), size_t( ui64BaseSize ) );
		//	return SL2_E_SUCCESS;
		//}

		ui64BaseSize = GetActualPlaneSize( ui64BaseSize );
		if ( !ui64BaseSize || (uint64_t( size_t( ui64BaseSize ) ) != ui64BaseSize) ) { return SL2_E_BADFORMAT; }
		std::vector<uint8_t> vTmp;
		try {
			vTmp.resize( size_t( ui64BaseSize ) );
		}
		catch ( ... ) {
			return SL2_E_OUTOFMEMORY;
		}

		CFormat::SL2_KTX_INTERNAL_FORMAT_DATA ifdData = (*Format());
		ifdData.pvCustom = this;
		if ( !Format()->pfToRgba64F( Data( _sMip, 0, _sArray, _sFace ), vTmp.data(), m_vMipMaps[_sMip]->Width(), m_vMipMaps[_sMip]->Height(), m_vMipMaps[_sMip]->Depth(), &ifdData ) ) {
			return SL2_E_INTERNALERROR;
		}
		
		/*BakeGamma( vTmp.data(), m_dGamma, m_vMipMaps[_sMip]->Width(), m_vMipMaps[_sMip]->Height(), m_vMipMaps[_sMip]->Depth(), CFormat::TransferFunc( m_cgcInputCurve ) );
		if ( m_bApplyInputColorSpaceTransfer ) {
			ApplySrcColorSpace( vTmp.data(), m_vMipMaps[_sMip]->Width(), m_vMipMaps[_sMip]->Height(), m_vMipMaps[_sMip]->Depth() );
		}
		if ( !m_bIsPreMultiplied && m_bNeedsPreMultiply ) {
			CFormat::ApplyPreMultiply( vTmp.data(), m_vMipMaps[_sMip]->Width(), m_vMipMaps[_sMip]->Height(), m_vMipMaps[_sMip]->Depth() );
		}

		if ( m_bFlipX && m_vMipMaps[_sMip]->Width() > 1 ) {
			CFormat::FlipX( vTmp.data(), m_vMipMaps[_sMip]->Width(), m_vMipMaps[_sMip]->Height(), m_vMipMaps[_sMip]->Depth() );
		}
		if ( _bInvertY != m_bFlipY && m_vMipMaps[_sMip]->Height() > 1 ) {
			CFormat::FlipY( vTmp.data(), m_vMipMaps[_sMip]->Width(), m_vMipMaps[_sMip]->Height(), m_vMipMaps[_sMip]->Depth() );
		}
		if ( m_bFlipZ && m_vMipMaps[_sMip]->Depth() > 1 ) {
			CFormat::FlipZ( vTmp.data(), m_vMipMaps[_sMip]->Width(), m_vMipMaps[_sMip]->Height(), m_vMipMaps[_sMip]->Depth() );
		}
		if ( m_bSwap ) {
			CFormat::Swap( vTmp.data(), m_vMipMaps[_sMip]->Width(), m_vMipMaps[_sMip]->Height(), m_vMipMaps[_sMip]->Depth() );
		}
		if ( !CFormat::SwizzleIsDefault( m_sSwizzle ) ) {
			CFormat::ApplySwizzle( vTmp.data(), m_vMipMaps[_sMip]->Width(), m_vMipMaps[_sMip]->Height(), m_vMipMaps[_sMip]->Depth(), m_sSwizzle );
		}


		if ( m_dTargetGamma ) {
			BakeGamma( vTmp.data(), 1.0 / m_dTargetGamma, m_vMipMaps[_sMip]->Width(), m_vMipMaps[_sMip]->Height(), m_vMipMaps[_sMip]->Depth(), CFormat::TransferFunc( m_cgcOutputCurve ) );
		}
		if ( m_bApplyInputColorSpaceTransfer ) {
			ApplyDstColorSpace( vTmp.data(), m_vMipMaps[_sMip]->Width(), m_vMipMaps[_sMip]->Height(), m_vMipMaps[_sMip]->Depth() );
		}*/

		if ( _bInvertY ) {
			CFormat::FlipY( vTmp.data(), m_vMipMaps[_sMip]->Width(), m_vMipMaps[_sMip]->Height(), m_vMipMaps[_sMip]->Depth() );
		}

		// Back up our palette.
		auto pTmp = m_pPalette;
		if ( SL2_GET_IDX_FLAG( _pkifFormat->ui32Flags ) ) {
			// Converting to an indexed format?  May need to generate a palette.
			size_t sMax = size_t( 1ULL << _pkifFormat->ui32BlockSizeInBits );
			if ( m_bGenPalette || Palette().Palette().size() == 0 || Palette().Palette().size() > sMax || _bGenPalette ) {
				uint64_t ui64Total = ui64BaseSize / sizeof( CFormat::SL2_RGBA64F );
				if ( ui64Total ) {
					if ( !GeneratePalette( vTmp.data(), ui64Total, uint32_t( sMax ) ) ) { return SL2_E_OUTOFMEMORY; }
				}
			}
		}

		if ( _pNewPalette ) {
			(*_pNewPalette) = m_pPalette.Palette();
		}

		ifdData = (*_pkifFormat);
		ifdData.pvCustom = this;
		if ( !_pkifFormat->pfFromRgba64F( vTmp.data(), _pui8Dst, m_vMipMaps[_sMip]->Width(), m_vMipMaps[_sMip]->Height(), m_vMipMaps[_sMip]->Depth(), &ifdData ) ) {
			m_pPalette = pTmp;
			return SL2_E_INTERNALERROR;
		}
		m_pPalette = pTmp;
		return SL2_E_SUCCESS;
	}

	/**
	 * Sets normal-map parameters.
	 * 
	 * \param _kKernel The kernel to use.
	 * \param _dScale The normal-map scale.
	 * \param _caNormalChannel The channel to use.
	 * \param _dY The Y axis multiplier.
	 * \return Returns true if the kernel copied successfully.  False indicates a memory failure.
	 **/
	bool CImage::SetNormalMapParms( const CKernel &_kKernel, double _dScale, SL2_CHANNEL_ACCESS _caNormalChannel, double _dY ) {
		m_kKernel = _kKernel;
		if ( m_kKernel.Size() != _kKernel.Size() ) { return false; }
		m_dKernelScale = _dScale;
		m_caKernelChannal = _caNormalChannel;
		m_dKernelYAxis = _dY;

		return true;
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
	bool CImage::ToCmyk( uint32_t _ui32Slice, size_t _sMip, size_t _sArray, size_t _sFace, std::vector<uint8_t> &_vResult ) {
		if ( _sMip >= Mipmaps() ) { return false; }
		if ( _ui32Slice >= Depth() ) { return false; }
		if ( _sArray >= ArraySize() ) { return false; }
		if ( _sFace >= Faces() ) { return false; }
		uint32_t ui32W = GetMipmaps()[_sMip]->Width();
		uint32_t ui32H = GetMipmaps()[_sMip]->Height();
		uint64_t ui64Size = uint64_t( ui32W ) * ui32H;
		if ( ui64Size != cmsUInt32Number( ui64Size ) ) { return false; }

		//std::vector<CFormat::SL2_RGBA64F> vLinearBuffer;
		try {
			//vLinearBuffer.resize( ui64Size );
			_vResult.resize( ui64Size * sizeof( CFormat::SL2_RGBA64F ) );
		}
		catch ( ... ) { return false; }

		if ( !Format() || !Format()->pfToRgba64F ) { return false; }

		CFormat::SL2_KTX_INTERNAL_FORMAT_DATA ifdData = (*Format());
		ifdData.pvCustom = this;
		if ( !Format()->pfToRgba64F( Data( _sMip, _ui32Slice, _sArray, _sFace ), reinterpret_cast<uint8_t *>(_vResult.data()), ui32W, ui32H, 1, &ifdData ) ) { return false; }

		if ( m_dTargetGamma ) {
			BakeGamma( reinterpret_cast<uint8_t *>(_vResult.data()), 1.0 / m_dTargetGamma, ui32W, ui32H, 1, CFormat::TransferFunc( m_cgcOutputCurve ) );
		}



		CIcc::SL2_CMS_PROFILE pSrc, pDst;
		std::vector<uint8_t> vPro;
		std::vector<uint8_t> * pvUseMe = &m_vOutIccProfile;
		if ( m_vOutIccProfile.size() == 0 ) {
			// If there is an output colorspace, we need to provide a starting point for it.
			CIcc::SL2_CMS_PROFILE cpTmp;
			if ( !CIcc::CreateProfile( NULL, m_cgcInputCurve, cpTmp, false ) ) { return false; }
			if ( !CIcc::SaveProfileToMemory( cpTmp, vPro ) ) { return false; }
			pvUseMe = &vPro;
		}

		if ( pvUseMe->size() != static_cast<size_t>(static_cast<cmsUInt32Number>(pvUseMe->size())) || static_cast<cmsUInt32Number>(pvUseMe->size()) <= 0 ) { return false; }
		if ( pSrc.Set( ::cmsOpenProfileFromMem( pvUseMe->data(), static_cast<cmsUInt32Number>(pvUseMe->size()) ) ).hProfile == NULL ) { return false; }

		if ( !CIcc::CreateCmykProfile( pDst ) ) { return false; }

		CIcc::SL2_CMS_TRANSFORM tTransform( ::cmsCreateTransform( pSrc.hProfile, TYPE_RGBA_DBL, pDst.hProfile, TYPE_CMYK_DBL, INTENT_PERCEPTUAL/*m_i32OutRenderingIntent*/, 0 ) );
		if ( tTransform.hTransform == NULL ) { return false; }

		::cmsDoTransform( tTransform.hTransform, _vResult.data(), _vResult.data(), cmsUInt32Number( ui64Size ) );

		return true;
	}

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
	double CImage::ApplyKernel( double * _pdImage, uint32_t _ui32X, uint32_t _ui32Y, uint32_t _ui32W, uint32_t _ui32H, uint32_t _ui32D, const CKernel &_kKernel,
		SL2_TEXTURE_ADDRESSING _taAddressW, SL2_TEXTURE_ADDRESSING _taAddressH, double _dBorder ) {
		size_t sPageSize = _ui32W * _ui32H;

		const uint32_t ui32Size = _kKernel.Size();
		const int32_t i32KernelOffset = static_cast<int32_t>(ui32Size >> 1);
		double dSum = 0.0;
		double dDivisor = 0.0;
		for ( uint32_t Y = 0; Y < ui32Size; ++Y ) {
			int32_t i32IdxH = CTextureAddressing::m_pfFuncs[_taAddressH]( _ui32H, static_cast<int32_t>(Y + _ui32Y) - i32KernelOffset );
			for ( uint32_t X = 0; X < ui32Size; ++X ) {
				int32_t i32IdxW = CTextureAddressing::m_pfFuncs[_taAddressW]( _ui32W, static_cast<int32_t>(X + _ui32X) - i32KernelOffset );
				if ( i32IdxH == -1 || i32IdxW == -1 ) {
					dSum += _kKernel[Y][X] * _dBorder;
					++dDivisor;
				}
				else if ( i32IdxH != -2 && i32IdxW != -2 ) {
					size_t sIdx = (sPageSize * _ui32D) + (i32IdxH * _ui32W) + i32IdxW;
					dSum += _kKernel[Y][X] * _pdImage[sIdx];
					++dDivisor;
				}
			}
		}
		return dSum / dDivisor;
	}

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
	bool CImage::AllocateTexture( const CFormat::SL2_KTX_INTERNAL_FORMAT_DATA * _pkifFormat, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Depth, size_t _sMips, size_t _sArray, size_t _sFaces ) {
		if ( !_pkifFormat ) { return false; }
		if ( !_sMips ) {
			_sMips = CUtilities::Max( size_t( std::floor( std::log2( _ui32Width ) ) ), size_t( std::floor( std::log2( _ui32Height ) ) ) );
			_sMips = CUtilities::Max( size_t( std::floor( std::log2( _ui32Depth ) ) ), _sMips ) + 1;
		}
		if ( !_sMips ) { return false; }
		

		m_sArraySize = _sArray;
		m_sFaces = _sFaces;
		m_pkifFormat = _pkifFormat;
		
		uint64_t ui64BaseSize = CFormat::GetFormatSize( _pkifFormat, _ui32Width, _ui32Height, _ui32Depth );
		if ( !ui64BaseSize || (uint64_t( size_t( ui64BaseSize ) ) != ui64BaseSize) ) { Reset(); return false; }
		try {
			m_vMipMaps.resize( _sMips );
			for ( size_t I = 0; I < _sMips; ++I ) {
				uint64_t ui64ThisBaseSize = GetActualPlaneSize( CFormat::GetFormatSize( _pkifFormat, _ui32Width, _ui32Height, _ui32Depth ) );
				if ( _pkifFormat->vfVulkanFormat == SL2_VK_FORMAT_R64G64B64A64_SFLOAT ) {
					// Special case for RGBA64F.  Keep them tightly packed.
					ui64ThisBaseSize = ui64BaseSize;
				}
				uint64_t ui64FullSize = ui64ThisBaseSize * _sArray * _sFaces;
				if ( !ui64FullSize || (uint64_t( size_t( ui64FullSize ) ) != ui64FullSize) ) { Reset(); return false; }

				if ( m_vMipMaps[I].get() ) {
					if ( !m_vMipMaps[I]->Reallocate( size_t( ui64FullSize ), size_t( ui64ThisBaseSize ), _ui32Width, _ui32Height, _ui32Depth ) ) { Reset(); return false; }
				}
				else {
					m_vMipMaps[I] = std::make_unique<CSurface>( size_t( ui64FullSize ), size_t( ui64ThisBaseSize ), _ui32Width, _ui32Height, _ui32Depth );
				}

				_ui32Width = CUtilities::Max<uint32_t>( _ui32Width >> 1, 1 );
				_ui32Height = CUtilities::Max<uint32_t>( _ui32Height >> 1, 1 );
				_ui32Depth = CUtilities::Max<uint32_t>( _ui32Depth >> 1, 1 );
			}
		}
		catch ( ... ) {
			Reset();
			return false;
		}
		
		return true;
	}

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
	bool CImage::ParametersAreUnchanged( const CFormat::SL2_KTX_INTERNAL_FORMAT_DATA * _pkifFormat, bool _bFlip, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Depth ) {
		if ( (m_dGamma == 0.0 || m_dGamma == 1.0) && (m_dTargetGamma == 0.0 || m_dTargetGamma == 1.0) &&
			(_bFlip == m_bFlipY) &&
			!m_bFlipX && !m_bFlipZ &&
			CFormat::SwizzleIsDefault( m_sSwizzle ) &&
			!m_bSwap &&
			!(!m_bIsPreMultiplied && m_bNeedsPreMultiply) &&
			!m_bIgnoreAlpha &&
			m_kKernel.Size() == 0 &&
			((_pkifFormat->vfVulkanFormat != SL2_VK_FORMAT_UNDEFINED && _pkifFormat->vfVulkanFormat == Format()->vfVulkanFormat) ||
			(_pkifFormat->dfDxFormat != SL2_DXGI_FORMAT_UNKNOWN && _pkifFormat->dfDxFormat == Format()->dfDxFormat) ||
			(_pkifFormat->mfMetalFormat != SL2_MTLPixelFormatInvalid && _pkifFormat->mfMetalFormat == Format()->mfMetalFormat) ||
			((_pkifFormat->kifInternalFormat != SL2_GL_INVALID && _pkifFormat->kifInternalFormat == Format()->kifInternalFormat &&
				_pkifFormat->kbifBaseInternalFormat != SL2_KBIF_GL_INVALID && _pkifFormat->kbifBaseInternalFormat == Format()->kbifBaseInternalFormat &&
				_pkifFormat->ktType != SL2_KT_GL_INVALID && _pkifFormat->ktType == Format()->ktType))) ) {
			if ( (m_rResample.ui32NewW == 0 || m_rResample.ui32NewW == _ui32Width) &&
				(m_rResample.ui32NewH == 0 || m_rResample.ui32NewH == _ui32Height) &&
				(m_rResample.ui32NewD == 0 || m_rResample.ui32NewD == _ui32Depth) ) { return true; }
		}
		return false;
	}

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
	void CImage::Crop( uint8_t * &_pui8Src, std::vector<uint8_t> &_vDst,
		uint32_t &_ui32Width, uint32_t &_ui32Height, uint32_t &_ui32Depth,
		SL2_TEXTURE_ADDRESSING _taX, SL2_TEXTURE_ADDRESSING _taY, SL2_TEXTURE_ADDRESSING _taZ,
		const SL2_WINDOW &_wWindow, const double * _pdBorderColor ) {
		if ( CropIsFullSize( _ui32Width, _ui32Height, _ui32Depth, _wWindow ) ) { return; }

		_vDst.resize( _wWindow.ui32D * _wWindow.ui32H * _wWindow.ui32W * sizeof( CFormat::SL2_RGBA64F ) );
		const CFormat::SL2_RGBA64F * prgbSrc = reinterpret_cast<const CFormat::SL2_RGBA64F *>(_pui8Src);
		CFormat::SL2_RGBA64F * prgbDst = reinterpret_cast<CFormat::SL2_RGBA64F *>(_vDst.data());

		size_t sSrcPageSize = _ui32Height * _ui32Width;
		size_t sDstPageSize = _wWindow.ui32H * _wWindow.ui32W;
		for ( int64_t D = 0; D < _wWindow.ui32D; ++D ) {
			for ( int64_t H = 0; H < _wWindow.ui32H; ++H ) {
				for ( int64_t W = 0; W < _wWindow.ui32W; ++W ) {
					size_t sDstIdx = size_t( (sDstPageSize * D) + (H * _wWindow.ui32W) + W );
					int32_t i32IdxW = CTextureAddressing::m_pfFuncs[_taX]( _ui32Width, static_cast<int32_t>(W + _wWindow.i32X) );
					int32_t i32IdxH = CTextureAddressing::m_pfFuncs[_taY]( _ui32Height, static_cast<int32_t>(H + _wWindow.i32Y) );
					int32_t i32IdxD = CTextureAddressing::m_pfFuncs[_taZ]( _ui32Depth, static_cast<int32_t>(D + _wWindow.i32Z) );
					// -1 = Border Color.
					// -2 = Invalid.  We can't treat anything as invalid here, so to make it distinct from Boder Color it will be black with full alpha.
					if ( i32IdxW == -2 || i32IdxH == -2 || i32IdxD == -2 ) {
						prgbDst[sDstIdx].dRgba[SL2_PC_R] = 0.0;
						prgbDst[sDstIdx].dRgba[SL2_PC_G] = 0.0;
						prgbDst[sDstIdx].dRgba[SL2_PC_B] = 0.0;
						prgbDst[sDstIdx].dRgba[SL2_PC_A] = 1.0;
					}
					else if ( i32IdxW == -1 || i32IdxH == -1 || i32IdxD == -1 ) {
						prgbDst[sDstIdx].dRgba[SL2_PC_R] = _pdBorderColor[SL2_PC_R];
						prgbDst[sDstIdx].dRgba[SL2_PC_G] = _pdBorderColor[SL2_PC_G];
						prgbDst[sDstIdx].dRgba[SL2_PC_B] = _pdBorderColor[SL2_PC_B];
						prgbDst[sDstIdx].dRgba[SL2_PC_A] = _pdBorderColor[SL2_PC_A];
					}
					else {
						size_t sSrcIdx = size_t( (sSrcPageSize * i32IdxD) + (i32IdxH * _ui32Width) + i32IdxW );
						prgbDst[sDstIdx] = prgbSrc[sSrcIdx];
					}
				}
			}
		}
		_ui32Width = _wWindow.ui32W;
		_ui32Height = _wWindow.ui32H;
		_ui32Depth = _wWindow.ui32D;
		_pui8Src = _vDst.data();
	}

	/**
	 * Determines if the given window contains exactly the full image.
	 * 
	 * \param _ui32Width The image width.
	 * \param _ui32Height The image height.
	 * \param _ui32Depth The image depth.
	 * \param _wWindow The cropping window to test
	 * \return Returns true if the cropping window exactly matches the full image.
	 **/
	bool CImage::CropIsFullSize( uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Depth, const SL2_WINDOW &_wWindow ) {
		if ( _wWindow.ui32W == 0 || (_wWindow.ui32W == _ui32Width && _wWindow.i32X == 0) ) {
			if ( _wWindow.ui32H == 0 || (_wWindow.ui32H == _ui32Height && _wWindow.i32Y == 0) ) {
				if ( _wWindow.ui32D == 0 || (_wWindow.ui32D == _ui32Depth && _wWindow.i32Z == 0) ) {
					// Full crop.
					return true;
				}
			}
		}
		return false;
	}

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
	void CImage::BakeGamma( uint8_t * _pui8Buffer, double _dGamma, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Depth, CFormat::SL2_TRANSFER_FUNCS _ptfGamma ) {
		if ( !_pui8Buffer ) { return; }
		if ( _dGamma == 0.0 || _dGamma == 1.0 ) { return; }
		CFormat::SL2_RGBA64F * prDst = reinterpret_cast<CFormat::SL2_RGBA64F *>(_pui8Buffer);
		if ( _dGamma <= -1.0 ) {
			// True Linear -> sRGB conversion.
			for ( uint32_t D = 0; D < _ui32Depth; ++D ) {
				uint32_t ui32Slice = _ui32Width * _ui32Height * D;
				for ( uint32_t H = 0; H < _ui32Height; ++H ) {
					uint32_t ui32Row = _ui32Width * H;
					for ( uint32_t W = 0; W < _ui32Width; ++W ) {
						CFormat::SL2_RGBA64F * prThis = &prDst[ui32Slice+ui32Row+W];
						prThis->dRgba[SL2_PC_R] = _ptfGamma.pfLinearToX( prThis->dRgba[SL2_PC_R] );
						prThis->dRgba[SL2_PC_G] = _ptfGamma.pfLinearToX( prThis->dRgba[SL2_PC_G] );
						prThis->dRgba[SL2_PC_B] = _ptfGamma.pfLinearToX( prThis->dRgba[SL2_PC_B] );
					}
				}
			}
		}
		else if ( _dGamma < 0.0 ) {
			// True sRGB -> Linear conversion.
			for ( uint32_t D = 0; D < _ui32Depth; ++D ) {
				uint32_t ui32Slice = _ui32Width * _ui32Height * D;
				for ( uint32_t H = 0; H < _ui32Height; ++H ) {
					uint32_t ui32Row = _ui32Width * H;
					for ( uint32_t W = 0; W < _ui32Width; ++W ) {
						CFormat::SL2_RGBA64F * prThis = &prDst[ui32Slice+ui32Row+W];
						prThis->dRgba[SL2_PC_R] = _ptfGamma.pfXtoLinear( prThis->dRgba[SL2_PC_R] );
						prThis->dRgba[SL2_PC_G] = _ptfGamma.pfXtoLinear( prThis->dRgba[SL2_PC_G] );
						prThis->dRgba[SL2_PC_B] = _ptfGamma.pfXtoLinear( prThis->dRgba[SL2_PC_B] );
					}
				}
			}
		}
		else {
			// Custom gamma curve.
			_dGamma = 1.0 / _dGamma;
			for ( uint32_t D = 0; D < _ui32Depth; ++D ) {
				uint32_t ui32Slice = _ui32Width * _ui32Height * D;
				for ( uint32_t H = 0; H < _ui32Height; ++H ) {
					uint32_t ui32Row = _ui32Width * H;
					for ( uint32_t W = 0; W < _ui32Width; ++W ) {
						CFormat::SL2_RGBA64F * prThis = &prDst[ui32Slice+ui32Row+W];
						prThis->dRgba[SL2_PC_R] = std::pow( prThis->dRgba[SL2_PC_R], _dGamma );
						prThis->dRgba[SL2_PC_G] = std::pow( prThis->dRgba[SL2_PC_G], _dGamma );
						prThis->dRgba[SL2_PC_B] = std::pow( prThis->dRgba[SL2_PC_B], _dGamma );
					}
				}
			}
		}
	}

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
	void CImage::ApplyColorSpaceTransferFunction( uint8_t * _pui8Buffer, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Depth,
		CIcc::PfTransferFunc _pfGammaFuncR, const void * _pvGammaFuncParmR,
		CIcc::PfTransferFunc _pfGammaFuncG, const void * _pvGammaFuncParmG,
		CIcc::PfTransferFunc _pfGammaFuncB, const void * _pvGammaFuncParmB ) {
		if ( !_pui8Buffer ) { return; }
		CFormat::SL2_RGBA64F * prDst = reinterpret_cast<CFormat::SL2_RGBA64F *>(_pui8Buffer);
		for ( uint32_t D = 0; D < _ui32Depth; ++D ) {
			uint32_t ui32Slice = _ui32Width * _ui32Height * D;
			for ( uint32_t H = 0; H < _ui32Height; ++H ) {
				uint32_t ui32Row = _ui32Width * H;
				for ( uint32_t W = 0; W < _ui32Width; ++W ) {
					CFormat::SL2_RGBA64F * prThis = &prDst[ui32Slice+ui32Row+W];
					prThis->dRgba[SL2_PC_R] = _pfGammaFuncR( prThis->dRgba[SL2_PC_R], _pvGammaFuncParmR );
					prThis->dRgba[SL2_PC_G] = _pfGammaFuncG( prThis->dRgba[SL2_PC_G], _pvGammaFuncParmG );
					prThis->dRgba[SL2_PC_B] = _pfGammaFuncB( prThis->dRgba[SL2_PC_B], _pvGammaFuncParmB );
				}
			}
		}
	}

	/**
	 * Applies the source colorspace profile.  The source color profile is whatever came with the loaded image file.  The destination is a linear color space based on m_cgcOutputCurve
	 * 
	 * \param _pui8Buffer The texture texels.
	 * \param _ui32Width The width of the image.
	 * \param _ui32Height The height of the image.
	 * \param _ui32Depth The depth of the image.
	 * \return Returns true if the profile was applied.
	 **/
	bool CImage::ApplySrcColorSpace( uint8_t * _pui8Buffer, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Depth ) {
		if ( !_pui8Buffer ) { return false; }
		if ( m_cgcInputCurve == SL2_CGC_NONE && m_vIccProfile.size() == 0 && m_vOutIccProfile.size() ) {
			// If there is an output colorspace, we need to provide a starting point for it.
			CIcc::SL2_CMS_PROFILE cpTmp;
			if ( !CIcc::CreateProfile( NULL, SL2_CGC_sRGB_PRECISE, cpTmp, false ) ) { return false; }
			if ( !CIcc::SaveProfileToMemory( cpTmp, m_vIccProfile ) ) { return false; }
			/*if ( !m_bManuallySetGamma ) {
				m_dGamma = 0.0;
			}*/
		}
		if ( m_cgcInputCurve == SL2_CGC_NONE && m_vIccProfile.size() == 0 ) { return false; }		// No user selection and no embedded profile.

		CIcc::SL2_CMS_PROFILE pSrc, pDst;
		if ( m_vIccProfile.size() != 0 ) {
			if ( m_vIccProfile.size() != static_cast<size_t>(static_cast<cmsUInt32Number>(m_vIccProfile.size())) || static_cast<cmsUInt32Number>(m_vIccProfile.size()) <= 0 ) { return false; }
			if ( m_bIgnoreSourceColorspaceGamma ) {
				if ( !CIcc::CreateLinearProfile( m_vIccProfile, pSrc ) ) { return false; }
			}
			else {
				if ( pSrc.Set( ::cmsOpenProfileFromMem( m_vIccProfile.data(), static_cast<cmsUInt32Number>(m_vIccProfile.size()) ) ).hProfile == NULL ) { return false; }
			}
		}
		else if ( m_cgcInputCurve != SL2_CGC_NONE ) {
			// User selection overrides embedded profile.
			if ( !CIcc::CreateProfile( NULL, m_cgcInputCurve, pSrc, true ) ) {
				// That's weird.  This can't happen but anyway we can still fall back to the embedded profile.
				if ( m_vIccProfile.size() ) {
					if ( m_vIccProfile.size() != static_cast<size_t>(static_cast<cmsUInt32Number>(m_vIccProfile.size())) || static_cast<cmsUInt32Number>(m_vIccProfile.size()) <= 0 ) { return false; }
					if ( pSrc.Set( ::cmsOpenProfileFromMem( m_vIccProfile.data(), static_cast<cmsUInt32Number>(m_vIccProfile.size()) ) ).hProfile == NULL ) { return false; }
				}
			}
		}
		else {
			return false;
		}
		if ( !m_vOutIccProfile.size() ) {
			CIcc::SL2_CMS_PROFILE cpTmp;
			if ( !CIcc::CreateProfile( NULL, SL2_CGC_sRGB_PRECISE, cpTmp, false ) ) { return false; }
			if ( !CIcc::SaveProfileToMemory( cpTmp, m_vOutIccProfile ) ) { return false; }
			/*if ( !m_bManuallySetTargetGamma ) {
				m_dTargetGamma = 0.0;
			}*/
		}
		if ( !CIcc::CreateLinearProfile( m_vOutIccProfile, pDst ) ) { return false; }

		CIcc::SL2_CMS_TRANSFORM tTransform( ::cmsCreateTransform( pSrc.hProfile, TYPE_RGBA_DBL, pDst.hProfile, TYPE_RGBA_DBL, m_i32InRenderingIntent, 0 ) );
		if ( tTransform.hTransform == NULL ) { return false; }
		
		::cmsDoTransform( tTransform.hTransform, _pui8Buffer, _pui8Buffer, _ui32Width * _ui32Height * _ui32Depth );

		return true;
	}

	/**
	 * Applies the destination colorspace profile.  It actually only applies the gamma curve from the output ICC profile.
	 * 
	 * \param _pui8Buffer The texture texels.
	 * \param _ui32Width The width of the image.
	 * \param _ui32Height The height of the image.
	 * \param _ui32Depth The depth of the image.
	 * \return Returns true if the profile was applied.
	 **/
	bool CImage::ApplyDstColorSpace( uint8_t * _pui8Buffer, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Depth ) {
		if ( !_pui8Buffer || m_vOutIccProfile.size() == 0 ) { return false; }
		CIcc::SL2_CMS_PROFILE pSrc, pDst;

		if ( !CIcc::CreateLinearProfile( m_vOutIccProfile, pSrc ) ) { return false; }

		if ( m_vOutIccProfile.size() != static_cast<size_t>(static_cast<cmsUInt32Number>(m_vOutIccProfile.size())) || static_cast<cmsUInt32Number>(m_vOutIccProfile.size()) <= 0 ) { return false; }
		if ( pDst.Set( ::cmsOpenProfileFromMem( m_vOutIccProfile.data(), static_cast<cmsUInt32Number>(m_vOutIccProfile.size()) ) ).hProfile == NULL ) { return false; }

		CIcc::SL2_CMS_TRANSFORM tTransform( ::cmsCreateTransform( pSrc.hProfile, TYPE_RGBA_DBL, pDst.hProfile, TYPE_RGBA_DBL, INTENT_PERCEPTUAL/*m_i32OutRenderingIntent*/, 0 ) );
		if ( tTransform.hTransform == NULL ) { return false; }
		
		::cmsDoTransform( tTransform.hTransform, _pui8Buffer, _pui8Buffer, _ui32Width * _ui32Height * _ui32Depth );

		return true;
	}

	/**
	 * Generates a palette with the given number of entries.  The palette format is used to determine the color format.
	 * 
	 * \param _pui8Buffer The pointer to the buffer of colors.
	 * \param _ui64Total The total number of RGBA64F colors to which _pui8Buffer points.
	 * \param _ui32PalTotal The total number of entries to generate in the palette.
	 * \return Returns true if all necessary allocations succeed.
	 **/
	bool CImage::GeneratePalette( const uint8_t * _pui8Buffer, uint64_t _ui64Total, uint32_t _ui32PalTotal ) {
		if ( _ui64Total != size_t( _ui64Total ) ) { return false; }
		if ( !Palette().Format() ) {
			bool bAlpha = m_bIgnoreAlpha ? false : AlphaIsFullyEqualTo( _pui8Buffer, 1.0, _ui64Total );
			if ( bAlpha ) {
				Palette().SetFormat( CFormat::FindPaletteFormatData( SL2_GL_PALETTE8_RGBA8_OES ) );
			}
			else {
				Palette().SetFormat( CFormat::FindPaletteFormatData( SL2_GL_PALETTE8_RGB8_OES ) );
			}
		}

		//Palette().SetFormat( CFormat::FindPaletteFormatData( SL2_GL_PALETTE4_RGBA4_OES ) );
		if ( !Palette().Format() || !Palette().Format()->pfFromRgba64F ) { return false; }

		// Create a quantized copy of the image.
		std::vector<CFormat::SL2_RGBA64F, CAlignmentAllocator<CFormat::SL2_RGBA64F, 64>> vQuant;
		try {
			vQuant.resize( size_t( _ui64Total ) );
		}
		catch ( ... ) { return false; }

		CFormat::SL2_KTX_INTERNAL_FORMAT_DATA ifdData = (*Palette().Format());
		ifdData.pvCustom = this;
		size_t sOff = 0;
		while ( sOff < vQuant.size() ) {
			uint64_t ui64Total = vQuant.size() - sOff;
			if ( ui64Total > ~uint32_t( 0 ) ) {
				ui64Total = ~uint32_t( 0 );
			}
			if ( !Palette().Format()->pfFromRgba64F( _pui8Buffer + (sOff * sizeof( CFormat::SL2_RGBA64F )),
				reinterpret_cast<uint8_t *>(vQuant.data() + sOff),
				uint32_t( ui64Total ), 1, 1, &ifdData ) ) { return false; }
			sOff += uint32_t( ui64Total );
		}


		return Palette().GenPalette_kMeans( reinterpret_cast<const CPalette::CColor *>(vQuant.data()), vQuant.size(),
			_ui32PalTotal,
			(CFormat::m_skMeansIterations == ~size_t( 0 )) ? _ui32PalTotal : CFormat::m_skMeansIterations );
	}

	/**
	 * Sets alpha to 1.0.
	 *
	 * \param _pui8Buffer The texture texels.
	 * \param _dValue The value to apply.
	 * \param _ui32Width The width of the image.
	 * \param _ui32Height The height of the image.
	 * \param _ui32Depth The depth of the image.
	 **/
	void CImage::SetAlpha( uint8_t * _pui8Buffer, double _dValue, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Depth ) {
		CFormat::SL2_RGBA64F * prDst = reinterpret_cast<CFormat::SL2_RGBA64F *>(_pui8Buffer);
		for ( uint32_t D = 0; D < _ui32Depth; ++D ) {
			uint32_t ui32Slice = _ui32Width * _ui32Height * D;
			for ( uint32_t H = 0; H < _ui32Height; ++H ) {
				uint32_t ui32Row = _ui32Width * H;
				for ( uint32_t W = 0; W < _ui32Width; ++W ) {
					CFormat::SL2_RGBA64F * prThis = &prDst[ui32Slice+ui32Row+W];
					prThis->dRgba[SL2_PC_A] = _dValue;
				}
			}
		}
	}

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
	bool CImage::AlphaIsFullyEqualTo( const uint8_t * _pui8Buffer, double _dValue, uint32_t _ui32Width, uint32_t _ui32Height, uint32_t _ui32Depth ) {
		return AlphaIsFullyEqualTo( _pui8Buffer, _dValue, uint64_t( _ui32Width ) * _ui32Height * _ui32Depth );
	}

	/**
	 * Tests alpha for being entirely of a given value.
	 *
	 * \param _pui8Buffer The texture texels.
	 * \param _dValue The value to check.
	 * \param _ui64Total The total number of texels to which _pui8Buffer points.
	 * \return Returns true if all values in the alpha channel are euqal to _dValue.
	 **/
	bool CImage::AlphaIsFullyEqualTo( const uint8_t * _pui8Buffer, double _dValue, uint64_t _ui64Total ) {
		const CFormat::SL2_RGBA64F * prDst = reinterpret_cast<const CFormat::SL2_RGBA64F *>(_pui8Buffer);
		for ( uint64_t I = 0; I < _ui64Total; ++I ) {
			if ( prDst[I].dRgba[SL2_PC_A] != _dValue ) { return false; }
		}
		return true;
	}

	/**
	 * Converts a given RGBA64F buffer to a normal map.
	 * 
	 * \param _prgbaData The RGBA buffer to convert to a normal map.
	 * \param _ui32W The width of the input texture.
	 * \param _ui32H The height of the input texture.
	 * \param _ui32D The depth slice to convert.
	 * \return Returns true if the temporary buffer was allocated.
	 **/
	bool CImage::ConvertToNormalMap( CFormat::SL2_RGBA64F * _prgbaData, uint32_t _ui32W, uint32_t _ui32H, uint32_t _ui32D ) {
		std::vector<double> vBuffer;
		try {
			vBuffer.resize( _ui32W * _ui32H * _ui32D );
		}
		catch ( ... ) { return false; }

		size_t sPageSize = size_t( _ui32W ) * size_t( _ui32H );
		double dBorder = 0.0;
		for ( uint32_t D = 0; D < _ui32D; ++D ) {
			for ( uint32_t H = 0; H < _ui32H; ++H ) {
				for ( uint32_t W = 0; W < _ui32W; ++W ) {
					size_t sIdx = (sPageSize * D) + (size_t( H ) * size_t( _ui32W )) + size_t( W );
					if ( m_caKernelChannal == SL2_CA_AVERAGE ) {
						vBuffer[sIdx] = (_prgbaData[sIdx].dRgba[SL2_CA_R] + _prgbaData[sIdx].dRgba[SL2_CA_G] + _prgbaData[sIdx].dRgba[SL2_CA_B]) / 3.0;
						dBorder = (m_rResample.dBorderColor[0] + m_rResample.dBorderColor[1] + m_rResample.dBorderColor[2]) / 3.0;
					}
					else if ( m_caKernelChannal == SL2_CA_WEIGHTED_AVERAGE ) {
						vBuffer[sIdx] = _prgbaData[sIdx].dRgba[SL2_CA_R] * CFormat::Luma().dRgb[0] + _prgbaData[sIdx].dRgba[SL2_CA_G] * CFormat::Luma().dRgb[1] + _prgbaData[sIdx].dRgba[SL2_CA_B] * CFormat::Luma().dRgb[2];
						dBorder = m_rResample.dBorderColor[SL2_CA_R] * CFormat::Luma().dRgb[0] + m_rResample.dBorderColor[SL2_CA_G] * CFormat::Luma().dRgb[1] + m_rResample.dBorderColor[SL2_CA_B] * CFormat::Luma().dRgb[2];
					}
					else if ( m_caKernelChannal == SL2_CA_MAX ) {
						CVector4<SL2_ST_AVX512> vVec( _prgbaData[sIdx].dRgba );
						vBuffer[sIdx] = vVec.Max();
						CVector4<SL2_ST_AVX512> vVecBorder( m_rResample.dBorderColor );
						dBorder = vVecBorder.Max();
					}
					else {
						vBuffer[sIdx] = _prgbaData[sIdx].dRgba[m_caKernelChannal];
						dBorder = m_rResample.dBorderColor[m_caKernelChannal];
					}
				}
			}
		}
		CKernel kTransp;
		kTransp = m_kKernel;
		kTransp.Transpose();
		for ( uint32_t D = 0; D < _ui32D; ++D ) {
			for ( uint32_t H = 0; H < _ui32H; ++H ) {
				for ( uint32_t W = 0; W < _ui32W; ++W ) {
					size_t sIdx = (sPageSize * D) + (size_t( H ) * size_t( _ui32W )) + size_t( W );
					double dVal0 = ApplyKernel( vBuffer.data(), W, H, _ui32W, _ui32H, D, m_kKernel, m_rResample.taColorW, m_rResample.taColorH, dBorder );
					double dVal1 = ApplyKernel( vBuffer.data(), W, H, _ui32W, _ui32H, D, kTransp, m_rResample.taColorW, m_rResample.taColorH, dBorder );
					CVector4<SL2_ST_AVX512> vTmp( dVal0, dVal1, m_dKernelScale, 0.0 );
					vTmp.Normalize();
					_prgbaData[sIdx].dRgba[SL2_CA_R] = -vTmp[0] * 0.5 + 0.5;
					_prgbaData[sIdx].dRgba[SL2_CA_G] = (vTmp[1] * m_dKernelYAxis) * 0.5 + 0.5;
					_prgbaData[sIdx].dRgba[SL2_CA_B] = vTmp[2] * 0.5 + 0.5;
					_prgbaData[sIdx].dRgba[SL2_CA_A] = 1.0;
				}
			}
		}


		return true;
	}

	/**
	 * Loads using the FreeImage library.
	 * 
	 * \param _vData The file to load.
	 * \return Returns an error code.
	 **/
	SL2_ERRORS CImage::LoadFreeImage( const std::vector<uint8_t> &_vData ) {
		SL2_FREE_IMAGE fiImage( _vData );
		if ( !fiImage.pmMemory ) { return SL2_E_OUTOFMEMORY; }

		FREE_IMAGE_FORMAT fifFormat = ::FreeImage_GetFileTypeFromMemory( fiImage.pmMemory, 0 );
		if ( FIF_UNKNOWN == fifFormat ) { return SL2_E_INVALIDFILETYPE; }

		if ( fifFormat == FIF_GIF ) {
			SL2_FREEIMAGE_LOAD_MULTI_BIPMAP_FROM_MEMORY flmbfmData( fiImage );
			int iFrameCount = ::FreeImage_GetPageCount( flmbfmData.pbBitmap );

			SL2_FREEIMAGE_CLONE fcBase;
			for ( int I = 0; I < iFrameCount; ++I ) {
				SL2_FREEIMAGE_LOCK_PAGE flpLocked( flmbfmData.pbBitmap, I );
				if ( !flpLocked.pbBitmap ) { continue; }

				// Retrieve frame metadata.
				FITAG * pTag = nullptr;
				long lFrameTime = 0;
				uint16_t iFrameLeft = 0, iFrameTop = 0;
				uint8_t iDisposalMethod = 0;

				::FreeImage_GetMetadata( FIMD_ANIMATION, flpLocked.pbBitmap, "FrameLeft", &pTag );
				if ( pTag ) { iFrameLeft = (*reinterpret_cast<const uint16_t *>(::FreeImage_GetTagValue( pTag ))); }

				pTag = nullptr;
				::FreeImage_GetMetadata( FIMD_ANIMATION, flpLocked.pbBitmap, "FrameTop", &pTag );
				if ( pTag ) { iFrameTop = (*reinterpret_cast<const uint16_t *>(::FreeImage_GetTagValue( pTag ))); }

				pTag = nullptr;
				::FreeImage_GetMetadata( FIMD_ANIMATION, flpLocked.pbBitmap, "DisposalMethod", &pTag );
				if ( pTag ) { iDisposalMethod = (*reinterpret_cast<const uint8_t *>(::FreeImage_GetTagValue( pTag ))); }

				pTag = nullptr;
				::FreeImage_GetMetadata( FIMD_ANIMATION, flpLocked.pbBitmap, "FrameTime", &pTag );
				if ( pTag ) { lFrameTime = (*reinterpret_cast<const long *>(::FreeImage_GetTagValue( pTag ))); }
				
				// Initialize or restore the base canvas.
				if ( I == 0 || !fcBase.Bitmap() ) { fcBase = flpLocked.pbBitmap; }

				// Convert to RGBA32.
				SL2_FREEIMAGE_FIBITMAP fbTmpBase;
				SL2_FREEIMAGE_FIBITMAP fbTmpLocked;
				try {
					fbTmpBase = ConvertToRGBA32( fcBase.Bitmap() );
					fbTmpLocked = ConvertToRGBA32( flpLocked.pbBitmap );
				}
				catch ( ... ) { return SL2_E_INTERNALERROR; }

				// Blend.
				AlphaBlend( fbTmpBase.Bitmap(), fbTmpLocked.Bitmap(), iFrameLeft, iFrameTop );

				auto aErr = LoadFreeImagePage( fbTmpBase.Bitmap(), I, iFrameCount );
				if ( aErr != SL2_E_SUCCESS ) { return aErr; }

				// Disposal.
				if ( iDisposalMethod == 0 || iDisposalMethod == 1 ) {			// GIF_DISPOSAL_LEAVE
					// Keeps the current contents for the next image.
					fcBase = fbTmpBase.Bitmap();
				}
				else if ( iDisposalMethod == 2 ) {								// GIF_DISPOSAL_BACKGROUND
					// Clear the current contents for the next image.
					switch ( ::FreeImage_GetBPP( fbTmpBase.Bitmap() ) ) {
						case 1 : {} SL2_FALLTHROUGH
						case 4 : {} SL2_FALLTHROUGH
						case 8 : {
							RGBQUAD rgbaVal;
							rgbaVal.rgbReserved = 0;
							::FreeImage_FillBackground( fcBase.Bitmap(), &rgbaVal, FI_COLOR_ALPHA_IS_INDEX );
						}
						default : {
							RGBQUAD rgbaVal;
							rgbaVal.rgbRed = rgbaVal.rgbBlue = rgbaVal.rgbGreen = rgbaVal.rgbReserved = 0xFF;
							::FreeImage_FillBackground( fcBase.Bitmap(), &rgbaVal, FI_COLOR_IS_RGBA_COLOR );
						}
					}
				}
				else if ( iDisposalMethod == 3 ) {								// GIF_DISPOSAL_PREVIOUS
					// fcBase already contains the previous contents.
				}
			}
			return SL2_E_SUCCESS;
		}
		else {
			SL2_FREEIMAGE_LOAD_FROM_MEMORY flfmData( fiImage );
			return LoadFreeImagePage( flfmData.pbBitmap );
		}
	}

	/**
	 * Loads a page given a FreeImage FIBITMAP pointer and the page index.
	 * 
	 * \param _pbBitmap The page to load.
	 * \param _sIdx The index of the image to load (image array).
	 * \param _sTotalIdx The total number of images to load.
	 * \return Returns an error code.
	 **/
	SL2_ERRORS CImage::LoadFreeImagePage( FIBITMAP * _pbBitmap, size_t _sIdx, size_t _sTotalIdx ) {
		int iTransIndex = ::FreeImage_GetTransparentIndex( _pbBitmap );

		uint32_t ui32Width = ::FreeImage_GetWidth( _pbBitmap );
		uint32_t ui32Height = ::FreeImage_GetHeight( _pbBitmap );
		constexpr uint32_t ui32Depth = 1;

#define FreeImage_GetScanLine( DATA, Y )	FreeImage_GetScanLine( DATA, ui32Height - Y - 1 )

		FREE_IMAGE_TYPE fitType = ::FreeImage_GetImageType( _pbBitmap );

		if ( _sIdx == 0 ) {
			FIICCPROFILE * pProfile = _pbBitmap ? ::FreeImage_GetICCProfile( _pbBitmap ) : nullptr;
			if ( pProfile && (pProfile->flags & FIICC_COLOR_IS_CMYK) == FIICC_COLOR_IS_CMYK ) {
				return SL2_E_BADFORMAT;
			}
			if ( pProfile && pProfile->size ) {
				try {
					m_vIccProfile.resize( pProfile->size );
					std::memcpy( m_vIccProfile.data(), pProfile->data, pProfile->size );
				}
				catch ( ... ) { return SL2_E_OUTOFMEMORY; }
				m_dGamma = 0.0;
				m_dTargetGamma = 0.0;
				size_t sSize;
				size_t sOffset = CIcc::GetTagDataOffset( static_cast<uint8_t *>(pProfile->data), pProfile->size, icSigRedTRCTag, sSize );
				if ( sOffset ) {
					uint8_t * pui8Data = static_cast<uint8_t *>(pProfile->data) + sOffset;
					if ( CIcc::FillOutTransferFunc( m_tfInColorSpaceTransferFunc[SL2_PC_R], pui8Data, sSize ) ) {
						m_dGamma = 0.0;
					}
				}
				/*sOffset = CIcc::GetTagDataOffset( static_cast<uint8_t *>(pProfile->data), pProfile->size, icSigGreenTRCTag, sSize );
				if ( sOffset ) {
					uint8_t * pui8Data = static_cast<uint8_t *>(pProfile->data) + sOffset;
					if ( CIcc::FillOutTransferFunc( m_tfInColorSpaceTransferFunc[SL2_PC_G], pui8Data, sSize ) ) {
						m_dGamma = 0.0;
					}
				}
				sOffset = CIcc::GetTagDataOffset( static_cast<uint8_t *>(pProfile->data), pProfile->size, icSigBlueTRCTag, sSize );
				if ( sOffset ) {
					uint8_t * pui8Data = static_cast<uint8_t *>(pProfile->data) + sOffset;
					if ( CIcc::FillOutTransferFunc( m_tfInColorSpaceTransferFunc[SL2_PC_B], pui8Data, sSize ) ) {
						m_dGamma = 0.0;
					}
				}*/


				/*sOffset = CIcc::GetTagDataOffset( static_cast<uint8_t *>(pProfile->data), pProfile->size, icSigRedColorantTag, sSize );
				if ( sOffset ) {
					CVector4<SL2_ST_AVX512> vTmp( 0.43603515625, 0.2224884033203125, 0.013916015625, 0.0 );
					CVector4<SL2_ST_AVX512> vTmp1( vTmp[0] / vTmp[1], 1.0, vTmp[2] / vTmp[1], 0.0 );
					CVector4<SL2_ST_AVX512> vTmp2 = vTmp1;
					vTmp2.Normalize();
					double dX = vTmp1[0] / (vTmp1[0] + vTmp1[1] + vTmp1[2]);
					double dY = vTmp1[1] / (vTmp1[0] + vTmp1[1] + vTmp1[2]);
					double dChromaX, dChromaZ;
					CUtilities::XYZtoChromaticity( 0.43603515625, 0.2224884033203125, 0.013916015625, dChromaX, dChromaZ );
					CVector4<SL2_ST_AVX512> vChroma( dX * (0.2224884033203125 / dY), 0.2224884033203125, (1.0 - dX - dY) * (0.2224884033203125 / dY), 0.0 );
					vTmp = vChroma;
					vTmp.Normalize();
					uint8_t * pui8Data = static_cast<uint8_t *>(pProfile->data) + sOffset;
					sOffset = 0;
				}*/
			}
		}

		switch ( fitType ) {
			case FIT_BITMAP : {
				switch ( ::FreeImage_GetBPP( _pbBitmap ) ) {
					case 1 : {
						if ( _sIdx == 0 ) {
							if ( !AllocateTexture( CFormat::FindFormatDataByVulkan( SL2_VK_FORMAT_R8G8B8A8_UNORM ), ui32Width, ui32Height, ui32Depth, 1, _sTotalIdx ) ) { return SL2_E_OUTOFMEMORY; }
						}
						uint8_t * pui8Dst = Data( 0, 0, _sIdx );
						const RGBQUAD * prgbqPal = ::FreeImage_GetPalette( _pbBitmap );
						uint32_t ui32Pitch = SL2_ROUND_UP( ui32Width * sizeof( CFormat::SL2_RGBA_UNORM ), 4 );
						if ( prgbqPal ) {
							for ( uint32_t Y = 0; Y < ui32Height; ++Y ) {
								const uint8_t * pui8Data = ::FreeImage_GetScanLine( _pbBitmap, Y );
								for ( uint32_t X = 0; X < ui32Width; ++X ) {
									uint8_t ui8Bit = (pui8Data[X>>3] >> (X % 8)) & 1;
									BYTE bAlpha = prgbqPal[ui8Bit].rgbReserved;
									if ( iTransIndex != -1 ) {
										bAlpha = (iTransIndex == ui8Bit) ? 0 : 255;
									}
									CFormat::SL2_RGBA_UNORM * pRgb = reinterpret_cast<CFormat::SL2_RGBA_UNORM *>(pui8Dst + ui32Pitch * Y) + X;
									pRgb->ui8Rgba[SL2_PC_R] = prgbqPal[ui8Bit].rgbRed;
									pRgb->ui8Rgba[SL2_PC_G] = prgbqPal[ui8Bit].rgbGreen;
									pRgb->ui8Rgba[SL2_PC_B] = prgbqPal[ui8Bit].rgbBlue;
									pRgb->ui8Rgba[SL2_PC_A] = bAlpha;
								}
							}
							break;
						}
						return SL2_E_INVALIDDATA;
					}
					case 4 : {
						if ( _sIdx == 0 ) {
							if ( !AllocateTexture( CFormat::FindFormatDataByVulkan( SL2_VK_FORMAT_R8G8B8A8_UNORM ), ui32Width, ui32Height, ui32Depth, 1, _sTotalIdx ) ) { return SL2_E_OUTOFMEMORY; }
						}
						uint8_t * pui8Dst = Data( 0, 0, _sIdx );
						const RGBQUAD * prgbqPal = ::FreeImage_GetPalette( _pbBitmap );
						uint32_t ui32Pitch = SL2_ROUND_UP( ui32Width * sizeof( CFormat::SL2_RGBA_UNORM ), 4 );
						if ( prgbqPal ) {
							for ( uint32_t Y = 0; Y < ui32Height; ++Y ) {
								const uint8_t * pui8Data = ::FreeImage_GetScanLine( _pbBitmap, Y );
								for ( uint32_t X = 0; X < ui32Width; ++X ) {
									uint8_t ui8Bit = (pui8Data[X>>1] >> (X % 2 << 2)) & 0xF;
									BYTE bAlpha = prgbqPal[ui8Bit].rgbReserved;
									if ( iTransIndex != -1 ) {
										bAlpha = (iTransIndex == ui8Bit) ? 0 : 255;
									}
									CFormat::SL2_RGBA_UNORM * pRgb = reinterpret_cast<CFormat::SL2_RGBA_UNORM *>(pui8Dst + ui32Pitch * Y) + X;
									pRgb->ui8Rgba[SL2_PC_R] = prgbqPal[ui8Bit].rgbRed;
									pRgb->ui8Rgba[SL2_PC_G] = prgbqPal[ui8Bit].rgbGreen;
									pRgb->ui8Rgba[SL2_PC_B] = prgbqPal[ui8Bit].rgbBlue;
									pRgb->ui8Rgba[SL2_PC_A] = bAlpha;
								}
							}
							break;
						}
						return SL2_E_INVALIDDATA;
					}
					case 8 : {
						if ( _sIdx == 0 ) {
							if ( !AllocateTexture( CFormat::FindFormatDataByVulkan( SL2_VK_FORMAT_R8G8B8A8_UNORM ), ui32Width, ui32Height, ui32Depth, 1, _sTotalIdx ) ) { return SL2_E_OUTOFMEMORY; }
						}
						uint8_t * pui8Dst = Data( 0, 0, _sIdx );
						const RGBQUAD * prgbqPal = ::FreeImage_GetPalette( _pbBitmap );
						uint32_t ui32Pitch = SL2_ROUND_UP( ui32Width * sizeof( CFormat::SL2_RGBA_UNORM ), 4 );
						if ( prgbqPal ) {
							for ( uint32_t Y = 0; Y < ui32Height; ++Y ) {
								const uint8_t * pui8Data = ::FreeImage_GetScanLine( _pbBitmap, Y );
								for ( uint32_t X = 0; X < ui32Width; ++X ) {
									uint8_t ui8Bit = pui8Data[X];
									BYTE bAlpha = prgbqPal[ui8Bit].rgbReserved;
									if ( iTransIndex != -1 ) {
										bAlpha = (iTransIndex == ui8Bit) ? 0 : 255;
									}
									CFormat::SL2_RGBA_UNORM * pRgb = reinterpret_cast<CFormat::SL2_RGBA_UNORM *>(pui8Dst + ui32Pitch * Y) + X;
									pRgb->ui8Rgba[SL2_PC_R] = prgbqPal[ui8Bit].rgbRed;
									pRgb->ui8Rgba[SL2_PC_G] = prgbqPal[ui8Bit].rgbGreen;
									pRgb->ui8Rgba[SL2_PC_B] = prgbqPal[ui8Bit].rgbBlue;
									pRgb->ui8Rgba[SL2_PC_A] = bAlpha;
								}
							}
							break;
						}
						return SL2_E_INVALIDDATA;
					}
					case 16 : {
						unsigned uRedMask = ::FreeImage_GetRedMask( _pbBitmap );
						unsigned uGreenMask = ::FreeImage_GetGreenMask( _pbBitmap );
						unsigned uBlueMask = ::FreeImage_GetBlueMask( _pbBitmap );
						uint32_t ui32Pitch = SL2_ROUND_UP( ui32Width * sizeof( uint16_t ), 4 );
						if ( (uRedMask == FI16_565_RED_MASK) && (uGreenMask == FI16_565_GREEN_MASK) && (uBlueMask == FI16_565_BLUE_MASK) ) {
							if ( _sIdx == 0 ) {
								if ( !AllocateTexture( CFormat::FindFormatDataByVulkan( SL2_VK_FORMAT_R5G6B5_UNORM_PACK16 ), ui32Width, ui32Height, ui32Depth, 1, _sTotalIdx ) ) { return SL2_E_OUTOFMEMORY; }
							}
							uint8_t * pui8Dst = Data( 0, 0, _sIdx );
							for ( uint32_t Y = 0; Y < ui32Height; ++Y ) {
								const uint16_t * pui16Data = reinterpret_cast<uint16_t *>(::FreeImage_GetScanLine( _pbBitmap, Y ));
								for ( uint32_t X = 0; X < ui32Width; ++X ) {
									CFormat::SL2_R5G6B5_PACKED * pRgb = reinterpret_cast<CFormat::SL2_R5G6B5_PACKED *>(pui8Dst + ui32Pitch * Y) + X;
									pRgb->ui16R = (pui16Data[X] & FI16_565_RED_MASK) >> FI16_565_RED_SHIFT;
									pRgb->ui16G = (pui16Data[X] & FI16_565_GREEN_MASK) >> FI16_565_GREEN_SHIFT;
									pRgb->ui16B = (pui16Data[X] & FI16_565_BLUE_MASK) >> FI16_565_BLUE_SHIFT;
								}
							}
						}
						else {
							if ( _sIdx == 0 ) {
								if ( !AllocateTexture( CFormat::FindFormatDataByVulkan( SL2_VK_FORMAT_A1R5G5B5_UNORM_PACK16 ), ui32Width, ui32Height, ui32Depth, 1, _sTotalIdx ) ) { return SL2_E_OUTOFMEMORY; }
							}
							uint8_t * pui8Dst = Data( 0, 0, _sIdx );
							for ( uint32_t Y = 0; Y < ui32Height; ++Y ) {
								const uint16_t * pui16Data = reinterpret_cast<uint16_t *>(::FreeImage_GetScanLine( _pbBitmap, Y ));
								for ( uint32_t X = 0; X < ui32Width; ++X ) {
									CFormat::SL2_A1R5G6B5_PACKED * pRgba = reinterpret_cast<CFormat::SL2_A1R5G6B5_PACKED *>(pui8Dst + ui32Pitch * Y) + X;
									pRgba->ui16R = (pui16Data[X] & FI16_555_RED_MASK) >> FI16_555_RED_SHIFT;
									pRgba->ui16G = (pui16Data[X] & FI16_555_GREEN_MASK) >> FI16_555_GREEN_SHIFT;
									pRgba->ui16B = (pui16Data[X] & FI16_555_BLUE_MASK) >> FI16_555_BLUE_SHIFT;
									pRgba->ui16A = (pui16Data[X] >> 7);
								}
							}
						}
						break;
					}
					case 24 : {
						if ( _sIdx == 0 ) {
							if ( !AllocateTexture( CFormat::FindFormatDataByVulkan( SL2_VK_FORMAT_R8G8B8_UNORM ), ui32Width, ui32Height, ui32Depth, 1, _sTotalIdx ) ) { return SL2_E_OUTOFMEMORY; }
						}
						uint8_t * pui8Dst = Data( 0, 0, _sIdx );
						uint32_t ui32Pitch = SL2_ROUND_UP( ui32Width * sizeof( CFormat::SL2_RGB_UNORM ), 4 );
						for ( uint32_t Y = 0; Y < ui32Height; ++Y ) {
							const RGBTRIPLE * prgbtData = reinterpret_cast<RGBTRIPLE *>(::FreeImage_GetScanLine( _pbBitmap, Y ));
							for ( uint32_t X = 0; X < ui32Width; ++X ) {
								CFormat::SL2_RGB_UNORM * pRgb = reinterpret_cast<CFormat::SL2_RGB_UNORM *>(pui8Dst + ui32Pitch * Y) + X;
								pRgb->ui8Rgb[SL2_PC_R] = prgbtData[X].rgbtRed;
								pRgb->ui8Rgb[SL2_PC_G] = prgbtData[X].rgbtGreen;
								pRgb->ui8Rgb[SL2_PC_B] = prgbtData[X].rgbtBlue;
							}
						}
						break;
					}
					case 32 : {
						if ( _sIdx == 0 ) {
							if ( !AllocateTexture( CFormat::FindFormatDataByVulkan( SL2_VK_FORMAT_R8G8B8A8_UNORM ), ui32Width, ui32Height, ui32Depth, 1, _sTotalIdx ) ) { return SL2_E_OUTOFMEMORY; }
						}
						uint8_t * pui8Dst = Data( 0, 0, _sIdx );
						uint32_t ui32Pitch = SL2_ROUND_UP( ui32Width * sizeof( CFormat::SL2_RGBA_UNORM ), 4 );
						for ( uint32_t Y = 0; Y < ui32Height; ++Y ) {
							const RGBQUAD * prgbqData = reinterpret_cast<RGBQUAD *>(::FreeImage_GetScanLine( _pbBitmap, Y ));
							for ( uint32_t X = 0; X < ui32Width; ++X ) {
								CFormat::SL2_RGBA_UNORM * pRgb = reinterpret_cast<CFormat::SL2_RGBA_UNORM *>(pui8Dst + ui32Pitch * Y) + X;
								pRgb->ui8Rgba[SL2_PC_R] = prgbqData[X].rgbRed;
								pRgb->ui8Rgba[SL2_PC_G] = prgbqData[X].rgbGreen;
								pRgb->ui8Rgba[SL2_PC_B] = prgbqData[X].rgbBlue;
								pRgb->ui8Rgba[SL2_PC_A] = prgbqData[X].rgbReserved;
							}
						}
						break;
					}
				}
				break;	// FIT_BITMAP
			}
			case FIT_UINT16 : {
				if ( _sIdx == 0 ) {
					if ( !AllocateTexture( CFormat::FindFormatDataByVulkan( SL2_VK_FORMAT_R16_UNORM ), ui32Width, ui32Height, ui32Depth, 1, _sTotalIdx ) ) { return SL2_E_OUTOFMEMORY; }
				}
				uint8_t * pui8Dst = Data( 0, 0, _sIdx );
				uint32_t ui32Pitch = SL2_ROUND_UP( ui32Width * sizeof( uint16_t ), 4 );
				for ( uint32_t Y = 0; Y < ui32Height; ++Y ) {
					const uint16_t * pui16Data = reinterpret_cast<uint16_t *>(::FreeImage_GetScanLine( _pbBitmap, Y ));
					for ( uint32_t X = 0; X < ui32Width; ++X ) {
						uint16_t * pRgb = reinterpret_cast<uint16_t *>(pui8Dst + ui32Pitch * Y) + X;
						pRgb[0] = pui16Data[X];
					}
				}
				break;	// FIT_UINT16
			}
			case FIT_INT16 : {
				if ( _sIdx == 0 ) {
					if ( !AllocateTexture( CFormat::FindFormatDataByVulkan( SL2_VK_FORMAT_R16_SNORM ), ui32Width, ui32Height, ui32Depth, 1, _sTotalIdx ) ) { return SL2_E_OUTOFMEMORY; }
				}
				uint8_t * pui8Dst = Data( 0, 0, _sIdx );
				uint32_t ui32Pitch = SL2_ROUND_UP( ui32Width * sizeof( uint16_t ), 4 );
				for ( uint32_t Y = 0; Y < ui32Height; ++Y ) {
					const int16_t * pi16Data = reinterpret_cast<int16_t *>(::FreeImage_GetScanLine( _pbBitmap, Y ));
					for ( uint32_t X = 0; X < ui32Width; ++X ) {
						int16_t * pRgb = reinterpret_cast<int16_t *>(pui8Dst + ui32Pitch * Y) + X;
						pRgb[0] = pi16Data[X];
					}
				}
				break;	// FIT_INT16
			}
			case FIT_UINT32 : {
				if ( _sIdx == 0 ) {
					if ( !AllocateTexture( CFormat::FindFormatDataByVulkan( SL2_VK_FORMAT_R32_UINT ), ui32Width, ui32Height, ui32Depth, 1, _sTotalIdx ) ) { return SL2_E_OUTOFMEMORY; }
				}
				uint8_t * pui8Dst = Data( 0, 0, _sIdx );
				uint32_t ui32Pitch = SL2_ROUND_UP( ui32Width * sizeof( uint32_t ), 4 );
				for ( uint32_t Y = 0; Y < ui32Height; ++Y ) {
					const uint32_t * pui32Data = reinterpret_cast<uint32_t *>(::FreeImage_GetScanLine( _pbBitmap, Y ));
					for ( uint32_t X = 0; X < ui32Width; ++X ) {
						uint32_t * pRgb = reinterpret_cast<uint32_t *>(pui8Dst + ui32Pitch * Y) + X;
						pRgb[0] = pui32Data[X];
					}
				}
				break;	// FIT_UINT32
			}
			case FIT_INT32 : {
				if ( _sIdx == 0 ) {
					if ( !AllocateTexture( CFormat::FindFormatDataByVulkan( SL2_VK_FORMAT_R32_SINT ), ui32Width, ui32Height, ui32Depth, 1, _sTotalIdx ) ) { return SL2_E_OUTOFMEMORY; }
				}
				uint8_t * pui8Dst = Data( 0, 0, _sIdx );
				uint32_t ui32Pitch = SL2_ROUND_UP( ui32Width * sizeof( uint32_t ), 4 );
				for ( uint32_t Y = 0; Y < ui32Height; ++Y ) {
					const int32_t * pi32Data = reinterpret_cast<int32_t *>(::FreeImage_GetScanLine( _pbBitmap, Y ));
					for ( uint32_t X = 0; X < ui32Width; ++X ) {
						int32_t * pRgb = reinterpret_cast<int32_t *>(pui8Dst + ui32Pitch * Y) + X;
						pRgb[0] = pi32Data[X];
					}
				}
				break;	// FIT_INT32
			}
			case FIT_FLOAT : {
				if ( _sIdx == 0 ) {
					if ( !AllocateTexture( CFormat::FindFormatDataByVulkan( SL2_VK_FORMAT_R32_SFLOAT ), ui32Width, ui32Height, ui32Depth, 1, _sTotalIdx ) ) { return SL2_E_OUTOFMEMORY; }
				}
				uint8_t * pui8Dst = Data( 0, 0, _sIdx );
				uint32_t ui32Pitch = SL2_ROUND_UP( ui32Width * sizeof( float ), 4 );
				for ( uint32_t Y = 0; Y < ui32Height; ++Y ) {
					const float * pfData = reinterpret_cast<float *>(::FreeImage_GetScanLine( _pbBitmap, Y ));
					for ( uint32_t X = 0; X < ui32Width; ++X ) {
						float fBit = pfData[X];
						float * pRgb = reinterpret_cast<float *>(pui8Dst + ui32Pitch * Y) + X;
						pRgb[0] = fBit;
					}
				}
				break;	// FIT_FLOAT
			}
			case FIT_DOUBLE : {
				if ( _sIdx == 0 ) {
					if ( !AllocateTexture( CFormat::FindFormatDataByVulkan( SL2_VK_FORMAT_R64_SFLOAT ), ui32Width, ui32Height, ui32Depth, 1, _sTotalIdx ) ) { return SL2_E_OUTOFMEMORY; }
				}
				uint8_t * pui8Dst = Data( 0, 0, _sIdx );
				uint32_t ui32Pitch = SL2_ROUND_UP( ui32Width * sizeof( double ), 4 );
				for ( uint32_t Y = 0; Y < ui32Height; ++Y ) {
					const double * pdData = reinterpret_cast<double *>(::FreeImage_GetScanLine( _pbBitmap, Y ));
					for ( uint32_t X = 0; X < ui32Width; ++X ) {
						double * pRgb = reinterpret_cast<double *>(pui8Dst + ui32Pitch) + X;
						pRgb[0] = pdData[X];
					}
				}
				break;	// FIT_DOUBLE
			}
			case FIT_COMPLEX : {
				if ( _sIdx == 0 ) {
					if ( !AllocateTexture( CFormat::FindFormatDataByVulkan( SL2_VK_FORMAT_R64G64_SFLOAT ), ui32Width, ui32Height, ui32Depth, 1, _sTotalIdx ) ) { return SL2_E_OUTOFMEMORY; }
				}
				uint8_t * pui8Dst = Data( 0, 0, _sIdx );
				uint32_t ui32Pitch = SL2_ROUND_UP( ui32Width * sizeof( FICOMPLEX ), 4 );
				for ( uint32_t Y = 0; Y < ui32Height; ++Y ) {
					const FICOMPLEX * pfData = reinterpret_cast<FICOMPLEX *>(::FreeImage_GetScanLine( _pbBitmap, Y ));
					for ( uint32_t X = 0; X < ui32Width; ++X ) {
						FICOMPLEX * pRgb = reinterpret_cast<FICOMPLEX *>(pui8Dst + ui32Pitch * Y) + X;
						(*pRgb) = pfData[X];
					}
				}
				break;	// FIT_COMPLEX
			}
			case FIT_RGB16 : {
				if ( _sIdx == 0 ) {
					if ( !AllocateTexture( CFormat::FindFormatDataByVulkan( SL2_VK_FORMAT_R16G16B16_UNORM ), ui32Width, ui32Height, ui32Depth, 1, _sTotalIdx ) ) { return SL2_E_OUTOFMEMORY; }
				}
				uint8_t * pui8Dst = Data( 0, 0, _sIdx );
				uint32_t ui32Pitch = SL2_ROUND_UP( ui32Width * sizeof( CFormat::SL2_RGB16_UNORM ), 4 );
				for ( uint32_t Y = 0; Y < ui32Height; ++Y ) {
					const FIRGB16 * pfrgb16Data = reinterpret_cast<FIRGB16 *>(::FreeImage_GetScanLine( _pbBitmap, Y ));
					for ( uint32_t X = 0; X < ui32Width; ++X ) {
						CFormat::SL2_RGB16_UNORM * pRgb = reinterpret_cast<CFormat::SL2_RGB16_UNORM *>(pui8Dst + ui32Pitch * Y) + X;
						pRgb->ui16Rgb[SL2_PC_R] = pfrgb16Data[X].red;
						pRgb->ui16Rgb[SL2_PC_G] = pfrgb16Data[X].green;
						pRgb->ui16Rgb[SL2_PC_B] = pfrgb16Data[X].blue;
					}
				}
				break;	// FIT_RGB16
			}
			case FIT_RGBA16 : {
				if ( _sIdx == 0 ) {
					if ( !AllocateTexture( CFormat::FindFormatDataByVulkan( SL2_VK_FORMAT_R16G16B16A16_UNORM ), ui32Width, ui32Height, ui32Depth, 1, _sTotalIdx ) ) { return SL2_E_OUTOFMEMORY; }
				}
				uint8_t * pui8Dst = Data( 0, 0, _sIdx );
				uint32_t ui32Pitch = SL2_ROUND_UP( ui32Width * sizeof( CFormat::SL2_RGB16_UNORM ), 4 );
				for ( uint32_t Y = 0; Y < ui32Height; ++Y ) {
					const FIRGBA16 * pfrgba16Data = reinterpret_cast<FIRGBA16 *>(::FreeImage_GetScanLine( _pbBitmap, Y ));
					for ( uint32_t X = 0; X < ui32Width; ++X ) {
						CFormat::SL2_RGBA16_UNORM * pRgb = reinterpret_cast<CFormat::SL2_RGBA16_UNORM *>(pui8Dst + ui32Pitch * Y) + X;
						pRgb->ui16Rgba[SL2_PC_R] = pfrgba16Data[X].red;
						pRgb->ui16Rgba[SL2_PC_G] = pfrgba16Data[X].green;
						pRgb->ui16Rgba[SL2_PC_B] = pfrgba16Data[X].blue;
						pRgb->ui16Rgba[SL2_PC_A] = pfrgba16Data[X].alpha;
					}
				}
				break;	// FIT_RGBA16
			}
			case FIT_RGBF : {
				if ( _sIdx == 0 ) {
					if ( !AllocateTexture( CFormat::FindFormatDataByVulkan( SL2_VK_FORMAT_R32G32B32_SFLOAT ), ui32Width, ui32Height, ui32Depth, 1, _sTotalIdx ) ) { return SL2_E_OUTOFMEMORY; }
				}
				uint8_t * pui8Dst = Data( 0, 0, _sIdx );
				uint32_t ui32Pitch = SL2_ROUND_UP( ui32Width * sizeof( CFormat::SL2_RGB ), 4 );
				for ( uint32_t Y = 0; Y < ui32Height; ++Y ) {
					const FIRGBF * pfData = reinterpret_cast<FIRGBF *>(::FreeImage_GetScanLine( _pbBitmap, Y ));
					for ( uint32_t X = 0; X < ui32Width; ++X ) {
						CFormat::SL2_RGB * pRgb = reinterpret_cast<CFormat::SL2_RGB *>(pui8Dst + ui32Pitch * Y) + X;
						pRgb->fRgb[SL2_PC_R] = pfData[X].red;
						pRgb->fRgb[SL2_PC_G] = pfData[X].green;
						pRgb->fRgb[SL2_PC_B] = pfData[X].blue;
					}
				}
				break;	// FIT_RGBF
			}
			case FIT_RGBAF : {
				if ( _sIdx == 0 ) {
					if ( !AllocateTexture( CFormat::FindFormatDataByVulkan( SL2_VK_FORMAT_R32G32B32A32_SFLOAT ), ui32Width, ui32Height, ui32Depth, 1, _sTotalIdx ) ) { return SL2_E_OUTOFMEMORY; }
				}
				uint8_t * pui8Dst = Data( 0, 0, _sIdx );
				uint32_t ui32Pitch = SL2_ROUND_UP( ui32Width * sizeof( CFormat::SL2_RGB ), 4 );
				for ( uint32_t Y = 0; Y < ui32Height; ++Y ) {
					const FIRGBAF * pfData = reinterpret_cast<FIRGBAF *>(::FreeImage_GetScanLine( _pbBitmap, Y ));
					for ( uint32_t X = 0; X < ui32Width; ++X ) {
						CFormat::SL2_RGBA * pRgb = reinterpret_cast<CFormat::SL2_RGBA *>(pui8Dst + ui32Pitch * Y) + X;
						pRgb->fRgba[SL2_PC_R] = pfData[X].red;
						pRgb->fRgba[SL2_PC_G] = pfData[X].green;
						pRgb->fRgba[SL2_PC_B] = pfData[X].blue;
						pRgb->fRgba[SL2_PC_A] = pfData[X].alpha;
					}
				}
				break;	// FIT_RGBAF
			}
			case FIT_UNKNOWN : { return SL2_E_INVALIDFILETYPE; }
			default : { break; }
		};

#undef FreeImage_GetScanLine

		return SL2_E_SUCCESS;
	}

	/**
	 * \brief Converts a FreeImage bitmap to 32-bit RGBA.
	 *
	 * \param _pbBitmap The input bitmap to convert.
	 * \return FIBITMAP* A 32-bit RGBA bitmap. The caller is responsible for freeing the returned bitmap.
	 */
	FIBITMAP * CImage::ConvertToRGBA32( FIBITMAP * _pbBitmap ) {
		if ( !_pbBitmap ) { throw std::invalid_argument( "Invalid bitmap provided." ); }

		FREE_IMAGE_TYPE itType = ::FreeImage_GetImageType( _pbBitmap );
		unsigned uBpp = ::FreeImage_GetBPP( _pbBitmap );

		if ( itType == FIT_BITMAP && uBpp == 32 ) {
			return ::FreeImage_Clone( _pbBitmap );
		}

		return ::FreeImage_ConvertTo32Bits( _pbBitmap );
	}

	/**
	 * \brief Converts a 32-bit RGBA bitmap to another bit depth supported by FreeImage.
	 *
	 * \param _pbBitmap The 32-bit RGBA bitmap to convert.
	 * \param _uTargetBpp The target bit depth (1, 2, 4, 8, 24, or 32).
	 * \param _bIs565 Whether the target 16-bit format should use 5-6-5 instead of 5-5-5.
	 * \return FIBITMAP* A bitmap with the specified bit depth. The caller is responsible for freeing the returned bitmap.
	 */
	//FIBITMAP * CImage::ConvertFromRGBA32( FIBITMAP * _pbBitmap, unsigned _uTargetBpp, bool _bIs565 ) {
	//	if ( !_pbBitmap ) { throw std::invalid_argument( "Invalid bitmap provided." ); }

	//	if ( ::FreeImage_GetBPP( _pbBitmap ) != 32 ) { throw std::invalid_argument( "Input bitmap is not 32-bit RGBA." ); }

	//	switch ( _uTargetBpp ) {
	//		case 1 : {}	SL2_FALLTHROUGH
	//		case 4 : {}	SL2_FALLTHROUGH
	//		case 8 : {
	//			// Convert to 24-bit first, as FreeImage_ColorQuantize only works on 24-bit images.
	//			FIBITMAP * pConverted24 = FreeImage_ConvertTo24Bits( _pbBitmap );
	//			if ( !pConverted24 ) {
	//				throw std::runtime_error( "Failed to convert to 24-bit for color quantization." );
	//			}
	//			FIBITMAP * pQuantized = ::FreeImage_ColorQuantize( pConverted24, FIQ_WUQUANT );
	//			::FreeImage_Unload( pConverted24 );
	//			if ( !pQuantized ) {
	//				throw std::runtime_error( "Failed to quantize image to target bit depth." );
	//			}
	//			return pQuantized;
	//		}
	//		case 16: {
	//			if ( _bIs565 ) {
	//				return ::FreeImage_ConvertTo16Bits565( _pbBitmap );
	//			}
	//			else {
	//				return ::FreeImage_ConvertTo16Bits555( _pbBitmap );
	//			}
	//		}
	//		case 24 : {
	//			return ::FreeImage_ConvertTo24Bits( _pbBitmap );
	//		}
	//		case 32 : {
	//			return ::FreeImage_Clone( _pbBitmap );
	//		}
	//		default : {
	//			throw std::invalid_argument( "Unsupported target bit depth." );
	//		}
	//	}
	//}

	/**
	 * \brief Blends one image onto another with transparency support.
	 *
	 * \param _pbBaseCanvas The destination image.
	 * \param _pbBitmapFrame The source image to blend onto the destination.
	 * \param _iFrameLeft The left offset for the frame.
	 * \param _iFrameTop The top offset for the frame.
	 */
	void CImage::AlphaBlend( FIBITMAP * _pbBaseCanvas, FIBITMAP * _pbBitmapFrame, uint16_t _iFrameLeft, uint16_t _iFrameTop ) {
		unsigned uBaseW = ::FreeImage_GetWidth( _pbBaseCanvas );
		unsigned uBaseH = ::FreeImage_GetHeight( _pbBaseCanvas );
		unsigned uFrameW = ::FreeImage_GetWidth( _pbBitmapFrame );
		unsigned uFrameH = ::FreeImage_GetHeight( _pbBitmapFrame );

		unsigned uBasePitch = ::FreeImage_GetPitch( _pbBaseCanvas );
		unsigned uFramePitch = ::FreeImage_GetPitch( _pbBitmapFrame );

		BYTE * pbBaseBits = ::FreeImage_GetBits( _pbBaseCanvas );
		BYTE * pbFrameBits = ::FreeImage_GetBits( _pbBitmapFrame );
		_iFrameTop = (uBaseH) - (uFrameH + _iFrameTop);
		for ( unsigned Y = 0; Y < uFrameH; ++Y ) {
			if ( _iFrameTop + Y >= uBaseH ) { continue; }

			BYTE * pbBaseRow = pbBaseBits + (_iFrameTop + Y) * uBasePitch;
			BYTE * pbFrameRow = pbFrameBits + Y * uFramePitch;

			for ( unsigned X = 0; X < uFrameW; ++X ) {
				if ( _iFrameLeft + X >= uBaseW ) { continue; }

				BYTE * pbBasePixel = pbBaseRow + (_iFrameLeft + X) * 4; // Assuming 32-bit images (RGBA)
				BYTE * pbFramePixel = pbFrameRow + X * 4;

				BYTE bFrameAlpha = pbFramePixel[FI_RGBA_ALPHA];
				float fAlpha = bFrameAlpha / 255.0f;

				pbBasePixel[FI_RGBA_RED] = static_cast<BYTE>((1 - fAlpha) * pbBasePixel[FI_RGBA_RED] + fAlpha * pbFramePixel[FI_RGBA_RED]);
				pbBasePixel[FI_RGBA_GREEN] = static_cast<BYTE>((1 - fAlpha) * pbBasePixel[FI_RGBA_GREEN] + fAlpha * pbFramePixel[FI_RGBA_GREEN]);
				pbBasePixel[FI_RGBA_BLUE] = static_cast<BYTE>((1 - fAlpha) * pbBasePixel[FI_RGBA_BLUE] + fAlpha * pbFramePixel[FI_RGBA_BLUE]);
				pbBasePixel[FI_RGBA_ALPHA] = static_cast<BYTE>(std::max( pbBasePixel[FI_RGBA_ALPHA], bFrameAlpha ));
			}
		}
	}

	/**
	 * Loads a KTX1 file from memory.
	 * 
	 * \param _vData The file to load.
	 * \return Returns an error code.
	 **/
	SL2_ERRORS CImage::LoadKtx1( const std::vector<uint8_t> &_vData ) {
		CKtxTexture<ktxTexture1> kt1Tex;
		::KTX_error_code ecErr = ::ktxTexture1_CreateFromMemory( _vData.data(), _vData.size(),
			KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT,
			kt1Tex.HandlePointer() );
		if ( KTX_SUCCESS == ecErr && kt1Tex.Handle() ) {
			if ( !AllocateTexture( CFormat::FindFormatDataByOgl( static_cast<SL2_KTX_INTERNAL_FORMAT>(kt1Tex.Handle()->glInternalformat) ),
				kt1Tex.Handle()->baseWidth, kt1Tex.Handle()->baseHeight, kt1Tex.Handle()->baseDepth,
				kt1Tex.Handle()->numLevels, kt1Tex.Handle()->numLayers, kt1Tex.Handle()->numFaces ) ) { return SL2_E_OUTOFMEMORY; }

			if ( KTX_SUCCESS == ::ktxTexture_IterateLevelFaces( ktxTexture( kt1Tex.Handle() ), KtxImageLoad, this ) ) {
				return SL2_E_SUCCESS;
			}

		}
		return SL2_E_INVALIDFILETYPE;
	}

	/**
	 * Loads a KTX2 file from memory.
	 * 
	 * \param _vData The file to load.
	 * \return Returns an error code.
	 **/
	SL2_ERRORS CImage::LoadKtx2( const std::vector<uint8_t> &_vData ) {
		CKtxTexture<ktxTexture2> kt2Tex;
		::KTX_error_code ecErr = ::ktxTexture2_CreateFromMemory( _vData.data(), _vData.size(),
			KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT,
			kt2Tex.HandlePointer() );
		if ( KTX_SUCCESS == ecErr && kt2Tex.Handle() ) {
			if ( ::ktxTexture2_NeedsTranscoding( (*kt2Tex.HandlePointer()) ) ) {
				if ( KTX_SUCCESS != ::ktxTexture2_TranscodeBasis( (*kt2Tex.HandlePointer()), KTX_TTF_RGBA32, 0 ) ) {
					return SL2_E_INVALIDDATA;
				}
			}
			if ( !AllocateTexture( CFormat::FindFormatDataByVulkan( static_cast<SL2_VKFORMAT>(kt2Tex.Handle()->vkFormat) ),
				kt2Tex.Handle()->baseWidth, kt2Tex.Handle()->baseHeight, kt2Tex.Handle()->baseDepth,
				kt2Tex.Handle()->numLevels, kt2Tex.Handle()->numLayers, kt2Tex.Handle()->numFaces ) ) { return SL2_E_OUTOFMEMORY; }

			if ( KTX_SUCCESS == ::ktxTexture_IterateLevelFaces( ktxTexture( kt2Tex.Handle() ), KtxImageLoad, this ) ) {
				return SL2_E_SUCCESS;
			}
		}
		return SL2_E_INVALIDFILETYPE;
	}

	/**
	 * Loads a DDS file from memory.
	 * 
	 * \param _vData The file to load.
	 * \return Returns an error code.
	 **/
	SL2_ERRORS CImage::LoadDds( const std::vector<uint8_t> &_vData ) {
		CDds dFile;
		if ( !dFile.LoadDds( _vData ) ) { return SL2_E_INVALIDFILETYPE; }

		const CFormat::SL2_KTX_INTERNAL_FORMAT_DATA * aFmt = nullptr;
		if ( static_cast<SL2_DXGI_FORMAT>(dFile.Format()) == SL2_DXGI_FORMAT_UNKNOWN ) {
			for ( size_t I = 0; I < CFormat::TotalFormats(); ++I ) {
				auto aTmp = CFormat::FormatByIdx( I );
				if ( aTmp->ui32BlockSizeInBits == dFile.Header().dpPixelFormat.ui32RGBBitCount ) {
					uint32_t ui32RMask = ((1 << aTmp->ui8RBits) - 1) << aTmp->ui8RShift;
                    uint32_t ui32GMask = ((1 << aTmp->ui8GBits) - 1) << aTmp->ui8GShift;
                    uint32_t ui32BMask = ((1 << aTmp->ui8BBits) - 1) << aTmp->ui8BShift;
                    uint32_t ui32AMask = ((1 << aTmp->ui8ABits) - 1) << aTmp->ui8AShift;

					if ( ui32RMask == dFile.Header().dpPixelFormat.ui32RBitMask &&
						ui32GMask == dFile.Header().dpPixelFormat.ui32GBitMask &&
						ui32BMask == dFile.Header().dpPixelFormat.ui32BBitMask &&
						ui32AMask == dFile.Header().dpPixelFormat.ui32ABitMask ) {
						// TODO: Handle YUV etc.
						aFmt = aTmp;
						break;
					}
				}
			}
		}
		else {
			aFmt = CFormat::FindFormatDataByDx( static_cast<SL2_DXGI_FORMAT>(dFile.Format()) );
		}
		if ( !aFmt ) { return SL2_E_INVALIDFILETYPE; }
		if ( !AllocateTexture( aFmt,
			dFile.Width(), dFile.Height(), dFile.Depth(),
			dFile.Mips(), dFile.Array(), dFile.Faces() ) ) { return SL2_E_OUTOFMEMORY; }
		if ( dFile.Faces() > 1 ) {
			for ( uint32_t F = 0; F < dFile.Faces(); ++F ) {
				for ( uint32_t M = 0; M < dFile.Mips(); ++M ) {
					size_t sIdx = F * dFile.Mips() + M;
					if ( sIdx >= dFile.Buffers().size() ) { return SL2_E_INVALIDDATA; }
					uint32_t ui32Width = std::max( dFile.Width() >> M, static_cast<uint32_t>(1) );
					uint32_t ui32Height = std::max( dFile.Height() >> M, static_cast<uint32_t>(1) );
					uint32_t ui32Depth = std::max( dFile.Depth() >> M, static_cast<uint32_t>(1) );

					if ( dFile.Header().ui32Flags & SL2_DF_LINEARSIZE ) {
						// Compressed texture.
						uint64_t ui64PageSize = CFormat::GetFormatSize( aFmt, ui32Width,
							ui32Height,
							1 );
						if ( uint64_t( size_t( ui64PageSize ) ) != ui64PageSize ) { return SL2_E_UNSUPPORTEDSIZE; }
						for ( uint32_t D = 0; D < ui32Depth; ++D ) {
							if ( dFile.Buffers()[sIdx].vTexture.size() - (ui64PageSize * D) < ui64PageSize ) { return SL2_E_INVALIDDATA; }
							std::memcpy( Data( M, D, 0, F ), dFile.Buffers()[sIdx].vTexture.data() + ui64PageSize * D, size_t( ui64PageSize ) );
						}
					}
					else {
						uint64_t ui64SrcPitch = CFormat::GetRowSize_NoPadding( aFmt, std::max( dFile.Width() >> M, static_cast<uint32_t>(1) ) );
						if ( uint64_t( size_t( ui64SrcPitch ) ) != ui64SrcPitch ) { return SL2_E_UNSUPPORTEDSIZE; }
						auto aPitch = CFormat::GetRowSize( aFmt, std::max( dFile.Width() >> M, static_cast<uint32_t>(1) ) );

						for ( uint32_t D = 0; D < ui32Depth; ++D ) {
							uint64_t ui64DstSliceOffset = D * ui32Height * aPitch;
							uint64_t ui64SrcSliceOffset = D * ui32Height * ui64SrcPitch;

							for ( uint32_t H = 0; H < ui32Height; ++H ) {
								uint64_t ui64Dst = aPitch * H + ui64DstSliceOffset;
								uint64_t ui64Src = ui64SrcPitch * H + ui64SrcSliceOffset;

								if ( (dFile.Buffers()[sIdx].vTexture.size() - ui64Src) < ui64SrcPitch ) { return SL2_E_INVALIDDATA; }
								std::memcpy( Data( M, 0, 0, F ) + ui64Dst, dFile.Buffers()[sIdx].vTexture.data() + ui64Src, size_t( ui64SrcPitch ) );
							}
						}
					}
					//std::memcpy( Data( M, 0, 0, F ), dFile.Buffers()[sIdx].vTexture.data(), dFile.Buffers()[sIdx].vTexture.size() );
				}
			}
		}
		else {
			for ( uint32_t A = 0; A < dFile.Array(); ++A ) {
				for ( uint32_t M = 0; M < dFile.Mips(); ++M ) {
					size_t sIdx = A * dFile.Mips() + M;
					if ( sIdx >= dFile.Buffers().size() ) { return SL2_E_INVALIDDATA; }
					
					uint32_t ui32Width = std::max( dFile.Width() >> M, static_cast<uint32_t>(1) );
					uint32_t ui32Height = std::max( dFile.Height() >> M, static_cast<uint32_t>(1) );
					uint32_t ui32Depth = std::max( dFile.Depth() >> M, static_cast<uint32_t>(1) );

					if ( dFile.Header().ui32Flags & SL2_DF_LINEARSIZE ) {
						// Compressed texture.
						uint64_t ui64PageSize = CFormat::GetFormatSize( aFmt, ui32Width,
							ui32Height,
							1 );
						if ( uint64_t( size_t( ui64PageSize ) ) != ui64PageSize ) { return SL2_E_UNSUPPORTEDSIZE; }
						for ( uint32_t D = 0; D < ui32Depth; ++D ) {
							if ( dFile.Buffers()[sIdx].vTexture.size() - (ui64PageSize * D) < ui64PageSize ) { return SL2_E_INVALIDDATA; }
							std::memcpy( Data( M, D, A, 0 ), dFile.Buffers()[sIdx].vTexture.data() + ui64PageSize * D, size_t( ui64PageSize ) );
						}
					}
					else {
						uint64_t ui64SrcPitch = CFormat::GetRowSize_NoPadding( aFmt, std::max( dFile.Width() >> M, static_cast<uint32_t>(1) ) );
						if ( uint64_t( size_t( ui64SrcPitch ) ) != ui64SrcPitch ) { return SL2_E_UNSUPPORTEDSIZE; }
						auto aPitch = CFormat::GetRowSize( aFmt, std::max( dFile.Width() >> M, static_cast<uint32_t>(1) ) );

						for ( uint32_t D = 0; D < ui32Depth; ++D ) {
							uint64_t ui64DstSliceOffset = D * ui32Height * aPitch;
							uint64_t ui64SrcSliceOffset = D * ui32Height * ui64SrcPitch;
							for ( uint32_t H = 0; H < ui32Height; ++H ) {
								uint64_t ui64Dst = aPitch * H + ui64DstSliceOffset;
								uint64_t ui64Src = ui64SrcPitch * H + ui64SrcSliceOffset;

								if ( dFile.Buffers()[sIdx].vTexture.size() - ui64Src < ui64SrcPitch ) { return SL2_E_INVALIDDATA; }
								std::memcpy( Data( M, 0, A, 0 ) + ui64Dst, dFile.Buffers()[sIdx].vTexture.data() + ui64Src, size_t( ui64SrcPitch ) );
							}
						}
					}

					//std::memcpy( Data( M, 0, 0, F ), dFile.Buffers()[sIdx].vTexture.data(), dFile.Buffers()[sIdx].vTexture.size() );
				}
			}
		}

		if ( dFile.Header().ui32Caps2 & SL2_DDSCAPS2_CUBEMAP ) {
			m_ttType = SL2_TT_CUBE;
		}
		else if ( dFile.UsesExtHeader() && dFile.Header10().ui32ResourceDimension >= SL2_DDS_DIMENSION_TEXTURE1D && dFile.Header10().ui32ResourceDimension <= SL2_DDS_DIMENSION_TEXTURE3D ) {
			switch ( dFile.Header10().ui32ResourceDimension ) {
				case SL2_DDS_DIMENSION_TEXTURE1D : {
					m_ttType = SL2_TT_1D;
					break;
				}
				case SL2_DDS_DIMENSION_TEXTURE2D : {
					m_ttType = SL2_TT_2D;
					break;
				}
				case SL2_DDS_DIMENSION_TEXTURE3D : {
					m_ttType = SL2_TT_3D;
					break;
				}
			}
		}
		else if ( Depth() > 1 ) {
			m_ttType = SL2_TT_3D;
		}
		else if ( Height() > 1 ) {
			m_ttType = SL2_TT_2D;
		}

		if ( dFile.UsesExtHeader() ) {
			switch ( dFile.Header10().ui32MiscFlags2 & 0b111 ) {
				case SL2_DDS_ALPHA_MODE_PREMULTIPLIED : {
					m_bIsPreMultiplied = true;
					break;
				}
			}
		}
		return SL2_E_SUCCESS;
	}

	/**
	 * Loads a BMP file from memory.
	 * 
	 * \param _vData The file to load.
	 * \return Returns an error code.
	 **/
	SL2_ERRORS CImage::LoadBmp( const std::vector<uint8_t> &_vData ) {
		// Size checks.
		if ( _vData.size() < sizeof( SL2_BITMAPFILEHEADER ) + sizeof( SL2_BITMAPINFOHEADER ) ) { return SL2_E_INVALIDFILETYPE; }
		const SL2_BITMAPFILEHEADER * lpbfhHeader = reinterpret_cast<const SL2_BITMAPFILEHEADER *>(_vData.data());
		// Check the header.
		if ( lpbfhHeader->ui16Header != 0x4D42 ) { return SL2_E_INVALIDFILETYPE; }

		// Verify the size of the file.
		if ( lpbfhHeader->ui32Size != _vData.size() ) { return SL2_E_INVALIDFILETYPE; }

		// Header checks are done.  Move on to the bitmap info.
		const SL2_BITMAPINFOHEADER * lpbfhInfo = reinterpret_cast<const SL2_BITMAPINFOHEADER *>(&_vData.data()[sizeof( SL2_BITMAPFILEHEADER )]);
		
		// Check the size of the information header.
		if ( lpbfhInfo->ui32InfoSize < sizeof( SL2_BITMAPINFOHEADER ) ) { return SL2_E_INVALIDFILETYPE; }

		// Verify the compression.
		int32_t i32Compression = static_cast<int32_t>(lpbfhInfo->ui32Compression);
		if ( i32Compression < BI_RGB || i32Compression > BI_BITFIELDS ) { return SL2_E_INVALIDFILETYPE; }
		if ( i32Compression == BI_RLE8 ) {
			if ( _vData.size() < sizeof( SL2_BITMAPFILEHEADER ) + sizeof( SL2_BITMAPINFOHEADER ) + sizeof( SL2_BITMAPPALETTE ) * lpbfhInfo->ui32ColorsInPalette ) { return SL2_E_INVALIDFILETYPE; }
		}
		if ( i32Compression == BI_BITFIELDS ) {
			if ( _vData.size() < sizeof( SL2_BITMAPFILEHEADER ) + sizeof( SL2_BITMAPINFOHEADER ) + sizeof( SL2_BITMAPCOLORMASK ) ) { return SL2_E_INVALIDFILETYPE; }
		}


		// We only handle bits-per-pixels of 1, 4, 8, 16, 24, and 32.
		switch ( lpbfhInfo->ui16BitsPerPixel ) {
			case 1 : {}
			case 4 : { /*return SL2_E_INVALIDFILETYPE;*/ }	// Temporarily.
			case 8 : {
				if ( lpbfhInfo->ui32ColorsInPalette > 256 ) { return SL2_E_INVALIDFILETYPE; }
				if ( _vData.size() < sizeof( SL2_BITMAPFILEHEADER ) + sizeof( SL2_BITMAPINFOHEADER ) + sizeof( SL2_BITMAPPALETTE ) * lpbfhInfo->ui32ColorsInPalette ) { return SL2_E_INVALIDFILETYPE; }
				break;
			}
			case 16 : {}
			case 24 : {}
			case 32 : { break; }
			default : { return SL2_E_INVALIDFILETYPE; }
		}

		SL2_VKFORMAT vFormat = SL2_VK_FORMAT_UNDEFINED;
		SL2_KTX_INTERNAL_FORMAT kifOglFormat = SL2_GL_INVALID;
		uint32_t ui32BytesPerPixel;
		uint32_t ui32BytesPerPixelDst;
		uint32_t ui32BitMask = 0x0;
		switch ( lpbfhInfo->ui16BitsPerPixel ) {
			case 1 : {
				//vFormat = SL2_VK_FORMAT_R8G8B8_UNORM;
				kifOglFormat = SL2_GL_COLOR_INDEX1_EXT;
				ui32BytesPerPixel = 1;
				ui32BytesPerPixelDst = 3;
				ui32BitMask = 0x1;
				break;
			}
			case 4 : {
				//vFormat = SL2_VK_FORMAT_R8G8B8_UNORM;
				kifOglFormat = SL2_GL_COLOR_INDEX4_EXT;
				ui32BytesPerPixel = 4;
				ui32BytesPerPixelDst = 3;
				ui32BitMask = 0xF;
				break;
			}
			case 8 : {
				kifOglFormat = SL2_GL_COLOR_INDEX8_EXT;
				//vFormat = SL2_VK_FORMAT_R8G8B8_UNORM;
				ui32BytesPerPixel = 8;
				ui32BytesPerPixelDst = 3;
				ui32BitMask = 0xFF;
				break;
			}
			case 16 : {
				vFormat = SL2_VK_FORMAT_R5G6B5_UNORM_PACK16;
				ui32BytesPerPixel = ui32BytesPerPixelDst = 2;
				break;
			}
			case 24 : { vFormat = SL2_VK_FORMAT_R8G8B8_UNORM; ui32BytesPerPixel = ui32BytesPerPixelDst = 3; break; }
			case 32 : { vFormat = SL2_VK_FORMAT_R8G8B8A8_UNORM; ui32BytesPerPixel = ui32BytesPerPixelDst = 4; break; }
			default : { return SL2_E_INVALIDFILETYPE; }
		}


		// We are now ready to begin the extraction of the image data.
		// Start by allocating enough RAM.
		uint32_t ui32Height = lpbfhInfo->ui32Height;
		bool bReverse = false;
		if ( ui32Height & 0x80000000 ) {
			bReverse = true;
			// Invert the height.
			ui32Height = ~ui32Height + 1;
		}


		// Everything is set.  Start the extraction.

		uint32_t ui32RowWidth = lpbfhInfo->ui32Width * ui32BytesPerPixel;
		if ( lpbfhInfo->ui16BitsPerPixel < 16 ) {
			ui32RowWidth = lpbfhInfo->ui32Width;
		}
		if ( ui32RowWidth & 0x3 ) {
			ui32RowWidth = (ui32RowWidth & ~3) + 4;
		}

		const CFormat::SL2_KTX_INTERNAL_FORMAT_DATA * pkiffFormat = CFormat::FindFormatDataByVulkan( vFormat );
		if ( kifOglFormat != SL2_GL_INVALID ) {
			pkiffFormat = CFormat::FindFormatDataByOgl( kifOglFormat );
		}
		else {
			pkiffFormat = CFormat::FindFormatDataByVulkan( vFormat );
		}
		uint64_t ui64DestRowWidth = CFormat::GetRowSize( pkiffFormat, lpbfhInfo->ui32Width );

		switch ( lpbfhInfo->ui16BitsPerPixel ) {
			case 8 : {
				if ( !AllocateTexture( pkiffFormat,
					lpbfhInfo->ui32Width, ui32Height, 1,
					1, 1, 1 ) ) { return SL2_E_OUTOFMEMORY; }
				Palette().SetFormat( CFormat::FindPaletteFormatData( SL2_GL_PALETTE8_RGB8_OES ) );

				uint32_t ui32ActualOffset = lpbfhHeader->ui32Offset;

				// Pointer to the palette data.
				const SL2_BITMAPPALETTE * lpbpPalette = reinterpret_cast<const SL2_BITMAPPALETTE *>(&_vData.data()[sizeof( SL2_BITMAPFILEHEADER )+sizeof( SL2_BITMAPINFOHEADER )]);
				for ( uint32_t I = 0; I < lpbfhInfo->ui32ColorsInPalette; ++I ) {
					uint32_t ui32B = (lpbpPalette[I].ui32Color >> 0) & 0xFF;
					uint32_t ui32G = (lpbpPalette[I].ui32Color >> 8) & 0xFF;
					uint32_t ui32R = (lpbpPalette[I].ui32Color >> 16) & 0xFF;
					uint32_t ui32A = (lpbpPalette[I].ui32Color >> 24) & 0xFF;
					m_pPalette.Add( CPalette::CColor( ui32R / 255.0, ui32G / 255.0, ui32B / 255.0, ui32A / 255.0 ) );
				}

				// If it is RLE-compressed.
				if ( i32Compression == BI_RLE8 ) {
					return SL2_E_FEATURENOTSUPPORTED;
				}
				else {
					uint32_t ui32EightOverBytes = 8 / ui32BytesPerPixel;
					// Otherwise not RLE compressed.
					for ( uint32_t Y = 0; Y < ui32Height; ++Y ) {
						uint32_t ui32BitIndexBase = Y * lpbfhInfo->ui32Width;
						uint32_t ui32YOff = ((Y * ui32RowWidth * ui32BytesPerPixel) >> 3) + ui32ActualOffset;

						// This line is what handles reverse-encoded bitmaps.
						uint64_t ui64YOffDest = bReverse ? Y * ui64DestRowWidth :
							(ui32Height - Y - 1) * ui64DestRowWidth;

						uint8_t * pui8Dest = &Data()[ui64YOffDest];

						

						for ( uint32_t X = 0; X < lpbfhInfo->ui32Width; ++X ) {
							// Get the byte from which we will read.
							const uint8_t * pui8TargetByte = &_vData.data()[ui32YOff+((X*ui32BytesPerPixel)>>3)];
							// Get the bits within that byte we need.
							uint32_t ui32BitIndex = (ui32BitIndexBase + X) % (ui32EightOverBytes);
							uint32_t ui32Shift = ui32BitIndex * ui32BytesPerPixel;

							// Decode into an index.
							uint32_t ui32Index = ((*pui8TargetByte) & (ui32BitMask << (ui32Shift))) >> ui32Shift;

							// Sanity check.
							if ( ui32Index >= lpbfhInfo->ui32ColorsInPalette ) { return SL2_E_INVALIDFILETYPE; }
							pui8Dest[X] = uint8_t( ui32Index );

							// Get the color from the palette.
							//uint32_t ui32B = (lpbpPalette[ui32Index].ui32Color >> 0) & 0xFF;
							//uint32_t ui32G = (lpbpPalette[ui32Index].ui32Color >> 8) & 0xFF;
							//uint32_t ui32R = (lpbpPalette[ui32Index].ui32Color >> 16) & 0xFF;
							//uint32_t ui32A = (lpbpPalette[ui32Index].ui32Color >> 24) & 0xFF;
							//CFormat::SL2_RGBA_UNORM ruColor;
							//ruColor.ui8Rgba[SL2_PC_R] = static_cast<uint8_t>(ui32R);
							//ruColor.ui8Rgba[SL2_PC_G] = static_cast<uint8_t>(ui32G);
							//ruColor.ui8Rgba[SL2_PC_B] = static_cast<uint8_t>(ui32B);
							//ruColor.ui8Rgba[SL2_PC_A] = static_cast<uint8_t>(ui32A);

							//// Convert to the destination format.
							//uint32_t ui32Final = (*reinterpret_cast<uint32_t *>(&ruColor.ui8Rgba));

							//static const uint32_t ui32Mask[] = {
							//	0xFFFFFFFF,
							//	0xFFFFFF00,
							//	0xFFFF0000,
							//	0xFF000000,
							//	0x00000000
							//};
							//static const uint32_t ui32Sizes[] = {
							//	0x00000000,
							//	0x000000FF,
							//	0x0000FFFF,
							//	0x00FFFFFF,
							//	0xFFFFFFFF
							//};

							//uint32_t * pui32Dst = reinterpret_cast<uint32_t *>(&pui8Dest[X*ui32BytesPerPixelDst]);
							//(*pui32Dst) = ((*pui32Dst) & ui32Mask[ui32BytesPerPixelDst]) | (ui32Final & ui32Sizes[ui32BytesPerPixelDst]);
						}
					}
				}

				break;
			}
			case 16 : {}
			case 24 : {}
			case 32 : {
				uint32_t ui32ActualOffset = lpbfhHeader->ui32Offset;

				// The color masks tell us the order and sizes of the colors in the image.
				const SL2_BITMAPCOLORMASK * lpbcmMask;
				if ( i32Compression == BI_BITFIELDS ) {
					lpbcmMask = reinterpret_cast<const SL2_BITMAPCOLORMASK *>(&_vData.data()[sizeof( SL2_BITMAPFILEHEADER )+sizeof( SL2_BITMAPINFOHEADER )]);
				}
				else {
					static const SL2_BITMAPCOLORMASK bcmDefaultMask32 = {
						0x00FF0000,
						0x0000FF00,
						0x000000FF,
						0xFF000000,
					};
					static const SL2_BITMAPCOLORMASK bcmDefaultMask24 = {
						0x00FF0000,
						0x0000FF00,
						0x000000FF,
						0x00000000,
					};
					static const SL2_BITMAPCOLORMASK bcmDefaultMask16 = {
						0x00007C00,
						0x000003E0,
						0x0000001F,
						0x00000000,
					};
					if ( lpbfhInfo->ui16BitsPerPixel == 16 ) {
						lpbcmMask = &bcmDefaultMask16;
					}
					else if ( lpbfhInfo->ui16BitsPerPixel == 32 ) {
						lpbcmMask = &bcmDefaultMask32;
					}
					else {
						lpbcmMask = &bcmDefaultMask24;
					}
				}
				// Determine how much we need to shift each channel to get the values we expect.
				uint32_t ui32RShift = 0;
				uint32_t ui32GShift = 0;
				uint32_t ui32BShift = 0;
				uint32_t ui32AShift = 0;
				while ( ui32RShift < 32 && !(lpbcmMask->ui32Red & (1 << ui32RShift)) ) { ++ui32RShift; }
				while ( ui32GShift < 32 && !(lpbcmMask->ui32Green & (1 << ui32GShift)) ) { ++ui32GShift; }
				while ( ui32BShift < 32 && !(lpbcmMask->ui32Blue & (1 << ui32BShift)) ) { ++ui32BShift; }
				while ( ui32AShift < 32 && !(lpbcmMask->ui32Alpha & (1 << ui32AShift)) ) { ++ui32AShift; }

				// Also get the number of bits per component.
				uint32_t ui32RBits = 0;
				uint32_t ui32GBits = 0;
				uint32_t ui32BBits = 0;
				uint32_t ui32ABits = 0;
				while ( (lpbcmMask->ui32Red & (1 << (ui32RShift + ui32RBits)) ) ) { ++ui32RBits; }
				while ( (lpbcmMask->ui32Green & (1 << (ui32GShift + ui32GBits)) ) ) { ++ui32GBits; }
				while ( (lpbcmMask->ui32Blue & (1 << (ui32BShift + ui32BBits)) ) ) { ++ui32BBits; }
				while ( (lpbcmMask->ui32Alpha & (1 << (ui32AShift + ui32ABits)) ) ) { ++ui32ABits; }
				

				// It is not too late to change the format as long as it stays the same size.
				// Specifically, here we want to verify/modify the 16-bit format.
				if ( lpbfhInfo->ui16BitsPerPixel == 16 ) {
					// If there is an alpha channel, we need to modify the format to have one
					//	as well.
					if ( ui32ABits ) {
						// If more than one bit is needed for alpha.
						if ( ui32ABits > 1 ) {
							vFormat = SL2_VK_FORMAT_R4G4B4A4_UNORM_PACK16;
						}
						// Otherwise use one bit.
						else {
							vFormat = SL2_VK_FORMAT_R5G5B5A1_UNORM_PACK16;
						}
					}
				}
				pkiffFormat = CFormat::FindFormatDataByVulkan( vFormat );
				if ( !AllocateTexture( pkiffFormat,
					lpbfhInfo->ui32Width, ui32Height, 1,
					1, 1, 1 ) ) { return SL2_E_OUTOFMEMORY; }

				for ( uint32_t Y = 0; Y < ui32Height; ++Y ) {
					uint32_t ui32YOffSrc = Y * ui32RowWidth + ui32ActualOffset;
					// This line is what handles reverse-encoded bitmaps.
					uint64_t ui64YOffDest = bReverse ? Y * ui64DestRowWidth :
						(ui32Height - Y - 1) * ui64DestRowWidth;

					// We can copy whole rows at a time if they are in the same format already.
					const uint8_t * pui8Src = &_vData.data()[ui32YOffSrc];
					uint8_t * pui8Dest = &Data()[ui64YOffDest];

					// Since bitmaps store the alpha on the reverse side, we cannot use this trick
					//	when there is an alpha channel.  In the end, it turns out that this only
					//	works on 24-bit bitmaps.  But that is not so bad since they are actually
					//	fairly common.
					if ( i32Compression == BI_RGB &&
						ui32RShift == offsetof( CFormat::SL2_RGBA_UNORM, ui8Rgba[SL2_PC_R] ) &&
						ui32GShift == offsetof( CFormat::SL2_RGBA_UNORM, ui8Rgba[SL2_PC_G] ) &&
						ui32BShift == offsetof( CFormat::SL2_RGBA_UNORM, ui8Rgba[SL2_PC_B] ) &&
						(ui32AShift == 32 || ui32AShift == offsetof( CFormat::SL2_RGBA_UNORM, ui8Rgba[SL2_PC_A] )) ) {
						std::memcpy( pui8Dest, pui8Src, lpbfhInfo->ui32Width * ui32BytesPerPixel );
					}
					else {
						for ( uint32_t X = 0; X < lpbfhInfo->ui32Width; ++X ) {
							uint32_t ui32Src = (*reinterpret_cast<const uint32_t *>(&pui8Src[X*ui32BytesPerPixel]));

							uint32_t ui32R = (ui32Src & lpbcmMask->ui32Red) >> ui32RShift;
							uint32_t ui32G = (ui32Src & lpbcmMask->ui32Green) >> ui32GShift;
							uint32_t ui32B = (ui32Src & lpbcmMask->ui32Blue) >> ui32BShift;
							uint32_t ui32A = (ui32Src & lpbcmMask->ui32Alpha) >> ui32AShift;
							// Correct the missing alpha channel.
							if ( ui32AShift == 32 ) { ui32A = 0xFF; }
							switch ( vFormat ) {
								case SL2_VK_FORMAT_R5G6B5_UNORM_PACK16 : {
									CFormat::SL2_R5G6B5_PACKED * prgbDst = reinterpret_cast<CFormat::SL2_R5G6B5_PACKED *>(&pui8Dest[X*ui32BytesPerPixelDst]);
									prgbDst->ui16R = static_cast<uint8_t>(std::round( ui32R / ((1ULL << ui32RBits) - 1.0) * ((1ULL << 5) - 1) ));
									prgbDst->ui16G = static_cast<uint8_t>(std::round( ui32G / ((1ULL << ui32GBits) - 1.0) * ((1ULL << 6) - 1) ));
									prgbDst->ui16B = static_cast<uint8_t>(std::round( ui32B / ((1ULL << ui32BBits) - 1.0) * ((1ULL << 5) - 1) ));
									break;
								}
								case SL2_VK_FORMAT_R5G5B5A1_UNORM_PACK16 : {
									CFormat::SL2_A1R5G6B5_PACKED * prgbDst = reinterpret_cast<CFormat::SL2_A1R5G6B5_PACKED *>(&pui8Dest[X*ui32BytesPerPixelDst]);
									prgbDst->ui16R = static_cast<uint8_t>(std::round( ui32R / ((1ULL << ui32RBits) - 1.0) * ((1ULL << 5) - 1) ));
									prgbDst->ui16G = static_cast<uint8_t>(std::round( ui32G / ((1ULL << ui32GBits) - 1.0) * ((1ULL << 5) - 1) ));
									prgbDst->ui16B = static_cast<uint8_t>(std::round( ui32B / ((1ULL << ui32BBits) - 1.0) * ((1ULL << 5) - 1) ));
									prgbDst->ui16A = static_cast<uint8_t>(std::round( ui32A / ((1ULL << ui32ABits) - 1.0) * ((1ULL << 1) - 1) ));
									break;
								}
								case SL2_VK_FORMAT_R4G4B4A4_UNORM_PACK16 : {
									CFormat::SL2_RGBA4_PACKED * prgbDst = reinterpret_cast<CFormat::SL2_RGBA4_PACKED *>(&pui8Dest[X*ui32BytesPerPixelDst]);
									prgbDst->ui16R = static_cast<uint8_t>(std::round( ui32R / ((1ULL << ui32RBits) - 1.0) * ((1ULL << 4) - 1) ));
									prgbDst->ui16G = static_cast<uint8_t>(std::round( ui32G / ((1ULL << ui32GBits) - 1.0) * ((1ULL << 4) - 1) ));
									prgbDst->ui16B = static_cast<uint8_t>(std::round( ui32B / ((1ULL << ui32BBits) - 1.0) * ((1ULL << 4) - 1) ));
									prgbDst->ui16A = static_cast<uint8_t>(std::round( ui32A / ((1ULL << ui32ABits) - 1.0) * ((1ULL << 4) - 1) ));
									break;
								}
								case SL2_VK_FORMAT_R8G8B8_UNORM : {
									CFormat::SL2_RGB_UNORM * prgbDst = reinterpret_cast<CFormat::SL2_RGB_UNORM *>(&pui8Dest[X*ui32BytesPerPixelDst]);
									prgbDst->ui8Rgb[SL2_PC_R] = ui32R;
									prgbDst->ui8Rgb[SL2_PC_G] = ui32G;
									prgbDst->ui8Rgb[SL2_PC_B] = ui32B;
									break;
								}
								case SL2_VK_FORMAT_R8G8B8A8_UNORM : {
									CFormat::SL2_RGBA_UNORM * prgbDst = reinterpret_cast<CFormat::SL2_RGBA_UNORM *>(&pui8Dest[X*ui32BytesPerPixelDst]);
									prgbDst->ui8Rgba[SL2_PC_R] = ui32R;
									prgbDst->ui8Rgba[SL2_PC_G] = ui32G;
									prgbDst->ui8Rgba[SL2_PC_B] = ui32B;
									prgbDst->ui8Rgba[SL2_PC_A] = ui32A;
									break;
								}
							}

						}
					}
				}
				break;
			}
		}

		return SL2_E_SUCCESS;
	}

	/**
	 * Loads a Phoenix BMP file from memory.
	 * 
	 * \param _vData The file to load.
	 * \return Returns an error code.
	 **/
	SL2_ERRORS CImage::LoadPBmp( const std::vector<uint8_t> &_vData ) {
		CStream sFile( _vData );


		if ( _vData.size() < sizeof( SL2_CHUNK ) ) { return SL2_E_INVALIDFILETYPE; }
		SL2_CHUNK_EX cFile;
		if ( !sFile.Read( cFile ) ) { return SL2_E_INVALIDFILETYPE; }
		if ( cFile.ui32Name != 0x504D4250 ) { return SL2_E_INVALIDFILETYPE; }														// PBMP.
		if ( cFile.ui32Size > _vData.size() ) { return SL2_E_INVALIDFILETYPE; }


		const SL2_HEAD_CHUNK * pcHeader = NULL;
		const SL2_CHUNK_EX * pcData = NULL;
		const SL2_DETL_CHUNK * pcDetl = NULL;
		const SL2_PIDX_CHUNK * pcPidX = NULL;
		const SL2_CHUNK_EX * pcRiff = NULL;
		bool bContinue = true;
		while ( bContinue ) {
			const SL2_CHUNK * pcThis = reinterpret_cast<const SL2_CHUNK *>(_vData.data() + sFile.Pos());
			if ( (sizeof( SL2_CHUNK ) + pcThis->ui32Size) > (_vData.size() - sFile.Pos()) ) { return SL2_E_INVALIDFILETYPE; }
			switch ( pcThis->ui32Name ) {
				case 0x64616568 : {																					// head.
					pcHeader = reinterpret_cast<const SL2_HEAD_CHUNK *>(_vData.data() + sFile.Pos());
					break;
				}
				case 0x61746164 : {																					// data.
					pcData = reinterpret_cast<const SL2_CHUNK_EX *>(_vData.data() + sFile.Pos());
					break;
				}
				case 0x4C544544 : {																					// DETL.
					pcDetl = reinterpret_cast<const SL2_DETL_CHUNK *>(_vData.data() + sFile.Pos());
					break;
				}
				case 0x58446950 : {																					// PiDX.
					pcPidX = reinterpret_cast<const SL2_PIDX_CHUNK *>(_vData.data() + sFile.Pos());
					break;
				}
				default : {
					bContinue = false;
					break;
				}
			}
			if ( !sFile.Read( nullptr, sizeof( SL2_CHUNK ) + pcThis->ui32Size ) ) { return SL2_E_INVALIDFILETYPE; }
		}


		if ( pcHeader == NULL ) { return SL2_E_INVALIDFILETYPE; }
		if ( pcDetl == NULL ) { return SL2_E_INVALIDFILETYPE; }
		if ( pcPidX == NULL ) { return SL2_E_INVALIDFILETYPE; }
		if ( pcData == NULL ) { return SL2_E_INVALIDFILETYPE; }


		const CFormat::SL2_KTX_INTERNAL_FORMAT_DATA * pkifdFormat = nullptr;
		switch ( pcHeader->ui32BitDepth ) {
			case 1 : {
				pkifdFormat = CFormat::FindFormatDataByOgl( SL2_GL_COLOR_INDEX1_EXT );
				break;
			}
			case 2 : {
				pkifdFormat = CFormat::FindFormatDataByOgl( SL2_GL_COLOR_INDEX2_EXT );
				break;
			}
			case 4 : {
				pkifdFormat = CFormat::FindFormatDataByOgl( SL2_GL_COLOR_INDEX4_EXT );
				break;
			}
			case 8 : {
				pkifdFormat = CFormat::FindFormatDataByOgl( SL2_GL_COLOR_INDEX8_EXT );
				break;
			}
			default : { return SL2_E_INVALIDFILETYPE; }
		}

		uint32_t ui32Mips = 1;
		if ( pcDetl->ui32MipLevels == 0 ) { return SL2_E_INVALIDFILETYPE; }
		if ( pcDetl->ui32MipLevels > 1 ) {
			uint32_t ui32MaxMipsW = static_cast<uint32_t>(std::floor( std::log2( pcHeader->ui32Width ) + 1.0 ));
			uint32_t ui32MaxMipsH = static_cast<uint32_t>(std::floor( std::log2( pcHeader->ui32Height ) + 1.0 ));
			ui32Mips = std::min( std::min( ui32MaxMipsW, ui32MaxMipsH ), pcDetl->ui32MipLevels );
			//if ( !_vMipMaps.Resize( ui32Mips - 1 ) ) { return false; }
		}

		if ( !AllocateTexture( pkifdFormat,
			pcHeader->ui32Width, pcHeader->ui32Height, 1,
			ui32Mips, 1, 1 ) ) { return SL2_E_OUTOFMEMORY; }
		//Palette().SetFormat( CFormat::FindPaletteFormatData( SL2_GL_PALETTE8_RGB8_OES ) );


		const uint8_t * pui8Src = pcData->ui8Data;
		uint32_t ui32SizeRem = pcData->ui32Size;
		for ( uint32_t I = 0; I < ui32Mips; ++I ) {
			uint32_t ui32W = pcHeader->ui32Width >> I;
			if ( !ui32W ) { return SL2_E_INVALIDFILETYPE; }
			uint32_t ui32H = pcHeader->ui32Height >> I;
			if ( !ui32H ) { return SL2_E_INVALIDFILETYPE; }
			uint32_t ui32ThisSize = ((ui32W * ui32H * pcHeader->ui32BitDepth >> 3) + 3) & ~3;

			auto pui8Dst = Data( I );
			if ( ui32SizeRem < ui32ThisSize ) { return SL2_E_INVALIDFILETYPE; }

			uint64_t ui64DestRowWidth = CFormat::GetRowSize( Format(), ui32W );

			constexpr bool bReverse = true;
			switch ( pcHeader->ui32BitDepth ) {
				case 8 : {
					for ( uint32_t Y = 0; Y < ui32H; ++Y ) {
						uint32_t ui32YOff = Y * ui32W;

						// This line is what handles reverse-encoded bitmaps.
						uint64_t ui64YOffDest = bReverse ? Y * ui64DestRowWidth :
							(ui32H - Y - 1) * ui64DestRowWidth;

						uint8_t * pui8Dest = &pui8Dst[ui64YOffDest];
						for ( uint32_t X = 0; X < ui32W; ++X ) {
							pui8Dest[X] = pui8Src[ui32YOff+X];
						}
					}
					break;
				}
				default : { return SL2_E_INVALIDFILETYPE; }
			}


			pui8Src += ui32ThisSize;
			ui32SizeRem -= ui32ThisSize;
		}
		return SL2_E_SUCCESS;
	}

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
	::KTX_error_code CImage::KtxImageLoad( int _iMipLevel, int _iFace,
		int _iWidth, int _iHeight, int _iDepth,
		ktx_uint64_t _ui64FaceLodSize,
		void * _pvPixels, void * _pvUserdata ) {

		CImage * piImage = reinterpret_cast<CImage *>(_pvUserdata);
		uint64_t ui64SrcSize = CFormat::GetFormatSize( piImage->Format(), _iWidth, _iHeight, _iDepth );
		_ui64FaceLodSize /= piImage->ArraySize();
		if ( _ui64FaceLodSize < ui64SrcSize || 0 == ui64SrcSize || (uint64_t( size_t( ui64SrcSize ) ) != ui64SrcSize) ) {
			return ::KTX_FILE_READ_ERROR;
		}
		const uint8_t * pui8Src = reinterpret_cast<uint8_t *>(_pvPixels);
		for ( size_t I = 0; I < piImage->ArraySize(); ++I ) {
			uint8_t * pui8Dst = piImage->Data( _iMipLevel, 0, I, _iFace );
			std::memcpy( pui8Dst, pui8Src + _ui64FaceLodSize * I, size_t( ui64SrcSize ) );
		}
		return ::KTX_SUCCESS;
	}

}	// namespace sl2

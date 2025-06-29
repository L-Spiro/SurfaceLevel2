/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Let's convert and modify textures!
 */

#ifndef SL2_LIB

#pragma once

#include "Files/SL2StdFile.h"
#include "Image/PVRTexTool/PVRTexLib.hpp"
#include "Image/SL2Formats.h"
#include "Image/SL2Image.h"
#include <string>
#include <vector>


namespace sl2 {

	struct SL2_OPEN_FILE {
		std::u16string													u16Path;														/**< Path to the file to open. */
		const CFormat::SL2_KTX_INTERNAL_FORMAT_DATA *					pkifduvFormat = nullptr;										/**< The YUV format. */
		uint32_t														ui32YuvW = 0;													/**< The YUV file width. */
		uint32_t														ui32YuvH = 0;													/**< The YUV file height. */
		bool															bFromClipBoard = false;											/**< If true, the file is loaded from the clipboard instead of from a file. */
	};

	// == Types.
	/** The conversion options. */
	struct SL2_OPTIONS {
		std::vector<SL2_OPEN_FILE>										vInputs;														/**< The input files. */
		std::vector<std::u16string>										vOutputs;														/**< The output files. */
		const sl2::CFormat::SL2_KTX_INTERNAL_FORMAT_DATA *				pkifdFinalFormat = nullptr;										/**< Actual final format.  If not manually specified, vfAutoFormat is used. */
		sl2::CFormat::SL2_SWIZZLE										sSwizzle = CFormat::DefaultSwizzle();							/**< The swizzle to apply. */
		double															dGamma = -2.2;													/**< User-supplied gamma. */
		double															dTargetGamma = -2.2;											/**< User-supplied target gamma. */
		bool															bManuallySetGamma = false;										/**< Have we used -g or -gamma or -srgb or _rgbe? */
		bool															bManuallySetTargetGamma = false;								/**< Have we manually set the target gamma? */
		SL2_COLORSPACE_GAMMA_CURVES										cgcInputGammaCurve = SL2_CGC_NONE;								/**< The input gamma curve. */
		SL2_COLORSPACE_GAMMA_CURVES										cgcOutputGammaCurve = SL2_CGC_sRGB_PRECISE;						/**< The output gamma curve. */
		std::vector<uint8_t>											vInColorProfile;												/**< The input color profile. */
		std::vector<uint8_t>											vOutColorProfile;												/**< The output color profile. */
		int32_t															i32InRenderingIntent = INTENT_RELATIVE_COLORIMETRIC;			/**< Input -> linear rendering intent. */
		int32_t															i32OutRenderingIntent = INTENT_RELATIVE_COLORIMETRIC;			/**< Input -> linear rendering intent. */
		bool															bEmbedColorProfile = true;										/**< Embed the color profile. */
		bool															bIgnoreSourceColorspaceGamma = false;							/**< Ignores the gamma curve inside any embedded or selected ICC profiles. */

		sl2::CResampler::SL2_RESAMPLE									rResample;														/**< Resampling parameters. */
		sl2::CResampler::SL2_RESAMPLE									rMipResample;													/**< Mipmap resampling parameters. */
		sl2::SL2_RESAMPLE_TO											rtResampleTo = SL2_RT_NONE;										/**< Resample size for -rescale. */
		double															dRelScaleW = 1.0;												/**< Relative width scale. */
		double															dRelScaleH = 1.0;												/**< Relative height scale. */
		double															dRelScaleD = 1.0;												/**< Relative depth scale. */
		int32_t															i32ScaleDims = 2;												/**< Which scaling dimension was specified (prescale or prescale3). */
		uint32_t														ui32FitW = 0;													/**< The fit resmapling width. */
		uint32_t														ui32FitH = 0;													/**< The fit resmapling height. */
		uint32_t														ui32FitD = 0;													/**< The fit resmapling depth. */

		CResampler::SL2_FILTER_FUNCS									fFilterFuncW = CResampler::SL2_FF_QUADRATICSHARP;				/**< The width filter. */
		CResampler::SL2_FILTER_FUNCS									fFilterFuncH = CResampler::SL2_FF_QUADRATICSHARP;				/**< The height filter. */
		CResampler::SL2_FILTER_FUNCS									fFilterFuncD = CResampler::SL2_FF_QUADRATICSHARP;				/**< The depth filter. */
		CResampler::SL2_FILTER_FUNCS									fAlphaFilterFuncW = CResampler::SL2_FF_QUADRATICSHARP;			/**< The width alpha-channel filter. */
		CResampler::SL2_FILTER_FUNCS									fAlphaFilterFuncH = CResampler::SL2_FF_QUADRATICSHARP;			/**< The height alpha-channel filter. */
		CResampler::SL2_FILTER_FUNCS									fAlphaFilterFuncD = CResampler::SL2_FF_QUADRATICSHARP;			/**< The depth alpha-channel filter. */

		CResampler::SL2_FILTER_FUNCS									fMipFilterFuncW = CResampler::SL2_FF_CARDINALSPLINEUNIFORM;		/**< The width filter. */
		CResampler::SL2_FILTER_FUNCS									fMipFilterFuncH = CResampler::SL2_FF_CARDINALSPLINEUNIFORM;		/**< The height filter. */
		CResampler::SL2_FILTER_FUNCS									fMipFilterFuncD = CResampler::SL2_FF_CARDINALSPLINEUNIFORM;		/**< The depth filter. */
		CResampler::SL2_FILTER_FUNCS									fMipAlphaFilterFuncW = CResampler::SL2_FF_LINEAR;				/**< The width alpha-channel filter. */
		CResampler::SL2_FILTER_FUNCS									fMipAlphaFilterFuncH = CResampler::SL2_FF_LINEAR;				/**< The height alpha-channel filter. */
		CResampler::SL2_FILTER_FUNCS									fMipAlphaFilterFuncD = CResampler::SL2_FF_LINEAR;				/**< The depth alpha-channel filter. */

		uint32_t														ui32ClampW = 0;													/**< Width clamp. */
		uint32_t														ui32ClampH = 0;													/**< Height clamp. */
		uint32_t														ui32ClampD = 0;													/**< Depth clamp. */

		SL2_MIPMAP_HANDLING												mhMipHandling = SL2_MH_GENERATE_NEW;							/**< Fully generate a new set. */
		size_t															sTotalMips = 0;													/**< How many mipmaps to put into the final result, or 0 to keep existing mipmaps or to generate a full set. */

		CKernel															kKernel;														/**< Normal-map Sobel kernel. */
		uint32_t														ui32NormalKernelSize = 0;										/**< Normal-map generation kernel size. */
		SL2_CHANNEL_ACCESS												caChannelAccess = SL2_CA_MAX;									/**< Normal-map channel access. */
		double															dNormalScale = 0.35;											/**< Normal-map scalar. */
		double															dNormalYAxis = 1.0;												/**< Normal map Y axis.  1.0 for OpenGL, -1.0 for DirectX. */
		bool															bNormalizeMips = false;											/**< If mipmaps should be normalized or not. */

		bool															bGenNewPalatte = false;											/**< Generate a new palette (applies only when there is an existing palette). */

		sl2::CImage::SL2_WINDOW											wCropWindow;													/**< The cropping window. */
		uint32_t														ui32BakedW = 0;													/**< Number of baked horizontal iterations. */
		uint32_t														ui32BakedH = 0;													/**< Number of baked vertical iterations. */
		uint32_t														ui32BakedD = 0;													/**< Number of baked depth iterations. */
		sl2::SL2_QUICK_ROTATION											qrQuickRot = sl2::SL2_QR_ROT_0;									/**< Quick rotation. */

		int																iPngSaveOption = PNG_Z_DEFAULT_COMPRESSION;						/**< Option for saving as PNG. */
		const CFormat::SL2_KTX_INTERNAL_FORMAT_DATA *					pkifdPngFormat = nullptr;										/**< The PNG format. */

		sl2::SL2_VKFORMAT												vkBmpFormat = SL2_VK_FORMAT_UNDEFINED;							/**< The BMP format. */
		sl2::SL2_VKFORMAT												vkBmpFormatNoMask = SL2_VK_FORMAT_UNDEFINED;					/**< The BMP format when not using a mask. */
		int																iBmpSaveOption = BMP_DEFAULT;									/**< Option for saving as BMP. */
		bool															bBmpHasAlpha = true;											/**< Does the BMP file have alpha? */
		bool															bIgnoreAlpha = false;											/**< Ignore alpha? */
		bool															bBmpStoreBitmask = true;										/**< Try to store the bitmask? */

		sl2::SL2_VKFORMAT												vkExrFormat = SL2_VK_FORMAT_UNDEFINED;							/**< The EXR format. */
		int																iExrSaveOption = EXR_DEFAULT;									/**< Options for saving as EXR. */

		sl2::SL2_VKFORMAT												vkJ2kFormat = SL2_VK_FORMAT_UNDEFINED;							/**< The J2K format. */
		int																iJ2kSaveOption = J2K_DEFAULT;									/**< J2K compression amount. */

		sl2::SL2_VKFORMAT												vkJp2Format = SL2_VK_FORMAT_UNDEFINED;							/**< The JP2 format. */
		int																iJp2SaveOption = JP2_DEFAULT;									/**< JP2 compression amount. */

		int																iJpgSaveOption = JPEG_DEFAULT;									/**< JPG compression amount. */

		sl2::SL2_VKFORMAT												vkTgaFormat = SL2_VK_FORMAT_UNDEFINED;							/**< The TGA format. */
		int																iTgaSaveOption = TARGA_DEFAULT;									/**< TGA option. */

		const CFormat::SL2_KTX_INTERNAL_FORMAT_DATA *					pkifdYuvFormat = nullptr;										/**< The YUV format. */

		int																iPbmSaveOption = PNM_DEFAULT;									/**< Option for saving as PBM. */

		int																iPgmSaveOption = PNM_DEFAULT;									/**< Option for saving as PGM. */

		int																iPpmSaveOption = PNM_DEFAULT;									/**< Option for saving as PPM. */

		const CFormat::SL2_KTX_INTERNAL_FORMAT_DATA *					pkifdIcoFormat = nullptr;										/**< The ICO format. */
		int																iIcoSaveOption = ICO_DEFAULT;									/**< Option for saving as PPM. */

		bool															bNeedsPreMultiply = false;										/**< Does the target format, or user request, demand pre-multiplied alpha? */
		bool															bSwap = false;													/**< Swap R and B? */
		bool															bFlipX = false;													/**< Horizontal flip? */
		bool															bFlipY = false;													/**< Vertical flip? */
		bool															bFlipZ = false;													/**< Depth flip? */
		bool															bPause = false;													/**< If true, the program pauses before closing the command window. */
		bool															bShowTime = true;												/**< If true, the time taken to perform the conversion is printed. */
		
	};


	// == Functions.
	/**
	 * Returns a string representing the given error code.
	 * 
	 * \param _eError The error code to print to a string.
	 * \return Returns a string representing the given error code.
	 **/
	std::u16string														ErrorToString( SL2_ERRORS _eError );

	/**
	 * Prints a given error code to the console.
	 * 
	 * \param _eError The error code to print.
	 **/
	void																PrintError( SL2_ERRORS _eError );

	/**
	 * Prints a given error code to the console.
	 * 
	 * \param _pcText Text to print.
	 * \param _eError The error code to print.
	 **/
	void																PrintError( const char16_t * _pcText, SL2_ERRORS _eError );

	/**
	 * Fix up the resampling parameters.
	 * 
	 * \param _oOptions The options to fix up.
	 * \param _iImage The image off of which to base the adjustments.
	 **/
	void																FixResampling( SL2_OPTIONS &_oOptions, CImage &_iImage );

	/**
	 * Exports as PNG.
	 * 
	 * \param _iImage The image to export.
	 * \param _sPath The path to which to export _iImage.
	 * \param _oOptions Export options.
	 * \return Returns an error code.
	 **/
	SL2_ERRORS															ExportAsPng( CImage &_iImage, const std::u16string &_sPath, SL2_OPTIONS &_oOptions );

	/**
	 * Exports as PNG.
	 * 
	 * \param _iImage The image to export.
	 * \param _sPath The path to which to export _iImage.
	 * \param _oOptions Export options.
	 * \param _sMip The mipmap level to export.
	 * \param _sArray The array index to export.
	 * \param _sFace The face to export.
	 * \param _sSlice The slice to export.
	 * \return Returns an error code.
	 **/
	SL2_ERRORS															ExportAsPng( CImage &_iImage, const std::u16string &_sPath, SL2_OPTIONS &_oOptions, size_t _sMip, size_t _sArray, size_t _sFace, size_t _sSlice );

	/**
	 * Exports as PNG.
	 * 
	 * \param _iImage The image to export.
	 * \param _sPath The path to which to export _iImage.
	 * \param _oOptions Export options.
	 * \param _sMip The mipmap level to export.
	 * \param _sArray The array index to export.
	 * \param _sFace The face to export.
	 * \param _sSlice The slice to export.
	 * \param _pbifFormat The target indexed format.
	 * \return Returns an error code.
	 **/
	SL2_ERRORS															ExportAsPng_Indexed( CImage &_iImage, const std::u16string &_sPath, SL2_OPTIONS &_oOptions, size_t _sMip, size_t _sArray, size_t _sFace, size_t _sSlice,
		const CFormat::SL2_BEST_INTERNAL_FORMAT * _pbifFormat );

	/**
	 * Exports as BMP.
	 * 
	 * \param _iImage The image to export.
	 * \param _sPath The path to which to export _iImage.
	 * \param _oOptions Export options.
	 * \return Returns an error code.
	 **/
	SL2_ERRORS															ExportAsBmp( CImage &_iImage, const std::u16string &_sPath, SL2_OPTIONS &_oOptions );

	/**
	 * Exports as BMP.
	 * 
	 * \param _iImage The image to export.
	 * \param _sPath The path to which to export _iImage.
	 * \param _oOptions Export options.
	 * \param _sMip The mipmap level to export.
	 * \param _sArray The array index to export.
	 * \param _sFace The face to export.
	 * \param _sSlice The slice to export.
	 * \return Returns an error code.
	 **/
	SL2_ERRORS															ExportAsBmp( CImage &_iImage, const std::u16string &_sPath, SL2_OPTIONS &_oOptions, size_t _sMip, size_t _sArray, size_t _sFace, size_t _sSlice );

	/**
	 * Exports as BMP.
	 * 
	 * \param _iImage The image to export.
	 * \param _sPath The path to which to export _iImage.
	 * \param _oOptions Export options.
	 * \param _sMip The mipmap level to export.
	 * \param _sArray The array index to export.
	 * \param _sFace The face to export.
	 * \param _sSlice The slice to export.
	 * \return Returns an error code.
	 **/
	SL2_ERRORS															ExportAsBmpWithMasks( CImage &_iImage, const std::u16string &_sPath, SL2_OPTIONS &_oOptions, size_t _sMip, size_t _sArray, size_t _sFace, size_t _sSlice );

	/**
	 * Exports as EXR.
	 * 
	 * \param _iImage The image to export.
	 * \param _sPath The path to which to export _iImage.
	 * \param _oOptions Export options.
	 * \return Returns an error code.
	 **/
	SL2_ERRORS															ExportAsExr( CImage &_iImage, const std::u16string &_sPath, SL2_OPTIONS &_oOptions );

	/**
	 * Exports as EXR.
	 * 
	 * \param _iImage The image to export.
	 * \param _sPath The path to which to export _iImage.
	 * \param _oOptions Export options.
	 * \param _sMip The mipmap level to export.
	 * \param _sArray The array index to export.
	 * \param _sFace The face to export.
	 * \param _sSlice The slice to export.
	 * \return Returns an error code.
	 **/
	SL2_ERRORS															ExportAsExr( CImage &_iImage, const std::u16string &_sPath, SL2_OPTIONS &_oOptions, size_t _sMip, size_t _sArray, size_t _sFace, size_t _sSlice );

	/**
	 * Exports as J2K.
	 * 
	 * \param _iImage The image to export.
	 * \param _sPath The path to which to export _iImage.
	 * \param _oOptions Export options.
	 * \return Returns an error code.
	 **/
	SL2_ERRORS															ExportAsJ2k( CImage &_iImage, const std::u16string &_sPath, SL2_OPTIONS &_oOptions );

	/**
	 * Exports as J2K.
	 * 
	 * \param _iImage The image to export.
	 * \param _sPath The path to which to export _iImage.
	 * \param _oOptions Export options.
	 * \param _sMip The mipmap level to export.
	 * \param _sArray The array index to export.
	 * \param _sFace The face to export.
	 * \param _sSlice The slice to export.
	 * \return Returns an error code.
	 **/
	SL2_ERRORS															ExportAsJ2k( CImage &_iImage, const std::u16string &_sPath, SL2_OPTIONS &_oOptions, size_t _sMip, size_t _sArray, size_t _sFace, size_t _sSlice );

	/**
	 * Exports as JP2.
	 * 
	 * \param _iImage The image to export.
	 * \param _sPath The path to which to export _iImage.
	 * \param _oOptions Export options.
	 * \return Returns an error code.
	 **/
	SL2_ERRORS															ExportAsJp2( CImage &_iImage, const std::u16string &_sPath, SL2_OPTIONS &_oOptions );

	/**
	 * Exports as JP2.
	 * 
	 * \param _iImage The image to export.
	 * \param _sPath The path to which to export _iImage.
	 * \param _oOptions Export options.
	 * \param _sMip The mipmap level to export.
	 * \param _sArray The array index to export.
	 * \param _sFace The face to export.
	 * \param _sSlice The slice to export.
	 * \return Returns an error code.
	 **/
	SL2_ERRORS															ExportAsJp2( CImage &_iImage, const std::u16string &_sPath, SL2_OPTIONS &_oOptions, size_t _sMip, size_t _sArray, size_t _sFace, size_t _sSlice );

	/**
	 * Exports as JPG.
	 * 
	 * \param _iImage The image to export.
	 * \param _sPath The path to which to export _iImage.
	 * \param _oOptions Export options.
	 * \return Returns an error code.
	 **/
	SL2_ERRORS															ExportAsJpg( CImage &_iImage, const std::u16string &_sPath, SL2_OPTIONS &_oOptions );

	/**
	 * Exports as JPG.
	 * 
	 * \param _iImage The image to export.
	 * \param _sPath The path to which to export _iImage.
	 * \param _oOptions Export options.
	 * \param _sMip The mipmap level to export.
	 * \param _sArray The array index to export.
	 * \param _sFace The face to export.
	 * \param _sSlice The slice to export.
	 * \return Returns an error code.
	 **/
	SL2_ERRORS															ExportAsJpg( CImage &_iImage, const std::u16string &_sPath, SL2_OPTIONS &_oOptions, size_t _sMip, size_t _sArray, size_t _sFace, size_t _sSlice );

	/**
	 * Exports as DDS.
	 * 
	 * \param _iImage The image to export.
	 * \param _sPath The path to which to export _iImage.
	 * \param _oOptions Export options.
	 * \return Returns an error code.
	 **/
	SL2_ERRORS															ExportAsDds( CImage &_iImage, const std::u16string &_sPath, SL2_OPTIONS &_oOptions );

	/**
	 * Exports as KTX 1.
	 * 
	 * \param _iImage The image to export.
	 * \param _sPath The path to which to export _iImage.
	 * \param _oOptions Export options.
	 * \return Returns an error code.
	 **/
	SL2_ERRORS															ExportAsKtx1( CImage &_iImage, const std::u16string &_sPath, SL2_OPTIONS &_oOptions );

	/**
	 * Exports as PVR.
	 * 
	 * \param _iImage The image to export.
	 * \param _sPath The path to which to export _iImage.
	 * \param _oOptions Export options.
	 * \return Returns an error code.
	 **/
	SL2_ERRORS															ExportAsPvr( CImage &_iImage, const std::u16string &_sPath, SL2_OPTIONS &_oOptions );

	/**
	 * Exports as TGA.
	 * 
	 * \param _iImage The image to export.
	 * \param _sPath The path to which to export _iImage.
	 * \param _oOptions Export options.
	 * \return Returns an error code.
	 **/
	SL2_ERRORS															ExportAsTga( CImage &_iImage, const std::u16string &_sPath, SL2_OPTIONS &_oOptions );

	/**
	 * Exports as TGA.
	 * 
	 * \param _iImage The image to export.
	 * \param _sPath The path to which to export _iImage.
	 * \param _oOptions Export options.
	 * \param _sMip The mipmap level to export.
	 * \param _sArray The array index to export.
	 * \param _sFace The face to export.
	 * \param _sSlice The slice to export.
	 * \return Returns an error code.
	 **/
	SL2_ERRORS															ExportAsTga( CImage &_iImage, const std::u16string &_sPath, SL2_OPTIONS &_oOptions, size_t _sMip, size_t _sArray, size_t _sFace, size_t _sSlice );

	/**
	 * Exports as YUV.
	 * 
	 * \param _iImage The image to export.
	 * \param _sPath The path to which to export _iImage.
	 * \param _oOptions Export options.
	 * \return Returns an error code.
	 **/
	SL2_ERRORS															ExportAsYuv( CImage &_iImage, const std::u16string &_sPath, SL2_OPTIONS &_oOptions );

	/**
	 * Exports as YUV.
	 * 
	 * \param _iImage The image to export.
	 * \param _sPath The path to which to export _iImage.
	 * \param _oOptions Export options.
	 * \param _sMip The mipmap level to export.
	 * \param _sArray The array index to export.
	 * \param _sFace The face to export.
	 * \param _sSlice The slice to export.
	 * \return Returns an error code.
	 **/
	SL2_ERRORS															ExportAsYuv( CImage &_iImage, const std::u16string &_sPath, SL2_OPTIONS &_oOptions, size_t _sMip, size_t _sArray, size_t _sFace, size_t _sSlice );

	/**
	 * Exports as PBM.
	 * 
	 * \param _iImage The image to export.
	 * \param _sPath The path to which to export _iImage.
	 * \param _oOptions Export options.
	 * \return Returns an error code.
	 **/
	SL2_ERRORS															ExportAsPbm( CImage &_iImage, const std::u16string &_sPath, SL2_OPTIONS &_oOptions );

	/**
	 * Exports as PBM.
	 * 
	 * \param _iImage The image to export.
	 * \param _sPath The path to which to export _iImage.
	 * \param _oOptions Export options.
	 * \param _sMip The mipmap level to export.
	 * \param _sArray The array index to export.
	 * \param _sFace The face to export.
	 * \param _sSlice The slice to export.
	 * \return Returns an error code.
	 **/
	SL2_ERRORS															ExportAsPbm( CImage &_iImage, const std::u16string &_sPath, SL2_OPTIONS &_oOptions, size_t _sMip, size_t _sArray, size_t _sFace, size_t _sSlice );

	/**
	 * Exports as PGM.
	 * 
	 * \param _iImage The image to export.
	 * \param _sPath The path to which to export _iImage.
	 * \param _oOptions Export options.
	 * \return Returns an error code.
	 **/
	SL2_ERRORS															ExportAsPgm( CImage &_iImage, const std::u16string &_sPath, SL2_OPTIONS &_oOptions );

	/**
	 * Exports as PGM.
	 * 
	 * \param _iImage The image to export.
	 * \param _sPath The path to which to export _iImage.
	 * \param _oOptions Export options.
	 * \param _sMip The mipmap level to export.
	 * \param _sArray The array index to export.
	 * \param _sFace The face to export.
	 * \param _sSlice The slice to export.
	 * \return Returns an error code.
	 **/
	SL2_ERRORS															ExportAsPgm( CImage &_iImage, const std::u16string &_sPath, SL2_OPTIONS &_oOptions, size_t _sMip, size_t _sArray, size_t _sFace, size_t _sSlice );

	/**
	 * Export as ICO.
	 * 
	 * \param _iImage The image to export.
	 * \param _sPath The path to which to export _iImage.
	 * \param _oOptions Export options.
	 * \return Returns an error code.
	 **/
	SL2_ERRORS															ExportAsIco( CImage &_iImage, const std::u16string &_sPath, SL2_OPTIONS &_oOptions );

	/**
	 * Export as ICO.
	 * 
	 * \param _iImage The image to export.
	 * \param _sPath The path to which to export _iImage.
	 * \param _oOptions Export options.
	 * \param _sMip The mipmap level to export.
	 * \param _sArray The array index to export.
	 * \param _sFace The face to export.
	 * \param _sSlice The slice to export.
	 * \return Returns an error code.
	 **/
	SL2_ERRORS															ExportAsIco( CImage &_iImage, const std::u16string &_sPath, SL2_OPTIONS &_oOptions, size_t _sMip, size_t _sArray, size_t _sFace, size_t _sSlice );

	/**
	 * Exports as ICO.
	 * 
	 * \param _iImage The image to export.
	 * \param _sPath The path to which to export _iImage.
	 * \param _oOptions Export options.
	 * \param _sMip The mipmap level to export.
	 * \param _sArray The array index to export.
	 * \param _sFace The face to export.
	 * \param _sSlice The slice to export.
	 * \param _pbifFormat The target indexed format.
	 * \return Returns an error code.
	 **/
	SL2_ERRORS															ExportAsIco_Indexed( CImage &_iImage, const std::u16string &_sPath, SL2_OPTIONS &_oOptions, size_t _sMip, size_t _sArray, size_t _sFace, size_t _sSlice,
		const CFormat::SL2_BEST_INTERNAL_FORMAT * _pbifFormat );



}	// namespace sl2

#endif	// #ifndef SL2_LIB

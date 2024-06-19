/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Let's convert and modify textures!
 */


#pragma once

#include "Image/SL2Formats.h"
#include "Image/SL2Image.h"
#include <string>
#include <vector>


namespace sl2 {

	// == Types.
	/** The conversion options. */
	struct SL2_OPTIONS {
		std::vector<std::u16string>										vInputs;												/**< The input files. */
		std::vector<std::u16string>										vOutputs;												/**< The output files. */
		const sl2::CFormat::SL2_KTX_INTERNAL_FORMAT_DATA *				pkifdFinalFormat = nullptr;								/**< Actual final format.  If not manually specified, vfAutoFormat is used. */
		sl2::CFormat::SL2_SWIZZLE										sSwizzle = CFormat::DefaultSwizzle();					/**< The swizzle to apply. */
		double															dGamma = 0.0;											/**< User-supplied gamma. */
		uint32_t														ui32NewWidth = 0;										/**< -prescale width. */
		uint32_t														ui32NewHeight = 0;										/**< -prescale height. */
		uint32_t														ui32NewDepth = 0;										/**< -prescale3 depth. */
		sl2::SL2_RESAMPLE_TO											rtResampleTo = sl2::SL2_RT_NONE;						/**< Resample size for -rescale. */

		int																iPngSaveOption = PNG_Z_DEFAULT_COMPRESSION;				/**< Option for saving as PNG. */
		const CFormat::SL2_KTX_INTERNAL_FORMAT_DATA *					pkifdPngFormat = nullptr;								/**< The PNG format. */

		sl2::SL2_VKFORMAT												vkBmpFormat = SL2_VK_FORMAT_UNDEFINED;					/**< The BMP format. */
		sl2::SL2_VKFORMAT												vkBmpFormatNoMask = SL2_VK_FORMAT_UNDEFINED;			/**< The BMP format when not using a mask. */
		int																iBmpSaveOption = BMP_DEFAULT;							/**< Option for saving as BMP. */
		bool															bBmpHasAlpha = true;									/**< Does the BMP file have alpha? */
		bool															bBmpStoreBitmask = true;								/**< Try to store the bitmask? */

		sl2::SL2_VKFORMAT												vkExrFormat = SL2_VK_FORMAT_UNDEFINED;					/**< The EXR format. */
		int																iExrSaveOption = EXR_DEFAULT;							/**< Options for saving as EXR. */

		sl2::SL2_VKFORMAT												vkJ2kFormat = SL2_VK_FORMAT_UNDEFINED;					/**< The J2K format. */
		int																iJ2kSaveOption = J2K_DEFAULT;							/**< J2K compression amount. */

		sl2::SL2_VKFORMAT												vkJp2Format = SL2_VK_FORMAT_UNDEFINED;					/**< The JP2 format. */
		int																iJp2SaveOption = JP2_DEFAULT;							/**< JP2 compression amount. */

		int																iJpgSaveOption = JPEG_DEFAULT;							/**< JPG compression amount. */

		bool															bNeedsPreMultiply = false;								/**< Does the target format, or user request, demand pre-multiplied alpha? */
		bool															bSwap = false;											/**< Swap R and B? */
		bool															bPause = false;											/**< If true, the program pauses before closing the command window. */
		bool															bShowTime = true;										/**< If true, the time taken to perform the conversion is printed. */
		
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

}	// namespace sl2

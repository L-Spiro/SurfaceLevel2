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
		double															dGamma = 0.0;											/**< User-supplied gamma. */
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

}	// namespace sl2

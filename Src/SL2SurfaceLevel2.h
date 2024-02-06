/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Let's convert and modify textures!
 */


#pragma once

#include "Image/SL2Formats.h"
#include <string>
#include <vector>


namespace sl2 {

	// == Types.
	/** The conversion options. */
	struct SL2_OPTIONS {
		std::vector<std::u16string>										vInputs;												/**< The input files. */
		std::vector<std::u16string>										vOutputs;												/**< The output files. */
		sl2::SL2_VKFORMAT												vfAutoFormat = SL2_VK_FORMAT_R8G8B8A8_UNORM;			/**< Automatic format suggested by the file format. */
		sl2::SL2_VKFORMAT												vfFinalFormat = SL2_VK_FORMAT_UNDEFINED;				/**< Actual final format.  If not manually specified, vfAutoFormat is used. */
		bool															bPause = false;											/**< If true, the program pauses before closing the command window. */
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

}	// namespace sl2

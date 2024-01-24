/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Let's convert and modify textures!
 */


#pragma once

#include <string>
#include <vector>


namespace sl2 {

	// == Enumerations.
	/** Errors. */
	enum SL2_ERRORS : int16_t {
		SL2_E_SUCCESS													= 0,							/**< No problem. */
		SL2_E_OUTOFMEMORY												= -1,							/**< Out of memory. */
		SL2_E_FILENOTFOUND												= -2,							/**< File was not found at the given path. */
		SL2_E_INVALIDWRITEPERMISSIONS									= -3,							/**< Unable to write to a file. */
		SL2_E_NODISKSPACE												= -4,							/**< Unable to write to a file (disk space). */
		SL2_E_INVALIDFILETYPE											= -5,							/**< File exists but is not in the expected format. */
		SL2_E_INVALIDCALL												= -6,							/**< Invalid call. */
		SL2_E_INVALIDDATA												= -7,							/**< Invalid input data. */
		SL2_E_INTERNALERROR												= -8,							/**< Internal error. */
		SL2_E_FEATURENOTSUPPORTED										= -9,							/**< Feature not yet supported. */
		SL2_E_PARTIALFAILURE											= -10,							/**< Within multiple tasks, one or more failed. */
		SL2_E_BADVERSION												= -11,							/**< Unsupported version (of a file etc.) */
		SL2_E_FILEOVERFLOW												= -12,							/**< The file exceeded the maximum size supported by the system. */
		SL2_E_FILEWRITEERROR											= -13,							/**< An error occurred while writing the file. */
		SL2_E_BADFORMAT													= -14,							/**< Bad data format. */
	};


	// == Types.
	/** The conversion options. */
	struct SL2_OPTIONS {
		std::vector<std::u16string>										vInputs;						/**< The input files. */
		std::vector<std::u16string>										vOutputs;						/**< The output files. */
		bool															bPause = false;					/**< If true, the program pauses before closing the command window. */
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

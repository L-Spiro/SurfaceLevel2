/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Useful utility functions.
 */


#pragma once

#include "SL2FeatureSet.h"
#include "../OS/SL2Os.h"

#include <cmath>
#include <string>
#include <vector>


#ifndef SL2_ELEMENTS
#define SL2_ELEMENTS( X )									((sizeof( X ) / sizeof( 0[X] )) / (static_cast<size_t>(!(sizeof( X ) % sizeof(0[X])))))
#endif	// #ifndef SL2_ELEMENTS

#ifndef SL2_PI
#define SL2_PI												3.1415926535897932384626433832795
#endif	// #ifndef SL2_PI

#ifndef SL2_ROUND_UP
/** Round up to the next nearest Xth, where X is a power of 2. */
#define SL2_ROUND_UP( VALUE, X )							((VALUE) + (((X) - (VALUE) & ((X) - 1)) & ((X) - 1)))
#endif	// #ifndef SL2_ROUND_UP



namespace sl2 {

	/**
	 * Class CUtilities
	 * \brief Useful utility functions.
	 *
	 * Description: Useful utility functions.
	 */
	class CUtilities {
	public :
		// == Enumerations.
		enum SL2_MAT_SETTINGS : uint32_t {
			SL2_MS_CLAMP_U									= 1 << 0,
			SL2_MS_CLAMP_V									= 1 << 1,
			SL2_MS_MIRROR_U									= 1 << 2,
			SL2_MS_MIRROR_V									= 1 << 3,
			SL2_MS_ENV										= 1 << 4,
			SL2_MS_DECAL									= 1 << 5,
			SL2_MS_SCALE_U									= 1 << 6,
			SL2_MS_SCALE_V									= 1 << 7,
			SL2_MS_TRANSPARENT								= 1 << 8,
		};


		// == Types.
		/* A structure for associating material names with textures and the texture file names. **/
		struct SL2_MAT_TEXTURE {
			std::string										sMatName;								/**< The material name. */
			std::u16string									sTexName;								/**< The associated texture. */
			std::u16string									sTexPath;								/**< The full path to the texture. */
			mutable uint32_t								ui32Flags = uint32_t( ~0 );				/**< Material flags (SL2_MAT_SETTINGS values. */
			bool											bCullBoth = false;						/**< If true, triangles are duplicated with reverse winding order. */


			// == Operators.
			/**
			 * The equality operator.
			 * 
			 * \param _vOther The object against which to compare.
			 * \return Returns true if all values in both objects are exactly equal.
			 **/
			inline bool										operator == ( const SL2_MAT_TEXTURE & _mtOther ) const {
				return sMatName == _mtOther.sMatName &&
					sTexName == _mtOther.sTexName;
			}

			/**
			 * The material-name equality operator.
			 * 
			 * \param _sOther The string to compare against our material name.
			 * \return Returns true if the given string matches sMatName.
			 **/
			inline bool										operator == ( const std::string & _sOther ) const {
				return sMatName == _sOther;
			}

			/**
			 * The material-texture equality operator.
			 * 
			 * \param _sOther The string to compare against our texture name.
			 * \return Returns true if the given string matches sTexName.
			 **/
			inline bool										operator == ( const std::u16string & _sOther ) const {
				return sTexName == _sOther;
			}


			// == Functions.
			/**
			 * Gets the material flags based off its name.
			 * 
			 * \return Returns a bitmask with flags set with properties as described by the material name.
			 **/
			uint32_t										Flags() const {
				if ( ui32Flags == uint32_t( ~0 ) ) {
					ui32Flags = 0;
					ui32Flags |= (sMatName.find( "ClampS" ) != std::string::npos) ? SL2_MS_CLAMP_U : 0;
					ui32Flags |= (sMatName.find( "ClampT" ) != std::string::npos) ? SL2_MS_CLAMP_V : 0;
					ui32Flags |= (sMatName.find( "MirrorS" ) != std::string::npos) ? SL2_MS_MIRROR_U : 0;
					ui32Flags |= (sMatName.find( "MirrorT" ) != std::string::npos) ? SL2_MS_MIRROR_V : 0;
					ui32Flags |= (sMatName.find( "EnvMapping" ) != std::string::npos) ? SL2_MS_ENV : 0;
					ui32Flags |= (sMatName.find( "TopFlag" ) != std::string::npos) ? SL2_MS_DECAL : 0;
					ui32Flags |= (sMatName.find( "TexScaleS" ) != std::string::npos) ? SL2_MS_SCALE_U : 0;
					ui32Flags |= (sMatName.find( "TexScaleT" ) != std::string::npos) ? SL2_MS_SCALE_V : 0;
					ui32Flags |= (sMatName.find( "Transparent" ) != std::string::npos) ? SL2_MS_TRANSPARENT : 0;
				}
				return ui32Flags;
			}
		};


		// == Functions.
		/**
		 * Creates a string with _cReplaceMe replaced with _cWithMe inside _s16String.
		 *
		 * \param _s16String The string in which replacements are to be made.
		 * \param _cReplaceMe The character to replace.
		 * \param _cWithMe The character with which to replace _cReplaceMe.
		 * \return Returns the new string with the given replacements made.
		 */
		static std::u16string								Replace( const std::u16string &_s16String, char16_t _cReplaceMe, char16_t _cWithMe );

		/**
		 * Replaces a string inside a data vector.
		 * 
		 * \param _vData The buffer of data in which to replace a string.
		 * \param _sReplaceMe The string to replace.
		 * \param _sWithMe The string with which to replace _sReplaceMe inside _vData.
		 * \return Returns a reference to _vData.
		 **/
		static std::vector<uint8_t> &						Replace( std::vector<uint8_t> &_vData, const std::string &_sReplaceMe, const std::string &_sWithMe );

		/**
		 * Converts a UTF-8 string to a UTF-16 string.  The resulting string may have allocated more characters than necessary but will be terminated with a NULL.
		 *
		 * \param _pcString String to convert.
		 * \param _pbErrored If not nullptr, holds a returned boolean indicating success or failure of the conversion.
		 * \return Returns the converted UTF-16 string.
		 */
		static std::u16string								Utf8ToUtf16( const char8_t * _pcString, bool * _pbErrored = nullptr );

		/**
		 * Converts a UTF-16 string to a UTF-8 string.  The resulting string may have allocated more characters than necessary but will be terminated with a NULL.
		 *
		 * \param _pcString String to convert.
		 * \param _pbErrored If not nullptr, holds a returned boolean indicating success or failure of the conversion.
		 * \return Returns the converted UTF-8 string.
		 */
		static std::string									Utf16ToUtf8( const char16_t * _pcString, bool * _pbErrored = nullptr );

		/**
		 * Reads a line from a buffer.
		 * 
		 * \param _vBuffer The buffer from which to read a line.
		 * \param _stPos The current position inside the buffer, updated on return.
		 * \return Returns the line read from the file.
		 **/
		static std::string									ReadLine( const std::vector<uint8_t> &_vBuffer, size_t &_stPos );

		/**
		 * Tokenizes a string by a given character.
		 * 
		 * \param _sString The string to tokenize.
		 * \param _vtDelimiter The character by which to deliminate the string into sections.
		 * \param _bAllowEmptyStrings If true, the return value could contain empty strings when the delimiter is found more than once in a row.
		 * \return DESC
		 **/
		static std::vector<std::string>						Tokenize( const std::string &_sString, std::string::value_type _vtDelimiter, bool _bAllowEmptyStrings );

		/**
		 * Gets the extension from a file path.
		 *
		 * \param _s16Path The file path whose extension is to be obtained.
		 * \return Returns a string containing the file extension.
		 */
		static std::u16string								GetFileExtension( const std::u16string &_s16Path );

		/**
		 * Removes the extension from a file path.
		 *
		 * \param _s16Path The file path whose extension is to be removed.
		 * \return Returns a string containing the file mname without the extension.
		 */
		static std::u16string								NoExtension( const std::u16string &_s16Path );

		/**
		 * Gets the file name from a file path.
		 *
		 * \param _s16Path The file path whose name is to be obtained.
		 * \return Returns a string containing the file name.
		 */
		static std::u16string								GetFileName( const std::u16string &_s16Path );

		/**
		 * Gets the file path without the file name
		 *
		 * \param _s16Path The file path whose path is to be obtained.
		 * \return Returns a string containing the file path.
		 */
		static std::u16string								GetFilePath( const std::u16string &_s16Path );

		/**
		 * Gets the last character in a string or std::u16string::traits_type::char_type( 0 ).
		 * 
		 * \param _s16Str The string whose last character is to be returned, if it has any characters.
		 * \return Returns the last character in the given string or std::u16string::traits_type::char_type( 0 ).
		 **/
		static std::u16string::traits_type::char_type		LastChar( const std::u16string &_s16Str ) {
			return _s16Str.size() ? _s16Str[_s16Str.size()-1] : std::u16string::traits_type::char_type( 0 );
		}

		/**
		 * Appends a char string to a char16_t string.
		 * 
		 * \param _sDst The string to which to append the string.
		 * \param _pcString The string to append to the string.
		 * \return Returns the new string.
		 **/
		static std::u16string								Append( const std::u16string &_sDst, const char * _pcString ) {
			try {
				std::u16string sTmp = _sDst;
				while ( (*_pcString) ) {
					sTmp.push_back( (*_pcString++) );
				}
				return sTmp;
			}
			catch ( ... ) { return std::u16string(); }
		}

		/**
		 * Appends a _pwcString string to a char16_t string.
		 * 
		 * \param _sDst The string to which to append the string.
		 * \param _pwcString The string to append to the string.
		 * \return Returns the new string.
		 **/
		static std::u16string								Append( const std::u16string &_sDst, const wchar_t * _pwcString ) {
			try {
				std::u16string sTmp = _sDst;
				while ( (*_pwcString) ) {
					sTmp.push_back( (*_pwcString++) );
				}
				return sTmp;
			}
			catch ( ... ) { return std::u16string(); }
		}

		/**
		 * Takes a bit mask and returns a shift and divisor.
		 * 
		 * \param _ui64Mask The bit mask.
		 * \param _dMaxVal Holds teh returned maximum value for the given mask.
		 * \return Returns the mask shift.
		 **/
		static size_t										BitMaskToShift( uint64_t _ui64Mask, double &_dMaxVal ) {
			if ( !_ui64Mask ) { _dMaxVal = 0.0; return 0; }
			size_t sShift = 0;
			while ( !(_ui64Mask & 1) ) {
				_ui64Mask >>= 1;
				++sShift;
			}
			_dMaxVal = static_cast<double>(_ui64Mask);
			return sShift;
		}

		/**
		 * Converts a single double value from sRGB space to linear space.  Performs a conversion according to the standard.
		 *
		 * \param _dVal The value to convert.
		 * \return Returns the converted value.
		 */
		static inline double SL2_FASTCALL					SRgbToLinear( double _dVal ) {
			if ( _dVal < -0.04045 ) { return -std::pow( (-_dVal + 0.055) / 1.055, 2.4 ); }
			return _dVal <= 0.04045 ?
				_dVal / 12.92 :
				std::pow( (_dVal + 0.055) / 1.055, 2.4 );
		}

		/**
		 * Converts a single double value from linear space to sRGB space.  Performs a conversion according to the standard.
		 *
		 * \param _dVal The value to convert.
		 * \return Returns the converted value.
		 */
		static inline double SL2_FASTCALL					LinearToSRgb( double _dVal ) {
			if ( _dVal < -0.0031308 ) { return -1.055 * std::pow( -_dVal, 1.0 / 2.4 ) + 0.055; }
			return _dVal <= 0.0031308 ?
				_dVal * 12.92 :
				1.055 * std::pow( _dVal, 1.0 / 2.4 ) - 0.055;
		}

		/**
		 * Converts a single double value from sRGB space to linear space.  Performs a precise conversion without a gap.
		 *
		 * \param _dVal The value to convert.
		 * \return Returns the converted value.
		 */
		static inline double SL2_FASTCALL					SRgbToLinear_Precise( double _dVal ) {
			if ( _dVal < -0.039285714285714291860163172032116563059389591217041015625 ) { return -std::pow( (-_dVal + 0.055) / 1.055, 2.4 ); }
			return _dVal <= 0.039285714285714291860163172032116563059389591217041015625 ?
				_dVal / 12.92321018078785499483274179510772228240966796875 :
				std::pow( (_dVal + 0.055) / 1.055, 2.4 );
		}

		/**
		 * Converts a single double value from linear space to sRGB space.  Performs a precise conversion without a gap.
		 *
		 * \param _dVal The value to convert.
		 * \return Returns the converted value.
		 */
		static inline double SL2_FASTCALL					LinearToSRgb_Precise( double _dVal ) {
			if ( _dVal < -0.003039934639778431833823102437008856213651597499847412109375 ) { return -1.055 * std::pow( -_dVal, 1.0 / 2.4 ) + 0.055; }
			return _dVal <= 0.003039934639778431833823102437008856213651597499847412109375 ?
				_dVal * 12.92321018078785499483274179510772228240966796875 :
				1.055 * std::pow( _dVal, 1.0 / 2.4 ) - 0.055;
		}

		/**
		 * Converts from SMPTE 170M-2004 to linear.  Performs a conversion according to the standard.
		 * 
		 * \param _dVal The value to convert.
		 * \return Returns the color value converted to linear space.
		 **/
		static inline double SL2_FASTCALL					SMPTE170MtoLinear( double _dVal ) {
			if ( _dVal < -0.081 ) { return -std::pow( (-_dVal + 0.099) / 1.099, 1.0 / 0.45 ); }
			return _dVal <= 0.081 ?
				_dVal / 4.5 :
				std::pow( (_dVal + 0.099) / 1.099, 1.0 / 0.45 );
		}

		/**
		 * Converts from linear to SMPTE 170M-2004.  Performs a conversion according to the standard.
		 *
		 * \param _dVal The value to convert.
		 * \return Returns the value converted to SMPTE 170M-2004 space.
		 */
		static inline double SL2_FASTCALL					LinearToSMPTE170M( double _dVal ) {
			if ( _dVal < -0.018 ) { return -1.099 * std::pow( -_dVal, 0.45 ) + 0.099; }
			return _dVal <= 0.018 ?
				_dVal * 4.5 :
				1.099 * std::pow( _dVal, 0.45 ) - 0.099;
		}

		/**
		 * Converts from SMPTE 170M-2004 to linear.  Performs a precise conversion without a gap.
		 * 
		 * \param _dVal The value to convert.
		 * \return Returns the color value converted to linear space.
		 **/
		static inline double SL2_FASTCALL					SMPTE170MtoLinear_Precise( double _dVal ) {
			if ( _dVal < -0.081 ) { return -std::pow( (-_dVal + 0.099) / 1.099, 1.0 / 0.45 ); }
			return _dVal <= 0.081 ?
				_dVal / 4.51378626511533997955893937614746391773223876953125 :
				std::pow( (_dVal + 0.099) / 1.099, 1.0 / 0.45 );
		}

		/**
		 * Converts from linear to SMPTE 170M-2004.  Performs a precise conversion without a gap.
		 *
		 * \param _dVal The value to convert.
		 * \return Returns the value converted to SMPTE 170M-2004 space.
		 */
		static inline double SL2_FASTCALL					LinearToSMPTE170M_Precise( double _dVal ) {
			if ( _dVal < -0.01794502336674778930625251405217568390071392059326171875 ) { return -1.099 * std::pow( -_dVal, 0.45 ) + 0.099; }
			return _dVal <= 0.01794502336674778930625251405217568390071392059326171875 ?
				_dVal * 4.51378626511533997955893937614746391773223876953125 :
				1.099 * std::pow( _dVal, 0.45 ) - 0.099;
		}

		/**
		 * Converts from DCI-P3 to linear.
		 * 
		 * \param _dVal The value to convert.
		 * \return Returns the color value converted to linear space.
		 **/
		static inline double SL2_FASTCALL					DCIP3toLinear( double _dVal ) {
			if ( _dVal < 0 ) { return -std::pow( -_dVal, 2.6 ); }
			return std::pow( _dVal, 2.6 );
		}

		/**
		 * Converts from linear to DCI-P3.
		 *
		 * \param _dVal The value to convert.
		 * \return Returns the value converted to DCI-P3 space.
		 */
		static inline double SL2_FASTCALL					LinearToDCIP3( double _dVal ) {
			if ( _dVal < 0 ) { return -std::pow( -_dVal, 1.0 / 2.6 ); }
			return std::pow( _dVal, 1.0 / 2.6 );
		}

		/**
		 * Converts from Adobe RGB to linear.
		 * 
		 * \param _dVal The value to convert.
		 * \return Returns the color value converted to linear space.
		 **/
		static inline double SL2_FASTCALL					AdobeRGBtoLinear( double _dVal ) {
			if ( _dVal < 0 ) { return -std::pow( -_dVal, 2.19921875 ); }
			return std::pow( _dVal, 2.19921875 );
		}

		/**
		 * Converts from linear to Adobe RGB.
		 *
		 * \param _dVal The value to convert.
		 * \return Returns the value converted to Adobe RGB space.
		 */
		static inline double SL2_FASTCALL					LinearToAdobeRGB( double _dVal ) {
			if ( _dVal < 0 ) { return -std::pow( -_dVal, 1.0 / 2.19921875 ); }
			return std::pow( _dVal, 1.0 / 2.19921875 );
		}

		/**
		 * Converts from SMPTE 240M to linear.  Performs a conversion according to the standard.
		 * 
		 * \param _dVal The value to convert.
		 * \return Returns the color value converted to linear space.
		 **/
		static inline double SL2_FASTCALL					SMPTE240MtoLinear( double _dVal ) {
			if ( _dVal <= -0.0913 ) { return -std::pow( (-_dVal + 0.1115) / 1.1115, 1.0 / 0.45 ); }
			return _dVal < 0.0913 ?
				_dVal / 4.0 :
				std::pow( (_dVal + 0.1115) / 1.1115, 1.0 / 0.45 );
		}

		/**
		 * Converts from linear to SMPTE 240M.  Performs a conversion according to the standard.
		 *
		 * \param _dVal The value to convert.
		 * \return Returns the value converted to SMPTE 240M space.
		 */
		static inline double SL2_FASTCALL					LinearToSMPTE240M( double _dVal ) {
			if ( _dVal <= -0.0228 ) { return -1.1115 * std::pow( -_dVal, 0.45 ) + 0.1115; }
			return _dVal < 0.0228 ?
				_dVal * 4.0 :
				1.1115 * std::pow( _dVal, 0.45 ) - 0.1115;
		}

		/**
		 * Converts from SMPTE 240M to linear.  Performs a precise conversion without a gap.
		 * 
		 * \param _dVal The value to convert.
		 * \return Returns the color value converted to linear space.
		 **/
		static inline double SL2_FASTCALL					SMPTE240MtoLinear_Precise( double _dVal ) {
			if ( _dVal <= -0.09122727272727272629371242373963468708097934722900390625 ) { return -std::pow( (-_dVal + 0.1115) / 1.1115, 1.0 / 0.45 ); }
			return _dVal < 0.09122727272727272629371242373963468708097934722900390625 ?
				_dVal / 4.00258800708078954500024337903596460819244384765625 :
				std::pow( (_dVal + 0.1115) / 1.1115, 1.0 / 0.45 );
		}

		/**
		 * Converts from linear to SMPTE 240M.  Performs a precise conversion without a gap.
		 *
		 * \param _dVal The value to convert.
		 * \return Returns the value converted to SMPTE 240M space.
		 */
		static inline double SL2_FASTCALL					LinearToSMPTE240M_Precise( double _dVal ) {
			if ( _dVal <= -0.022792071671100512519902991925846436060965061187744140625 ) { return -1.1115 * std::pow( -_dVal, 0.45 ) + 0.1115; }
			return _dVal < 0.022792071671100512519902991925846436060965061187744140625 ?
				_dVal * 4.00258800708078954500024337903596460819244384765625 :
				1.1115 * std::pow( _dVal, 0.45 ) - 0.1115;
		}

		/**
		 * Minimum between 2 values.
		 *
		 * \param _tLeft Left value.
		 * \param _tRight Right value.
		 * \return Returns the minimum between the two values.
		 */
		template <class T>
		static T											Min( T _tLeft, T _tRight ) {
			return _tLeft < _tRight ? _tLeft : _tRight;
		}

		/**
		 * Maximum between 2 values.
		 *
		 * \param _tLeft Left value.
		 * \param _tRight Right value.
		 * \return Returns the maximum between the two values.
		 */
		template <class T>
		static T											Max( T _tLeft, T _tRight ) {
			return _tLeft > _tRight ? _tLeft : _tRight;
		}

		/**
		 * Clamps the given value between the given range.
		 *
		 * \param _tValue The value to clamp.
		 * \param _tLow Lower limit for the clamped value.
		 * \param _tHigh Upper limit for the clamped value.
		 * \return Returns the given value clamped within the given range.
		 */
		template <class T>
		static T											Clamp( T _tValue, T _tLow, T _tHigh ) {
			if ( _tValue < _tLow ) { return _tLow; }
			if ( _tValue > _tHigh ) { return _tHigh; }
			return _tValue;
		}

		/**
		 * Is the given value a power of 2?
		 * 
		 * \param _ui32Val The value.
		 * \return Returns true if the given value is a power of 2.
		 **/
		static inline bool									IsPo2( uint32_t _ui32Val ) {
			if ( !_ui32Val ) { return false; }
			return (_ui32Val & (_ui32Val - 1UL)) == 0;
		}

		/**
		 * Gets the lowest power-of-2 value not below the given input value.
		 *
		 * \param _ui32Value Value for which to derive the lowest power-of-2 value not under this value.
		 * \return Returns the lowest power-of-2 value not below the given input value.
		 */
		static uint32_t										GetLowestPo2( uint32_t _ui32Value );

		/**
		 * Is AVX supported?
		 *
		 * \return Returns true if AVX is supported.
		 **/
		static inline bool									IsAvxSupported() {
#if defined( __i386__ ) || defined( __x86_64__ ) || defined( _MSC_VER )
			return CFeatureSet::AVX();
#else
			return false;
#endif	// #if defined( __i386__ ) || defined( __x86_64__ )
		}

		/**
		 * Is AVX 2 supported?
		 *
		 * \return Returns true if AVX is supported.
		 **/
		static inline bool									IsAvx2Supported() {
#if defined( __i386__ ) || defined( __x86_64__ ) || defined( _MSC_VER )
			return CFeatureSet::AVX2();
#else
			return false;
#endif	// #if defined( __i386__ ) || defined( __x86_64__ )
		}

		/**
		 * Is AVX-512F supported?
		 *
		 * \return Returns true if AVX-512F is supported.
		 **/
		static inline bool									IsAvx512FSupported() {
#if defined( __i386__ ) || defined( __x86_64__ ) || defined( _MSC_VER )
			return CFeatureSet::AVX512F();
#else
			return false;
#endif	// #if defined( __i386__ ) || defined( __x86_64__ )
		}

		/**
		 * Is AVX-512BW supported?
		 *
		 * \return Returns true if AVX-512BW is supported.
		 **/
		static inline bool									IsAvx512BWSupported() {
#if defined( __i386__ ) || defined( __x86_64__ ) || defined( _MSC_VER )
			return CFeatureSet::AVX512BW();
#else
			return false;
#endif	// #if defined( __i386__ ) || defined( __x86_64__ )
		}

		/**
		 * Is SSE 4 supported?
		 *
		 * \return Returns true if SSE 4 is supported.
		 **/
		static inline bool									IsSse4Supported() {
#if defined( __i386__ ) || defined( __x86_64__ ) || defined( _MSC_VER )
			return CFeatureSet::SSE41();
#else
			return false;
#endif	// #if defined( __i386__ ) || defined( __x86_64__ )
		}

#ifdef __AVX512F__
		/**
		 * Horizontally adds all the floats in a given AVX-512 register.
		 * 
		 * \param _mReg The register containing all of the values to sum.
		 * \return Returns the sum of all the floats in the given register.
		 **/
		static inline double								HorizontalSum( __m512d _mReg ) {
			// Step 1: Reduce 512 bits to 256 bits by adding high and low 256 bits.
			__m256d mLow256 = _mm512_castpd512_pd256( _mReg );			// Low 256 bits.
			__m256d mHigh256 = _mm512_extractf64x4_pd( _mReg, 1 );		// High 256 bits.
			__m256d mSum256 = _mm256_add_pd( mLow256, mHigh256 );

			// Step 2: Reduce 256 bits to 128 bits (similar to AVX version).
			__m128d mHigh128 = _mm256_castpd256_pd128( mSum256 );		// High 128 bits.
			__m128d mLow128 = _mm256_extractf128_pd( mSum256, 1 );		// Low 128 bits.
			__m128d mSum128 = _mm_add_pd( mHigh128, mLow128 );			// Add them.

			mSum128 = _mm_add_sd( mSum128, _mm_unpackhi_pd( mSum128, mSum128 ) );

			// Step 4: Extract the scalar value.
			return _mm_cvtsd_f64( mSum128 );
		}

		/**
		 * Horizontally adds all the floats in a given AVX-512 register.
		 * 
		 * \param _mReg The register containing all of the values to sum.
		 * \return Returns the sum of all the floats in the given register.
		 **/
		static inline float									HorizontalSum( __m512 _mReg ) {
			// Step 1: Reduce 512 bits to 256 bits by adding high and low 256 bits.
			__m256 mLow256 = _mm512_castps512_ps256( _mReg );			// Low 256 bits.
			__m256 mHigh256 = _mm512_extractf32x8_ps( _mReg, 1 );		// High 256 bits.
			__m256 mSum256 = _mm256_add_ps( mLow256, mHigh256 );

			// Step 2: Reduce 256 bits to 128 bits (similar to AVX version).
			__m128 mHigh128 = _mm256_extractf128_ps( mSum256, 1 );		// High 128 bits.
			__m128 mLow128 = _mm256_castps256_ps128( mSum256 );			// Low 128 bits.
			__m128 mSum128 = _mm_add_ps( mHigh128, mLow128 );			// Add them.

			// Step 3: Perform horizontal addition on 128 bits.
			__m128 mAddH1 = _mm_hadd_ps( mSum128, mSum128 );
			__m128 mAddH2 = _mm_hadd_ps( mAddH1, mAddH1 );

			// Step 4: Extract the scalar value.
			return _mm_cvtss_f32( mAddH2 );
		}
#endif	// #ifdef __AVX512F__

#ifdef __AVX__
		/**
		 * Horizontally adds all the floats in a given AVX register.
		 * 
		 * \param _mReg The register containing all of the values to sum.
		 * \return Returns the sum of all the floats in the given register.
		 **/
		static inline double									HorizontalSum( __m256d &_mReg ) {
			__m256d mShuf = _mm256_permute2f128_pd( _mReg, _mReg, 0x1 );	// Swap the low and high halves.
			__m256d mSums = _mm256_add_pd( _mReg, mShuf );					// Add the low and high halves.

			mShuf = _mm256_shuffle_pd( mSums, mSums, 0x5 );					// Swap the pairs of doubles.
			mSums = _mm256_add_pd( mSums, mShuf );							// Add the pairs.

			return _mm256_cvtsd_f64( mSums );								// Extract the sum.
		}

		/**
		 * Horizontally adds all the floats in a given AVX register.
		 * 
		 * \param _mReg The register containing all of the values to sum.
		 * \return Returns the sum of all the floats in the given register.
		 **/
		static inline float									HorizontalSum( __m256 &_mReg ) {
			// Step 1 & 2: Shuffle and add the high 128 to the low 128.
			__m128 mHigh128 = _mm256_extractf128_ps( _mReg, 1 );		// Extract high 128 bits.
			__m128 mLow128 = _mm256_castps256_ps128( _mReg );			// Directly use low 128 bits.
			__m128 mSum128 = _mm_add_ps( mHigh128, mLow128 );			// Add them.

			// Step 3: Perform horizontal addition.
			__m128 mAddH1 = _mm_hadd_ps( mSum128, mSum128 );
			__m128 mAddH2 = _mm_hadd_ps( mAddH1, mAddH1 );

			// Step 4: Extract the scalar value.
			return _mm_cvtss_f32( mAddH2 );
		}
#endif	// #ifdef __AVX__

#ifdef __SSE4_1__
		/**
		 * Horizontally adds all the floats in a given SSE register.
		 * 
		 * \param _mReg The register containing all of the values to sum.
		 * \return Returns the sum of all the floats in the given register.
		 **/
		static inline double								HorizontalSum( __m128d &_mReg ) {
			__m128d mAddH1 = _mm_shuffle_pd( _mReg, _mReg, 0x1 );
			__m128d mAddH2 = _mm_add_pd( _mReg, mAddH1 );
			return _mm_cvtsd_f64( mAddH2 );
		}

		/**
		 * Horizontally adds all the floats in a given SSE register.
		 * 
		 * \param _mReg The register containing all of the values to sum.
		 * \return Returns the sum of all the floats in the given register.
		 **/
		static inline float									HorizontalSum( __m128 &_mReg ) {
			__m128 mAddH1 = _mm_hadd_ps( _mReg, _mReg );
			__m128 mAddH2 = _mm_hadd_ps( mAddH1, mAddH1 );
			return _mm_cvtss_f32( mAddH2 );
		}
#endif	// #ifdef __SSE4_1__
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Functions.

}	// namespace sl2

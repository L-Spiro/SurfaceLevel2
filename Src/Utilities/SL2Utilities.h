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

#include <cassert>
#include <cmath>
#include <filesystem>
#include <numbers>
#include <string>
#include <vector>

#if defined( __APPLE__ )
#import <Cocoa/Cocoa.h>
#elif defined( __linux__ )
#include <gtk/gtk.h>
#endif


#ifndef SL2_ELEMENTS
#define SL2_ELEMENTS( X )									((sizeof( X ) / sizeof( 0[X] )) / (static_cast<size_t>(!(sizeof( X ) % sizeof(0[X])))))
#endif	// #ifndef SL2_ELEMENTS

#ifndef SL2_PI
#define SL2_PI												3.14159265358979323846264338327950288419716939937510	// All good things in excess.
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

		enum SL2_CLIP_FORMATS {
			SL2_CF_NONE,
			SL2_CF_DIB,
			SL2_CF_DIBV5,
			SL2_CF_BITMAP,
			SL2_CF_PNG,
			SL2_CF_TIFF,
		};


		// == Types.
		/** A structure for associating material names with textures and the texture file names. **/
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

#ifdef _WIN32
		/** A wrapper around ::GlobalLock()/::GlobalUnlock() for Windows. */
		struct SL2_GLOBALLOCK {
			SL2_GLOBALLOCK( HGLOBAL _gLockMe ) :
				gLockMe( _gLockMe ) {
				if ( gLockMe ) {
					lpvData = ::GlobalLock( gLockMe );
				}
			}
			~SL2_GLOBALLOCK() {
				if ( gLockMe ) {
					::GlobalUnlock( gLockMe );
				}
			}


			// == Functions.
			/**
			 * Gets a pointer to the memory pointer returned by ::GlobalLock().
			 * 
			 * \return Returns the pointer to the memory pointer returned by ::GlobalLock().
			 **/
			inline LPVOID									Data() { return lpvData; }

		private :
			// == Members.
			HGLOBAL											gLockMe = NULL;							/**< The handle we manage. */
			LPVOID											lpvData = NULL;							/**< The pointer to the data. */
		};

		/** A wrapper around cliboard access for Windows. */
		struct SL2_CLIPBOARD {
			SL2_CLIPBOARD( HWND _hWnd = NULL ) :
				bOpen( ::OpenClipboard( _hWnd ) != FALSE ) {
			}
			~SL2_CLIPBOARD() {
				if ( bOpen ) {
					::CloseClipboard();
				}
			}


			// == Functions.
			/**
			 * Gets data of a give format.  Call within a try/catch block.  An exception indicates a memory-allocation failure when resizing _vData.
			 * 
			 * \param _uiFormat The format of the data to attempt to retreive from the clipboard.
			 * \param _vData Holds the returned vector of data.
			 * \return Returns true if data of the given format was found in the clipboard.
			 **/
			template <typename _tType = std::vector<uint8_t>>
			bool											GetData( UINT _uiFormat, _tType &_vData ) {
				static_assert( sizeof( _tType::value_type ) == 1, "Compile-time assertion failed: sizeof( _tType::value_type ) must equal 1." );
				if ( !::IsClipboardFormatAvailable( _uiFormat ) ) { return false; }

				HGLOBAL hMem = ::GetClipboardData( _uiFormat );
				if ( hMem ) {
					_vData.resize( ::GlobalSize( hMem ) );
					SL2_GLOBALLOCK glLock( hMem );
					if ( !glLock.Data() ) { return false; }
					std::memcpy( _vData.data(), glLock.Data(), _vData.size() );
					return true;
				}
				return false;
			}


		protected :
			bool											bOpen = false;							/**< Is the clipboard open? */
		};
#endif	// #ifdef _WIN32


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
		 * Converts an * string to a std::u16string.  Call inside try{}catch(...){}.
		 * 
		 * \param _pwcStr The string to convert.
		 * \param _sLen The length of the string or 0.
		 * \return Returns the converted string.
		 **/
		template <typename _tCharType>
		static inline std::u16string						XStringToU16String( const _tCharType * _pwcStr, size_t _sLen ) {
			std::u16string u16Tmp;
			if ( _sLen ) {
				u16Tmp.reserve( _sLen );
			}
			for ( size_t I = 0; (I < _sLen) || (_sLen == 0 && !_pwcStr[I]); ++I ) {
				u16Tmp.push_back( static_cast<char16_t>(_pwcStr[I]) );
			}
			return u16Tmp;
		}

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
		 * \param _sStr The string to which to append the string.
		 * \param _pwcString The string to append to the string.
		 * \return Returns the new string.
		 **/
		static std::u16string								Append( const std::u16string &_sStr, const wchar_t * _pwcString ) {
			try {
				std::u16string sTmp = _sStr;
				while ( (*_pwcString) ) {
					sTmp.push_back( (*_pwcString++) );
				}
				return sTmp;
			}
			catch ( ... ) { return std::u16string(); }
		}

		/**
		 * Appends a std::u16string string to a std::filesystem::path.  Call inside a try/catch.
		 * 
		 * \param _pStr The path to which to append the given string.
		 * \param _u16String The string to append to the given path.
		 * \return Returns the string with _u16String appended.
		 **/
		static inline std::filesystem::path					Append( const std::filesystem::path &_pStr, const std::u16string &_u16String ) {
			std::filesystem::path pTmp = _pStr;
			for ( size_t I = 0; I < _u16String.size(); ++I ) {
				pTmp += std::filesystem::path( std::u16string( 1, _u16String[I] ) );
			}
			return pTmp;
		}

		/**
		 * Determines if the given character array has any UTF encodings.
		 * 
		 * \param _ptString Pointer to the array to scan for UTF characters.
		 * \param _sLen Length of the array to which _ptString points.
		 * \return Returns true if any of the characters in the given array have any bits set above the 7th.
		 **/
		template <typename _tType>
		static bool											HasUtf( const _tType * _ptString, size_t _sLen = 0 ) {
			if ( !_sLen ) {
				for ( size_t I = 0; _ptString[I]; ++I ) {
					if ( _ptString[I] & ~static_cast<_tType>(0x7F) ) { return true; }
				}
			}
			for ( auto I = _sLen; I--; ) {
				if ( _ptString[I] & ~static_cast<_tType>(0x7F) ) { return true; }
			}
			return false;
		}

		/**
		 * Creates an ASCII path from the given file name, even if it is already an ASCII path.
		 *
		 * \param _sPath The input path, including the file name.
		 * \param _pAsciiPath The output folder.
		 * \param _pAsciiFileName The output file name.
		 * \return Returns true if allocation of all strings succeeded.  Failure indicates a memory failure.
		 **/
		static bool											CreateAsciiPath( const std::u16string &_sPath, std::filesystem::path &_pAsciiPath, std::filesystem::path &_pAsciiFileName );

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
		static inline double SL2_FASTCALL					sRGBtoLinear( double _dVal ) {
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
		static inline double SL2_FASTCALL					LinearTosRGB( double _dVal ) {
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
		static inline double SL2_FASTCALL					sRGBtoLinear_Precise( double _dVal ) {
			/*constexpr double dAlpha = 0.05501071894758659264201838823282741941511631011962890625;
			constexpr double dBeta = 1.055010718947586578764230580418370664119720458984375;
			constexpr double dTheta = 12.9200000000000017053025658242404460906982421875;
			constexpr double dCut = 0.03929337067684757212049362351535819470882415771484375;*/

			constexpr double dAlpha = 0.055000000000000000277555756156289135105907917022705078125;
			constexpr double dBeta = 1.0549999999999999378275106209912337362766265869140625;
			constexpr double dTheta = 12.92321018078785499483274179510772228240966796875;
			constexpr double dCut = 0.039285714285714291860163172032116563059389591217041015625;
			if ( _dVal < -dCut ) { return -std::pow( (-_dVal + dAlpha) / dBeta, 2.4 ); }
			return _dVal <= dCut ?
				_dVal / dTheta :
				std::pow( (_dVal + dAlpha) / dBeta, 2.4 );
		}

		/**
		 * Converts a single double value from linear space to sRGB space.  Performs a precise conversion without a gap.
		 *
		 * \param _dVal The value to convert.
		 * \return Returns the converted value.
		 */
		static inline double SL2_FASTCALL					LinearTosRGB_Precise( double _dVal ) {
			/*constexpr double dAlpha = 0.05501071894758659264201838823282741941511631011962890625;
			constexpr double dBeta = 1.055010718947586578764230580418370664119720458984375;
			constexpr double dTheta = 12.9200000000000017053025658242404460906982421875;
			constexpr double dCut = 0.0030412825601275205074369711866211218875832855701446533203125;*/

			constexpr double dAlpha = 0.055000000000000000277555756156289135105907917022705078125;
			constexpr double dBeta = 1.0549999999999999378275106209912337362766265869140625;
			constexpr double dTheta = 12.92321018078785499483274179510772228240966796875;
			constexpr double dCut = 0.003039934639778431833823102437008856213651597499847412109375;
			if ( _dVal < -dCut ) { return -dBeta * std::pow( -_dVal, 1.0 / 2.4 ) + dAlpha; }
			return _dVal <= dCut ?
				_dVal * dTheta :
				dBeta * std::pow( _dVal, 1.0 / 2.4 ) - dAlpha;
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
			if ( _dVal < -0.08124285829863515939752716121802222914993762969970703125 ) { return -std::pow( (-_dVal + 0.09929682680944297568093048766968422569334506988525390625) / 1.09929682680944296180314267985522747039794921875, 1.0 / 0.45 ); }
			return _dVal <= 0.08124285829863515939752716121802222914993762969970703125 ?
				_dVal / 4.5 :
				std::pow( (_dVal + 0.09929682680944297568093048766968422569334506988525390625) / 1.09929682680944296180314267985522747039794921875, 1.0 / 0.45 );
		}

		/**
		 * Converts from linear to SMPTE 170M-2004.  Performs a precise conversion without a gap.
		 *
		 * \param _dVal The value to convert.
		 * \return Returns the value converted to SMPTE 170M-2004 space.
		 */
		static inline double SL2_FASTCALL					LinearToSMPTE170M_Precise( double _dVal ) {
			if ( _dVal < -0.0180539685108078128139563744980478077195584774017333984375 ) { return -1.09929682680944296180314267985522747039794921875 * std::pow( -_dVal, 0.45 ) + 0.09929682680944297568093048766968422569334506988525390625; }
			return _dVal <= 0.0180539685108078128139563744980478077195584774017333984375 ?
				_dVal * 4.5 :
				1.09929682680944296180314267985522747039794921875 * std::pow( _dVal, 0.45 ) - 0.09929682680944297568093048766968422569334506988525390625;
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
		 * Converts from linear to linear.
		 * 
		 * \param _dVal The value to convert.
		 * \return Returns the color value converted to linear space.
		 **/
		static inline double SL2_FASTCALL					PassThrough( double _dVal ) {
			return _dVal;
		}

		/**
		 * Converts from 2.2 to linear.
		 * 
		 * \param _dVal The value to convert.
		 * \return Returns the color value converted to linear space.
		 **/
		static inline double SL2_FASTCALL					Pow2_2toLinear( double _dVal ) {
			if ( _dVal < 0 ) { return -std::pow( -_dVal, 2.2 ); }
			return std::pow( _dVal, 2.2 );
		}

		/**
		 * Converts from linear to 2.2.
		 *
		 * \param _dVal The value to convert.
		 * \return Returns the value converted to 2.2 space.
		 */
		static inline double SL2_FASTCALL					LinearToPow2_2( double _dVal ) {
			if ( _dVal < 0 ) { return -std::pow( -_dVal, 1.0 / 2.2 ); }
			return std::pow( _dVal, 1.0 / 2.2 );
		}

		/**
		 * Converts from 2.8 to linear.
		 * 
		 * \param _dVal The value to convert.
		 * \return Returns the color value converted to linear space.
		 **/
		static inline double SL2_FASTCALL					Pow2_8toLinear( double _dVal ) {
			if ( _dVal < 0 ) { return -std::pow( -_dVal, 2.8 ); }
			return std::pow( _dVal, 2.8 );
		}

		/**
		 * Converts from linear to 2.8.
		 *
		 * \param _dVal The value to convert.
		 * \return Returns the value converted to 2.8 space.
		 */
		static inline double SL2_FASTCALL					LinearToPow2_8( double _dVal ) {
			if ( _dVal < 0 ) { return -std::pow( -_dVal, 1.0 / 2.8 ); }
			return std::pow( _dVal, 1.0 / 2.8 );
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
			if ( _dVal < -0.0912863421177801115380390228892792947590351104736328125 ) { return -std::pow( (-_dVal + 0.1115721959217312597711924126997473649680614471435546875) / 1.1115721959217312875267680283286608755588531494140625, 1.0 / 0.45 ); }
			return _dVal <= 0.0912863421177801115380390228892792947590351104736328125 ?
				_dVal / 4.0 :
				std::pow( (_dVal + 0.1115721959217312597711924126997473649680614471435546875) / 1.1115721959217312875267680283286608755588531494140625, 1.0 / 0.45 );
		}

		/**
		 * Converts from linear to SMPTE 240M.  Performs a precise conversion without a gap.
		 *
		 * \param _dVal The value to convert.
		 * \return Returns the value converted to SMPTE 240M space.
		 */
		static inline double SL2_FASTCALL					LinearToSMPTE240M_Precise( double _dVal ) {
			if ( _dVal < -0.022821585529445027884509755722319823689758777618408203125 ) { return -1.1115721959217312875267680283286608755588531494140625 * std::pow( -_dVal, 0.45 ) + 0.1115721959217312597711924126997473649680614471435546875; }
			return _dVal <= 0.022821585529445027884509755722319823689758777618408203125 ?
				_dVal * 4.0 :
				1.1115721959217312875267680283286608755588531494140625 * std::pow( _dVal, 0.45 ) - 0.1115721959217312597711924126997473649680614471435546875;
		}

		/**
		 * Converts from ACEScc to linear.  Performs a conversion according to the standard.
		 * 
		 * \param _dVal The value to convert.
		 * \return Returns the color value converted to linear space.
		 **/
		static inline double SL2_FASTCALL					ACESccToLinear( double _dVal ) {
			constexpr double dB = 9.72;
			constexpr double dC = 17.52;
			constexpr double dThresh = (dB - 15.0) / dC;														// -0.30136986301369863013698630136986.
			constexpr double dUpperLimit = (15.9992953870234106972247900557704269886016845703125 + dB) / dC;	// (log2(65504)+9.72)/17.52 -> 1.4679963120447152224443373319503.

			if ( _dVal <= dThresh ) {
				return (std::pow( 2.0, _dVal * dC - dB ) - 0.0000152587890625 /*std::pow( 2.0, -16.0 )*/) * 2.0;
			}
			else if ( _dVal < dUpperLimit ) {
				return std::pow( 2.0, _dVal * dC - dB );
			}
			else {
				return 65504.0;
			}

		}

		/**
		 * Converts from linear to ACEScc.  Performs a conversion according to the standard.
		 *
		 * \param _dVal The value to convert.
		 * \return Returns the value converted to ACEScc space.
		 */
		static inline double SL2_FASTCALL					LinearToACEScc( double _dVal ) {
			constexpr double dB = 9.72;
			constexpr double dC = 17.52;
    
			if ( _dVal <= 0.0 ) {
				return (-16.0 /*std::log2( std::pow(2.0, -16.0 ) )*/ + dB) / dC;
			}
			else if ( _dVal < 0.000030517578125 /*std::pow( 2.0, -15.0 )*/ ) {
				return (std::log2( 0.0000152587890625 /*std::pow( 2.0, -16.0 )*/ + _dVal * 0.5 ) + dB) / dC;
			}
			else {
				return (std::log2( _dVal ) + dB) / dC;
			}
		}

		/**
		 * Converts from ROMM RGB to linear.
		 * 
		 * \param _dVal The value to convert.
		 * \return Returns the color value converted to linear space.
		 **/
		static inline double SL2_FASTCALL					RommRgbToLinear( double _dVal ) {
			if ( _dVal <= 0 ) { return 0.0; }
			if ( _dVal <= 0.03125 ) {
				return _dVal / 16.0;
			}
			if ( _dVal < 1.0 ) {
				return std::pow( _dVal, 1.8 );
			}
			return 1.0;
		}

		/**
		 * Converts from linear to ROMM RGB.
		 *
		 * \param _dVal The value to convert.
		 * \return Returns the value converted to ROMM RGB space.
		 */
		static inline double SL2_FASTCALL					LinearToRommRgb( double _dVal ) {
			if ( _dVal <= 0 ) { return 0.0; }
			if ( _dVal <= 0.001953125 ) {	// pow(16.0, 1.8/(1-1.8));
				return _dVal * 16.0;
			}
			if ( _dVal < 1.0 ) {
				return std::pow( _dVal, 1.0 / 1.8 );
			}
			return 1.0;
		}

		/**
		 * Converts from RIMM RGB to linear.
		 * 
		 * \param _dVal The value to convert.
		 * \return Returns the color value converted to linear space.
		 **/
		static inline double SL2_FASTCALL					RimmRgbToLinear( double _dVal ) {
			//constexpr double dVclip = 1.4022782421730806134974045562557876110076904296875;	// 1.099 * pow( 2.0, 0.45 ) - 0.099
			constexpr double dVclip = 1.4023868927346205826012237594113685190677642822265625;	// 1.09929682680944296180314267985522747039794921875 * pow( 2.0, 0.45 ) - 0.09929682680944297568093048766968422569334506988525390625
			_dVal *= dVclip;
			if ( _dVal < -0.08124285829863515939752716121802222914993762969970703125 ) { return -std::pow( (-_dVal + 0.09929682680944297568093048766968422569334506988525390625) / 1.09929682680944296180314267985522747039794921875, 1.0 / 0.45 ); }
			return _dVal <= 0.08124285829863515939752716121802222914993762969970703125 ?
				_dVal / 4.5 :
				std::pow( (_dVal + 0.09929682680944297568093048766968422569334506988525390625) / 1.09929682680944296180314267985522747039794921875, 1.0 / 0.45 );
		}

		/**
		 * Converts from linear to RIMM RGB.
		 *
		 * \param _dVal The value to convert.
		 * \return Returns the value converted to RIMM RGB space.
		 */
		static inline double SL2_FASTCALL					LinearToRimmRgb( double _dVal ) {
			//constexpr double dVclip = 1.4022782421730806134974045562557876110076904296875;	// 1.099 * pow( 2.0, 0.45 ) - 0.099
			constexpr double dVclip = 1.4023868927346205826012237594113685190677642822265625;	// 1.09929682680944296180314267985522747039794921875 * pow( 2.0, 0.45 ) - 0.09929682680944297568093048766968422569334506988525390625
			if ( _dVal < -0.0180539685108078128139563744980478077195584774017333984375 ) { return (-1.09929682680944296180314267985522747039794921875 * std::pow( -_dVal, 0.45 ) + 0.09929682680944297568093048766968422569334506988525390625) *
				dVclip; }
			return (_dVal <= 0.0180539685108078128139563744980478077195584774017333984375 ?
				_dVal * 4.5 :
				1.09929682680944296180314267985522747039794921875 * std::pow( _dVal, 0.45 ) - 0.09929682680944297568093048766968422569334506988525390625) / dVclip;
		}

		/**
		 * Converts from ERIMM RGB to linear.
		 * 
		 * \param _dVal The value to convert.
		 * \return Returns the color value converted to linear space.
		 **/
		static inline double SL2_FASTCALL					ErimmRgbToLinear( double _dVal ) {
			constexpr double dLogEclip = 2.5;
			constexpr double dEclip = 316.22776601683796116049052216112613677978515625;				// pow( 10, 2.5 ).
			constexpr double dLogEmin = -3.0;
			constexpr double dEmin = 0.001;
			constexpr double dLogEt = -2.56570551809674807230976512073539197444915771484375;
			constexpr double dEt = 0.0027182818284590451983484538089896886958740651607513427734375;	// e * dEmin.

			constexpr double dLogEclipMinusLogEmin = dLogEclip - dLogEmin;
			constexpr double dLogEtMinusLogEmin = dLogEt - dLogEmin;

			if ( _dVal <= 0.0 ) { return 0.0; }
			if ( _dVal <= (dLogEtMinusLogEmin / dLogEclipMinusLogEmin) ) {
				return (dLogEclipMinusLogEmin / dLogEtMinusLogEmin) * (_dVal * dEt);
			}
			if ( _dVal < 1.0 ) {
				return std::pow( 10.0, _dVal * dLogEclipMinusLogEmin + dLogEmin );
			}
			return 1.0;
		}

		/**
		 * Converts from linear to ERIMM RGB.
		 *
		 * \param _dVal The value to convert.
		 * \return Returns the value converted to ERIMM RGB space.
		 */
		static inline double SL2_FASTCALL					LinearToErimmRgb( double _dVal ) {
			constexpr double dLogEclip = 2.5;
			constexpr double dEclip = 316.22776601683796116049052216112613677978515625;				// pow( 10, 2.5 ).
			constexpr double dLogEmin = -3.0;
			constexpr double dEmin = 0.001;
			constexpr double dLogEt = -2.56570551809674807230976512073539197444915771484375;
			constexpr double dEt = 0.0027182818284590451983484538089896886958740651607513427734375;	// e * dEmin.

			constexpr double dLogEclipMinusLogEmin = dLogEclip - dLogEmin;

			if ( _dVal <= 0.0 ) { return 0.0; }
			if ( _dVal <= dEt ) {
				return ((dLogEt - dLogEmin) / dLogEclipMinusLogEmin) * (_dVal / dEt);
			}
			if ( _dVal <= dEclip && _dVal < 1.0 ) {
				return (std::log10( _dVal ) - dLogEmin) / dLogEclipMinusLogEmin;
			}
			return 1.0;
		}

		/**
		 * Converts from S-Log to linear.
		 * 
		 * \param _dVal The value to convert.
		 * \return Returns the color value converted to linear space.
		 **/
		static inline double SL2_FASTCALL					SLogToLinear( double _dVal ) {
			return std::pow( 10.0, ((_dVal - 0.616596 - 0.03) / 0.432699) ) - 0.037584;

		}

		/**
		 * Converts from linear to S-Log.
		 *
		 * \param _dVal The value to convert.
		 * \return Returns the value converted to S-Log space.
		 */
		static inline double SL2_FASTCALL					LinearToSLog( double _dVal ) {
			return (0.432699 * std::log10( _dVal + 0.037584 ) + 0.616596) + 0.03;
		}

		/**
		 * Converts from S-Log2 to linear.
		 * 
		 * \param _dVal The value to convert.
		 * \return Returns the color value converted to linear space.
		 **/
		static inline double SL2_FASTCALL					SLog2ToLinear( double _dVal ) {
			return (std::pow( 10.0, (((_dVal * 1023.0 / 4.0 - 16.0) / 219.0) - 0.616596 - 0.03) / 0.432699 ) - 0.037584) * 0.9;
		}

		/**
		 * Converts from linear to S-Log2.
		 *
		 * \param _dVal The value to convert.
		 * \return Returns the value converted to S-Log2 space.
		 */
		static inline double SL2_FASTCALL					LinearToSLog2( double _dVal ) {
			return (4.0 * (16.0 + 219.0 * (0.616596 + 0.03 + 0.432699 *
				std::log10( 0.037584 + _dVal / 0.9 )))) / 1023.0;

		}

		/**
		 * Converts from S-Log3 to linear.
		 * 
		 * \param _dVal The value to convert.
		 * \return Returns the color value converted to linear space.
		 **/
		static inline double SL2_FASTCALL					SLog3ToLinear( double _dVal ) {
			if ( _dVal >= 171.2102946929 / 1023.0 ) {
				return std::pow( 10.0, ((_dVal * 1023.0 - 420.0) / 261.5) ) * (0.18 + 0.01) - 0.01;
			}
			return (_dVal * 1023.0 - 95.0) * 0.01125000 / (171.2102946929 - 95.0);

		}

		/**
		 * Converts from linear to S-Log3.
		 *
		 * \param _dVal The value to convert.
		 * \return Returns the value converted to S-Log3 space.
		 */
		static inline double SL2_FASTCALL					LinearToSLog3( double _dVal ) {
			if ( _dVal >= 0.01125000 ) {
				return (420.0 + std::log10( (_dVal + 0.01) / (0.18 + 0.01) ) * 261.5) / 1023.0;
			}
			return (_dVal * (171.2102946929 - 95.0) / 0.01125000 + 95.0) / 1023.0;
		}

		/**
		 * Converts from Protune to linear.
		 * 
		 * \param _dVal The value to convert.
		 * \return Returns the color value converted to linear space.
		 **/
		static inline double SL2_FASTCALL					ProtuneToLinear( double _dVal ) {
			return (std::pow( 113.0, _dVal ) - 1.0) / 112.0;

		}

		/**
		 * Converts from linear to Protune.
		 *
		 * \param _dVal The value to convert.
		 * \return Returns the value converted to Protune space.
		 */
		static inline double SL2_FASTCALL					LinearToProtune( double _dVal ) {
			constexpr double dD = 4.727387818712340816773576079867780208587646484375;	// std::log( 113.0 )
			return std::log( _dVal * 112.0 + 1.0 ) / dD;
		}

		/**
		 * Converts from Canon Log to linear.
		 * 
		 * \param _dVal The value to convert.
		 * \return Returns the color value converted to linear space.
		 **/
		static inline double SL2_FASTCALL					CanonLogToLinear( double _dVal ) {
			return (_dVal < 0.0730597) ?
				-(std::pow( 10.0, (0.0730597 - _dVal) / 0.529136 ) - 1.0) / 10.1596 :
				(std::pow( 10.0, (_dVal - 0.0730597) / 0.529136 ) - 1.0) / 10.1596;
		}

		/**
		 * Converts from linear to Canon Log.
		 *
		 * \param _dVal The value to convert.
		 * \return Returns the value converted to Canon Log space.
		 */
		static inline double SL2_FASTCALL					LinearToCanonLog( double _dVal ) {
			constexpr double dThreshold = 0.0;	// CanonLogToLinear( 0.0730597 );	// 0.0

			return (_dVal < dThreshold) ?
				-(0.529136 * (std::log10( -_dVal * 10.1596 + 1.0 )) - 0.0730597) :
				0.529136 * std::log10( 10.1596 * _dVal + 1.0 ) + 0.0730597;
		}

		/**
		 * Converts from Canon Log 2 to linear.
		 * 
		 * \param _dVal The value to convert.
		 * \return Returns the color value converted to linear space.
		 **/
		static inline double SL2_FASTCALL					CanonLog2ToLinear( double _dVal ) {
			return (_dVal < 0.035388128) ?
				-(std::pow( 10.0, (0.035388128 - _dVal) / 0.281863093 ) - 1) / 87.09937546 :
				(std::pow( 10.0, (_dVal - 0.035388128) / 0.281863093 ) - 1) / 87.09937546;
		}

		/**
		 * Converts from linear to Canon Log 2.
		 *
		 * \param _dVal The value to convert.
		 * \return Returns the value converted to Canon Log 2 space.
		 */
		static inline double SL2_FASTCALL					LinearToCanonLog2( double _dVal ) {
			constexpr double dThreshold = 0.0;	// CanonLog2ToLinear( 0.035388128 );	// 0.0

			return (_dVal < dThreshold) ?
				-(0.281863093 * (std::log10( -_dVal * 87.09937546 + 1.0 )) - 0.035388128) :
				0.281863093 * std::log10( _dVal * 87.09937546 + 1.0 ) + 0.035388128;
		}

		/**
		 * Converts from Canon Log 3 to linear.
		 * 
		 * \param _dVal The value to convert.
		 * \return Returns the color value converted to linear space.
		 **/
		static inline double SL2_FASTCALL					CanonLog3ToLinear( double _dVal ) {
			if ( _dVal < 0.04076162 ) {
				return -(std::pow( 10.0, (0.069886632 - _dVal) / 0.42889912 ) - 1.0) / 14.98325;
			}
			else if ( _dVal <= 0.105357102 ) {
				return (_dVal - 0.073059361) / 2.3069815;
			}
			return (std::pow( 10.0, (_dVal - 0.069886632) / 0.42889912 ) - 1.0) / 14.98325;
		}

		/**
		 * Converts from linear to Canon Log 3.
		 *
		 * \param _dVal The value to convert.
		 * \return Returns the value converted to Canon Log 3 space.
		 */
		static inline double SL2_FASTCALL					LinearToCanonLog3( double _dVal ) {
			//const double dThresh1 = CanonLog3ToLinear( 0.04076162 );	// -0.0140000000000000020261570199409106862731277942657470703125
			//const double dThresh2 = CanonLog3ToLinear( 0.105357102 );	// 0.01399999999999999682198659201048940303735435009002685546875
			constexpr double dThresh1 = -0.014;	// -0.0140000000000000020261570199409106862731277942657470703125
			constexpr double dThresh2 = 0.014;	// 0.01399999999999999682198659201048940303735435009002685546875

			double clog3_ire = 0.0;
			if ( _dVal < dThresh1 ) {
				return -(0.42889912 * (std::log10( -_dVal * 14.98325 + 1.0 )) - 0.069886632);
			} else if ( _dVal <= dThresh2 ) {
				return 2.3069815 * _dVal + 0.073059361;
			}
			return 0.42889912 * std::log10( _dVal * 14.98325 + 1.0 ) + 0.069886632;
		}

		/**
		 * Converts from Viper to linear.
		 * 
		 * \param _dVal The value to convert.
		 * \return Returns the color value converted to linear space.
		 **/
		static inline double SL2_FASTCALL					ViperToLinear( double _dVal ) {
			return std::pow( 10.0, (1023.0 * _dVal - 1023.0) / 500.0 );
		}

		/**
		 * Converts from linear to Viper.
		 *
		 * \param _dVal The value to convert.
		 * \return Returns the value converted to Viper space.
		 */
		static inline double SL2_FASTCALL					LinearToViper( double _dVal ) {
			return (1023.0 + 500.0 * std::log10( _dVal )) / 1023.0;
		}

		/**
		 * Converts from ARIB STD-B67 to linear.
		 * 
		 * \param _dVal The value to convert.
		 * \return Returns the color value converted to linear space.
		 **/
		static inline double SL2_FASTCALL					ARIBSTDB67ToLinear( double _dVal ) {
			constexpr double dA = 0.17883277;
			constexpr double dB = 0.28466892;
			constexpr double dC = 0.55991073;

			constexpr double dR = 0.5;		// Optional: Video level corresponding to reference white level.

			return (_dVal <= (1.0 * dR)) ?
				std::pow( _dVal / dR, 2.0 ) :
				std::exp( (_dVal - dC) / dA ) + dB;
		}

		/**
		 * Converts from linear to ARIB STD-B67.
		 *
		 * \param _dVal The value to convert.
		 * \return Returns the value converted to ARIB STD-B67 space.
		 */
		static inline double SL2_FASTCALL					LinearToARIBSTDB67( double _dVal ) {
			constexpr double dA = 0.17883277;
			constexpr double dB = 0.28466892;
			constexpr double dC = 0.55991073;

			constexpr double dR = 0.5;		// Optional: Video level corresponding to reference white level.

			return (_dVal <= 1.0) ?
				dR * std::sqrt( _dVal ) :
				dA * std::log( _dVal - dB ) + dC;
		}

		/**
		 * Converts from Panalog to linear.
		 * 
		 * \param _dVal The value to convert.
		 * \return Returns the color value converted to linear space.
		 **/
		static inline double SL2_FASTCALL					PanalogToLinear( double _dVal ) {
			//double dBlackOffset = std::pow( 10.0, (64.0 - 681.0) / 444.0 );		// 0.04077184461038073487917182305836
			constexpr double dBlackOffset = 0.04077184461038073359784306148867472074925899505615234375;
			return (std::pow( 10.0, (1023.0 * _dVal - 681.0) / 444.0 ) - dBlackOffset) / (1.0 - dBlackOffset);
		}

		/**
		 * Converts from linear to Panalog.
		 *
		 * \param _dVal The value to convert.
		 * \return Returns the value converted to Panalog space.
		 */
		static inline double SL2_FASTCALL					LinearToPanalog( double _dVal ) {
			//double dBlackOffset = std::pow( 10.0, (64.0 - 681.0) / 444.0 );		// 0.04077184461038073487917182305836
			constexpr double dBlackOffset = 0.04077184461038073359784306148867472074925899505615234375;
			return (681.0 + 444.0 * std::log10( _dVal * (1.0 - dBlackOffset) + dBlackOffset )) / 1023.0;
		}

		/**
		 * A proper CRT curve based on measurements with WHITE and BRIGHTNESS controls.
		 * 
		 * \param param _dVal The value to convert.
		 * \param _dLw Screen luminance for white, reference setting is LW = 100 cd/m2.
		 * \param _dB Variable for black level lift (legacy “brightness” control).
		 *	The value of _dB is set so that the calculated luminance can be the same as the
		 *	measurement data at input signal level 0.0183 (= (80-64)/876).
		 *	The value of _dB changes depending on “brightness” control.
		 * \return Returns the corresponding value from a decent CRT curve back to linear.
		 **/
		static inline double SL2_FASTCALL					CrtProperToLinear( double _dVal, double _dLw = 1.0, double _dB = 0.0181 ) {
			constexpr double dAlpha1 = 2.6;						// Alpha1 parameter.
			constexpr double dAlpha2 = 3.0;						// Alpha2 parameter.
			constexpr double dVc = 0.35;						// Threshold value for dVc.
			double dK = _dLw / std::pow( 1.0 + _dB, dAlpha1 );	// Coefficient for normalization.

			if ( _dVal < dVc ) {
				return dK * std::pow( dVc + _dB, (dAlpha1 - dAlpha2) ) * std::pow( _dVal + _dB, dAlpha2 );
			}
			return dK * std::pow( _dVal + _dB, dAlpha1 );
		}

		/**
		 * The inverse of CrtProperToLinear().
		 * 
		 * \param _dVal The value to convert.
		 * \param _dLw Screen luminance for white, reference setting is LW = 100 cd/m2.
		 * \param _dB Variable for black level lift (legacy “brightness” control).
		 *	The value of _dB is set so that the calculated luminance can be the same as the
		 *	measurement data at input signal level 0.0183 (= (80-64)/876).
		 *	The value of _dB changes depending on “brightness” control.
		 * \return Returns the corresponding value along a decent CRT curve.
		 **/
		static inline double SL2_FASTCALL					LinearToCrtProper( double _dVal, double _dLw = 1.0, double _dB = 0.0181 ) {
			constexpr double dAlpha1 = 2.6;						// Alpha1 parameter.
			constexpr double dAlpha2 = 3.0;						// Alpha2 parameter.
			constexpr double dVc = 0.35;						// Threshold value for dVc.
			double dK = _dLw / std::pow( 1.0 + _dB, dAlpha1 );	// Coefficient for normalization.

			_dVal /= dK;
			if ( _dVal < std::pow( dVc + _dB, dAlpha1 ) ) {
				return std::pow( _dVal / std::pow( dVc + _dB, (dAlpha1 - dAlpha2) ), 1.0 / dAlpha2 ) - _dB;
			}
			return std::pow( _dVal, 1.0 / dAlpha1 ) - _dB;
		}

		/**
		 * A proper CRT curve based on measurements.
		 * 
		 * \param _dVal The value to convert.
		 * \return Returns the color value converted to linear space.
		 **/
		static inline double SL2_FASTCALL					CrtProper2ToLinear( double _dVal ) {
			constexpr double dAlpha = 0.1115721959217312597711924126997473649680614471435546875;
			constexpr double dBeta = 1.1115721959217312875267680283286608755588531494140625;
			constexpr double dCut = 0.0912863421177801115380390228892792947590351104736328125;
			if ( _dVal >= 0.36 ) { return std::pow( _dVal, 2.31 ); }
			double dFrac = _dVal / 0.36;
			return ((_dVal <= dCut ?
				_dVal / 4.0 :
				std::pow( (_dVal + dAlpha) / dBeta, 1.0 / 0.45 ))
				* (1.0 - dFrac))
				+ (dFrac * std::pow( _dVal, 2.31 ));
		}

		/**
		 * The inverse of CrtProper2ToLinear().
		 *
		 * \param _dVal The value to convert.
		 * \return Returns the value converted to SMPTE 240M space.
		 */
		static inline double SL2_FASTCALL					LinearToCrtProper2( double _dVal ) {
			constexpr double dAlpha = 0.1115721959217312597711924126997473649680614471435546875;
			constexpr double dBeta = 1.1115721959217312875267680283286608755588531494140625;
			constexpr double dCut = 0.022821585529445027884509755722319823689758777618408203125;
			if ( _dVal >= 0.36 ) { return std::pow( _dVal, 1.0 / 2.31 ); }
			double dFrac = _dVal / 0.36;
			return ((_dVal <= dCut ?
				_dVal * 4.0 :
				dBeta * std::pow( _dVal, 0.45 ) - dAlpha)
				* (1.0 - dFrac))
				+ (dFrac * std::pow( _dVal, 1.0 / 2.31 ));
		}

		/**
		 * Converts XYZ values to chromaticities.
		 * 
		 * \param _dX The input X.
		 * \param _dY The input Y.
		 * \param _dZ The input Z.
		 * \param _dChromaX The output chromaticity X.
		 * \param _dChromaY The output chromaticity Y.
		 **/
		static inline void									XYZtoChromaticity( double _dX, double _dY, double _dZ, double &_dChromaX, double &_dChromaY ) {
			double dX = _dX / _dY;
			constexpr double dY = 1.0;
			double dZ = _dZ / _dY;

			_dChromaX = dX / (dX + dY + dZ);
			_dChromaY = dY / (dX + dY + dZ);
		}

		/**
		 * Converts chromaticities to XYZ values.
		 * 
		 * \param _dChromaX The input chromaticity X.
		 * \param _dChromaY The input chromaticity Y.
		 * \param _dY0 The input XYZ Y value.
		 * \param _dX0 The output XYZ Z value.
		 * \param _dZ0 The output XYZ Z value.
		 **/
		static void											ChromaticityToXYZ( double _dChromaX, double _dChromaY, double _dY0, double &_dX0, double &_dZ0 ) {
			_dX0 = _dChromaX * (_dY0 / _dChromaY);
			_dZ0 = (1.0 - _dChromaX - _dChromaY) * (_dY0 / _dChromaY);
		}

		/**
		 * 6-point, 5th-order Hermite X-form sampling.
		 *
		 * \param _pfsSamples The array of 6 input samples, indices -2, -1, 0, 1, 2, and 3.
		 * \param _dFrac The interpolation amount.
		 * \return Returns the interpolated point.
		 */
		static inline double								Sample_6Point_5thOrder_Hermite_X( const double * _pfsSamples, double _dFrac ) {
			// 6-point, 5th-order Hermite (X-form).
			double dEightThym2 = 1.0 / 8.0 * _pfsSamples[-2+2];
			double dElevenTwentyFourThy2 = 11.0 / 24.0 * _pfsSamples[2+2];
			double dTwelvThy3 = 1.0 / 12.0 * _pfsSamples[3+2];
			double dC0 = _pfsSamples[0+2];
			double dC1 = 1.0 / 12.0 * (_pfsSamples[-2+2] - _pfsSamples[2+2]) + 2.0 / 3.0 * (_pfsSamples[1+2] - _pfsSamples[-1+2]);
			double dC2 = 13.0 / 12.0 * _pfsSamples[-1+2] - 25.0 / 12.0 * _pfsSamples[0+2] + 3.0 / 2.0 * _pfsSamples[1+2] -
				dElevenTwentyFourThy2 + dTwelvThy3 - dEightThym2;
			double dC3 = 5.0 / 12.0 * _pfsSamples[0+2] - 7.0 / 12.0 * _pfsSamples[1+2] + 7.0 / 24.0 * _pfsSamples[2+2] -
				1.0 / 24.0 * (_pfsSamples[-2+2] + _pfsSamples[-1+2] + _pfsSamples[3+2]);
			double dC4 = dEightThym2 - 7.0 / 12.0 * _pfsSamples[-1+2] + 13.0 / 12.0 * _pfsSamples[0+2] - _pfsSamples[1+2] +
				dElevenTwentyFourThy2 - dTwelvThy3;
			double dC5 = 1.0 / 24.0 * (_pfsSamples[3+2] - _pfsSamples[-2+2]) + 5.0 / 24.0 * (_pfsSamples[-1+2] - _pfsSamples[2+2]) +
				5.0 / 12.0 * (_pfsSamples[1+2] - _pfsSamples[0+2]);
			return ((((dC5 * _dFrac + dC4) * _dFrac + dC3) * _dFrac + dC2) * _dFrac + dC1) * _dFrac + dC0;
		}

		/**
		 * 4-point, 3rd-order Hermite X-form sampling.
		 *
		 * \param _pfsSamples The array of 6 input samples, indices -1, 0, 1, and 2.
		 * \param _dFrac The interpolation amount.
		 * \return Returns the interpolated point.
		 */
		static inline double								Sample_4Point_3rdhOrder_Hermite_X( const double * _pfsSamples, double _dFrac ) {
			// 4-point, 5th-order Hermite (X-form).
			double dC0 = _pfsSamples[0+1];
			double dC1 = 1.0 / 2.0 * (_pfsSamples[1+1] - _pfsSamples[-1+1]);
			double dC2 = _pfsSamples[-1+1] - 5.0 / 2.0 * _pfsSamples[0+1] + 2.0 * _pfsSamples[1+1] - 1.0 / 2.0 * _pfsSamples[2+1];
			double dC3 = 1.0 / 2.0 * (_pfsSamples[2+1] - _pfsSamples[-1+1]) + 3.0 / 2.0 * (_pfsSamples[0+1] - _pfsSamples[1+1]);
			return ((dC3 * _dFrac + dC2) * _dFrac + dC1) * _dFrac + dC0;
		}

		/**
		 * Standard sinc() function.
		 * 
		 * \param _dX The operand.
		 * \return Returns sin(x) / x.
		 **/
		static inline double								Sinc( double _dX ) {
			_dX *= std::numbers::pi;
			if ( _dX < 0.01 && _dX > -0.01 ) {
				return 1.0 + _dX * _dX * (-1.0 / 6.0 + _dX * _dX * 1.0 / 120.0);
			}

			return std::sin( _dX ) / _dX;
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
		 * Gets an image format from the clipboard and stores its raw data to a vector.  Call within a try/catch block.  All exceptions will be due to a lack of memory allocation.
		 * 
		 * \tparam _tType The vector type into which to store the returned clipboard data.
		 * \param _vData The vector into which to store the found clipboard data.
		 * \return The type of the raw data is returned, or SL2_CF_NONE to indicate an error.
		 **/
		template <typename _tType>
		static inline SL2_CLIP_FORMATS						ImageFromClipBoard( _tType &_vData );

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
			return _mm512_reduce_add_pd( _mReg );
			// Step 1: Reduce 512 bits to 256 bits by summing the high and low 256 bits.
			//__m256d mLow256 = _mm512_castpd512_pd256( _mReg );			// Low 256 bits.
			//__m256d mHigh256 = _mm512_extractf64x4_pd( _mReg, 1 );		// High 256 bits.
			//__m256d mSum256 = _mm256_add_pd( mLow256, mHigh256 );		// Add high and low 256-bit parts.

			//// Step 2: Follow the same 256-bit reduction routine.
			//__m256d mShuf = _mm256_permute2f128_pd( mSum256, mSum256, 0x1 );	// Swap low and high 128-bit halves.
			//__m256d mSums = _mm256_add_pd( mSum256, mShuf );					// Add low and high halves.

			//mShuf = _mm256_shuffle_pd( mSums, mSums, 0x5 );						// Swap the pairs of doubles.
			//mSums = _mm256_add_pd( mSums, mShuf );								// Add the pairs.

			//// Step 3: Extract the scalar value (final sum).
			//return _mm256_cvtsd_f64( mSums );									// Extract the lower double as the sum.
		}

		/**
		 * Horizontally adds all the floats in a given AVX-512 register.
		 * 
		 * \param _mReg The register containing all of the values to sum.
		 * \return Returns the sum of all the floats in the given register.
		 **/
		static inline float									HorizontalSum( __m512 _mReg ) {
			return _mm512_reduce_add_ps( _mReg );
			// Step 1: Reduce 512 bits to 256 bits by permuting and adding high and low 256 bits.
			//__m256 mLow256 = _mm512_castps512_ps256( _mReg );			// Low 256 bits.
			//__m256 mHigh256 = _mm512_extractf32x8_ps( _mReg, 1 );		// High 256 bits.
			//__m256 mSum256 = _mm256_add_ps( mLow256, mHigh256 );		// Add high and low 256-bit parts.

			//// Step 2: Perform horizontal addition on 256 bits.
			//mSum256 = _mm256_hadd_ps( mSum256, mSum256 );				// First horizontal add.
			//mSum256 = _mm256_hadd_ps( mSum256, mSum256 );				// Second horizontal add.

			//// Step 3: Extract the lower float which now contains the sum.
			//return _mm256_cvtss_f32( mSum256 );
		}
#endif	// #ifdef __AVX512F__

#ifdef __AVX__
		/**
		 * Horizontally adds all the floats in a given AVX register.
		 * 
		 * \param _mReg The register containing all of the values to sum.
		 * \return Returns the sum of all the floats in the given register.
		 **/
		static inline double								HorizontalSum( __m256d &_mReg ) {
#if 1
			__m256d mT1 = _mm256_hadd_pd( _mReg, _mReg );
			__m128d mT2 = _mm256_extractf128_pd( mT1, 1 );
			__m128d mT3 = _mm256_castpd256_pd128( mT1 );
			return _mm_cvtsd_f64( _mm_add_pd( mT2, mT3 ) );
#else
			__m256d mShuf = _mm256_permute2f128_pd( _mReg, _mReg, 0x1 );	// Swap the low and high halves.
			__m256d mSums = _mm256_add_pd( _mReg, mShuf );					// Add the low and high halves.

			mShuf = _mm256_shuffle_pd( mSums, mSums, 0x5 );					// Swap the pairs of doubles.
			mSums = _mm256_add_pd( mSums, mShuf );							// Add the pairs.

			return _mm256_cvtsd_f64( mSums );								// Extract the sum.
#endif	// #if 0
		}

		/**
		 * Horizontally adds all the floats in a given AVX register.
		 * 
		 * \param _mReg The register containing all of the values to sum.
		 * \return Returns the sum of all the floats in the given register.
		 **/
		static inline float									HorizontalSum( const __m256 &_mReg ) {
			SL2_ALIGN( 32 )
			float fSumArray[8];
			__m256 mTmp = _mm256_hadd_ps( _mReg, _mReg );
			mTmp = _mm256_hadd_ps( mTmp, mTmp );
			_mm256_store_ps( fSumArray, mTmp );
			return fSumArray[0] + fSumArray[4];
#if 0
			__m256 mTmp = _mm256_permute2f128_ps(_mReg, _mReg, 1);	// Shuffle high 128 to low.
			mTmp = _mm256_add_ps( _mReg, mTmp );					// Add high and low parts.

			mTmp = _mm256_hadd_ps( mTmp, mTmp );					// First horizontal add.
			mTmp = _mm256_hadd_ps( mTmp, mTmp );					// Second horizontal add.

			// Extract the lower float which now contains the sum.
			return _mm256_cvtss_f32( mTmp );
#endif	// #if 0
		}
#endif	// #ifdef __AVX__

#ifdef __SSE4_1__
		/**
		 * Horizontally adds all the floats in a given SSE register.
		 * 
		 * \param _mReg The register containing all of the values to sum.
		 * \return Returns the sum of all the floats in the given register.
		 **/
		static inline double								HorizontalSum( const __m128d &_mReg ) {
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
		static inline float									HorizontalSum( const __m128 &_mReg ) {
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
	/**
	 * Gets an image format from the clipboard and stores its raw data to a vector.  Call within a try/catch block.  All exceptions will be due to a lack of memory allocation.
	 * 
	 * \tparam _tType The vector type into which to store the returned clipboard data.
	 * \param _vData The vector into which to store the found clipboard data.
	 * \return The type of the raw data is returned, or SL2_CF_NONE to indicate an error.
	 **/
	template <typename _tType>
	inline CUtilities::SL2_CLIP_FORMATS CUtilities::ImageFromClipBoard( _tType &_vData ) {
#ifdef _WIN32
		UINT uiFmt = ::RegisterClipboardFormatW( L"PNG" );
		CUtilities::SL2_CLIPBOARD cbClip;
		if ( uiFmt ) {
			if ( cbClip.GetData( uiFmt, _vData ) ) { return SL2_CF_PNG; }
		}
		if ( cbClip.GetData( CF_DIB, _vData ) ) { return SL2_CF_DIB; }
		if ( cbClip.GetData( CF_DIBV5, _vData ) ) { return SL2_CF_DIBV5; }
		return SL2_CF_NONE;
#elif defined( __APPLE__ )
		@autoreleasepool {
			NSPasteboard *	pPb		= [NSPasteboard generalPasteboard];
			NSData *		pPng	= [pPb dataForType:NSPasteboardTypePNG];
			if ( pPng ) {
				_vData.resize( static_cast<size_t>([pPng length]) );
				std::memcpy( _vData.data(), [pPng bytes], _vData.size() );
				return SL2_CF_PNG;
			}
			// Try TIFF.
			NSData *		pTiff	= [pPb dataForType:NSPasteboardTypeTIFF];
			if ( pTiff ) {
				_vData.resize( static_cast<size_t>([pTiff length]) );
				std::memcpy( _vData.data(), [pTiff bytes], _vData.size() );
				return SL2_CF_TIFF;
			}
			return SL2_CF_NONE;
		}
#elif defined(__linux__)
		// requires linking to gtk and initializing before use
		if ( !::gtk_init_check( nullptr, nullptr ) ) { return SL2_CF_NONE; }

		GtkClipboard *	pCb		= ::gtk_clipboard_get( GDK_SELECTION_CLIPBOARD );
		GdkPixbuf *		pPixbuf	= ::gtk_clipboard_wait_for_image( pCb );
		if ( pPixbuf ) {
			guchar *	pBuf	= nullptr;
			gsize		sBufLen	= 0;
			if ( ::gdk_pixbuf_save_to_buffer(	pPixbuf,
												reinterpret_cast<gchar **>(&pBuf),
												&sBufLen,
												"png",
												nullptr, nullptr ) ) {
				_vData.assign( pBuf, pBuf + sBufLen );
				::g_free( pBuf );
				::g_object_unref( pPixbuf );
				return SL2_CI_PNG;
			}
			::g_object_unref( pPixbuf );
		}
		return SL2_CF_NONE;
#endif	// #ifdef _WIN32
	}

}	// namespace sl2

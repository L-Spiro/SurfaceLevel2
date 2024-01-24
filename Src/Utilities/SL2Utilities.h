/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Useful utility functions.
 */


#pragma once

#include <string>
#include <vector>


#ifndef SL2_ELEMENTS
#define SL2_ELEMENTS( x )									((sizeof( x ) / sizeof( 0[x] )) / (static_cast<size_t>(!(sizeof( x ) % sizeof(0[x])))))
#endif	// #ifndef SL2_ELEMENTS

#ifndef SL2_PI
#define SL2_PI												3.1415926535897932384626433832795
#endif	// #ifndef SL2_PI


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

	};

}	// namespace sl2

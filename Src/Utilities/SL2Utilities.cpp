/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Useful utility functions.
 */

#include "SL2Utilities.h"
#include "../OS/SL2Os.h"

namespace sl2 {

	// == Functions.
	/**
	 * Creates a string with _cReplaceMe replaced with _cWithMe inside _s16String.
	 *
	 * \param _s16String The string in which replacements are to be made.
	 * \param _cReplaceMe The character to replace.
	 * \param _cWithMe The character with which to replace _cReplaceMe.
	 * \return Returns the new string with the given replacements made.
	 */
	std::u16string CUtilities::Replace( const std::u16string &_s16String, char16_t _cReplaceMe, char16_t _cWithMe ) {
		std::u16string s16Copy = _s16String;
		auto aFound = s16Copy.find( _cReplaceMe );
		while ( aFound != std::string::npos ) {
			s16Copy[aFound] = _cWithMe;
			aFound = s16Copy.find( _cReplaceMe, aFound + 1 );
		}
		return s16Copy;
	}

	/**
	 * Replaces a string inside a data vector.
	 * 
	 * \param _vData The buffer of data in which to replace a string.
	 * \param _sReplaceMe The string to replace.
	 * \param _sWithMe The string with which to replace _sReplaceMe inside _vData.
	 * \return Returns a reference to _vData.
	 **/
	std::vector<uint8_t> & CUtilities::Replace( std::vector<uint8_t> &_vData, const std::string &_sReplaceMe, const std::string &_sWithMe ) {
		if ( _sReplaceMe.size() > _vData.size() || _sReplaceMe.size() == 0 ) { return _vData; }

		for ( size_t I = 0; I < _vData.size() - _sReplaceMe.size(); ) {
			if ( std::memcmp( _vData.data() + I, reinterpret_cast<const uint8_t *>(_sReplaceMe.c_str()), _sReplaceMe.size() ) == 0 ) {
				_vData.erase( _vData.begin() + I, _vData.begin() + I + _sReplaceMe.size() );
				for ( size_t J = I; J < I + _sWithMe.size(); ++J ) {
					_vData.insert( _vData.begin() + J, _sWithMe.c_str()[J-I] );
				}
				I += _sWithMe.size();
				continue;
			}
			++I;
		}
		return _vData;
	}

	/**
	 * Converts a UTF-8 string to a UTF-16 string.  The resulting string may have allocated more characters than necessary but will be terminated with a NULL.
	 *
	 * \param _pcString String to convert.
	 * \param _pbErrored If not nullptr, holds a returned boolean indicating success or failure of the conversion.
	 * \return Returns the converted UTF-16 string.
	 */
	std::u16string CUtilities::Utf8ToUtf16( const char8_t * _pcString, bool * _pbErrored ) {
#ifdef SL2_WINDOWS
		if ( _pbErrored != nullptr ) { (*_pbErrored) = true; }
		std::u16string wsOutput;
		int iLen = ::MultiByteToWideChar( CP_UTF8, MB_ERR_INVALID_CHARS, reinterpret_cast<LPCCH>(_pcString), -1, reinterpret_cast<LPWSTR>(wsOutput.data()), static_cast<int>(wsOutput.size()) );
		if ( iLen <= 0 ) { return wsOutput; }
		wsOutput.resize( iLen + 10 );	// Pretty sure it doesn't have to be + 10; + 1 would probably be fine but an example on MSDN uses + 10 so meh, let's just go with it.
		iLen = ::MultiByteToWideChar( CP_UTF8, MB_ERR_INVALID_CHARS, reinterpret_cast<LPCCH>(_pcString), -1, reinterpret_cast<LPWSTR>(wsOutput.data()), static_cast<int>(wsOutput.size()) );
		if ( iLen <= 0 ) {
			wsOutput.clear();
			return wsOutput;
		}
		if ( _pbErrored != nullptr ) { (*_pbErrored) = false; }
		while ( wsOutput.size() && !wsOutput[wsOutput.size()-1] ) { wsOutput.pop_back(); }
		return wsOutput;
#else
		// Visual Studio reports these as deprecated since C++17.
		if ( _pbErrored != nullptr ) { (*_pbErrored) = false; }
		try {
			return std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t>{}.from_bytes( reinterpret_cast<const char *>(_pcString) );
		}
		catch ( ... ) { 
			if ( _pbErrored != nullptr ) { (*_pbErrored) = true; }
			return std::u16string();
		}
#endif	// #ifdef SL2_WINDOWS
	}

	/**
	 * Converts a UTF-16 string to a UTF-8 string.  The resulting string may have allocated more characters than necessary but will be terminated with a NULL.
	 *
	 * \param _pcPath String to convert.
	 * \param _pbErrored If not nullptr, holds a returned boolean indicating success or failure of the conversion.
	 * \return Returns the converted UTF-8 string.
	 */
	std::string CUtilities::Utf16ToUtf8( const char16_t * _pcString, bool * _pbErrored ) {
#ifdef SL2_WINDOWS
		if ( _pbErrored != nullptr ) { (*_pbErrored) = true; }
		std::string sOutput;
		int iLen = ::WideCharToMultiByte( CP_UTF8, WC_ERR_INVALID_CHARS, reinterpret_cast<LPCWCH>(_pcString), -1, sOutput.data(), static_cast<int>(sOutput.size()), NULL, NULL );
		if ( iLen <= 0 ) { return sOutput; }
		sOutput.resize( iLen + 10 );
		iLen = ::WideCharToMultiByte( CP_UTF8, WC_ERR_INVALID_CHARS, reinterpret_cast<LPCWCH>(_pcString), -1, sOutput.data(), static_cast<int>(sOutput.size()), NULL, NULL );
		if ( iLen <= 0 ) {
			sOutput.clear();
			return sOutput;
		}
		if ( _pbErrored != nullptr ) { (*_pbErrored) = false; }
		while ( sOutput.size() && !sOutput[sOutput.size()-1] ) { sOutput.pop_back(); }
		return sOutput;
#else
		// Visual Studio reports these as deprecated since C++17.
		if ( _pbErrored != nullptr ) { (*_pbErrored) = false; }
		try {
			return std::wstring_convert<std::codecvt_utf8<wchar_t>>{}.to_bytes( reinterpret_cast<const wchar_t *>(_pcString) );
		}
		catch ( ... ) { 
			if ( _pbErrored != nullptr ) { (*_pbErrored) = true; }
			return std::string();
		}
#endif	// #ifdef SL2_WINDOWS
	}

	/**
	 * Reads a line from a buffer.
	 * 
	 * \param _vBuffer The buffer from which to read a line.
	 * \param _stPos The current position inside the buffer, updated on return.
	 * \return Returns the line read from the file.
	 **/
	std::string CUtilities::ReadLine( const std::vector<uint8_t> &_vBuffer, size_t &_stPos ) {
		std::string sTmp;
		while ( _stPos < _vBuffer.size() ) {
			uint8_t ui8This = _vBuffer[_stPos++];
			if ( ui8This == '\r' ) { continue; }
			if ( ui8This == '\n' ) { break; }
			sTmp.push_back( ui8This );
		}

		return sTmp;
	}

	/**
	 * Tokenizes a string by a given character.
	 * 
	 * \param _sString The string to tokenize.
	 * \param _vtDelimiter The character by which to deliminate the string into sections.
	 * \param _bAllowEmptyStrings If true, the return value could contain empty strings when the delimiter is found more than once in a row.
	 * \return DESC
	 **/
	std::vector<std::string> CUtilities::Tokenize( const std::string &_sString, std::string::value_type _vtDelimiter, bool _bAllowEmptyStrings ) {
		std::vector<std::string> vRet;
		std::string sTmp;
		for ( size_t I = 0; I < _sString.size(); ++I ) {
			if ( _sString[I] == _vtDelimiter ) {
				if ( _bAllowEmptyStrings || sTmp.size() ) {
					vRet.push_back( sTmp );
					sTmp.clear();
				}
			}
			else {
				sTmp.push_back( _sString[I] );
			}
		}
		if ( sTmp.size() ) {
			vRet.push_back( sTmp );
		}
		return vRet;
	}

	/**
	 * Gets the extension from a file path.
	 *
	 * \param _s16Path The file path whose extension is to be obtained.
	 * \return Returns a string containing the file extension.
	 */
	std::u16string CUtilities::GetFileExtension( const std::u16string &_s16Path ) {
		std::u16string s16File = GetFileName( _s16Path );
		std::string::size_type stFound = s16File.rfind( u'.' );
		if ( stFound == std::string::npos ) { return std::u16string(); }
		return s16File.substr( stFound + 1 );
	}

	/**
	 * Removes the extension from a file path.
	 *
	 * \param _s16Path The file path whose extension is to be removed.
	 * \return Returns a string containing the file mname without the extension.
	 */
	std::u16string CUtilities::NoExtension( const std::u16string &_s16Path ) {
		std::u16string s16File = GetFileName( _s16Path );
		std::string::size_type stFound = s16File.rfind( u'.' );
		if ( stFound == std::string::npos ) { return std::u16string(); }
		return s16File.substr( 0, stFound );
	}

	/**
	 * Gets the file name from a file path.
	 *
	 * \param _s16Path The file path whose name is to be obtained.
	 * \return Returns a string containing the file name.
	 */
	std::u16string CUtilities::GetFileName( const std::u16string &_s16Path ) {
		// If the last character is } then it is a file inside a ZIP.
		if ( _s16Path.size() && _s16Path[_s16Path.size()-1] == u'}' ) {
			std::string::size_type stFound = _s16Path.rfind( u'{' );
			std::u16string s16File = _s16Path.substr( stFound + 1 );
			s16File.pop_back();
			return s16File;
		}
		std::u16string s16Normalized = Replace( _s16Path, u'/', u'\\' );
		std::string::size_type stFound = s16Normalized.rfind( u'\\' );
		std::u16string s16File = s16Normalized.substr( stFound + 1 );

		return s16File;
	}

	/**
	 * Gets the file path without the file name
	 *
	 * \param _s16Path The file path whose path is to be obtained.
	 * \return Returns a string containing the file path.
	 */
	std::u16string CUtilities::GetFilePath( const std::u16string &_s16Path ) {
		if ( _s16Path.size() ) {
			std::u16string s16Normalized = Replace( _s16Path, u'/', u'\\' );
			std::string::size_type stFound = s16Normalized.rfind( u'\\' );
			if ( stFound >= s16Normalized.size() ) { return std::u16string(); }
			std::u16string s16File = s16Normalized.substr( 0, stFound + 1 );
			return s16File;
		}
		return std::u16string();
	}

	/**
	 * Gets the lowest power-of-2 value not below the given input value.
	 *
	 * \param _ui32Value Value for which to derive the lowest power-of-2 value not under this value.
	 * \return Returns the lowest power-of-2 value not below the given input value.
	 */
	uint32_t CUtilities::GetLowestPo2( uint32_t _ui32Value ) {
		if ( !_ui32Value ) { return 0; }
#ifdef SL2_X86
		// On x86 processors there is an instruction that gets the highest-
		//	set bit automatically.
		uint32_t ui32Ret;
		SL2_ASM_BEGIN
			xor eax, eax
			bsr eax, _ui32Value
			mov ui32Ret, eax
		SL2_ASM_END
		ui32Ret = 1 << ui32Ret;
		return (ui32Ret == _ui32Value) ? ui32Ret : ui32Ret << 1;
#else	// SL2_X86
		// Get it the hard way.
		uint32_t ui32Ret = 1;
		while ( ui32Ret < _ui32Value ) { ui32Ret <<= 1; }

		// By now, ui32Ret either equals _ui32Value or is the next power of 2 up.
		// If they are equal, _ui32Value is already a power of 2.
		return ui32Ret;
#endif	// SL2_X86
	}

}	// namespace sl2

/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The base class for working with files.
 */


#include "SL2FileBase.h"
#include "../OS/SL2Os.h"
#include "../Utilities/SL2Utilities.h"

namespace sl2 {

	CFileBase::~CFileBase() {}

	// == Functions.
	/**
	 * Opens a file.  The path is given in UTF-8.
	 *
	 * \param _pcPath Path to the file to open.
	 * \return Returns true if the file was opened, false otherwise.
	 */
	bool CFileBase::Open( const char8_t * _pcFile ) {
		bool bErrored;
		std::u16string swTmp = CUtilities::Utf8ToUtf16( _pcFile, &bErrored );
		if ( bErrored ) { return false; }
		return Open( swTmp.c_str() );
	}

	/**
	 * Opens a file.  The path is given in UTF-16.
	 *
	 * \param _pcPath Path to the file to open.
	 * \return Returns true if the file was opened, false otherwise.
	 */
	bool CFileBase::Open( const char16_t * _pcFile ) {
		bool bErrored;
		std::string sTmp = CUtilities::Utf16ToUtf8( _pcFile, &bErrored );
		if ( bErrored ) { return false; }
		return Open( reinterpret_cast<const char8_t *>(sTmp.c_str()) );
	}

	/**
	 * Creates a file.  The path is given in UTF-8.
	 *
	 * \param _pcPath Path to the file to create.
	 * \return Returns true if the file was created, false otherwise.
	 */
	bool CFileBase::Create( const char8_t * _pcFile ) {
		bool bErrored;
		std::u16string swTmp = CUtilities::Utf8ToUtf16( _pcFile, &bErrored );
		if ( bErrored ) { return false; }
		return Create( swTmp.c_str() );
	}

	/**
	 * Creates a file.  The path is given in UTF-16.
	 *
	 * \param _pcPath Path to the file to create.
	 * \return Returns true if the file was created, false otherwise.
	 */
	bool CFileBase::Create( const char16_t * _pcFile ) {
		bool bErrored;
		std::string sTmp = CUtilities::Utf16ToUtf8( _pcFile, &bErrored );
		if ( bErrored ) { return false; }
		return Create( reinterpret_cast<const char8_t *>(sTmp.c_str()) );
	}

	/**
	 * Closes the opened file.
	 */
	void CFileBase::Close() {}

	/**
	 * If true, the file is an archive containing more files.
	 *
	 * \return Returns true if the file is an archive, false otherwise.
	 */
	bool CFileBase::IsArchive() const { return false; }

	/**
	 * Loads the opened file to memory, storing the result in _vResult.
	 *
	 * \param _vResult The location where to store the file in memory.
	 * \return Returns true if the file was successfully loaded into memory.
	 */
	bool CFileBase::LoadToMemory( std::vector<uint8_t> &/*_vResult*/ ) const { return false; }

	/**
	 * Gathers the file names in the archive into an array.
	 *
	 * \param _vResult The location where to store the file names.
	 * \return Returns true if the file names were successfully added to the given array.  A return of false will typically indicate that the file is not opened or that it is not a valid .ZIP file.
	 */
	bool CFileBase::GatherArchiveFiles( std::vector<std::u16string> &/*_vResult*/ ) const { return false; }

	/**
	 * Gathers the file names in the archive into an array.
	 *
	 * \param _s16File The name of the file to extract.
	 * \param _vResult The location where to store the file in memory.
	 * \return Returns true if the file was extracted successfully.
	 */
	bool CFileBase::ExtractToMemory( const std::u16string &/*_s16File*/, std::vector<uint8_t> &/*_vResult*/ ) const { return false; }

	/**
	 * Finds files/folders in a given directory.
	 * 
	 * \param _pcFolderPath The path to the directory to search.
	 * \param _pcSearchString A wildcard search string to find only certain files/folders.
	 * \param _bIncludeFolders If true, folders are included in the return.
	 * \param _vResult The return array.  Found files and folders are appended to the array.
	 * \return Returns _vResult.
	 **/
	std::vector<std::u16string> & CFileBase::FindFiles( const char16_t * _pcFolderPath, const char16_t * _pcSearchString, bool _bIncludeFolders, std::vector<std::u16string> &_vResult ) {
#ifdef SL2_WINDOWS
		std::u16string sPath = CUtilities::Replace( _pcFolderPath, u'/', u'\\' );
		while ( sPath.size() && sPath[sPath.size()-1] == u'\\' ) { sPath.pop_back(); }
		sPath.push_back( u'\\' );
		std::u16string sSearch;
		if ( _pcSearchString ) {
			sSearch = CUtilities::Replace( _pcSearchString, u'/', u'\\' );
			while ( sSearch[0] == u'\\' ) {
				sSearch.erase( sSearch.begin() );
			}
		}
		else {
			sSearch = u"*";
		}


		std::u16string sSearchPath = sPath + sSearch;
		/*if ( sSearchPath.FindString( 0, L"\\\\?\\" ).ui32Start != 0 ) {
			if ( !sSearchPath.Insert( 0, L"\\\\?\\" ) ) { return false; }
		}*/
		WIN32_FIND_DATAW wfdData;
		HANDLE hDir = ::FindFirstFileW( reinterpret_cast<LPCWSTR>(sSearchPath.c_str()), &wfdData );
		if ( INVALID_HANDLE_VALUE == hDir ) { return _vResult; }
		
		do {
			if ( wfdData.cFileName[0] == u'.' ) { continue; }
			bool bIsFolder = ((wfdData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0);
			if ( !_bIncludeFolders && bIsFolder ) { continue; }
			try {
				_vResult.push_back( sPath + reinterpret_cast<const char16_t *>(wfdData.cFileName) );
			}
			catch ( ... ) {
				::FindClose( hDir );
				return _vResult;
			}
		} while ( ::FindNextFileW( hDir, &wfdData ) );

		::FindClose( hDir );
		return _vResult;
#else
#error "CFileBase::FindFiles() unimplemented!"
#endif	// #ifdef SL2_WINDOWS
	}

	/**
	 * Gets the extension from a file path.
	 *
	 * \param _s16Path The file path whose extension is to be obtained.
	 * \return Returns a string containing the file extension.
	 */
	std::u16string CFileBase::GetFileExtension( const std::u16string &_s16Path ) {
		std::u16string s16File = GetFileName( _s16Path );
		std::string::size_type stFound = s16File.rfind( u'.' );
		if ( stFound == std::string::npos ) { return std::u16string(); }
		return s16File.substr( stFound + 1 );
	}

	/**
	 * Compares the extention from a given file path to a given extension string.
	 * 
	 * \param _s16Path The file path whose extension is to be checked.
	 * \param _pcExt The extension to check for being in _s16Path.
	 * \return Returns true if the given file path's extension matches _pcExt.
	 **/
	bool CFileBase::CmpFileExtension( const std::u16string &_s16Path, const char16_t * _pcExt ) {
		return ::_wcsicmp( reinterpret_cast<const wchar_t *>(sl2::CFileBase::GetFileExtension( _s16Path ).c_str()),
			reinterpret_cast<const wchar_t *>(_pcExt) ) == 0;
	}

	/**
	 * Removes the extension from a file path.
	 *
	 * \param _s16Path The file path whose extension is to be removed.
	 * \return Returns a string containing the file mname without the extension.
	 */
	std::u16string CFileBase::NoExtension( const std::u16string &_s16Path ) {
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
	std::u16string CFileBase::GetFileName( const std::u16string &_s16Path ) {
		// If the last character is } then it is a file inside a ZIP.
		if ( _s16Path.size() && _s16Path[_s16Path.size()-1] == u'}' ) {
			std::string::size_type stFound = _s16Path.rfind( u'{' );
			std::u16string s16File = _s16Path.substr( stFound + 1 );
			s16File.pop_back();
			return s16File;
		}
		std::u16string s16Normalized = CUtilities::Replace( _s16Path, u'/', u'\\' );
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
	std::u16string CFileBase::GetFilePath( const std::u16string &_s16Path ) {
		if ( _s16Path.size() ) {
			std::u16string s16Normalized = CUtilities::Replace( _s16Path, u'/', u'\\' );
			std::string::size_type stFound = s16Normalized.rfind( u'\\' );
			if ( stFound >= s16Normalized.size() ) { return std::u16string(); }
			std::u16string s16File = s16Normalized.substr( 0, stFound + 1 );
			return s16File;
		}
		return std::u16string();
	}

}	// namespace sl2

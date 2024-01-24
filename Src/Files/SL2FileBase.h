/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: The base class for working with files.
 */


#pragma once

#include <string>
#include <vector>

namespace sl2 {

	/**
	 * Class CFileBase
	 * \brief The base class for working with files.
	 *
	 * Description: The base class for working with files.
	 */
	class CFileBase {
	public :
		virtual ~CFileBase();


		// == Functions.
		/**
		 * Opens a file.  The path is given in UTF-8.
		 *
		 * \param _pcPath Path to the file to open.
		 * \return Returns true if the file was opened, false otherwise.
		 */
		virtual bool										Open( const char8_t * _pcFile );

		/**
		 * Opens a file.  The path is given in UTF-16.
		 *
		 * \param _pcPath Path to the file to open.
		 * \return Returns true if the file was opened, false otherwise.
		 */
		virtual bool										Open( const char16_t * _pcFile );

		/**
		 * Creates a file.  The path is given in UTF-8.
		 *
		 * \param _pcPath Path to the file to create.
		 * \return Returns true if the file was created, false otherwise.
		 */
		virtual bool										Create( const char8_t * _pcFile );

		/**
		 * Creates a file.  The path is given in UTF-16.
		 *
		 * \param _pcPath Path to the file to create.
		 * \return Returns true if the file was created, false otherwise.
		 */
		virtual bool										Create( const char16_t * _pcFile );

		/**
		 * Closes the opened file.
		 */
		virtual void										Close();

		/**
		 * If true, the file is an archive containing more files.
		 *
		 * \return Returns true if the file is an archive, false otherwise.
		 */
		virtual bool										IsArchive() const;

		/**
		 * Loads the opened file to memory, storing the result in _vResult.
		 *
		 * \param _vResult The location where to store the file in memory.
		 * \return Returns true if the file was successfully loaded into memory.
		 */
		virtual bool										LoadToMemory( std::vector<uint8_t> &_vResult ) const;

		/**
		 * Gathers the file names in the archive into an array.
		 *
		 * \param _vResult The location where to store the file names.
		 * \return Returns true if the file names were successfully added to the given array.  A return of false will typically indicate that the file is not opened or that it is not a valid .ZIP file.
		 */
		virtual bool										GatherArchiveFiles( std::vector<std::u16string> &_vResult ) const;

		/**
		 * Gathers the file names in the archive into an array.
		 *
		 * \param _s16File The name of the file to extract.
		 * \param _vResult The location where to store the file in memory.
		 * \return Returns true if the file was extracted successfully.
		 */
		virtual bool										ExtractToMemory( const std::u16string &_s16File, std::vector<uint8_t> &_vResult ) const;

		/**
		 * Finds files/folders in a given directory.
		 * 
		 * \param _pcFolderPath The path to the directory to search.
		 * \param _pcSearchString A wildcard search string to find only certain files/folders.
		 * \param _bIncludeFolders If true, folders are included in the return.
		 * \param _vResult The return array.  Found files and folders are appended to the array.
		 * \return Returns _vResult.
		 **/
		static std::vector<std::u16string> &				FindFiles( const char16_t * _pcFolderPath, const char16_t * _pcSearchString, bool _bIncludeFolders, std::vector<std::u16string> &_vResult );

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
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Functions.

}	// namespace sl2

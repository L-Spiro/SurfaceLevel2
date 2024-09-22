/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A class for opening files using the standard C library (FILE).
 */


#pragma once

#include "../OS/SL2Os.h"
#include "SL2FileBase.h"

namespace sl2 {

	/**
	 * Class CStdFile
	 * \brief A class for opening files using the standard C library (FILE).
	 *
	 * Description: A class for opening files using the standard C library (FILE).
	 */
	class CStdFile : public CFileBase {
	public :
		CStdFile();
		virtual ~CStdFile();


		// == Functions.
#ifdef SL2_WINDOWS
		/**
		 * Opens a file.  The path is given in UTF-8.
		 *
		 * \param _pcPath Path to the file to open.
		 * \return Returns true if the file was opened, false otherwise.
		 */
		virtual bool										Open( const char8_t * _pcFile ) { return CFileBase::Open( _pcFile ); }

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
		virtual bool										Create( const char8_t * _pcFile ) { return CFileBase::Create( _pcFile ); }

		/**
		 * Creates a file.  The path is given in UTF-16.
		 *
		 * \param _pcPath Path to the file to create.
		 * \return Returns true if the file was created, false otherwise.
		 */
		virtual bool										Create( const char16_t * _pcFile );
#else
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
		virtual bool										Open( const char16_t * _pcFile ) { return CFileBase::Open( _pcFile ); }

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
		virtual bool										Create( const char16_t * _pcFile ) { return CFileBase::Create( _pcFile ); }
#endif	// #ifdef SL2_WINDOWS

		/**
		 * Closes the opened file.
		 */
		virtual void										Close();

		/**
		 * Loads the opened file to memory, storing the result in _vResult.
		 *
		 * \param _vResult The location where to store the file in memory.
		 * \return Returns true if the file was successfully loaded into memory.
		 */
		virtual bool										LoadToMemory( std::vector<uint8_t> &_vResult ) const;

		/**
		 * Writes the given data to the created file.  File must have been cerated with Create().
		 *
		 * \param _vData The data to write to the file.
		 * \return Returns true if the data was successfully written to the file.
		 */
		virtual bool										WriteToFile( const std::vector<uint8_t> &_vData );

		/**
		 * Writes the given data to the created file.  File must have been cerated with Create().
		 *
		 * \param _pui8Data The data to write to the file.
		 * \param _tsSize The size of the buffer to which _pui8Data points.
		 * \return Returns true if the data was successfully written to the file.
		 */
		virtual bool										WriteToFile( const uint8_t * _pui8Data, size_t _tsSize );

		/**
		 * Loads the opened file to memory, storing the result in _vResult.
		 *
		 * \param _pcFile The file to open.
		 * \param _vResult The location where to store the file in memory.
		 * \return Returns true if the file was successfully loaded into memory.
		 **/
		template <typename _tStrType>
		static inline bool									LoadToMemory( const _tStrType * _pcFile, std::vector<uint8_t> &_vResult );

		/**
		 * Writes the given data to the created file.
		 *
		 * \param _pcFile The file to create.
		 * \param _pui8Data The data to write to the file.
		 * \param _tsSize The size of the buffer to which _pui8Data points.
		 * \return Returns true if the data was successfully written to the file.
		 */
		template <typename _tStrType>
		static inline bool									WriteToFile( const _tStrType * _pcFile, const uint8_t * _pui8Data, size_t _tsSize );

		/**
		 * Writes the given data to the created file.
		 *
		 * \param _pcFile The file to create.
		 * \param _vData The data to write to the file.
		 * \return Returns true if the data was successfully written to the file.
		 */
		template <typename _tStrType>
		static inline bool									WriteToFile( const _tStrType * _pcFile, const std::vector<uint8_t> &_vData );


	protected :
		// == Members.
		FILE *												m_pfFile;							/**< The FILE object to maintain. */
		uint64_t											m_ui64Size;							/**< The file size. */


		// == Functions.
		/**
		 * Performs post-loading operations after a successful loading of the file.  m_pfFile will be valid when this is called.  Override to perform additional loading operations on m_pfFile.
		 */
		virtual void										PostLoad();
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Functions.
	/**
	 * Loads the opened file to memory, storing the result in _vResult.
	 *
	 * \param _pcFile The file to open.
	 * \param _vResult The location where to store the file in memory.
	 * \return Returns true if the file was successfully loaded into memory.
	 **/
	template <typename _tStrType>
	inline bool CStdFile::LoadToMemory( const _tStrType * _pcFile, std::vector<uint8_t> &_vResult ) {
		CStdFile sfFile;
		if ( !sfFile.Open( _pcFile ) ) { return false; }
		return sfFile.LoadToMemory( _vResult );
	}

	/**
	 * Writes the given data to the created file.
	 *
	 * \param _pcFile The file to create.
	 * \param _pui8Data The data to write to the file.
	 * \param _tsSize The size of the buffer to which _pui8Data points.
	 * \return Returns true if the data was successfully written to the file.
	 */
	template <typename _tStrType>
	inline bool CStdFile::WriteToFile( const _tStrType * _pcFile, const uint8_t * _pui8Data, size_t _tsSize ) {
		CStdFile sfFile;
		if ( !sfFile.Create( _pcFile ) ) { return false; }
		return sfFile.WriteToFile( _pui8Data, _tsSize );
	}

	/**
	 * Writes the given data to the created file.
	 *
	 * \param _pcFile The file to create.
	 * \param _vData The data to write to the file.
	 * \return Returns true if the data was successfully written to the file.
	 */
	template <typename _tStrType>
	inline bool CStdFile::WriteToFile( const _tStrType * _pcFile, const std::vector<uint8_t> &_vData ) {
		return WriteToFile( _pcFile, _vData.data(), _vData.size() );
	}

}	// namespace sl2

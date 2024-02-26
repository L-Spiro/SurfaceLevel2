/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A DDS reader and writer.
 */

#include "SL2Dds.h"
#include "../../Utilities/SL2Stream.h"


namespace sl2 {

	CDds::CDds() {
	}
	CDds::~CDds() {
	}


	// == Functions.
	/**
	 * Loads a DDS file from memory.
	 *
	 * \param _vFileData The in-memory image of the file.
	 * \return Returns true if the file was successfully loaded.  False indicates an invalid file or lack of RAM.
	 */
	bool CDds::LoadDds( const std::vector<uint8_t> &_vFileData ) {
		CStream sStream( _vFileData );
		uint32_t ui32Magic;
		if ( !sStream.Read( ui32Magic ) ) { return false; }
		if ( ui32Magic != 0x20534444 ) { return false; }

		LSI_DDS_HEADER dhHeader;
		if ( !sStream.Read( dhHeader ) ) { return false; }
		
		if ( dhHeader.dpPixelFormat.ui32Size != sizeof( dhHeader.dpPixelFormat ) ) { return false; }

		if ( (dhHeader.ui32Flags & (LSI_DF_CAPS | LSI_DF_HEIGHT | LSI_DF_WIDTH | LSI_DF_PIXELFORMAT)) !=
			(LSI_DF_CAPS | LSI_DF_HEIGHT | LSI_DF_WIDTH | LSI_DF_PIXELFORMAT) ) {
			if ( (dhHeader.ui32Flags & (LSI_DF_HEIGHT | LSI_DF_WIDTH)) !=
				(LSI_DF_HEIGHT | LSI_DF_WIDTH) ) {
				return false;
			}
		}

		if ( dhHeader.dpPixelFormat.ui32Flags & LSI_DPFF_FOURCC ) {
			switch ( dhHeader.dpPixelFormat.ui32FourCC ) {
			}
		}
	}

}	// namespace sl2

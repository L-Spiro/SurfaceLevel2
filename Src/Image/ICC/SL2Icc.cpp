/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Loading and working with ICC profiles.
 */

#include "SL2Icc.h"


namespace sl2 {

	// == Functions.
	/**
	 * Gets the offset and size of the given tag.
	 *
	 * \param _pui8Profile The ICC profile.
	 * \param _sProfileSize The size of the ICC profile.
	 * \param _ui32Sig The tag whose data is to be found.
	 * \param _sSize Holds teh returned size of the curve data or 0 upon failure.
	 * \return Returns the offset of the curve data or 0 upon failure.
	 **/
	size_t CIcc::GetTagDataOffset( const uint8_t * _pui8Profile, size_t _sProfileSize,
		uint32_t _ui32Sig, size_t &_sSize ) {
		_sSize = 0;
		if ( _sProfileSize < sizeof( icHeader ) + sizeof( uint32_t ) ) { return 0; }

#define SL2_R32( OFFSET )					::_byteswap_ulong( (*reinterpret_cast<const uint32_t *>(&_pui8Profile[OFFSET])) )
#define SL2_CHECK( OFFSET, TYPE )			if ( OFFSET + sizeof( TYPE ) > _sProfileSize ) { return 0; }
		uint32_t ui32Count = SL2_R32( sizeof( icHeader ) );
		const icTagList * ptlTags = reinterpret_cast<const icTagList *>(&_pui8Profile[sizeof( icHeader )]);
		for ( uint32_t I = 0; I < ui32Count; ++I ) {
			SL2_CHECK( sizeof( icHeader ) + sizeof( uint32_t ) + I * sizeof( icTag ), sizeof( icTag ) );
			if ( ::_byteswap_ulong( ptlTags->tags[I].sig ) == _ui32Sig ) {
				SL2_CHECK( ::_byteswap_ulong( ptlTags->tags[I].offset ), ::_byteswap_ulong( ptlTags->tags[I].size ) );
				_sSize = ::_byteswap_ulong( ptlTags->tags[I].size );
				return ::_byteswap_ulong( ptlTags->tags[I].offset );
			}
		}
		return 0;
#undef SL2_CHECK
#undef SL2_R32
	}

}	// namespace sl2

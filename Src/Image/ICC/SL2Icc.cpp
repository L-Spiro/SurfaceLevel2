/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Loading and working with ICC profiles.
 */

#include "SL2Icc.h"

#include <cmath>


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

	/**
	 * A type-3 "para" handler.
	 * 
	 * \param _dIn The value to convert.
	 * \param _pvParm Associated structure data (SL2_PARA).
	 * \return Returns the linear value of the _dIn.
	 **/
	double CIcc::Type3_Para_To_Linear( double _dIn, const void * _pvParm ) {
		const SL2_PARA * ppPara = reinterpret_cast<const SL2_PARA *>(_pvParm);
		double dG = ppPara->dParms[0];
		double dA = ppPara->dParms[1];
		double dB = ppPara->dParms[2];
		double dC = ppPara->dParms[3];
		double dD = ppPara->dParms[4];
		return _dIn <= dD ?
			_dIn * dC :
			std::pow( dA * _dIn + dB, dG );

		/*
			return _dVal <= 0.08124285829863515939752716121802222914993762969970703125 ?
				_dVal / 4.5 :
				std::pow( (_dVal + 0.09929682680944297568093048766968422569334506988525390625) / 1.09929682680944296180314267985522747039794921875, 1.0 / 0.45 );
		*/
	}

	/**
	 * A type-3 "para" handler.
	 * 
	 * \param _dIn The value to convert.
	 * \param _pvParm Associated structure data (SL2_PARA).
	 * \return Returns the adjusted value of the _dIn.
	 **/
	double CIcc::Type3_Para_To_ColorSpace( double _dIn, const void * _pvParm ) {
		const SL2_PARA * ppPara = reinterpret_cast<const SL2_PARA *>(_pvParm);
		double dG = ppPara->dParms[0];
		double dA = ppPara->dParms[1];
		double dB = ppPara->dParms[2];
		double dC = ppPara->dParms[3];
		double dD = ppPara->dParms[4];
		return _dIn <= dD / (1.0 / dC) ?
			_dIn / dC :
			(1.0 / dA) * std::pow( _dIn, 1.0 / dG ) - (dB / dA);
		/*
			return _dVal <= 0.018 ?
				_dVal * 4.5 :
				1.099 * std::pow( _dVal, 0.45 ) - 0.099;
		*/
	}

}	// namespace sl2

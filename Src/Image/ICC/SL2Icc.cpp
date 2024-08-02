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
	 * Fills out a SL2_TRANSFER_FUNC structure given a curv/para tag.
	 * 
	 * \param _tfFunc The structure to fill out.
	 * \param _pui8Data The parametrc/curve data.
	 * \param _sSize Size of the data.
	 * \return Returns true if the curve was recognized and the structure could be filled out.
	 **/
	bool CIcc::FillOutTransferFunc( SL2_TRANSFER_FUNC &_tfFunc, const uint8_t * _pui8Data, size_t _sSize ) {
		if ( _sSize >= sizeof( icTagBase ) ) {
			const icTagBase * ptbTag = reinterpret_cast<const icTagBase *>(_pui8Data);
			_sSize -= sizeof( icTagBase );
			if ( ::_byteswap_ulong( ptbTag->sig ) == icSigCurveType ) {
				const icCurve * pcCurve = reinterpret_cast<const icCurve *>(_pui8Data + sizeof( icTagBase ));
				auto aCnt = ::_byteswap_ulong( pcCurve->count );
				switch ( aCnt ) {
					case 0 : {
						_tfFunc.pfXtoLinear = PassThrough;
						_tfFunc.pfLinearToX = PassThrough;
						return true;
					}
					default : {
						_sSize -= sizeof( pcCurve->count );
						size_t sElemSize = _sSize / aCnt;
						try {
							_tfFunc.cCurv.vTable.resize( aCnt );
							_tfFunc.pvParm = &_tfFunc.cCurv;
						}
						catch ( ... ) { return false; }

						switch ( sElemSize ) {
							case sizeof( uint8_t ) : {
								const uint8_t * pui8Data = reinterpret_cast<const uint8_t *>(pcCurve->data);
								for ( auto I = aCnt; I--; ) {
									_tfFunc.cCurv.vTable[I] = pui8Data[I] / double( 0xFF );
								}
								break;
							}
							case sizeof( uint16_t ) : {
								// Byte-swap in-place.
								const uint16_t * pui16Data = reinterpret_cast<const uint16_t *>(pcCurve->data);
								for ( auto I = aCnt; I--; ) {
									_tfFunc.cCurv.vTable[I] = ::_byteswap_ushort( pui16Data[I] ) / double( 0xFFFF );
								}
								break;
							}
							case sizeof( uint32_t ) : {
								// Byte-swap in-place.
								const uint32_t * pui32Data = reinterpret_cast<const uint32_t *>(pcCurve->data);
								for ( auto I = aCnt; I--; ) {
									_tfFunc.cCurv.vTable[I] = ::_byteswap_ulong( pui32Data[I] ) / double( 0xFFFFFFFF );
								}
								break;
							}
							default : { return false; }
						}
						if ( aCnt == 1 ) {
							_tfFunc.pfXtoLinear = Len1_Curve_To_Linear;
							_tfFunc.pfLinearToX = Len1_Linear_To_Curve;
						}
						else {
							char szBuffer[256];
							::sprintf_s( szBuffer, "XtoLinear: LUT[%u] = {\r\n\t", uint32_t( _tfFunc.cCurv.vTable.size() ) );
							::OutputDebugStringA( szBuffer );
							for ( size_t I = 0; I < _tfFunc.cCurv.vTable.size(); ++I ) {
								::sprintf_s( szBuffer, "%24.19g, ", _tfFunc.cCurv.vTable[I] );
								::OutputDebugStringA( szBuffer );
								if ( I % 16 == 15 ) {
									::OutputDebugStringA( "\r\n\t" );
								}
							}
							::OutputDebugStringA( "\r\n};\r\n" );

							::sprintf_s( szBuffer, "LinearToX: LUT[%u] = {\r\n\t", uint32_t( _tfFunc.cCurv.vTable.size() ) );
							::OutputDebugStringA( szBuffer );
							for ( size_t I = 0; I < _tfFunc.cCurv.vTable.size(); ++I ) {
								double dVal = double( I ) / (_tfFunc.cCurv.vTable.size() - 1.0);
								::sprintf_s( szBuffer, "%24.19g %24.19g %24.19g, ", 1.0 - dVal, (dVal - _tfFunc.cCurv.vTable[I]), (dVal - _tfFunc.cCurv.vTable[I]) + dVal );
								::OutputDebugStringA( szBuffer );
								if ( I % 16 == 15 ) {
									::OutputDebugStringA( "\r\n\t" );
								}
							}
							::OutputDebugStringA( "\r\n};\r\n" );
						}

						return true;
					}
				}
			}
			else if ( ::_byteswap_ulong( ptbTag->sig ) == icSigParaType ) {
				const icPara * ppPara = reinterpret_cast<const icPara *>(_pui8Data + sizeof( icTagBase ));
				_tfFunc.pvParm = &_tfFunc.pPara;
				switch ( ::_byteswap_ushort( ppPara->funcType ) ) {
					case 0 : {
						_tfFunc.pPara.dParms[0] = static_cast<int32_t>(::_byteswap_ulong( ppPara->parameters[0] )) / 65536.0;
						char szBuffer[256];
						::sprintf_s( szBuffer, "XtoLinear: std::pow( X, %.19g );\r\n", _tfFunc.pPara.dParms[0] );
						::OutputDebugStringA( szBuffer );
						::sprintf_s( szBuffer, "LinearToX: std::pow( X, 1.0 / %.19g );\r\n", _tfFunc.pPara.dParms[0] );
						::OutputDebugStringA( szBuffer );
						if ( _tfFunc.pPara.dParms[0] == 1.0 || _tfFunc.pPara.dParms[0] == 0.0 ) {
							_tfFunc.pfXtoLinear = PassThrough;
							_tfFunc.pfLinearToX = PassThrough;
							return true;
						}
						else if ( _tfFunc.pPara.dParms[0] < 0.0 ) {
							_tfFunc.pPara.dParms[0] = -_tfFunc.pPara.dParms[0];
						}
						_tfFunc.pfXtoLinear = Type0_Para_To_Linear;
						_tfFunc.pfLinearToX = Type0_Para_To_ColorSpace;
						return true;
					}
					case 1 : {
						::OutputDebugStringA( "\tICC TYPE 1:\r\n" );
						return true;
					}
					case 2 : {
						::OutputDebugStringA( "\tICC TYPE 2:\r\n" );
						return true;
					}
					case 3 : {
						_tfFunc.pPara.dParms[0] = static_cast<int32_t>(::_byteswap_ulong( ppPara->parameters[0] )) / 65536.0;
						_tfFunc.pPara.dParms[1] = static_cast<int32_t>(::_byteswap_ulong( ppPara->parameters[1] )) / 65536.0;
						_tfFunc.pPara.dParms[2] = static_cast<int32_t>(::_byteswap_ulong( ppPara->parameters[2] )) / 65536.0;
						_tfFunc.pPara.dParms[3] = static_cast<int32_t>(::_byteswap_ulong( ppPara->parameters[3] )) / 65536.0;
						_tfFunc.pPara.dParms[4] = static_cast<int32_t>(::_byteswap_ulong( ppPara->parameters[4] )) / 65536.0;

						char szBuffer[256];
						::sprintf_s( szBuffer, "XtoLinear: X <= %.19g ?\r\n"
							"\tX / %.19g :\r\n"
							"\tstd::pow( (X + %.19g) / %.19g, %.19g );\r\n",
							_tfFunc.pPara.dParms[4],
							1.0 / _tfFunc.pPara.dParms[3],
							_tfFunc.pPara.dParms[2] / _tfFunc.pPara.dParms[1], 1.0 / _tfFunc.pPara.dParms[1], _tfFunc.pPara.dParms[0] );

						::OutputDebugStringA( szBuffer );
						::sprintf_s( szBuffer, "LinearToX: X <= %.19g ?\r\n"
							"\tX * %.19g :\r\n"
							"\t%.19g * std::pow( X, 1.0 / %.19g ) - %.19g;\r\n",
							_tfFunc.pPara.dParms[4] / (1.0 / _tfFunc.pPara.dParms[3]),
							1.0 / _tfFunc.pPara.dParms[3],
							1.0 / _tfFunc.pPara.dParms[1], _tfFunc.pPara.dParms[0], _tfFunc.pPara.dParms[2] / _tfFunc.pPara.dParms[1] );
						::OutputDebugStringA( szBuffer );

						_tfFunc.pfXtoLinear = Type3_Para_To_Linear;
						_tfFunc.pfLinearToX = Type3_Para_To_ColorSpace;
						return true;
					}
					case 4 : {
						return true;
					}
				}
			}
		}
		return false;
	}

	/**
	 * A type-0 "para" handler.
	 * 
	 * \param _dIn The value to convert.
	 * \param _pvParm Associated structure data (SL2_PARA).
	 * \return Returns the linear value of the _dIn.
	 **/
	double CIcc::Type0_Para_To_Linear( double _dIn, const void * _pvParm ) {
		const SL2_PARA * ppPara = reinterpret_cast<const SL2_PARA *>(_pvParm);
		return std::pow( _dIn, ppPara->dParms[0] );
	}

	/**
	 * A type-0 "para" handler.
	 * 
	 * \param _dIn The value to convert.
	 * \param _pvParm Associated structure data (SL2_PARA).
	 * \return Returns the adjusted value of the _dIn.
	 **/
	double CIcc::Type0_Para_To_ColorSpace( double _dIn, const void * _pvParm ) {
		const SL2_PARA * ppPara = reinterpret_cast<const SL2_PARA *>(_pvParm);
		return std::pow( _dIn, 1.0 / ppPara->dParms[0] );
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
			(std::pow( _dIn, 1.0 / dG ) - dB) / dA;
	}

}	// namespace sl2

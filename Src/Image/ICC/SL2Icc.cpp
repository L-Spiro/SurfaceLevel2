/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Loading and working with ICC profiles.
 */

#include "SL2Icc.h"
#include "../../Utilities//SL2Utilities.h"

#include <algorithm>


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
						::OutputDebugStringA( "XtoLinear: X = X;\r\n" );
						::OutputDebugStringA( "LinearToX: X = X;\r\n" );
						return true;
					}
					default : {
						_sSize -= sizeof( pcCurve->count );
						size_t sElemSize = _sSize / aCnt;
						try {
							_tfFunc.cCurv.vTable.resize( aCnt );
							_tfFunc.cCurv.vInvTable.resize( aCnt );
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
								if ( aCnt == 1 ) {
									const uint16_t * pui16Data = reinterpret_cast<const uint16_t *>(pcCurve->data);
									_tfFunc.cCurv.vTable[0] = ::_byteswap_ushort( pui16Data[0] ) / 256.0;
									_tfFunc.cCurv.vInvTable[0] = 1.0 / _tfFunc.cCurv.vTable[0];
								}
								else {
									// Byte-swap in-place.
									const uint16_t * pui16Data = reinterpret_cast<const uint16_t *>(pcCurve->data);
									for ( auto I = aCnt; I--; ) {
										_tfFunc.cCurv.vTable[I] = ::_byteswap_ushort( pui16Data[I] ) / double( 0xFFFF );
									}
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

							char szBuffer[256];
							::sprintf_s( szBuffer, "XtoLinear: std::pow( X, %.19g );\r\n", _tfFunc.cCurv.vTable[0] );
							::OutputDebugStringA( szBuffer );
							::sprintf_s( szBuffer, "LinearToX: std::pow( X, 1.0 / %.19g );\r\n", _tfFunc.cCurv.vTable[0] );
							::OutputDebugStringA( szBuffer );
						}
						else {
							_tfFunc.pfXtoLinear = LenX_Curve_To_Linear;
							_tfFunc.pfLinearToX = LenX_Linear_To_Curve;

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
								_tfFunc.cCurv.vInvTable[I] = InverseLut( &_tfFunc.cCurv, dVal );
								::sprintf_s( szBuffer, "%24.19g, ", _tfFunc.cCurv.vInvTable[I] );
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
						return false;
					}
					case 2 : {
						::OutputDebugStringA( "\tICC TYPE 2:\r\n" );
						return false;
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
						::OutputDebugStringA( "\tICC TYPE 4:\r\n" );
						return false;
					}
				}
			}
		}
		return false;
	}

	/**
	 * An X-length "curve" handler.
	 * 
	 * \param _dIn The value to convert.
	 * \param _pvParm Associated structure data (SL2_CURV).
	 * \return Returns the linear value of the _dIn.
	 **/
	double CIcc::LenX_Curve_To_Linear( double _dIn, const void * _pvParm ) {
		const SL2_CURV * pcCurv = reinterpret_cast<const SL2_CURV *>(_pvParm);

		double dIdx = _dIn * (pcCurv->vTable.size() - 1.0);
		if ( pcCurv->vTable.size() == 0 ) { return 0.0; }
		if ( pcCurv->vTable.size() == 1 ) { return _dIn * pcCurv->vTable[0]; }

		if ( dIdx <= 0.0 ) { return pcCurv->vTable[0]; }
		size_t sEnd = pcCurv->vTable.size() - 1;
		
		size_t sIdx = static_cast<size_t>(dIdx);
		if ( sIdx >= sEnd ) { return pcCurv->vTable[sEnd]; }

		
		double dFrac = dIdx - static_cast<double>(sIdx);
		if ( pcCurv->vTable.size() >= 6 ) {
			size_t sClampedIdx = std::max( sIdx, size_t( 2 ) );
			sClampedIdx = std::min( sClampedIdx, size_t( sEnd - 3 ) );
			dFrac = dIdx - static_cast<double>(sClampedIdx);
			return CUtilities::Sample_6Point_5thOrder_Hermite_X( &pcCurv->vTable[sClampedIdx-2], dFrac );
		}
		if ( pcCurv->vTable.size() >= 4 ) {
			size_t sClampedIdx = std::max( sIdx, size_t( 1 ) );
			sClampedIdx = std::min( sClampedIdx, size_t( sEnd - 2 ) );
			dFrac = dIdx - static_cast<double>(sClampedIdx);
			return CUtilities::Sample_4Point_3rdhOrder_Hermite_X( &pcCurv->vTable[sClampedIdx-1], dFrac );
		}
		return ((pcCurv->vTable[sIdx+1] - pcCurv->vTable[sIdx]) * dFrac) + pcCurv->vTable[sIdx];
	}

	/**
	 * An X-length "curve" handler.
	 * 
	 * \param _dIn The value to convert.
	 * \param _pvParm Associated structure data (SL2_CURV).
	 * \return Returns the colorspace value of the _dIn.
	 **/
	double CIcc::LenX_Linear_To_Curve( double _dIn, const void * _pvParm ) {
		const SL2_CURV * pcCurv = reinterpret_cast<const SL2_CURV *>(_pvParm);

		double dIdx = _dIn * (pcCurv->vInvTable.size() - 1.0);
		if ( pcCurv->vInvTable.size() == 0 ) { return 0.0; }
		if ( pcCurv->vInvTable.size() == 1 ) { return _dIn * pcCurv->vInvTable[0]; }

		if ( dIdx <= 0.0 ) { return pcCurv->vInvTable[0]; }
		size_t sEnd = pcCurv->vInvTable.size() - 1;
		
		size_t sIdx = static_cast<size_t>(dIdx);
		if ( sIdx >= sEnd ) { return pcCurv->vInvTable[sEnd]; }

		
		double dFrac = dIdx - static_cast<double>(sIdx);
		if ( pcCurv->vInvTable.size() >= 6 ) {
			size_t sClampedIdx = std::max( sIdx, size_t( 2 ) );
			sClampedIdx = std::min( sClampedIdx, size_t( sEnd - 3 ) );
			dFrac = dIdx - static_cast<double>(sClampedIdx);
			return CUtilities::Sample_6Point_5thOrder_Hermite_X( &pcCurv->vInvTable[sClampedIdx-2], dFrac );
		}
		if ( pcCurv->vInvTable.size() >= 4 ) {
			size_t sClampedIdx = std::max( sIdx, size_t( 1 ) );
			sClampedIdx = std::min( sClampedIdx, size_t( sEnd - 2 ) );
			dFrac = dIdx - static_cast<double>(sClampedIdx);
			return CUtilities::Sample_4Point_3rdhOrder_Hermite_X( &pcCurv->vInvTable[sClampedIdx-1], dFrac );
		}
		return ((pcCurv->vInvTable[sIdx+1] - pcCurv->vInvTable[sIdx]) * dFrac) + pcCurv->vInvTable[sIdx];
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

	/**
	 * Finds the inverse of a LUT index.
	 *
	 * \param _pcCurv The curve of points.
	 * \param _dPoint The point to reverse look-up.
	 * \return Returns the inverse look-up value for a given point in a table.
	 **/
	double CIcc::InverseLut( const SL2_CURV * _pcCurv, double _dPoint ) {
		size_t sTotal = _pcCurv->vTable.size();
		if ( !sTotal ) { return _dPoint; }

		const double * pdPoints = _pcCurv->vTable.data();
		if ( sTotal == 1 ) { return 1.0 / pdPoints[0]; }

		if ( _dPoint <= pdPoints[0] ) { return 0.0; }
		if ( _dPoint >= pdPoints[sTotal-1] ) { return 1.0; }

		double dTarget = _dPoint;

		double dJump = 0.5;
		double dOffset = dJump;
		double dThis = LenX_Curve_To_Linear( dOffset, _pcCurv );
		while ( dThis != dTarget ) {
			if ( dThis < dTarget ) {
				double dPrevOffset = dOffset;
				dOffset += dJump;
				if ( dPrevOffset == dOffset ) { break; }
				dThis = LenX_Curve_To_Linear( dOffset, _pcCurv );
			}
			else {
				double dPrevOffset = dOffset;
				dOffset -= dJump;
				dJump /= 2.0;
				dOffset += dJump;
				if ( dPrevOffset == dOffset ) { break; }
				dThis = LenX_Curve_To_Linear( dOffset, _pcCurv );
			}
		}
		return dOffset;


		/*double dLinVal = _dPoint;
		auto aClosestIt = std::min_element( _pcCurv->vTable.begin(), _pcCurv->vTable.end(), 
			[dLinVal]( double _dA, double _dB ) {
				return std::abs( _dA - dLinVal ) < std::abs( _dB - dLinVal );
			});
		return std::distance( _pcCurv->vTable.begin(), aClosestIt ) / (sTotal / 1.0);*/
	}

	/**
	 * Creates a colorspace profile using a predefined colorspace.
	 *
	 * \param _cContextID The context or nullptr.
	 * \param _cgcCurve The type of profile to create.
	 * \param _cpProfile The returned profile.
	 * \param _bIncludeCurves If true, tone curves are included.
	 * \return Returns true if all allocations succeed.
	 **/
	bool CIcc::CreateProfile( cmsContext _cContextID, SL2_COLORSPACE_GAMMA_CURVES _cgcCurve, SL2_CMS_PROFILE &_cpProfile, bool _bIncludeCurves ) {
		const CFormat::SL2_TRANSFER_FUNCS & tfFunc = CFormat::TransferFunc( _cgcCurve );
		cmsCIExyY		cieD65 = { tfFunc.dWhite[0], tfFunc.dWhite[1], 1.0 };
		cmsCIExyYTRIPLE	ciePrimaries = {
							{ tfFunc.dChromaR[0], tfFunc.dChromaR[1], 1.0 },
							{ tfFunc.dChromaG[0], tfFunc.dChromaG[1], 1.0 },
							{ tfFunc.dChromaB[0], tfFunc.dChromaB[1], 1.0 },
		};
		

		if ( _bIncludeCurves ) {
			if ( tfFunc.i32CurveType == 0 ) {
				std::vector<cmsFloat32Number> vValues;
				try {
					vValues.resize( 4096 );
				}
				catch ( ... ) { return false; }

				for ( size_t I = 0; I < vValues.size(); ++I ) {
					vValues[I] = static_cast<cmsFloat32Number>(tfFunc.pfXtoLinear( static_cast<double>(I) / (vValues.size() - 1.0) ));
				}
				cmsToneCurve *	ptcGamma[3];
				SL2_CMS_TONECURVE tcCurve;
				ptcGamma[0] = ptcGamma[1] = ptcGamma[2] = tcCurve.Set( ::cmsBuildTabulatedToneCurveFloat( _cContextID, static_cast<cmsUInt32Number>(vValues.size()), vValues.data() ) ).tcCurve;
				if ( ptcGamma[0] == NULL ) { return false; }

				if ( !_cpProfile.Set( ::cmsCreateRGBProfileTHR( _cContextID, &cieD65, &ciePrimaries, ptcGamma ), true ).hProfile ) { return false; }

			}
			else {
				cmsToneCurve *	ptcGamma[3];
				SL2_CMS_TONECURVE tcCurve;
				ptcGamma[0] = ptcGamma[1] = ptcGamma[2] = tcCurve.Set( ::cmsBuildParametricToneCurve( _cContextID, tfFunc.i32CurveType, tfFunc.dParaCurve ) ).tcCurve;
				if ( ptcGamma[0] == NULL ) { return false; }

				if ( !_cpProfile.Set( ::cmsCreateRGBProfileTHR( _cContextID, &cieD65, &ciePrimaries, ptcGamma ), true ).hProfile ) { return false; }
			}
		}
		else {
			cmsFloat64Number f64nParm[1] = { 1.0 };
			cmsToneCurve *	ptcGamma[3];
			SL2_CMS_TONECURVE tcCurve;
			ptcGamma[0] = ptcGamma[1] = ptcGamma[2] = tcCurve.Set( ::cmsBuildParametricToneCurve( _cContextID, 1, f64nParm ) ).tcCurve;
			if ( ptcGamma[0] == NULL ) { return false; }

			if ( !_cpProfile.Set( ::cmsCreateRGBProfileTHR( _cContextID, &cieD65, &ciePrimaries, ptcGamma ), true ).hProfile ) { return false; }
		}
		if ( !SetTextTags( _cpProfile.hProfile, tfFunc.pwcDesc )) { return false; }

		return true;
	}

	/**
	 * Creates an in-memory ICC file.
	 * 
	 * \param _pProfile The profile to save.
	 * \param _vFile The in-memory file after saving.
	 * \return Returns true if the file was saved to memory.
	 **/
	bool CIcc::SaveProfileToMemory( const SL2_CMS_PROFILE &_pProfile, std::vector<uint8_t> &_vFile ) {
		cmsUInt32Number ui32Size = 0;
		if ( !::cmsSaveProfileToMem( _pProfile.hProfile, nullptr, &ui32Size ) ) { return false; }
		try {
			_vFile.resize( ui32Size );
		}
		catch ( ... ) { return false; }
		return ::cmsSaveProfileToMem( _pProfile.hProfile, _vFile.data(), &ui32Size );
	}

	/**
	 * Creates a linear version of the given in-memory ICC profile.
	 * 
	 * \param _vFile The in-memory ICC file.
	 * \param _pProfile The created profile.
	 * \return Returns true if the ICC file was loaded and set to linear.
	 **/
	bool CIcc::CreateLinearProfile( std::vector<uint8_t> &_vFile, SL2_CMS_PROFILE &_pProfile ) {
		if ( _vFile.size() != static_cast<size_t>(static_cast<cmsUInt32Number>(_vFile.size())) || static_cast<cmsUInt32Number>(_vFile.size()) <= 0 ) { return false; }
		if ( _pProfile.Set( ::cmsOpenProfileFromMem( _vFile.data(), static_cast<cmsUInt32Number>(_vFile.size()) ), true ).hProfile == NULL ) { return false; }
		CIcc::SL2_CMS_TONECURVE tcGamma( ::cmsBuildGamma( NULL, 1.0 ) );
		if ( !tcGamma.tcCurve ) { return false; }
		cmsToneCurve * ptcCurves[4] = { tcGamma.tcCurve, tcGamma.tcCurve, tcGamma.tcCurve, tcGamma.tcCurve };
		if ( !::cmsWriteTag( _pProfile.hProfile, cmsSigRedTRCTag, ptcCurves[0] ) ||
			!::cmsWriteTag( _pProfile.hProfile, cmsSigGreenTRCTag, ptcCurves[1] ) ||
			!::cmsWriteTag( _pProfile.hProfile, cmsSigBlueTRCTag, ptcCurves[2] ) ||
			!::cmsWriteTag( _pProfile.hProfile, cmsSigGrayTRCTag, ptcCurves[3] ) ) { return false; }
		return true;
	}

	cmsBool CIcc::SetTextTags( cmsHPROFILE hProfile, const wchar_t * Description ) {
		cmsMLU *DescriptionMLU, *CopyrightMLU, *ManufacturerMLU;
		cmsBool  rc = FALSE;
		cmsContext ContextID = cmsGetProfileContextID(hProfile);

		DescriptionMLU  = cmsMLUalloc(ContextID, 1);
		ManufacturerMLU = cmsMLUalloc(ContextID, 1);
		CopyrightMLU    = cmsMLUalloc(ContextID, 1);

		if (DescriptionMLU == NULL || CopyrightMLU == NULL) goto Error;

		if (!cmsMLUsetWide(DescriptionMLU,  "en", "US", Description)) goto Error;
		if (!cmsMLUsetWide(ManufacturerMLU, "en", "US", L"L. Spiro SurfaceLevel 2.0")) goto Error;
		if (!cmsMLUsetWide(CopyrightMLU,    "en", "US", L"Public Domain")) goto Error;

		if (!cmsWriteTag(hProfile, cmsSigProfileDescriptionTag,  DescriptionMLU)) goto Error;
		if (!cmsWriteTag(hProfile, cmsSigDeviceMfgDescTag,       ManufacturerMLU)) goto Error;
		if (!cmsWriteTag(hProfile, cmsSigCopyrightTag,           CopyrightMLU)) goto Error;

		rc = TRUE;

	Error:

		if (DescriptionMLU)
			cmsMLUfree(DescriptionMLU);
		if (ManufacturerMLU)
			cmsMLUfree(ManufacturerMLU);
		if (CopyrightMLU)
			cmsMLUfree(CopyrightMLU);
		return rc;
	}

}	// namespace sl2

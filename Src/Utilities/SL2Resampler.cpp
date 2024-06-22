/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: All of the image formats, along with conversion routines and functions for working with the formats (encoding, decoding, etc.)
 */

#include "SL2Resampler.h"
#include "../OS/SL2Os.h"
#include "../Utilities/SL2Utilities.h"

#include <immintrin.h>


namespace sl2 {

	// == Members.
	/** Filter parameters. */
	CResampler::SL2_FILTER CResampler::m_fFilter[] = {
		{ CResampler::PointFilterFunc,					0.5 },
		{ CResampler::BilinearFilterFunc,				1.0 },
		{ CResampler::QuadraticSharpFilterFunc,			1.0 },
		{ CResampler::QuadraticInterpolFilterFunc,		1.5 },
		{ CResampler::QuadraticApproxFilterFunc,		1.5 },
		{ CResampler::QuadraticMixFilterFunc,			1.5 },
		{ CResampler::KaiserFilterFunc,					3.0 },
		{ CResampler::LanczosXFilterFunc<2>,			2.0 },
		{ CResampler::LanczosXFilterFunc<3>,			3.0 },
		{ CResampler::LanczosXFilterFunc<4>,			4.0 },
		{ CResampler::LanczosXFilterFunc<6>,			6.0 },
		{ CResampler::LanczosXFilterFunc<8>,			8.0 },
		{ CResampler::LanczosXFilterFunc<12>,			12.0 },
		{ CResampler::LanczosXFilterFunc<64>,			64.0 },
		{ CResampler::MitchellFilterFunc,				2.0 },
		{ CResampler::CatmullRomFilterFunc,				2.0 },
		{ CResampler::BSplineFilterFunc,				2.0 },
		{ CResampler::BlackmanFilterFunc,				3.0 },
		{ CResampler::GaussianSharpFilterFunc,			1.25 },
		{ CResampler::GaussianFilterFunc,				1.25 },
		{ CResampler::BellFilterFunc,					1.5 },
	};

	CResampler::CResampler() {
	}
	CResampler::~CResampler() {
	}

	// == Functions.
	/**
	 * Resamples an image in-place.
	 * 
	 * \param _pdIn The input buffer.
	 * \param _vOut The output buffer.
	 * \param _pParms Image/resampling parameters.
	 * \return Returns true if all allocations succeed.
	 **/
	bool CResampler::Resample( const double * _pdIn, double * _pdOut, const SL2_RESAMPLE &_pParms ) {
		std::vector<double> dBufferR;
		std::vector<double> dBufferG;
		std::vector<double> dBufferB;
		std::vector<double> dBufferA;
		std::vector<double> dBufferR2;
		std::vector<double> dBufferG2;
		std::vector<double> dBufferB2;
		std::vector<double> dBufferA2;

		uint32_t ui32NewW = std::max( 1U, _pParms.ui32NewW );
		uint32_t ui32NewH = std::max( 1U, _pParms.ui32NewH );
		uint32_t ui32NewD = std::max( 1U, _pParms.ui32NewD );
		uint32_t ui32W = std::max( 1U, _pParms.ui32W );
		uint32_t ui32H = std::max( 1U, _pParms.ui32H );
		uint32_t ui32D = std::max( 1U, _pParms.ui32D );
		try {
			size_t sSize = ui32NewW * ui32H * ui32D;
			dBufferR.resize( sSize );
			dBufferG.resize( sSize );
			dBufferB.resize( sSize );
			if ( _pParms.bAlpha ) {
				dBufferA.resize( sSize );
			}
			/*sSize = ui32NewW * ui32NewH * ui32D;
			dBufferR2.resize( sSize );
			dBufferG2.resize( sSize );
			dBufferB2.resize( sSize );
			if ( _pParms.bAlpha ) {
				dBufferA2.resize( sSize );
			}*/
		}
		catch ( ... ) { return true; }

		// Resize width first for best caching.
		if ( !CreateContribList( ui32W, ui32NewW, _pParms.taAlphaW, _pParms.pfFilterW, _pParms.dFilterSupportW, _pParms.fFilterScale ) ) { return false; }
		double * pdDst[4] = { dBufferR.data(), dBufferG.data(), dBufferB.data(), dBufferA.data() };
		// Resize W.
		size_t sPagesSize = ui32W * ui32H * 4;
		size_t sNewPagesSize = ui32NewW * ui32H;

		uint32_t ui32DstW = ui32H;
		uint32_t ui32DstH = ui32NewW;
		for ( size_t I = 0; I < (_pParms.bAlpha ? 4 : 3); ++I ) {
			for ( size_t D = 0; D < ui32D; ++D ) {
				for ( size_t H = 0; H < ui32H; ++H ) {
					const double * pdRowStart = _pdIn + I + ((sPagesSize * D) + (H * ui32W * 4));
					for ( size_t W = 0; W < ui32NewW; ++W ) {
						for ( size_t J = 0; J < m_cContribs[W].i32Indices.size(); ++J ) {
							int32_t i32Index = m_cContribs[W].i32Indices[J];
							if ( i32Index == -1 ) {
								m_dBuffer[J] = _pParms.dBorderColor[I];
							}
							else {
								m_dBuffer[J] = pdRowStart[i32Index*4];
							}
						}
						double dConvolved = ConvolveAligned( m_cContribs[W].dContributions.data(), m_dBuffer.data(), m_cContribs[W].dContributions.size() );
						size_t sDstIdx = (sNewPagesSize * D) + (W * ui32H) + H;
						pdDst[I][sDstIdx] = dConvolved;
						//pdDst[I][sDstIdx] = H / double( ui32H );
					}
				}
			}
		}

		// Resize H, now aligned horizontally in the buffers.
		if ( !CreateContribList( ui32H, ui32NewH, _pParms.taAlphaH, _pParms.pfFilterH, _pParms.dFilterSupportH, _pParms.fFilterScale ) ) { return false; }
		sPagesSize = sNewPagesSize;
		sNewPagesSize = ui32NewW * ui32NewH;
		uint32_t tW = ui32H;
		uint32_t tH = ui32NewW;
		//double * pdDst2[4] = { dBufferR2.data(), dBufferG2.data(), dBufferB2.data(), dBufferA2.data() };
		for ( size_t I = 0; I < (_pParms.bAlpha ? 4 : 3); ++I ) {
			for ( size_t D = 0; D < ui32D; ++D ) {
				for ( size_t H = 0; H < tH; ++H ) {
					const double * pdRowStart = pdDst[I] + (sPagesSize * D) + (H * tW);
					for ( size_t W = 0; W < ui32NewH; ++W ) {
						double dConvolved;
						if ( m_cContribs[W].bInsideBounds ) {
							dConvolved = ConvolveUnaligned( m_cContribs[W].dContributions.data(), pdRowStart + m_cContribs[W].i32Indices[0], m_cContribs[W].dContributions.size() );
						}
						else {
							for ( size_t J = 0; J < m_cContribs[W].i32Indices.size(); ++J ) {
								int32_t i32Index = m_cContribs[W].i32Indices[J];
								if ( i32Index == -1 ) {
									m_dBuffer[J] = _pParms.dBorderColor[I];
								}
								else {
									m_dBuffer[J] = (*(pdRowStart + i32Index));
								}
							}
							dConvolved = ConvolveAligned( m_cContribs[W].dContributions.data(), m_dBuffer.data(), m_cContribs[W].dContributions.size() );
						}
						if ( ui32D > 1 || ui32NewD > 1 ) {
						}
						else {
							//size_t sDstIdx = ((sNewPagesSize * D) + (W * ui32NewH) + H) * 4 + I;
							size_t sDstIdx = ((sNewPagesSize * D) + (W * ui32NewW) + H) * 4 + I;
							_pdOut[sDstIdx] = dConvolved;
							//_pdOut[sDstIdx] = double( W ) / ui32NewW;
							//_pdOut[sDstIdx] = (*(pdRowStart + W));
						}
					}
				}
			}
		}

		if ( ui32D > 1 || ui32NewD > 1 ) {
		}
		else {
			// Add alpha to the output.
			if ( !_pParms.bAlpha ) {
				sPagesSize = sNewPagesSize;
				for ( size_t D = 0; D < ui32D; ++D ) {
					for ( size_t H = 0; H < ui32NewH; ++H ) {
						for ( size_t W = 0; W < ui32NewW; ++W ) {
							size_t sDstIdx = ((sPagesSize * D) + (H * ui32NewW) + W) * 4;
							_pdOut[sDstIdx+3] = 1.0;
						}
					}
				}
			}
		}
		return true;
	}

	/**
	 * Creates a new contribution list.
	 *
	 * \param _ui32SrcSize Size of the source.
	 * \param _ui32DstSize Size of the destination.
	 * \param _taAddressMode Texture addressing mode.
	 * \param _pfFilter Filter function.
	 * \param _dFilterSupport Filter support value.
	 * \param _fFilterScale Filter scale.
	 * \return Returns true if all allocations succeeded.
	 */
	bool CResampler::CreateContribList( uint32_t _ui32SrcSize, uint32_t _ui32DstSize,
		SL2_TEXTURE_ADDRESSING _taAddressMode,
		PfFilterFunc _pfFilter, double _dFilterSupport, float _fFilterScale ) {

		std::vector<SL2_CONTRIB_BOUNDS> vBounds;
		try {
			m_cContribs.clear();
			m_cContribs.resize( _ui32DstSize );
			vBounds.resize( _ui32DstSize );
		}
		catch ( ... ) { return false; }


		// For fast division.
		const double dFilterScale = 1.0 / _fFilterScale;
		constexpr double dNudge = 0.5;

		double dScale = static_cast<double>(_ui32DstSize) / static_cast<double>(_ui32SrcSize);
		double dOrigScale = dScale;

		dScale = std::min( dScale, 1.0 );

		double dHalfWidth = (_dFilterSupport / dScale) * dFilterScale;


		for ( uint32_t I = 0; I < _ui32DstSize; ++I ) {
			// Convert from discrete to continuous coordinates, then back.
			vBounds[I].dCenter = (static_cast<double>(I) + dNudge) / dOrigScale;
			vBounds[I].dCenter -= dNudge;
			vBounds[I].i32Left = static_cast<int32_t>(std::floor( vBounds[I].dCenter - dHalfWidth ));
			vBounds[I].i32Right = static_cast<int32_t>(std::ceil( vBounds[I].dCenter + dHalfWidth ));
		}

		size_t sMaxSize = 0;
		for ( uint32_t I = 0; I < _ui32DstSize; ++I ) {
			int32_t i32MaxK = -1;
			double dMaxW = -1.0e20;
			double dCenter = vBounds[I].dCenter;
			int32_t i32Left = vBounds[I].i32Left;
			int32_t i32Right = vBounds[I].i32Right;

			try {
				m_cContribs[I].dContributions.resize( i32Right - i32Left + 1 );
				m_cContribs[I].i32Indices.resize( i32Right - i32Left + 1 );
				sMaxSize = std::max( sMaxSize, size_t( i32Right - i32Left + 1 ) );
			}
			catch ( ... ) { return false; }

			double dTotalWeight = 0.0;
			for ( int32_t J = i32Left; J <= i32Right; ++J ) {
				//dTotalWeight += (*_pfFilter)( ((vBounds[I].dCenter - (J + dNudge))) * dScale * _fFilterScale );
				dTotalWeight += (*_pfFilter)( (vBounds[I].dCenter - J) * dScale * _fFilterScale );
			}

			const double dNorm = 1.0 / dTotalWeight;
			dTotalWeight = 0.0;

			for ( int32_t J = i32Left; J <= i32Right; ++J ) {
				//double dThisWeight = (*_pfFilter)( ((vBounds[I].dCenter - (J + dNudge))) * dScale * _fFilterScale ) * dNorm;
				double dThisWeight = (*_pfFilter)( (vBounds[I].dCenter - J) * dScale * _fFilterScale ) * dNorm;

				m_cContribs[I].i32Indices[J-i32Left] = CTextureAddressing::m_pfFuncs[_taAddressMode]( _ui32SrcSize, J );

				if ( dThisWeight == 0.0 ) { continue; }

				m_cContribs[I].dContributions[J-i32Left] = dThisWeight;

				dTotalWeight += dThisWeight;

				if ( dThisWeight > dMaxW ) {
					dMaxW = dThisWeight;
					i32MaxK = J - i32Left;
				}
			}
			

			if ( i32MaxK == -1 ) { return false; }

			if ( dTotalWeight != 1.0 && i32MaxK >= 0 ) {
				m_cContribs[I].dContributions[i32MaxK] += 1.0 - dTotalWeight;
			}

			// Trim 0 values.
			for ( size_t J = m_cContribs[I].dContributions.size(); J--; ) {
				if ( 0.0 == m_cContribs[I].dContributions[J] ) {
					m_cContribs[I].dContributions.erase( m_cContribs[I].dContributions.begin() + J );
					m_cContribs[I].i32Indices.erase( m_cContribs[I].i32Indices.begin() + J );
				}
			}

			// We can take a shortcut to gathering samples if all of the samples are in-range and sequential.
			m_cContribs[I].bInsideBounds = vBounds[I].i32Left >= 0 && (vBounds[I].i32Right) < int32_t( _ui32SrcSize );
			for ( int32_t J = 1; J < m_cContribs[I].dContributions.size(); ++J ) {
				if ( m_cContribs[I].i32Indices[J] != (m_cContribs[I].i32Indices[J-1] + 1) ) { m_cContribs[I].bInsideBounds = false; break; }
			}
		}
		if ( sMaxSize ) {
			try {
				m_dBuffer.resize( sMaxSize );
			}
			catch ( ... ) { return false; }
		}
		return true;
	}

	/**
	 * Convolvinate.
	 *
	 * \param _pdWeights The convolution weights.
	 * \param _pdTexels The texels to be convolved.
	 * \param _sTotal The total values to which pdTexels and _pdWeights point.
	 * \return Returns the summed weights * texels.
	 **/
	double CResampler::ConvolveAligned( const double * _pdWeights, const double * _pdTexels, size_t _sTotal ) {
		SL2_ALIGN( 64 )
		double dSum[4] = { 0.0 };
#ifdef __AVX512F__
		if ( CUtilities::IsAvx512FSupported() ) {
			__m512d mAcc = _mm512_setzero_pd();
			while ( _sTotal >= sizeof( __m512d ) / sizeof( double ) ) {
				__m512d mW = _mm512_load_pd( _pdWeights );
				__m512d dT = _mm512_load_pd( _pdTexels );
				mAcc = _mm512_fmadd_pd( mW, dT, mAcc );
				_pdWeights += sizeof( __m512d ) / sizeof( double );
				_pdTexels += sizeof( __m512d ) / sizeof( double );
				_sTotal -= sizeof( __m512d ) / sizeof( double );
			}
			__m256d mLow256 = _mm512_castpd512_pd256( mAcc );			// Low 256 bits.
			__m256d mHigh256 = _mm512_extractf64x4_pd( mAcc, 1 );		// High 256 bits.
			__m256d mSum = _mm256_add_pd( mLow256, mHigh256 );
			_mm256_store_pd( dSum, mSum );
			dSum[0] += dSum[1] + dSum[2] + dSum[3];
		}
#endif	// #ifdef __AVX512F__

#ifdef __AVX__
		if ( CUtilities::IsAvxSupported() ) {
			__m256d mAcc = _mm256_setzero_pd();
			while ( _sTotal >= sizeof( __m256d ) / sizeof( double ) ) {
				__m256d mW = _mm256_load_pd( _pdWeights );
				__m256d dT = _mm256_load_pd( _pdTexels );
				mAcc = _mm256_add_pd( _mm256_mul_pd( mW, dT ), mAcc );
				_pdWeights += sizeof( __m256d ) / sizeof( double );
				_pdTexels += sizeof( __m256d ) / sizeof( double );
				_sTotal -= sizeof( __m256d ) / sizeof( double );
			}
			dSum[0] += CUtilities::HorizontalSum( mAcc );
		}
#endif	// #ifdef __AVX__

#ifdef __SSE4_1__
		if ( CUtilities::IsSse4Supported() ) {
			__m128d mAcc = _mm_setzero_pd();
			while ( _sTotal >= sizeof( __m128d ) / sizeof( double ) ) {
				__m128d mW = _mm_load_pd( _pdWeights );
				__m128d dT = _mm_load_pd( _pdTexels );
				mAcc = _mm_add_pd( _mm_mul_pd( mW, dT ), mAcc );
				_pdWeights += sizeof( __m128d ) / sizeof( double );
				_pdTexels += sizeof( __m128d ) / sizeof( double );
				_sTotal -= sizeof( __m128d ) / sizeof( double );
			}
			dSum[0] += CUtilities::HorizontalSum( mAcc );
		}
#endif	// #ifdef __SSE4_1__

		while ( _sTotal >= 1 ) {
			dSum[0] += (*_pdWeights++) * (*_pdTexels++);
			--_sTotal;
		}
		return dSum[0];
	}

	/**
	 * Convolvinate using unaligned reads.
	 *
	 * \param _pdWeights The convolution weights.
	 * \param _pdTexels The texels to be convolved.
	 * \param _sTotal The total values to which pdTexels and _pdWeights point.
	 * \return Returns the summed weights * texels.
	 **/
	double CResampler::ConvolveUnaligned( const double * _pdWeights, const double * _pdTexels, size_t _sTotal ) {
		SL2_ALIGN( 64 )
		double dSum[4] = { 0.0 };
#ifdef __AVX512F__
		if ( CUtilities::IsAvx512FSupported() ) {
			__m512d mAcc = _mm512_setzero_pd();
			while ( _sTotal >= sizeof( __m512d ) / sizeof( double ) ) {
				__m512d mW = _mm512_load_pd( _pdWeights );
				__m512d dT = _mm512_loadu_pd( _pdTexels );
				mAcc = _mm512_fmadd_pd( mW, dT, mAcc );
				_pdWeights += sizeof( __m512d ) / sizeof( double );
				_pdTexels += sizeof( __m512d ) / sizeof( double );
				_sTotal -= sizeof( __m512d ) / sizeof( double );
			}
			__m256d mLow256 = _mm512_castpd512_pd256( mAcc );			// Low 256 bits.
			__m256d mHigh256 = _mm512_extractf64x4_pd( mAcc, 1 );		// High 256 bits.
			__m256d mSum = _mm256_add_pd( mLow256, mHigh256 );
			_mm256_store_pd( dSum, mSum );
			dSum[0] += dSum[1] + dSum[2] + dSum[3];
		}
#endif	// #ifdef __AVX512F__

#ifdef __AVX__
		if ( CUtilities::IsAvxSupported() ) {
			__m256d mAcc = _mm256_setzero_pd();
			while ( _sTotal >= sizeof( __m256d ) / sizeof( double ) ) {
				__m256d mW = _mm256_load_pd( _pdWeights );
				__m256d dT = _mm256_loadu_pd( _pdTexels );
				mAcc = _mm256_add_pd( _mm256_mul_pd( mW, dT ), mAcc );
				_pdWeights += sizeof( __m256d ) / sizeof( double );
				_pdTexels += sizeof( __m256d ) / sizeof( double );
				_sTotal -= sizeof( __m256d ) / sizeof( double );
			}
			dSum[0] += CUtilities::HorizontalSum( mAcc );
		}
#endif	// #ifdef __AVX__

#ifdef __SSE4_1__
		if ( CUtilities::IsSse4Supported() ) {
			__m128d mAcc = _mm_setzero_pd();
			while ( _sTotal >= sizeof( __m128d ) / sizeof( double ) ) {
				__m128d mW = _mm_load_pd( _pdWeights );
				__m128d dT = _mm_loadu_pd( _pdTexels );
				mAcc = _mm_add_pd( _mm_mul_pd( mW, dT ), mAcc );
				_pdWeights += sizeof( __m128d ) / sizeof( double );
				_pdTexels += sizeof( __m128d ) / sizeof( double );
				_sTotal -= sizeof( __m128d ) / sizeof( double );
			}
			dSum[0] += CUtilities::HorizontalSum( mAcc );
		}
#endif	// #ifdef __SSE4_1__

		while ( _sTotal >= 1 ) {
			dSum[0] += (*_pdWeights++) * (*_pdTexels++);
			--_sTotal;
		}
		return dSum[0];
	}

}	// namespace sl2

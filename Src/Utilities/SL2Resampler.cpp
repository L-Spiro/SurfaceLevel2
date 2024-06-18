/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: All of the image formats, along with conversion routines and functions for working with the formats (encoding, decoding, etc.)
 */

#include "SL2Resampler.h"
#include "../Utilities/SL2Utilities.h"

#include <immintrin.h>


namespace sl2 {

	// == Functions.
	/**
	 * Resamples an image in-place.
	 * 
	 * \param _vInOut The input buffer.
	 * \param _vOut The output buffer.
	 * \param _pParms Image/resampling parameters.
	 * \return Returns true if all allocations succeed.
	 **/
	bool CResampler::Resample( const std::vector<double> &_vIn, double * _pdOut, const SL2_RESAMPLE &_pParms ) {
		std::vector<double> dBufferR;
		std::vector<double> dBufferG;
		std::vector<double> dBufferB;
		std::vector<double> dBufferA;

		uint32_t ui32NewW = std::max( 1U, _pParms.ui32NewW );
		uint32_t ui32NewH = std::max( 1U, _pParms.ui32NewH );
		uint32_t ui32NewD = std::max( 1U, _pParms.ui32NewD );
		uint32_t ui32W = std::max( 1U, _pParms.ui32W );
		uint32_t ui32H = std::max( 1U, _pParms.ui32H );
		uint32_t ui32D = std::max( 1U, _pParms.ui32D );
		try {
			dBufferR.resize( ui32NewW * ui32NewH * ui32NewD );
			dBufferG.resize( ui32NewW * ui32NewH * ui32NewD );
			dBufferB.resize( ui32NewW * ui32NewH * ui32NewD );
			if ( _pParms.bAlpha ) {
				dBufferA.resize( ui32NewW * ui32NewH * ui32NewD );
			}
		}
		catch ( ... ) { return true; }

		// Resize width first for best caching.
		if ( !CreateContribList( ui32W, ui32NewW, _pParms.taAlphaW, _pParms.pfFilterW, _pParms.dFilterSupport, _pParms.fFilterScale ) ) { return false; }
		double * pdDst[4] = { dBufferR.data(), dBufferG.data(), dBufferB.data(), dBufferA.data() };
		for ( size_t I = 0; I < (_pParms.bAlpha ? 4 : 3); ++I ) {
			const double * pdRowStart = _vIn.data() + I;
			for ( size_t X = 0; X < ui32NewW; ++X ) {
				for ( size_t J = 0; J < m_cContribs[X].i32Indices.size(); ++J ) {
					int32_t i32Index = m_cContribs[X].i32Indices[J];
					if ( i32Index == -1 ) {
						m_dBuffer[J] = _pParms.dBorderColor[I];
					}
					else {
						m_dBuffer[J] = (*(pdRowStart + (i32Index * 4)));
					}
				}
			}
		}
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
		const double dNudge = 0.5;

		double dScale = static_cast<double>(_ui32DstSize) / static_cast<double>(_ui32SrcSize);
		double dOrigScale = dScale;

		dScale = std::min( dScale, 1.0 );

		double dHalfWidth = (_dFilterSupport / dScale) * dFilterScale;


		for ( uint32_t I = 0; I < _ui32DstSize; ++I ) {
			// Convert from discrete to continuous coordinates, then back.
			vBounds[I].dCenter = (static_cast<double>(I) + dNudge) / dOrigScale;
			//vBounds[I].dCenter -= dNudge;
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
				double dIdx = std::floor( vBounds[I].dCenter ) + 0.5;
				dTotalWeight += (*_pfFilter)( (J - dIdx) * dScale * _fFilterScale );
			}

			const double dNorm = 1.0 / dTotalWeight;
			dTotalWeight = 0.0;

			for ( int32_t J = i32Left; J <= i32Right; ++J ) {
				double dIdx = std::floor( vBounds[I].dCenter ) + 0.5;
				double dThisWeight = (*_pfFilter)( (J - dIdx) * dScale * _fFilterScale ) * dNorm;

				m_cContribs[I].i32Indices[J+i32Left] = CTextureAddressing::m_pfFuncs[_taAddressMode]( _ui32SrcSize, int32_t( std::floor( dIdx ) ) );
				if ( dThisWeight == 0.0 ) { continue; }

				m_cContribs[I].dContributions[J+i32Left] = dThisWeight;

				dTotalWeight += dThisWeight;

				if ( dThisWeight > dMaxW ) {
					dMaxW = dThisWeight;
					i32MaxK = J;
				}
			}

			if ( i32MaxK == -1 ) { return false; }

			if ( dTotalWeight != 1.0 && i32MaxK >= 0 ) {
				m_cContribs[I].dContributions[i32MaxK+i32Left] += 1.0 - dTotalWeight;
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
	double CResampler::Convolve( const double * _pdWeights, const double * _pdTexels, size_t _sTotal ) {
		SL2_ALIGN( 64 )
		double dSum[8] = { 0.0 };
#ifdef __AVX512F__
		if ( CUtilities::IsAvx512FSupported() ) {
			__m512d acc = _mm512_setzero_pd();
			while ( _sTotal <= sizeof( __m512d ) / sizeof( double ) ) {
				__m512d w = _mm512_loadu_pd( _pdWeights );
				__m512d t = _mm512_loadu_pd( _pdTexels );
				acc = _mm512_fmadd_pd( w, t, acc );
				_pdWeights += sizeof( __m512d ) / sizeof( double );
				_pdTexels += sizeof( __m512d ) / sizeof( double );
				_sTotal -= sizeof( __m512d ) / sizeof( double );
			}
		}
#endif	// #ifdef __AVX512F__
	}

}	// namespace sl2

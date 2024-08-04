/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A 4-element vector.
 */


#pragma once


#include "../OS/SL2Os.h"
#include "SL2SimdTypes.h"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <immintrin.h>


namespace sl2 {

	/**
	 * Class CVector
	 * \brief A 4-element vector.
	 *
	 * Description: A 4-element vector.
	 */
	SL2_ALIGN( 64 )
	class CVector {
	public :
		CVector() {
			m_dElements[0] = m_dElements[1] = m_dElements[2] = m_dElements[3] = 0.0;
		}
		CVector( double _dX, double _dY, double _dZ, double _dW ) {
			m_dElements[0] = _dX;
			m_dElements[1] = _dY;
			m_dElements[2] = _dZ;
			m_dElements[3] = _dW;
		}
		CVector( const double * _pdData ) {
			m_dElements[0] = (*_pdData++);
			m_dElements[1] = (*_pdData++);
			m_dElements[2] = (*_pdData++);
			m_dElements[3] = (*_pdData++);
		}


		// == Operators.
		/**
		 * Array access.
		 *
		 * \param _ui32Index Index of the dimension to get.
		 * \return Returns the value at the given dimension.
		 */
		inline double											operator [] ( size_t _sIndex ) const { return m_dElements[_sIndex]; }

		/**
		 * Array access.
		 *
		 * \param _ui32Index Index of the dimension to get.
		 * \return Returns the value at the given dimension.
		 */
		inline double &											operator [] ( size_t _sIndex ) { return m_dElements[_sIndex]; }


		// == Functions.
		/**
		 * Normalizes this vector.  Normalization is the process of adjusting the length of the vector so that it is
		 *	unit length (1 unit in length) while maintaining its direction.
		 * Accuracy/speed depends on the LSM_PERFORMANCE macro.
		 */
		template <unsigned _uSimd = SL2_ST_RAW>
		inline void												Normalize() {
			if constexpr ( SL2_ST_AVX == _uSimd ) {
				__m256d mElements = _mm256_load_pd( m_dElements );													// Load mElements into an AVX register.
				__m256d mSquared = _mm256_mul_pd( mElements, mElements );											// Square each element.

				// Sum the squared elements.
				__m256d mSum1 = _mm256_hadd_pd( mSquared, mSquared );												// Horizontal add.
				__m128d mSum2 = _mm_add_pd( _mm256_castpd256_pd128( mSum1 ), _mm256_extractf128_pd( mSum1, 1 ) );	// Add the lower and higher halves

				// Compute the length.
				double dLen = _mm_cvtsd_f64( _mm_sqrt_sd( _mm_setzero_pd(), mSum2 ) );								// Compute the square root of the sum of squares.
				double dInvLen = 1.0 / dLen;																		// Compute the inverse of the length.

				__m256d mInvLen = _mm256_set1_pd( dInvLen );														// Set all elements of a register to dInvLen.
				mElements = _mm256_mul_pd( mElements, mInvLen );													// Normalize the elements.
				_mm256_store_pd( m_dElements, mElements );															// Store the result back to memory.
			}
			else {
				double dInvLen = 1.0 / std::sqrt( m_dElements[0] * m_dElements[0] + m_dElements[1] * m_dElements[1] + m_dElements[2] * m_dElements[2] + m_dElements[3] * m_dElements[3] );
				m_dElements[0] *= dInvLen;
				m_dElements[1] *= dInvLen;
				m_dElements[2] *= dInvLen;
				m_dElements[3] *= dInvLen;
			}
		}

		/**
		 * Performs a dot-product operation.  The dot product of two normalized vectors is the cosine of the angle between
		 *	them.
		 *
		 * \param _v4bOther The vector against which the dot product is to be determined.
		 * \return Returns the dot product between the two vectors.
		 */
		template <unsigned _uSimd = SL2_ST_RAW>
		inline double 											Dot( const CVector &_v4bOther ) const {
			if constexpr ( SL2_ST_AVX == _uSimd ) {
				__m256d mVec1 = _mm256_load_pd( m_dElements );														// Load elements of the first vector into an AVX register.
				__m256d mVec2 = _mm256_load_pd( _v4bOther.m_dElements );											// Load elements of the second vector into an AVX register.
				__m256d mMul = _mm256_mul_pd( mVec1, mVec2 );														// Multiply corresponding elements.
				__m256d mHAdd = _mm256_hadd_pd( mMul, mMul );														// Horizontally add the results.

				__m128d mSunHi = _mm256_extractf128_pd( mHAdd, 1 );													// Extract high 128 bits.
				__m128d mSumLo = _mm256_castpd256_pd128( mHAdd );													// Extract low 128 bits.
				__m128d mFinalSum = _mm_add_pd( mSunHi, mSumLo );													// Add high and low parts.

				return _mm_cvtsd_f64( mFinalSum );																	// Return the dot product.
			}
			else {
				return m_dElements[0] * _v4bOther.m_dElements[0] + m_dElements[1] * _v4bOther.m_dElements[1] + m_dElements[2] * _v4bOther.m_dElements[2] + m_dElements[3] * _v4bOther.m_dElements[3];
			}
		}

		/**
		 * Gets the maximum component.
		 *
		 * \return Returns the maximum component.
		 */
		inline double 											Max() const {
			return std::max( std::max( std::max( m_dElements[0], m_dElements[1] ), m_dElements[2] ), m_dElements[3] );
		}

		/**
		 * Gets the minimum component.
		 *
		 * \return Returns the minimum component.
		 */
		inline double 											Min() const {
			return std::min( std::min( std::min( m_dElements[0], m_dElements[1] ), m_dElements[2] ), m_dElements[3] );
		}


		// == Members.
		/** The components. */
		double													m_dElements[4];
	};

}	// namespace sl2

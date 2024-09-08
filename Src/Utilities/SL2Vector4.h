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
	 * Class CVector4
	 * \brief A 4-element vector.
	 *
	 * Description: A 4-element vector.
	 */
	template <unsigned _uSimd = SL2_ST_RAW>
	class CVector4 {
	public :
		CVector4() {
#ifdef __AVX__
			if constexpr ( SL2_ST_AVX <= _uSimd ) {
				_mm256_store_pd( m_dElements, _mm256_setzero_pd() );
				return;
			}
#endif	// #ifdef __AVX__
			m_dElements[0] = m_dElements[1] = m_dElements[2] = m_dElements[3] = 0.0;
		}
		CVector4( double _dX, double _dY, double _dZ, double _dW ) {
#ifdef __AVX__
			if constexpr ( SL2_ST_AVX <= _uSimd ) {
				_mm256_store_pd( m_dElements, _mm256_set_pd( _dW, _dZ, _dY, _dX ) );
				return;
			}
#endif	// #ifdef __AVX__
			m_dElements[0] = _dX;
			m_dElements[1] = _dY;
			m_dElements[2] = _dZ;
			m_dElements[3] = _dW;
		}
		CVector4( const double * _pdData ) {
#ifdef __AVX__
			if constexpr ( SL2_ST_AVX <= _uSimd ) {
				_mm256_store_pd( m_dElements, _mm256_loadu_pd( _pdData ) );
				return;
			}
#endif	// #ifdef __AVX__
			m_dElements[0] = (*_pdData++);
			m_dElements[1] = (*_pdData++);
			m_dElements[2] = (*_pdData++);
			m_dElements[3] = (*_pdData++);
		}
		CVector4( const CVector4<SL2_ST_RAW> &_vOther ) {
#ifdef __AVX__
			if constexpr ( SL2_ST_AVX <= _uSimd ) {
				_mm256_store_pd( m_dElements, _mm256_load_pd( _vOther.m_dElements ) );
			}
#endif	// #ifdef __AVX__
			m_dElements[0] = _vOther[0];
			m_dElements[1] = _vOther[1];
			m_dElements[2] = _vOther[2];
			m_dElements[3] = _vOther[3];
		}
		CVector4( const CVector4<SL2_ST_NEON> &_vOther ) {
#ifdef __AVX__
			if constexpr ( SL2_ST_AVX <= _uSimd ) {
				_mm256_store_pd( m_dElements, _mm256_load_pd( _vOther.m_dElements ) );
			}
#endif	// #ifdef __AVX__
			m_dElements[0] = _vOther[0];
			m_dElements[1] = _vOther[1];
			m_dElements[2] = _vOther[2];
			m_dElements[3] = _vOther[3];
		}
		CVector4( const CVector4<SL2_ST_SSE4_1> &_vOther ) {
#ifdef __AVX__
			if constexpr ( SL2_ST_AVX <= _uSimd ) {
				_mm256_store_pd( m_dElements, _mm256_load_pd( _vOther.m_dElements ) );
			}
#endif	// #ifdef __AVX__
			m_dElements[0] = _vOther[0];
			m_dElements[1] = _vOther[1];
			m_dElements[2] = _vOther[2];
			m_dElements[3] = _vOther[3];
		}
		CVector4( const CVector4<SL2_ST_AVX> &_vOther ) {
#ifdef __AVX__
			if constexpr ( SL2_ST_AVX <= _uSimd ) {
				_mm256_store_pd( m_dElements, _mm256_load_pd( _vOther.m_dElements ) );
			}
#endif	// #ifdef __AVX__
			m_dElements[0] = _vOther[0];
			m_dElements[1] = _vOther[1];
			m_dElements[2] = _vOther[2];
			m_dElements[3] = _vOther[3];
		}
		CVector4( const CVector4<SL2_ST_AVX512> &_vOther ) {
#ifdef __AVX__
			if constexpr ( SL2_ST_AVX <= _uSimd ) {
				_mm256_store_pd( m_dElements, _mm256_load_pd( _vOther.m_dElements ) );
			}
#endif	// #ifdef __AVX__
			m_dElements[0] = _vOther[0];
			m_dElements[1] = _vOther[1];
			m_dElements[2] = _vOther[2];
			m_dElements[3] = _vOther[3];
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

		/**
		 * Copy.
		 * 
		 * \param _vOther The vector to copy.
		 * \return Returns the copied item.
		 **/
		inline CVector4<_uSimd> &								operator == ( const CVector4<SL2_ST_RAW> &_vOther ) {
#ifdef __AVX__
			if constexpr ( SL2_ST_AVX <= _uSimd ) {
				_mm256_store_pd( m_dElements, _mm256_load_pd( _vOther.m_dElements ) );
				return (*this);
			}
#endif	// #ifdef __AVX__
			m_dElements[0] = _vOther[0];
			m_dElements[1] = _vOther[1];
			m_dElements[2] = _vOther[2];
			m_dElements[3] = _vOther[3];
			return (*this);
		}

		/**
		 * Copy.
		 * 
		 * \param _vOther The vector to copy.
		 * \return Returns the copied item.
		 **/
		inline CVector4<_uSimd> &								operator == ( const CVector4<SL2_ST_NEON> &_vOther ) {
#ifdef __AVX__
			if constexpr ( SL2_ST_AVX <= _uSimd ) {
				_mm256_store_pd( m_dElements, _mm256_load_pd( _vOther.m_dElements ) );
				return (*this);
			}
#endif	// #ifdef __AVX__
			m_dElements[0] = _vOther[0];
			m_dElements[1] = _vOther[1];
			m_dElements[2] = _vOther[2];
			m_dElements[3] = _vOther[3];
			return (*this);
		}

		/**
		 * Copy.
		 * 
		 * \param _vOther The vector to copy.
		 * \return Returns the copied item.
		 **/
		inline CVector4<_uSimd> &								operator == ( const CVector4<SL2_ST_SSE4_1> &_vOther ) {
#ifdef __AVX__
			if constexpr ( SL2_ST_AVX <= _uSimd ) {
				_mm256_store_pd( m_dElements, _mm256_load_pd( _vOther.m_dElements ) );
				return (*this);
			}
#endif	// #ifdef __AVX__
			m_dElements[0] = _vOther[0];
			m_dElements[1] = _vOther[1];
			m_dElements[2] = _vOther[2];
			m_dElements[3] = _vOther[3];
			return (*this);
		}

		/**
		 * Copy.
		 * 
		 * \param _vOther The vector to copy.
		 * \return Returns the copied item.
		 **/
		inline CVector4<_uSimd> &								operator == ( const CVector4<SL2_ST_AVX> &_vOther ) {
#ifdef __AVX__
			if constexpr ( SL2_ST_AVX <= _uSimd ) {
				_mm256_store_pd( m_dElements, _mm256_load_pd( _vOther.m_dElements ) );
				return (*this);
			}
#endif	// #ifdef __AVX__
			m_dElements[0] = _vOther[0];
			m_dElements[1] = _vOther[1];
			m_dElements[2] = _vOther[2];
			m_dElements[3] = _vOther[3];
			return (*this);
		}

		/**
		 * Copy.
		 * 
		 * \param _vOther The vector to copy.
		 * \return Returns the copied item.
		 **/
		inline CVector4<_uSimd> &								operator == ( const CVector4<SL2_ST_AVX512> &_vOther ) {
#ifdef __AVX__
			if constexpr ( SL2_ST_AVX <= _uSimd ) {
				_mm256_store_pd( m_dElements, _mm256_load_pd( _vOther.m_dElements ) );
				return (*this);
			}
#endif	// #ifdef __AVX__
			m_dElements[0] = _vOther[0];
			m_dElements[1] = _vOther[1];
			m_dElements[2] = _vOther[2];
			m_dElements[3] = _vOther[3];
			return (*this);
		}


		// == Functions.
		/**
		 * Normalizes this vector.  Normalization is the process of adjusting the length of the vector so that it is
		 *	unit length (1 unit in length) while maintaining its direction.
		 * Accuracy/speed depends on the LSM_PERFORMANCE macro.
		 */
		inline CVector4<_uSimd>									Normalize() {
			CVector4 vCopy;
#ifdef __AVX__
			if constexpr ( SL2_ST_AVX <= _uSimd ) {
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
				_mm256_store_pd( vCopy.m_dElements, mElements );													// Store the result back to memory.
				return vCopy;
			}
#endif	// #ifdef __AVX__
			double dInvLen = 1.0 / std::sqrt( m_dElements[0] * m_dElements[0] + m_dElements[1] * m_dElements[1] + m_dElements[2] * m_dElements[2] + m_dElements[3] * m_dElements[3] );
			vCopy.m_dElements[0] = m_dElements[0] * dInvLen;
			vCopy.m_dElements[1] = m_dElements[1] * dInvLen;
			vCopy.m_dElements[2] = m_dElements[2] * dInvLen;
			vCopy.m_dElements[3] = m_dElements[3] * dInvLen;
			return vCopy;
		}

		/**
		 * Performs a dot-product operation.  The dot product of two normalized vectors is the cosine of the angle between
		 *	them.
		 *
		 * \param _v4bOther The vector against which the dot product is to be determined.
		 * \return Returns the dot product between the two vectors.
		 */
		inline double 											Dot( const CVector4 &_v4bOther ) const {
#ifdef __AVX__
			if constexpr ( SL2_ST_AVX <= _uSimd ) {
				__m256d mVec1 = _mm256_load_pd( m_dElements );														// Load elements of the first vector into an AVX register.
				__m256d mVec2 = _mm256_load_pd( _v4bOther.m_dElements );											// Load elements of the second vector into an AVX register.
				__m256d mMul = _mm256_mul_pd( mVec1, mVec2 );														// Multiply corresponding elements.
				__m256d mHAdd = _mm256_hadd_pd( mMul, mMul );														// Horizontally add the results.

				__m128d mSunHi = _mm256_extractf128_pd( mHAdd, 1 );													// Extract high 128 bits.
				__m128d mSumLo = _mm256_castpd256_pd128( mHAdd );													// Extract low 128 bits.
				__m128d mFinalSum = _mm_add_pd( mSunHi, mSumLo );													// Add high and low parts.

				return _mm_cvtsd_f64( mFinalSum );																	// Return the dot product.
			}
#endif	// #ifdef __AVX__
			return m_dElements[0] * _v4bOther.m_dElements[0] + m_dElements[1] * _v4bOther.m_dElements[1] + m_dElements[2] * _v4bOther.m_dElements[2] + m_dElements[3] * _v4bOther.m_dElements[3];
		}

		/**
		 * Gets the maximum component.
		 *
		 * \return Returns the maximum component.
		 */
		inline double 											Max() const {
#ifdef __AVX__
			if constexpr ( SL2_ST_AVX <= _uSimd ) {
				__m256d mElements = _mm256_loadu_pd( m_dElements );

				__m256d mMax1 = _mm256_max_pd( mElements, _mm256_permute2f128_pd( mElements, mElements, 0x1 ) );
				__m128d mMax2 = _mm_max_pd( _mm256_castpd256_pd128( mMax1 ), _mm256_extractf128_pd( mMax1, 1 ) );

				return _mm_cvtsd_f64( mMax2 );
			}
#endif	// #ifdef __AVX__
			return std::max( std::max( std::max( m_dElements[0], m_dElements[1] ), m_dElements[2] ), m_dElements[3] );
		}

		/**
		 * Gets the minimum component.
		 *
		 * \return Returns the minimum component.
		 */
		inline double 											Min() const {
#ifdef __AVX__
			if constexpr ( SL2_ST_AVX <= _uSimd ) {
				__m256d mElements = _mm256_loadu_pd( m_dElements );

				__m256d mMin1 = _mm256_min_pd( mElements, _mm256_permute2f128_pd( mElements, mElements, 0x1 ) );
				__m128d mMin2 = _mm_min_pd( _mm256_castpd256_pd128( mMin1 ), _mm256_extractf128_pd( mMin1, 1 ) );

				return _mm_cvtsd_f64( mMin2 );
			}
#endif	// #ifdef __AVX__
			return std::min( std::min( std::min( m_dElements[0], m_dElements[1] ), m_dElements[2] ), m_dElements[3] );
		}

		/**
		 * Sets the vector to 0.
		 **/
		inline CVector4<_uSimd>									Zero() {
#ifdef __AVX__
			if constexpr ( SL2_ST_AVX <= _uSimd ) {
				_mm256_store_pd( m_dElements, _mm256_setzero_pd() );
				return (*this);
			}
#endif	// #ifdef __AVX__
			m_dElements[0] = m_dElements[1] = m_dElements[2] = m_dElements[3] = 0.0;
			return (*this);
		}

		/**
		 * X.
		 * 
		 * \return Returns X.
		 **/
		inline double 											X() const { return m_dElements[0]; }

		/**
		 * Y.
		 * 
		 * \return Returns X.
		 **/
		inline double 											Y() const { return m_dElements[1]; }

		/**
		 * Z.
		 * 
		 * \return Returns X.
		 **/
		inline double 											Z() const { return m_dElements[2]; }

		/**
		 * W.
		 * 
		 * \return Returns X.
		 **/
		inline double 											W() const { return m_dElements[3]; }


		// == Members.
		/** The components. */
		SL2_ALIGN( 32 )
		double													m_dElements[4];
	};

}	// namespace sl2

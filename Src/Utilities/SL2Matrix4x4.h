/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A 4-element matrix.
 */


#pragma once


#include "../OS/SL2Os.h"
#include "SL2Vector.h"

#include <algorithm>
#include <cmath>
#include <cstdint>

#define SL2_MAT_EPSILON											1.192092896e-07


namespace sl2 {

	/**
	 * Class CMatrix4x4
	 * \brief A 4-element matrix.
	 *
	 * Description: A 4-element matrix.
	 */
	SL2_ALIGN( 64 )
	class CMatrix4x4 {
		CMatrix4x4() {
			std::memset( m_dElements, 0, sizeof( m_dElements ) );
		}
		CMatrix4x4( const CVector &_vVec0, const CVector &_vVec1, const CVector &_vVec2 ) {
			std::memcpy( (*this)[0], _vVec0.m_dElements, sizeof( CVector ) );
			std::memcpy( (*this)[1], _vVec1.m_dElements, sizeof( CVector ) );
			std::memcpy( (*this)[2], _vVec2.m_dElements, sizeof( CVector ) );
			std::memset( (*this)[3], 0, sizeof( CVector ) );
		}


		// == Operators.
		/**
		 * Array access.
		 *
		 * \param _ui32Index Index of the row to get.
		 * \return Returns a pointer to the given row.
		 */
		inline const double *									operator [] ( size_t _sIndex ) const { return &m_dElements[_sIndex<<2]; }

		/**
		 * Array access.
		 *
		 * \param _ui32Index Index of the row to get.
		 * \return Returns a pointer to the given row.
		 */
		inline double *											operator [] ( size_t _sIndex ) { return &m_dElements[_sIndex<<2]; }

		/**
		 * Multiplies this matrix with the given vector.
		 * 
		 * \param _vVec The vector to multiply against this matrix.
		 * \return Returns the multiplied vector.
		 **/
		inline CVector											operator * ( const CVector &_vVec ) const { return MultiplyVec4( (*this), _vVec ); }


		// == Functions.
		/**
		 * Creates an identity matrix.
		 *
		 * \return Returns this matrix after setting to identity.
		 **/
		inline CMatrix4x4 &										Identity() {
			for ( size_t I = 0; I < 4; ++I ) {
				for ( size_t J = 0; J < 4; ++J ) {
					(*this)[I][J] = I == J ? 1.0 : 0.0;
				}
			}
			return (*this);
		}

		/**
		 * Gets the inverse of this matrix.  Upon failure, the identity matrix is returned.
		 *
		 * \return Returns the inverse of this matrix, or identity on failure.
		 */
		CMatrix4x4												Inverse() const {
			CMatrix4x4 m44bOut;
			double fA0 = (*this)[0][0] * (*this)[1][1] - (*this)[0][1] * (*this)[1][0];
			double fA1 = (*this)[0][0] * (*this)[1][2] - (*this)[0][2] * (*this)[1][0];
			double fA2 = (*this)[0][0] * (*this)[1][3] - (*this)[0][3] * (*this)[1][0];
			double fA3 = (*this)[0][1] * (*this)[1][2] - (*this)[0][2] * (*this)[1][1];
			double fA4 = (*this)[0][1] * (*this)[1][3] - (*this)[0][3] * (*this)[1][1];
			double fA5 = (*this)[0][2] * (*this)[1][3] - (*this)[0][3] * (*this)[1][2];
			double fB0 = (*this)[2][0] * (*this)[3][1] - (*this)[2][1] * (*this)[3][0];
			double fB1 = (*this)[2][0] * (*this)[3][2] - (*this)[2][2] * (*this)[3][0];
			double fB2 = (*this)[2][0] * (*this)[3][3] - (*this)[2][3] * (*this)[3][0];
			double fB3 = (*this)[2][1] * (*this)[3][2] - (*this)[2][2] * (*this)[3][1];
			double fB4 = (*this)[2][1] * (*this)[3][3] - (*this)[2][3] * (*this)[3][1];
			double fB5 = (*this)[2][2] * (*this)[3][3] - (*this)[2][3] * (*this)[3][2];
			double fDet = fA0 * fB5 - fA1 * fB4 + fA2 * fB3 + fA3 * fB2 - fA4 * fB1 + fA5 * fB0;
			
			const CMatrix4x4 * pmSrc = this;
			
			if ( std::abs( fDet ) > SL2_MAT_EPSILON ) {
				double fTemp = (*pmSrc)[1][3];
				m44bOut[0][0] = +(*pmSrc)[1][1] * fB5 - (*pmSrc)[1][2] * fB4 + fTemp * fB3;
					m44bOut[1][0] = -(*pmSrc)[1][0] * fB5 + (*pmSrc)[1][2] * fB2 - fTemp * fB1;
					m44bOut[2][0] = +(*pmSrc)[1][0] * fB4 - (*pmSrc)[1][1] * fB2 + fTemp * fB0;
					m44bOut[3][0] = -(*pmSrc)[1][0] * fB3 + (*pmSrc)[1][1] * fB1 - (*pmSrc)[1][2] * fB0;
				fTemp = (*pmSrc)[0][3];
				m44bOut[0][1] = -(*pmSrc)[0][1] * fB5 + (*pmSrc)[0][2] * fB4 - fTemp * fB3;
					m44bOut[1][1] = +(*pmSrc)[0][0] * fB5 - (*pmSrc)[0][2] * fB2 + fTemp * fB1;
					m44bOut[2][1] = -(*pmSrc)[0][0] * fB4 + (*pmSrc)[0][1] * fB2 - fTemp * fB0;
					m44bOut[3][1] = +(*pmSrc)[0][0] * fB3 - (*pmSrc)[0][1] * fB1 + (*pmSrc)[0][2] * fB0;
				fTemp = (*pmSrc)[3][3];
				m44bOut[0][2] = +(*pmSrc)[3][1] * fA5 - (*pmSrc)[3][2] * fA4 + fTemp * fA3;
					m44bOut[1][2] = -(*pmSrc)[3][0] * fA5 + (*pmSrc)[3][2] * fA2 - fTemp * fA1;
					m44bOut[2][2] = +(*pmSrc)[3][0] * fA4 - (*pmSrc)[3][1] * fA2 + fTemp * fA0;
					m44bOut[3][2] = -(*pmSrc)[3][0] * fA3 + (*pmSrc)[3][1] * fA1 - (*pmSrc)[3][2] * fA0;
				fTemp = (*pmSrc)[2][3];
				m44bOut[0][3] = -(*pmSrc)[2][1] * fA5 + (*pmSrc)[2][2] * fA4 - fTemp * fA3;
					m44bOut[1][3] = +(*pmSrc)[2][0] * fA5 - (*pmSrc)[2][2] * fA2 + fTemp * fA1;
					m44bOut[2][3] = -(*pmSrc)[2][0] * fA4 + (*pmSrc)[2][1] * fA2 - fTemp * fA0;
					m44bOut[3][3] = +(*pmSrc)[2][0] * fA3 - (*pmSrc)[2][1] * fA1 + (*pmSrc)[2][2] * fA0;
				
				double fInvDet = 1.0 / fDet;
				if ( fInvDet != 1.0 ) {
					double * pfThis = reinterpret_cast<double *>(&m44bOut);
					(*pfThis++) *= fInvDet;
					(*pfThis++) *= fInvDet;
					(*pfThis++) *= fInvDet;
					(*pfThis++) *= fInvDet;
					(*pfThis++) *= fInvDet;
					(*pfThis++) *= fInvDet;
					(*pfThis++) *= fInvDet;
					(*pfThis++) *= fInvDet;
					(*pfThis++) *= fInvDet;
					(*pfThis++) *= fInvDet;
					(*pfThis++) *= fInvDet;
					(*pfThis++) *= fInvDet;
					(*pfThis++) *= fInvDet;
					(*pfThis++) *= fInvDet;
					(*pfThis++) *= fInvDet;
					(*pfThis) *= fInvDet;
				}
			}
			else {
				m44bOut.Identity();
			}
			
			return m44bOut;
		}

		/**
		 * Gets the determinant.
		 *
		 * \return Returns the determinant of this matrix.
		 */
		double													Determ() const {
			double fA0 = (*this)[0][0] * (*this)[1][1] - (*this)[0][1] * (*this)[1][0];
			double fA1 = (*this)[0][0] * (*this)[1][2] - (*this)[0][2] * (*this)[1][0];
			double fA2 = (*this)[0][0] * (*this)[1][3] - (*this)[0][3] * (*this)[1][0];
			double fA3 = (*this)[0][1] * (*this)[1][2] - (*this)[0][2] * (*this)[1][1];
			double fA4 = (*this)[0][1] * (*this)[1][3] - (*this)[0][3] * (*this)[1][1];
			double fA5 = (*this)[0][2] * (*this)[1][3] - (*this)[0][3] * (*this)[1][2];
			double fB0 = (*this)[2][0] * (*this)[3][1] - (*this)[2][1] * (*this)[3][0];
			double fB1 = (*this)[2][0] * (*this)[3][2] - (*this)[2][2] * (*this)[3][0];
			double fB2 = (*this)[2][0] * (*this)[3][3] - (*this)[2][3] * (*this)[3][0];
			double fB3 = (*this)[2][1] * (*this)[3][2] - (*this)[2][2] * (*this)[3][1];
			double fB4 = (*this)[2][1] * (*this)[3][3] - (*this)[2][3] * (*this)[3][1];
			double fB5 = (*this)[2][2] * (*this)[3][3] - (*this)[2][3] * (*this)[3][2];
			return fA0 * fB5 - fA1 * fB4 + fA2 * fB3 + fA3 * fB2 - fA4 * fB1 + fA5 * fB0;
		}

		/**
		 * Transforms vector (x, y, z, w) by a given matrix.
		 *
		 * \param _m44bMat The matrix by which to transform the given vector.
		 * \param _v4bIn The vector to transform.
		 */
		static CVector											MultiplyVec4( const CMatrix4x4 &_m44bMat, const CVector &_v4bIn ) {
			CVector _v4bOut;
			const double * pfIn = _v4bIn.m_dElements;
			_v4bOut[0] = _m44bMat[0][0] * pfIn[0] + _m44bMat[1][0] * pfIn[1] + 
				_m44bMat[2][0] * pfIn[2] + _m44bMat[3][0] * pfIn[3];
			_v4bOut[1] = _m44bMat[0][1] * pfIn[0] + _m44bMat[1][1] * pfIn[1] +
				_m44bMat[2][1] * pfIn[2] + _m44bMat[3][1] * pfIn[3];
			_v4bOut[2] = _m44bMat[0][2] * pfIn[0] + _m44bMat[1][2] * pfIn[1] +
				_m44bMat[2][2] * pfIn[2] + _m44bMat[3][2] * pfIn[3];
			_v4bOut[3] = _m44bMat[0][3] * pfIn[0] + _m44bMat[1][3] * pfIn[1] +
				_m44bMat[2][3] * pfIn[2] + _m44bMat[3][3] * pfIn[3];
			return _v4bOut;
		}


		// == Members.
		/** The components. */
		double													m_dElements[4*4];
	};

}	// namespace sl2

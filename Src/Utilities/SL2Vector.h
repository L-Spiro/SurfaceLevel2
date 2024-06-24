/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A 4-element vector.
 */


#pragma once


#include "../OS/SL2Os.h"

#include <algorithm>
#include <cmath>
#include <cstdint>


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
		void													Normalize() {
			double fInvLen = 1.0 / std::sqrt( m_dElements[0] * m_dElements[0] + m_dElements[1] * m_dElements[1] + m_dElements[2] * m_dElements[2] + m_dElements[3] * m_dElements[3] );
			m_dElements[0] *= fInvLen;
			m_dElements[1] *= fInvLen;
			m_dElements[2] *= fInvLen;
			m_dElements[3] *= fInvLen;
		}

		/**
		 * Performs a dot-product operation.  The dot product of two normalized vectors is the cosine of the angle between
		 *	them.
		 *
		 * \param _v4bOther The vector against which the dot product is to be determined.
		 * \return Returns the dot product between the two vectors.
		 */
		double 													Dot( const CVector &_v4bOther ) const {
			return m_dElements[0] * _v4bOther.m_dElements[0] + m_dElements[1] * _v4bOther.m_dElements[1] + m_dElements[2] * _v4bOther.m_dElements[2] + m_dElements[3] * _v4bOther.m_dElements[3];
		}

		/**
		 * Gets the maximum component.
		 *
		 * \return Returns the maximum component.
		 */
		double  												Max() const {
			return std::max( std::max( std::max( m_dElements[0], m_dElements[1] ), m_dElements[2] ), m_dElements[3] );
		}

		/**
		 * Gets the minimum component.
		 *
		 * \return Returns the minimum component.
		 */
		double   												Min() const {
			return std::min( std::min( std::min( m_dElements[0], m_dElements[1] ), m_dElements[2] ), m_dElements[3] );
		}


		// == Members.
		/** The components. */
		double													m_dElements[4];
	};

}	// namespace sl2

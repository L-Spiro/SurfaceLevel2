/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A kernel.
 */

#include "SL2Kernel.h"


namespace sl2 {

	CKernel::CKernel() :
		m_ui32Size( 0 ) {
	}
	CKernel::CKernel( const CKernel &_kOther ) :
		m_ui32Size( 0 ) {
		SetSize( _kOther.Size() );
		if ( m_vKernel.data() ) {
			std::memcpy( m_vKernel.data(), _kOther.m_vKernel.data(), sizeof( double ) * m_vKernel.size() );
		}
	}
	CKernel::~CKernel() {
	}


	// == Functions.
	/**
	 * Creates a 3-by-3 Sobel kernel.
	 *
	 * \return Returns true if the filter was created.
	 */
	bool CKernel::CreateSobel3x3() {
		if ( !SetSize( 3 ) ) { return false; }
		static const double dTable[3][3] = {
			-1.0, 0.0, 1.0,
			-2.0, 0.0, 2.0,
			-1.0, 0.0, 1.0,
		};
		std::memcpy( m_vKernel.data(), dTable, sizeof( dTable ) );
		return true;
	}

	/**
	 * Creates a 5-by-5 Sobel kernel.
	 *
	 * \return Returns true if the filter was created.
	 */
	bool CKernel::CreateSobel5x5() {
		if ( !SetSize( 5 ) ) { return false; }
		static const double dTable[5][5] = {
			-1.0, -2.0, 0.0, 2.0, 1.0,
            -2.0, -3.0, 0.0, 3.0, 2.0,
            -3.0, -4.0, 0.0, 4.0, 3.0,
            -2.0, -3.0, 0.0, 3.0, 2.0,
            -1.0, -2.0, 0.0, 2.0, 1.0,
		};
		std::memcpy( m_vKernel.data(), dTable, sizeof( dTable ) );
		return true;
	}

	/**
	 * Creates a 7-by-7 Sobel kernel.
	 *
	 * \return Returns true if the filter was created.
	 */
	bool CKernel::CreateSobel7x7() {
		if ( !SetSize( 7 ) ) { return false; }
		static const double dTable[7][7] = {
			-1.0, -2.0, -3.0, 0.0, 3.0, 2.0, 1.0,
            -2.0, -3.0, -4.0, 0.0, 4.0, 3.0, 2.0,
            -3.0, -4.0, -5.0, 0.0, 5.0, 4.0, 3.0,
            -4.0, -5.0, -6.0, 0.0, 6.0, 5.0, 4.0,
            -3.0, -4.0, -5.0, 0.0, 5.0, 4.0, 3.0,
            -2.0, -3.0, -4.0, 0.0, 4.0, 3.0, 2.0,
            -1.0, -2.0, -3.0, 0.0, 3.0, 2.0, 1.0,
		};
		std::memcpy( m_vKernel.data(), dTable, sizeof( dTable ) );
		return true;
	}

	/**
	 * Creates a 9-by-9 Sobel kernel.
	 *
	 * \return Returns true if the filter was created.
	 */
	bool CKernel::CreateSobel9x9() {
		if ( !SetSize( 9 ) ) { return false; }
		static const double dTable[9][9] = {
			-1.0, -2.0, -3.0, -4.0, 0.0, 4.0, 3.0, 2.0, 1.0,
            -2.0, -3.0, -4.0, -5.0, 0.0, 5.0, 4.0, 3.0, 2.0,
            -3.0, -4.0, -5.0, -6.0, 0.0, 6.0, 5.0, 4.0, 3.0,
            -4.0, -5.0, -6.0, -7.0, 0.0, 7.0, 6.0, 5.0, 4.0,
            -5.0, -6.0, -7.0, -8.0, 0.0, 8.0, 7.0, 6.0, 5.0,
            -4.0, -5.0, -6.0, -7.0, 0.0, 7.0, 6.0, 5.0, 4.0,
            -3.0, -4.0, -5.0, -6.0, 0.0, 6.0, 5.0, 4.0, 3.0,
            -2.0, -3.0, -4.0, -5.0, 0.0, 5.0, 4.0, 3.0, 2.0,
            -1.0, -2.0, -3.0, -4.0, 0.0, 4.0, 3.0, 2.0, 1.0,
		};
		std::memcpy( m_vKernel.data(), dTable, sizeof( dTable ) );
		return true;
	}

}	// namespace sl2

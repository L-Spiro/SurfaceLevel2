/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A kernel.
 */


#pragma once

#include <vector>


namespace sl2 {

	/**
	 * Class CKernel
	 * \brief A kernel.
	 *
	 * Description: A kernel.
	 */
	class CKernel {
	public :
		CKernel();
		CKernel( const CKernel &_kOther );
		~CKernel();


		// == Operators.
		/**
		 * Gets a pointer to the start of the given line in the kernel.  Meant to be used for double array access.
		 *
		 * \param _ui32Index Index of the row to get.
		 * \return Returns a pointer to the start of the given row.
		 */
		inline double *											operator [] ( uint32_t _ui32Index );

		/**
		 * Gets a read-only pointer to the start of the given line in the kernel.  Meant to be used for double array access.
		 *
		 * \param _ui32Index Index of the row to get.
		 * \return Returns a pointer to the start of the given row.
		 */
		inline const double *									operator [] ( uint32_t _ui32Index ) const;

		/**
		 * Copy operator.
		 *
		 * \param _kOther The object to copy.
		 * \return Returns a reference to this object after the copy.
		 */
		inline CKernel &										operator = ( const CKernel &_kOther );


		// == Functions.
		/**
		 * Sets the size of the kernel.  The kernel is initialized to all 0's.
		 *
		 * \param _ui32Size The new size of the kernel.
		 * \return Returns true if the new kernel was made.
		 */
		inline bool												SetSize( uint32_t _ui32Size );

		/**
		 * Gets the size of the kernel.
		 *
		 * \return Returns the size of the kernel.
		 */
		inline uint32_t 										Size() const;

		/**
		 * Transposes the kernel in-place.
		 */
		inline void 											Transpose();

		/**
		 * Normalizes the kernel.
		 */
		inline void												Normalize();

		/**
		 * Creates a 3-by-3 Sobel kernel.
		 *
		 * \return Returns true if the filter was created.
		 */
		bool													CreateSobel3x3();

		/**
		 * Creates a 5-by-5 Sobel kernel.
		 *
		 * \return Returns true if the filter was created.
		 */
		bool													CreateSobel5x5();

		/**
		 * Creates a 7-by-7 Sobel kernel.
		 *
		 * \return Returns true if the filter was created.
		 */
		bool													CreateSobel7x7();

		/**
		 * Creates a 9-by-9 Sobel kernel.
		 *
		 * \return Returns true if the filter was created.
		 */
		bool													CreateSobel9x9();


	protected :
		// == Members.
		/** The size of the kernel. */
		uint32_t												m_ui32Size;
		/**< The kernel. */
		std::vector<double>										m_vKernel;
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Operators.
	/**
	 * Gets a pointer to the start of the given line in the kernel.  Meant to be used for double array access.
	 *
	 * \param _ui32Index Index of the row to get.
	 * \return Returns a pointer to the start of the given row.
	 */
	inline double * CKernel::operator [] ( uint32_t _ui32Index ) {
		return &m_vKernel[_ui32Index*m_ui32Size];
	}

	/**
	 * Gets a read-only pointer to the start of the given line in the kernel.  Meant to be used for double array access.
	 *
	 * \param _ui32Index Index of the row to get.
	 * \return Returns a pointer to the start of the given row.
	 */
	inline const double * CKernel::operator [] ( uint32_t _ui32Index ) const {
		return &m_vKernel[_ui32Index*m_ui32Size];
	}

	/**
	 * Copy operator.
	 *
	 * \param _kOther The object to copy.
	 * \return Returns a reference to this object after the copy.
	 */
	inline CKernel & CKernel::operator = ( const CKernel &_kOther ) {
		if ( SetSize( _kOther.Size() ) ) {
			std::memcpy( m_vKernel.data(), _kOther.m_vKernel.data(), Size() * Size() * sizeof( double ) );
		}
		return (*this);
	}

	// == Functions.
	/**
	 * Sets the size of the kernel.  The kernel is initialized to all 0's.
	 *
	 * \param _ui32Size The new size of the kernel.
	 * \return Returns true if the new kernel was made.
	 */
	inline bool CKernel::SetSize( uint32_t _ui32Size ) {
		if ( !_ui32Size ) {
			m_ui32Size = 0;
			m_vKernel.clear();
			m_vKernel = std::vector<double>();
			return true;
		}
		try {
			m_vKernel.resize( _ui32Size * _ui32Size );
		}
		catch ( ... ) { m_ui32Size = 0; return false; }

		std::memset( m_vKernel.data(), 0, _ui32Size * _ui32Size * sizeof( m_vKernel[0] ) );
		m_ui32Size = _ui32Size;
		return true;
	}

	/**
	 * Gets the size of the kernel.
	 *
	 * \return Returns the size of the kernel.
	 */
	inline uint32_t CKernel::Size() const {
		return m_ui32Size;
	}

	/**
	 * Transposes the kernel in-place.
	 */
	inline void CKernel::Transpose() {
		for ( uint32_t Y = 0; Y < Size(); ++Y ) {
			for ( uint32_t X = 0; X < Size(); ++X ) {
				if ( Y == X ) { continue; }
				std::swap( (*this)[Y][X], (*this)[X][Y] );
			}
		}
	}

	/**
	 * Normalizes the kernel.
	 */
	inline void CKernel::Normalize() {
		double dSum = 0.0;
		for ( uint32_t I = Size() * Size(); I--; ) {
			dSum += ::fabs( m_vKernel[I] );
		}
		double fDiv = 1.0 / dSum;
		for ( uint32_t I = Size() * Size(); I--; ) {
			m_vKernel[I] *= fDiv;
		}
	}

}	// namespace sl2

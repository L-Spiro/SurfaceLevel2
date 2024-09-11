/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A palette and palette functionality.
 */

#pragma once

#include "../Utilities/SL2Vector4.h"
#include "SL2Formats.h"

#include <vector>


namespace sl2 {

	/**
	 * Class CImage
	 * \brief A palette and palette functionality.
	 *
	 * Description: A palette and palette functionality.
	 */
	class CPalette {
	public :
		CPalette();
		~CPalette();


		// == Types.
		/** A color. */
		typedef CVector4<SL2_ST_AVX512>						CColor;

		/** The palette. */
		typedef std::vector<CColor>							CPal;


		// == Functions.
		/**
		 * Appends a color to the palette.
		 * 
		 * \param _cColor The color to add.
		 * \return Returns true if the color was appended successfully.
		 **/
		inline bool											Add( const CColor &_cColor );

		/**
		 * Resets the palette back to scratch.
		 **/
		void												Reset();

		/**
		 * Gets a constant pointer to the array of colors.
		 * 
		 * \return Returns a constant pointer to the array of colors.
		 **/
		inline const CPal &									Palette() const { return m_pPalette; }

		/**
		 * Sets the palette format.
		 * 
		 * \param _pkifdFormat The format of the palette.
		 **/
		inline void											SetFormat( const CFormat::SL2_KTX_INTERNAL_FORMAT_DATA * _pkifdFormat );

		/**
		 * Gets the palette format.
		 * 
		 * \return Returns the palette format.
		 **/
		inline const CFormat::SL2_KTX_INTERNAL_FORMAT_DATA *Format() const;

		/**
		 * Sets the size of the palette.
		 * 
		 * \param _ui32Size The new size of the palette.
		 * \return Returns true if allocation of the new palette size succeds.
		 **/
		bool												SetSize( uint32_t _ui32Size );

		/**
		 * Gets a pointer to the palette data.
		 * 
		 * \return Returns a pointer to the palette data.
		 **/
		inline CColor *										Data();

		/**
		 * Generates a palette of a given size using K-Means.
		 * 
		 * \param _pcColors Input array of colors.
		 * \param _sColorsSize Number of colors to which _pcColors points.
		 * \param _ui32Size Size of the palette to generate.
		 * \param _sIterations Number of refinement iterations.
		 * \return Returns true if all internal allocations succeed.
		 **/
		bool												GenPalette_kMeans( const CColor * _pcColors, size_t _sColorsSize, uint32_t _ui32Size, size_t _sIterations = 10 );


	protected :
		CPal												m_pPalette;								/**< The actual palette. */
		const CFormat::SL2_KTX_INTERNAL_FORMAT_DATA *		m_pkifFormat = nullptr;					/**< The palette format. */


		// == Functions.
		/**
		 * A better distruction of initial clusters.
		 * 
		 * \param _pcColors Input array of colors.
		 * \param _sColorsSize Number of colors to which _pcColors points.
		 * \param _vCentroids Input array of centroids.
		 * \param _sK A pretty cool dude IMO.
		 **/
		static void											InitializeCentroidsKMeansPlusPlus( const CColor * _pcColors, size_t _sColorsSize, std::vector<CColor> &_vCentroids, size_t _sK );

		/**
		 * Checks for convergence between 2 centroid lists.
		 * 
		 * \param _vOldCentroids The old centroid list.
		 * \param _vNewCentroids The new centroid list.
		 * \param _dTolerance Tolerance for change.
		 * \return Returns true if the given lists are the same within tolerance.
		 **/
		static bool											HasConverged( const std::vector<CColor> &_vOldCentroids, const std::vector<CColor> &_vNewCentroids, double _dTolerance );

		/**
		 * Cluster assignment.  It's pretty cool.
		 * 
		 * \param _pcColors Input array of colors.
		 * \param _sColorsSize Number of colors to which _pcColors points.
		 * \param _vCentroids Input array of centroids.
		 * \param _vClusterAssignment Output cluster-assignment results.
		 * \param _sK A pretty cool dude IMO.
		 **/
		static void											AssignClusters( const CColor * _pcColors, size_t _sColorsSize, const std::vector<CColor> & _vCentroids, std::vector<size_t> & _vClusterAssignment, size_t _sK );

		/**
		 * Updates centroids like a BOSS.
		 * 
		 * \param _pcColors Input array of colors.
		 * \param _sColorsSize Number of colors to which _pcColors points.
		 * \param _vCentroids Output array of centroids.
		 * \param _vClusterAssignment Output cluster-assignment results.
		 * \param _vClusterSize Output cluster-size results.
		 * \param _sK A pretty cool dude IMO.
		 **/
		static void											UpdateCentroids( const CColor * _pcColors, size_t _sColorsSize, std::vector<CColor> & _vCentroids, std::vector<size_t> & _vClusterAssignment, std::vector<size_t> & _vClusterSize, size_t _sK );

		/**
		 * Implements K-Means color quantization to generate a palette.
		 * 
		 * \param _pcColors Input array of colors.
		 * \param _sColorsSize Number of colors to which _pcColors points.
		 * \param _pPalette The palette to generate.
		 * \param _sK A pretty cool dude IMO.
		 * \param _sIterations Number of iterations.
		 * \return Returns true if all internal allocations succeed.
		 **/
		static bool											kMeansColorQuantization( const CColor * _pcColors, size_t _sColorsSize, CPal & _pPalette, size_t _sK, size_t _sIterations );
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Functions.
	/**
	 * Appends a color to the palette.
	 * 
	 * \param _cColor The color to add.
	 * \return Returns true if the color was appended successfully.
	 **/
	inline bool CPalette::Add( const CColor &_cColor ) {
		try {
			m_pPalette.push_back( _cColor );
		}
		catch ( ... ) { return false; }
		return true;
	}

	/**
	 * Sets the palette format.
	 * 
	 * \param _pkifdFormat The format of the palette.
	 **/
	inline void CPalette::SetFormat( const CFormat::SL2_KTX_INTERNAL_FORMAT_DATA * _pkifdFormat ) {
		m_pkifFormat = _pkifdFormat;
	}

	/**
	 * Gets the palette format.
	 * 
	 * \return Returns the palette format.
	 **/
	inline const CFormat::SL2_KTX_INTERNAL_FORMAT_DATA * CPalette::Format() const { return m_pkifFormat; }

	/**
	 * Gets a pointer to the palette data.
	 * 
	 * \return Returns a pointer to the palette data.
	 **/
	inline CPalette::CColor * CPalette::Data() { return m_pPalette.data(); }

}	// namespace sl2

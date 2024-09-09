/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A palette and palette functionality.
 */


#include "SL2Palette.h"

namespace sl2 {

	CPalette::CPalette() {
	}
	CPalette::~CPalette() {
		Reset();
	}

	// == Functions.
	/**
	 * Resets the palette back to scratch.
	 **/
	void CPalette::Reset() {
		m_pPalette.clear();
		m_pPalette = CPal();
		m_pkifFormat = nullptr;
	}

	/**
	 * Sets the size of the palette.
	 * 
	 * \param _ui32Size The new size of the palette.
	 * \return Returns true if allocation of the new palette size succeds.
	 **/
	bool CPalette::SetSize( uint32_t _ui32Size ) {
		size_t sStart = m_pPalette.size();
		try {
			m_pPalette.resize( _ui32Size );
		}
		catch ( ... ) { return false; }
		for ( auto I = sStart; I < m_pPalette.size(); ++I ) {
			m_pPalette[I].Zero();
		}
		return true;
	}

	/**
	 * Generates a palette of a given size using K-Means.
	 * 
	 * \param _pvColors The input array of colors.
	 * \param _ui32Size Size of the palette to generate.
	 * \param _sIterations Number of refinement iterations.
	 * \return Returns true if all internal allocations succeed.
	 **/
	bool CPalette::GenPalette_kMeans( const std::vector<CColor> * _pvColors, uint32_t _ui32Size, size_t _sIterations ) {
		return kMeansColorQuantization( (*_pvColors), m_pPalette, _ui32Size, _sIterations );
	}

	/**
	 * Cluster assignment.  It's pretty cool.
	 * 
	 * \param _vColors Input array of colors.
	 * \param _vCentroids Input array of centroids.
	 * \param _vClusterAssignment Output cluster-assignment results.
	 * \param _sK A pretty cool dude IMO.
	 **/
	void CPalette::AssignClusters( const std::vector<CColor> & _vColors, const std::vector<CColor> & _vCentroids, std::vector<size_t> & _vClusterAssignment, size_t _sK ) {
		auto aImgSize = _vColors.size();
		for ( size_t C = 0; C < aImgSize; ++C ) {
			double dBestDist = CColor::ColorDistance( _vColors[C], _vCentroids[0] );
			size_t sBest = 0;

			for ( size_t J = 1; J < _sK; ++J ) {
				double dDIst = CColor::ColorDistance( _vColors[C], _vCentroids[J] );
				if (dDIst < dBestDist) {
					dBestDist = dDIst;
					sBest = J;
				}
			}

			_vClusterAssignment[C] = sBest;
		}
	}

	/**
	 * Updates centroids like a BOSS.
	 * 
	 * \param _vColors Input array of colors.
	 * \param _vCentroids Input array of centroids.
	 * \param _vClusterAssignment Output cluster-assignment results.
	 * \param _vClusterSize Output cluster-size results.
	 * \param _sK A pretty cool dude IMO.
	 **/
	void CPalette::UpdateCentroids( const std::vector<CColor> & _vColors, std::vector<CColor> & _vCentroids, std::vector<size_t> & _vClusterAssignment, std::vector<size_t> & _vClusterSize, size_t _sK ) {
		std::vector<CColor> vNewCentroids( _sK, { 0.0, 0.0, 0.0, 0.0 } );
		_vClusterSize.assign( _sK, 0 );

		size_t sImageSize = _vColors.size();
		for ( size_t C = 0; C < sImageSize; ++C ) {
			size_t sCluster = _vClusterAssignment[C];
			vNewCentroids[sCluster] += _vColors[C];
			_vClusterSize[sCluster]++;
		}

		for ( size_t J = 0; J < _sK; ++J ) {
			if ( _vClusterSize[J] > 0 ) {
				_vCentroids[J] = (vNewCentroids[J] / static_cast<double>(_vClusterSize[J]));
			}
		}
	}

	/**
	 * Implements K-Means color quantization to generate a palette.
	 * 
	 * \param _vColors Input array of colors.
	 * \param _pPalette The palette to generate.
	 * \param _sK A pretty cool dude IMO.
	 * \param _sIterations Number of iterations.
	 * \return Returns true if all internal allocations succeed.
	 **/
	bool CPalette::kMeansColorQuantization( const std::vector<CColor> & _vColors, CPal & _pPalette, size_t _sK, size_t _sIterations ) {
		try {
			std::vector<CColor> vCentroids( _sK );
			std::vector<size_t> vclsterAssignment( _vColors.size() );
			std::vector<size_t> vClstSize( _sK );

			// Initialize centroids randomly from image colors.
			for ( size_t I = 0; I < _sK; ++I ) {
				vCentroids[I] = _vColors[I%_vColors.size()];
			}

			for ( size_t sIter = 0; sIter < _sIterations; ++sIter ) {
				AssignClusters( _vColors, vCentroids, vclsterAssignment, _sK );
				UpdateCentroids( _vColors, vCentroids, vclsterAssignment, vClstSize, _sK );
			}

			_pPalette = vCentroids;
			return true;
		}
		catch ( ... ) { return false; }
	}

}	// namespace sl2

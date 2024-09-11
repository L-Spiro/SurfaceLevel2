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
	 * \param _pcColors Input array of colors.
	 * \param _sColorsSize Number of colors to which _pcColors points.
	 * \param _ui32Size Size of the palette to generate.
	 * \param _sIterations Number of refinement iterations.
	 * \return Returns true if all internal allocations succeed.
	 **/
	bool CPalette::GenPalette_kMeans( const CColor * _pcColors, size_t _sColorsSize, uint32_t _ui32Size, size_t _sIterations ) {
		return kMeansColorQuantization( _pcColors, _sColorsSize, m_pPalette, _ui32Size, _sIterations );
	}

	/**
	 * A better distruction of initial clusters.
	 * 
	 * \param _pcColors Input array of colors.
	 * \param _sColorsSize Number of colors to which _pcColors points.
	 * \param _vCentroids Input array of centroids.
	 * \param _sK A pretty cool dude IMO.
	 **/
	void CPalette::InitializeCentroidsKMeansPlusPlus( const CColor * _pcColors, size_t _sColorsSize, std::vector<CColor> &_vCentroids, size_t _sK ) {
		// Initialize first centroid randomly
		_vCentroids[0] = _pcColors[rand() % _sColorsSize];

		// Initialize remaining centroids based on their distances from already chosen centroids
		for ( size_t I = 1; I < _sK; ++I ) {
			double dMaxDist = 0.0;
			size_t sBest = 0;
			for ( size_t J = 0; J < _sColorsSize; ++J ) {
				double dMinDist = std::numeric_limits<double>::max();
				for ( size_t C = 0; C < I; ++C ) {
					double dDist = CColor::EuclideanDistanceSq( _pcColors[J], _vCentroids[C] );
					if ( dDist < dMinDist ) {
						dMinDist = dDist;
					}
				}
				if ( dMinDist > dMaxDist ) {
					dMaxDist = dMinDist;
					sBest = J;
				}
			}
			_vCentroids[I] = _pcColors[sBest];
		}
	}

	/**
	 * Checks for convergence between 2 centroid lists.
	 * 
	 * \param _vOldCentroids The old centroid list.
	 * \param _vNewCentroids The new centroid list.
	 * \param _dTolerance Tolerance for change.
	 * \return Returns true if the given lists are the same within tolerance.
	 **/
	bool CPalette::HasConverged( const std::vector<CColor> &_vOldCentroids, const std::vector<CColor> &_vNewCentroids, double _dTolerance ) {
		for ( size_t I = 0; I < _vOldCentroids.size(); ++I ) {
			if ( CColor::EuclideanDistanceSq(_vOldCentroids[I], _vNewCentroids[I]) > _dTolerance ) {
				return false;
			}
		}
		return true;
	}

	/**
	 * Cluster assignment.  It's pretty cool.
	 * 
	 * \param _pcColors Input array of colors.
	 * \param _sColorsSize Number of colors to which _pcColors points.
	 * \param _vCentroids Input array of centroids.
	 * \param _vClusterAssignment Output cluster-assignment results.
	 * \param _sK A pretty cool dude IMO.
	 **/
	void CPalette::AssignClusters( const CColor * _pcColors, size_t _sColorsSize, const std::vector<CColor> & _vCentroids, std::vector<size_t> & _vClusterAssignment, size_t _sK ) {
		auto aImgSize = _sColorsSize;
		for ( size_t C = 0; C < aImgSize; ++C ) {
			double dBestDist = CColor::EuclideanDistanceSq( _pcColors[C], _vCentroids[0] );
			size_t sBest = 0;

			for ( size_t J = 1; J < _sK; ++J ) {
				double dDist = CColor::EuclideanDistanceSq( _pcColors[C], _vCentroids[J] );
				if ( dDist < dBestDist ) {
					dBestDist = dDist;
					sBest = J;
				}
			}

			_vClusterAssignment[C] = sBest;
		}
	}

	/**
	 * Updates centroids like a BOSS.
	 * 
	 * \param _pcColors Input array of colors.
	 * \param _sColorsSize Number of colors to which _pcColors points.
	 * \param _vCentroids Input array of centroids.
	 * \param _vClusterAssignment Output cluster-assignment results.
	 * \param _vClusterSize Output cluster-size results.
	 * \param _sK A pretty cool dude IMO.
	 **/
	void CPalette::UpdateCentroids( const CColor * _pcColors, size_t _sColorsSize, std::vector<CColor> & _vCentroids, std::vector<size_t> & _vClusterAssignment, std::vector<size_t> & _vClusterSize, size_t _sK ) {
		std::vector<CColor> vNewCentroids( _sK, { 0.0, 0.0, 0.0, 0.0 } );
		_vClusterSize.assign( _sK, 0 );

		size_t sImageSize = _sColorsSize;
		for ( size_t C = 0; C < sImageSize; ++C ) {
			size_t sCluster = _vClusterAssignment[C];
			vNewCentroids[sCluster] += _pcColors[C];
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
	 * \param _pcColors Input array of colors.
	 * \param _sColorsSize Number of colors to which _pcColors points.
	 * \param _pPalette The palette to generate.
	 * \param _sK A pretty cool dude IMO.
	 * \param _sIterations Number of iterations.
	 * \return Returns true if all internal allocations succeed.
	 **/
	bool CPalette::kMeansColorQuantization( const CColor * _pcColors, size_t _sColorsSize, CPal & _pPalette, size_t _sK, size_t _sIterations ) {
		try {
			std::vector<CColor> vCentroids( _sK );
			std::vector<size_t> vClsterAssignment( _sColorsSize );
			std::vector<size_t> vClstSize( _sK );

			// Initialize centroids using k-means++ instead of random initialization
			InitializeCentroidsKMeansPlusPlus( _pcColors, _sColorsSize, vCentroids, _sK );

			std::vector<CColor> vOldCentroids(vCentroids);

			for ( size_t sIter = 0; sIter < _sIterations; ++sIter ) {
				AssignClusters( _pcColors, _sColorsSize, vCentroids, vClsterAssignment, _sK );
				UpdateCentroids( _pcColors, _sColorsSize, vCentroids, vClsterAssignment, vClstSize, _sK );

				if ( HasConverged( vOldCentroids, vCentroids, 1e-5 ) ) { break; }
				vOldCentroids = vCentroids;
			}

			_pPalette = vCentroids;
			return true;
		}
		catch ( ... ) { return false; }
	}

}	// namespace sl2

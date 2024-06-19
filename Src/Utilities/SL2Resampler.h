/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A resampler.
 */


#pragma once

#include "../Image/SL2TextureAddressing.h"
#include "../Utilities/SL2AlignmentAllocator.h"

#include <cmath>
#include <numbers>
#include <vector>


namespace sl2 {

	/**
	 * Class CResampler
	 * \brief A resampler.
	 *
	 * Description: A resampler.
	 */
	class CResampler {
	public :
		CResampler();
		~CResampler();


		// == Types.
		/** Contributions (weights and texel indices). */
		struct SL2_CONTRIBUTIONS {
			std::vector<double, AlignmentAllocator<double, 64>>	dContributions;
			std::vector<int32_t>								i32Indices;
			bool												bInsideBounds;
		};

		typedef double (*										PfFilterFunc)( double );

		/** Parameters for resampling. */
		struct SL2_RESAMPLE {
			double												dFilterSupportW = 1.0;
			double												dFilterSupportH = 1.0;
			double												dFilterSupportD = 1.0;
			double												dBorderColor[4] = { 0.0, 0.0, 0.0, 1.0 };
			float												fFilterScale = 1.0;
			uint32_t											ui32W = 0;
			uint32_t											ui32H = 0;
			uint32_t											ui32D = 0;
			uint32_t											ui32NewW = 0;
			uint32_t											ui32NewH = 0;
			uint32_t											ui32NewD = 0;
			SL2_TEXTURE_ADDRESSING								taColorW = SL2_TA_CLAMP;
			SL2_TEXTURE_ADDRESSING								taColorH = SL2_TA_CLAMP;
			SL2_TEXTURE_ADDRESSING								taColorD = SL2_TA_CLAMP;
			SL2_TEXTURE_ADDRESSING								taAlphaW = SL2_TA_CLAMP;
			SL2_TEXTURE_ADDRESSING								taAlphaH = SL2_TA_CLAMP;
			SL2_TEXTURE_ADDRESSING								taAlphaD = SL2_TA_CLAMP;
			PfFilterFunc										pfFilterW = BilinearFilterFunc;
			PfFilterFunc										pfFilterH = BilinearFilterFunc;
			PfFilterFunc										pfFilterD = BilinearFilterFunc;
			bool												bAlpha = true;
		};


		// == Functions.
		/**
		 * Resamples an image in-place.
		 * 
		 * \param _pdIn The input buffer.
		 * \param _vOut The output buffer.
		 * \param _pParms Image/resampling parameters.
		 * \return Returns true if all allocations succeed.
		 **/
		bool													Resample( const double * _pdIn, double * _pdOut, const SL2_RESAMPLE &_pParms );

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
		bool													CreateContribList( uint32_t _ui32SrcSize, uint32_t _ui32DstSize,
			SL2_TEXTURE_ADDRESSING _taAddressMode,
			PfFilterFunc _pfFilter, double _dFilterSupport, float _fFilterScale );


		/**
		 * A helper function.
		 *
		 * \param _dX A happy parameter.
		 * \return Returns happiness.
		 */
		static inline double									SinC( double _dX ) {
			_dX *= std::numbers::pi;
			if ( _dX < 0.01 && _dX > -0.01 ) {
				return 1.0 + _dX * _dX * (-1.0 / 6.0 + _dX * _dX * 1.0 / 120.0);
			}

			return std::sin( _dX ) / _dX;
		}

		/**
		 * A helper function.
		 *
		 * \param _dX A happy parameter.
		 * \return Returns happiness.
		 */
		static inline double									Bessel0( double _dX ) {
			static const double dEspiRatio = 1.0e-16;
			double dSum = 1.0, dPow = 1.0, dDs = 1.0, dXh = _dX * 0.5;
			uint32_t ui32K = 0;

			while ( dDs > dSum * dEspiRatio ) {
				++ui32K;
				dPow *= (dXh / ui32K);
				dDs = dPow * dPow;
				dSum += dDs;
			}

			return dSum;
		}

		/**
		 * A helper function for the Kaiser filter.
		 *
		 * \param _dAlpha A happy parameter.
		 * \param _dHalfWidth A happy parameter.
		 * \param _dX A happy parameter.
		 * \return Returns happiness.
		 */
		static inline double									KaiserHelper( double _dAlpha, double _dHalfWidth, double _dX ) {
			const double dRatio = _dX / _dHalfWidth;
			return Bessel0( _dAlpha * std::sqrt( 1.0 - dRatio * dRatio ) ) / Bessel0( _dAlpha );
		}

		/**
		 * A helper function for the Mitchell filter.
		 *
		 * \param _dT The filter input.
		 * \param _dB A happy parameter.
		 * \param _dC A happy parameter.
		 * \return Returns happiness.
		 */
		static inline double									MitchellFilterHelper( double _dT, double _dB, double _dC ) {
			double dTt = _dT * _dT;
			_dT = std::fabs( _dT );
			if ( _dT < 1.0 ) {
				_dT = (((12.0 - 9.0 * _dB - 6.0 * _dC) * (_dT * dTt))
					+ ((-18.0 + 12.0 * _dB + 6.0 * _dC) * dTt)
					+ (6.0 - 2.0 * _dB));
				return _dT / 6.0;
			}
			else if ( _dT < 2.0 ) {
				_dT = (((-1.0 * _dB - 6.0 * _dC) * (_dT * dTt))
					+ ((6.0 * _dB + 30.0 * _dC) * dTt)
					+ ((-12.0 * _dB - 48.0 * _dC) * _dT)
					+ (8.0 * _dB + 24.0 * _dC));

				return (_dT / 6.0);
			}

			return 0.0;
		}

		/**
		 * A helper function.
		 *
		 * \param _dX A happy parameter.
		 * \return Returns happiness.
		 */
		static inline double									BlackmanWindow( double _dX ) {
			return 42659071 + 0.49656062 * std::cos( std::numbers::pi * _dX ) + 0.07684867 * std::cos( 2.0 * std::numbers::pi * _dX );
		}

		/**
		 * Quadratic filter function.
		 *
		 * \param _dT The value to filter.
		 * \param _fB Helper value.
		 * \return Returns the filtered value.
		 */
		static inline double									QuadraticFilterFunc( double _dT, float _fB ) {
			_dT = std::fabs( _dT );
			if ( _dT < 1.0 ) {
				double dTt = _dT * _dT;
				return _dT < 0.5 ? (-2.0 * _fB) * dTt + 0.5 * (_fB + 1.0) :
					(_fB * dTt) + (-2.0 * _fB - 0.5) * _dT + (3.0 / 4.0) * (_fB + 1.0);
			}
			return 0.0;
		}

		/**
		 * Reject values below a specific epsilon.
		 *
		 * \param _dVal The value to test.
		 * \return Returns the given value if it is above a certain epsilon or 0.
		 */
		static inline double									Clean( double _dVal ) {
			static const double dEps = 0.0000125;
			return std::fabs( _dVal ) >= dEps ? _dVal : 0.0;
		}

		/**
		 * The point filter function.
		 *
		 * \param _dT The value to filter.
		 * \return Returns the filtered value.
		 */
		static inline double									PointFilterFunc( double _dT ) {
			return (_dT >= -0.5 && _dT < 0.5) ? 1.0 : 0.0;
		}

		/**
		 * The tent/bilinear filter function.
		 *
		 * \param _dT The value to filter.
		 * \return Returns the filtered value.
		 */
		static inline double									BilinearFilterFunc( double _dT ) {
			_dT = std::fabs( _dT );
			if ( _dT < 1.0 ) { return 1.0 - _dT; }
			return 0.0;
		}

		/**
		 * Quadratic filter function.
		 *
		 * \param _dT The value to filter.
		 * \return Returns the filtered value.
		 */
		static inline double									QuadraticInterpFilterFunc( double _dT ) { return QuadraticFilterFunc( _dT, 1.0 ); }

		/**
		 * The Kaiser filter function.
		 *
		 * \param _dT The value to filter.
		 * \return Returns the filtered value.
		 */
		static inline double									KaiserFilterFunc( double _dT ) {
			_dT = std::fabs( _dT );
			if ( _dT < 3.0 ) {
				static const float fAtt = 40.0;
				static const double dAlpha = std::exp( std::log( 0.58417 * (fAtt - 20.96) ) * 0.4 ) + 0.07886 * (fAtt - 20.96);
				return static_cast<float>(Clean( SinC( _dT ) * KaiserHelper( dAlpha, 3.0, _dT ) ));
			}
			return 0.0;
		}

		/**
		 * The Lanczos filter function with 2 samples.
		 *
		 * \param _dT The value to filter.
		 * \return Returns the filtered value.
		 */
		static inline double									Lanczos2FilterFunc( double _dT ) {
			_dT = std::fabs( _dT );
			if ( _dT <= 2.0 ) {
				return Clean( SinC( _dT ) * SinC( _dT / 2.0 ) );
			}
			return 0.0;
		}

		/**
		 * The Lanczos filter function with 3 samples.
		 *
		 * \param _dT The value to filter.
		 * \return Returns the filtered value.
		 */
		static inline double									Lanczos3FilterFunc( double _dT ) {
			_dT = std::fabs( _dT );
			if ( _dT <= 3.0 ) {
				return Clean( SinC( _dT ) * SinC( _dT / 3.0 ) );
			}
			return 0.0;
		}

		/**
		 * The Lanczos filter function with 6 samples.
		 *
		 * \param _dT The value to filter.
		 * \return Returns the filtered value.
		 */
		static inline double									Lanczos6FilterFunc( double _dT ) {
			_dT = std::fabs( _dT );
			if ( _dT <= 6.0 ) {
				return Clean( SinC( _dT ) * SinC( _dT / 6.0 ) );
			}
			return 0.0;
		}

		/**
		 * The Lanczos filter function with 8 samples.
		 *
		 * \param _dT The value to filter.
		 * \return Returns the filtered value.
		 */
		static inline double									Lanczos8FilterFunc( double _dT ) {
			_dT = std::fabs( _dT );
			if ( _dT <= 8.0 ) {
				return Clean( SinC( _dT ) * SinC( _dT / 8.0 ) );
			}
			return 0.0;
		}

		/**
		 * The Lanczos filter function with 12 samples.
		 *
		 * \param _dT The value to filter.
		 * \return Returns the filtered value.
		 */
		static inline double									Lanczos12FilterFunc( double _dT ) {
			_dT = std::fabs( _dT );
			if ( _dT <= 12.0 ) {
				return Clean( SinC( _dT ) * SinC( _dT / 12.0 ) );
			}
			return 0.0;
		}

		/**
		 * The Lanczos filter function with 64 samples.
		 *
		 * \param _dT The value to filter.
		 * \return Returns the filtered value.
		 */
		static inline double									Lanczos64FilterFunc( double _dT ) {
			_dT = std::fabs( _dT );
			if ( _dT < 64.0 ) {
				return static_cast<float>(Clean( SinC( _dT ) * SinC( _dT / 64.0 ) ));
			}
			return 0.0;
		}

		/**
		 * The Mitchell filter function.
		 *
		 * \param _dT The value to filter.
		 * \return Returns the filtered value.
		 */
		static inline double									MitchellFilterFunc( double _dT ) {
			return MitchellFilterHelper( _dT, 1.0 / 3.0, 1.0 / 3.0 );
		}

		/**
		 * The Catmull-Rom filter function.
		 *
		 * \param _dT The value to filter.
		 * \return Returns the filtered value.
		 */
		static inline double									CatmullRomFilterFunc( double _dT ) {
			return MitchellFilterHelper( _dT, 0.0, 0.5 );
		}

		/**
		 * The B-Spline filter function.
		 *
		 * \param _dT The value to filter.
		 * \return Returns the filtered value.
		 */
		static inline double									BSplineFilterFunc( double _dT ) {
			return MitchellFilterHelper( _dT, 1.0, 0.0 );
		}

		/**
		 * The Blackman filter function.
		 *
		 * \param _dT The value to filter.
		 * \return Returns the filtered value.
		 */
		static inline double									BlackmanFilterFunc( double _dT ) {
			if ( _dT < 0.0 ) { _dT = -_dT; }
			if ( _dT < 3.0 ) {
				return Clean( SinC( _dT ) * BlackmanWindow( _dT / 3.0 ) );
			}
			return 0.0;
		}

		/**
		 * The Gaussian filter function.
		 *
		 * \param _dT The value to filter.
		 * \return Returns the filtered value.
		 */
		static inline double									GaussianFilterFunc( double _dT ) {
			if ( _dT < 0.0 ) { _dT = -_dT; }
			if ( _dT < 1.25 ) {
				return Clean( std::exp( -2.0 * _dT * _dT ) * std::sqrt( 2.0 / std::numbers::pi ) * BlackmanWindow( _dT / 1.25 ) );
			}
			return 0.0;
		}

		/**
		 * The Bell filter function.
		 *
		 * \param _dT The value to filter.
		 * \return Returns the filtered value.
		 */
		static inline double									BellFilterFunc( double _dT ) {
			if ( _dT < 0.0 ) { _dT = -_dT; }
			if ( _dT < 0.5 ) {
				return 0.75 - _dT * _dT;
			}
			if ( _dT < 1.5 ) {
				_dT = _dT - 1.5;
				return 0.5 * _dT * _dT;
			}
			return 0.0;
		}

	protected :
		// == Types.
		/** Contribution bounds. */
		typedef struct SL2_CONTRIB_BOUNDS {
			/** The center. */
			double												dCenter;

			/** Left. */
			int32_t												i32Left;

			/** Right. */
			int32_t												i32Right;
		} * LPSL2_CONTRIB_BOUNDS, * const LPCSL2_CONTRIB_BOUNDS;


		// == Members.
		/** Our array of contributions. */
		std::vector<SL2_CONTRIBUTIONS>							m_cContribs;
		/** Buffer for convolution. */
		std::vector<double, AlignmentAllocator<double, 64>>		m_dBuffer;


		// == Functions.
		/**
		 * Convolvinate.
		 *
		 * \param _pdWeights The convolution weights.
		 * \param _pdTexels The texels to be convolved.
		 * \param _sTotal The total values to which pdTexels and _pdWeights point.
		 * \return Returns the summed weights * texels.
		 **/
		static double											ConvolveAligned( const double * _pdWeights, const double * _pdTexels, size_t _sTotal );

		/**
		 * Convolvinate using unaligned reads.
		 *
		 * \param _pdWeights The convolution weights.
		 * \param _pdTexels The texels to be convolved.
		 * \param _sTotal The total values to which pdTexels and _pdWeights point.
		 * \return Returns the summed weights * texels.
		 **/
		static double											ConvolveUnaligned( const double * _pdWeights, const double * _pdTexels, size_t _sTotal );

	};

}	// namespace sl2

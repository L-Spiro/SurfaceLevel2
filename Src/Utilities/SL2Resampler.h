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


		// == Enumerations.
		/** Filter functions. */
		enum SL2_FILTER_FUNCS : size_t {
			SL2_FF_POINT,
			SL2_FF_LINEAR,
			SL2_FF_QUADRATICSHARP,
			SL2_FF_QUADRATIC,
			SL2_FF_QUADRATICAPPROX,
			SL2_FF_QUADRATICMIX,
			SL2_FF_KAISER,
			SL2_FF_LANCZOS2,
			SL2_FF_LANCZOS3,
			SL2_FF_LANCZOS4,
			SL2_FF_LANCZOS6,
			SL2_FF_LANCZOS8,
			SL2_FF_LANCZOS12,
			SL2_FF_LANCZOS64,
			SL2_FF_MITCHELL,
			SL2_FF_CATMULLROM,
			SL2_FF_BSPLINE,
			SL2_FF_CARDINALSPLINEUNIFORM,
			SL2_FF_HERMITE,
			SL2_FF_HAMMING,
			SL2_FF_HANNING,
			SL2_FF_BLACKMAN,
			SL2_FF_GAUSSIANSHARP,
			SL2_FF_GAUSSIAN,
			SL2_FF_BELL,
		};


		// == Types.
		/** Contributions (weights and texel indices). */
		struct SL2_CONTRIBUTIONS {
			std::vector<double, CAlignmentAllocator<double, 64>>dContributions;
			std::vector<int32_t>								i32Indices;
			bool												bInsideBounds;
		};

		typedef double (*										PfFilterFunc)( double );

		/** Filter information. */
		typedef struct SL2_FILTER {
			/** The filter function to use. */
			PfFilterFunc										pfFunc = BilinearFilterFunc;

			/** Filter helper value. */
			double												dfSupport = 1.0;
		} * LPSL2_FILTER, * const LPCSL2_FILTER;

		/** Parameters for resampling. */
		struct SL2_RESAMPLE {
			double												dBorderColor[4] = { 0.0, 0.0, 0.0, 1.0 };
			float												fFilterScale = 1.0;
			uint32_t											ui32W = 0;
			uint32_t											ui32H = 0;
			uint32_t											ui32D = 0;
			uint32_t											ui32NewW = 0;
			uint32_t											ui32NewH = 0;
			uint32_t											ui32NewD = 0;
			SL2_TEXTURE_ADDRESSING								taColorW = SL2_TA_NULL_BORDER;
			SL2_TEXTURE_ADDRESSING								taColorH = SL2_TA_NULL_BORDER;
			SL2_TEXTURE_ADDRESSING								taColorD = SL2_TA_NULL_BORDER;
			SL2_TEXTURE_ADDRESSING								taAlphaW = SL2_TA_NULL_BORDER;
			SL2_TEXTURE_ADDRESSING								taAlphaH = SL2_TA_NULL_BORDER;
			SL2_TEXTURE_ADDRESSING								taAlphaD = SL2_TA_NULL_BORDER;
			SL2_FILTER											fFilterW;
			SL2_FILTER											fFilterH;
			SL2_FILTER											fFilterD;
			SL2_FILTER											fAlphaFilterW;
			SL2_FILTER											fAlphaFilterH;
			SL2_FILTER											fAlphaFilterD;
			bool												bAlpha = true;
		};


		// == Functions.
		/**
		 * Resamples an image.
		 * 
		 * \param _pdIn The input buffer.
		 * \param _vOut The output buffer.
		 * \param _pParms Image/resampling parameters.
		 * \return Returns true if all allocations succeed.
		 **/
		bool													Resample( const double * _pdIn, double * _pdOut, const SL2_RESAMPLE &_pParms );

		/**
		 * A resample specialized for a 2-D image with a single channel (typically U or V).
		 * 
		 * \param _pdIn The input buffer.
		 * \param _vOut The output buffer.
		 * \param _pParms Image/resampling parameters.
		 * \param _sOutputStride The output stride in channels (IE RGB = 3, RGBA = 4).
		 * \return Returns true if all allocations succeed.
		 **/
		bool													Resample_1Channel_2d( const double * _pdIn, double * _pdOut, const SL2_RESAMPLE &_pParms, size_t _sOutputStride );

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
		 * Standard sinc() function.
		 * 
		 * \param _dX The operand.
		 * \return Returns sin(x) / x.
		 **/
		static inline double									Sinc( double _dX ) {
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
			constexpr double dEspiRatio = 1.0e-16;
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
			// These exact values place zeros at the third and fourth sidelobes, but result in a discontinuity at the edges and a 6 dB/oct fall-off.
			//constexpr double dA0 = 7938.0 / 18608.0;	// 0.42659071367153911236158592146239243447780609130859375
			//constexpr double dA1 = 9240.0 / 18608.0;	// 0.4965606190885640813803547644056379795074462890625
			//constexpr double dA2 = 1430.0 / 18608.0;	// 0.07684866723989682013584712194642634131014347076416015625
			
			// The truncated coefficients do not null the sidelobes as well, but have an improved 18 dB/oct fall-off.
			constexpr double dA = 0.16;
			constexpr double dA0 = (1.0 - dA) / 2.0;	// 0.419999999999999984456877655247808434069156646728515625
			constexpr double dA1 = 1.0 / 2.0;			// 0.5
			constexpr double dA2 = dA / 2.0;			// 0.08000000000000000166533453693773481063544750213623046875
			return dA0 + dA1 * std::cos( std::numbers::pi * _dX ) + dA2 * std::cos( 2.0 * std::numbers::pi * _dX );
		}

		/**
		 * Quadratic filter function.
		 *
		 * \param _dT The value to filter.
		 * \param _dB Helper value.
		 * \return Returns the filtered value.
		 */
		static inline double									QuadraticSharpFilterFunc( double _dT, double _dB ) {
			_dT = std::fabs( _dT );
			if ( _dT < 1.0 ) {
				double dTt = _dT * _dT;
				return _dT < 0.5 ? (-2.0 * _dB) * dTt + 0.5 * (_dB + 1.0) :
					(_dB * dTt) + (-2.0 * _dB - 0.5) * _dT + (3.0 / 4.0) * (_dB + 1.0);
			}
			return 0.0;
		}

		/**
		 * Quadratic filter function.
		 *
		 * \param _dT The value to filter.
		 * \param _dB Helper value.
		 * \return Returns the filtered value.
		 */
		static inline double									QuadraticFilterFunc2( double _dT, double _dB ) {
			_dT = std::fabs( _dT );
			constexpr double dThresh = 1.5;
			if ( _dT < dThresh ) {
				double dTt = _dT * _dT;
				if ( _dT < 0.5 ) {
					return (-2.0 * _dB) * dTt + 0.5 * (_dB + 1.0);
				}
				else {
					return (_dB * dTt) + (-2.0 * _dB - 0.5) * _dT + (3.0 / 4.0) * (_dB + 1.0);
				}
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
			constexpr double dEps = 0.0000125;
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
		static inline double									QuadraticSharpFilterFunc( double _dT ) { return QuadraticSharpFilterFunc( _dT, 1.0 ); }

		/**
		 * Quadratic filter function.
		 *
		 * \param _dT The value to filter.
		 * \return Returns the filtered value.
		 */
		static inline double									QuadraticInterpolFilterFunc( double _dT ) { return QuadraticFilterFunc2( _dT, 1.0 ); }

		/**
		 * Quadratic filter function.
		 *
		 * \param _dT The value to filter.
		 * \return Returns the filtered value.
		 */
		static inline double									QuadraticApproxFilterFunc( double _dT ) { return QuadraticFilterFunc2( _dT, 0.5 ); }

		/**
		 * Quadratic filter function.
		 *
		 * \param _dT The value to filter.
		 * \return Returns the filtered value.
		 */
		static inline double									QuadraticMixFilterFunc( double _dT ) { return QuadraticFilterFunc2( _dT, 0.8 ); }

		/**
		 * The Kaiser filter function.
		 *
		 * \param _dT The value to filter.
		 * \return Returns the filtered value.
		 */
		static inline double									KaiserFilterFunc( double _dT ) {
			_dT = std::fabs( _dT );
			if ( _dT < 3.0 ) {
				static const double dAtt = 40.0;
				static const double dAlpha = std::exp( std::log( 0.58417 * (dAtt - 20.96) ) * 0.4 ) + 0.07886 * (dAtt - 20.96);
				return static_cast<float>(Clean( Sinc( _dT ) * KaiserHelper( dAlpha, 3.0, _dT ) ));
			}
			return 0.0;
		}

		/**
		 * The Lanczos filter function with X samples.
		 *
		 * \param _dT The value to filter.
		 * \return Returns the filtered value.
		 */
		template <unsigned _uX>
		static inline double									LanczosXFilterFunc( double _dT ) {
			_dT = std::fabs( _dT );
			if ( _dT <= double( _uX ) ) {
				return Clean( Sinc( _dT ) * Sinc( _dT / double( _uX ) ) );
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
		 * The Cardinal Spline Uniform filter function.
		 *
		 * \param _dT The value to filter.
		 * \return Returns the filtered value.
		 */
		static inline double									CardinalSplineUniformFilterFunc( double _dT ) {
			return MitchellFilterHelper( _dT, 0.0, 1.0 );
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
				return Clean( Sinc( _dT ) * BlackmanWindow( _dT / 3.0 ) );
			}
			return 0.0;
		}

		/**
		 * The Gaussian filter function.
		 *
		 * \param _dT The value to filter.
		 * \return Returns the filtered value.
		 */
		static inline double									GaussianSharpFilterFunc( double _dT ) {
			if ( _dT < 0.0 ) { _dT = -_dT; }
			if ( _dT < 1.25 ) {
				return Clean( std::exp( -2.0 * _dT * _dT ) * std::sqrt( 2.0 / std::numbers::pi ) * BlackmanWindow( _dT / 1.25 ) );
			}
			return 0.0;
		}

		/**
		 * The Gaussian Sharp filter function.
		 *
		 * \param _dT The value to filter.
		 * \return Returns the filtered value.
		 */
		static inline double									GaussianFilterFunc( double _dT ) {
			if ( _dT < 0.0 ) { _dT = -_dT; }
			if ( _dT < 1.25 ) {
				const double dSigma = 0.52;
				return Clean( std::exp( -(_dT * _dT) / (2.0 * dSigma * dSigma) ) * (1.0 / (dSigma * std::sqrt( 2.0 * std::numbers::pi ))) * BlackmanWindow( _dT / 1.25 ) );
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

		/**
		 * The Hermite filter function.
		 *
		 * \param _dT The value to filter.
		 * \return Returns the filtered value.
		 */
		static inline double									HermiteFilterFunc( double _dT ) {
			if ( _dT < 0.0 ) { _dT = -_dT; }
			if ( _dT < 1.0 ) {
				return (2.0 * _dT - 3.0) * _dT * _dT + 1.0;
			}
			return 0.0;
		}

		/**
		 * The Hamming filter function.
		 *
		 * \param _dT The value to filter.
		 * \return Returns the filtered value.
		 */
		static inline double									HammingFilterFunc( double _dT ) {
			if ( _dT < 0.0 ) { _dT = -_dT; }
			if ( _dT < 1.0 ) {
				return 0.54 + 0.46 * std::cos( 2.0 * std::numbers::pi * _dT );
			}
			return 0.0;
		}

		/**
		 * The Hanning filter function.
		 *
		 * \param _dT The value to filter.
		 * \return Returns the filtered value.
		 */
		static inline double									HanningFilterFunc( double _dT ) {
			if ( _dT < 0.0 ) { _dT = -_dT; }
			if ( _dT < 1.0 ) {
				return 0.5 + 0.5 * std::cos( 2.0 * std::numbers::pi * _dT );
			}
			return 0.0;
		}


		// == Members.
		/** Filter parameters. */
		static SL2_FILTER										m_fFilter[];

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
		std::vector<double, CAlignmentAllocator<double, 64>>	m_dBuffer;


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

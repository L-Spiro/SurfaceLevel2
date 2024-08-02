/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Loading and working with ICC profiles.
 */


#pragma once

#include "SL2IccDefs.h"
#include "../../OS/SL2Os.h"

#include <cstdint>
#include <vector>


namespace sl2 {

	/**
	 * Class CIcc
	 * \brief Loading and working with ICC profiles.
	 *
	 * Description: Loading and working with ICC profiles.
	 */
	class CIcc {
	public :
		CIcc() {}
		~CIcc() {}


		// == Types.
		/** A parametric equation. */
		struct SL2_PARA {
			double												dParms[7];
		};

		/** A curve equation. */
		struct SL2_CURV {
			std::vector<double>									vTable;
		};

		/** Function prototype (X -> Linear). */
		typedef double (*										PfX_to_Linear)( double _dIn, const void * _pvParm );

		/** Function prototype (Linear -> X). */
		typedef double (*										PfLinear_to_X)( double _dIn, const void * _pvParm );

		/** The structure for holding the transfer-function data. */
		struct SL2_TRANSFER_FUNC {
			PfX_to_Linear										pfXtoLinear = PassThrough;						/**< X-to-linear function. */
			PfLinear_to_X										pfLinearToX = PassThrough;						/**< Linear-to-X function. */
			SL2_PARA											pPara;											/**< Parametric parameters. */
			SL2_CURV											cCurv;											/**< Curve parameters. */
			void *												pvParm = nullptr;								/**< Points to either pPara or cCurv. */


			// == Operators.
			/**
			 * Safely copies from another object into this one.
			 * 
			 * \param _tfOther The object to copy.
			 * \return Returns a reference to this object following the copy.
			 **/
			SL2_TRANSFER_FUNC &									operator = ( const SL2_TRANSFER_FUNC &_tfOther ) {
				if ( &_tfOther != this ) {
					pfXtoLinear = _tfOther.pfLinearToX;
					pfLinearToX = _tfOther.pfLinearToX;
					if ( _tfOther.pvParm == &_tfOther.pPara ) {
						pPara = _tfOther.pPara;
						pvParm = &pPara;
					}
					else {
						cCurv = _tfOther.cCurv;
						pvParm = &cCurv;
					}
				}
				return (*this);
			}
		};


		// == Functions.
		/**
		 * Gets the offset and size of the given tag.
		 *
		 * \param _pui8Profile The ICC profile.
		 * \param _sProfileSize The size of the ICC profile.
		 * \param _ui32Sig The tag whose data is to be found.
		 * \param _sSize Holds teh returned size of the curve data or 0 upon failure.
		 * \return Returns the offset of the curve data or 0 upon failure.
		 **/
		static size_t											GetTagDataOffset( const uint8_t * _pui8Profile, size_t _sProfileSize,
			uint32_t _ui32Sig, size_t &_sSize );

		/**
		 * Fills out a SL2_TRANSFER_FUNC structure given a curv/para tag.
		 * 
		 * \param _tfFunc The structure to fill out.
		 * \param _pui8Data The parametrc/curve data.
		 * \param _sSize Size of the data.
		 * \return Returns true if the curve was recognized and the structure could be filled out.
		 **/
		static bool												FillOutTransferFunc( SL2_TRANSFER_FUNC &_tfFunc, const uint8_t * _pui8Data, size_t _sSize );

		/**
		 * A pass-through handler.
		 * 
		 * \param _dIn The value to convert.
		 * \param _pvParm Not used.
		 * \return Returns _dIn.
		 **/
		static double											PassThrough( double _dIn, const void * /*_pvParm*/ ) { return _dIn; }

		/**
		 * A 1-length "curve" handler.
		 * 
		 * \param _dIn The value to convert.
		 * \param _pvParm Associated structure data (SL2_CURV).
		 * \return Returns the linear value of the _dIn.
		 **/
		static double											Len1_Curve_To_Linear( double _dIn, const void * _pvParm ) {
			const SL2_CURV * pcCurv = reinterpret_cast<const SL2_CURV *>(_pvParm);
			return pcCurv->vTable[0] * _dIn;
		}

		/**
		 * A 1-length "curve" handler.
		 * 
		 * \param _dIn The value to convert.
		 * \param _pvParm Associated structure data (SL2_CURV).
		 * \return Returns the colorspace value of the _dIn.
		 **/
		static double											Len1_Linear_To_Curve( double _dIn, const void * _pvParm ) {
			const SL2_CURV * pcCurv = reinterpret_cast<const SL2_CURV *>(_pvParm);
			return (1.0 / pcCurv->vTable[0]) * _dIn;
		}

		/**
		 * A type-0 "para" handler.
		 * 
		 * \param _dIn The value to convert.
		 * \param _pvParm Associated structure data (SL2_PARA).
		 * \return Returns the linear value of the _dIn.
		 **/
		static double											Type0_Para_To_Linear( double _dIn, const void * _pvParm );

		/**
		 * A type-0 "para" handler.
		 * 
		 * \param _dIn The value to convert.
		 * \param _pvParm Associated structure data (SL2_PARA).
		 * \return Returns the adjusted value of the _dIn.
		 **/
		static double											Type0_Para_To_ColorSpace( double _dIn, const void * _pvParm );

		/**
		 * A type-3 "para" handler.
		 * 
		 * \param _dIn The value to convert.
		 * \param _pvParm Associated structure data (SL2_PARA).
		 * \return Returns the linear value of the _dIn.
		 **/
		static double											Type3_Para_To_Linear( double _dIn, const void * _pvParm );

		/**
		 * A type-3 "para" handler.
		 * 
		 * \param _dIn The value to convert.
		 * \param _pvParm Associated structure data (SL2_PARA).
		 * \return Returns the adjusted value of the _dIn.
		 **/
		static double											Type3_Para_To_ColorSpace( double _dIn, const void * _pvParm );

	};

}	// namespace sl2

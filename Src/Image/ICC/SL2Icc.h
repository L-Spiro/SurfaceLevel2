/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Loading and working with ICC profiles.
 */


#pragma once

#include "SL2IccDefs.h"
#include "../SL2Formats.h"
#include "../../OS/SL2Os.h"
#include "../Little-CMS/include/lcms2.h"

#include <cmath>
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
			std::vector<double>									vInvTable;
		};

		/** Little CMS cmsHPROFILE wrapper. */
		struct SL2_CMS_PROFILE {
			SL2_CMS_PROFILE() {}
			SL2_CMS_PROFILE( const uint8_t * _pui8Data, size_t _sSize ) :
				hProfile( ::cmsOpenProfileFromMem( _pui8Data, static_cast<cmsUInt32Number>(_sSize) ) ) {}
			SL2_CMS_PROFILE( cmsHPROFILE _hProfile, bool _bFree = false ) :
				hProfile( _hProfile ) {
			}
			~SL2_CMS_PROFILE() {
				Reset();
			}


			// == Functions.
			/**
			 * Resets the object.
			 **/
			void												Reset() {
				if ( hProfile ) {
					::cmsCloseProfile( hProfile );
					hProfile = nullptr;
				}
			}

			/**
			 * Sets the transform.
			 * 
			 * \param _hTransform The transform to set.
			 * \return Returns a reference to this object.
			 **/
			SL2_CMS_PROFILE &									Set( cmsHPROFILE _hProfile ) {
				Reset();
				hProfile = _hProfile;
				return (*this);
			}


			// == Members.
			cmsHPROFILE											hProfile = nullptr;
		};

		/** Little CMS cmsHTRANSFORM wrapper. */
		struct SL2_CMS_TRANSFORM {
			SL2_CMS_TRANSFORM() {}
			SL2_CMS_TRANSFORM( cmsHTRANSFORM _hProfile ) :
				hTransform( _hProfile ) {
			}
			~SL2_CMS_TRANSFORM() {
				Reset();
			}


			// == Functions.
			/**
			 * Resets the object.
			 **/
			void												Reset() {
				if ( hTransform ) {
					::cmsDeleteTransform( hTransform );
					hTransform = nullptr;
				}
			}

			/**
			 * Sets the transform.
			 * 
			 * \param _hTransform The transform to set.
			 * \return Returns a reference to this object.
			 **/
			SL2_CMS_TRANSFORM &									Set( cmsHTRANSFORM _hTransform ) {
				Reset();
				hTransform = _hTransform;
				return (*this);
			}


			// == Members.
			cmsHTRANSFORM										hTransform = nullptr;
		};

		/**  Little CMS cmsToneCurve wrapper. */
		struct SL2_CMS_TONECURVE {
			SL2_CMS_TONECURVE() {}
			SL2_CMS_TONECURVE( cmsToneCurve * _htcToneCurve ) :
				tcCurve( _htcToneCurve ) {
			}
			~SL2_CMS_TONECURVE() {
				Reset();
			}


			// == Functions.
			/**
			 * Resets the object.
			 **/
			void												Reset() {
				if ( tcCurve ) {
					::cmsFreeToneCurve( tcCurve );
					tcCurve = nullptr;
				}
			}

			/**
			 * Sets the transform.
			 * 
			 * \param _hTransform The transform to set.
			 * \return Returns a reference to this object.
			 **/
			SL2_CMS_TONECURVE &									Set( cmsToneCurve * _hTransform ) {
				Reset();
				tcCurve = _hTransform;
				return (*this);
			}


			// == Members.
			cmsToneCurve *										tcCurve = nullptr;
		};

		/** Function prototype (X -> Linear/Linear -> X). */
		typedef double (*										PfTransferFunc)( double _dIn, const void * _pvParm );

		/** The structure for holding the transfer-function data. */
		struct SL2_TRANSFER_FUNC {
			PfTransferFunc										pfXtoLinear = PassThrough;						/**< X-to-linear function. */
			PfTransferFunc										pfLinearToX = PassThrough;						/**< Linear-to-X function. */
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
			return std::pow( _dIn, pcCurv->vTable[0] );
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
			return std::pow( _dIn, pcCurv->vInvTable[0] );
		}

		/**
		 * An X-length "curve" handler.
		 * 
		 * \param _dIn The value to convert.
		 * \param _pvParm Associated structure data (SL2_CURV).
		 * \return Returns the linear value of the _dIn.
		 **/
		static double											LenX_Curve_To_Linear( double _dIn, const void * _pvParm );

		/**
		 * An X-length "curve" handler.
		 * 
		 * \param _dIn The value to convert.
		 * \param _pvParm Associated structure data (SL2_CURV).
		 * \return Returns the colorspace value of the _dIn.
		 **/
		static double											LenX_Linear_To_Curve( double _dIn, const void * _pvParm );

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

		/**
		 * Finds the inverse of a LUT index.
		 *
		 * \param _pcCurv The curve of points.
		 * \param _dPoint The point to reverse look-up.
		 * \return Returns the inverse look-up value for a given point in a table.
		 **/
		static double											InverseLut( const SL2_CURV * _pcCurv, double _dPoint );

		/**
		 * Creates a colorspace profile using a predefined colorspace.
		 *
		 * \param _cContextID The context or nullptr.
		 * \param _cgcCurve The type of profile to create.
		 * \param _cpProfile The returned profile.
		 * \param _bIncludeCurves If true, tone curves are included.
		 * \return Returns true if all allocations succeed.
		 **/
		static bool												CreateProfile( cmsContext _cContextID, SL2_COLORSPACE_GAMMA_CURVES _cgcCurve, SL2_CMS_PROFILE &_cpProfile, bool _bIncludeCurves = false );

		/**
		 * Creates a basic CMYK profile.
		 * 
		 * \param _cpProfile Holds the created CMYK profile.
		 * \return Returns true if the operation succeeded.
		 **/
		static bool												CreateCmykProfile( SL2_CMS_PROFILE &_cpProfile );

		/**
		 * Creates an in-memory ICC file.
		 * 
		 * \param _pProfile The profile to save.
		 * \param _vFile The in-memory file after saving.
		 * \return Returns true if the file was saved to memory.
		 **/
		static bool												SaveProfileToMemory( const SL2_CMS_PROFILE &_pProfile, std::vector<uint8_t> &_vFile );

		/**
		 * Creates a linear version of the given in-memory ICC profile.
		 * 
		 * \param _vFile The in-memory ICC file.
		 * \param _pProfile The created profile.
		 * \return Returns true if the ICC file was loaded and set to linear.
		 **/
		static bool												CreateLinearProfile( std::vector<uint8_t> &_vFile, SL2_CMS_PROFILE &_pProfile );

		static cmsBool											SetTextTags(cmsHPROFILE hProfile, const wchar_t* Description);

	};

}	// namespace sl2

/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A resampler.
 */


#pragma once

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
		typedef std::vector<double>								SL2_CONTRIBUTIONS;


		// == Functions.
		/**
		 * Allocates and returns a new contribution list.
		 *
		 * \param _ui32SrcSize Size of the source.
		 * \param _ui32DstSize Size of the destination.
		 * \param _amAddressMode Texture addressing mode.
		 * \param _pfFilter Filter function.
		 * \param _fFilterSupport Filter support value.
		 * \param _fFilterScale Filter scale.
		 */
		/*void													CreateContribList( uint32_t _ui32SrcSize, uint32_t _ui32DstSize,
			LSI_ADDRESS_MODE _amAddressMode,
			PfFilterFunc _pfFilter, float _fFilterSupport, float _fFilterScale )*/


	protected :
		// == Members.
		/** Our array of contributions. */
		std::vector<SL2_CONTRIBUTIONS>							m_cContribs;

	};

}	// namespace sl2

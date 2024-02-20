/**
 * Copyright L. Spiro 2022
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A time-keeping class.  Used to keep the master clock running in real-time.
 */


#pragma once

#include "../OS/SL2Os.h"
#include <cstdint>

namespace sl2 {

	/**
	 * Class CBus
	 * \brief A time-keeping class.  Used to keep the master clock running in real-time.
	 *
	 * Description: A time-keeping class.  Used to keep the master clock running in real-time.
	 */
	class CClock {
	public :
		// == Various constructors.
		CClock();


		// == Functions.
		/**
		 * Gets the clock resolution.
		 *
		 * \return Returns the resolution.
		 */
		inline uint64_t							GetResolution() const;

		/**
		 * Gets the clock start tick.
		 *
		 * \return Returns the start tick.
		 */
		inline uint64_t							GetStartTick() const;

		/**
		 * Gets the current clock tick.
		 *
		 * \return Returns the current clock tick.
		 */
		uint64_t								GetRealTick() const;

		/**
		 * Sets the starting clock tick to GetRealTick().
		 */
		void									SetStartingTick();

	protected :
		// == Members.
		uint64_t								m_ui64Resolution;							/**< The resolution of the clock. */
		uint64_t								m_ui64StartTime;							/**< The starting clock time. */
	};


	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// DEFINITIONS
	// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	// == Functions.
	/**
	 * Gets the clock resolution.
	 *
	 * \return Returns the resolution.
	 */
	inline uint64_t CClock::GetResolution() const { return m_ui64Resolution; }

	/**
	 * Gets the clock start tick.
	 *
	 * \return Returns the start tick.
	 */
	inline uint64_t CClock::GetStartTick() const { return m_ui64StartTime; }

}	// namespace sl2

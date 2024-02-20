/**
 * Copyright L. Spiro 2022
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A time-keeping class.  Used to keep the master clock running in real-time.
 */


#include "SL2Clock.h"

namespace sl2 {

	// == Various constructors.
	CClock::CClock() {
#ifdef SL2_WINDOWS
		LARGE_INTEGER liTmp;
		::QueryPerformanceFrequency( &liTmp );
		m_ui64Resolution = liTmp.QuadPart;
#endif	// #ifdef SL2_WINDOWS

		SetStartingTick();
	}

	// == Functions.
	/**
	 * Gets the current clock tick.
	 *
	 * \return Returns the current clock tick.
	 */
	uint64_t CClock::GetRealTick() const {
#ifdef SL2_WINDOWS
		LARGE_INTEGER liTmp;
		::QueryPerformanceCounter( &liTmp );
		return liTmp.QuadPart;
#endif	// #ifdef SL2_WINDOWS
	}

	/**
	 * Sets the starting clock tick to GetRealTick().
	 */
	void CClock::SetStartingTick() {
		m_ui64StartTime = GetRealTick();
	}

}	// namespace sl2

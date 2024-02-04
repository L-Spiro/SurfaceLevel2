/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: Thread events, allowing signalling and waiting between threads.
 */

#include "SL2Events.h"


namespace sl2 {

	CEvent::CEvent() :
#ifdef SL2_WINDOWS
		m_hHandle( ::CreateEventW( NULL, FALSE, FALSE, NULL ) )
#else
#endif	// #ifdef SL2_WINDOWS
	{
	}
	CEvent::~CEvent() {
#ifdef SL2_WINDOWS
		::CloseHandle( m_hHandle );
		m_hHandle = NULL;
#else
#endif	// #ifdef SL2_WINDOWS
	}

	// == Functions.
	/**
	 * Waits for the event/signal.
	 */
	uint32_t CEvent::WaitForSignal() {
#ifdef SL2_WINDOWS
		return WaitForSignal( INFINITE );
#else
#endif	// #ifdef SL2_WINDOWS
	}

	/**
	 * Waits for the event/signal for a given number of milliseconds.
	 * 
	 * \param _ui32Milliseconds The time-out time in milliseconds.
	 **/
	uint32_t CEvent::WaitForSignal( uint32_t _ui32Milliseconds ) {
#ifdef SL2_WINDOWS
		return ::WaitForSingleObject( m_hHandle, _ui32Milliseconds );
#else
#endif	// #ifdef SL2_WINDOWS
	}

	/**
	 * Signals the event.
	 */
	void CEvent::Signal() {
#ifdef SL2_WINDOWS
		::SetEvent( m_hHandle );
#else
#endif	// #ifdef SL2_WINDOWS
	}

}	// namespace sl2

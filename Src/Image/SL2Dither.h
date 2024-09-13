/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: A palette and palette functionality.
 */

#pragma once


namespace sl2 {

	// == Enumerations.
	/** Dithers. */
	enum SL2_DITHER {
		SL2_D_FLOYD_STEINBERG,
		SL2_D_JARVIS_JUDICE_NINKE,
		SL2_D_STUCKI,
		SL2_D_BURKES,
		SL2_D_SIERRA,
		SL2_D_SIERRA_2,
		SL2_D_SIERRA_LITE,
		SL2_D_ATKINSON,
	};

}	// namespace sl2

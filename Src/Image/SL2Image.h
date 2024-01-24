/**
 * Copyright L. Spiro 2024
 *
 * Written by: Shawn (L. Spiro) Wilcoxen
 *
 * Description: An image is a collection of surfaces:
 *	1: Mipmaps
 *	2: Array
 *	3: 3D Slice
 *	4: Cube Face
 * 
 * This class is the primary workhorse of the tool.  Image conversion and manipulation takes place here.
 */

#pragma once


namespace sl2 {

	/**
	 * Class CImage
	 * \brief This class is the primary workhorse of the tool.
	 *
	 * Description: An image is a collection of surfaces:
	 *	1: Mipmaps
	 *	2: Array
	 *	3: 3D Slice
	 *	4: Cube Face
	 * 
	 * This class is the primary workhorse of the tool.  Image conversion and manipulation takes place here.
	 */
	class CImage {
	public :
		CImage();
		~CImage();
	};

}	// namespace sl2

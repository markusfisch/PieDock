/*
 *   O         ,-
 *  ° o    . -´  '     ,-
 *   °  .´        ` . ´,´
 *     ( °   ))     . (
 *      `-;_    . -´ `.`.
 *          `._'       ´
 *
 * Copyright (c) 2007-2010 Markus Fisch <mf@markusfisch.de>
 *
 * Licensed under the MIT license:
 * http://www.opensource.org/licenses/mit-license.php
 */
#ifndef _PieDock_PngSurface_
#define _PieDock_PngSurface_

#include "ArgbSurface.h"

#include <istream>
#include <string>
#include <png.h>

namespace PieDock
{
	/**
	 * A ARGB surface loaded from a PNG file
	 *
	 * @author Markus Fisch <mf@markusfisch.de>
	 */
	class PngSurface : public ArgbSurface
	{
		public:
			PngSurface( std::string );
			PngSurface( std::istream & );
			virtual ~PngSurface() {}
			void load( std::istream & );

		private:
			static void readPng( png_structp, png_bytep, png_size_t );
	};
}

#endif

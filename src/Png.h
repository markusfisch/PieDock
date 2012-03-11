/*
 *   O         ,-
 *  ° o    . -´  '     ,-
 *   °  .´        ` . ´,´
 *     ( °   ))     . (
 *      `-;_    . -´ `.`.
 *          `._'       ´
 *
 * Copyright (c) 2012 Markus Fisch <mf@markusfisch.de>
 *
 * Licensed under the MIT license:
 * http://www.opensource.org/licenses/mit-license.php
 */
#ifndef _PieDock_Png_
#define _PieDock_Png_

#include "ArgbSurface.h"

#include <istream>
#include <ostream>
#include <string>
#include <png.h>

namespace PieDock
{
	/**
	 * Load/Save a PNG file
	 */
	class Png
	{
		public:
			virtual ~Png() {}
			static ArgbSurface *load( const std::string & );
			static ArgbSurface *load( std::istream & );
			static void save( const std::string &, const ArgbSurface * );
			static void save( std::ostream &, const ArgbSurface * );

		protected:
			static void read( png_structp, png_bytep, png_size_t );
			static void write( png_structp, png_bytep, png_size_t );
			static void flush( png_structp );

		private:
			Png() {}
	};
}

#endif

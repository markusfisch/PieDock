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
#ifndef _PieDock_Cartouche_
#define _PieDock_Cartouche_

#include "ArgbSurface.h"

namespace PieDock
{
	/**
	 * A plain solid rectangle with rounded corners
	 *
	 * @author Markus Fisch <mf@markusfisch.de>
	 */
	class Cartouche : public ArgbSurface
	{
		public:
			Cartouche( int, int, int, unsigned int = 0xff000000 );
			virtual ~Cartouche() {}

		protected:
			struct Details
			{
				int top;
				int left;
				int bottom;
				int right;
				int bytesPerLine;
				int color;
				unsigned char *data;
			};

			virtual void drawRectangle( int, int, int, int, unsigned int );
			virtual void drawRoundedRectangle( int, int, int, int, int, 
				unsigned int );
			virtual void drawCurveSlices( Details &, int, int, unsigned char );
	};
}

#endif

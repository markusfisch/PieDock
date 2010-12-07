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
#ifndef _PieDock_ArgbSurface_
#define _PieDock_ArgbSurface_

#include "Surface.h"

namespace PieDock
{
	/**
	 * ARGB surface
	 *
	 * @author Markus Fisch <mf@markusfisch.de>
	 */
	class ArgbSurface : public Surface
	{
		public:
			ArgbSurface( int, int );
			virtual ~ArgbSurface() {}
	};
}

#endif

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
#ifndef _PieDock_Icon_
#define _PieDock_Icon_

#include "ArgbSurfaceSizeMap.h"

namespace PieDock
{
	/**
	 * Icon
	 *
	 * @author Markus Fisch <mf@markusfisch.de>
	 */
	class Icon : public ArgbSurfaceSizeMap
	{
		public:
			Icon( ArgbSurface &s ) :
				ArgbSurfaceSizeMap( s ) {}
			virtual ~Icon() {}
	};
}

#endif

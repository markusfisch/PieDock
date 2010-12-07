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
#ifndef _PieDock_Resampler_
#define _PieDock_Resampler_

#include "ArgbSurface.h"

namespace PieDock
{
	/**
	 * Resample some surface
	 *
	 * @author Markus Fisch <mf@markusfisch.de>
	 */
	class Resampler
	{
		public:
			virtual ~Resampler() {}
			static void resample( ArgbSurface &, ArgbSurface & );

		protected:
			static void biLinear( ArgbSurface &, ArgbSurface & );
			static void areaAveraging( ArgbSurface &, ArgbSurface & );

		private:
			Resampler() {}
	};
}

#endif

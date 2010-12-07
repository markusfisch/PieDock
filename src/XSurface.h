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
#ifndef _PieDock_XSurface_
#define _PieDock_XSurface_

#include "Surface.h"
#include "ArgbSurface.h"

#include <X11/Xlib.h>

namespace PieDock
{
	/**
	 * XSurface provides a Display-compatible surface
	 *
	 * @author Markus Fisch <mf@markusfisch.de>
	 */
	class XSurface : public Surface
	{
		public:
			XSurface( int, int, Display *, Visual *, int );
			virtual ~XSurface();
			inline XImage *getResource() const { return resource; }
			inline Visual *getVisual() const { return visual; }

		protected:
			Display *display;
			Visual *visual;
			int orginalDepth;
			XImage *resource;

			virtual int determineBitsPerPixel( int );
			virtual void allocateData();
			virtual void freeData();
	};
}

#endif

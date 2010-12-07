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
#ifndef _PieDock_Hotspot_
#define _PieDock_Hotspot_

#include <X11/Xlib.h>

namespace PieDock
{
	/**
	 * Query pointer
	 *
	 * @author Markus Fisch <mf@markusfisch.de>
	 */
	class Hotspot
	{
		public:
			Hotspot( Display *, Window = 0 );
			virtual ~Hotspot() {}
			inline const Window &getRoot() const { return root; }
			inline const Window &getChild() const { return child; }
			inline const int &getRootX() const { return rootX; }
			inline const int &getRootY() const { return rootY; }
			inline const int &getX() const { return childX; }
			inline const int &getY() const { return childY; }
			inline const unsigned int &getMask() const { return mask; }

		private:
			Window root;
			Window child;
			int rootX;
			int rootY;
			int childX;
			int childY;
			unsigned int mask;
	};
}

#endif

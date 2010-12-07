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
#ifndef _PieDock_MenuItems_
#define _PieDock_MenuItems_

#include "MenuItem.h"

#include <vector>

namespace PieDock
{
	/**
	 * Menu items
	 *
	 * @author Markus Fisch <mf@markusfisch.de>
	 */
	class MenuItems : public std::vector<MenuItem *>
	{
		public:
			MenuItems() : windows( false ) {}
			virtual ~MenuItems() {}
			inline const bool &includeWindows() const {
				return windows; }
			inline void setIncludeWindows( bool w ) { windows = w; }

		private:
			bool windows;
	};
}

#endif

/*
 *   O         ,-
 *  ° o    . -´  '     ,-
 *   °  .´        ` . ´,´
 *     ( °   ))     . (
 *      `-;_    . -´ `.`.
 *          `._'       ´
 *
 * Copyright (c) 2007-2011 Markus Fisch <mf@markusfisch.de>
 *
 * Licensed under the MIT license:
 * http://www.opensource.org/licenses/mit-license.php
 */
#ifndef _PieDock_MenuItems_
#define _PieDock_MenuItems_

#include "MenuItem.h"

#include <list>

namespace PieDock
{
	/**
	 * Menu items
	 *
	 * @author Markus Fisch <mf@markusfisch.de>
	 */
	class MenuItems : public std::list<MenuItem *>
	{
		public:
			MenuItems() : windows( false ), one( false ) {}
			virtual ~MenuItems() {}
			inline const bool &includeWindows() const {
				return windows; }
			inline void setIncludeWindows( bool w ) { windows = w; }
			inline const bool &oneIconPerWindow() const {
				return one; }
			inline void setOneIconPerWindow( bool o ) { one = o; }
			inline const bool &onlyFromActive() const {
				return only; }
			inline void setOnlyFromActive( bool o ) { only = o; }

		private:
			bool windows;
			bool one;
			bool only;
	};
}

#endif

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
#ifndef _PieDock_Utilities_
#define _PieDock_Utilities_

#include <X11/Xlib.h>

namespace PieDock
{
	/**
	 * Some independent utilities
	 *
	 * @author Markus Fisch <mf@markusfisch.de>
	 */
	class Utilities
	{
		public:
			enum Action
			{
				Close,
				Iconify,
				Maximize,
				Fullscreen,
				Lower,
				Raise,
				Shade,
				Stick
			};

			enum NetWmStates
			{
				StateRemove = 0,
				StateAdd,
				StateToggle				
			};

			Utilities();
			virtual ~Utilities();
			void listWindows();
			void execute( Action, Window );

		private:
			Display *display;
	};
}

#endif

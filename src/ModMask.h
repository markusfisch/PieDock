/*
 * Copyright (c) 2008 Tatiana Azundris <hacks@azundris.com>
 *
 * Licensed under the MIT license:
 * http://www.opensource.org/licenses/mit-license.php
 */
#ifndef _PieDock_ModMask_
#define _PieDock_ModMask_

#include <X11/Xlib.h>

namespace PieDock
{
	/**
	 * Mod mask
	 *
	 * @author Tatiana Azundris <hacks@azundris.com>
	 */
	class ModMask
	{
		public:
			ModMask( Display * );
			virtual ~ModMask();
			unsigned int getModMaskFor( const char * );

		protected:
			Display *display;

			struct XlatEntry
			{
				const char *name;
        		int index;
        		int mask;
			};

			struct ModKeyEntry
			{
				KeyCode code;
				const char *name;
				const XlatEntry *xlat;
				char *type;
			};

			int modKeyCount;
			ModKeyEntry *modKey;
	};
}

#endif

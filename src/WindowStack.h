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
#ifndef _PieDock_WindowStack_
#define _PieDock_WindowStack_

#include <X11/Xlib.h>

#include <vector>

namespace PieDock
{
	/**
	 * Controller for windows
	 *
	 * @author Markus Fisch <mf@markusfisch.de>
	 */
	class WindowStack
	{
		public:
			typedef struct
			{
				Window window;
				int left;
				int top;
				int right;
				int bottom;
				int workspace;
				int state;
			} WindowAttribute;

			typedef std::vector<WindowAttribute> WindowAttributes;

			WindowStack() : last( 0 ) {}
			virtual ~WindowStack() {}
			inline WindowAttributes &getWindowAttributes() {
				return windowAttributes; }
			inline const bool hasWindows() const {
				return windowAttributes.empty()^true; }
			inline void clearWindows() { windowAttributes.clear(); }
			void addWindow( Display *, Window );
			const Window getNextWindow();
			const Window getPreviousWindow();
			const bool isUnmapped();

		private:
			WindowAttributes windowAttributes;
			Window last;
	};
}

#endif

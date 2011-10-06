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
#include "WindowStack.h"
#include "WindowManager.h"

using namespace PieDock;

/**
 * Add a window to menu item
 *
 * @param d - display
 * @param w - window to add
 */
void WindowStack::addWindow( Display *d, Window w )
{
	XWindowAttributes xwa;

	XGetWindowAttributes( d, w, &xwa );

	WindowAttribute wa = {
		w,
		xwa.x,
		xwa.y,
		xwa.x+xwa.width,
		xwa.y+xwa.height,
		WindowManager::getWorkspace( d, w ),
		xwa.map_state };

	windowAttributes.push_back( wa );
}

/**
 * Return next window
 */
const Window WindowStack::getNextWindow()
{
	if( !hasWindows() )
		return 0;

	for( WindowAttributes::iterator i = windowAttributes.begin();
		i != windowAttributes.end();
		++i )
		if( (*i).window == last )
		{
			if( ++i == windowAttributes.end() )
				i = windowAttributes.begin();

			last = (*i).window;

			return last;
		}

	last = (*windowAttributes.begin()).window;

	return last;
}

/**
 * Return previous window
 */
const Window WindowStack::getPreviousWindow()
{
	if( !hasWindows() )
		return 0;

	for( WindowAttributes::iterator i = windowAttributes.begin();
		i != windowAttributes.end();
		++i )
		if( (*i).window == last )
		{
			if( i == windowAttributes.begin() )
				last = windowAttributes.back().window;
			else
				last = (*--i).window;

			return last;
		}

	last = windowAttributes.back().window;

	return last;
}

/**
 * Returns true if there's at least one window visible
 */
const bool WindowStack::isUnmapped()
{
	for( WindowAttributes::iterator i = windowAttributes.begin();
		i != windowAttributes.end();
		++i )
		if( (*i).state != IsUnmapped )
			return false;

	return true;
}

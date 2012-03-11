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
	WindowInfo wa = { w };

	XGetWindowAttributes( d, w, &wa.attributes );

	windowInfos.push_back( wa );
}

/**
 * Return next window
 */
const Window WindowStack::getNextWindow()
{
	if( !hasWindows() )
		return 0;

	for( WindowInfos::iterator i = windowInfos.begin();
		i != windowInfos.end();
		++i )
		if( (*i).window == last )
		{
			if( ++i == windowInfos.end() )
				i = windowInfos.begin();

			last = (*i).window;

			return last;
		}

	last = (*windowInfos.begin()).window;

	return last;
}

/**
 * Return previous window
 */
const Window WindowStack::getPreviousWindow()
{
	if( !hasWindows() )
		return 0;

	for( WindowInfos::iterator i = windowInfos.begin();
		i != windowInfos.end();
		++i )
		if( (*i).window == last )
		{
			if( i == windowInfos.begin() )
				last = windowInfos.back().window;
			else
				last = (*--i).window;

			return last;
		}

	last = windowInfos.back().window;

	return last;
}

/**
 * Returns true if there's at least one window visible
 */
const bool WindowStack::isUnmapped()
{
	for( WindowInfos::iterator i = windowInfos.begin();
		i != windowInfos.end();
		++i )
		if( (*i).attributes.map_state != IsUnmapped )
			return false;

	return true;
}

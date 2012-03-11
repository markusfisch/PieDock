/*
 *   O         ,-
 *  ° o    . -´  '     ,-
 *   °  .´        ` . ´,´
 *     ( °   ))     . (
 *      `-;_    . -´ `.`.
 *          `._'       ´
 *
 * Copyright (c) 2007-2012 Markus Fisch <mf@markusfisch.de>
 *
 * Licensed under the MIT license:
 * http://www.opensource.org/licenses/mit-license.php
 */
#include "MenuItem.h"

using namespace PieDock;

/**
 * Create a new menu item without pre-defined icon
 *
 * @param t - title
 * @param c - command
 * @param s - true if sticky (optional)
 */
MenuItem::MenuItem( std::string t, std::string c, bool s ) :
	title( t ),
	command( c ),
	icon( 0 ),
	sticky( s )
{
}

/**
 * Create menu item from icon
 *
 * @param i - icon
 */
MenuItem::MenuItem( Icon *i ) :
	icon( i ),
	sticky( false )
{
}

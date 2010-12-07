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
#include "Hotspot.h"

using namespace PieDock;

/**
 * Get current (mouse) cursor position
 *
 * @param d - display
 * @param w - relative to which window, if not given the root window (optional)
 */
Hotspot::Hotspot( Display *d, Window w )
{
	XQueryPointer(
		d,
		(w ? w : DefaultRootWindow( d )),
		&root,
		&child,
		&rootX,
		&rootY,
		&childX,
		&childY,
		&mask );
}

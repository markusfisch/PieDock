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
#include "ArgbSurface.h"

using namespace PieDock;

/**
 * Create a ARGB surface
 *
 * @param w - width of surface in pixels
 * @param h - height of surface in pixels
 */
ArgbSurface::ArgbSurface( int w, int h ) :
	Surface()
{
	calculateSize( w, h, ARGB );
	allocateData();
}

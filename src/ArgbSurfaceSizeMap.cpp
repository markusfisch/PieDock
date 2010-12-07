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
#include "ArgbSurfaceSizeMap.h"
#include "Resampler.h"

using namespace PieDock;

/**
 * Initialize object
 *
 * @param s - some ARGB surface
 */
ArgbSurfaceSizeMap::ArgbSurfaceSizeMap( ArgbSurface &s ) :
	surface( s )
{
}

/**
 * Clean up
 */
ArgbSurfaceSizeMap::~ArgbSurfaceSizeMap()
{
	clear();
}

/**
 * Return a sized version of the icon image
 *
 * @param width - width of surface in pixels
 * @param height - height of surface in pixels
 */
const ArgbSurface *ArgbSurfaceSizeMap::getSurface( int width, int height )
{
	if( width == surface.getWidth() &&
		height == surface.getHeight() )
		return &surface;

	int format = (width<<16)+height;
	SurfaceMap::iterator i;

	if( (i = surfaceMap.find( format )) == surfaceMap.end() )
	{
		ArgbSurface *s = new ArgbSurface( width, height );

		Resampler::resample( *s, surface );
		surfaceMap.insert( std::make_pair( format, s ) );

		return s;
	}

	return (*i).second;
}

/**
 * Reset surface
 *
 * @param s - some ARGB surface
 */
void ArgbSurfaceSizeMap::setSurface( ArgbSurface &s )
{
	clear();
	surface = s;
}

/**
 * Clear map
 */
void ArgbSurfaceSizeMap::clear()
{
	for( SurfaceMap::iterator i = surfaceMap.begin();
		i != surfaceMap.end();
		i++ )
		delete (*i).second;

	surfaceMap.clear();
}

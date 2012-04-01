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
#include "Surface.h"

#include <string.h>

#include <stdexcept>

using namespace PieDock;

/**
 * Copy constructor
 *
 * @param s - some surface
 */
Surface::Surface( const Surface &s ) :
	// because data will be deleted in operator=()
	data( 0 )
{
	*this = s;
}

/**
 * Free resources
 */
Surface::~Surface()
{
	freeData();
}

/**
 * Copy surface
 *
 * @param s - some surface
 */
Surface &Surface::operator=( const Surface &s )
{
	freeData();

	width = s.getWidth();
	height = s.getHeight();
	depth = s.getDepth();
	bytesPerPixel = s.getBytesPerPixel();
	bytesPerLine = s.getBytesPerLine();
	padding = s.getPadding();
	size = s.getSize();

	allocateData();

	memcpy( data, s.getData(), size );

	return *this;
}

/**
 * Initialize virtual surface
 */
Surface::Surface() : 
	data( 0 ),
	width( 0 ),
	height( 0 ),
	depth( 0 ),
	bytesPerPixel( 0 ),
	bytesPerLine( 0 ),
	padding( 0 ),
	size( 0 )
{
}

/**
 * Calculate size in bytes of this surface
 *
 * @param w - width of surface in pixels
 * @param h - height of surface in pixels
 * @param d - color depth, bits per pixel (optional)
 */
void Surface::calculateSize( int w, int h, int d )
{
	width = w;
	height = h;
	depth = d;
	bytesPerPixel = depth>>3;

	// calculate bytes per line
	{
		int bitsPerLine = width*depth;

		// ceil to full byte if bits per pixel is fewer than a byte
		// or divide by 8 if there are at last 8 bits per pixel
		if( depth < 8 )
			bytesPerLine = (bitsPerLine+7) & ~7;
		else
			bytesPerLine = bitsPerLine>>3;
	}

	// pad every image line to a multiple of 4 bytes
	padding = bytesPerLine;
	bytesPerLine = (bytesPerLine+3) & ~3;
	padding = bytesPerLine-padding;

	size = height*bytesPerLine;
}

/**
 * Allocate data for surface
 */
void Surface::allocateData()
{
	if( !(data = new unsigned char[size]) )
		throw std::runtime_error( "cannot allocate surface memory" );
}

/**
 * Free data of surface
 */
void Surface::freeData()
{
	// it's valid to delete 0
	delete data;
}

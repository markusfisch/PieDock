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
#include "Application.h"
#include "Text.h"

#include <string.h>
#include <stdio.h>

#include <string>
#include <stdexcept>

using namespace PieDock;

/**
 * Initialize color by string
 *
 * @param s - color string
 */
Text::Color::Color( const char *s ) :
	alpha( 0xff ),
	red( 0 ),
	green( 0 ),
	blue( 0 )
{
	// old sscanf still does it best
	int n = strlen( s );

	switch( n )
	{
		case 6:
			sscanf(
				s,
				"%02x%02x%02x",
				&red,
				&green,
				&blue );
			break;
		case 8:
			sscanf(
				s,
				"%02x%02x%02x%02x",
				&alpha,
				&red,
				&green,
				&blue );
			break;
		default:
			throw std::invalid_argument( "invalid color" );
	}
}

/**
 * Text
 *
 * @param d - display
 * @param da - drawable
 * @param v - visual
 * @param f - font object
 */
Text::Text( Display *d, Drawable da, Visual *v, Text::Font f ) :
	display( d )
{
#ifdef HAVE_XFT
	if( !(xftFont = XftFontOpen(
			display,
			DefaultScreen( display ),
			XFT_FAMILY,
			XftTypeString,
			f.getFamily().c_str(),
			XFT_SIZE,
			XftTypeDouble,
			f.getSize(),
			NULL )) ||
		!(xftDraw = XftDrawCreate(
			display,
			da,
			v,
			DefaultColormap(
				display,
				DefaultScreen( display ) ) )) )
		throw std::invalid_argument( "cannot open font" );
#else
	if( !(fontInfo = XLoadQueryFont(
			display,
			const_cast<char *>( f.getFamily().c_str() ) )) )
		throw std::invalid_argument( "cannot open font" );

	// set font
	{
		XGCValues values;

		values.font = fontInfo->fid;

		gc = XCreateGC(
			display,
			da,
			GCFont,
			&values );
	}

	drawable = da;
#endif

	setColor( f.getColor() );
}

/**
 * Set foreground color
 *
 * @param c - color object
 */
void Text::setColor( Color c )
{
#ifdef HAVE_XFT
	translateColor( c, &xftColor );
#else
	translateColor( c, &xColor );
#endif
}

/**
 * Draw a string into canvas
 *
 * @param x - x position
 * @param y - y position
 * @param s - string to write
 */
void Text::draw( const int x, const int y, const std::string s ) const
{
#ifdef HAVE_XFT
	XftDrawStringUtf8(
		xftDraw,
		&xftColor,
		xftFont,
		x,
		y,
		reinterpret_cast<const XftChar8 *>( s.c_str() ),
		s.length() );
#else
	XDrawString(
		display,
		drawable,
		gc,
		x,
		y,
		const_cast<char *>( s.c_str() ),
		s.length() );
#endif
}

/**
 * Determine metrics of given string
 *
 * @param s - some string
 */
Text::Metrics Text::getMetrics( const std::string s ) const
{
#ifdef HAVE_XFT
	XGlyphInfo extents;

	XftTextExtents8(
		display,
		xftFont,
		reinterpret_cast<const XftChar8 *>( s.c_str() ),
		s.length(),
		&extents );

	return Metrics(
		extents.x,
		extents.y,
		extents.width,
		extents.height );
#else
	int direction;
	int ascent;
	int descent;
	XCharStruct overall;

	XQueryTextExtents(
		display,
		fontInfo->fid,
		const_cast<char *>( s.c_str() ),
		s.length(),
		&direction,
		&ascent,
		&descent,
		&overall );

	return Metrics(
		0,
		overall.ascent,
		overall.width,
		overall.ascent+overall.descent );
#endif
}

/**
 * Transform color object into a XftColor
 *
 * @param src - source color
 * @param dest - destination color
 */
#ifdef HAVE_XFT
void Text::translateColor( const Text::Color &src, XftColor *dest )
#else
void Text::translateColor( const Text::Color &src, XColor *dest )
#endif
{
#ifdef HAVE_XFT
    XRenderColor renderColor;

	renderColor.red = src.getRed()*257;
	renderColor.green = src.getGreen()*257;
	renderColor.blue = src.getBlue()*257;
	renderColor.alpha = src.getAlpha()*257;

	XftColorAllocValue(
		display,
		DefaultVisual(
			display,
			DefaultScreen( display ) ),
		DefaultColormap(
			display,
			DefaultScreen( display ) ),
		&renderColor,
		dest );
#else
	XGCValues values;

	dest->red = src.getRed()*257;
	dest->green = src.getGreen()*257;
	dest->blue = src.getBlue()*257;

	XAllocColor(
		display,
		DefaultColormap(
			display,
			DefaultScreen( display ) ),
		dest );

	values.foreground = dest->pixel;

	XChangeGC(
		display,
		gc,
		GCForeground,
		&values );
#endif
}

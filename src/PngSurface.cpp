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
#include "PngSurface.h"

#include <fstream>

using namespace PieDock;

/**
 * Initialize surface from file
 *
 * @param f - path and filename of PNG to load
 */
PngSurface::PngSurface( std::string f ) :
	ArgbSurface( 0, 0 )
{
	std::ifstream in( f.c_str(), std::ios::in );

	if( !in.good() )
		throw ("cannot open PNG file \""+f+"\"").c_str();

	load( in );
}

/**
 * Initialize surface from stream
 *
 * @param in - input stream
 */
PngSurface::PngSurface( std::istream &in ) :
	ArgbSurface( 0, 0 )
{
	load( in );
}

/**
 * Load image from a PNG input stream
 *
 * @param in - input stream
 */
void PngSurface::load( std::istream &in )
{
	png_structp png = 0;
	png_infop info = 0;

	if( !(png = png_create_read_struct( 
			PNG_LIBPNG_VER_STRING,
			0, 0, 0 )) )
		throw "PNG library error";

	if( !in.good() ||
		!(info = png_create_info_struct( png )) ||
		setjmp( png_jmpbuf( png ) ) )
		throw "cannot open read from PNG stream";

	png_set_read_fn( png, reinterpret_cast<void *>( &in ), readPng );

	png_set_sig_bytes( png, 0 );
	png_read_png( png, info, 
		PNG_TRANSFORM_IDENTITY | 
		PNG_TRANSFORM_STRIP_16 |
		PNG_TRANSFORM_EXPAND |
		PNG_TRANSFORM_BGR,
		0 );

	if( !png_get_valid( png, info, PNG_INFO_IDAT ) )
		throw "invalid PNG stream";

	png_uint_32 w = png_get_image_width( png, info );
	png_uint_32 h = png_get_image_height( png, info );

	freeData();
	calculateSize( w, h );
	allocateData();

	png_bytepp rows = png_get_rows( png, info );

	for( int y = 0, *src, *dest = reinterpret_cast<int *>( getData() ); 
		y < h && 
			(src = reinterpret_cast<int *>( rows[y] )); 
		y++, dest += w )
		memcpy( dest, src, w<<2 );

	// convert grayscale image to argb
	{
		int ct = png_get_color_type( png, info );

		if( ct == PNG_COLOR_TYPE_GRAY_ALPHA )
			for( int y = h, 
					*line = reinterpret_cast<int *>( getData() );
				--y; 
				line += w )
			{
				unsigned char *dest = 
					reinterpret_cast<unsigned char *>( line )+
					(w<<2);
				unsigned char *src = 
					reinterpret_cast<unsigned char *>( line )+
					(w<<1);

				for( int x = w; --x; )
				{
					*(--dest) = *(--src);
					*(--dest) = *(--src);
					*(--dest) = *src;
					*(--dest) = *src;
				}
			}
	}

	png_destroy_read_struct( &png, &info, (png_infopp) 0 );
}

/**
 * Read png data from a stream; this needs to be a static member
 *
 * @param png - png pointer
 * @param data - pointer to data
 * @param length - data length
 */
void PngSurface::readPng( 
	png_structp png, 
	png_bytep data, 
	png_size_t length )
{
	std::istream *in = reinterpret_cast<std::istream *>(
		png_get_io_ptr( png ) );

	if( !in ||
		!in->good() )
	{
		png_error( png, "Cannot read from stream" );
		return;
	}

	in->read( reinterpret_cast<char *>( data ), length );
}

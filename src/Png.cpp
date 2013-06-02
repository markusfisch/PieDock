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
#include "Png.h"

#include <string>
#include <fstream>
#include <stdexcept>

using namespace PieDock;

/**
 * Load image by path and filename
 *
 * @param f - path and filename of PNG to load
 */
ArgbSurface *Png::load( const std::string &f )
{
	std::ifstream in( f.c_str(), std::ios::in );

	if( !in.good() )
		throw std::ios_base::failure( "cannot open \""+f+"\"" );

	return load( in );
}

/**
 * Load image from a PNG input stream
 *
 * @param in - input stream
 */
ArgbSurface *Png::load( std::istream &in )
{
	png_structp png = 0;
	png_infop info = 0;

	if( !(png = png_create_read_struct( 
			PNG_LIBPNG_VER_STRING,
			0, 0, 0 )) )
		throw std::runtime_error( "PNG library error" );

	if( !in.good() ||
		!(info = png_create_info_struct( png )) )
		throw std::ios_base::failure( "cannot read from PNG stream" );

	if( setjmp( png_jmpbuf( png ) ) )
	{
		png_destroy_read_struct( &png, &info, (png_infopp) 0 );
		throw std::runtime_error( "PNG error" );
	}

	png_set_read_fn(
		png,
		reinterpret_cast<void *>( &in ),
		read );

	png_set_sig_bytes( png, 0 );
	png_read_png( png, info, 
		PNG_TRANSFORM_IDENTITY | 
		PNG_TRANSFORM_STRIP_16 |
		PNG_TRANSFORM_EXPAND |
		PNG_TRANSFORM_BGR,
		0 );

	if( !png_get_valid( png, info, PNG_INFO_IDAT ) )
		throw std::runtime_error( "invalid PNG stream" );

	png_uint_32 w = png_get_image_width( png, info );
	png_uint_32 h = png_get_image_height( png, info );

	ArgbSurface *s;
	
	if( !(s = new ArgbSurface( w, h )) )
		throw std::runtime_error( "cannot allocate surface" );

	png_bytepp rows = png_get_rows( png, info );

	for( int y = 0, *src, *dest = reinterpret_cast<int *>( s->getData() ); 
		y < h && 
			(src = reinterpret_cast<int *>( rows[y] )); 
		++y, dest += w )
		memcpy( dest, src, w<<2 );

	// convert grayscale image to argb
	{
		int ct = png_get_color_type( png, info );

		if( ct == PNG_COLOR_TYPE_GRAY_ALPHA )
			for( int y = h, 
					*line = reinterpret_cast<int *>( s->getData() );
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

	return s;
}

/**
 * Save surface to given path and filename
 *
 * @param f - path and filename of PNG
 * @param s - surface to save
 */
void Png::save( const std::string &f, const ArgbSurface *s )
{
	std::ofstream out( f.c_str(), std::ios::out );

	if( !out.good() )
		throw std::ios_base::failure( "cannot create \""+f+"\"" );

	return save( out, s );
}

/**
 * Save surface to output stream in PNG format
 *
 * @param out - output stream
 * @param s - surface to save
 */
void Png::save( std::ostream &out, const ArgbSurface *s )
{
	png_structp png = 0;
	png_infop info = 0;

	if( !(png = png_create_write_struct( 
			PNG_LIBPNG_VER_STRING,
			0, 0, 0 )) )
		throw std::runtime_error( "PNG library error" );

	if( !out.good() ||
		!(info = png_create_info_struct( png )) )
		throw std::ios_base::failure( "cannot write to PNG stream" );

	if( setjmp( png_jmpbuf( png ) ) )
	{
		png_destroy_write_struct( &png, &info );
		throw std::runtime_error( "PNG error" );
	}

	png_set_write_fn(
		png,
		reinterpret_cast<void *>( &out ),
		write,
		flush );

	png_set_IHDR(
		png,
		info,
		s->getWidth(),
		s->getHeight(),
		8,
		PNG_COLOR_TYPE_RGB_ALPHA,
		PNG_INTERLACE_NONE,
		PNG_COMPRESSION_TYPE_DEFAULT,
		PNG_FILTER_TYPE_DEFAULT );

	png_bytep *rows = new png_bytep[s->getHeight()];

	for( int y = 0, *src = reinterpret_cast<int *>( s->getData() ); 
		y < s->getHeight(); 
		++y, src += s->getWidth() )
		rows[y] = reinterpret_cast<png_bytep>( src );

	png_set_bgr( png );

	png_write_info( png, info );
	png_write_image( png, rows );
	png_write_end( png, info );
	png_destroy_write_struct( &png, &info );

	delete rows;
}

/**
 * Read png data from a stream
 *
 * @param png - png pointer
 * @param data - pointer to data
 * @param length - data length
 */
void Png::read( 
	png_structp png, 
	png_bytep data, 
	png_size_t length )
{
	std::istream *in = reinterpret_cast<std::istream *>(
		png_get_io_ptr( png ) );

	if( !in ||
		!in->good() )
	{
		png_error( png, "cannot read from stream" );
		return;
	}

	in->read( reinterpret_cast<char *>( data ), length );
}

/**
 * Write png data to a stream
 *
 * @param png - png pointer
 * @param data - pointer to data
 * @param length - data length
 */
void Png::write(
	png_structp png,
	png_bytep data,
	png_size_t length )
{
	std::ostream *out = reinterpret_cast<std::ostream *>(
		png_get_io_ptr( png ) );

	if( !out )
	{
		png_error( png, "cannot write to stream" );
		return;
	}

	out->write( reinterpret_cast<char *>( data ), length );
}

/**
 * Flush data to disk
 *
 * @param png - png pointer
 */
void Png::flush( png_structp png )
{
	std::ostream *out = reinterpret_cast<std::ostream *>(
		png_get_io_ptr( png ) );

	if( !out )
	{
		png_error( png, "cannot flush stream" );
		return;
	}

	out->flush();
}

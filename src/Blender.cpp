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
#include "Blender.h"

#include <stdint.h>

using namespace PieDock;

/**
 * Initialize blender
 *
 * @param c - canvas image of arbitrary color-depth
 */
Blender::Blender( Surface &c ) :
	canvas( &c )
{
}

/**
 * Blend ARGB surface into canvas
 *
 * @param src - ARGB surface to blend into canvas
 * @param x - left position in canvas (optional)
 * @param y - upper position in canvas (optional)
 * @param a - alpha value (optional)
 */
void Blender::blend( const ArgbSurface &src, int x, int y, int a )
{
	Details details = {
		canvas->getData(),
		src.getData(),
		src.getWidth(),
		src.getHeight(),
		src.getPadding(),
		0,
		a };

	if( x > canvas->getWidth() ||
		y > canvas->getHeight() )
		return;

	if( x < 0 )
	{
		int o = x*-src.getBytesPerPixel();

		details.src += o;
		details.srcSkip += o;
		details.length += x;

		x = 0;
	}

	if( y < 0 )
	{
		details.src += y*-src.getBytesPerLine();
		details.repeats += y;
		y = 0;
	}

	details.destSkip = x*canvas->getBytesPerPixel();
	details.dest +=
		y*canvas->getBytesPerLine()+
		details.destSkip;
	details.destSkip += canvas->getPadding();

	{
		int e = x+details.length;

		if( e >= canvas->getWidth() )
		{
			details.length = canvas->getWidth()-x;
			details.srcSkip +=
				(e-canvas->getWidth())*
				src.getBytesPerPixel();
		}
		else
			details.destSkip +=
				(canvas->getWidth()-e)*
				canvas->getBytesPerPixel();
	}

	if( y+details.repeats >= canvas->getHeight() )
		details.repeats = canvas->getHeight()-y;

	if( details.length <= 0 ||
		details.repeats <= 0 )
		return;

	switch( canvas->getBytesPerPixel() )
	{
		case 4:
			blendInto32Bit( details );
			break;
		case 3:
			blendInto24Bit( details );
			break;
		case 2:
			blendInto16Bit( details );
			break;
		default:
			throw "number of bytes per pixel not supported";
	}
}

/**
 * Blend into ARGB image
 *
 * @param details - blending details
 */
void Blender::blendInto32Bit( Details &details )
{
	uint32_t *src = reinterpret_cast<uint32_t *>( details.src );
	uint32_t *dest = reinterpret_cast<uint32_t *>( details.dest );

	// define before the loops, so this will stay in stack
	double mod;

	// global alpha, bool for speed
	double alphaMax = 255.0;
	bool useGlobalAlpha = false;

#ifdef HAVE_XRENDER
	double alphaMod = details.alpha/255.0;
#endif

	if( details.alpha != 0xff )
	{
		alphaMax *= 255.0/static_cast<double>( details.alpha );
		useGlobalAlpha = true;
	}

	// only 4-byte alignments are sane for 32 bits per pixel
	if( details.destSkip%4 ||
		details.srcSkip%4 )
		throw "cannot deal with strange 32 bits per pixel alignment";

	// bytes to uint32_t
	details.destSkip >>= 2;
	details.srcSkip >>= 2;

	for( int r = details.repeats;
		r--;
		dest += details.destSkip, src += details.srcSkip )
		for( int l = details.length; l--; )
		{
			uint32_t a = (*src)&0xff000000;

			if( !a )
			{
				++src;
				++dest;
			}
			else if( a == 0xff000000 &&
				!useGlobalAlpha )
			{
				*(dest++) = *(src++);
			}
			else
			{
				a >>= 24;
				mod = alphaMax/static_cast<double>( a );

				register int d = (*dest)&0xff;
				register int blue = (*src)&0xff;
				blue -= d;
				blue /= mod;
				blue += d;

				d = (*dest>>8)&0xff;
				register int green = (*src>>8)&0xff;
				green -= d;
				green /= mod;
				green += d;

				d = (*dest>>16)&0xff;
				register int red = (*src>>16)&0xff;
				red -= d;
				red /= mod;
				red += d;

#ifdef HAVE_XRENDER
				a *= alphaMod;
				a += (*dest>>24)&0xff;

				if( a > 0xff )
					a = 0xff;
#endif

				*dest =
#ifdef HAVE_XRENDER
					(a<<24) |
#endif
					(red<<16) |
					(green<<8) |
					blue;

				++dest;
				++src;
			}
		}
}

/**
 * Blend into RGB image
 *
 * @param details - blending details
 */
void Blender::blendInto24Bit( Details &details )
{
	uint8_t *src = reinterpret_cast<uint8_t *>( details.src );
	uint8_t *dest = reinterpret_cast<uint8_t *>( details.dest );

	// define before the loops, so this will stay in stack
	double diff;
	double mod;

	// global alpha, bool for speed
	double alphaMax = 255.0;
	bool useGlobalAlpha = false;

	if( details.alpha != 0xff )
	{
		alphaMax *= 255.0/static_cast<double>( details.alpha );
		useGlobalAlpha = true;
	}

	for( int r = details.repeats;
		r--;
		dest += details.destSkip, src += details.srcSkip )
		for( int l = details.length; l--; )
		{
			uint32_t a = *reinterpret_cast<uint32_t *>( src ) & 0xff000000;

			if( !a )
			{
				src += 4;
				dest += 3;
			}
			else if( a == 0xff000000 &&
				!useGlobalAlpha )
			{
				*(dest++) = *(src++);
				*(dest++) = *(src++);
				*(dest++) = *(src++);
				++src;
			}
			else
			{
				mod = alphaMax/static_cast<double>( *(src+3) );

				diff = *(src++);
				diff -= *dest;
				diff /= mod;
				*(dest++) += static_cast<uint8_t>( diff );

				diff = *(src++);
				diff -= *dest;
				diff /= mod;
				*(dest++) += static_cast<uint8_t>( diff );

				diff = *(src++);
				diff -= *dest;
				diff /= mod;
				*(dest++) += static_cast<uint8_t>( diff );

				++src;
			}
		}
}

/**
 * Blend into HighColor image
 *
 * @param details - blending details
 */
void Blender::blendInto16Bit( Details &details )
{
	uint8_t *src = reinterpret_cast<uint8_t *>( details.src );
	uint8_t *dest = reinterpret_cast<uint8_t *>( details.dest );

	// define before the loops, so this will stay in stack
	double diff;
	double mod;

	// global alpha, bool for speed
	double alphaMax = 255.0;
	bool useGlobalAlpha = false;

	if( details.alpha != 0xff )
	{
		alphaMax *= 255.0/static_cast<double>( details.alpha );
		useGlobalAlpha = true;
	}

	for( int r = details.repeats;
		r--;
		dest += details.destSkip, src += details.srcSkip )
		for( int l = details.length; l--; )
		{
			uint32_t a = *reinterpret_cast<uint32_t *>( src ) & 0xff000000;

			if( !a )
			{
				src += 4;
				dest += 2;
			}
			else if( a == 0xff000000 &&
				!useGlobalAlpha )
			{
				register int blue = *(src++);
				register int green = *(src++);
				register int red = *(src++);

				*(reinterpret_cast<uint16_t *>( dest )) =
					static_cast<uint16_t>( (blue&0xf8)>>3 ) |
					static_cast<uint16_t>( (green&0xf8)<<3 ) |
					static_cast<uint16_t>( (red&0xf8)<<8 );

				++src;
				dest += 2;
			}
			else
			{
				mod = alphaMax/static_cast<double>( *(src+3) );

				uint16_t pixel = *(reinterpret_cast<uint16_t *>( dest ));
				register int blue = (pixel<<3)&0xf8;
				register int green = (pixel>>3)&0xf8;
				register int red = (pixel>>8)&0xf8;

				diff = *(src++);
				diff -= blue;
				diff /= mod;
				blue += static_cast<uint8_t>( diff );

				diff = *(src++);
				diff -= green;
				diff /= mod;
				green += static_cast<uint8_t>( diff );

				diff = *(src++);
				diff -= red;
				diff /= mod;
				red += static_cast<uint8_t>( diff );

				*(reinterpret_cast<uint16_t *>( dest )) =
					static_cast<uint16_t>( (blue&0xf8)>>3 ) |
					static_cast<uint16_t>( (green&0xf8)<<3 ) |
					static_cast<uint16_t>( (red&0xf8)<<8 );

				dest += 2;
				++src;
			}
		}
}

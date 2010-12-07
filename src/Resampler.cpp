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
#include "Resampler.h"

#include <string.h>
#include <math.h>

using namespace PieDock;

/**
 * Resample surface
 *
 * @param dest - new surface
 * @param src - source surface
 */
void Resampler::resample( ArgbSurface &dest, ArgbSurface &src )
{
	if( src.getWidth() == dest.getWidth() &&
		src.getHeight() == dest.getHeight() )
		// when the format is the same, just make a copy
		memcpy( dest.getData(), src.getData(), src.getSize() );
	else if( dest.getWidth() > src.getWidth() ||
		dest.getHeight() > dest.getHeight() )
		// upsampling by bi-linear filtering
		Resampler::biLinear( dest, src );
	else
		// downsampling by area averaging
		Resampler::areaAveraging( dest, src );
}

/**
 * Resample by bi-linear filtering; use this method for magnification
 *
 * @param dest - new surface
 * @param src - source surface
 */
void Resampler::biLinear( ArgbSurface &dest, ArgbSurface &src )
{
	unsigned char *s = src.getData();
	unsigned char *d = dest.getData();
	double xCell = static_cast<double>( src.getWidth() )/dest.getWidth();
	double yCell = static_cast<double>( src.getHeight() )/dest.getHeight();
	double yPos = 0.0;
	double xLast = src.getWidth()-1;
	double yLast = src.getHeight()-1;

	for( int dy = dest.getHeight();
		dy--;
		d += dest.getPadding(),
			yPos += yCell )
	{
		double xPos = 0.0;
		unsigned char *base = s+
			(static_cast<int>( floor( yPos ) )*src.getBytesPerLine());

		for( int dx = dest.getWidth();
			dx--;
			xPos += xCell )
		{
			unsigned char *sp = base+
				(static_cast<int>( floor( xPos ) )*src.getBytesPerPixel());
			double uRatio = fmod( xPos, 1.0 );
			double vRatio = fmod( yPos, 1.0 );
			double uOpposite = 1-uRatio;
			double vOpposite = 1-vRatio;

			// unrolled loop for speed
			if( yPos < yLast )
			{
				// above last line in source image
				if( xPos < xLast )
				{
					// pixel has a horizontal neighbor
					*d++ = static_cast<int>(
						(uOpposite*sp[0]+uRatio*sp[4])*vOpposite +
						(uOpposite*sp[src.getBytesPerLine()]+
							uRatio*sp[src.getBytesPerLine()+4])*vRatio );
					sp++;
					*d++ = static_cast<int>(
						(uOpposite*sp[0]+uRatio*sp[4])*vOpposite +
						(uOpposite*sp[src.getBytesPerLine()]+
							uRatio*sp[src.getBytesPerLine()+4])*vRatio );
					sp++;
					*d++ = static_cast<int>(
						(uOpposite*sp[0]+uRatio*sp[4])*vOpposite +
						(uOpposite*sp[src.getBytesPerLine()]+
							uRatio*sp[src.getBytesPerLine()+4])*vRatio );
					sp++;
					*d++ = static_cast<int>(
						(uOpposite*sp[0]+uRatio*sp[4])*vOpposite +
						(uOpposite*sp[src.getBytesPerLine()]+
							uRatio*sp[src.getBytesPerLine()+4])*vRatio );
				}
				else
				{
					*d++ = static_cast<int>(
						(uOpposite*sp[0])*vOpposite +
						(uOpposite*sp[src.getBytesPerLine()])*vRatio );
					sp++;
					*d++ = static_cast<int>(
						(uOpposite*sp[0])*vOpposite +
						(uOpposite*sp[src.getBytesPerLine()])*vRatio );
					sp++;
					*d++ = static_cast<int>(
						(uOpposite*sp[0])*vOpposite +
						(uOpposite*sp[src.getBytesPerLine()])*vRatio );
					sp++;
					*d++ = static_cast<int>(
						(uOpposite*sp[0])*vOpposite +
						(uOpposite*sp[src.getBytesPerLine()])*vRatio );
				}
			}
			else
			{
				// last line in source image
				if( xPos < xLast )
				{
					// pixel has a horizontal neighbor
					*d++ = static_cast<int>(
						(uOpposite*sp[0]+uRatio*sp[4])*vOpposite );
					sp++;
					*d++ = static_cast<int>(
						(uOpposite*sp[0]+uRatio*sp[4])*vOpposite );
					sp++;
					*d++ = static_cast<int>(
						(uOpposite*sp[0]+uRatio*sp[4])*vOpposite );
					sp++;
					*d++ = static_cast<int>(
						(uOpposite*sp[0]+uRatio*sp[4])*vOpposite );
				}
				else
				{
					// last source pixel
					*d++ = static_cast<int>( (uOpposite*sp[0])*vOpposite );
					sp++;
					*d++ = static_cast<int>( (uOpposite*sp[0])*vOpposite );
					sp++;
					*d++ = static_cast<int>( (uOpposite*sp[0])*vOpposite );
					sp++;
					*d++ = static_cast<int>( (uOpposite*sp[0])*vOpposite );
				}
			}
		}
	}
}

/**
 * Resample by area averaging; use this method for minification
 *
 * @param dest - new surface
 * @param src - source surface
 */
void Resampler::areaAveraging( ArgbSurface &dest, ArgbSurface &src )
{
	unsigned char *s = src.getData();
	unsigned char *d = dest.getData();
	double xCell = static_cast<double>( src.getWidth() )/dest.getWidth();
	double yCell = static_cast<double>( src.getHeight() )/dest.getHeight();
	double yPos = 0.0;

	for( int dy = dest.getHeight();
		dy--;
		d += dest.getPadding(),
			yPos += yCell )
	{
		double xPos = 0.0;
		unsigned char *base = s+
			(static_cast<int>( floor( yPos ) )*src.getBytesPerLine());

		for( int dx = dest.getWidth();
			dx--;
			xPos += xCell )
		{
			unsigned char *sp = base+
				(static_cast<int>( floor( xPos ) )*src.getBytesPerPixel());

			int width = static_cast<int>( ceil( xPos+xCell )-floor( xPos ) );
			int height = static_cast<int>( ceil( yPos+yCell )-floor( yPos ) );
			int xLast = width-1;
			int yLast = height-1;

			if( !dx )
				width = static_cast<int>( round( xPos+xCell )-floor( xPos ) );

			if( !dy )
				height = static_cast<int>( round( yPos+yCell )-floor( yPos ) );

			int skip = src.getBytesPerLine()-(width*src.getBytesPerPixel());
			double blue = 0;
			double green = 0;
			double red = 0;
			double alpha = 0;
			double samples = 0;

			for( int sy = 0;
				sy < height;
				sy++, sp += skip )
				for( int sx = 0;
					sx < width;
					sx++ )
				{
					double b = *sp++;
					double g = *sp++;
					double r = *sp++;
					double a = *sp++;
					double fraction = 1.0;
					double ratio = 0;

					if( !sx &&
						(ratio = fmod( xPos, 1.0 )) > 0.0 )
						fraction = 1.0-ratio;
					else if( sx == xLast &&
						(ratio = fmod( xPos+xCell, 1.0 )) > 0.0 )
						fraction = ratio;

					if( !sy &&
						(ratio = fmod( yPos, 1.0 )) > 0.0 )
						fraction *= 1.0-ratio;
					else if( sy == yLast &&
						(ratio = fmod( yPos+yCell, 1.0 )) > 0.0 )
						fraction *= ratio;

					if( fraction != 1.0 )
					{
						b *= fraction;
						g *= fraction;
						r *= fraction;
						a *= fraction;
						samples += fraction;
					}
					else
						samples += 1.0;

					blue += b;
					green += g;
					red += r;
					alpha += a;
				}

			*d++ = static_cast<int>( blue/samples );
			*d++ = static_cast<int>( green/samples );
			*d++ = static_cast<int>( red/samples );
			*d++ = static_cast<int>( alpha/samples );
		}
	}
}

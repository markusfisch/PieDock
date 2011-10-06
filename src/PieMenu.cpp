/*
 *   O         ,-
 *  ° o    . -´  '     ,-
 *   °  .´        ` . ´,´
 *     ( °   ))     . (
 *      `-;_    . -´ `.`.
 *          `._'       ´
 *
 * Copyright (c) 2007-2011 Markus Fisch <mf@markusfisch.de>
 *
 * Licensed under the MIT license:
 * http://www.opensource.org/licenses/mit-license.php
 */
#include "PieMenu.h"

#include <math.h>

using namespace PieDock;

const double PieMenu::radiansPerCircle = M_PI+M_PI;
const double PieMenu::turnSteps[] = {
	.1,
	.3,
	.4,
	.5,
	.7,
	.9,
	.7,
	.5,
	.3,
	.1,
	.0 };

/**
 * Initialize menu
 *
 * @param a - application
 * @param s - surface to draw menu into
 */
PieMenu::PieMenu( Application *a, Surface &s ) :
	Menu( a ),
	blender( s ),
	size( (s.getWidth() < s.getHeight() ? s.getWidth() : s.getHeight()) ),
	maxRadius( (size-static_cast<int>( .3*size ))>>1 ),
	radius( size>>2 ),
	twist( .0 ),
	centerX( s.getWidth()>>1 ),
	centerY( s.getHeight()>>1 ),
	turnStack( 0 )
{
#ifdef HAVE_XRENDER
	if( a->getSettings()->useCompositing() )
		blender.setCompositing( true );
#endif

	if( maxRadius < radius )
		throw "window too small";
}

/**
 * Reset and update menu
 *
 * @param n - menu name (optional)
 */
bool PieMenu::update( std::string n )
{
	radius = static_cast<int>(
		app->getSettings()->getStartRadius()*maxRadius );
	twist = -.05*static_cast<double>( (maxRadius-radius)>>1 );
	lastX = lastY = -1;

	if( turnStack )
	{
		delete turnStack;
		turnStack = 0;
	}

	return Menu::update( n );
}

/**
 * Returns true if menu needs to be redrawn
 *
 * @param x - x coordinate of cursor position within target surface
 * @param y - y coordinate of cursor position within target surface
 */
bool PieMenu::isObsolete( int x, int y )
{
	// menu is always obsolete while animating
	if( radius < maxRadius ||
		turnStack )
		return true;

	// redraw only if position changed
	if( x != lastX ||
		y != lastY )
	{
		lastX = x;
		lastY = y;

		return true;
	}

	return false;
}

/**
 * Draw menu
 *
 * @param x - x coordinate of cursor position within target surface
 * @param y - y coordinate of cursor position within target surface
 */
void PieMenu::draw( int x, int y )
{
	int numberOfIcons = menuItems->size();
	int closestIcon = 0;
	bool cursorNearCenter = false;
	struct
	{
		int x;
		int y;
		double weight;
		double size;
		double cellSize;
	} iconGeometries[numberOfIcons];

	// reset selected icon
	selected = 0;

	// don't do anything if there are no icons
	if( !numberOfIcons )
		return;

	// calculate positions and sizes
	{
		double circumference = M_PI*(radius<<1);
		double pixelsPerRadian = radiansPerCircle/circumference;
		int centeredY = y-centerY;
		int centeredX = x-centerX;
		double cursorAngle = atan2( centeredY, centeredX );
		double cellSize = radiansPerCircle/numberOfIcons;
		double closestAngle = 0;
		double weight = 0;
		double maxIconSize = .8*radius;
		double maxWeight;

		// calculate weight of each icon
		{
			double cursorRadius = sqrt(
				(centeredY*centeredY)+(centeredX*centeredX) );
			double infieldRadius = radius>>1;
			double z = app->getSettings()->getZoomModifier();
			double f = cursorRadius/infieldRadius*z;

			if( f > z )
				f = z;

			if( f < 1.0 )
			{
				double b = (circumference/numberOfIcons)*.75;

				if( b < maxIconSize )
					maxIconSize = b+(maxIconSize-b)*f;
			}

			if( cursorRadius < infieldRadius )
				cursorNearCenter = true;

			// determine how close every icon is to the cursor
			{
				double closestDistance = radiansPerCircle;
				double a = twist;
				double m = (maxIconSize*pixelsPerRadian)/cellSize;

				maxWeight = M_PI_2+pow( M_PI, m );

				for( int n = 0; n < numberOfIcons; ++n )
				{
					double d = fabs( getAngleDifference( a, cursorAngle ) );

					if( d < closestDistance )
					{
						closestDistance = d;
						closestIcon = n;
						closestAngle = a;
					}

					if( f < 1.0 )
						d *= f;

					iconGeometries[n].weight = M_PI_2+pow( M_PI-d, m );
					weight += iconGeometries[n].weight;

					if( (a += cellSize) > M_PI )
						a -= radiansPerCircle;
				}
			}
		}

		// calculate size of icons
		{
			double sizeUnit = circumference/weight;

			for( int n = numberOfIcons; n--; )
				iconGeometries[n].size =
					iconGeometries[n].cellSize =
						sizeUnit*iconGeometries[n].weight;

			// scale icons within cell
			{
				double maxSize = sizeUnit*maxWeight;

				if( maxSize > maxIconSize )
				{
					double f = maxIconSize/maxSize;

					for( int n = numberOfIcons; n--; )
						iconGeometries[n].size *= f;
				}
			}
		}

		// calculate icon positions
		{
			double difference = getAngleDifference(
				cursorAngle, closestAngle );
			double angle = getValidAngle(
				cursorAngle-
					(pixelsPerRadian*
						iconGeometries[closestIcon].cellSize)/cellSize*
					difference );

			// active icon
			iconGeometries[closestIcon].x =
				centerX+static_cast<int>( radius*cos( angle ) );
			iconGeometries[closestIcon].y =
				centerY+static_cast<int>( radius*sin( angle ) );

			// calculate positions of all other icons
			{
				double leftAngle = angle;
				double rightAngle = angle;
				int left = closestIcon;
				int right = closestIcon;
				int previousRight = closestIcon;
				int previousLeft = closestIcon;

				for( int n = 0; ; ++n )
				{
					if( (--left) < 0 )
						left = numberOfIcons-1;

					// break here when number of icons is odd
					if( right == left )
						break;

					if( (++right) >= numberOfIcons )
						right = 0;

					leftAngle = getValidAngle(
						leftAngle-
							(
								(.5*iconGeometries[previousLeft].cellSize)+
								(.5*iconGeometries[left].cellSize)
							)*pixelsPerRadian );

					iconGeometries[left].x =
						centerX+static_cast<int>( radius*cos( leftAngle ) );
					iconGeometries[left].y =
						centerY+static_cast<int>( radius*sin( leftAngle ) );

					// break here when number of icons is even
					if( left == right )
						break;

					rightAngle = getValidAngle(
						rightAngle+
							(
								(.5*iconGeometries[previousRight].cellSize)+
								(.5*iconGeometries[right].cellSize)
							)*pixelsPerRadian );

					iconGeometries[right].x =
						centerX+static_cast<int>( radius*cos( rightAngle ) );
					iconGeometries[right].y =
						centerY+static_cast<int>( radius*sin( rightAngle ) );

					previousRight = right;
					previousLeft = left;
				}
			}
		}
	}

	// draw icons
	{
		const ActiveIndicator *activeIndicator =
			&app->getSettings()->getActiveIndicator();
		ArgbSurfaceSizeMap *activeIndicatorSizeMap =
			activeIndicator->getSizeMap();
		const bool selectInCenter =
			(app->getSettings()->getCenterAction() ==
				Settings::CenterNearestIcon ?
				true :
				false);
		int n = 0;

		for( MenuItems::iterator i = menuItems->begin();
			i != menuItems->end();
			++i, ++n )
		{
			const int size = static_cast<int>( iconGeometries[n].size )>>1<<1;
			const ArgbSurface *surface =
				(*i)->getIcon()->getSurface( size, size );

			if( !surface )
				continue;

			const int x = iconGeometries[n].x-(surface->getWidth()>>1);
			const int y = iconGeometries[n].y-(surface->getHeight()>>1);
			int opacity;

			if( n == closestIcon &&
				(!cursorNearCenter ||
					selectInCenter) )
			{
				selected = *i;
				opacity = app->getSettings()->getFocusedAlpha();
			}
			else
				opacity = app->getSettings()->getUnfocusedAlpha();

			blender.blend(
				*surface,
				x,
				y,
				opacity );

			if( (*i)->hasWindows() )
			{
				const int activeIndicatorSize = size/3;
				const ArgbSurface *s =
					(activeIndicatorSizeMap ?
						activeIndicatorSizeMap :
						(*i)->getIcon())->getSurface(
							activeIndicatorSize,
							activeIndicatorSize );

				if( s )
					blender.blend(
						*s,
						x+activeIndicator->getX(
							activeIndicatorSize,
							surface->getWidth() ),
						y+activeIndicator->getY(
							activeIndicatorSize,
							surface->getHeight() ),
						opacity );
			}
		}
	}

	// zoom and rotate into appearance
	if( radius < maxRadius )
	{
		if( (radius += 2) > maxRadius )
			radius = maxRadius;

		if( (twist += .05) > radiansPerCircle )
			twist -= radiansPerCircle;
	}
	else if( turnStack )
	{
		if( *turnBy == .0 )
		{
			delete turnStack;
			turnStack = 0;
		}
		else
		{
			twist += *(turnBy++);

			if( twist > radiansPerCircle )
				twist -= radiansPerCircle;
			else if( twist < 0 )
				twist += radiansPerCircle;
		}
	}
}

/**
 * Turn menu
 *
 * @param r - radians to turn
 */
void PieMenu::turn( double r )
{
	twist += r;

	if( twist > radiansPerCircle )
		twist -= radiansPerCircle;
	else if( twist < 0 )
		twist += radiansPerCircle;

	// force redraw
	lastX = lastY = -1;
}

/**
 * Turn menu by the given number of cells
 *
 * @param c - cells to turn
 */
void PieMenu::turn( int c )
{
	double f = radiansPerCircle/
		static_cast<double>( menuItems->size() )*
		static_cast<double>( c );

	if( turnStack )
	{
		for( ; *turnBy != 0.0; ++turnBy )
			f += *turnBy;

		delete turnStack;
	}

	f /= 4.5; // turn steps

	turnBy = turnStack = new double[sizeof( turnSteps )/sizeof( double )];

	double *t = turnStack;
	const double *s = turnSteps;

	for( ; ; ++t, ++s )
	{
		*t = f*(*s);
		if( *s == 0.0 )
			break;
	}
}

/**
 * Set twist to have the last selected item selected again
 */
void PieMenu::setTwistForSelection()
{
	if( !selected )
		return;

	int n = 0;
	double f = radiansPerCircle/
		static_cast<double>( menuItems->size() );

	for( MenuItems::iterator i = menuItems->begin();
		i != menuItems->end();
		++i, ++n )
		if( *i == selected )
		{
			if( menuItems->oneIconPerWindow() &&
				++n >= menuItems->size() )
				break;

			if( n )
				twist -= f*static_cast<double>( n );

			break;
		}
}

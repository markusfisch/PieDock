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
#ifndef _PieDock_PieMenu_
#define _PieDock_PieMenu_

#include "Application.h"
#include "Surface.h"
#include "Blender.h"
#include "Icon.h"
#include "Menu.h"

#include <vector>
#include <math.h>

namespace PieDock
{
	/**
	 * Realize a pie menu
	 *
	 * @author Markus Fisch <mf@markusfisch.de>
	 */
	class PieMenu : public Menu
	{
		public:
			PieMenu( Application *, Surface & );
			virtual ~PieMenu() {}
			inline const bool cursorInCenter() const { return (selected == 0); }
			inline const int &getRadius() const { return maxRadius; }
			inline Blender *getBlender() { return &blender; }
			virtual bool update( std::string = "" );
			virtual bool isObsolete( int, int );
			virtual void draw( int, int );
			virtual void turn( double );
			virtual void turn( int );
			virtual void setTwistForSelection();

		protected:
			static const double radiansPerCircle;
			static const double turnSteps[];

			Blender blender;
			int size;
			int maxRadius;
			int radius;
			double twist;
			int centerX;
			int centerY;
			int lastX;
			int lastY;
			double *turnStack;
			double *turnBy;

			/**
			 * Return the difference of two angles in radians; implemented
			 * here to ensure the method will be compiled inline
			 *
			 * @param a - angle in radians
			 * @param b - angle in radians
			 */
			inline virtual double getAngleDifference( double a, double b )
			{
				double c = a-b;
				double d;

				if( a > b )
					d = a-(b+radiansPerCircle);
				else
					d = a-(b-radiansPerCircle);

				if( fabs( c ) < fabs( d ) )
					return c;

				return d;
			}

			/**
			 * Recalculate angle to be within a valid range; implemented
			 * here to ensure the method will be compiled inline
			 *
			 * @param a - angle in radians
			 */
			inline virtual double getValidAngle( double a )
			{
				if( a < -M_PI )
					a += radiansPerCircle;
				else if( a > M_PI )
					a -= radiansPerCircle;

				return a;
			}
	};
}

#endif

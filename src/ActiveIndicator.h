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
#ifndef _PieDock_ActiveIndicator_
#define _PieDock_ActiveIndicator_

#include "ArgbSurfaceSizeMap.h"

namespace PieDock
{
	/**
	 * Active indicator
	 *
	 * @author Markus Fisch <mf@markusfisch.de>
	 */
	class ActiveIndicator
	{
		public:
			ActiveIndicator() : sizeMap( 0 ) { reset(); }
			virtual ~ActiveIndicator() { clear(); }
			inline ArgbSurfaceSizeMap *getSizeMap() const {
				return sizeMap; }
			inline const int getX( int s, int i ) const
			{
				switch( xAlign )
				{
					default:
					case Left:
						return 0;
					case Center:
						return (i-s)>>1;
					case Right:
						return i-s;
				}
			}
			inline const int getY( int s, int i ) const
			{
				switch( yAlign )
				{
					default:
					case Top:
						return 0;
					case Center:
						return (i-s)>>1;
					case Bottom:
						return i-s;
				}
			}
			virtual void load( std::string );
			virtual void parseX( std::string );
			virtual void parseY( std::string );
			virtual void reset();

		private:
			enum Alignment
			{
				Left,
				Right,
				Top,
				Bottom,
				Center
			};

			ArgbSurfaceSizeMap *sizeMap;
			Alignment xAlign;
			Alignment yAlign;

			void clear();
	};
}

#endif

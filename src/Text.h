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
#ifndef _PieDock_Text_
#define _PieDock_Text_

#include <X11/Xlib.h>

#ifdef HAVE_XFT
#include <X11/Xft/Xft.h>
#endif

#include <string>

namespace PieDock
{
	// forward declaration
	class Application;

	/**
	 * Text output
	 *
	 * @author Markus Fisch <mf@markusfisch.de>
	 */
	class Text
	{
		public:
			/**
			 * Text color
			 */
			class Color
			{
				public:
					Color() :
						alpha( 0 ),
						red( 0 ),
						green( 0 ),
						blue( 0 ) {}
					Color( int r, int g, int b, int a = 0xff ) :
						alpha( a ),
						red( r ),
						green( g ),
						blue( b ) {}
					Color( const char * );
					virtual ~Color() {}
					inline const int &getAlpha() const { return alpha; }
					inline const int &getRed() const { return red; }
					inline const int &getGreen() const { return green; }
					inline const int &getBlue() const { return blue; }
					inline const unsigned int getColor() const { return
						alpha<<24 |
						red<<16 |
						green<<8 |
						blue; }

				private:
					int alpha;
					int red;
					int green;
					int blue;
			};

			/**
			 * Text appearance
			 */
			class Font
			{
				public:
					Font() { reset(); }
					Font( const std::string f, const double s, const Color c ) :
						family( f ),
						size( s ),
						color( c ) {}
					virtual ~Font() {}
					inline const std::string &getFamily() const {
						return family; }
					inline void setFamily( const std::string &s ) {
						family = s; }
					inline const double &getSize() const { return size; }
					inline void setSize( const double s ) { size = s; }
					inline const Color &getColor() const { return color; }
					inline void setColor( const Color c ) { color = c; }
					inline void reset()
					{
#ifdef HAVE_XFT
						family = "Sans";
#else
						family = "6x10";
#endif
						size = 10.0;
						color = Color( 0xff, 0xff, 0xff, 0xff );
					}

				private:
					std::string family;
					double size;
					Color color;
			};

			/**
			 * Font metrics
			 */
			class Metrics
			{
				public:
					Metrics() : x( 0 ), y( 0 ), width( 0 ), height( 0 ) {}
					Metrics( int xx, int yy, int w, int h ) :
						x( xx ),
						y( yy ),
						width( w ),
						height( h ) {}
					virtual ~Metrics() {}
					inline const int &getX() const { return x; }
					inline void setX( int xx ) { x = xx; }
					inline const int &getY() const { return y; }
					inline void setY( int yy ) { y = yy; }
					inline const int &getWidth() const { return width; }
					inline void setWidth( int w ) { width = w; }
					inline const int &getHeight() const { return height; }
					inline void setHeight( int h ) { height = h; }					

				private:
					int x;
					int y;
					int width;
					int height;
			};

			Text( Application *, Drawable, Visual *, Font );
			virtual ~Text() {}
			virtual void setColor( const Color );
			virtual void draw( const int, const int, const std::string ) const;
			virtual Metrics getMetrics( const std::string ) const;

		private:
			Application *app;
#ifdef HAVE_XFT
			XftFont *xftFont;
			XftDraw *xftDraw;
			XftColor xftColor;
#else
			Drawable drawable;
			XFontStruct *fontInfo;
			XColor xColor;
			GC gc;
#endif

#ifdef HAVE_XFT
			void translateColor( const Color &, XftColor * );
#else
			void translateColor( const Color &, XColor * );
#endif
	};
}

#endif

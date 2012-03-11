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
#ifndef _PieDock_Blender_
#define _PieDock_Blender_

#include "Surface.h"
#include "ArgbSurface.h"

namespace PieDock
{
	/**
	 * Blend ARGB surfaces into some other surface of arbitrary color depth
	 *
	 * @author Markus Fisch <mf@markusfisch.de>
	 */
	class Blender
	{
		public:
			enum Opacity
			{
				Transparent = 0,
				Translucent = 128,
				Opaque = 0xff
			};

			Blender( Surface & );
			virtual ~Blender() {}
#ifdef HAVE_XRENDER
			virtual void setCompositing( bool c ) { compositing = c; }
			virtual const bool &useCompositing() const { return compositing; }
#endif
			virtual void blend( const ArgbSurface &, int, int, int = Opaque );

		protected:
			typedef struct
			{
				unsigned char *dest;
				unsigned char *src;
				int length;
				int repeats;
				int srcSkip;
				int destSkip;
				int alpha;
			} Details;

			virtual void blendInto32Bit( Details & );
			virtual void blendInto24Bit( Details & );
			virtual void blendInto16Bit( Details & );

		private:
			Surface *canvas;
#ifdef HAVE_XRENDER
			bool compositing;
#endif
	};
}

#endif

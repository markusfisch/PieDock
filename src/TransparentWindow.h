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
#ifndef _PieDock_TransparentWindow_
#define _PieDock_TransparentWindow_

#include "Application.h"
#include "XSurface.h"

#include <X11/Xlib.h>

#ifdef HAVE_XRENDER
#include <X11/extensions/Xrender.h>
#endif

namespace PieDock
{
	/**
	 * A (possibly pseudo-)transparent window
	 *
	 * @author Markus Fisch <mf@markusfisch.de>
	 */
	class TransparentWindow
	{
		public:
			TransparentWindow( Application & );
			virtual ~TransparentWindow();
			inline const Window &getWindow() const { return window; }
			virtual void appear() {}
			virtual void draw() {}
			virtual bool processEvent( XEvent & ) { return false; }

		protected:
			Application *app;
			Window window;
			int width;
			int height;
			XSurface *canvas;
			unsigned char *buffer;
			GC gc;
#ifdef HAVE_XRENDER
			Pixmap alphaPixmap;
			Picture windowPicture;
			Picture alphaPicture;

			/**
			 * Composite
			 */
			inline virtual void composite() const
			{
				XRenderComposite(
					app->getDisplay(),
					PictOpOver,
					alphaPicture,
					None,
					windowPicture,
					0,
					0,
					0,
					0,
					0,
					0,
					width,
					height );
			}
#endif

			virtual void show();
			virtual void hide() const;
			virtual void clear();
			virtual void update() const;
	};
}

#endif

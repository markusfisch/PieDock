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
#ifndef _PieDock_PieMenuWindow_
#define _PieDock_PieMenuWindow_

#include "TransparentWindow.h"
#include "PieMenu.h"
#include "Text.h"
#include "Cartouche.h"

#include <time.h>

namespace PieDock
{
	/**
	 * This object realizes the pie dock on screen
	 *
	 * @author Markus Fisch <mf@markusfisch.de>
	 */
	class PieMenuWindow : public TransparentWindow
	{
		public:
			enum Placement
			{
				AroundCursor,
				IconBelowCursor
			};

			PieMenuWindow( Application & );
			virtual ~PieMenuWindow();
			bool appear( std::string = "", Placement = AroundCursor );
			void draw();
			bool processEvent( XEvent & );

		protected:
			typedef std::map<int, Cartouche *> CartoucheMap;

			virtual void show( Placement = AroundCursor );
			virtual void hide();
			virtual void updateWithText();
			virtual bool performAction( Settings::Action );
			virtual void place( Placement );

		private:
			static const std::string ShowMessage;
			PieMenu menu;
			Pixmap textCanvas;
			Text *text;
			CartoucheMap cartoucheMap;
	};
}

#endif

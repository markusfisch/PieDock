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
#ifndef _PieDock_Menu_
#define _PieDock_Menu_

#include "Application.h"
#include "Settings.h"
#include "Icon.h"
#include "MenuItems.h"

#include <string>

namespace PieDock
{
	/**
	 * Abstract menu
	 *
	 * @author Markus Fisch <mf@markusfisch.de>
	 */
	class Menu
	{
		public:
			Menu( Application * );
			virtual ~Menu() {}
			inline void setWindowBelowCursor( Window w ) {
				windowBelowCursor = w; }
			inline const Window &getWindowBelowCursor() const {
				return windowBelowCursor; }
			virtual bool update( std::string );
			virtual void draw( int, int ) {}
			virtual bool change( Settings::Action = Settings::Launch );
			virtual void execute( Settings::Action = Settings::Launch );
			virtual std::string getItemTitle() const;
			inline MenuItem *getSelected() const {
				return selected; }

		protected:
			Application *app;
			MenuItem *selected;
			MenuItems *menuItems;
			Window windowBelowCursor;

			virtual int run( std::string ) const;
	};
}

#endif

/*
 *   O         ,-
 *  ° o    . -´  '     ,-
 *   °  .´        ` . ´,´
 *     ( °   ))     . (
 *      `-;_    . -´ `.`.
 *          `._'       ´
 *
 * Copyright (c) 2007-2012 Markus Fisch <mf@markusfisch.de>
 *
 * Licensed under the MIT license:
 * http://www.opensource.org/licenses/mit-license.php
 */
#ifndef _PieDock_MenuItem_
#define _PieDock_MenuItem_

#include "WindowStack.h"
#include "Icon.h"

namespace PieDock
{
	/**
	 * Menu item
	 *
	 * @author Markus Fisch <mf@markusfisch.de>
	 */
	class MenuItem : public WindowStack
	{
		public:
			MenuItem( std::string, std::string, bool = true );
			MenuItem( Icon * );
			virtual ~MenuItem() {}
			virtual inline const std::string &getTitle() const {
				return title; }
			virtual inline void setTitle( std::string t ) { title = t; }
			virtual inline const std::string &getCommand() const {
				return command; }
			virtual inline void setCommand( std::string c ) { command = c; }
			virtual inline Icon *getIcon() const { return icon; }
			virtual inline void setIcon( Icon *i ) { icon = i; }
			virtual inline const bool isSticky() const { return sticky; }
			virtual inline void makeSticky() { sticky = true; }

		private:
			std::string title;
			std::string command;
			Icon *icon;
			bool sticky;
	};
}

#endif

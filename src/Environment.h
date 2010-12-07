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
#ifndef _PieDock_Environment_
#define _PieDock_Environment_

#include <string>

namespace PieDock
{
	/**
	 * Environment
	 *
	 * @author Markus Fisch <mf@markusfisch.de>
	 */
	class Environment
	{
		public:
			virtual ~Environment() {}
			static const std::string getHome();

		private:
			Environment() {}
	};
}

#endif

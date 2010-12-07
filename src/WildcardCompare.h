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
#ifndef _PieDock_WildardCompare_
#define _PieDock_WildardCompare_

namespace PieDock
{
	/**
	 * Compare strings with wildcards
	 *
	 * @author Markus Fisch <mf@markusfisch.de>
	 */
	class WildcardCompare
	{
		public:
			virtual ~WildcardCompare() {}
			static const bool match( const char *, const char *, 
				bool = false );

		private:
			WildcardCompare() {}
	};
}

#endif

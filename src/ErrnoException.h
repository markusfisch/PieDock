/*
 *   O         ,-
 *  ° o    . -´  '     ,-
 *   °  .´        ` . ´,´
 *     ( °   ))     . (
 *      `-;_    . -´ `.`.
 *          `._'       ´
 *
 * Copyright (c) 2012 Markus Fisch <mf@markusfisch.de>
 *
 * Licensed under the MIT license:
 * http://www.opensource.org/licenses/mit-license.php
 */
#ifndef _PieDock_ErrnoException_
#define _PieDock_ErrnoException_

#include <errno.h>
#include <string.h>

#include <stdexcept>

namespace PieDock
{
	/**
	 * Run time exception initialized to error string from errno
	 *
	 * @author Markus Fisch <mf@markusfisch.de>
	 */
	class ErrnoException : public std::runtime_error
	{
		public:
			ErrnoException() : runtime_error( strerror( errno ) ) {}
	};
}

#endif

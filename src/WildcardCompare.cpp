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
#include "WildcardCompare.h"

#include <string.h>

using namespace PieDock;

/**
 * Returns if pattern matches string
 *
 * @param literal - string to match
 * @param pattern - pattern with optional wildcard characters (*?)
 * @param caseSensitive - true if matching should be case-sensitive (optional)
 */
const bool WildcardCompare::match(
	const char *literal,
	const char *pattern,
	bool caseSensitive )
{
	for( ; *pattern; )
	{
		switch( *pattern )
		{
			case '*':
				{
					const char *match = pattern;

					while( *match == '*' || *match == '?' )
						++match;

					if( !*match )
						return true;

					const char *wildcard;
					char save;
					size_t length;

					if( (length = strcspn( match, "*?" )) )
					{
						wildcard = match+length;
						save = *wildcard;
						*((char *) wildcard) = 0;
					}

					const char *last = NULL;
					const char *pos;

					if( caseSensitive )
						for( pos = literal;
							(pos = strstr( pos, match ));
							++pos )
							last = pos;
					else
						for( pos = literal;
							(pos = strcasestr( pos, match ));
							++pos )
							last = pos;

					if( wildcard )
						*((char *) wildcard) = save;

					if( !last )
						return false;

					literal = last+length;

					if( !wildcard )
					{
						if( *literal )
							return false;

						return true;
					}

					pattern = wildcard;
				}
				break;
			case '?':
				if( !*(literal++) )
					return false;
				++pattern;
				break;
			default:
				if( !*literal )
					return false;

				if( caseSensitive )
				{
					if( *pattern != *literal )
						return false;
				}
				else
				{
					register unsigned char p = *pattern;
					register unsigned char l = *literal;

					if( p > 64 && p < 91 )
						p += 32;
					if( l > 64 && l < 91 )
						l += 32;

					if( l != p )
						return false;
				}

				++literal;
				++pattern;
				break;
		}
	}

	if( *literal )
		return false;

	return true;
}

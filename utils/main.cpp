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
#include "Utilities.h"

#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <libgen.h>
#include <stdlib.h>

#include <iostream>
#include <stdexcept>

/**
 * Process entry
 *
 * @param argc - number of arguments
 * @param argv - pointer to pointers of arguments
 */
int main( int argc, char **argv )
{
	try
	{
		PieDock::Utilities u;

		for( char *binary = *argv; --argc; )
			if( **++argv == '-' )
				switch( *((*argv)+1) )
				{
					default:
						std::cerr << "Skipping unknown flag '" <<
							*((*argv)+1) << "'" << std::endl;
						break;
					case 'l':
						u.listWindows();
						exit( 0 );
					case '?':
					case 'h':
						std::cout <<
							"usage: " << basename( binary ) <<
							" [vhlcdikmors]" << std::endl <<
							"\t-v      print version and exit" << std::endl <<
							"\t-h      show this help and exit" << std::endl <<
							"\t-l      list windows and exit" << std::endl <<
							"\t-c WID  close window" << std::endl <<
							"\t-d WID  shade window" << std::endl <<
							"\t-i WID  iconify window" << std::endl <<
							"\t-m WID  maximize window" << std::endl <<
							"\t-o WID  lower window" << std::endl <<
							"\t-r WID  raise window" << std::endl <<
							"\t-s WID  make window sticky" << std::endl;
						exit( 0 );
					case 'v':
						std::cout <<
							basename( binary ) << " 1.0.2" <<
							std::endl <<
							"Copyright (c) 2007-2011" <<
							std::endl <<
							"Markus Fisch <mf@markusfisch.de>" <<
							std::endl <<
							std::endl <<
							"Licensed under the MIT license:" <<
							std::endl <<
							"http://www.opensource.org/licenses/mit-license.php" <<
							std::endl;
						exit( 0 );
					case 'c':
					case 'd':
					case 'f':
					case 'i':
					case 'm':
					case 'o':
					case 'r':
					case 's':
						if( !--argc )
							throw std::invalid_argument( "Missing argument" );
						else
						{
							PieDock::Utilities::Action a;

							switch( *((*argv)+1) )
							{
								case 'c':
									a = PieDock::Utilities::Close;
									break;
								case 'd':
									a = PieDock::Utilities::Shade;
									break;
								case 'f':
									a = PieDock::Utilities::Fullscreen;
									break;
								case 'i':
									a = PieDock::Utilities::Iconify;
									break;
								case 'm':
									a = PieDock::Utilities::Maximize;
									break;
								case 'o':
									a = PieDock::Utilities::Lower;
									break;
								case 'r':
									a = PieDock::Utilities::Raise;
									break;
								case 's':
									a = PieDock::Utilities::Stick;
									break;
							}

							u.execute(
								a,
								static_cast<Window>( atof( *++argv ) ) );
						}
						break;
				}
			else
				std::cerr << "Skipping unknown argument \"" <<
					*argv << "\"" << std::endl;

		return 0;
	}
	catch( std::exception &e )
	{
		std::cerr << "error: " << e.what() << std::endl;

		return -1;
	}
}

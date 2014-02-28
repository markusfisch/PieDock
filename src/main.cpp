/*
 *   O         ,-
 *  � o    . -�  '     ,-
 *   �  .�        ` . �,�
 *     ( �   ))     . (
 *      `-;_    . -� `.`.
 *          `._'       �
 *
 * Copyright (c) 2007-2012 Markus Fisch <mf@markusfisch.de>
 *
 * Licensed under the MIT license:
 * http://www.opensource.org/licenses/mit-license.php
 */
#ifdef HAVE_KDE
#include <QApplication>
#endif

#include "Application.h"

#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <libgen.h>
#include <stdlib.h>

#ifdef HAVE_GTK
#include <gtk/gtk.h>
#endif

#include <iostream>
#include <stdexcept>

bool stop = false;

/**
 * Signal handler
 *
 * @param id - signal id
 */
void signalHandler( int id )
{
	switch( id )
	{
		case SIGCHLD:
			// more than one process may have been terminated
			while( waitpid( -1, 0, WNOHANG | WUNTRACED ) > 0 );
			break;
		case SIGHUP:
		case SIGINT:
		case SIGTERM:
			stop = true;
			break;
	}

	return;
}

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
#ifdef HAVE_GTK
		gtk_init( &argc, &argv );
#endif

#ifdef HAVE_KDE
		QApplication q( argc, argv );
#endif

		PieDock::Settings settings;
		char *menuName = 0;

		// parse arguments
		{
			char *binary = basename( *argv );

			while( --argc )
				if( **++argv == '-' )
					switch( *((*argv)+1) )
					{
						default:
							std::cerr << "Skipping unknown flag '" <<
								*((*argv)+1) << "'" << std::endl;
							break;
						case '?':
						case 'h':
							std::cout <<
								binary << " [hvrm]" << std::endl <<
								"\t-h         this help" << std::endl <<
								"\t-v         show version" << std::endl <<
								"\t-r FILE    path and name of alternative " <<
									"configuration file" << std::endl <<
								"\t-m [MENU]  show already running " <<
									"instance" << std::endl;
							return 0;
						case 'v':
							std::cout <<
								binary << " 1.6.4" <<
								std::endl <<
								"Copyright (c) 2007-2012" <<
								std::endl <<
								"Markus Fisch <mf@markusfisch.de>" <<
								std::endl <<
								std::endl <<
								"Tatiana Azundris <hacks@azundris.com>" <<
								std::endl <<
								"* Modifier masks for key control" <<
								std::endl <<
								std::endl <<
								"Jonas Gehring <jonas.gehring@boolsoft.org>" <<
								std::endl <<
								"* Custom button actions for menus and icons" <<
								std::endl <<
								"* Better tokenization of settings statements" <<
								std::endl <<
								std::endl <<
								"Licensed under the MIT license:" <<
								std::endl <<
								"http://www.opensource.org/licenses/mit-license.php" <<
								std::endl;
							return 0;
						case 'r':
							if( !--argc )
								throw std::invalid_argument(
									"missing FILE argument" );
							settings.setConfigurationFile( *++argv );
							break;
						case 'm':
							if( argc > 1 &&
								**(argv+1) != '-' )
							{
								--argc;
								menuName = *++argv;
							}
							break;
					}
				else
					std::cerr << "skipping unknown argument \"" <<
						*argv << "\"" << std::endl;

			if( settings.getConfigurationFile().empty() )
				settings.setConfigurationFileFromBinary( binary );
		}

		switch( fork() )
		{
			default:
				// terminate parent process to detach from shell
				return 0;
			case 0:
				// pursue in child process
				break;
			case -1:
				throw std::runtime_error( "cannot fork" );
		}

		// always open display after fork
		PieDock::Application a( settings );

		// if another instance is already running, wake it
		if( a.remote( menuName ) )
			return 0;

		// obtain new process group
		setsid();

		signal( SIGCHLD, signalHandler );
		signal( SIGHUP, signalHandler );
		signal( SIGINT, signalHandler );
		signal( SIGTERM, signalHandler );

#ifdef HAVE_KDE
		int r = a.run( &stop );
		q.quit();
		return r;
#else
		return a.run( &stop );
#endif
	}
	catch( std::exception &e )
	{
		std::cerr << "error: " << e.what() << std::endl;

		return -1;
	}
}

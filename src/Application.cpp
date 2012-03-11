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
#include "Application.h"
#include "Settings.h"
#include "PieMenuWindow.h"

#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include <libgen.h>
#include <stdlib.h>
#include <sstream>

using namespace PieDock;

const char Application::StopMarker = '\n';
const char *Application::Show = "show";

/**
 * Initialize application
 *
 * @param s - settings object
 */
Application::Application( Settings &s ) :
	display( XOpenDisplay( 0 ) ),
	root( DefaultRootWindow( display ) ),
	settings( &s ),
	suspend( StandBy )
{
	if( !display )
		throw "cannot open display";

	socketFile =
		s.getConfigurationFile()+
		std::string( "-socket" );
}

/**
 * Clean up
 */
Application::~Application()
{
	XCloseDisplay( display );
}

/**
 * Try to forward request to an already running instance; returns true
 * if successful
 *
 * @param menu - name of menu to open
 */
bool Application::remote( const char *menu ) const
{
	struct stat buf;

	if( stat( socketFile.c_str(), &buf ) < 0 )
		return false;

	struct sockaddr_un address;
	int s;

	if( (s = socket( PF_UNIX, SOCK_DGRAM, 0 )) < 0 )
		throw strerror( errno );

	memset( &address, 0, sizeof( struct sockaddr_un ) );

	address.sun_family = AF_LOCAL;
	strncpy(
		reinterpret_cast<char *>( address.sun_path ),
		socketFile.c_str(),
		UnixPathMax );

	if( connect( s,
		(struct sockaddr *) &address,
		sizeof( struct sockaddr_un ) ) < 0 )
	{
		// if there's no listener assume the file has been left
		// over from a previous instance and try to remove it
		// to start anew
		if( errno != ECONNREFUSED ||
			unlink( socketFile.c_str() ) )
			throw strerror( errno );

		return false;
	}

	// send command
	{
		std::string cmd = Show;

		if( menu )
			cmd += std::string( " " )+menu;

		cmd += StopMarker;

		if( send( s, cmd.c_str(), cmd.size(), 0 ) < 0 )
			throw strerror( errno );

		close( s );
	}

	return true;
}

/**
 * Run application
 *
 * @param stopFlag - pointer to stop flag
 */
int Application::run( bool *stopFlag )
{
	int xfd = ConnectionNumber( display );
	int s = 0;

	// at first, load settings
	settings->load( display );

	// create socket for external activation
	{
		struct sockaddr_un address;

		if( (s = socket( PF_UNIX, SOCK_DGRAM, 0 )) < 0 )
			throw strerror( errno );

		memset( &address, 0, sizeof( struct sockaddr_un ) );

		address.sun_family = AF_LOCAL;
		strncpy(
			reinterpret_cast<char *>( address.sun_path ),
			socketFile.c_str(),
			UnixPathMax );

		if( bind( s,
			(struct sockaddr *) &address,
			sizeof( struct sockaddr_un ) ) < 0 )
			throw strerror( errno );
	}

	grabTriggers();

	for( PieMenuWindow w( *this ); !*stopFlag; )
	{
		if( !XPending( display ) )
		{
			fd_set rfds;
			struct timeval tv, *ptv = 0;

			FD_ZERO( &rfds );
			FD_SET( xfd, &rfds );
			FD_SET( s, &rfds );

			if( suspend > 0 )
			{
				tv.tv_sec = 0;
				tv.tv_usec = suspend;
				ptv = &tv;
			}

			// wait for descriptors to become readable
			{
				int highest = (s > xfd ? s : xfd)+1;
				int hits;

				if( (hits = select( highest, &rfds, 0, 0, ptv )) < 0 )
				{
					// signal caught
					if( errno == EINTR )
						continue;

					// some descriptor has been closed
					break;
				}
				else if( !hits )
				{
					// timeout
					if( suspend == Active )
						w.draw();

					continue;
				}
				else
				{
					// some descriptor has become readable
					if( FD_ISSET( s, &rfds ) )
					{
						std::string message;

						// read from socket, don't do that byte by byte
						// as you would do with a network socket, file
						// sockets will block after a recv() no matter
						// how many bytes recv requested from the buffer
						{
							char m[0xff];

							bzero( m, sizeof( m ) );

							if( (recv( s, m, sizeof( m ), 0 )) < 0 )
								continue;

							message = m;
						}

						if( !message.find( Show ) &&
							suspend == StandBy )
						{
							std::string menuName = "";

							// get menu name
							{
								std::string::size_type p;

								if( (p = message.find( StopMarker )) !=
										std::string::npos )
									message.erase( p );

								if( (p = message.find( ' ' )) !=
										std::string::npos )
									menuName = message.substr( ++p );
							}

							// why? see below
							ungrabTriggers();

							if( w.appear(
									menuName,
									PieMenuWindow::AroundCursor ) )
								suspend = Active;
							else
								grabTriggers();
						}
					}

					if( !FD_ISSET( xfd, &rfds ) )
						continue;
				}
			}
		}

		XEvent event;

		bzero( &event, sizeof( event ) );
		XNextEvent( display, &event );

		if( suspend == StandBy &&
			event.xany.window == root &&
			(event.type == ButtonPress ||
				event.type == ButtonRelease ||
				event.type == KeyPress) )
		{
			// valid? read below, see button event masks
			bool valid = false;

			// find menu name from trigger
			std::string menuName = "";

			if( event.type == KeyPress )
			{
				for( Settings::Keys::iterator i = settings->getKeys().begin();
					i != settings->getKeys().end();
					++i )
					if( XKeysymToKeycode( event.xany.display, (*i).keySym ) ==
							event.xkey.keycode &&
						(
							// AnyModifier (32768 in X.h) cannot be used
							// for binary operations
							(*i).modifier == AnyModifier ||
							((*i).modifier & event.xkey.state)
						) )
					{
						valid = true;
						menuName = (*i).menuName;
						break;
					}
			}
			else
			{
				for( Settings::Buttons::iterator i =
						settings->getButtons().begin();
					i != settings->getButtons().end();
					++i )
					if( (*i).button == event.xbutton.button &&
						(
							// AnyModifier (32768 in X.h) cannot be used
							// for binary operations
							(*i).modifier == AnyModifier ||
							((*i).modifier & event.xbutton.state)
						) &&
						// on some implementations of X, for example
						// X.Org X 1.6.3.901 (1.6.4 RC1) 64bit, we receive
						// ButtonPress events even when XGrabButton had only
						// ButtonReleaseMask selected which causes all
						// menus to appear on a button press;
						// hence the "valid" flag
						(
							((*i).eventMask == ButtonPressMask &&
								event.type == ButtonPress) ||
							((*i).eventMask == ButtonReleaseMask &&
								event.type == ButtonRelease)
						) )
					{
						valid = true;
						menuName = (*i).menuName;
						break;
					}
			}

			if( valid )
			{
				// ungrab triggers to avoid being triggered again while
				// already being visible; since receiving a new trigger
				// event will be preceded by a LeaveWindowEvent in
				// PieMenuWindow which will cause the menu to disappear
				// again, it's best to simply ungrab the triggers
				ungrabTriggers();

				if( w.appear(
						menuName,
						(event.type == KeyPress ?
							PieMenuWindow::IconBelowCursor :
							PieMenuWindow::AroundCursor) ) )
					suspend = Active;
				else
					grabTriggers();
			}
		}
		else if( suspend == Active &&
			!w.processEvent( event ) )
		{
			suspend = StandBy;
			grabTriggers();
		}
	}

	ungrabTriggers();
	close( s );
	unlink( socketFile.c_str() );

	return 0;
}

/**
 * Grab triggers
 */
void Application::grabTriggers()
{
	// buttons
	for( Settings::Buttons::iterator i = settings->getButtons().begin();
		i != settings->getButtons().end();
		++i )
		XGrabButton(
			display,
			(*i).button,
			(*i).modifier,
			root,
			True,
			(*i).eventMask,
			GrabModeAsync,
			GrabModeAsync,
			None,
			None );

	// keys
	for( Settings::Keys::iterator i = settings->getKeys().begin();
		i != settings->getKeys().end();
		++i )
		XGrabKey(
			display,
			XKeysymToKeycode( display, (*i).keySym ),
			(*i).modifier,
			root,
			True,
			GrabModeAsync,
			GrabModeAsync );
}

/**
 * Ungrab triggers
 */
void Application::ungrabTriggers()
{
	// buttons
	for( Settings::Buttons::iterator i = settings->getButtons().begin();
		i != settings->getButtons().end();
		++i )
		XUngrabButton(
			display,
			(*i).button,
			(*i).modifier,
			root );

	// keys
	for( Settings::Keys::iterator i = settings->getKeys().begin();
		i != settings->getKeys().end();
		++i )
		XUngrabKey(
			display,
			XKeysymToKeycode( display, (*i).keySym ),
			(*i).modifier,
			root );
}

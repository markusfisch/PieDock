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
#include "Utilities.h"
#include "../src/WindowManager.h"

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <iostream>

using namespace PieDock;

/**
 * Initialize
 */
Utilities::Utilities() :
	display( XOpenDisplay( 0 ) )
{
}

/**
 * Clean up
 */
Utilities::~Utilities()
{
	XCloseDisplay( display );
}

/**
 * List windows to stdout
 */
void Utilities::listWindows()
{
	WindowManager::WindowList wl( display );

	for( WindowManager::WindowList::iterator i = wl.begin();
		i != wl.end();
		i++ )
	{
		XClassHint xch;

		if( !XGetClassHint( display, (*i), &xch ) )
			continue;

		std::cout <<
			"Window 0x" << std::hex << (*i) << std::endl <<
			"\tTitle: \"" << WindowManager::getTitle(
				display, (*i) ) << "\"" << std::endl <<
			"\tName: \"" << xch.res_name << "\"" << std::endl <<
			"\tClass: \"" << xch.res_class << "\"" << std::endl;
	}
}

/**
 * Execute action
 *
 * @param a - action to execute on given window
 * @param w - window
 */
void Utilities::execute( Utilities::Action a, Window w )
{
	if( !w )
		return;

	switch( a )
	{
		case Close:
			WindowManager::close( display, w );
			break;
		case Iconify:
			XIconifyWindow( display, w, DefaultScreen( display ) );
			break;
		case Maximize:
			WindowManager::sendClientMessage(
				display,
				w, 
				"_NET_WM_STATE",
				static_cast<unsigned long>( StateToggle ),
				static_cast<unsigned long>( WindowManager::getAtom( 
					display, 
					"_NET_WM_STATE_MAXIMIZED_VERT" ) ),
				static_cast<unsigned long>( WindowManager::getAtom( 
					display, 
					"_NET_WM_STATE_MAXIMIZED_HORZ" ) ) );
			break;
		case Fullscreen:
			WindowManager::sendClientMessage(
				display,
				w, 
				"_NET_WM_STATE",
				static_cast<unsigned long>( StateToggle ),
				static_cast<unsigned long>( WindowManager::getAtom( 
					display, 
					"_NET_WM_STATE_FULLSCREEN" ) ) );
			break;
/* for some reason this will kill the whole X server when called out of 
   PieDock; when called manually from aterm it works ?! xkill shows the
   same behaviour so the error may lie somewhere in PieDock::Menu::run()
		case Kill:
			XSync( display, 0 );
			XKillClient( display, w );
			XSync( display, 0 );
			break; */
		case Lower:
			XLowerWindow( display, w );
			break;
		case Raise:
			XRaiseWindow( display, w );
			break;
		case Shade:
			WindowManager::sendClientMessage(
				display,
				w, 
				"_NET_WM_STATE",
				static_cast<unsigned long>( StateToggle ),
				static_cast<unsigned long>( WindowManager::getAtom( 
					display, 
					"_NET_WM_STATE_SHADED" ) ) );
			break;
		case Stick:
			WindowManager::sendClientMessage(
				display,
				w, 
				"_NET_WM_STATE",
				static_cast<unsigned long>( StateToggle ),
				static_cast<unsigned long>( WindowManager::getAtom( 
					display, 
					"_NET_WM_STATE_STICKY" ) ) );
			break;
	}
}

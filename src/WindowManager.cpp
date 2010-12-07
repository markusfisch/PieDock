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
#include "WindowManager.h"

#include <X11/Xutil.h>

using namespace PieDock;

WindowManager::StringToAtom WindowManager::stringToAtom;

/**
 * Add client of given display
 *
 * @param d - display
 */
void WindowManager::WindowList::addClientsOf( Display *d )
{
	Property<Window> p( d, DefaultRootWindow( d ) );

	if( !p.fetch( XA_WINDOW, "_NET_CLIENT_LIST" ) &&
		!p.fetch( XA_CARDINAL, "_WIN_CLIENT_LIST" ) )
		return;

	Window *w = p.getData();

	for( int i = p.getItems(); i--; )
		push_back( *w++ );
}

/**
 * Activate, raise and switch to the workspace of some window
 *
 * @param d - display
 * @param w - window id
 */
void WindowManager::activate( Display *d, Window w )
{
	// switch to workspace
	{
		unsigned long workspace = getWorkspace( d, w );
		Window root = DefaultRootWindow( d );
		Property <unsigned long> p( d, root );

		if( (p.fetch( XA_CARDINAL, "_NET_CURRENT_DESKTOP" ) ||
				p.fetch( XA_CARDINAL, "_WIN_WORKSPACE" )) &&
			workspace != *p.getData() )
			sendClientMessage(
				d,
				root,
				"_NET_CURRENT_DESKTOP",
				workspace );
	}

	sendClientMessage( d, w, "_NET_ACTIVE_WINDOW" );
	XMapRaised( d, w );

	// wait until the window becomes viewable before setting input focus;
	// abort if the window doesn't become viewable within roughly 2 seconds
	for( time_t start = time( 0 )+2;
		start > time( 0 );
		usleep( 1000 ) )
	{
		XWindowAttributes wa;

		XGetWindowAttributes( d, w, &wa );

		if( wa.map_state != IsViewable )
			continue;

		XSetInputFocus( d, w, RevertToPointerRoot, CurrentTime );
		break;
	}
}

/**
 * Iconify some window
 *
 * @param d - display
 * @param w - window id
 */
void WindowManager::iconify( Display *d, Window w )
{
	XIconifyWindow( d, w, DefaultScreen( d ) );
}

/**
 * Close some window
 *
 * @param d - display
 * @param w - window id
 */
void WindowManager::close( Display *d, Window w )
{
	sendClientMessage( d, w, "_NET_CLOSE_WINDOW" );
}

/**
 * Return currently active window
 *
 * @param d - display
 */
Window WindowManager::getActive( Display *d )
{
	Property<Window> p( d, DefaultRootWindow( d ) );

	if( !p.fetch( XA_WINDOW, "_NET_ACTIVE_WINDOW" ) )
		return 0;

	return *p.getData();
}

/**
 * Return (visible) title of some window
 *
 * @param d - display
 * @param w - window id
 */
std::string WindowManager::getTitle( Display *d, Window w )
{
	Property<char> p( d, w );

	if( !p.fetch( getAtom( d, "UTF8_STRING" ), "_NET_WM_NAME" ) &&
		!p.fetch( XA_STRING, "WM_NAME" ) )
		return "";

	return p.getData();
}

/**
 * Return workspace of window
 *
 * @param d - display
 * @param w - window id
 */
unsigned long WindowManager::getWorkspace( Display *d, Window w )
{
	Property<unsigned long> p( d, w );

	if( p.fetch( XA_CARDINAL, "_NET_WM_DESKTOP" ) ||
		p.fetch( XA_CARDINAL, "_WIN_WORKSPACE" ) )
		return *p.getData();

	return 0;
}

/**
 * Return number of workspaces
 *
 * @param d - display
 */
unsigned long WindowManager::getNumberOfWorkspaces( Display *d )
{
	Property<unsigned long> p( d, DefaultRootWindow( d ) );

	if( p.fetch( XA_CARDINAL, "_NET_NUMBER_OF_DESKTOPS" ) ||
		p.fetch( XA_CARDINAL, "_WIN_WORKSPACE_COUNT" ) )
		return *p.getData();

	return 0;
}

/**
 * Returns true if given window is not a panel or dock or something like that
 *
 * @param d - display
 * @param w - window id
 */
bool WindowManager::isNormalWindow( Display *d, Window w )
{
	Property<Atom> p( d, w );

	if(	p.fetch( XA_ATOM, "_NET_WM_WINDOW_TYPE" ) )
		for( int n = p.getItems(); n > 0; --n )
			if( *p.getData() == getAtom( d, "_NET_WM_WINDOW_TYPE_NORMAL" ) )
				break;
			else if( *p.getData() == getAtom( d, "_NET_WM_WINDOW_TYPE_SPLASH" ) ||
				*p.getData() == getAtom( d, "_NET_WM_WINDOW_TYPE_DOCK" ) ||
				*p.getData() == getAtom( d, "_NET_WM_WINDOW_TYPE_TOOLBAR" ) ||
				*p.getData() == getAtom( d, "_NET_WM_WINDOW_TYPE_DESKTOP" ) )
				return false;

	return true;
}

/**
 * Set window type
 *
 * @param d - display
 * @param w - target window
 * @param t - type
 */
void WindowManager::setWindowType( Display *d, Window w, const char *type )
{
	Atom a[2];
	int n = 0;

	a[n++] = getAtom( d, type );	

	XChangeProperty(
		d,
		w,
		getAtom( d, "_NET_WM_WINDOW_TYPE" ),
		XA_ATOM,
		32,
		PropModeReplace,
		reinterpret_cast<unsigned char *>( &a ),
		n );
}

/**
 * Send a ClientMessage to some window
 *
 * @param d - display
 * @param w - target window
 * @param message - message to send
 * @param data0 - data (optional)
 * @param data1 - data (optional)
 * @param data2 - data (optional)
 * @param data3 - data (optional)
 * @param data4 - data (optional)
 */
void WindowManager::sendClientMessage(
	Display *d,
	Window w,
	const char *message,
	unsigned long data0,
	unsigned long data1,
	unsigned long data2,
	unsigned long data3,
	unsigned long data4 )
{
	XEvent event;
	long mask = SubstructureRedirectMask | SubstructureNotifyMask;

	event.xclient.type = ClientMessage;
	event.xclient.serial = 0;
	event.xclient.send_event = True;
	event.xclient.message_type = getAtom( d, message );
	event.xclient.window = w;
	event.xclient.format = 32;
	event.xclient.data.l[0] = data0;
	event.xclient.data.l[1] = data1;
	event.xclient.data.l[2] = data2;
	event.xclient.data.l[3] = data3;
	event.xclient.data.l[4] = data4;

	XSendEvent(
		d,
		w,
		False,
		mask,
		&event );
}

/**
 * Get X internal atom
 *
 * @param d - display
 * @param n - name of atom
 */
Atom WindowManager::getAtom( Display *d, const char *n )
{
	StringToAtom::iterator i;

	if( (i = stringToAtom.find( n )) != stringToAtom.end() )
		return (*i).second;

	return (stringToAtom[n] = XInternAtom( d, n, False ));
}

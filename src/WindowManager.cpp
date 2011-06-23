/*
 *   O         ,-
 *  ° o    . -´  '     ,-
 *   °  .´        ` . ´,´
 *     ( °   ))     . (
 *      `-;_    . -´ `.`.
 *          `._'       ´
 *
 * Copyright (c) 2007-2011 Markus Fisch <mf@markusfisch.de>
 *
 * Licensed under the MIT license:
 * http://www.opensource.org/licenses/mit-license.php
 */
#include "WindowManager.h"
#include "ArgbSurface.h"

#include <X11/Xutil.h>

#ifdef HAVE_XMU
#include <X11/Xmu/WinUtil.h>
#endif

#include <stdint.h>
#include <string.h>

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
	Window root = DefaultRootWindow( d );

	// switch to workspace
	{
		Property <unsigned long> p( d, root );

		if( p.fetch( XA_CARDINAL, "_NET_NUMBER_OF_DESKTOPS" ) &&
			*p.getData() == 1 )
		{
			XWindowAttributes desktop;
			XWindowAttributes window;

			// get desktop geometry, can't use _NET_WORKAREA here because
			// it returns the geometry minus dock windows; nor does
			// _NET_DESKTOP_GEOMETRY fit because it returns the dimensions
			// of the large desktop
			if( XGetWindowAttributes( d, root, &desktop ) &&
				XGetWindowAttributes( d, w, &window ) )
			{
				Window dummy;

				// get position of window relative to the current viewport
				XTranslateCoordinates(
					d,
					w,
					window.root,
					-window.border_width,
					-window.border_width,
					&window.x,
					&window.y,
					&dummy );

				if( (window.x < 0 ||
						window.y < 0 ||
						window.x > desktop.width ||
						window.y > desktop.height) &&
					p.fetch( XA_CARDINAL, "_NET_DESKTOP_VIEWPORT" ) )
				{
					struct
					{
						unsigned long x;
						unsigned long y;
					} view = { p.getData()[0], p.getData()[1] };

					if( p.fetch( XA_CARDINAL, "_NET_DESKTOP_GEOMETRY" ) )
					{
						if( window.x < 0 &&
							!view.x )
							window.x += p.getData()[0];

						if( window.y < 0 &&
							!view.y )
							window.y += p.getData()[1];
					}

					sendClientMessage(
						d,
						root,
						"_NET_DESKTOP_VIEWPORT",
						// this coordinates need to be a multiple of
						// of the root window geometry and NOT of the
						// workspace geometry
						(window.x+view.x)/desktop.width*desktop.width,
						(window.y+view.y)/desktop.height*desktop.height,
						CurrentTime );
				}
			}
		}
		else
		{
			unsigned long workspace = getWorkspace( d, w );

			if( (p.fetch( XA_CARDINAL, "_NET_CURRENT_DESKTOP" ) ||
					p.fetch( XA_CARDINAL, "_WIN_WORKSPACE" )) &&
				workspace != *p.getData() )
				sendClientMessage(
					d,
					root,
					"_NET_CURRENT_DESKTOP",
					workspace );
		}
	}

	sendClientMessage(
		d,
		w,
		"_NET_ACTIVE_WINDOW",
		2L,
		CurrentTime );

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
		XRaiseWindow( d, w );
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
 * Get client/content window from a possible frame/decor window
 *
 * @param d - display
 * @param w - window
 */
Window WindowManager::getClientWindow( Display *d, Window w )
{
	Window root;
	int i;
	unsigned int u;

	if( XGetGeometry( d, w, &root, &i, &i, &u, &u, &u, &u ) &&
		w != root )
	{
#ifdef HAVE_XMU
		w = XmuClientWindow( d, w );
#else
		/**
		 * Private class to stand in if XmuClientWindow is missing;
		 * didn't like the thought of having a depedency for this
		 */
		class WindowDiver
		{
			public:
				WindowDiver( Display *d ) : display( d ) {}
				virtual ~WindowDiver() {}
				Window find( Window window )
				{
					if( hasWmState( window ) )
						return window;

					Window w = checkChildren( window );
					return w ? w : window;
				}

			private:
				Window checkChildren( Window window ) const
				{
					Window root;
					Window parent;
					Window *children;
					unsigned int n;

					if( !XQueryTree( display, window, &root, &parent,
							&children, &n ) )
						return 0;

					for( int i = 0; i < n; ++i )
						if( hasWmState( children[i] ) )
							return children[i];

					for( int i = 0; i < n; ++i )
					{
						Window w;

						if( (w = checkChildren( children[i] )) )
							return w;
					}

					return 0;
				};
				const bool hasWmState( const Window window ) const
				{
					Property<unsigned char*> p( display, window );

					return p.fetch(
						getAtom( display, "WM_STATE" ),
						"WM_STATE" );
				};

				Display *display;
		};

		WindowDiver wd( d );
		return wd.find( w );
#endif
	}

	return w;
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
 * Return icon of some window
 *
 * @param d - display
 * @param w - window id
 */
ArgbSurface *WindowManager::getIcon( Display *d, Window w )
{
	Property<unsigned long> p( d, w );

	if( !p.fetch( XA_CARDINAL, "_NET_WM_ICON", 0xffffffff ) )
		return 0;

	unsigned long *icon = 0;
	unsigned long iconSize = 0;

	// find biggest icon
	{
		unsigned long *b = p.getData();

		for( unsigned long l = p.getItems();
			l > 2; )
		{
			unsigned long s = b[0]*b[1];

			if( s > iconSize )
			{
				icon = b;
				iconSize = s;
			}

			s += 2;
			b += s;
			l -= s;
		}
	}

	ArgbSurface *s;

	if( !icon ||
		!(s = new ArgbSurface( icon[0], icon[1] )) )
		return 0;

	// copy image data, don't use memcpy here since the bytes per pixel
	// may be different from ArgbSurface's 32 bits
	{
		uint32_t *dest = reinterpret_cast<uint32_t *>( s->getData() );
		unsigned long *src = &icon[2];

		for( int y = s->getHeight(); y--; )
			for( int x = s->getWidth(); x--; )
				*dest++ = *src++ & 0xffffffff;
	}

	return s;
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
		DefaultRootWindow( d ),
		False,
		SubstructureRedirectMask | SubstructureNotifyMask,
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

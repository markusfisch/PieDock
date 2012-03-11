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
#include "WorkspaceLayout.h"
#include "WindowManager.h"

#include <math.h>

using namespace PieDock;

WorkspaceLayout *WorkspaceLayout::singleton = 0;

/**
 * Return singleton instance
 *
 * @param d - display
 * @param l - preferred layout of workspaces (optional)
 */
WorkspaceLayout *WorkspaceLayout::getWorkspaceLayout(
	Display *d,
	PreferredVirtualLayout l )
{
	if( singleton )
		return singleton;

	return (singleton = new WorkspaceLayout( d, l ));
}

/**
 * Returns true if the given window is on another workspace
 *
 * @param w - window
 * @param p - workspace position
 */
bool WorkspaceLayout::isOnAnotherWorkspace(
	Window w,
	WorkspacePosition &p )
{
	XWindowAttributes wa;

	if( XGetWindowAttributes( display, w, &wa ) )
		return isOnAnotherWorkspace( w, wa, p );

	return false;
}

/**
 * Returns true if the given window is on another workspace
 *
 * @param w - window
 * @param wa - window attributes
 * @param p - workspace position
 */
bool WorkspaceLayout::isOnAnotherWorkspace(
	Window w,
	XWindowAttributes &wa,
	WorkspacePosition &p )
{
	// get position of window relative to the current viewport
	{
		Window dummy;
		
		XTranslateCoordinates(
			display,
			w,
			wa.root,
			-wa.border_width,
			-wa.border_width,
			&p.x,
			&p.y,
			&dummy );
	}

	if( virtualDesktop )
	{
		unsigned long vx;
		unsigned long vy;

		if( (p.x < 0 ||
				p.y < 0 ||
				p.x > screen.width ||
				p.y > screen.height) &&
			WindowManager::getWorkspacePosition(
				display,
				vx,
				vy ) )
		{
			if( p.x < 0 &&
				!vx )
				p.x += total.width;
			else
				p.x += vx;				

			if( p.y < 0 &&
				!vy )
				p.y += total.height;
			else
				p.y += vy;

			return true;
		}
	}
	else
	{
		p.number = WindowManager::getWorkspace( display, w );

		if( p.number != WindowManager::getCurrentWorkspace( display ) )
		{
			switch( preferredLayout )
			{
				case Horizontal:
					p.x += p.number*screen.width;
					break;
				case Vertical:
					p.y += p.number*screen.height;
					break;
				default:
				case Square:
					p.x += p.number%columns*screen.width;
					p.y += p.number/columns*screen.height;
					break;
			}

			return true;
		}
	}

	return false;
}

/**
 * Initialize workspace geometry info
 *
 * @param d - display
 * @param l - preferred layout of workspaces
 */
WorkspaceLayout::WorkspaceLayout(
	Display *d,
	PreferredVirtualLayout l ) :
	display( d ),
	preferredLayout( l )
{
	Window root = DefaultRootWindow( d );

	// get desktop geometry, can't use _NET_WORKAREA here because
	// it returns the geometry minus dock windows; nor does
	// _NET_DESKTOP_GEOMETRY fit because it returns the dimensions
	// of the large desktop
	{
		XWindowAttributes xwa;

		if( !XGetWindowAttributes( d, root, &xwa ) )
			throw "cannot get attributes of root window";

		screen.width = xwa.width;
		screen.height = xwa.height;
	}

	// get total size of all workspaces
	{
		unsigned long n;

		if( (n = WindowManager::getNumberOfWorkspaces( d )) > 1 )
		{
			switch( l )
			{
				case Horizontal:
					total.width = n*screen.width;
					total.height = screen.height;
					break;
				case Vertical:
					total.width = screen.width;
					total.height = n*screen.height;
					break;
				default:
				case Square:
					int s = static_cast<int>( ceil( sqrt( n ) ) );
					total.width = s*screen.width;
					total.height = s*screen.height;
					break;
			}

			virtualDesktop = false;
		}
		else
		{
			if( !WindowManager::getDesktopGeometry(
				d,
				total.width,
				total.height ) )
			{
				total.width = screen.width;
				total.height = screen.height;
			}

			virtualDesktop = true;
		}
	}

	columns = total.width/screen.width;
	rows = total.height/screen.height;
}

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
#include "Menu.h"
#include "WindowManager.h"
#include "WorkspaceLayout.h"
#include "MenuItemWithWorkspaces.h"

#include <X11/Xutil.h>
#include <unistd.h>
#include <stdlib.h>
#include <sstream>

using namespace PieDock;

/**
 * Initialize menu
 *
 * @param a - application
 */
Menu::Menu( Application *a ) :
	app( a ),
	selected( 0 ),
	menuItems( 0 )
{
	openWindows.setOneIconPerWindow( true );
}

/**
 * Update menu
 *
 * @param menuName - menu name
 * @param forWindow - first window of a list of open windows that belong
 *                    to the same application (optional)
 */
bool Menu::update( std::string menuName, Window forWindow )
{
	if( !forWindow &&
		!(menuItems = app->getSettings()->getMenu( menuName )) )
		return false;

	if( forWindow )
		menuItems = &openWindows;

	name = menuName;

	// multiple windows per icon
	typedef std::map<Icon *, MenuItem *> IconToItem;
	IconToItem iconToItem;

	// one icon per window
	typedef std::map<Window, MenuItem *> WindowToItem;
	WindowToItem windowToItem;

	IconMap *iconMap = &app->getSettings()->getIconMap();

	// display workspaces
	const Settings::WorkspaceDisplaySettings wsds =
		app->getSettings()->getWorkspaceDisplaySettings();
	WorkspaceLayout *wsl = WorkspaceLayout::getWorkspaceLayout(
		app->getDisplay(),
		wsds.preferredLayout );

	// clear windows and make sure all items have valid icons
	{
		MenuItems::iterator si = menuItems->end();

		for( MenuItems::iterator i = menuItems->begin();
			i != menuItems->end();
			++i )
		{
			Icon *icon;

			if( !(icon = (*i)->getIcon()) )
			{
				if( !(icon = iconMap->getIconByName( (*i)->getTitle() )) )
					icon = iconMap->getMissingIcon( (*i)->getTitle() );

				(*i)->setIcon( icon );
			}

			if( menuItems->oneIconPerWindow() )
			{
				windowToItem[(*i)->getNextWindow()] = (*i);

				if( *i == selected )
					si = i;
			}

			iconToItem[icon] = (*i);
			(*i)->clearWindows();
		}

		// move menu item to the top when one icon per window is used
		if( si != menuItems->end() )
		{
			menuItems->erase( si );
			menuItems->push_front( selected );
		}
	}

	// get filter
	std::string classFilter;

	if( forWindow ||
		menuItems->onlyFromActive() )
	{
		Window w = forWindow ?
			forWindow :
			WindowManager::getActive( app->getDisplay() );
		XClassHint xch;

		if( w &&
			XGetClassHint( app->getDisplay(), w, &xch ) )
		{
			classFilter = xch.res_class;

			XFree( xch.res_name );
			XFree( xch.res_class );
		}
	}

	// assign windows to menu items; this is done by evaluating name, class
	// and title of the windows since you just can't trust window IDs over time
	{
		WindowManager::WindowList wl( app->getDisplay() );

		for( WindowManager::WindowList::iterator i = wl.begin();
			i != wl.end();
			++i )
		{
			if( !WindowManager::isNormalWindow( app->getDisplay(), (*i) ) )
				continue;

			XClassHint xch;

			if( !XGetClassHint( app->getDisplay(), (*i), &xch ) )
				continue;

			if( (!menuItems->oneIconPerWindow() &&
					app->getSettings()->ignoreWindow( xch.res_name )) ||
				((forWindow || menuItems->onlyFromActive()) &&
					classFilter.compare( xch.res_class )) )
			{
				XFree( xch.res_name );
				XFree( xch.res_class );

				continue;
			}

			std::string windowTitle = WindowManager::getTitle(
				app->getDisplay(),
				(*i) );

			Icon *icon = iconMap->getIcon(
				windowTitle,
				xch.res_class,
				xch.res_name );

			// handle missing icons
			if( !icon ||
				icon->getType() == Icon::Missing )
			{
				ArgbSurface *s;

				if( (s = WindowManager::getIcon( app->getDisplay(), (*i) )) )
				{
					if( icon )
					{
						icon->setSurface( s );
						icon->setType( Icon::Window );
					}
					else
						icon = iconMap->createIcon(
							s,
							xch.res_name,
							Icon::Window );

					iconMap->saveIcon( s, xch.res_name );
					delete s;
				}
				else if( !icon )
					icon = iconMap->getMissingIcon( xch.res_name );
			}

			XFree( xch.res_name );
			XFree( xch.res_class );

			if( menuItems->oneIconPerWindow() )
			{
				WindowToItem::iterator w;
				MenuItem *item;

				// try to use existing icon
				if( (w = windowToItem.find( (*i) )) != windowToItem.end() )
				{
					item = (*w).second;

					// always get icon anew when reusing a window ID
					item->setIcon( icon );
				}
				else
					menuItems->push_back(
						(item = new MenuItemWithWorkspaces( icon )) );

				item->addWindow( app->getDisplay(), (*i) );
				item->setTitle( windowTitle );

				if( wsds.visible )
				{
					MenuItemWithWorkspaces *w =
						dynamic_cast<MenuItemWithWorkspaces *>( item );

					if( w )
						w->showWorkspace(
							wsl,
							wsds.workspaceColor,
							wsds.windowColor );
				}

				continue;
			}

			// find existing icon or create a new one
			{
				IconToItem::iterator m;

				if( (m = iconToItem.find( icon )) != iconToItem.end() )
					(*m).second->addWindow( app->getDisplay(), (*i) );
				else if( menuItems->includeWindows() )
				{
					MenuItem *item = new MenuItem( icon );
					item->addWindow( app->getDisplay(), (*i) );
					item->setTitle( windowTitle );

					iconToItem[icon] = item;
					menuItems->push_back( item );
				}
			}
		}
	}

	// remove all menu items that have no windows and
	// are not sticky
	{
		MenuItems::iterator i = menuItems->begin();

		while( i != menuItems->end() )
			if( !(*i)->isSticky() &&
				!(*i)->hasWindows() )
			{
				delete (*i);
				i = menuItems->erase( i );
			}
			else
				++i;
	}

	// fill menu with dummy icons if there is a minimum number
	{
		int m = app->getSettings()->getMinimumNumber();

		if( m > 0 &&
			menuItems->size() < m )
		{
			for( m -= menuItems->size(); m--; )
				menuItems->push_back( new MenuItem(
					iconMap->getFillerIcon() ) );
		}
	}

	return true;
}

/**
 * Check if item points to another menu and if so, change to it
 *
 * @param a - action to execute (optional)
 */
bool Menu::change( Settings::Action a )
{
	if( !selected ||
		(
			a != Settings::Launch &&
			a != Settings::ShowNext &&
			a != Settings::ShowPrevious &&
			a != Settings::ShowWindows
		) )
		return false;

	std::string cmd = selected->getCommand();

	// check if this menu should launch another menu
	if( !cmd.compare( 0, 1, ":" ) )
	{
		update( cmd.substr( 1 ) );
		return true;
	}
	else if( a == Settings::ShowWindows )
	{
		// skip menu if there's only one window
		if( selected->getWindowInfos().size() < 2 )
			return false;

		update( "", selected->getNextWindow() );
		return true;
	}

	return false;
}

/**
 * Execute some action for the seleced icon
 *
 * @param a - action to execute (optional)
 */
void Menu::execute( Settings::Action a )
{
	if( !selected )
		return;

	// if there are no windows only Launch is allowed
	if( !selected->hasWindows() )
	{
		if( a != Settings::Launch &&
			a != Settings::ShowNext &&
			a != Settings::ShowPrevious &&
			a != Settings::ShowWindows )
			return;

		a = Settings::Launch;
	}

	switch( a )
	{
		case Settings::Launch:
			{
				std::string cmd = selected->getCommand();

				// substitute $WID with window ID
				{
					std::string::size_type p;

					if( (p = cmd.find( "$WID" )) != std::string::npos )
					{
						std::ostringstream oss;

						oss << cmd.substr( 0, p ) <<
							"0x" <<
							std::hex << WindowManager::getClientWindow(
								app->getDisplay(),
								getWindowBelowCursor() ) <<
							cmd.substr( p+4 );

						cmd = oss.str();
					}
				}

				run( cmd );
			}
			break;
		case Settings::ShowWindows:
		case Settings::ShowNext:
			WindowManager::activate(
				app->getDisplay(),
				selected->getNextWindow() );
			break;
		case Settings::ShowPrevious:
			WindowManager::activate(
				app->getDisplay(),
				selected->getPreviousWindow() );
			break;
		case Settings::Hide:
			WindowManager::iconify(
				app->getDisplay(),
				selected->getNextWindow() );
			break;
		case Settings::Close:
			WindowManager::close(
				app->getDisplay(),
				selected->getNextWindow() );
			break;
	}
}

/**
 * Return item title
 */
std::string Menu::getItemTitle() const
{
	if( selected )
		return selected->getTitle();

	return "";
};

/**
 * Run some command
 *
 * @param command - command to execute
 */
int Menu::run( std::string command ) const
{
	int pid = fork();

	if( pid < 0 )
		throw "fork failed";
	else if( pid )
		return pid;

	char *shell = getenv( "SHELL" );

	if( !shell )
		shell = const_cast<char *>( "/bin/sh" );

    setsid();
    execl( shell, shell, "-c", command.c_str(), NULL );

	throw "exec failed";

	// make compiler happy
	return 0;
}

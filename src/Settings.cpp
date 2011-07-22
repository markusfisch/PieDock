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
#include "Settings.h"
#include "ModMask.h"
#include "Environment.h"

#include <dirent.h>
#include <stdlib.h>
#include <string.h>

#include <fstream>
#include <sstream>

using namespace PieDock;

/**
 * Returns the mouse button functions for the given menu and icon
 */
Settings::ButtonFunctions Settings::getButtonFunctions(const std::string &menu,
	MenuItem *item)
{
	// Normally, the first matching button function will be used,
	// so start with defnitions that will override the others.
	ButtonFunctions functions;

	// First, append item functions
	if( itemButtonFunctions.find(item) != itemButtonFunctions.end() )
		functions.insert(functions.end(),
			itemButtonFunctions.find(item)->second.begin(),
			itemButtonFunctions.find(item)->second.end());

	// Menu functions
	if( menuButtonFunctions.find(menu) != menuButtonFunctions.end() )
		functions.insert(functions.end(),
			menuButtonFunctions.find(menu)->second.begin(),
			menuButtonFunctions.find(menu)->second.end());

	// Default functions
	functions.insert(functions.end(), buttonFunctions.begin(),
		buttonFunctions.end());

	return functions;
}

/**
 * Set configuration file from binary
 *
 * @param b - name of binary
 */
void Settings::setConfigurationFileFromBinary( std::string b )
{
	setConfigurationFile( Environment::getHome()+"/."+b+"rc" );
}

/**
 * (Re-)Load settings
 *
 * @param d - display to load settings for
 */
void Settings::load( Display *d )
{
	std::ifstream in( configurationFile.c_str(), std::ios::in );

	if( !in ||
		!in.good() )
		throw "cannot read configuration file !";

	// reset settings to default values
	{
		width = height = 256;
		keys.clear();
		buttons.clear();
		buttonFunctions.clear();
		keyFunctions.clear();
		iconMap.reset();
		clearMenus();
		activeIndicator.reset();
		focusedAlpha = unfocusedAlpha = 0xff;
		startRadius = .9;
		fitts = false;
		zoomModifier = 1.0;
		spinStep = .5;
		centerAction = CenterDisappear;
		showTitle = false;
		titleFont.reset();
		cartoucheSettings.cornerRadius = 4;
		cartoucheSettings.alpha = 196;
		cartoucheSettings.color = 0xff000000;
		minimumNumber = 0;
#ifdef HAVE_XRENDER
		compositing = false;
#endif
	}

	// initialize temporary memorizers
	typedef std::vector<unsigned int> MasksToIgnore;

	enum PreloadSetting
	{
		PreloadNone = 0,
		PreloadAll,
		PreloadMenus
	};

	PreloadSetting preload = PreloadNone;

	// mod mask
	MasksToIgnore masksToIgnore;
	ModMask modMask( d );

	// parse file
	Statement statement;

	for( int line = 1; getline( in, statement ); line++ )
	{
		statement.cutComments();

		// skip empty lines
		if( statement.empty() )
			continue;

		// split statement into tokens
		std::vector<std::string> tokens = statement.split();
		std::vector<std::string>::iterator i = tokens.begin();

		if( !(*i).compare( "size" ) )
		{
			if( ++i == tokens.end() )
				throwParsingError(
					"insufficient arguments to size directive",
					line );
			else
				width = height = atoi( (*i).c_str() );
		}
		else if( !(*i).compare( "width" ) )
		{
			if( ++i == tokens.end() )
				throwParsingError(
					"insufficient arguments to width directive",
					line );
			else
				width = atoi( (*i).c_str() );
		}
		else if( !(*i).compare( "height" ) )
		{
			if( ++i == tokens.end() )
				throwParsingError(
					"insufficient arguments to width directive",
					line );
			else
				height = atoi( (*i).c_str() );
		}
		else if( !(*i).compare( "ignore-mask" ) )
		{
			if( ++i == tokens.end() )
				throwParsingError(
					"insufficient arguments to ignore-mask directive",
					line );
			else
				masksToIgnore.push_back(
					modMask.getModMaskFor( (*i).c_str() ) );
		}
		else if( !(*i).compare( "trigger" ) )
		{
			int n = tokens.size();

			if( n < 3 ||
				n > 6 )
				throwParsingError(
					"insufficient arguments to trigger directive",
					line );
			else
			{
				std::string type = (*++i);
				struct Trigger trigger = {
					AnyModifier, 0, "", 0 };

				// substract directive and first argument
				n -= 2;

				if( !type.compare( "menu" ) )
				{
					trigger.menuName = (*++i);

					// substract menu and name
					n -= 2;
					type = (*++i);
				}

				// if there is more than one argument left, the next one
				// has to be a modifier
				if( n > 1 )
					trigger.modifier = modMask.getModMaskFor( (*++i).c_str() );

				if( !type.compare( "key" ) )
				{
					trigger.keySym = XStringToKeysym( (*++i).c_str() );
					keys.push_back( trigger );
				}
				else if( !type.compare( "button" ) ||
					!type.compare( "button-up" ) )
				{
					trigger.button = atoi( (*++i).c_str() );
					trigger.eventMask = ButtonReleaseMask;
					buttons.push_back( trigger );
				}
				else if( !type.compare( "button-down" ) )
				{
					trigger.button = atoi( (*++i).c_str() );
					trigger.eventMask = ButtonPressMask;
					buttons.push_back( trigger );
				}
				else
					throwParsingError(
						"unknown trigger type",
						line );
			}
		}
		else if( !(*i).compare( "button" ) )
		{
			if( tokens.size() != 3 )
				throwParsingError(
					"insufficient arguments to button directive",
					line );
			else
			{
				ButtonFunction bf = { 0, Settings::NoAction };
				int button = atoi( (*++i).c_str() );

				switch( button )
				{
					case 0:
						throwParsingError(
							"invalid button number",
							line );
						break;
					case 1:
						bf.button = Button1;
						break;
					case 2:
						bf.button = Button2;
						break;
					case 3:
						bf.button = Button3;
						break;
					case 4:
						bf.button = Button4;
						break;
					case 5:
						bf.button = Button5;
						break;
					default:
						// not defined by X11
						bf.button = button;
						break;
				}

				if( bf.button &&
					(bf.action = resolveActionString( *++i )) !=
						Settings::NoAction )
					buttonFunctions.push_back( bf );
				else
					throwParsingError(
						"invalid button action",
						line );
			}
		}
		else if( !(*i).compare( "key" ) ||
			!(*i).compare( "key-up" ) )
		{
			if( tokens.size() != 3 )
				throwParsingError(
					"insufficient arguments to key directive",
					line );
			else
			{
				KeyFunction kf = { 0, Settings::NoAction, KeyPress };

				if( !(*i).compare( "key-up" ) )
					kf.eventType = KeyRelease;

				if( (kf.keySym = XStringToKeysym( (*++i).c_str() )) &&
					(kf.action = resolveActionString( *++i )) !=
						Settings::NoAction )
					keyFunctions.push_back( kf );
				else
					throwParsingError(
						"missing or unknown KeySym",
						line );
			}
		}
		else if( !(*i).compare( "path" ) )
		{
			if( tokens.size() < 2 )
				throwParsingError(
					"insufficient arguments to path directive",
					line );

			while( ++i != tokens.end() )
			{
				std::string path = (*i);

				// expand ~
				if( path[0] == '~' )
					path = Environment::getHome()+path.substr( 1 );

				// ensure there is a terminating slash
				if( path[path.length()-1] != '/' )
					path += '/';

				iconMap.addPath( path );
			}
		}
		else if( !(*i).compare( "ignore-window" ) )
		{
			if( ++i == tokens.end() )
				throwParsingError(
					"insufficient arguments to ignore-window directive",
					line );
			else
				windowsToIgnore[(*i)] = true;
		}
		else if( !(*i).compare( "alias" ) )
		{
			switch( tokens.size() )
			{
				default:
					throwParsingError(
						"insufficient arguments to alias directive",
						line );
					break;
				case 3:
					{
						// can't do this in just one line since the
						// compiler will iterate the latter one first,
						// at least with g++ 4.1.1
						std::string a = *++i;
						std::string n = *++i;

						iconMap.addNameAlias( a, n );
					}
					break;
				case 4:
					{
						std::string t = *++i;
						std::string a = *++i;
						std::string n = *++i;

						if( !t.compare( "name" ) )
							iconMap.addNameAlias( a, n );
						else if( !t.compare( "class" ) )
							iconMap.addClassAlias( a, n );
						else if( !t.compare( "title" ) )
							iconMap.addTitleAlias( a, n );
						else
							throwParsingError(
								"Unknown alias type",
								line );
					}
					break;
			}
		}
		else if( !(*i).compare( "menu" ) )
		{
			int n = tokens.size();
			std::string menuName = "";

			if( n > 2 )
				throwParsingError(
					"insufficient arguments to menu directive",
					line );
			else if( n > 1 )
				menuName = (*++i);

			line = readMenu( in, ++line, menuName );
		}
		else if( !(*i).compare( "preload" ) )
		{
			if( ++i == tokens.end() )
				throwParsingError(
					"insufficient arguments to preload directive",
					line );
			else
			{
				if( !(*i).compare( "all" ) )
					preload = PreloadAll;
				else if( !(*i).compare( "launchers" ) ||
					!(*i).compare( "sticky" ) ||
					!(*i).compare( "menus" ) )
					preload = PreloadMenus;
				else if( !(*i).compare( "none" ) )
					preload = PreloadNone;
				else
					throwParsingError(
						"unknown argument to preload directive",
						line );
			}
		}
		else if( !(*i).compare( "active-indicator" ) )
		{
			if( ++i == tokens.end() )
				throwParsingError(
					"insufficient arguments to active-indicator directive",
					line );
			else
			{
				// load file
				{
					std::string file = (*i);

					// expand ~
					if( file[0] == '~' )
						file = Environment::getHome()+file.substr( 1 );

					activeIndicator.load( file );
				}

				if( ++i != tokens.end() )
				{
					activeIndicator.parseX( (*i) );

					if( ++i != tokens.end() )
						activeIndicator.parseY( (*i) );
				}
			}
		}
		else if( !(*i).compare( "missing-icon" ) )
		{
			if( ++i == tokens.end() )
				throwParsingError(
					"insufficient arguments to missing-icon directive",
					line );
			else
			{
				std::string file = (*i);

				// expand ~
				if( file[0] == '~' )
					file = Environment::getHome()+file.substr( 1 );

				iconMap.setFileForMissing( file );
			}
		}
		else if( !(*i).compare( "filler-icon" ) )
		{
			if( ++i == tokens.end() )
				throwParsingError(
					"insufficient arguments to filler-icon directive",
					line );
			else
			{
				std::string file = (*i);

				// expand ~
				if( file[0] == '~' )
					file = Environment::getHome()+file.substr( 1 );

				iconMap.setFileForFiller( file );
			}
		}
		else if( !(*i).compare( "focused" ) )
		{
			if( ++i == tokens.end() )
				throwParsingError(
					"insufficient arguments to focused directive",
					line );
			else
				focusedAlpha = atoi( (*i).c_str() ) % 256;
		}
		else if( !(*i).compare( "unfocused" ) )
		{
			if( ++i == tokens.end() )
				throwParsingError(
					"insufficient arguments to unfocused directive",
					line );
			else
				unfocusedAlpha = atoi( (*i).c_str() ) % 256;
		}
		else if( !(*i).compare( "start-radius" ) )
		{
			if( ++i == tokens.end() )
				throwParsingError(
					"insufficient arguments to start-radius directive",
					line );
			else if( (startRadius = atof( (*i).c_str() )) > 1 ||
				startRadius < 0.1 )
				startRadius = 1;
		}
		else if( !(*i).compare( "centre" ) ||
			!(*i).compare( "center" ) )
		{
			if( ++i == tokens.end() )
				throwParsingError(
					"insufficient arguments to center directive",
					line );
			else
			{
				if( !(*i).compare( "Ignore" ) )
					centerAction = Settings::CenterIgnore;
				else if( !(*i).compare( "NearestIcon" ) )
					centerAction = Settings::CenterNearestIcon;
				else if( !(*i).compare( "Disappear" ) )
					centerAction = Settings::CenterDisappear;
				else
					throwParsingError(
						"invalid off-limits action",
						line );
			}
		}
		else if( !(*i).compare( "fitts" ) )
		{
			if( tokens.size() != 2 )
				throwParsingError(
					"insufficient arguments to fitts directive",
					line );
			else
				fitts = static_cast<bool>( atoi( (*++i).c_str() ) );
		}
		else if( !(*i).compare( "zoom" ) )
		{
			if( tokens.size() != 2 )
				throwParsingError(
					"insufficient arguments to zoom directive",
					line );
			else
				zoomModifier = atof( (*++i).c_str() );
		}
		else if( !(*i).compare( "spin-step" ) )
		{
			if( tokens.size() != 2 )
				throwParsingError(
					"insufficient arguments to spin-step directive",
					line );
			else
				spinStep = atof( (*++i).c_str() );
		}
		else if( !(*i).compare( "title" ) )
		{
			if( tokens.size() != 2 )
				throwParsingError(
					"insufficient arguments to title directive",
					line );
			else
				showTitle = static_cast<bool>( atoi( (*++i).c_str() ) );
		}
		else if( !(*i).compare( "font" ) )
		{
			int n = tokens.size();

			if( n < 3 ||
				n > 4 )
				throwParsingError(
					"insufficient arguments to font directive",
					line );
			else
			{
				titleFont.setFamily( *++i );
				titleFont.setSize( atof( (*++i).c_str() ) );

				if( n > 3 )
					titleFont.setColor( Text::Color( (*++i).c_str() ) );
			}
		}
		else if( !(*i).compare( "cartouche" ) )
		{
			int n = tokens.size();

			if( n < 2 ||
				n > 4 )
				throwParsingError(
					"insufficient arguments to cartouche directive",
					line );
			else
			{
				cartoucheSettings.cornerRadius =
					// ensure value is even, positive and below 32
					abs( atoi( (*++i).c_str() ) )>>1<<1%32;

				if( n > 2 )
					cartoucheSettings.alpha = atoi( (*++i).c_str() ) % 256;

				if( n > 3 )
				{
					Text::Color c( (*++i).c_str() );
					cartoucheSettings.color = c.getColor();
				}
			}
		}
		else if( !(*i).compare( "minimum-number" ) )
		{
			if( tokens.size() != 2 )
				throwParsingError(
					"insufficient arguments to minimum directive",
					line );
			else
				minimumNumber = atoi( (*++i).c_str() );
		}
#ifdef HAVE_XRENDER
		else if( !(*i).compare( "compositing" ) )
		{
			if( tokens.size() != 2 )
				throwParsingError(
					"insufficient arguments to compositing directive",
					line );
			else
				compositing = static_cast<bool>( atoi( (*++i).c_str() ) );
		}
#endif
		else
			throwParsingError(
				("unknown directive "+(*i)).c_str(),
				line );
	}

	// add additional triggers for masks to ignore
	for( MasksToIgnore::iterator mi = masksToIgnore.begin();
		mi != masksToIgnore.end();
		mi++ )
	{
		// duplicate keys
		{
			Keys keysToAdd;

			for( Keys::iterator ki = keys.begin();
				ki != keys.end();
				ki++ )
				if( (*ki).modifier != AnyModifier &&
					!((*ki).modifier & (*mi)) )
				{
					Trigger trigger = {
						(*ki).modifier | (*mi),
						(*ki).keySym,
						(*ki).menuName,
						(*ki).eventMask };

					keysToAdd.push_back( trigger );
				}

			for( Keys::iterator ki = keysToAdd.begin();
				ki != keysToAdd.end();
				ki++ )
				keys.push_back( (*ki) );
		}

		// duplicate buttons
		{
			Buttons buttonsToAdd;

			for( Buttons::iterator bi = buttons.begin();
				bi != buttons.end();
				bi++ )
				if( (*bi).modifier != AnyModifier &&
					!((*bi).modifier & (*mi)) )
				{
					Trigger trigger = {
						(*bi).modifier | (*mi),
						(*bi).button,
						(*bi).menuName,
						(*bi).eventMask };

					buttonsToAdd.push_back( trigger );
				}

			for( Keys::iterator bi = buttonsToAdd.begin();
				bi != buttonsToAdd.end();
				bi++ )
				buttons.push_back( (*bi) );
		}
	}

	// this should be done after parsing the whole file to ensure
	// all alias- and path-directives are processed
	if( preload != PreloadNone )
	{
		// those values are guesses for a pie menu; if that
		// application is supporting more menu forms some day,
		// this must be changed accordingly
		int size = (width > height ? width : height);
		int min = 2;
		int max = (static_cast<int>( size*.280 )>>1)<<1;
		int step = 2;

		switch( preload )
		{
			case PreloadAll:
				{
					const IconMap::Paths p = iconMap.getPath();

					for( IconMap::Paths::const_iterator i = p.begin();
						i != p.end();
						++i )
					{
						DIR *d = opendir( (*i).c_str() );

						if( !d )
							throw "cannot open image directory !";

						for( struct dirent *e; (e = readdir( d )); )
						{
							strtok( e->d_name, "." );

							if( !e->d_name ||
								*e->d_name == '.' )
								continue;

							Icon *icon = iconMap.getIconByName( e->d_name );

							if( !icon )
								continue;

							presizeIcon(
								icon,
								min,
								min,
								max,
								max,
								step,
								step );
						}

						closedir( d );
					}
				}
				break;
			case PreloadMenus:
				for( Menus::const_iterator m = menus.begin();
					m != menus.end();
					m++ )
					for( MenuItems::const_iterator i =
							(*m).second.begin();
						i != (*m).second.end();
						i++ )
					{
						Icon *icon = iconMap.getIconByName(
							(*i)->getTitle() );

						if( !icon )
							continue;

						presizeIcon(
							icon,
							min,
							min,
							max,
							max,
							step,
							step );
					}
				break;
		}
	}
}

/**
 * Read a menu structure
 *
 * @param in - in stream
 * @param line - in which line the menu starts
 * @param menuName - name of this menu
 */
int Settings::readMenu( std::istream &in, int line, std::string menuName )
{
	Statement statement;

	for( ; getline( in, statement ); line++ )
	{
		statement.cutComments();

		// skip empty lines
		if( statement.empty() )
			continue;

		// split statement into tokens
		std::vector<std::string> tokens = statement.split();
		std::vector<std::string>::iterator i = tokens.begin();

		if( !(*i).compare( "end" ) )
			return line;
		else if( !(*i).compare( "menu" ) )
		{
			if( ++i == tokens.end() )
				throwParsingError(
					"insufficient arguments to sub menu directive",
					line );

			std::string iconName = *i;
			std::string subMenuName = menuName+"/"+iconName;

			// read sub menu and add it to this menu
			line = readMenu( in, ++line, subMenuName );

			menus[menuName].push_back(
				new MenuItem( iconName, ":"+subMenuName ) );
		}
		else if( !(*i).compare( 0, 1, "*" ) )
		{
			menus[menuName].setIncludeWindows( true );

			if( !(*i).compare( 0, 2, "**" ) )
				menus[menuName].setOneIconPerWindow( true );

			if( !(*i).compare( "***" ) )
				menus[menuName].setOnlyFromActive( true );
		}
		else if( !(*i).compare( "icon" ) )
		{
			if( ++i == tokens.end() )
				throwParsingError(
					"insufficient arguments to icon directive",
					line );
			else
			{
				std::string name = *i;
				std::string command;

				if( ++i != tokens.end() )
					command = *i;
				else
					command = name;

				menus[menuName].push_back(
					new MenuItem( name, command ) );
			}
		}
		else if( !(*i).compare( "button" ) ) // Invidual menu or icon button definitions
		{
			if( tokens.size() != 3 )
				throwParsingError(
					"insufficient arguments to button directive",
					line );
			else
			{
				ButtonFunction bf = { 0, Settings::NoAction };
				int button = atoi( (*++i).c_str() );

				switch( button )
				{
					case 0:
						throwParsingError(
							"invalid button number",
							line );
						break;
					case 1:
						bf.button = Button1;
						break;
					case 2:
						bf.button = Button2;
						break;
					case 3:
						bf.button = Button3;
						break;
					case 4:
						bf.button = Button4;
						break;
					case 5:
						bf.button = Button5;
						break;
					default:
						// not defined by X11
						bf.button = button;
						break;
				}

				if( bf.button &&
					(bf.action = resolveActionString( *++i )) !=
						Settings::NoAction )
				{
					if (menus[menuName].empty()) // Assign to menu
						menuButtonFunctions[menuName].push_back(bf);
					else // Assign to icon
						itemButtonFunctions[menus[menuName].back()].push_back(bf);
				}
				else
					throwParsingError(
						"invalid button action",
						line );
			}
		}
	}

	throwParsingError(
		"Unclosed menu directive found",
		line );

	return line;
}

/**
 * Delete menu items
 */
void Settings::clearMenus()
{
	for( Menus::const_iterator m = menus.begin();
		m != menus.end();
		m++ )
		for( MenuItems::const_iterator i =
				(*m).second.begin();
			i != (*m).second.end();
			i++ )
			delete (*i);

	menus.clear();
}

/**
 * Resolve action string
 *
 * @param s - name of action to resolve
 */
Settings::Action Settings::resolveActionString( std::string s )
{
	if( !s.compare( "Launch" ) )
		return Launch;
	else if( !s.compare( "ShowNext" ) )
		return ShowNext;
	else if( !s.compare( "ShowPrevious" ) )
		return ShowPrevious;
	else if( !s.compare( "Hide" ) )
		return Hide;
	else if( !s.compare( "Close" ) )
		return Close;
	else if( !s.compare( "SpinUp" ) )
		return SpinUp;
	else if( !s.compare( "SpinDown" ) )
		return SpinDown;
	else if( !s.compare( "SpinNext" ) )
		return SpinNext;
	else if( !s.compare( "SpinPrevious" ) )
		return SpinPrevious;
	else if( !s.compare( "Disappear" ) )
		return Disappear;

	return NoAction;
}

/**
 * Throw a parsing error
 *
 * @param message - error message
 * @param line - line number
 */
void Settings::throwParsingError( const char *message, unsigned int line )
{
	std::ostringstream s;

	s << message << " in line " << line;

	// strdup() is required to not return a local pointer
	throw strdup( s.str().c_str() );
}

/**
 * Presize icon
 *
 * @param icon - icon to presize
 * @param fromWidth - width in pixels of smallest size
 * @param fromHeight - height in pixels of smallest size
 * @param toWidth - width in pixels of biggest size
 * @param toHeight - height in pixels of biggest size
 * @param xStep - horizontal step in pixels
 * @param yStep - vertical step in pixels
 */
void Settings::presizeIcon( Icon *icon, int fromWidth, int fromHeight,
	int toWidth, int toHeight, int xStep, int yStep )
{
	for( int w = fromWidth, h = fromHeight;
		w <= toWidth && h <= toHeight; )
	{
		icon->getSurface( w, h );

		if( w <= toWidth )
			w += xStep;

		if( h <= toHeight )
			h += yStep;
	}
}

/**
 * Cut off comments
 *
 * @param delimiter - character which heads a comment (optional)
 */
void Settings::Statement::cutComments( const char delimiter )
{
	std::string::size_type p;

	if( (p = find( delimiter )) != std::string::npos )
		erase( p );
}

/**
 * Split statement into its tokens
 *
 * @param delimiter - characters on which to split (optional)
 */
std::vector<std::string> Settings::Statement::split( const char *delimiter )
{
	std::vector<std::string> v;
	std::string::size_type last = 0;
	std::string::size_type until;

	// skip leading delimiters
	{
		std::string::size_type p;

		if( (p = find_first_not_of( delimiter )) != std::string::npos )
			last = p;
	}

	for( ;
		(until = lengthUntil( delimiter, last )) != std::string::npos;
		last = ++until )
		v.push_back( trim( substr( last, until-last ) ) );

	v.push_back( trim( substr( last ) ) );

	return v;
}

/**
 * Calculate the length of the initial segment of this string
 * which consists entirely of characters not in reject; respect
 * quoting
 *
 * @param reject - character that may appear within the span
 * @param offset - offset in string (optional)
 */
std::string::size_type Settings::Statement::lengthUntil(
	const char *reject,
	std::string::size_type offset )
{
	for( const char *s = c_str()+offset; *s; s++ )
	{
		if( *s == '\\' )
		{
			if( !*(++s) )
				break;

			continue;
		}

		if( *s == '"' )
		{
			for( ++s;
				 *s && *s != '"';
				 s++ )
				if( *s == '\\' )
				{
					if( !*(++s) )
						break;

					continue;
				}

			if( !*s )
				break;
		}

		for( const char *r = reject; *r; r++ )
			if( *r == *s )
				return s-c_str();
	}

	return std::string::npos;
}

/**
 * Return trimed string
 *
 * @param s - string to trim
 * @param d - characters to trim (optional)
 */
std::string Settings::Statement::trim( std::string s, const char *d )
{
	size_type p;

	if( (p = s.find_first_not_of( d )) !=
		std::string::npos )
		s.erase( 0, p );

	if( (p = s.find_last_not_of( d )) !=
		std::string::npos )
		s.erase( ++p );

	return s;
}

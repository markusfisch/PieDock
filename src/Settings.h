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
#ifndef _PieDock_Settings_
#define _PieDock_Settings_

#include "MenuItems.h"
#include "IconMap.h"
#include "ActiveIndicator.h"
#include "Text.h"

#include <X11/Xlib.h>
#include <vector>
#include <string>
#include <istream>

namespace PieDock
{
	/**
	 * Settings
	 *
	 * @author Markus Fisch <mf@markusfisch.de>
	 */
	class Settings
	{
		public:
			enum Action
			{
				NoAction,
				Launch,
				ShowNext,
				ShowPrevious,
				Hide,
				Close,
				SpinUp,
				SpinDown,
				SpinNext,
				SpinPrevious,
				Disappear
			};

			enum CenterAction
			{
				CenterIgnore,
				CenterNearestIcon,
				CenterDisappear
			};

			typedef struct Trigger
			{
				unsigned int modifier;
				union
				{
					unsigned int button;
					KeySym keySym;
				};
				std::string menuName;
				unsigned int eventMask;
			} Key, Button;

			typedef struct
			{
				unsigned int button;
				Action action;
			} ButtonFunction;

			typedef struct
			{
				KeySym keySym;
				Action action;
				unsigned int eventType;
			} KeyFunction;

			typedef struct
			{
				int cornerRadius;
				int alpha;
				unsigned int color;
			} CartoucheSettings;

			typedef std::vector<Key> Keys;
			typedef std::vector<Button> Buttons;
			typedef std::vector<ButtonFunction> ButtonFunctions;
			typedef std::vector<KeyFunction> KeyFunctions;
			typedef std::map<std::string, MenuItems> Menus;
			typedef std::map<std::string, bool> WindowsToIgnore;

			Settings() {}
			virtual ~Settings() { clearMenus(); }
			inline const std::string &getConfigurationFile() const {
				return configurationFile; }
			inline void setConfigurationFile( std::string s ) {
				configurationFile = s; }
			inline const int &getWidth() const { return width; }
			inline const int &getHeight() const { return height; }
#ifdef HAVE_XRENDER
			inline const bool &useCompositing() const { return compositing; }
#endif
			inline Keys &getKeys() { return keys; }
			inline Buttons &getButtons() { return buttons; }
			inline KeyFunctions &getKeyFunctions() {
				return keyFunctions; }
			inline IconMap &getIconMap() { return iconMap; }
			inline MenuItems *getMenu( std::string s ) {
				Menus::iterator i = menus.find( s );
				if( i == menus.end() )
					return 0;
				return &(*i).second; }
			inline bool ignoreWindow( std::string s ) {
				return (windowsToIgnore.find( s ) != windowsToIgnore.end()); }
			inline ActiveIndicator &getActiveIndicator() {
				return activeIndicator; }
			inline const int &getFocusedAlpha() const { return focusedAlpha; }
			inline const int &getUnfocusedAlpha() const {
				return unfocusedAlpha; }
			inline const double &getStartRadius() const {
				return startRadius; }
			inline const bool &isFitts() const { return fitts; }
			inline const double &getZoomModifier() const {
				return zoomModifier; }
			inline const CenterAction &getCenterAction() const {
				return centerAction; }
			inline const double &getSpinStep() const { return spinStep; }
			inline const bool &getShowTitle() const { return showTitle; }
			inline const Text::Font &getTitleFont() const { return titleFont; }
			inline const CartoucheSettings &getCartoucheSettings() const {
				return cartoucheSettings; }
			inline const int &getMinimumNumber() const { return minimumNumber; }
			virtual void setConfigurationFileFromBinary( std::string );
			virtual ButtonFunctions getButtonFunctions(
				const std::string &menu = std::string(),
				MenuItem *item = 0 );
			virtual void load( Display *d );

		protected:
			virtual int readMenu( std::istream &, int, std::string );
			virtual void clearMenus();
			virtual Action resolveActionString( const std::string & ) const;
			virtual unsigned int resolveButtonCode( const std::string & ) const;
			virtual void throwParsingError( const char *, unsigned int ) const;
			virtual void presizeIcon( Icon *, int, int, int, int, int, int );

		private:
			/**
			 * A statement string
			 */
			class Statement : public std::string
			{
				public:
					Statement( const char *s ) : std::string( s ) {}
					Statement( std::string s ) : std::string( s ) {}
					Statement() : std::string() {}
					virtual ~Statement() {}
					void cutComments( const char = '#' );
					std::vector<std::string> split( const char * = " \t" );

				private:
					std::string::size_type lengthUntil( const char *,
						 std::string::size_type = 0 );
					std::string trim( std::string,
						const char * = " \t\r\n\"" );
			};

			std::string configurationFile;
			int width;
			int height;
			Keys keys;
			Buttons buttons;
			ButtonFunctions buttonFunctions;
			std::map<MenuItem *, ButtonFunctions> itemButtonFunctions;
			std::map<std::string, ButtonFunctions> menuButtonFunctions;
			KeyFunctions keyFunctions;
			IconMap iconMap;
			Menus menus;
			WindowsToIgnore windowsToIgnore;
			ActiveIndicator activeIndicator;
			int focusedAlpha;
			int unfocusedAlpha;
			double startRadius;
			bool fitts;
			double zoomModifier;
			CenterAction centerAction;
			double spinStep;
			bool showTitle;
			Text::Font titleFont;
			CartoucheSettings cartoucheSettings;
			int minimumNumber;
#ifdef HAVE_XRENDER
			bool compositing;
#endif
	};
}

#endif

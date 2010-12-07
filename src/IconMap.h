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
#ifndef _PieDock_IconMap_
#define _PieDock_IconMap_

#include "Icon.h"

#include <string>
#include <vector>
#include <map>

namespace PieDock
{
	/**
	 * Icon map
	 *
	 * @author Markus Fisch <mf@markusfisch.de>
	 */
	class IconMap
	{
		public:
			typedef std::vector<std::string> Paths;

			IconMap() :
				missingSurface( 0 ),
				fillerSurface( 0 ) {}
			virtual ~IconMap();
			virtual inline void addPath( std::string p ) {
				paths.push_back( p ); }
			virtual inline const Paths &getPath() const {
				return paths; }
			virtual inline void setFileForMissing( std::string f ) {
				fileForMissing = f; }
			virtual inline const std::string &getFileForMissing() const {
				return fileForMissing; }
			virtual inline void setFileForFiller( std::string f ) {
				fileForFiller = f; }
			virtual inline const std::string &getFileForFiller() const {
				return fileForFiller; }
			virtual void reset();
			virtual void addNameAlias( std::string, std::string );
			virtual void addClassAlias( std::string, std::string );
			virtual void addTitleAlias( std::string, std::string );
			virtual Icon *getIconByName( std::string );
			virtual Icon *getIconByClass( std::string );
			virtual Icon *getIconByTitle( std::string );
			virtual Icon *getMissingIcon( std::string );
			virtual Icon *getFillerIcon();

		protected:
			typedef std::map<std::string, std::string> AliasToFile;
			typedef std::map<std::string, Icon *> FileToIcon;

			virtual Icon *surfaceIcon( std::string );
			virtual Icon *createIcon( ArgbSurface &, std::string );
			virtual void freeIcons();

		private:
			Paths paths;
			AliasToFile nameToFile;
			AliasToFile classToFile;
			AliasToFile titleToFile;
			FileToIcon cache;
			static const char fallbackPng[];
			ArgbSurface *missingSurface;
			ArgbSurface *fillerSurface;
			std::string fileForMissing;
			std::string fileForFiller;
	};
}

#endif

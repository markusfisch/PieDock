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
#ifndef _PieDock_WorkspaceLayout_
#define _PieDock_WorkspaceLayout_

#include <X11/Xlib.h>

namespace PieDock
{
	/**
	 * Layout of workspaces
	 *
	 * @author Markus Fisch <mf@markusfisch.de>
	 */
	class WorkspaceLayout
	{
		public:
			typedef struct
			{
				unsigned long number;
				int x;
				int y;
			} WorkspacePosition;

			enum PreferredVirtualLayout
			{
				Horizontal,
				Vertical,
				Square
			};

			virtual ~WorkspaceLayout() {}
			inline const unsigned long &getScreenWidth() const {
				return screen.width; }
			inline const unsigned long &getScreenHeight() const {
				return screen.height; }
			inline const unsigned long &getTotalWidth() const {
				return total.width; }
			inline const unsigned long &getTotalHeight() const {
				return total.height; }
			inline const int &getColumns() const {
				return columns; }
			inline const int &getRows() const {
				return rows; }
			inline const bool &isVirtual() const {
				return virtualDesktop; }
			static WorkspaceLayout *getWorkspaceLayout( Display *,
				PreferredVirtualLayout = Horizontal );
			bool isOnAnotherWorkspace( Window, WorkspacePosition & );
			bool isOnAnotherWorkspace( Window, XWindowAttributes &,
				WorkspacePosition & );

		private:
			typedef struct
			{
				unsigned long width;
				unsigned long height;
			} Geometry;

			static WorkspaceLayout *singleton;
			Display *display;
			PreferredVirtualLayout preferredLayout;
			Geometry screen;
			Geometry total;
			int columns;
			int rows;
			bool virtualDesktop;

			WorkspaceLayout( Display *, PreferredVirtualLayout );
			WorkspaceLayout &operator=( const WorkspaceLayout & ) {
				return *this; }
	};
}

#endif

/*
 *   O         ,-
 *  ° o    . -´  '     ,-
 *   °  .´        ` . ´,´
 *     ( °   ))     . (
 *      `-;_    . -´ `.`.
 *          `._'       ´
 *
 * Copyright (c) 2007-2012 Markus Fisch <mf@markusfisch.de>
 *
 * Licensed under the MIT license:
 * http://www.opensource.org/licenses/mit-license.php
 */
#ifndef _PieDock_MenuItemWithWorkspaces_
#define _PieDock_MenuItemWithWorkspaces_

#include "MenuItem.h"
#include "WorkspaceLayout.h"

namespace PieDock
{
	/**
	 * Menu item with workspaces
	 *
	 * @author Markus Fisch <mf@markusfisch.de>
	 */
	class MenuItemWithWorkspaces : public MenuItem
	{
		public:
			MenuItemWithWorkspaces( Icon * );
			virtual ~MenuItemWithWorkspaces();
			virtual inline Icon *getIcon() const {
				return useWorkspaceIcon ?
					workspaceIcon :
					MenuItem::getIcon(); }
			virtual void showWorkspace( WorkspaceLayout *, unsigned int,
				unsigned int );

		protected:
			virtual void freeWorkspaceIcon();

		private:
			typedef struct
			{
				int x;
				int y;
				int width;
				int height;
			} Geometry;

			Icon *workspaceIcon;
			bool useWorkspaceIcon;
			Geometry cachedGeometry;
	};
}

#endif

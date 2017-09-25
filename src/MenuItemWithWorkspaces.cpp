#include "MenuItemWithWorkspaces.h"
#include "WorkspaceLayout.h"
#include "Cartouche.h"
#include "Blender.h"

using namespace PieDock;

/**
 * Create menu item from icon
 *
 * @param i - icon
 */
MenuItemWithWorkspaces::MenuItemWithWorkspaces(Icon *i) :
	MenuItem(i),
	workspaceIcon(0),
	useWorkspaceIcon(false) {
}

/**
 * Clean up
 */
MenuItemWithWorkspaces::~MenuItemWithWorkspaces() {
	freeWorkspaceIcon();
}

/**
 * Show workspace icon
 *
 * @param wsl - workspace layout
 * @param workspaceColor - ARGB value of workspace
 * @param windowColor - ARGB value of window
 */
void MenuItemWithWorkspaces::showWorkspace(
	WorkspaceLayout *wsl,
	unsigned int workspaceColor,
	unsigned int windowColor) {
	for (;;) {
		if (!MenuItem::getIcon() || getWindowInfos().size() != 1) {
			break;
		}

		if (!workspaceIcon) {
			cachedGeometry.x = 0;
			cachedGeometry.y = 0;
			cachedGeometry.width = 0;
			cachedGeometry.height = 0;
		}

		Geometry g = { 0, 0, 0, 0 };

		// find out if window is on another workspace
		{
			WindowStack::WindowInfos::iterator i = getWindowInfos().begin();

			// no window information
			if (i == getWindowInfos().end()) {
				break;
			}

			WorkspaceLayout::WorkspacePosition p;

			if (!wsl->isOnAnotherWorkspace(
						(*i).window,
						(*i).attributes,
						p)) {
				// window is on active workspace
				useWorkspaceIcon = false;
				return;
			}

			g.x = p.x;
			g.y = p.y;
			g.width = (*i).attributes.width > 10 ?
				(*i).attributes.width :
				10;
			g.height = (*i).attributes.height > 10 ?
				(*i).attributes.height :
				10;
		}

		// same position as before
		if (workspaceIcon &&
				g.x == cachedGeometry.x &&
				g.y == cachedGeometry.y &&
				g.width == cachedGeometry.width &&
				g.height == cachedGeometry.height) {
			// so reuse current workspace icon
			useWorkspaceIcon = true;
			return;
		}

		ArgbSurface s(MenuItem::getIcon()->getSurface());
		Blender b(s);
		int paddingX = 0;
		int paddingY = 0;
		double f;

		if (wsl->getTotalWidth() > wsl->getTotalHeight()) {
			f = static_cast<double>(s.getWidth()) / wsl->getTotalWidth();
			paddingY = (s.getHeight() -
				static_cast<int>(f * wsl->getTotalHeight())) >> 1;
		} else {
			f = static_cast<double>(s.getHeight()) / wsl->getTotalHeight();
			paddingX = (s.getWidth()-
				static_cast<int>(f * wsl->getTotalWidth())) >> 1;
		}

		// draw workspace layout
		{
			int w = static_cast<int>(f*wsl->getScreenWidth());
			int h = static_cast<int>(f*wsl->getScreenHeight());

			// there's not enough space to display workspaces
			if (w < 2 || h < 2) {
				break;
			}

			Cartouche workspace(
				w-1,
				h-1,
				0,
				workspaceColor);

			for (int r = wsl->getRows(), y = paddingY;
					r--;
					y += h)
				for (int c = wsl->getColumns(), x = paddingX;
						c--;
						x += w) {
					b.blend(workspace, x, y);
				}
		}

		// draw window in the right workspace
		{
			Geometry r = { g.x, g.y, 0, 0 };
			int xe = g.x+g.width;
			int ye = g.y+g.height;

			if (xe > wsl->getTotalWidth()) {
				r.width = xe-wsl->getTotalWidth();
				g.width = wsl->getTotalWidth()-g.x;
				r.x = 0;
				r.height = g.height;
			}

			if (ye > wsl->getTotalHeight()) {
				r.height = ye-wsl->getTotalHeight();
				g.height = wsl->getTotalHeight()-g.y;
				r.y = 0;
				r.width = g.width;
			}

			for (;;) {
				int w = static_cast<int>(f*g.width);
				int h = static_cast<int>(f*g.height);
				Cartouche window(
					w < 1 ? 1 : w,
					h < 1 ? 1 : h,
					0,
					windowColor);

				b.blend(window,
						paddingX+static_cast<int>(f*g.x),
						paddingY+static_cast<int>(f*g.y));

				if (r.width || r.height) {
					g = r;
					r.width = r.height = 0;
					continue;
				}

				break;
			}
		}

		workspaceIcon = new Icon(&s);
		useWorkspaceIcon = true;
		cachedGeometry = g;
		return;
	}

	freeWorkspaceIcon();
	useWorkspaceIcon = false;
}

/**
 * Free workspace icon
 */
void MenuItemWithWorkspaces::freeWorkspaceIcon() {
	delete workspaceIcon;
	workspaceIcon = 0;
}

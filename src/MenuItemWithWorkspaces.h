#ifndef _PieDock_MenuItemWithWorkspaces_
#define _PieDock_MenuItemWithWorkspaces_

#include "MenuItem.h"
#include "WorkspaceLayout.h"

namespace PieDock {
class MenuItemWithWorkspaces : public MenuItem {
public:
	MenuItemWithWorkspaces(Icon *);
	virtual ~MenuItemWithWorkspaces();
	virtual inline Icon *getIcon() const {
		return useWorkspaceIcon ? workspaceIcon : MenuItem::getIcon();
	}
	virtual void showWorkspace(WorkspaceLayout *, unsigned int, unsigned int);

protected:
	virtual void freeWorkspaceIcon();

private:
	typedef struct {
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

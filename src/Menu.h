#ifndef _PieDock_Menu_
#define _PieDock_Menu_

#include "Application.h"
#include "Settings.h"
#include "Icon.h"
#include "MenuItems.h"

#include <string>

namespace PieDock {
class Menu {
public:
	Menu(Application *);
	virtual ~Menu() {}
	inline void setWindowBelowCursor(Window w) {
		windowBelowCursor = w;
	}
	inline const Window &getWindowBelowCursor() const {
		return windowBelowCursor;
	}
	inline MenuItem *getSelected() const {
		return selected;
	}
	inline const std::string &getName() const {
		return name;
	}
	virtual bool update(std::string, Window = 0);
	virtual void draw(int, int) {}
	virtual bool change(Settings::Action = Settings::Launch);
	virtual void execute(Settings::Action = Settings::Launch);
	virtual std::string getItemTitle() const;

protected:
	inline Application *getApp() const {
		return app;
	}
	inline void setSelected(MenuItem *s) {
		selected = s;
	}
	inline MenuItems *getMenuItems() const {
		return menuItems;
	}
	virtual int run(std::string) const;

private:
	Application *app;
	MenuItem *selected;
	MenuItems *menuItems;
	MenuItems openWindows;
	Window windowBelowCursor;
	std::string name;
};
}

#endif

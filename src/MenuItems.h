#ifndef _PieDock_MenuItems_
#define _PieDock_MenuItems_

#include "MenuItem.h"

#include <list>

namespace PieDock {
class MenuItems : public std::list<MenuItem *> {
public:
	MenuItems() : windows(false), one(false), only(false) {}
	virtual ~MenuItems() {}
	inline const bool &includeWindows() const {
		return windows;
	}
	inline void setIncludeWindows(bool w) {
		windows = w;
	}
	inline const bool &oneIconPerWindow() const {
		return one;
	}
	inline void setOneIconPerWindow(bool o) {
		one = o;
	}
	inline const bool &onlyFromActive() const {
		return only;
	}
	inline void setOnlyFromActive(bool o) {
		only = o;
	}

private:
	bool windows;
	bool one;
	bool only;
};
}

#endif

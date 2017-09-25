#ifndef _PieDock_WindowStack_
#define _PieDock_WindowStack_

#include <X11/Xlib.h>

#include <vector>

namespace PieDock {
class WindowStack {
public:
	typedef struct {
		Window window;
		XWindowAttributes attributes;
	} WindowInfo;

	typedef std::vector<WindowInfo> WindowInfos;

	WindowStack() : last(0) {}
	virtual ~WindowStack() {}
	inline WindowInfos &getWindowInfos() {
		return windowInfos;
	}
	inline const bool hasWindows() const {
		return windowInfos.empty() ^ true;
	}
	inline void clearWindows() {
		windowInfos.clear();
	}
	void addWindow(Display *, Window);
	const Window getNextWindow();
	const Window getPreviousWindow();
	const bool isUnmapped();

private:
	WindowInfos windowInfos;
	Window last;
};
}

#endif

#ifndef _PieDock_PieMenuWindow_
#define _PieDock_PieMenuWindow_

#include "TransparentWindow.h"
#include "PieMenu.h"
#include "Text.h"
#include "Cartouche.h"

#include <time.h>

namespace PieDock {
class PieMenuWindow : public TransparentWindow {
public:
	enum Placement {
		AroundCursor,
		IconBelowCursor
	};

	PieMenuWindow(Application &);
	virtual ~PieMenuWindow();
	bool appear(std::string = "", Placement = AroundCursor);
	void draw();
	bool processEvent(XEvent &);

protected:
	typedef std::map<int, Cartouche *> CartoucheMap;

	virtual void show(Placement = AroundCursor);
	virtual void hide();
	virtual void updateWithText();
	virtual bool performAction(Settings::Action);
	virtual void place(Placement);

private:
	static const std::string ShowMessage;
	PieMenu menu;
	Pixmap textCanvas;
	Text *text;
	CartoucheMap cartoucheMap;
};
}

#endif

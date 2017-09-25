#ifndef _PieDock_TransparentWindow_
#define _PieDock_TransparentWindow_

#include "Application.h"
#include "XSurface.h"

#include <X11/Xlib.h>

#ifdef HAVE_XRENDER
#include <X11/extensions/Xrender.h>
#endif

namespace PieDock {
class TransparentWindow {
public:
	TransparentWindow(Application &);
	virtual ~TransparentWindow();
	inline const Window &getWindow() const {
		return window;
	}
	virtual void appear() {}
	virtual void draw() {}
	virtual bool processEvent(XEvent &) {
		return false;
	}

protected:
	inline Application *getApp() const {
		return app;
	}
	inline const int &getWidth() const {
		return width;
	}
	inline const int &getHeight() const {
		return height;
	}
	inline XSurface *getCanvas() const {
		return canvas;
	}
	inline const GC &getGc() const {
		return gc;
	}
#ifdef HAVE_XRENDER
	inline const Pixmap &getAlphaPixmap() const {
		return alphaPixmap;
	}
	inline virtual void composite() const {
		XRenderComposite(
			app->getDisplay(),
			PictOpOver,
			windowPicture,
			None,
			alphaPicture,
			0,
			0,
			0,
			0,
			0,
			0,
			width,
			height);
	}
#endif
	virtual void show();
	virtual void hide() const;
	virtual void clear();
	virtual void update() const;

private:
	Application *app;
	Window window;
	int width;
	int height;
	XSurface *canvas;
	unsigned char *buffer;
	GC gc;
#ifdef HAVE_XRENDER
	Pixmap alphaPixmap;
	Picture windowPicture;
	Picture alphaPicture;
#endif
};
}

#endif

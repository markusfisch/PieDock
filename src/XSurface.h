#ifndef _PieDock_XSurface_
#define _PieDock_XSurface_

#include "Surface.h"
#include "ArgbSurface.h"

#include <X11/Xlib.h>

namespace PieDock {
class XSurface : public Surface {
public:
	XSurface(int, int, Display *, Visual *, int);
	virtual ~XSurface();
	inline XImage *getResource() const {
		return resource;
	}
	inline Visual *getVisual() const {
		return visual;
	}

protected:
	virtual int determineBitsPerPixel(int);
	virtual void allocateData();
	virtual void freeData();

private:
	Display *display;
	Visual *visual;
	int orginalDepth;
	XImage *resource;
};
}

#endif

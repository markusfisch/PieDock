#ifndef _PieDock_Blender_
#define _PieDock_Blender_

#include "Surface.h"
#include "ArgbSurface.h"

namespace PieDock {
class Blender {
public:
	enum Opacity {
		Transparent = 0,
		Translucent = 128,
		Opaque = 0xff
	};

	Blender(Surface &);
	virtual ~Blender() {}
#ifdef HAVE_XRENDER
	virtual void setCompositing(bool c) {
		compositing = c;
	}
	virtual const bool &useCompositing() const {
		return compositing;
	}
#endif
	virtual void blend(const ArgbSurface &, int, int, int = Opaque);

protected:
	typedef struct {
		unsigned char *dest;
		unsigned char *src;
		int length;
		int repeats;
		int srcSkip;
		int destSkip;
		int alpha;
	} Details;

	virtual void blendInto32Bit(Details &);
	virtual void blendInto24Bit(Details &);
	virtual void blendInto16Bit(Details &);

private:
	Surface *canvas;
#ifdef HAVE_XRENDER
	bool compositing;
#endif
};
}

#endif

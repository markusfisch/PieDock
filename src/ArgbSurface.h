#ifndef _PieDock_ArgbSurface_
#define _PieDock_ArgbSurface_

#include "Surface.h"

namespace PieDock {
class ArgbSurface : public Surface {
public:
	ArgbSurface(int, int);
	virtual ~ArgbSurface() {}
};
}

#endif

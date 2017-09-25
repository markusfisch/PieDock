#ifndef _PieDock_ArgbSurfaceSizeMap_
#define _PieDock_ArgbSurfaceSizeMap_

#include "ArgbSurface.h"

#include <string>
#include <map>

namespace PieDock {
class ArgbSurfaceSizeMap {
public:
	ArgbSurfaceSizeMap(const ArgbSurface *);
	virtual ~ArgbSurfaceSizeMap();
	inline const ArgbSurface &getSurface() const {
		return surface;
	}
	virtual const ArgbSurface *getSurface(int, int);
	virtual void setSurface(ArgbSurface *);

protected:
	virtual void clear();

private:
	typedef std::map<int, ArgbSurface *> SurfaceMap;

	ArgbSurface surface;
	SurfaceMap surfaceMap;
};
}

#endif

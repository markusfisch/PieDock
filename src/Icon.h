#ifndef _PieDock_Icon_
#define _PieDock_Icon_

#include "ArgbSurfaceSizeMap.h"

namespace PieDock {
class Icon : public ArgbSurfaceSizeMap {
public:
	enum Type {
		Missing,
		Filler,
		File,
		Window
	};

	Icon(const ArgbSurface *s, Type t = File) :
		ArgbSurfaceSizeMap(s),
		type(t) {}
	virtual ~Icon() {}
	inline const Type &getType() const {
		return type;
	}
	inline void setType(Type t) {
		type = t;
	}

private:
	Type type;
};
}

#endif

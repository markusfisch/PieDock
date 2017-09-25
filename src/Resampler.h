#ifndef _PieDock_Resampler_
#define _PieDock_Resampler_

#include "ArgbSurface.h"

namespace PieDock {
class Resampler {
public:
	virtual ~Resampler() {}
	static void resample(ArgbSurface &, ArgbSurface &);

protected:
	static void biLinear(ArgbSurface &, ArgbSurface &);
	static void areaAveraging(ArgbSurface &, ArgbSurface &);

private:
	Resampler() {}
};
}

#endif

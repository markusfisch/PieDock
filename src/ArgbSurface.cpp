#include "ArgbSurface.h"

using namespace PieDock;

/**
 * Create a ARGB surface
 *
 * @param w - width of surface in pixels
 * @param h - height of surface in pixels
 */
ArgbSurface::ArgbSurface(int w, int h) : Surface() {
	calculateSize(w, h, ARGB);
	allocateData();
}

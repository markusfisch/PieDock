#include "ActiveIndicator.h"
#include "Png.h"

using namespace PieDock;

/**
 * Load image file
 *
 * @param f - PNG file to load
 */
void ActiveIndicator::load(std::string f) {
	clear();
	ArgbSurface *s = Png::load(f);
	sizeMap = new ArgbSurfaceSizeMap(s);
	delete s;
}

/**
 * Parse horizontal position argument
 *
 * @param s - string describing position
 */
void ActiveIndicator::parseX(std::string s) {
	if (!s.compare("left")) {
		xAlign = Left;
	} else if (!s.compare("center")) {
		xAlign = Center;
	} else if (!s.compare("right")) {
		xAlign = Right;
	}
}

/**
 * Parse vertical position argument
 *
 * @param s - string describing position
 */
void ActiveIndicator::parseY(std::string s) {
	if (!s.compare("top")) {
		yAlign = Top;
	} else if (!s.compare("center")) {
		yAlign = Center;
	} else if (!s.compare("bottom")) {
		yAlign = Bottom;
	}
}

/**
 * Reset
 */
void ActiveIndicator::reset() {
	clear();
	xAlign = Right;
	yAlign = Bottom;
}

/**
 * Clean up
 */
void ActiveIndicator::clear() {
	delete sizeMap;
	sizeMap = 0;
}

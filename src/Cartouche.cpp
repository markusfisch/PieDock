#include "Cartouche.h"

#include <math.h>
#include <string.h>

using namespace PieDock;

/**
 * Create plain solid rectangle
 *
 * @param w - width of rectangle in pixels
 * @param h - height of rectangle in pixels
 * @param r - radius of rounded corner in pixels
 * @param c - color of rectangle (optional)
 */
Cartouche::Cartouche(int w, int h, int r, unsigned int c) :
	ArgbSurface(w, h) {
	bzero(getData(), getSize());

	// check if there is enough space to draw rounded corners
	{
		int corners = r << 1;

		if (!corners ||
				w - corners < 1 ||
				h - corners < 1) {
			drawRectangle(0, 0, w, h, c);
			return;
		}
	}

	drawRoundedRectangle(0, 0, w, h, r, c);
}

/**
 * Draw a solid rectangle
 *
 * @param left - left corner of rectangle
 * @param top - top corner of rectangle
 * @param right - right corner of rectangle
 * @param bottom - bottom corner of rectangle
 * @param color - color of rectangle
 */
void Cartouche::drawRectangle(
		int left,
		int top,
		int right,
		int bottom,
		unsigned int color) {
	int width = right-left;
	int height = bottom-top;

	if (width < 1 || height < 1) {
		return;
	}

	unsigned char *first = getData() + top * getBytesPerLine() + (left << 2);
	unsigned char *d = first;

	// first line
	for (int w = width; w--; d += 4) {
		memcpy(d, &color, sizeof(unsigned int));
	}

	if (height < 2) {
		return;
	}

	// remaining lines
	int bpl = width << 2;
	int skip = getBytesPerLine() - bpl;

	d += skip;
	skip += bpl;

	for (int h = height; --h; d += skip) {
		memcpy(d, first, bpl);
	}
}

/**
 * Draw rounded rectangle
 *
 * @param left - left corner of rectangle
 * @param top - top corner of rectangle
 * @param right - right corner of rectangle
 * @param bottom - bottom corner of rectangle
 * @param radius - radius of a corner
 * @param color - color of rectangle
 */
void Cartouche::drawRoundedRectangle(
		int left,
		int top,
		int right,
		int bottom,
		int radius,
		unsigned int color) {
	int width = right-left;
	int height = bottom-top;

	if (width < 1 || height < 1) {
		return;
	}

	drawRectangle(left, top+radius, right, bottom-radius, color);

	Details details = {
		top + radius,
		left + radius,
		bottom - radius - 1,
		right - radius - 1,
		getBytesPerLine(),
		color,
		getData() + top * getBytesPerLine()
	};

	for (int d = -radius, x = radius, y = 0; y <= x;) {
		double n = atan2(y, x);

		if (n > 0) {
			double fx = cos(n) * radius;
			double fy = sin(n) * radius;
			int mx = 0;

			if (fx < static_cast<double>(x)) {
				mx = 1;
			}

			unsigned char alpha = static_cast<unsigned char>(
				fmod(fx, 1.0) * fmod(fy, 1.0) * 255);

			drawCurveSlices(details, x - mx, y - 1, alpha);
			drawCurveSlices(details, y - mx, x - 1, alpha);
		}

		d += 2 * y + 1;
		++y;

		if (d > 0) {
			d -= 2 * x + 2;
			x--;
		}
	}
}

/**
 * Draw curve slices of a rounded rectangle
 *
 * @param d - details
 * @param x - x curve position
 * @param y - y curve position
 * @param n - alpha of neighbouring pixel
 */
void Cartouche::drawCurveSlices(Details &d, int x, int y, unsigned char n) {
	unsigned char *t = d.data + (d.top - y) * d.bytesPerLine;
	unsigned char *b = d.data + (d.bottom + y) * d.bytesPerLine;
	int l = (d.left - x) << 2;
	int r = (d.right + x) << 2;
	double alpha = 255.0 / n;

	if (n) {
		unsigned char *c = reinterpret_cast<unsigned char *>(&d.color);

		// left side, unrolled for speed
		if (l > 0) {
			int p = l - 4;

			// top
			{
				unsigned char *s = t + p;

				s[0] = c[0];
				s[1] = c[1];
				s[2] = c[2];
				s[3] = c[3] / alpha;
			}

			// bottom
			{
				unsigned char *s = b + p;

				s[0] = c[0];
				s[1] = c[1];
				s[2] = c[2];
				s[3] = c[3] / alpha;
			}
		}

		// right side, unrolled for speed
		if (r < d.bytesPerLine - 4) {
			int p = r + 4;

			// top
			{
				unsigned char *s = t + p;

				s[0] = c[0];
				s[1] = c[1];
				s[2] = c[2];
				s[3] = c[3] / alpha;
			}

			// bottom
			{
				unsigned char *s = b + p;

				s[0] = c[0];
				s[1] = c[1];
				s[2] = c[2];
				s[3] = c[3] / alpha;
			}
		}
	}

	// top
	for (int *s = reinterpret_cast<int *>(t + l),
			*e = reinterpret_cast<int *>(t + r);
			s <= e;
			++s) {
		*s = d.color;
	}

	// bottom
	for (int *s = reinterpret_cast<int *>(b + l),
			*e = reinterpret_cast<int *>(b + r);
			s <= e;
			++s) {
		*s = d.color;
	}
}

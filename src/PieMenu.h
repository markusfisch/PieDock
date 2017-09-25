#ifndef _PieDock_PieMenu_
#define _PieDock_PieMenu_

#include "Application.h"
#include "Surface.h"
#include "Blender.h"
#include "Icon.h"
#include "Menu.h"

#include <vector>
#include <math.h>

namespace PieDock {
class PieMenu : public Menu {
public:
	PieMenu(Application *, Surface &);
	virtual ~PieMenu() {}
	inline const bool cursorInCenter() const {
		return (getSelected() == 0);
	}
	inline const int &getRadius() const {
		return maxRadius;
	}
	inline Blender *getBlender() {
		return &blender;
	}
	inline void invalidate() {
		lastX = lastY = -1;
	}
	virtual bool update(std::string = "", Window = 0);
	virtual bool isObsolete(int, int);
	virtual void draw(int, int);
	virtual void turn(double);
	virtual void turn(int);
	virtual void setTwistForSelection();

protected:
	/**
	 * Return the difference of two angles in radians; implemented
	 * here to ensure the method will be compiled inline
	 *
	 * @param a - angle in radians
	 * @param b - angle in radians
	 */
	inline virtual double getAngleDifference(double a, double b) {
		double c = a - b;
		double d;

		if (a > b) {
			d = a - (b + tau);
		} else {
			d = a - (b - tau);
		}

		if (fabs(c) < fabs(d)) {
			return c;
		}

		return d;
	}

	/**
	 * Recalculate angle to be within a valid range; implemented
	 * here to ensure the method will be compiled inline
	 *
	 * @param a - angle in radians
	 */
	inline virtual double getValidAngle(double a) {
		if (a < -M_PI) {
			a += tau;
		} else if (a > M_PI) {
			a -= tau;
		}

		return a;
	}

private:
	static const double tau;
	static const double turnSteps[];

	Blender blender;
	int size;
	int maxRadius;
	int radius;
	double twist;
	int centerX;
	int centerY;
	int lastX;
	int lastY;
	double *turnStack;
	double *turnBy;
};
}

#endif

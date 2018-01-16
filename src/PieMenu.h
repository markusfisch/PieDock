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
	 * Return the smallest difference of two angles in radians; implemented
	 * here to ensure the method will be compiled inline
	 *
	 * @param a - angle in radians
	 * @param b - angle in radians
	 */
	inline virtual double getAngleDifference(double a, double b) {
		double d = fmod(((a - b) + tau), tau);
		if (d > M_PI) {
			d -= tau;
		}
		return d;
	}

	/**
	 * Make sure angle is between 0 and TAU; implemented here to ensure
	 * the method will be compiled inline
	 *
	 * @param a - angle in radians
	 */
	inline virtual double getValidAngle(double a) {
		return fmod(a + tau, tau);
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

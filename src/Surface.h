#ifndef _PieDock_Surface_
#define _PieDock_Surface_

namespace PieDock {
class Surface {
public:
	enum ColorDepth {
		Indexed = 8,
		HighColor = 16,
		RGB = 24,
		ARGB = 32
	};

	Surface(const Surface &);
	virtual ~Surface();
	inline unsigned char *getData() const {
		return data;
	}
	inline const int &getWidth() const {
		return width;
	}
	inline const int &getHeight() const {
		return height;
	}
	inline const int &getDepth() const {
		return depth;
	}
	inline const int &getBytesPerPixel() const {
		return bytesPerPixel;
	}
	inline const int &getBytesPerLine() const {
		return bytesPerLine;
	}
	inline const int &getPadding() const {
		return padding;
	}
	inline const int &getSize() const {
		return size;
	}
	Surface &operator=(const Surface &);

protected:
	Surface();
	inline void setData(unsigned char *d) {
		data = d;
	}
	virtual void calculateSize(int, int, int = ARGB);
	virtual void allocateData();
	virtual void freeData();

private:
	unsigned char *data;
	int width;
	int height;
	int depth;
	int bytesPerPixel;
	int bytesPerLine;
	int padding;
	int size;
};
}

#endif

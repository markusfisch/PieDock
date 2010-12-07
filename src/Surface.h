/*
 *   O         ,-
 *  ° o    . -´  '     ,-
 *   °  .´        ` . ´,´
 *     ( °   ))     . (
 *      `-;_    . -´ `.`.
 *          `._'       ´
 *
 * Copyright (c) 2007-2010 Markus Fisch <mf@markusfisch.de>
 *
 * Licensed under the MIT license:
 * http://www.opensource.org/licenses/mit-license.php
 */
#ifndef _PieDock_Surface_
#define _PieDock_Surface_

namespace PieDock
{
	/**
	 * Abstract surface
	 *
	 * @author Markus Fisch <mf@markusfisch.de>
	 */
	class Surface
	{
		public:
			enum ColorDepth
			{
				Indexed = 8,
				HighColor = 16,
				RGB = 24,
				ARGB = 32
			};

			Surface( Surface & );
			virtual ~Surface();
			inline unsigned char *getData() const { return data; }
			inline const int &getWidth() const { return width; }
			inline const int &getHeight() const { return height; }
			inline const int &getDepth() const { return depth; }
			inline const int &getBytesPerPixel() const { return bytesPerPixel; }
			inline const int &getBytesPerLine() const { return bytesPerLine; }
			inline const int &getPadding() const { return padding; }
			inline const int &getSize() const { return size; }
			Surface &operator=( Surface & );

		protected:
			unsigned char *data;
			int width;
			int height;
			int depth;
			int bytesPerPixel;
			int bytesPerLine;
			int padding;
			int size;

			Surface();
			virtual void calculateSize( int, int, int = ARGB );
			virtual void allocateData();
			virtual void freeData();
	};
}

#endif

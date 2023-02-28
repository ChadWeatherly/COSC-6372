#ifndef __GZ_IMAGE_H_
#define __GZ_IMAGE_H_

#include "GzCommon.h"
using namespace std;

//Bitmap image management class-----------------------------------------------
class GzImage {
public:
	// Constructors, which include width, height, and filename
	GzImage();
	GzImage(GzInt width, GzInt height);
	GzImage(const char* fileName);

	// Attribute/Methods of GzImage
	GzBool load(const char* fileName);
	GzBool save(const char* fileName);
	
	GzInt sizeW();
	GzInt sizeH();
	GzBool resize(GzInt width, GzInt height);
	
	GzColor get(GzInt x, GzInt y);
	GzBool set(GzInt x, GzInt y, const GzColor& c);

	void clear(const GzColor& c);

private:
	// GzColor is itself a vector, so this is a 2x2 matrix where each entry is a vector (R, G, B, A)
	// This is how the program will access individual pixels, by looking at pixel[i][j]
	vector<vector<GzColor> > pixel;
	GzInt w, h;
};
//----------------------------------------------------------------------------

#endif

#ifndef __GZ_FRAME_BUFFER_H_
#define __GZ_FRAME_BUFFER_H_

#include "GzCommon.h"
#include "GzImage.h"
#include <vector>
#include <cmath>

//Frame buffer with Z-buffer -------------------------------------------------
class GzFrameBuffer {
public:
	//The common interface

	// Methods declared beforehand in header
	void initFrameSize(GzInt width, GzInt height);
	GzImage toImage();

	void clear(GzFunctional buffer);
	void setClearColor(const GzColor& color); // Sets color to clear the buffer screen with
	void setClearDepth(GzReal depth); // Sets depth at which to clear all values past

	GzInt get_depth(GzInt x, GzInt y);
	GzBool set_depth(GzInt x, GzInt y, GzInt d);

	GzInt get_width();
	GzInt get_height();

	void drawPoint(const GzVertex& v, const GzColor& c, GzFunctional status);
	double find_intersection(double x1, double y1, double m, double y);
	vector<double> interpolate(const GzTriangle& tri, double x, double y);

	vector<int> MapToPixels(double x, double y, const GzTriangle& t);

	void drawTriangle(const GzVertex& v1, const GzVertex& v2, const GzVertex& v3, 
						const GzColor& c1, const GzColor& c2, const GzColor& c3, 
						GzFunctional status);
	
	

private:
	//Put any variables and private functions for your implementation here
	GzInt w;
	GzInt h;
	GzColor clear_color;
	GzInt clear_depth; // Any Z greater than this will be drawn
	GzImage internal_buffer;
	vector<vector<GzInt> > pixel_depth;
};
//----------------------------------------------------------------------------

#endif

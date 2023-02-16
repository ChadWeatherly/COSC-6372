#ifndef __GZ_FRAME_BUFFER_H_
#define __GZ_FRAME_BUFFER_H_

#include "GzCommon.h"
#include "GzImage.h"
#include <vector>
using namespace std;

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

	void drawPoint(const GzVertex& v, const GzColor& c, GzFunctional status);

private:
	//Put any variables and private functions for your implementation here
	GzInt w;
	GzInt h;
	GzColor clear_color;
	GzInt clear_depth; // Any Z greater than -100 will be drawn
	GzImage internal_buffer;
	vector<vector<GzInt> > pixel_depth;
};
//----------------------------------------------------------------------------

#endif

#ifndef __GZ_FRAME_BUFFER_H_
#define __GZ_FRAME_BUFFER_H_

#include "GzCommon.h"
#include "GzImage.h"
#include "GzVector.h"
#include "GzMatrix.h"
#include <vector>
#include <cmath>
using namespace std;

//Frame buffer with Z-buffer -------------------------------------------------
class GzFrameBuffer {
public:

//============================================================================
//Declarations in Assignment #1, 2, 3
//============================================================================
	//The common interface
	void initFrameSize(GzInt width, GzInt height);
	GzImage toImage();

	void clear(GzFunctional buffer);
	void setClearColor(const GzColor& color);
	void setClearDepth(GzReal depth);

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
	GzImage image;
	vector<vector<GzReal> > depthBuffer;
	GzColor clearColor;
	GzReal clearDepth;

//============================================================================
//End of Declarations in Assignment #1, 2, 3
//============================================================================



//============================================================================
//Declarations in Assignment #4
//============================================================================
public:
	void shadeModel(const GzInt model);						//Set the current shade model (curShadeModel)
	void material(GzReal _kA, GzReal _kD, GzReal _kS, GzReal _s);		//Specify the meterial of the object, includes:
																			//	_kA: The ambient coefficients
																			//	_kD: The diffuse coefficients
																			//	_kS: The specular coefficients
																			//	_s: The spec power
	GzTriangle addShading(GzTriangle t);					// if curShadeModel > 0, then the transformed triangle is shaded

private:
	GzInt curShadeModel = 0;
	
	
	
//============================================================================
//End of Declarations in Assignment #4
//============================================================================



};
//----------------------------------------------------------------------------

#endif

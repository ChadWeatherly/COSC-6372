#ifndef __GZ_COMMON_H_
#define __GZ_COMMON_H_

#include <vector>
using namespace std;



//============================================================================
//Declarations in Assignment #1
//============================================================================

//Common data type------------------------------------------------------------
typedef int GzInt;
typedef bool GzBool;
typedef double GzReal;
typedef unsigned int GzFunctional;
typedef unsigned int GzPrimitiveType;
//----------------------------------------------------------------------------

//Funtional constants---------------------------------------------------------
#define GZ_DEPTH_TEST   0x00000001
#define GZ_COLOR_BUFFER 0x00000002
#define GZ_DEPTH_BUFFER 0x00000004
//----------------------------------------------------------------------------

//Primitive types-------------------------------------------------------------
#define GZ_POINTS 0
//----------------------------------------------------------------------------

//3D coordinate data type-----------------------------------------------------
#define X 0
#define Y 1
#define Z 2

struct GzVertex:public vector<GzReal> {
	GzVertex():vector<GzReal>(3, 0) {}
	GzVertex(GzReal x, GzReal y, GzReal z):vector<GzReal>(3, 0) {
		at(X)=x; at(Y)=y; at(Z)=z;
	}
};
//----------------------------------------------------------------------------

//Color data type-------------------------------------------------------------
#define R 0
#define G 1
#define B 2
#define A 3

#include <iostream>
using namespace std;

struct GzColor:public vector<GzReal> {
	GzColor():vector<GzReal>(4, 0) {at(A)=1;}
	GzColor(GzReal r, GzReal g, GzReal b):vector<GzReal>(4, 0) {
		at(R)=r; at(G)=g; at(B)=b; at(A)=1;
	}
	GzColor(GzReal r, GzReal g, GzReal b, GzReal a):vector<GzReal>(4, 0) {
		at(R)=r; at(G)=g; at(B)=b; at(A)=a;
	}
};
//----------------------------------------------------------------------------

//============================================================================
//End of Declarations in Assignment #1
//============================================================================




//============================================================================
//Declarations in Assignment #2
//============================================================================

//Primitive types-------------------------------------------------------------
#define GZ_TRIANGLES 1
//----------------------------------------------------------------------------

// Triangle struct
struct GzTriangle {
	// Contains all vertices and colors, ordered from lowest x >> highest x
	vector<GzVertex> v;
	vector<GzColor> c;

	float x_min; float x_max;
	float y_min; float y_max;

	/*
	These are the line slopes between each vertex, m:

	m = (y2 - y1) / (x2 - x1)

	Because we want to be able to find the intersections for a given y
	*/
	double m12; double m23; double m13;

	// Constructor
	GzTriangle(GzVertex v1, GzVertex v2, GzVertex v3, GzColor c1, GzColor c2, GzColor c3) {

		// Brute force ordering from lowest x >> highest x
		if (v1[0] <= v2[0]) {
			if (v2[0] <= v3[0]) {
				v.push_back(v1); v.push_back(v2); v.push_back(v3);
				c.push_back(c1); c.push_back(c2); c.push_back(c3);
			}
			else if (v3[0] <= v1[0]) {
				v.push_back(v3); v.push_back(v1); v.push_back(v2);
				c.push_back(c3); c.push_back(c1); c.push_back(c2);
			}
			else {
				v.push_back(v1); v.push_back(v3); v.push_back(v2);
				c.push_back(c1); c.push_back(c3); c.push_back(c2);
			}
		}

		else {
			if (v1[0] <= v3[0]) {
				v.push_back(v2); v.push_back(v1); v.push_back(v3);
				c.push_back(c2); c.push_back(c1); c.push_back(c3);
			}
			else if (v3[0] <= v2[0]) {
				v.push_back(v3); v.push_back(v2); v.push_back(v1);
				c.push_back(c3); c.push_back(c2); c.push_back(c1);
			}
			else {
				v.push_back(v2); v.push_back(v3); v.push_back(v1);
				c.push_back(c2); c.push_back(c3); c.push_back(c1);
			}
		}

		// finding x_min and x_max
		x_min = v[0][0]; x_max = v[2][0];	

		// finding y_min and y_max
		y_min = v1[1]; y_max = v1[1];

		if (v2[1] < y_min) {
			y_min = v2[1];
		}
		else if (v2[1] > y_max) {
			y_max = v2[1];
		}

		if (v3[1] < y_min) {
			y_min = v3[1];
		}
		else if (v3[1] > y_max) {
			y_max = v3[1];
		}

	// Calculating line segments
	m12 = (v[1][1] - v[0][1]) / (v[1][0] - v[0][0]);
	m23 = (v[2][1] - v[1][1]) / (v[2][0] - v[1][0]);
	m13 = (v[2][1] - v[0][1]) / (v[2][0] - v[0][0]);
	}

};

//============================================================================
//End of Declarations in Assignment #2
//============================================================================



#endif

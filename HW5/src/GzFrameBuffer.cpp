#include "GzFrameBuffer.h"
// #include <iostream>

//Put your implementation here------------------------------------------------
#include <climits>
void GzFrameBuffer::initFrameSize(GzInt width, GzInt height) {
	image.resize(width, height);
	depthBuffer=vector<vector<GzReal> >(width, vector<GzReal>(height, clearDepth));
}

GzImage GzFrameBuffer::toImage() {
	return image;
}

void GzFrameBuffer::clear(GzFunctional buffer) {
	if (buffer&GZ_COLOR_BUFFER) image.clear(clearColor);
	if (buffer&GZ_DEPTH_BUFFER)
		for (GzInt x=0; x!=depthBuffer.size(); x++) 
			fill(depthBuffer[x].begin(), depthBuffer[x].end(), clearDepth);
}

void GzFrameBuffer::setClearColor(const GzColor& color) {
	clearColor=color;
}

void GzFrameBuffer::setClearDepth(GzReal depth) {
	clearDepth=depth;
}

void GzFrameBuffer::drawPoint(const GzVertex& v, const GzColor& c, GzFunctional status) {
	GzInt x=(GzInt)v[X];
	GzInt y=image.sizeH()-(GzInt)v[Y]-1;
	if ((x<0)||(y<0)||(x>=image.sizeW())||(y>=image.sizeH())) return;
	if (status&GZ_DEPTH_TEST) {
		if (v[Z]>=depthBuffer[x][y]) {
			image.set(x, y, c);
			depthBuffer[x][y]=v[Z];
		}
	} else {
		image.set(x, y, c);
		depthBuffer[x][y]=v[Z];
	}
}

void GzFrameBuffer::drawTriangle(vector<GzVertex>& v, vector<GzColor>& c, GzFunctional status) {
	GzInt yMin, yMax;
	GzReal xMin, xMax, zMin, zMax;
	GzColor cMin, cMax;

	v.push_back(v[0]);
	c.push_back(c[0]);

	yMin=INT_MAX;
	yMax=-INT_MAX;

	for (GzInt i=0; i<3; i++) {
		yMin=min((GzInt)floor(v[i][Y]), yMin);
		yMax=max((GzInt)floor(v[i][Y]-1e-3), yMax);
	}
		
	for (GzInt y=yMin; y<=yMax; y++) {
		xMin=INT_MAX;
		xMax=-INT_MAX;
		for (GzInt i=0; i<3; i++) {
			if ((GzInt)floor(v[i][Y])==y) {
				if (v[i][X]<xMin) {
					xMin=v[i][X];
					zMin=v[i][Z];
					cMin=c[i];
				}
				if (v[i][X]>xMax) {
					xMax=v[i][X];
					zMax=v[i][Z];
					cMax=c[i];
				}
			}
			if ((y-v[i][Y])*(y-v[i+1][Y])<0) {
				GzReal x;
				realInterpolate(v[i][Y], v[i][X], v[i+1][Y], v[i+1][X], y, x);
				if (x<xMin) {
					xMin=x;
					realInterpolate(v[i][Y], v[i][Z], v[i+1][Y], v[i+1][Z], y, zMin);
					colorInterpolate(v[i][Y], c[i], v[i+1][Y], c[i+1], y, cMin);
				}
				if (x>xMax) {
					xMax=x;
					realInterpolate(v[i][Y], v[i][Z], v[i+1][Y], v[i+1][Z], y, zMax);
					colorInterpolate(v[i][Y], c[i], v[i+1][Y], c[i+1], y, cMax);
				}
			}
		}
		drawRasLine(y, xMin, zMin, cMin, xMax-1e-3, zMax, cMax, status);
	}
}

void GzFrameBuffer::drawRasLine(GzInt y, GzReal xMin, GzReal zMin, GzColor& cMin, GzReal xMax, GzReal zMax, GzColor& cMax, GzFunctional status) {
	if ((y<0)||(y>=image.sizeH())) return;
	if ((GzInt)floor(xMin)==(GzInt)floor(xMax)) {
		if (zMin>zMax) drawPoint(GzVertex(floor(xMin), y, zMin), cMin, status);
			else drawPoint(GzVertex(floor(xMin), y, zMax), cMax, status);
	} else {
		GzReal z;
		GzColor c;
		y=image.sizeH()-y-1;
		int w=image.sizeW();
		if (status&GZ_DEPTH_TEST) {
			for (int x=max(0, (GzInt)floor(xMin)); x<=min(w-1, (GzInt)floor(xMax)); x++) {
				realInterpolate(xMin, zMin, xMax, zMax, x, z);
				if (z>=depthBuffer[x][y]) {
					colorInterpolate(xMin, cMin, xMax, cMax, x, c);
					image.set(x, y, c);
					depthBuffer[x][y]=z;
				}
			}
		} else {
			for (int x=max(0, (GzInt)floor(xMin)); x<=min(w-1, (GzInt)floor(xMax)); x++) {
				realInterpolate(xMin, zMin, xMax, zMax, x, z);
				colorInterpolate(xMin, cMin, xMax, cMax, x, c);
				image.set(x, y, c);
				depthBuffer[x][y]=z;
			}
		}
	}
}

void GzFrameBuffer::realInterpolate(GzReal key1, GzReal val1, GzReal key2, GzReal val2, GzReal key, GzReal& val) {
	val=val1+(val2-val1)*(key-key1)/(key2-key1);
}

void GzFrameBuffer::colorInterpolate(GzReal key1, GzColor& val1, GzReal key2, GzColor& val2, GzReal key, GzColor& val) {
	GzReal k=(key-key1)/(key2-key1);
	for (GzInt i=0; i<4; i++) val[i]=val1[i]+(val2[i]-val1[i])*k;
}

void GzFrameBuffer::shadeModel(const GzInt model) {
	curShadeModel=model;
}

void GzFrameBuffer::material(GzReal _kA, GzReal _kD, GzReal _kS, GzReal _s) {
	kA=_kA;
	kD=_kD;
	kS=_kS;
	s=_s;
}

void GzFrameBuffer::loadLightTrans(GzMatrix& mat) {
	GzMatrix rotMat;
	rotMat.resize(3, 3);
	for (int i=0; i<3; i++)
		for (int j=0; j<3; j++) rotMat[i][j]=mat[i][j];
	lightTrans=(rotMat.inverse3x3()).transpose();
}
	
void GzFrameBuffer::addLight(const GzVector& v, const GzColor& c) {
	GzVector vn(-v[0], -v[1], -v[2]);
	vn.normalize();
	lightDir.push_back(vn);
	lightColor.push_back(c);
}

void GzFrameBuffer::drawPointWLight(const GzVertex& v, const GzColor& c, const GzVector& n, GzFunctional status) {
	GzInt x=(GzInt)v[X];
	GzInt y=image.sizeH()-(GzInt)v[Y]-1;
	if ((x<0)||(y<0)||(x>=image.sizeW())||(y>=image.sizeH())) return;
	GzColor cl=colorWLight(c, n);
	if (status&GZ_DEPTH_TEST) {
		if (v[Z]>=depthBuffer[x][y]) {
			image.set(x, y, cl);
			depthBuffer[x][y]=v[Z];
		}
	} else {
		image.set(x, y, cl);
		depthBuffer[x][y]=v[Z];
	}
}

void GzFrameBuffer::drawTriangleWLight(vector<GzVertex>& v, vector<GzColor>& c, vector<GzVector>& n, GzFunctional status) {
	if (curShadeModel==GZ_GOURAUD) {
		vector<GzColor> cl;
		for (int i=0; i!=c.size(); i++) cl.push_back(colorWLight(c[i], n[i]));
		drawTriangle(v, cl, status);
	}
	if (curShadeModel==GZ_PHONG) {
		GzInt yMin, yMax;
		GzReal xMin, xMax, zMin, zMax;
		GzColor cMin, cMax;
		GzVector nMin, nMax;

		v.push_back(v[0]);
		c.push_back(c[0]);
		n.push_back(n[0]);

		yMin=INT_MAX;
		yMax=-INT_MAX;

		for (GzInt i=0; i<3; i++) {
			yMin=min((GzInt)floor(v[i][Y]), yMin);
			yMax=max((GzInt)floor(v[i][Y]-1e-3), yMax);
		}
			
		for (GzInt y=yMin; y<=yMax; y++) {
			xMin=INT_MAX;
			xMax=-INT_MAX;
			for (GzInt i=0; i<3; i++) {
				if ((GzInt)floor(v[i][Y])==y) {
					if (v[i][X]<xMin) {
						xMin=v[i][X];
						zMin=v[i][Z];
						cMin=c[i];
						nMin=n[i];
					}
					if (v[i][X]>xMax) {
						xMax=v[i][X];
						zMax=v[i][Z];
						cMax=c[i];
						nMax=n[i];
					}
				}
				if ((y-v[i][Y])*(y-v[i+1][Y])<0) {
					GzReal x;
					realInterpolate(v[i][Y], v[i][X], v[i+1][Y], v[i+1][X], y, x);
					if (x<xMin) {
						xMin=x;
						realInterpolate(v[i][Y], v[i][Z], v[i+1][Y], v[i+1][Z], y, zMin);
						colorInterpolate(v[i][Y], c[i], v[i+1][Y], c[i+1], y, cMin);
						normalInterpolate(v[i][Y], n[i], v[i+1][Y], n[i+1], y, nMin);
					}
					if (x>xMax) {
						xMax=x;
						realInterpolate(v[i][Y], v[i][Z], v[i+1][Y], v[i+1][Z], y, zMax);
						colorInterpolate(v[i][Y], c[i], v[i+1][Y], c[i+1], y, cMax);
						normalInterpolate(v[i][Y], n[i], v[i+1][Y], n[i+1], y, nMax);
					}
				}
			}
			drawRasLineWLight(y, xMin, zMin, cMin, nMin, xMax-1e-3, zMax, cMax, nMax, status);
		}
	}
}

void GzFrameBuffer::drawRasLineWLight(GzInt y, GzReal xMin, GzReal zMin, GzColor& cMin, GzVector& nMin, GzReal xMax, GzReal zMax, GzColor& cMax, GzVector& nMax, GzFunctional status) {
	if ((y<0)||(y>=image.sizeH())) return;
	if ((GzInt)floor(xMin)==(GzInt)floor(xMax)) {
		if (zMin>zMax) drawPointWLight(GzVertex(floor(xMin), y, zMin), cMin, nMin, status);
			else drawPointWLight(GzVertex(floor(xMin), y, zMax), cMax, nMax, status);
	} else {
		GzReal z;
		GzColor c;
		GzVector n;
		y=image.sizeH()-y-1;
		int w=image.sizeW();
		if (status&GZ_DEPTH_TEST) {
			for (int x=max(0, (GzInt)floor(xMin)); x<=min(w-1, (GzInt)floor(xMax)); x++) {
				realInterpolate(xMin, zMin, xMax, zMax, x, z);
				if (z>=depthBuffer[x][y]) {
					colorInterpolate(xMin, cMin, xMax, cMax, x, c);
					normalInterpolate(xMin, nMin, xMax, nMax, x, n);
					image.set(x, y, colorWLight(c, n));
					depthBuffer[x][y]=z;
				}
			}
		} else {
			for (int x=max(0, (GzInt)floor(xMin)); x<=min(w-1, (GzInt)floor(xMax)); x++) {
				realInterpolate(xMin, zMin, xMax, zMax, x, z);
				colorInterpolate(xMin, cMin, xMax, cMax, x, c);
				normalInterpolate(xMin, nMin, xMax, nMax, x, n);
				image.set(x, y, colorWLight(c, n));
				depthBuffer[x][y]=z;
			}
		}
	}
}

void GzFrameBuffer::normalInterpolate(GzReal key1, GzVector& val1, GzReal key2, GzVector& val2, GzReal key, GzVector& val) {
	GzReal k=(key-key1)/(key2-key1);
	for (GzInt i=0; i<3; i++) val[i]=val1[i]+(val2[i]-val1[i])*k;
}

GzColor GzFrameBuffer::colorWLight(GzColor c, GzVector n) {
	GzColor res;
	//Ambient
	for (int i=0; i<3; i++) res[i]=kA*c[i];

	for (int l=0; l!=lightDir.size(); l++) {
		//Diffuse
		GzReal nl=dotProduct(n, lightDir[l]);
		for (int i=0; i<3; i++) res[i]+=kD*lightColor[l][i]*nl;

		GzVector r=2*fabs(nl)*n-lightDir[l];
		GzMatrix rm;
		rm.resize(3, 1);
		rm[0][0]=r[0]; rm[1][0]=r[1]; rm[2][0]=r[2];
		rm=lightTrans*rm;
		r[0]=rm[0][0]; r[1]=rm[1][0]; r[2]=rm[2][0];
		r.normalize();
		GzVector e=GzVector(0, 0, 1);
		GzReal re=dotProduct(r, e);
		//Specular
		for (int i=0; i<3; i++) res[i]+=kS*lightColor[l][i]*pow(re, s);
	}

	//Cutoff
	for (int i=0; i<3; i++) res[i]=min(res[i], double(1));
	return res;
}

// Homework 5 implementations for texturing

void GzFrameBuffer::texture(const GzImage& t) {
	tex = t;
}

vector<GzVertex> GzFrameBuffer::MapVToPixels(vector<GzVertex>& v) {
	vector<GzVertex> new_v = v;
	for (GzInt i=0; i<3; i++) {
		new_v[i][X] = floor(((v[i][X]+1) / 2) * image.sizeW());
		new_v[i][Y] = floor(((v[i][Y]+1) / 2) * image.sizeH());
	}
	return new_v;
}

vector<GzTexCoord> GzFrameBuffer::MapUToPixels(vector<GzTexCoord>& u) {
	vector<GzTexCoord> new_u = u;
	for (GzInt i=0; i<3; i++) {
		new_u[i][X] = floor(u[i][X] * tex.sizeW());
		new_u[i][Y] = floor(u[i][Y] * tex.sizeH());
	}
	return new_u;
}

GzReal GzFrameBuffer::TriangleArea(const GzVertex& p1, const GzVertex& p2, const GzVertex& p3) {
	// Formula for triangle gathered from Heron's formula
	GzReal d12 = sqrt(pow((p1[X] - p2[X]), 2) + pow((p1[Y] - p2[Y]), 2));
	GzReal d23 = sqrt(pow((p2[X] - p3[X]), 2) + pow((p2[Y] - p3[Y]), 2));
	GzReal d13 = sqrt(pow((p1[X] - p3[X]), 2) + pow((p1[Y] - p3[Y]), 2));
	GzReal s = (d12 + d23 + d13) / 2;

	GzReal area = sqrt(s * (s-d12) * (s-d23) * (s-d13));
	return area;
}

void GzFrameBuffer::drawTexScanline(const GzInt& y, const vector<GzVertex>& v, const vector<GzTexCoord>& uv, const vector<GzReal>& mins, const vector<GzReal>& maxs, const GzFunctional& status, GzInt projection) {

	for (GzInt x=mins[X]; x<=maxs[X]; x++) {
		GzReal totalArea = TriangleArea(v[0], v[1], v[2]);

		GzVertex p = GzVertex(x, y, 1);

		GzReal area01 = TriangleArea(v[0], v[1], p);
		GzReal area02 = TriangleArea(v[0], v[2], p);
		GzReal area12 = TriangleArea(v[1], v[2], p);

		GzReal pctOff = abs((area01 + area02 + area12) - totalArea) / totalArea;

		// The area of the the 3 triangles made by connecting our test point (x,y) with each set of 2 vertices
		// of the triangle should equal the area of the whole triangle
		// If the sum of the 3 areas is within 1% of the total area, then we assume the point is inside the triangle
		// https://www.geeksforgeeks.org/check-whether-a-given-point-lies-inside-a-triangle-or-not/#
		if (pctOff < 0.01) {
			// We will need to get interpolated values for u and v
			GzReal w0 = area12 / totalArea;
			GzReal w1 = area02 / totalArea;
			GzReal w2 = area01 / totalArea;

			GzReal interpolatedZ = floor(w0*v[0][Z] + w1*v[1][Z] + w2*v[2][Z]);
			GzInt interpolatedU; GzInt interpolatedV;
			GzColor interpolatedColor;
			
			if (projection==GZ_ORTHO) {
				interpolatedU = floor(w0*uv[0][U] + w1*uv[1][U] + w2*uv[2][U]);
				interpolatedV = floor(w0*uv[0][V] + w1*uv[1][V] + w2*uv[2][V]);
			}
			else if (projection==GZ_PERSPECTIVE) {
				vector<GzTexCoord> scaledUV(3);
				vector<GzReal> reciprocalZ(3);
				for (GzInt i=0; i<3; i++) {
					if (v[i][Z] != 1.0) {
						reciprocalZ[i] = 1 / (v[i][Z]-1);
						scaledUV[i][U] = uv[i][U] * reciprocalZ[i]; // Our Z is in [1, -1] so this is mapped to [0, -2] then divided
						scaledUV[i][V] = uv[i][V] * reciprocalZ[i];
					}
					else {
						reciprocalZ[i] = 1;
						scaledUV[i][U] = uv[i][U];
						scaledUV[i][V] = uv[i][V];
					}
				}
				GzReal z = 1 / (w0*reciprocalZ[0] + w1*reciprocalZ[1] + w2*reciprocalZ[2]);
				interpolatedU = floor(w0*scaledUV[0][U] + w1*scaledUV[1][U] + w2*scaledUV[2][U]);
				interpolatedU *= z;
				interpolatedV = floor(w0*scaledUV[0][V] + w1*scaledUV[1][V] + w2*scaledUV[2][V]);
				interpolatedV *= z;
			}
		
			interpolatedColor = tex.get(interpolatedU, interpolatedV);

			GzVertex point (x, y, interpolatedZ);

			drawPoint(point, interpolatedColor, status);
			}

		}
}

void GzFrameBuffer::drawTexTriangle(vector<GzVertex>& v, vector<GzTexCoord>& uv, GzInt projection, const GzFunctional& status) {

	v = MapVToPixels(v);
	uv = MapUToPixels(uv);

	// triangle bounds
	GzReal minX = INT_MAX, minY = INT_MAX, minZ = INT_MAX;
	GzReal maxX = INT_MIN, maxY = INT_MIN, maxZ = INT_MIN;

	for (GzInt i=0; i<3; i++) {

		if (v[i][X] > maxX) (maxX = v[i][X]);
		if (v[i][X] < minX) (minX = v[i][X]);

		if (v[i][Y] > maxY) (maxY = v[i][Y]);
		if (v[i][Y] < minY) (minY = v[i][Y]);

		if (v[i][Z] > maxZ) (maxZ = v[i][Z]);
		if (v[i][Z] < minZ) (minZ = v[i][Z]);

		
	}
	// screen has view where x is in [0, Width]
	// 						 y is in [0, Height]
	//					but  z is in [1, -1] 	(1 is closest to camera)

	vector<GzReal> mins(3);
	mins[0] = minX; mins[1] = minY; mins[2] = minZ;
	vector<GzReal> maxs(3);
	maxs[0] = maxX; maxs[1] = maxY; maxs[2] = maxZ;

	// linearly interpolate pixel values from texture w/ scanline
	for (GzInt scanline=minY; scanline<=maxY; scanline++) {
		drawTexScanline(scanline, v, uv, mins, maxs, status, projection);
	}

}

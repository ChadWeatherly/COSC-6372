#include "Gz.h"
// #include <iostream>
using namespace std;


//============================================================================
//Implementations in Assignment #1
//============================================================================
GzImage Gz::toImage() {
	return frameBuffer.toImage();
}

void Gz::clear(GzFunctional buffer) {
	frameBuffer.clear(buffer);
}

void Gz::clearColor(const GzColor& color) {
	frameBuffer.setClearColor(color);
}

void Gz::clearDepth(GzReal depth) {
	frameBuffer.setClearDepth(depth);
}

void Gz::enable(GzFunctional f) {
	status=status|f;
}

void Gz::disable(GzFunctional f) {
	status=status&(~f);
}

GzBool Gz::get(GzFunctional f) {
	if (status&f) return true; else return false;
}

void Gz::begin(GzPrimitiveType p) {
	currentPrimitive=p;
}

void Gz::addVertex(const GzVertex& v) {
	vertexQueue.push(v);
}

void Gz::addColor(const GzColor& c) {
	colorQueue.push(c);
}
//============================================================================
//End of Implementations in Assignment #1
//============================================================================



//============================================================================
//Implementations in Assignment #3
//============================================================================

// Takes in a vertex and matrix and does the transformation with w
GzVertex Gz::transformVertex(const GzVertex& vt, const GzMatrix& m) {
	
	GzReal v1 = vt[0]; GzReal v2 = vt[1]; GzReal v3 = vt[2];
	GzReal w = 1;

	GzMatrix vmat;
	vmat.resize(4, 1);
	vmat[0][0] = v1; vmat[1][0] = v2; vmat[2][0] = v3; vmat[3][0] = w;

	GzMatrix transVmat = m * vmat;

	GzVertex transV (transVmat[0][0] / transVmat[3][0], transVmat[1][0] / transVmat[3][0], transVmat[2][0] / transVmat[3][0]);

	return transV;

} 

void Gz::initFrameSize(GzInt width, GzInt height) {
	//This function need to be updated since we have introduced the viewport.
	//The viewport size is set to the size of the frame buffer.
	wViewport=(GzReal)width;
	hViewport=(GzReal)height;
	frameBuffer.initFrameSize(width, height);
	viewport(0, 0);			//Default center of the viewport 
}

// Does all the projections/transformations,
// then passes to frameBuffer for rendering
void Gz::end() {
	// cout << "vq: " << vertexQueue.size() << ", cq: " << colorQueue.size() << "\n";

	//In our implementation, all rendering is done when Gz::end() is called.
	//Depends on selected primitive, different number of vetices, colors, ect.
	//are pop out of the queue.
	switch (currentPrimitive) {
		case GZ_POINTS: {
			while ( (vertexQueue.size()>=1) && (colorQueue.size()>=1) ) {
				GzVertex v=vertexQueue.front(); vertexQueue.pop();
				GzColor c=colorQueue.front(); colorQueue.pop();
				frameBuffer.drawPoint(v, c, status);
			}
		} break;
		case GZ_TRIANGLES: {
			//Put your triangle drawing implementation here:
			//   - Pop 3 vertices in the vertexQueue
			//   - Pop 3 colors in the colorQueue
			//   - Call the draw triangle function 
			//     (you may put this function in GzFrameBuffer)

	
			while ( (vertexQueue.size()>=1) && (colorQueue.size()>=1) ) {
				// cout << "vq: " << vertexQueue.size() << ", cq: " << colorQueue.size() << "\n";

				GzVertex v1=vertexQueue.front(); vertexQueue.pop();
				GzVertex v2=vertexQueue.front(); vertexQueue.pop();
				GzVertex v3=vertexQueue.front(); vertexQueue.pop();

				GzColor c1=colorQueue.front(); colorQueue.pop();
				GzColor c2=colorQueue.front(); colorQueue.pop(); 
				GzColor c3=colorQueue.front(); colorQueue.pop();

				// Applying transformation to v before drawing with frameBuffer
				GzVertex tv1 = transformVertex(v1, transMatrix); 
				GzVertex tv2 = transformVertex(v2, transMatrix); 
				GzVertex tv3 = transformVertex(v3, transMatrix); 

				// vector<GzVertex> tv;
				// tv.reserve(3);
				// tv.push_back(tv1); tv.push_back(tv2); tv.push_back(tv3);

				// vector<GzColor> c;
				// c.reserve(3);
				// c.push_back(c1); c.push_back(c2); c.push_back(c3);

				// cout << "vq: " << vertexQueue.size() << ", cq: " << colorQueue.size() << "\n\n";
				
				frameBuffer.drawTriangle(tv1, tv2, tv3, c1, c2, c3, status);
			
				// cout << transMatrix[0][0] << " " << transMatrix[0][1] << " " << transMatrix[0][2] << " " <<  transMatrix[0][3] << "\n";
				// cout << transMatrix[1][0] << " " << transMatrix[1][1] << " " << transMatrix[1][2] << " " << transMatrix[0][3] << "\n";
				// cout << transMatrix[2][0] << " " << transMatrix[2][1] << " " << transMatrix[2][2] << " " << transMatrix[0][3] << "\n";
				// cout << transMatrix[3][0] << " " << transMatrix[3][1] << " " << transMatrix[3][2] << " " << transMatrix[0][3] << "\n";
			}
		}
	}
}

void Gz::viewport(GzInt x, GzInt y) {
	//This function only updates xViewport and yViewport.
	//Viewport calculation will be done in different function, e.g. Gz::end().
	//See http://www.opengl.org/sdk/docs/man/xhtml/glViewport.xml
	//Or google: glViewport
	xViewport=x;
	yViewport=y;
}

//Transformations-------------------------------------------------------------

// From documentation:
// The matrix maps the reference point to the negative z axis and the eye point to the origin
void Gz::lookAt(GzReal eyeX, GzReal eyeY, GzReal eyeZ, GzReal centerX, GzReal centerY, GzReal centerZ, GzReal upX, GzReal upY, GzReal upZ) {
	//Define viewing transformation
	//See http://www.opengl.org/sdk/docs/man/xhtml/gluLookAt.xml
	//Or google: gluLookAt

	// f vector
	vector<GzReal> f;
	f.reserve(3);
	f.push_back(centerX - eyeX); f.push_back(centerY - eyeY); f.push_back(centerZ - eyeZ);
	GzReal f_length = sqrt(pow(f[0], 2) + pow(f[1], 2) + pow(f[2], 2));
	f[0] = f[0] / f_length; f[1] = f[1] / f_length; f[2] = f[2] / f_length;

	// UP vector
	vector<GzReal> up;
	up.reserve(3);
	up.push_back(upX); up.push_back(upY); up.push_back(upZ);
	GzReal up_length = sqrt(pow(up[0], 2) + pow(up[1], 2) + pow(up[2], 2));
	up[0] = up[0] / up_length; up[1] = up[1] / up_length; up[2] = up[2] / up_length;

	// s vector
	// Cross product of A x B = {A[1]*B[2] - A[2]*B[1],
	// 							 A[2]*B[0] - A[0]*B[2] 
	//							 A[0]*B[1] - A[1]*B[0]}						}
	vector<GzReal> s;
	s.reserve(3);
	s.push_back(f[1]*up[2] - f[2]*up[1]); 
	s.push_back(f[2]*up[0] - f[0]*up[2]); 
	s.push_back(f[0]*up[1] - f[1]*up[0]);

	GzReal s_length = sqrt(pow(s[0], 2) + pow(s[1], 2) + pow(s[2], 2));
	vector<GzReal> sNorm;
	sNorm.reserve(3);
	sNorm.push_back(s[0] / s_length); sNorm.push_back(s[1] / s_length); sNorm.push_back(s[2] / s_length);

	// u vector
	vector<GzReal> u;
	u.reserve(3);
	u.push_back(sNorm[1]*f[2] - sNorm[2]*f[1]);
	u.push_back(sNorm[2]*f[0] - sNorm[0]*f[2]);
	u.push_back(sNorm[0]*f[1] - sNorm[1]*f[0]);
	

	GzMatrix m_lookAt = Identity(4);
	m_lookAt[0][0] = s[0]; m_lookAt[0][1] = s[1]; m_lookAt[0][2] = s[2];
	m_lookAt[1][0] = u[0]; m_lookAt[1][1] = u[1]; m_lookAt[1][2] = u[2];
	m_lookAt[2][0] = -f[0]; m_lookAt[2][1] = -f[1]; m_lookAt[2][2] = -f[2];

	transMatrix = Identity(4);

	multMatrix(m_lookAt);
	translate(-eyeX, -eyeY, -eyeZ);

	
}

void Gz::translate(GzReal x, GzReal y, GzReal z) {
	//Multiply transMatrix by a translation matrix
	//See http://www.opengl.org/sdk/docs/man/xhtml/glTranslate.xml
	//    http://en.wikipedia.org/wiki/Translation_(geometry)
	//Or google: glTranslate

	GzMatrix m_translate = Identity(4);
	m_translate[0][3] = x;
	m_translate[1][3] = y;
	m_translate[2][3] = z;

	multMatrix(m_translate);

}

void Gz::rotate(GzReal angle, GzReal x, GzReal y, GzReal z) {
	//Multiply transMatrix by a rotation matrix
	//See http://www.opengl.org/sdk/docs/man/xhtml/glRotate.xml
	//    http://en.wikipedia.org/wiki/Rotation_(geometry)
	//Or google: glRotate

	// angle is given in degrees, but we need it in radians
	GzReal rads = (angle / 180) * M_PI;
	GzReal sa = sin(rads);
	GzReal ca = cos(rads);
	GzReal xyz = x*y*z;

	GzMatrix m_rotate = Identity(4);

	// Row 1
	m_rotate[0][0] = ca + pow(x, 2)*(1 - ca);
	m_rotate[0][1] = x*y*(1 - ca) - z*sa;
	m_rotate[0][2] = x*z*(1 - ca) + y*sa;

	// Row 2
	m_rotate[1][0] = y*x*(1 - ca) + z*sa;
	m_rotate[1][1] = ca + pow(y, 2)*(1 - ca);
	m_rotate[1][2] = y*z*(1 - ca) - x*sa;

	// Row 3
	m_rotate[2][0] = x*z*(1 - ca) - y*sa;
	m_rotate[2][1] = y*z*(1 - ca) + x*sa;
	m_rotate[2][2] = ca + pow(z, 2)*(1 - ca);

	multMatrix(m_rotate);
}

void Gz::scale(GzReal x, GzReal y, GzReal z) {
	//Multiply transMatrix by a scaling matrix
	//See http://www.opengl.org/sdk/docs/man/xhtml/glScale.xml
	//    http://en.wikipedia.org/wiki/
	//Or google: glScale

	GzMatrix m_scale = Identity(4);

	m_scale[0][0] = x;
	m_scale[1][1] = y;
	m_scale[2][2] = z;

	multMatrix(m_scale);

}

//This function was updated on September 26, 2010
void Gz::multMatrix(GzMatrix mat) {
	//Multiply transMatrix by the matrix mat
	transMatrix=transMatrix*mat;
}
//End of Transformations------------------------------------------------------

//Projections-----------------------------------------------------------------
void Gz::perspective(GzReal fovy, GzReal aspect, GzReal zNear, GzReal zFar) {
	//Set up a perspective projection matrix
	//See http://www.opengl.org/sdk/docs/man/xhtml/gluPerspective.xml
	//Or google: gluPerspective

	GzReal rads = (fovy / 180) * M_PI;

	GzMatrix m_perspective = Zeros(4);

	GzReal f = cos(rads/2) / sin(rads/2);

	m_perspective[0][0] = f / aspect;
	m_perspective[1][1] = f;
	m_perspective[2][2] = (zFar + zNear) / (zNear - zFar);
	m_perspective[2][3] = (2*zFar*zNear) / (zNear - zFar);
	m_perspective[3][2] = -1;

	// Change 3rd row to negative values for our implementation
	m_perspective[2][2] = -m_perspective[2][2]; m_perspective[3][3] = -m_perspective[3][3];

	// Multiply it
	multMatrix(m_perspective);

	// cout << "After perspective:" << "\n";
	// cout << transMatrix[0][0] << " " << transMatrix[0][1] << " " << transMatrix[0][2] << " " <<  transMatrix[0][3] << "\n";
	// cout << transMatrix[1][0] << " " << transMatrix[1][1] << " " << transMatrix[1][2] << " " << transMatrix[0][3] << "\n";
	// cout << transMatrix[2][0] << " " << transMatrix[2][1] << " " << transMatrix[2][2] << " " << transMatrix[0][3] << "\n";
	// cout << transMatrix[3][0] << " " << transMatrix[3][1] << " " << transMatrix[3][2] << " " << transMatrix[0][3] << "\n\n";

}

// From documentation:
// describes a transformation that produces a parallel projection
void Gz::orthographic(GzReal left, GzReal right, GzReal bottom, GzReal top, GzReal nearVal, GzReal farVal) {
	//Set up a orthographic projection matrix
	//See http://www.opengl.org/sdk/docs/man/xhtml/glOrtho.xml
	//Or google: glOrtho

	GzMatrix m_ortho = Identity(4);

	// Row 1
	m_ortho[0][0] = 2 / (right - left);
	GzReal t_x = - (right + left) / (right - left);
	m_ortho[0][3] = t_x;

	// Row 2
	m_ortho[1][1] = 2 / (top - bottom);
	GzReal t_y = - (top + bottom) / (top - bottom);
	m_ortho[1][3] = t_y;

	// Row 3
	m_ortho[2][2] = -2 / (farVal - nearVal);
	GzReal t_z = - (farVal + nearVal) / (farVal - nearVal);
	m_ortho[2][3] = t_z;

	// Multiply it ???
	multMatrix(m_ortho);

	// perspective(60, 1, nearVal, farVal);

}
//End of Projections----------------------------------------------------------


//============================================================================
//End of Implementations in Assignment #3
//============================================================================

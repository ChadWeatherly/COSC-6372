#include "GzFrameBuffer.h"

//Put your implementation here------------------------------------------------

void GzFrameBuffer::initFrameSize(GzInt width, GzInt height) {
    // cout << "check: " << "/n";
    w = width;
    h = height;
    clear_depth = -100;

    // cout << "check: " << "/n";
    pixel_depth.resize(width);
    // cout << "pixel_depth size: " << pixel_depth.size() << "/n";
    // fill(pixel_depth.begin(), pixel_depth.end()-1, vector<GzInt>(clear_depth));
    // internal_buffer is the image being created
    internal_buffer.resize(width, height);
    for (int x=0; x<w; x++)  {
            // cout << "pixel_depth size: " << pixel_depth.size() << "/n";
			pixel_depth[x] = vector<GzInt>(h, clear_depth);
		}
}

GzImage GzFrameBuffer::toImage() {
    return internal_buffer;
}

void GzFrameBuffer::clear(GzFunctional buffer) {
    // fill screen of h x w with clear color up to clear_depth
    for (int x=0; x<w; x++) 
		for (int y=0; y<h; y++) 
            if (pixel_depth[x][y] < clear_depth) {
			    pixel_depth[x][y] = clear_depth;
                internal_buffer.set(x, y, clear_color);
		}
}

// const GzColor& color is a reference to color
void GzFrameBuffer::setClearColor(const GzColor& color) {
    clear_color = color;
}
void GzFrameBuffer::setClearDepth(GzReal depth) {
    clear_depth = depth;
}

GzInt GzFrameBuffer::get_depth(GzInt x, GzInt y) {
    if ((x<0)||(y<0)||(x>=w)||(y>=h)) {
        return -100;
    }
	return pixel_depth[x][y];
}
	
GzBool GzFrameBuffer::set_depth(GzInt x, GzInt y, GzInt d) {
    if ((x<0)||(y<0)||(x>=w)||(y>=h)) return false;
	pixel_depth[x][y]=d;
	return true;
}

/*
Since a reference is passed to drawPoint, anything we do to those referenced
variables will inherently change the given values
*/
void GzFrameBuffer::drawPoint(const GzVertex& v, const GzColor& c, GzFunctional status) {
    // v is a vertex, a vector of size 3
    // c is a color, a vector of size 4
    int x = v[0]; int y = v[1]; int z = v[2];
    // cout << "pixel_depth size: " << pixel_depth.size() << "/n";
    if (get_depth(x, y) < z) {
        internal_buffer.set(x, y, c);
        set_depth(x, y, z);
    }
}
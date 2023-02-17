#include "GzFrameBuffer.h"

//Put your implementation here------------------------------------------------

void GzFrameBuffer::initFrameSize(GzInt width, GzInt height) {
    w = width;
    h = height;
    clear_depth = -100;
    pixel_depth.resize(width);
    // internal_buffer is the image being created
    internal_buffer.resize(width, height);
    for (GzInt x=0; x<w; x++) 
		for (GzInt y=0; y<h; y++) {
			pixel_depth[x][y] = clear_depth;
		}
}

GzImage GzFrameBuffer::toImage() {
    return internal_buffer;
}

void GzFrameBuffer::clear(GzFunctional buffer) {
    // fill screen of h x w with clear color up to clear_depth
    for (GzInt x=0; x<w; x++) 
		for (GzInt y=0; y<h; y++) 
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

void GzFrameBuffer::drawPoint(const GzVertex& v, const GzColor& c, GzFunctional status) {
    // v is a vertex, a vector of size 3
    // c is a color, a vector of size 4
    GzVertex temp_v = v;
    GzColor temp_c = c;
    GzInt x = temp_v[0]; GzInt y = temp_v[1]; GzInt z = temp_v[2];
    if (pixel_depth[x][y] < z) {internal_buffer.set(x, y, temp_c);}
}
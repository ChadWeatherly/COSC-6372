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

GzInt GzFrameBuffer::get_height() {return h;}
GzInt GzFrameBuffer::get_width() {return w;}

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

double GzFrameBuffer::find_intersection(double x1, double y1, double m, double y) {
    /* 
    y - y1 = m(x - x1)

    translates to

    [ (y - y1) / m ] + x1 = x
    */
   if (m==0) {return -1;}
   double x = ( (y - y1) / m ) + x1;
   return x;
}

// interpolate z_value for a given (x, y) in a triangle
vector<double> GzFrameBuffer::interpolate(const GzTriangle& tri, double x, double y) {
    double d1; double d2; double d3; // distances
    vector<double> vec_int;

    d1 = sqrt( pow((x - tri.v[0][0]), 2) + pow((y - tri.v[0][1]), 2) );
    d2 = sqrt( pow((x - tri.v[1][0]), 2) + pow((y - tri.v[1][1]), 2) );
    d3 = sqrt( pow((x - tri.v[2][0]), 2) + pow((y - tri.v[2][1]), 2) );

    d1 = d1 / (d1 + d2 + d3); 
    d2 = d2 / (d1 + d2 + d3); 
    d3 = d3 / (d1 + d2 + d3);

    double new_z = d1*tri.v[0][2] + d2*tri.v[1][2] + d3*tri.v[2][2];
    vec_int.push_back(new_z); // interpolated z

    double new_c1 = d1*tri.c[0][0] + d2*tri.c[1][0] + d3*tri.c[2][0];
    double new_c2 = d1*tri.c[0][1] + d2*tri.c[1][1] + d3*tri.c[2][1];
    double new_c3 = d1*tri.c[0][2] + d2*tri.c[1][2] + d3*tri.c[2][2];
    vec_int.push_back(new_c1);
    vec_int.push_back(new_c2);
    vec_int.push_back(new_c3);
    // Adds each of the 3 elements of the interpolated color RGB value to the back

    return vec_int;
}

vector<int> GzFrameBuffer::MapToPixels(double x, double y, const GzTriangle& t) {
    vector<int> new_pixels;

    int x_pixel = floor(((x - t.x_min) / (t.x_max - t.x_min)) * get_width());
    int y_pixel = floor(((y - t.y_min) / (t.y_max - t.y_min)) * get_height());

    new_pixels.push_back(x_pixel);
    new_pixels.push_back(y_pixel);

    return new_pixels;
}

/*

*/
void GzFrameBuffer::drawTriangle(const GzVertex& v1, const GzVertex& v2, const GzVertex& v3, 
                                    const GzColor& c1, const GzColor& c2, const GzColor& c3, 
                                    GzFunctional status) {
    // v1, v2, and v3 are the 3 vertices
    // c is the color (only need one since entire triangle is one color)
    GzTriangle tri(v1, v2, v3, c1, c2, c3);

    // for loop through y_min >> y_max (scanline)
    vector<double> intersections;
    int num_intersections;
    int fill_left = -1; int fill_right = -1;

    for (double scanline = floor(tri.y_min); scanline <= ceil(tri.y_max); scanline++) {
        num_intersections = 0;
        intersections.resize(0);

        // Find possible intersections at scanline
        if (tri.m12 != 0) {
            double x12 = find_intersection(tri.v[0][0], tri.v[0][1], tri.m12, scanline);
            if (x12 >= tri.x_min && x12 <= tri.x_max) {
                num_intersections++;
                intersections.push_back(x12);
            }
        }

        if (tri.m23 != 0) {
            double x23 = find_intersection(tri.v[1][0], tri.v[1][1], tri.m23, scanline);
            if (x23 >= tri.x_min && x23 <= tri.x_max) {
                num_intersections++;
                intersections.push_back(x23);
            }
        }

        if (tri.m13 != 0) {
            double x13 = find_intersection(tri.v[0][0], tri.v[0][1], tri.m13, scanline);
            if (x13 >= tri.x_min && x13 <= tri.x_max) {
                num_intersections++;
                intersections.push_back(x13);
            }
        }

        // If num_intersections == 2, then we need to fill in between two intersections
        if (num_intersections == 2) {
            if (intersections[0] < intersections[1]) {
                fill_left = round(intersections[0]); fill_right = round(intersections[1]);
            }
            else if (intersections[1] < intersections[0]) {
                fill_left = round(intersections[1]); fill_right = round(intersections[0]);
            }
            else {continue;}

            for (double x=fill_left; x <= fill_right; x++) {
                vector<double> new_vals = interpolate(tri, x, scanline);
                vector<int> pixel_points = MapToPixels(x, scanline, tri);
                drawPoint(GzVertex(x, h - scanline, new_vals[0]), GzColor(new_vals[1], new_vals[2], new_vals[3]), status);
            }

        }

    }


}
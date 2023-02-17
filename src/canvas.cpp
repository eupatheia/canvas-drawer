/* canvas.cpp
 * Implementation of a Canvas class that can draw lines and triangles on
 * a raster image, given specified vertices and colors
 * @author JL
 * @version February 16, 2023
 */

#include "canvas.h"
#include <cassert>
#include <cmath>

using namespace std;
using namespace agl;

Canvas::Canvas(int w, int h) : _canvas(w, h) {
  _color = {0, 0, 0};  // default black color
  _primitive = UNDEFINED;  // nothing being drawn
}

Canvas::~Canvas() {  }  // Image destructor should free canvas already

void Canvas::save(const std::string& filename) {
  // save image as png file
  _canvas.save(filename);
}

void Canvas::begin(PrimitiveType type) {
  if (_primitive == UNDEFINED && type != UNDEFINED) {
    // set primitive to signal "drawing in progress"
    _primitive = type;
  } else {
    // still have a drawing in progress (i.e. end() was not called yet)
    cout << "Error: cannot begin new drawing without ending previous" << endl;
  }
}

void Canvas::end() {
  // draw the primitive specified
  if (_primitive == LINES) {
    drawLines();
  } else if (_primitive == TRIANGLES) {
    drawTriangles();
  }
  _primitive = UNDEFINED;  // signal no further drawing
  _vertices.clear();  // reset vertex list
}

// Specify a vertex at raster position (x,y)
// x corresponds to the column, y to the row
void Canvas::vertex(int x, int y) {
  if (_primitive != UNDEFINED) {
      // clip vertex to size of canvas
      if (x < 0) {
        x = 0;
      } else if (x >= _canvas.width()) {
        x = _canvas.width() - 1;
      }
      if (y < 0) {
        y = 0;
      } else if (y >= _canvas.height()) {
        y = _canvas.height() - 1;
      }
      Vertex vertex = {x, y, _color};
      _vertices.push_back(vertex);
  } else {  // _primitive == UNDEFINED
    // should not add vertices without specifying type with begin()
    cout << "Error: cannot add vertices without specifying type" << endl;
  }
}

void Canvas::color(unsigned char r, unsigned char g, unsigned char b) {
  _color = {r, g, b};
}

void Canvas::background(unsigned char r, unsigned char g, unsigned char b) {
  int numPixels = _canvas.width() * _canvas.height();
  Pixel color = {r, g, b};
  // color every pixel, erasing any drawn lines
  for (int i = 0; i < numPixels; i++) {
    _canvas.set(i, color);
  }
}

void Canvas::drawLines() {
  int numLines = _vertices.size() / 2;
  for (int i = 0; i < numLines; i++) {
    Vertex a = _vertices[i * 2];
    Vertex b = _vertices[i * 2 + 1];
    int w = b.x - a.x;
    int h = b.y - a.y;
    if (abs(h) < abs(w)) {
      if (a.x > b.x) {
        // swap a and b
        Vertex temp = b;
        b = a;
        a = temp;
      }
      drawLineLow(a, b);
    } else {
      if (a.y > b.y) {
        // swap a and b
        Vertex temp = b;
        b = a;
        a = temp;
      }
      drawLineHigh(a, b);
    }
  }
}

void Canvas::drawLineLow(Vertex a, Vertex b) {
  int y = a.y;
  int w = b.x - a.x;  // width
  int h = b.y - a.y;  // height
  int dy = 1;  // change in y
  if (h < 0) {  // line sloping down
    dy = -1;
    h *= -1;
  }
  int F = (2 * h) - w;
  for (int x = a.x; x <= b.x; x++) {
    // y = col j, x = row i
    _canvas.set(y, x, interpolLinear(a, b, x, y));
    if (F > 0) {
      y += dy;
      F += 2 * (h - w);
    } else {
      F += 2 * h;
    }
  }
}

void Canvas::drawLineHigh(Vertex a, Vertex b) {
  int x = a.x;
  int w = b.x - a.x;  // width
  int h = b.y - a.y;  // height
  int dx = 1;  // change in x
  if (w < 0) {  // line sloping down
    dx = -1;
    w *= -1;
  }
  int F = (2 * w) - h;
  for (int y = a.y; y <= b.y; y++) {
    // y = col j, x = row i
    _canvas.set(y, x, interpolLinear(a, b, x, y));
    if (F > 0) {
      x += dx;
      F += 2 * (w - h);
    } else {
      F += 2 * w;
    }
  }
}

void Canvas::drawTriangles() {
  int numTriangles = _vertices.size() / 3;
  for (int i = 0; i < numTriangles; i++) {
    Vertex p0 = _vertices[i * 3];
    Vertex p1 = _vertices[i * 3 + 1];
    Vertex p2 = _vertices[i * 3 + 2];
    // compute min and max among 3 vertices, i.e. bounding box
    int xmin = min(min(p0.x, p1.x), p2.x);
    int xmax = max(max(p0.x, p1.x), p2.x);
    int ymin = min(min(p0.y, p1.y), p2.y);
    int ymax = max(max(p0.y, p1.y), p2.y);
    // iterate over bounding box
    for (int y = ymin; y <= ymax; y++) {
      for (int x = xmin; x <= xmax; x++) {
        // compute barycentric coordinates
        float fAlpha = implicit(p1, p2, p0.x, p0.y);
        float fBeta = implicit(p2, p0, p1.x, p1.y);
        float fGamma = implicit(p0, p1, p2.x, p2.y);
        float alpha = implicit(p1, p2, x, y) / fAlpha;
        float beta = implicit(p2, p0, x, y) / fBeta;
        float gamma = implicit(p0, p1, x, y) / fGamma;
        if (alpha >= 0 && beta >= 0 and gamma >= 0) {
          // use (-5, -1.1) as offscreen comparator point
          if ((alpha > 0 || (fAlpha * implicit(p1, p2, -5, -1.1)) > 0) &&
              (beta > 0 || (fBeta * implicit(p2, p0, -5, -1.1)) > 0) &&
              (gamma > 0 || (fGamma * implicit(p0, p1, -5, -1.1)) > 0)) {
            // point is inside or on edge that this triangle owns
            // y = col j, x = row i
            _canvas.set(y, x, interpolGouraud(p0, p1, p2, alpha, beta, gamma));
          }
        }
      }
    }
  }
}

Pixel Canvas::interpolLinear(const Vertex p1, const Vertex p2, int x, int y) {
  float t = (sqrt(pow(x - p1.x, 2) + pow(y - p1.y, 2))) /
      (sqrt(pow(p2.x - p1.x, 2) + pow(p2.y - p1.y, 2)));
  struct Pixel c;
  c.r = p1.color.r * (1 - t) + p2.color.r * t;
  c.g = p1.color.g * (1 - t) + p2.color.g * t;
  c.b = p1.color.b * (1 - t) + p2.color.b * t;
  return c;
}

Pixel Canvas::interpolGouraud(const Vertex p0, const Vertex p1,
    const Vertex p2, float alpha, float beta, float gamma) {
  Pixel c;
  // use gouraud shading interpolation
  c.r = alpha * p0.color.r + beta * p1.color.r + gamma * p2.color.r;
  c.g = alpha * p0.color.g + beta * p1.color.g + gamma * p2.color.g;
  c.b = alpha * p0.color.b + beta * p1.color.b + gamma * p2.color.b;
  return c;
}

float Canvas::implicit(const Vertex a, const Vertex b, float px, float py) {
  return (b.y - a.y) * (px - a.x) - (b.x - a.x) * (py - a.y);
}
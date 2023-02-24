/* canvas.cpp
 * Implementation of a Canvas class that can draw lines, triangles, and
 * circles on a raster image, given specified vertices and colors
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
  _lineWidth = 1;  // default one-pixel lines
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
    drawLines(_vertices);
  } else if (_primitive == TRIANGLES) {
    drawTriangles();
  } else if (_primitive == CIRCLES) {
    drawCircles();
  } else if (_primitive == ROSES) {
    drawRoses();
  } else if (_primitive == MAURERS) {
    drawMaurers();
  }
  _primitive = UNDEFINED;  // signal no further drawing
  _vertices.clear();  // reset vertex list
}

// Specify a vertex at raster position (x,y)
// x corresponds to the column, y to the row
void Canvas::vertex(int x, int y) {
  if (_primitive == LINES || _primitive == TRIANGLES) {
    // clip vertex to size of canvas
    Vertex vertex = {x, y, 1, 0, 0, _color, _lineWidth};
    clamp(vertex);
    _vertices.push_back(vertex);
  } else {
    // should not add vertices without specifying type with begin()
    cout << "Error: cannot add vertices to invalid type" << endl;
  }
}

void Canvas::center(int x, int y, int radius, int n, int d) {
  if (_primitive == CIRCLES) {
    // don't necessarily need to clamp center coordinates,
    // just clamp lines when drawing circumference
    Vertex center = {x, y, radius, 0, 0, _color, _lineWidth};
    _vertices.push_back(center);
  } else if (_primitive == ROSES || _primitive == MAURERS) {
    // "radius" parameter treated as amplitude for rose curves
    Vertex center = {x, y, radius, n, d, _color, _lineWidth};
    _vertices.push_back(center);
  } else {
    cout << "Error: cannot draw circle without specifying circular type" << endl;
  }
}

void Canvas::color(unsigned char r, unsigned char g, unsigned char b) {
  _color = {r, g, b};
}

void Canvas::lineWidth(int width) {
  if (width > 0) {
    _lineWidth = width;
  } else {
    cout << "Error: cannot set a non-positive line width" << endl;
  }
}

void Canvas::background(unsigned char r, unsigned char g, unsigned char b) {
  int numPixels = _canvas.width() * _canvas.height();
  Pixel color = {r, g, b};
  // color every pixel, erasing any drawn lines
  for (int i = 0; i < numPixels; i++) {
    _canvas.set(i, color);
  }
}

//------------------------------------------------------------//
//------------------------------------------------------------//

void Canvas::drawLines(vector<Vertex>& points) {
  int numLines = points.size() / 2;
  for (int i = 0; i < numLines; i++) {
    Vertex a = points[i * 2];
    Vertex b = points[i * 2 + 1];
    int w = b.x - a.x;
    int h = b.y - a.y;
    if (abs(h) < abs(w)) {
      if (a.x > b.x) {
        // swap a and b
        Vertex temp = b;
        b = a;
        a = temp;
      }
      drawLineLow(a, b, a.width);
    } else {
      if (a.y > b.y) {
        // swap a and b
        Vertex temp = b;
        b = a;
        a = temp;
      }
      drawLineHigh(a, b, a.width);
    }
  }
}

void Canvas::drawLineLow(Vertex& a, Vertex& b, int width) {
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
    Pixel col = interpolLinear(a, b, x, y);
    _canvas.set(y, x, col);
    // color up to line width horizontally
    for (int i = 1; i < width; i++) {
      int offset = ((i + 1) / 2) * pow(-1, i);
      Vertex v = {x + offset, y, 0, 0, 0, col, 0};
      clamp(v);
      // y = row i, x = col j
      _canvas.set(v.y, v.x, col);
    }
    if (F > 0) {
      y += dy;
      F += 2 * (h - w);
    } else {
      F += 2 * h;
    }
  }
}

void Canvas::drawLineHigh(Vertex& a, Vertex& b, int width) {
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
    Pixel col = interpolLinear(a, b, x, y);
    _canvas.set(y, x, col);
    // color up to line width vertically
    for (int i = 1; i < width; i++) {
      int offset = ((i + 1) / 2) * pow(-1, i);
      Vertex v = {x, y + offset, 0, 0, 0, col, 0};
      clamp(v);
      // y = row i, x = col j
      _canvas.set(v.y, v.x, col);
    }
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

void Canvas::drawCircles() {
  for (int i = 0; i < _vertices.size(); i++) {
    int cx = _vertices[i].x;  // center x
    int cy = _vertices[i].y;  // center y
    int r = _vertices[i].radius;
    Pixel color = _vertices[i].color;
    vector<Vertex> points;
    // use 2r points to approximate the circle
    float delta = (2 * M_PI) / (2 * r);
    for (float theta = 0.0; theta <= 2 * M_PI; theta += delta) {
      Vertex a, b;
      a.x = round(cx + (r * cos(theta)));
      a.y = round(cy + (r * sin(theta)));
      b.x = round(cx + (r * cos(theta + delta)));
      b.y = round(cy + (r * sin(theta + delta)));
      a.color = color;
      b.color = color;
      clamp(a);
      clamp(b);
      points.push_back(a);
      points.push_back(b);
    }
    drawLines(points);
  }
}

void Canvas::drawRoses() {
  for (int i = 0; i < _vertices.size(); i++) {
    int cx = _vertices[i].x;  // center x
    int cy = _vertices[i].y;  // center y
    int amp = _vertices[i].radius;
    int n = _vertices[i].n;
    int d = _vertices[i].d;
    Pixel color = _vertices[i].color;
    vector<Vertex> points;
    // use 361 * d points to approximate the rose curve
    for (int j = 0; j < 361 * d; j++) {
      Vertex a, b;
      // multiply by angular frequency n/d and convert degrees to radians
      float theta = j * (M_PI / 180);
      float nextTheta = (j + 1) * (M_PI / 180);
      float rad = (j * ((float) n / d)) * (M_PI / 180);
      float nextRad = ((j + 1) * ((float) n / d)) * (M_PI / 180);
      float r = amp * cos(rad);
      float nextR = amp * cos(nextRad);
      a.x = round(cx + (r * cos(theta)));
      a.y = round(cy + (r * sin(theta)));
      b.x = round(cx + (nextR * cos(nextTheta)));
      b.y = round(cy + (nextR * sin(nextTheta)));
      a.color = color;
      b.color = color;
      clamp(a);
      clamp(b);
      points.push_back(a);
      points.push_back(b);
    }
    drawLines(points);
  }
}

void Canvas::drawMaurers() {
  for (int i = 0; i < _vertices.size(); i++) {
    int cx = _vertices[i].x;  // center x
    int cy = _vertices[i].y;  // center y
    int amp = _vertices[i].radius;
    int n = _vertices[i].n;
    int d = _vertices[i].d;
    Pixel color = _vertices[i].color;
    vector<Vertex> points;
    // draw lines to connect the 361 points on the rose curve
    for (int j = 0; j < 361; j++) {
      Vertex a, b;
      // multiply by angular frequency n/d and convert degrees to radians
      float k = j * d;
      float nextK = (j + 1) * d;
      float theta = k * (M_PI / 180);
      float nextTheta = nextK * (M_PI / 180);
      float rad = n * k * (M_PI / 180);
      float nextRad = n * nextK * (M_PI / 180);
      float r = amp * cos(rad);
      float nextR = amp * cos(nextRad);
      a.x = round(cx + (r * cos(theta)));
      a.y = round(cy + (r * sin(theta)));
      b.x = round(cx + (nextR * cos(nextTheta)));
      b.y = round(cy + (nextR * sin(nextTheta)));
      a.color = color;
      b.color = color;
      clamp(a);
      clamp(b);
      points.push_back(a);
      points.push_back(b);
    }
    drawLines(points);
  }
}

Pixel Canvas::interpolLinear(const Vertex& p1, const Vertex& p2, int x, int y) {
  float t = (sqrt(pow(x - p1.x, 2) + pow(y - p1.y, 2))) /
      (sqrt(pow(p2.x - p1.x, 2) + pow(p2.y - p1.y, 2)));
  struct Pixel c;
  c.r = p1.color.r * (1 - t) + p2.color.r * t;
  c.g = p1.color.g * (1 - t) + p2.color.g * t;
  c.b = p1.color.b * (1 - t) + p2.color.b * t;
  return c;
}

Pixel Canvas::interpolGouraud(const Vertex& p0, const Vertex& p1,
    const Vertex& p2, float alpha, float beta, float gamma) {
  Pixel c;
  // use gouraud shading interpolation
  c.r = alpha * p0.color.r + beta * p1.color.r + gamma * p2.color.r;
  c.g = alpha * p0.color.g + beta * p1.color.g + gamma * p2.color.g;
  c.b = alpha * p0.color.b + beta * p1.color.b + gamma * p2.color.b;
  return c;
}

float Canvas::implicit(const Vertex& a, const Vertex& b, float px, float py) {
  return (b.y - a.y) * (px - a.x) - (b.x - a.x) * (py - a.y);
}

void Canvas::clamp(Vertex& v) {
  if (v.x < 0) {
    v.x = 0;
  } else if (v.x >= _canvas.width()) {
    v.x = _canvas.width() - 1;
  }
  if (v.y < 0) {
    v.y = 0;
  } else if (v.y >= _canvas.height()) {
    v.y = _canvas.height() - 1;
  }
}
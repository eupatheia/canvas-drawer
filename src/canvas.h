/*-----------------------------------------------
 * canvas.h
 * Author: JL
 * Date: February 16, 2023
 * Description: header file for canvas.cpp
 ----------------------------------------------*/

#ifndef canvas_H_
#define canvas_H_

#include <string>
#include <vector>
#include "image.h"

namespace agl {

  // defines type to draw (how to connect vertices)
  // UNDEFINED = not ready to draw, not accepting vertices
  enum PrimitiveType {UNDEFINED, LINES, TRIANGLES};

  // representation of vertex with coordinates and color
  struct Vertex {
    int x;  // column (on x-axis)
    int y;  // row (on y-axis)
    Pixel color;
  };

  class Canvas {
    public:
      Canvas(int w, int h);
      virtual ~Canvas();

      // Save to file
      void save(const std::string& filename);

      // Draw primitives with a given type (either LINES or TRIANGLES)
      // For example, the following draws a red line followed by a green line
      // begin(LINES);
      //    color(255,0,0);
      //    vertex(0,0);
      //    vertex(100,0);
      //    color(0,255,0);
      //    vertex(0, 0);
      //    vertex(0,100);
      // end();
      void begin(PrimitiveType type);
      void end();

      // Specify a vertex at raster position (x,y)
      // x corresponds to the column; y to the row
      void vertex(int x, int y);

      // Specify a color with components in range [0,255]
      void color(unsigned char r, unsigned char g, unsigned char b);

      // Fill the canvas with the given background color
      void background(unsigned char r, unsigned char g, unsigned char b);

    private:
      Image _canvas;
      Pixel _color;  // current color
      PrimitiveType _primitive;  // current primitive being drawn
      std::vector<Vertex> _vertices;  // list of vertices to draw

      // treat each pair of unique vertices as endpoints of a line
      void drawLines();
      // helper function to draw low line in Bresenham's
      void drawLineLow(Vertex a, Vertex b);
      // helper function to draw high line in Bresenham's
      void drawLineHigh(Vertex a, Vertex b);

      // treat each triplet of unique vertices as vertices of a triangle
      void drawTriangles();

      // helper function to get linear interpolated color between c1 and c2
      Pixel interpolLinear(const Vertex p1, const Vertex p2, int x, int y);

      // helper function to get gouraud-shaded color in triangle between
      // points p0, p1, and p2 at barycentric coordinate (alpha, beta, gamma)
      Pixel interpolGouraud(const Vertex p0, const Vertex p1, const Vertex p2,
          float alpha, float beta, float gamma);

      // helper function to compute implicit line function through points
      // a and b, given input point (px,py)
      float implicit(const Vertex a, const Vertex b, float px, float py);
  };
}

#endif



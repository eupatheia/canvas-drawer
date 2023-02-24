/* draw_art.cpp
 * tests functions from canvas.cpp to draw certain images
 * @author JL
 * @version February 23, 2023
 */

#include <iostream>
#include "canvas.h"
using namespace std;
using namespace agl;

int main(int argc, char** argv) {
  Canvas drawer(640, 640);
  drawer.background(0, 0, 0);

  drawer.begin(CIRCLES);
  drawer.color(255, 0, 0);
  drawer.center(320, 320, 200, 0, 0, true);
  drawer.color(255, 255, 255);
  for (int i = 50; i <= 400; i += 50) {
    drawer.center(100 + i, 100 + i, i / 5);
  }
  drawer.end();
  drawer.save("test_outline_circle.png");

  drawer.begin(TRIANGLES);
  drawer.background(0, 0, 0);
  drawer.color(255, 0, 0);
  drawer.vertex(320, 160, true);
  drawer.vertex(160, 480, true);
  drawer.color(0, 255, 255);
  drawer.vertex(480, 480, true);
  drawer.color(255, 255, 255);
  for (int i = 50; i <= 400; i += 50) {
    drawer.vertex(550 - 50, 100 + 50);
    drawer.vertex(520 - i, 160 + i);
    drawer.vertex(580 - i, 160 + i);
  }
  drawer.end();
  drawer.save("test_outline_triangle.png");
  
  drawer.begin(CIRCLES);
  drawer.background(0, 0, 0);
  drawer.color(255, 255, 255);
  for (int i = 50; i <= 400; i += 50) {
    drawer.center(320, 320, i);
  }
  drawer.end();
  drawer.save("test_circle.png");

  drawer.begin(ROSES);
  drawer.background(0, 0, 0);
  drawer.color(255, 255, 255);
  drawer.center(320, 320, 300, 5, 4);
  drawer.end();
  drawer.save("test_rose.png");

  drawer.begin(MAURERS);
  drawer.background(0, 0, 0);
  drawer.color(255, 255, 255);
  drawer.center(320, 320, 300, 2, 29);
  drawer.end();
  drawer.save("test_maurer.png");

  drawer.begin(TRIANGLES);
  drawer.background(0, 0, 0);
  drawer.color(255, 255, 255);
  drawer.vertex(320, 320, true);
  drawer.color(0, 0, 0);
  drawer.vertex(0, 0, true);
  drawer.vertex(640, 0, true);
  drawer.color(255, 255, 255);
  drawer.vertex(320, 320, true);
  drawer.color(0, 0, 0);
  drawer.vertex(0, 0, true);
  drawer.vertex(0, 640, true);
  drawer.color(255, 255, 255);
  drawer.vertex(320, 320, true);
  drawer.color(0, 0, 0);
  drawer.vertex(0, 640, true);
  drawer.vertex(640, 640, true);
  drawer.color(255, 255, 255);
  drawer.vertex(320, 320, true);
  drawer.color(0, 0, 0);
  drawer.vertex(640, 640, true);
  drawer.vertex(640, 0, true);
  drawer.end();
  drawer.begin(ROSES);
  drawer.color(50, 0, 0);
  drawer.center(320, 320, 200, 6, 1);
  drawer.center(320, 320, 250, 6, 1);
  drawer.center(320, 320, 300, 6, 1);
  drawer.center(320, 320, 350, 6, 1);
  drawer.color(100, 0, 0);
  drawer.center(320, 320, 200, 6, 2);
  drawer.color(200, 0, 0);
  drawer.center(320, 320, 200, 6, 4);
  drawer.end();
  drawer.begin(MAURERS);
  drawer.color(255, 255, 255);
  drawer.center(320, 320, 200, 6, 71);
  drawer.end();
  drawer.save("exhibit1.png");

  drawer.begin(TRIANGLES);
  drawer.background(0, 0, 0);
  drawer.color(0, 0, 255);
  drawer.vertex(0, 640, true);
  drawer.color(0, 255, 255);
  drawer.vertex(0, 0, true);
  drawer.vertex(640, 0, true);
  drawer.color(0, 0, 255);
  drawer.vertex(0, 640, true);
  drawer.color(0, 255, 255);
  drawer.vertex(640, 0, true);
  drawer.vertex(640, 640, true);
  drawer.end();
  drawer.begin(MAURERS);
  drawer.color(0, 255, 255);
  drawer.center(160, 480, 100, 30, 59);
  drawer.color(0, 0, 255);
  drawer.center(480, 160, 100, 30, 59);
  drawer.color(0, 128, 255);
  drawer.center(160, 160, 100, 50, 19);
  drawer.color(0, 128, 255);
  drawer.center(480, 480, 100, 50, 19);
  drawer.color(255, 255, 255);
  drawer.center(320, 320, 200, 50, 59);
  drawer.end();
  drawer.save("exhibit2.png");
  
  drawer.begin(LINES);
  drawer.background(0, 51, 102);
  for (int i = 40; i <= 640; i += 40) {
    drawer.color(0, 51, 102);
    drawer.vertex(0, i);
    drawer.color(102, 102, 0);
    drawer.vertex(i, 0);
    drawer.color(102, 102, 0);
    drawer.vertex(0, i);
    drawer.color(0, 51, 102);
    drawer.vertex(i, 640);
  }
  for (int i = 40; i < 640; i += 40) {
    drawer.color(0, 51, 102);
    drawer.vertex(i, 640);
    drawer.color(102, 102, 0);
    drawer.vertex(640, i);
    drawer.color(0, 51, 102);
    drawer.vertex(i, 640);
    drawer.color(102, 102, 0);
    drawer.vertex(0, i);
  }
  drawer.end();
  drawer.begin(CIRCLES);
  drawer.color(255, 255, 0);
  drawer.center(640, 0, 50);
  for (int i = 400; i >= 100; i -= 50) {
    drawer.color(200 - ((i / 50) * 12), 200 - ((i / 50) * 12), 0);
    drawer.center(640, 0, i, 0, 0, true);
  }
  drawer.end();
  drawer.save("exhibit3.png");
}
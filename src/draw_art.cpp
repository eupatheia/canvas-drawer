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
   drawer.color(255, 255, 255);
   for (int i = 50; i <= 400; i += 50) {
      drawer.center(320, 320, i);
   }
   drawer.end();
   drawer.save("test_circle.png");

   // drawer.begin(ROSES);
   // drawer.background(0, 0, 0);
   // drawer.color(255, 255, 255);
   // drawer.center(320, 320, 300, 5, 4);
   // drawer.end();
   // drawer.save("test_rose.png");

   // drawer.begin(MAURERS);
   // drawer.background(0, 0, 0);
   // drawer.color(255, 255, 255);
   // drawer.center(320, 320, 300, 2, 29);
   // drawer.end();
   // drawer.save("test_maurer.png");

   // drawer.begin(ROSES);
   // drawer.background(0, 0, 0);
   // drawer.color(255, 0, 0);
   // drawer.center(320, 320, 200, 6, 1);
   // drawer.end();
   // drawer.begin(MAURERS);
   // drawer.color(255, 255, 255);
   // drawer.center(320, 320, 200, 6, 71);
   // drawer.end();
   // drawer.save("exhibit1.png");
}
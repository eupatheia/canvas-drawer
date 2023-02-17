// Base code Copyright 2021, Aline Normoyle, alinen

/* image.cpp
 * Implementation of an Image class that uses the STB library to load an image
 * into a pixel representation and provides functions to transform the image
 * (e.g. color changes, resizing, cropping, blending, etc.); also allows
 * user to save/write the result to a .png file
 * @author JL
 * @version February 2, 2023
 *
 * STB documentation can be found at
 * https://github.com/nothings/stb/
 */

#include "image.h"

#include <cassert>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

namespace agl {

// helper function to free pixels
void Image::resetPixels() {
  if (_pixels != NULL) {
    _width = 0;
    _height = 0;
    _components = 0;
    if (_use_stbi_free) {
      // allocated with stbi_load, so must free with stbi_image_free
      stbi_image_free(_pixels);
      _use_stbi_free = false;
    } else {
      // regular array deallocation
      delete[] _pixels;
    }
    _pixels = NULL;
  }
}

// if no width/height provided, no need set instance variables
Image::Image() {  }

Image::Image(int width, int height): _width(width), _height(height) {
  resetPixels();
  _pixels = new struct Pixel[width * height];
}

Image::Image(const Image& orig) {
  resetPixels();
  _width = orig._width;
  _height = orig._height;
  _components = orig._components;
  _pixels = new struct Pixel[_width * _height];
  memcpy(_pixels, orig._pixels, sizeof(struct Pixel) * _width * _height);
}

Image& Image::operator=(const Image& orig) {
  if (&orig == this) {
    return *this;
  }
  resetPixels();
  _width = orig._width;
  _height = orig._height;
  _components = orig._components;
  _pixels = new struct Pixel[_width * _height];
  memcpy(_pixels, orig._pixels, sizeof(struct Pixel) * _width * _height);
  return *this;
}

Image::~Image() {
  // must free pixel memory
  resetPixels();
}

int Image::width() const {
  return _width;
}

int Image::height() const {
  return _height;
}

char* Image::data() const {
  return (char *) _pixels;
}

void Image::set(int width, int height, unsigned char* data) {
  resetPixels();
  _width = width;
  _height = height;
  _pixels = new struct Pixel[_width * _height];
  memcpy(_pixels, data, sizeof(struct Pixel) * _width * _height);
}

bool Image::load(const std::string& filename, bool flip) {
  // if flip = true, will set stbi's flip variable to true also
  // auto conversion to bool (false = 0, true = 1)
  stbi_set_flip_vertically_on_load(flip);
  // free pixel memory first
  resetPixels();
  // stbi_load returns unsigned char *, so must cast to struct Pixel *
  // also must convert filename from string to char *
  // requesting only 3 channels (RGB)
  _pixels = (struct Pixel *) stbi_load(filename.c_str(), &_width, &_height,
      &_components, 3);
  _use_stbi_free = true;
  if (_pixels == NULL) {
    // allocation failure
    return false;
  } else {
    // successful load
    return true;
  }
}

bool Image::save(const std::string& filename, bool flip) const {
  // if flip = true, will set stbi's flip variable to true also
  stbi_flip_vertically_on_write(flip);
  int result = stbi_write_png(filename.c_str(), _width, _height, 3,
      _pixels, sizeof(struct Pixel) * _width);
  if (result == 0) {
    // failed to write file
    return false;
  } else {
    // successful write
    return true;
  }
}

Pixel Image::get(int row, int col) const {
  return _pixels[row * _width + col];
}

void Image::set(int row, int col, const Pixel& color) {
  _pixels[row * _width + col] = color;
}

Pixel Image::get(int i) const {
  return _pixels[i];
}

void Image::set(int i, const Pixel& c) {
  _pixels[i] = c;
}

Image Image::resize(int w, int h) const {
  Image result(w, h);
  for (int i = 0; i < h; i++) {
    float row_ratio = (float) i / (h - 1);
    int orig_row = floor(row_ratio * (_height - 1));
    for (int j = 0; j < w; j++) {
      float col_ratio = (float) j / (w - 1);
      int orig_col = floor(col_ratio * (_width - 1));
      // set new pixel to old pixel at the same relative position
      //   in the old image, proportionally
      result.set(i, j, get(orig_row, orig_col));
    }
  }
  return result;
}

Image Image::flipHorizontal() const {
  Image result(_width, _height);
  for (int i = 0; i < _height; i++) {
    for (int j = 0; j < _width; j++) {
      // set new pixel to old pixel in mirrored half, folded horizontally
      result.set(i, j, get(_height - 1 - i, j));
    }
  }
  return result;
}

Image Image::flipVertical() const {
  Image result(_width, _height);
  for (int i = 0; i < _height; i++) {
    for (int j = 0; j < _width; j++) {
      // set new pixel to old pixel in mirrored half, folded vertically
      result.set(i, j, get(i, _width - 1 - j));
    }
  }
  return result;
}

Image Image::subimage(int startx, int starty, int w, int h) const {
  Image sub(w, h);
  for (int i = 0; i < h; i++) {
    for (int j = 0; j < w; j++) {
      sub.set(i, j, get(starty + i, startx + j));
    }
  }
  return sub;
}

void Image::replace(const Image& image, int startx, int starty) {
  // only replace as many pixels as will fit onto original image
  int rows = std::min(_height - starty, image.height()); // num rows to replace
  int cols = std::min(_width - startx, image.width());  // num cols to replace
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
      set(starty + i, startx + j, image.get(i, j));
    }
  }
}

Image Image::gammaCorrect(float gamma) const {
  Image result(_width, _height);
  for (int i = 0; i < _height; i++) {
    for (int j = 0; j < _width; j++) {
      struct Pixel pixel = get(i, j);  // copy original pixel
      // RGB values must be converted to float in range [0, 1.0] first
      pixel.r = round(pow((pixel.r / 255.0f), 1.0f / gamma) * 255.0f);
      pixel.g = round(pow((pixel.g / 255.0f), 1.0f / gamma) * 255.0f);
      pixel.b = round(pow((pixel.b / 255.0f), 1.0f / gamma) * 255.0f);
      result.set(i, j, pixel);
    }
  }
  return result;
}

Pixel Image::alphaBlendPixel(const struct Pixel& orig,
    const struct Pixel& other, float alpha) const {
  struct Pixel corrected;
  corrected.r = round((other.r * alpha) + (orig.r * (1 - alpha)));
  corrected.g = round((other.g * alpha) + (orig.g * (1 - alpha)));
  corrected.b = round((other.b * alpha) + (orig.b * (1 - alpha)));
  return corrected;
}

Image Image::alphaBlend(const Image& other, float alpha) const {
  Image result(_width, _height);
  for (int i = 0; i < _height; i++) {
    for (int j = 0; j < _width; j++) {
      struct Pixel blend = alphaBlendPixel(get(i, j), other.get(i, j), alpha);
      result.set(i, j, blend);
    }
  }
  return result;
}

Image Image::grayscale() const {
  Image result(_width, _height);
  for (int i = 0; i < _height; i++) {
    for (int j = 0; j < _width; j++) {
      struct Pixel orig = get(i,j);
      unsigned char intensity = round((orig.r * 0.3) + (orig.g * 0.59) +
          (orig.b * 0.11));  // weighted average
      struct Pixel corrected = {intensity, intensity, intensity};
      result.set(i, j, corrected);
    }
  }
  return result;
}

Image Image::rotate90() const {
  Image result(_height, _width);
  // width and height are switched (b/c image is transposed)
  for (int i = 0; i < _width; i++) {
    for (int j = 0; j < _height; j++) {
      result.set(i, j, get(j, _width - 1 - i));
    }
  }
  return result;
}

Image Image::add(const Image& other) const {
  Image result(_width, _height);
  for (int i = 0; i < _height; i++) {
    for (int j = 0; j < _width; j++) {
      struct Pixel p1 = get(i,j);
      struct Pixel p2 = other.get(i,j);
      // add colors component-wise, clamp at 255
      struct Pixel p3 = {(unsigned char) std::min(p1.r + p2.r, 255),
          (unsigned char) std::min(p1.g + p2.g, 255),
          (unsigned char) std::min(p1.b + p2.b, 255)};
      result.set(i, j, p3);
    }
  }
  return result;
}

Image Image::subtract(const Image& other) const {
  Image result(_width, _height);
  for (int i = 0; i < _height; i++) {
    for (int j = 0; j < _width; j++) {
      struct Pixel p1 = get(i,j);
      struct Pixel p2 = other.get(i,j);
      // subtract colors component-wise, clamp at 0
      struct Pixel p3 = {(unsigned char) std::max(p1.r - p2.r, 0),
          (unsigned char) std::max(p1.g - p2.g, 0),
          (unsigned char) std::max(p1.b - p2.b, 0)};
      result.set(i, j, p3);
    }
  }
  return result;
}

Image Image::multiply(const Image& other) const {
  Image result(_width, _height);
  for (int i = 0; i < _height; i++) {
    for (int j = 0; j < _width; j++) {
      struct Pixel p1 = get(i,j);
      struct Pixel p2 = other.get(i,j);
      // multiply colors component-wise, clamp at 255
      struct Pixel p3 = {(unsigned char) std::min(p1.r * p2.r, 255),
          (unsigned char) std::min(p1.g * p2.g, 255),
          (unsigned char) std::min(p1.b * p2.b, 255)};
      result.set(i, j, p3);
    }
  }
  return result;
}

Image Image::difference(const Image& other) const {
  Image result(_width, _height);
  for (int i = 0; i < _height; i++) {
    for (int j = 0; j < _width; j++) {
      struct Pixel p1 = get(i,j);
      struct Pixel p2 = other.get(i,j);
      // subtract colors component-wise, use absolute value
      struct Pixel p3 = {(unsigned char) std::abs(p1.r - p2.r),
          (unsigned char) std::abs(p1.g - p2.g),
          (unsigned char) std::abs(p1.b - p2.b)};
      result.set(i, j, p3);
    }
  }
  return result;
}

Image Image::swirl() const {
  Image result(_width, _height);
  for (int i = 0; i < _height; i++) {
    for (int j = 0; j < _width; j++) {
      struct Pixel orig = get(i,j);
      // rotate channels
      struct Pixel corrected = {orig.g, orig.b, orig.r};
      result.set(i, j, corrected);
    }
  }
  return result;
}

Image Image::lightest(const Image& other) const {
  Image result(_width, _height);
  for (int i = 0; i < _height; i++) {
    for (int j = 0; j < _width; j++) {
      struct Pixel p1 = get(i,j);
      struct Pixel p2 = other.get(i,j);
      // get lightest color
      struct Pixel p3 = {(unsigned char) std::max(p1.r, p2.r),
          (unsigned char) std::max(p1.g, p2.g),
          (unsigned char) std::max(p1.b, p2.b)};
      result.set(i, j, p3);
    }
  }
  return result;
}

Image Image::darkest(const Image& other) const {
  Image result(_width, _height);
  for (int i = 0; i < _height; i++) {
    for (int j = 0; j < _width; j++) {
      struct Pixel p1 = get(i,j);
      struct Pixel p2 = other.get(i,j);
      // get darkest color
      struct Pixel p3 = {(unsigned char) std::min(p1.r, p2.r),
          (unsigned char) std::min(p1.g, p2.g),
          (unsigned char) std::min(p1.b, p2.b)};
      result.set(i, j, p3);
    }
  }
  return result;
}

Image Image::invert() const {
  Image result(_width, _height);
  for (int i = 0; i < _height; i++) {
    for (int j = 0; j < _width; j++) {
      struct Pixel p = get(i,j);
      // subtract colors from 255
      p.r = 255 - p.r;
      p.g = 255 - p.g;
      p.b = 255 - p.b;
      result.set(i, j, p);
    }
  }
  return result;
}

Image Image::extractChannel(int channel) const {
  Image result(_width, _height);
  for (int i = 0; i < _height; i++) {
    for (int j = 0; j < _width; j++) {
      struct Pixel p = get(i,j);
      // only keep the specified channel, others set to zero
      if (channel == 1) {
        p.g = 0;
        p.b = 0;
      } else if (channel == 2) {
        p.r = 0;
        p.b = 0;
      } else if (channel == 3) {
        p.r = 0;
        p.g = 0;
      } else {
        // no change if invalid channel
        std::cout << "Invalid channel: " << channel << std::endl;
      }
      result.set(i, j, p);
    }
  }
  return result;
}

int* Image::convolve(const int * matrix, int * result, int i, int j,
    Position position) const {
  int startRow, endRow, startCol, endCol;
  if (position == MIDDLE) {
    // a middle pixel, has 8 neighbors + itself
    startRow = -1;
    endRow = 1;
    startCol = -1;
    endCol = 1;
  } else if (position == CORNER) {
    if (i == 0 && j == 0) {
      // top left pixel, has 3 neighbors + itself
      startRow = 0;
      endRow = 1;
      startCol = 0;
      endCol = 1;
    } else if (i == 0 && j == _width - 1) {
      // top right pixel, has 3 neighbors + itself
      startRow = 0;
      endRow = 1;
      startCol = -1;
      endCol = 0;
    } else if (i == _height - 1 && j == 0) {
      // bottom left pixel, has 3 neighbors + itself
      startRow = -1;
      endRow = 0;
      startCol = 0;
      endCol = 1;
    } else {  // i == _height - 1 && j == _width - 1
      // bottom right pixel, has 3 neighbors + itself
      startRow = -1;
      endRow = 0;
      startCol = -1;
      endCol = 0;
    }
  } else { //some type of edge pixel
    if (i == 0) {
      // top edge pixel, has 5 neighbors + itself
      startRow = 0;
      endRow = 1;
      startCol = -1;
      endCol = 1;
    } else if (i == _height - 1) {
      // bottom edge pixel, has 5 neighbors + itself
      startRow = -1;
      endRow = 0;
      startCol = -1;
      endCol = 1;
    } else if (j == 0) {
      // left edge pixel, has 5 neighbors + itself
      startRow = -1;
      endRow = 1;
      startCol = 0;
      endCol = 1;
    } else { // j == _width - 1
      // right edge pixel, has 5 neighbors + itself
      startRow = -1;
      endRow = 1;
      startCol = -1;
      endCol = 0;
    }
  }
  for (int m = startRow; m <= endRow; m++) {
    for (int n = startCol; n <= endCol; n++) {
      struct Pixel p = get(i + m, j + n);
      result[0] += p.r * matrix[((m + 1) * 3) + (n + 1)];
      result[1] += p.g * matrix[((m + 1) * 3) + (n + 1)];
      result[2] += p.b * matrix[((m + 1) * 3) + (n + 1)];
    }
  }
  return result;
}

Image Image::blur() const {
  Image result(_width, _height);
  int matrix[9] = {1, 1, 1, 1, 1, 1, 1, 1, 1};
  for (int i = 0; i < _height; i++) {
    for (int j = 0; j < _width; j++) {
      int convolved[3] = {0, 0, 0};  // sum of convolved area, component-wise
      int denom;  // denominator for averaging neighborhood
      struct Pixel p = get(i,j);
      if ((i > 0 && i < _height - 1) && (j > 0 && j < _width - 1)) {
        convolve(matrix, convolved, i, j, MIDDLE);
        denom = 9;  // middle pixels have 8 neighbors + itself
      } else if ((i == 0 && j == 0) || (i == 0 && j == _width - 1) ||
          (i == _height - 1 && j == 0) ||
          (i == _height - 1 && j == _width - 1)) {
        convolve(matrix, convolved, i, j, CORNER);
        denom = 4;  // corner pixels have 3 neighbors + itself
      } else {
        convolve(matrix, convolved, i, j, EDGE);
        denom = 6;  // edge pixels have 5 neighbors + itself
      }
      p.r = round((float) convolved[0] / denom);
      p.g = round((float) convolved[1] / denom);
      p.b = round((float) convolved[2] / denom);
      result.set(i, j, p);
    }
  }
  return result;
}

Image Image::extractWhite(int threshold) const {
  Image result(_width, _height);
  for (int i = 0; i < _height; i++) {
    for (int j = 0; j < _width; j++) {
      struct Pixel p = get(i,j);
      if (p.r >= threshold && p.g >= threshold && p.b >= threshold) {
        // meets threshold, set to white
        p = {255, 255, 255};
      } else {
        // set to black
        p = {0, 0, 0};
      }
      result.set(i, j, p);
    }
  }
  return result;
}

Image Image::glow(int threshold) const {
  Image result(_width, _height);
  Image whitened = extractWhite(threshold);
  whitened = whitened.blur();
  for (int i = 0; i < _height; i++) {
    for (int j = 0; j < _width; j++) {
      struct Pixel whiteP = whitened.get(i, j);
      float alpha = (whiteP.r + whiteP.g + whiteP.b) / (6 * 255.0f);
      result.set(i, j, alphaBlendPixel(get(i, j), whiteP, alpha));
    }
  }
  return result;
}

Image Image::sobelEdge() const {
  Image result(_width, _height);
  int gx[9] = {1, 0, -1, 2, 0, -2, 0, 0, -1};
  int gy[9] = {1, 2, 1, 0, 0, 0, -1, -2, -1};
  for (int i = 0; i < _height; i++) {
    for (int j = 0; j < _width; j++) {
      int gxConv[3] = {0, 0, 0};  // sum of convolved area, component-wise
      int gyConv[3] = {0, 0, 0};  // sum of convolved area, component-wise
      struct Pixel p = get(i, j);
      Position position;
      if ((i > 0 && i < _height - 1) && (j > 0 && j < _width - 1)) {
        position = MIDDLE;
      } else if ((i == 0 && j == 0) || (i == 0 && j == _width - 1) ||
          (i == _height - 1 && j == 0) ||
          (i == _height - 1 && j == _width - 1)) {
        position = CORNER;
      } else {
        position = EDGE;
      }
      convolve(gx, gxConv, i, j, position);
      convolve(gy, gyConv, i, j, position);
      float distanceRed = sqrt(pow(gxConv[0], 2) + pow(gyConv[0], 2));
      float distanceGreen = sqrt(pow(gxConv[1], 2) + pow(gyConv[1], 2));
      float distanceBlue = sqrt(pow(gxConv[2], 2) + pow(gyConv[2], 2));
      p.r = std::min((int) round(distanceRed), 255);
      p.g = std::min((int) round(distanceGreen), 255);
      p.b = std::min((int) round(distanceBlue), 255);
      result.set(i, j, p);
    }
  }
  return result;
}

Image Image::bitMap() const {
  Image result(_width, _height);
  int kernel[9] = {1, 1, 1, 1, 1, 1, 1, 1, 1};
  // copy over edge pixels
  for (int k = 0; k < _width; k++) {
    result.set(0, k, get(0, k));
    result.set(_height - 1, k, get(_height - 1, k));
  }
  for (int k = 0; k < _height; k++) {
    result.set(k, 0, get(k, 0));
    result.set(k, _width - 1, get(k, _width - 1));
  }
  // only convolve on middle pixels to prevent edge cases
  for (int i = 1; i < _height - 1; i += 2) {
    for (int j = 1; j < _width - 1; j += 2) {
      int conv[3] = {0, 0, 0};  // sum of convolved area, component-wise
      struct Pixel p = get(i, j);
      convolve(kernel, conv, i, j, MIDDLE);
      p.r = (int) (conv[0] / 9.0);
      p.g = (int) (conv[1] / 9.0);
      p.b = (int) (conv[2] / 9.0);
      // set 3x3 neighborhood to the avg color, like a larger "bit"
      for (int m = -1; m <= 1; m++) {
        for (int n = -1; n <= 1; n++)
        result.set(i + m, j + n, p);
      }
    }
  }
  return result;
}


}  // namespace agl

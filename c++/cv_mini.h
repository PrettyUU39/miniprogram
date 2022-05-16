#ifndef _CV_MINI_H
#define _CV_MINI_H

#include <algorithm>
#include <cstring>
#include <vector>

typedef unsigned char uchar;

#include <assert.h>

namespace cv_mini {

template <typename _Tp>
static inline _Tp* alignPtr(_Tp* ptr, int n = (int)sizeof(_Tp)) {
    assert((n & (n - 1)) == 0); // n is a power of 2
  return (_Tp*)(((size_t)ptr + n - 1) & -n);
}

struct Point2f {
  float x;
  float y;
};

struct KeyPoint {
  Point2f pt;
  float x;
  float y;
  float size;
  float angle;
  float response;
  int octave;
  int class_id;
  KeyPoint(float _x, float _y, float _size, float _angle = -1, float _response = 0, int _octave = 0,
           int _class_id = -1) {
    pt.x = _x;
    pt.y = _y;
    size = _size;
    angle = _angle;
    response = _response;
    octave = _octave;
    class_id = _class_id;
  }
};

struct Mat {
  uchar* data;
  int cols;
  int rows;
  int step;
  Mat(){};
  Mat(int _cols, int _rows, int _step) {
    cols = _cols;
    rows = _rows;
    step = _step;
    data = new uchar[_rows * _step];
  }
};

}  // namespace cv_mini

#endif  // _CV_MINI_H

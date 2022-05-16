#ifndef LIB_IMGPROC_H_
#define LIB_IMGPROC_H_

#include "cv_mini.h"

namespace WebTracking {

class ImgProc {
 public:
  ImgProc(int img_cols, int img_rows, int img_step);
  void detectFastCorner(uintptr_t ptr, int img_cols, int img_rows);
 private:
  cv_mini::Mat gray;
};

}  // namespace WebTracking

#endif

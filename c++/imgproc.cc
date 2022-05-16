#include "imgproc.h"
#include "cv_fast.h"

namespace WebTracking {

ImgProc::ImgProc(int img_cols, int img_rows, int img_step) {
  gray = cv_mini::Mat(img_cols, img_rows, img_step);
}

void ImgProc::detectFastCorner(uintptr_t ptr, int img_cols, int img_rows) {
  unsigned char* data = reinterpret_cast<unsigned char*>(ptr);

  // to gray
  for (int i = 0, iend = img_cols * img_rows; i < iend; ++i) {
    int r = data[4*i];
    int g = data[4*i + 1];
    int b = data[4*i + 2];
    int brightness = (r*.21+g*.72+b*.07);

    data[4*i] = brightness;
    data[4*i + 1] = brightness;
    data[4*i + 2] = brightness;
    gray.data[i] = brightness;
  }

  std::vector<cv_mini::KeyPoint> keypoints;
  cv_fast_FAST_9_16_reference(gray, keypoints, 40, 1);
  for (size_t i=0,iend=keypoints.size(); i < iend; ++i) {
    int x = (int)keypoints[i].pt.x;
    int y = (int)keypoints[i].pt.y;
    if (x > 10 && x < img_cols-10 && y > 10 && y < img_rows-10) {
      for (int r=-1;r<2;++r) {
        for (int c=-1;c<2;++c) {
          int pixel_loc = 4*((y-1+r)*img_cols+(x+c));
          data[pixel_loc] = 0;
          data[pixel_loc + 1] = 0;
          data[pixel_loc + 2] = 255;
        }
      }
    }
  }
}

}  // namespace WebTracking

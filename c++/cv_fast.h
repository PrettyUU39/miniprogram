#ifndef _CV_FAST_H
#define _CV_FAST_H

#include "cv_mini.h"

void cv_fast_FAST_9_16_reference(const uchar* img_data, const int img_cols, const int img_rows,
                                 const int img_step, std::vector<cv_mini::KeyPoint>& keypoints,
                                 int threshold, bool nonmax_suppression);
void cv_fast_FAST_9_16_reference(const cv_mini::Mat& img, std::vector<cv_mini::KeyPoint>& keypoints,
                                 int threshold, bool nonmax_suppression);

#endif  // _CV_FAST_H

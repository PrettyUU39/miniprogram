#include "cv_fast.h"

static void cv_fast_makeOffsets_16_reference(int pixel[25], int rowStride, int patternSize_OLD) {
  const int patternSize = 16;
  static const int offsets16[][2] = {{0, 3},  {1, 3},  {2, 2},  {3, 1},   {3, 0},   {3, -1},
                                     {2, -2}, {1, -3}, {0, -3}, {-1, -3}, {-2, -2}, {-3, -1},
                                     {-3, 0}, {-3, 1}, {-2, 2}, {-1, 3}};

  static const int offsets12[][2] = {{0, 2},  {1, 2},   {2, 1},   {2, 0},  {2, -1}, {1, -2},
                                     {0, -2}, {-1, -2}, {-2, -1}, {-2, 0}, {-2, 1}, {-1, 2}};

  static const int offsets8[][2] = {{0, 1},  {1, 1},   {1, 0},  {1, -1},
                                    {0, -1}, {-1, -1}, {-1, 0}, {-1, 1}};

  const int(*offsets)[2] = patternSize == 16
                               ? offsets16
                               : patternSize == 12 ? offsets12 : patternSize == 8 ? offsets8 : 0;

  int k = 0;
  for (; k < patternSize; k++) pixel[k] = offsets[k][0] + offsets[k][1] * rowStride;
  for (; k < 25; k++) pixel[k] = pixel[k - patternSize];
}

static int cv_fast_cornerScore_16_reference(const uchar* ptr, const int pixel[], int threshold) {
#define VERIFY_CORNERS 0

  const int K = 8, N = K * 3 + 1;
  int k, v = ptr[0];
  short d[N];
  for (k = 0; k < N; k++) d[k] = (short)(v - ptr[pixel[k]]);

  {
    int a0 = threshold;
    for (k = 0; k < 16; k += 2) {
      int a = std::min((int)d[k + 1], (int)d[k + 2]);
      a = std::min(a, (int)d[k + 3]);
      if (a <= a0) continue;
      a = std::min(a, (int)d[k + 4]);
      a = std::min(a, (int)d[k + 5]);
      a = std::min(a, (int)d[k + 6]);
      a = std::min(a, (int)d[k + 7]);
      a = std::min(a, (int)d[k + 8]);
      a0 = std::max(a0, std::min(a, (int)d[k]));
      a0 = std::max(a0, std::min(a, (int)d[k + 9]));
    }

    int b0 = -a0;
    for (k = 0; k < 16; k += 2) {
      int b = std::max((int)d[k + 1], (int)d[k + 2]);
      b = std::max(b, (int)d[k + 3]);
      b = std::max(b, (int)d[k + 4]);
      b = std::max(b, (int)d[k + 5]);
      if (b >= b0) continue;
      b = std::max(b, (int)d[k + 6]);
      b = std::max(b, (int)d[k + 7]);
      b = std::max(b, (int)d[k + 8]);

      b0 = std::min(b0, std::max(b, (int)d[k]));
      b0 = std::min(b0, std::max(b, (int)d[k + 9]));
    }

    threshold = -b0 - 1;
  }

  return threshold;
}

void cv_fast_FAST_9_16_reference(const uchar* img_data, const int img_cols, const int img_rows,
                                 const int img_step, std::vector<cv_mini::KeyPoint>& keypoints,
                                 int threshold, bool nonmax_suppression) {
  const int patternSize = 16;

  const int K = patternSize / 2, N = patternSize + K + 1;
  int i, j, k, pixel[25];
  cv_fast_makeOffsets_16_reference(pixel, (int)img_step, patternSize);

  keypoints.clear();

  threshold = std::min(std::max(threshold, 0), 255);

  uchar threshold_tab[512];
  for (i = -255; i <= 255; i++)
    threshold_tab[i + 255] = (uchar)(i < -threshold ? 1 : i > threshold ? 2 : 0);

  uchar _buf[((img_cols + 16) * 3 * (sizeof(int) + sizeof(uchar)) + 128)];
  uchar* buf[3];
  buf[0] = _buf;
  buf[1] = buf[0] + img_cols;
  buf[2] = buf[1] + img_cols;
  int* cpbuf[3];
  cpbuf[0] = (int*)cv_mini::alignPtr(buf[2] + img_cols, sizeof(int)) + 1;
  cpbuf[1] = cpbuf[0] + img_cols + 1;
  cpbuf[2] = cpbuf[1] + img_cols + 1;
  memset(buf[0], 0, img_cols * 3);

  for (i = 3; i < img_rows - 2; i++) {
    const uchar* ptr = img_data + img_step * i + 3;
    uchar* curr = buf[(i - 3) % 3];
    int* cornerpos = cpbuf[(i - 3) % 3];
    memset(curr, 0, img_cols);
    int ncorners = 0;

    if (i < img_rows - 3) {
      j = 3;

      for (; j < img_cols - 3; j++, ptr++) {
        int v = ptr[0];
        const uchar* tab = &threshold_tab[0] - v + 255;
        int d = tab[ptr[pixel[0]]] | tab[ptr[pixel[8]]];

        if (d == 0) continue;

        d &= tab[ptr[pixel[2]]] | tab[ptr[pixel[10]]];
        d &= tab[ptr[pixel[4]]] | tab[ptr[pixel[12]]];
        d &= tab[ptr[pixel[6]]] | tab[ptr[pixel[14]]];

        if (d == 0) continue;

        d &= tab[ptr[pixel[1]]] | tab[ptr[pixel[9]]];
        d &= tab[ptr[pixel[3]]] | tab[ptr[pixel[11]]];
        d &= tab[ptr[pixel[5]]] | tab[ptr[pixel[13]]];
        d &= tab[ptr[pixel[7]]] | tab[ptr[pixel[15]]];

        if (d & 1) {
          int vt = v - threshold, count = 0;

          for (k = 0; k < N; k++) {
            int x = ptr[pixel[k]];
            if (x < vt) {
              if (++count > K) {
                cornerpos[ncorners++] = j;
                if (nonmax_suppression)
                  curr[j] = (uchar)cv_fast_cornerScore_16_reference(ptr, pixel, threshold);
                break;
              }
            } else
              count = 0;
          }
        }

        if (d & 2) {
          int vt = v + threshold, count = 0;

          for (k = 0; k < N; k++) {
            int x = ptr[pixel[k]];
            if (x > vt) {
              if (++count > K) {
                cornerpos[ncorners++] = j;
                if (nonmax_suppression)
                  curr[j] = (uchar)cv_fast_cornerScore_16_reference(ptr, pixel, threshold);
                break;
              }
            } else
              count = 0;
          }
        }
      }
    }

    cornerpos[-1] = ncorners;

    if (i == 3) continue;

    const uchar* prev = buf[(i - 4 + 3) % 3];
    const uchar* pprev = buf[(i - 5 + 3) % 3];
    cornerpos = cpbuf[(i - 4 + 3) % 3];
    ncorners = cornerpos[-1];

    for (k = 0; k < ncorners; k++) {
      j = cornerpos[k];
      int score = prev[j];
      if (!nonmax_suppression ||
          (score > prev[j + 1] && score > prev[j - 1] && score > pprev[j - 1] && score > pprev[j] &&
           score > pprev[j + 1] && score > curr[j - 1] && score > curr[j] && score > curr[j + 1])) {
        keypoints.push_back(cv_mini::KeyPoint((float)j, (float)(i - 1), 7.f, -1, (float)score));
      }
    }
  }
}

void cv_fast_FAST_9_16_reference(const cv_mini::Mat& img, std::vector<cv_mini::KeyPoint>& keypoints,
                                 int threshold, bool nonmax_suppression) {
  cv_fast_FAST_9_16_reference(img.data, img.cols, img.rows, img.step, keypoints, threshold,
                              nonmax_suppression);
}

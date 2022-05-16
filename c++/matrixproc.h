#ifndef LIB_MATRIXPROC_H_
#define LIB_MATRIXPROC_H_

#include <Eigen/Dense>
#include <vector>

namespace WebTracking {

class MatrixProc {
 public:
  static std::vector<double> AngleVectorFromRotation(const std::vector<double>& quaternion);
  static double Chi2(const int& size, const std::vector<double>& res_data, const std::vector<double>& S_data);
};

}  // namespace WebTracking

#endif

#include "matrixproc.h"

namespace WebTracking {

std::vector<double> MatrixProc::AngleVectorFromRotation(const std::vector<double>& quaternion) {
  Eigen::AngleAxisd angleaxisd(
      Eigen::Quaterniond(quaternion[0], quaternion[1], quaternion[2], quaternion[3]));
  Eigen::Vector3d anglevector = angleaxisd.axis() * angleaxisd.angle();
  return std::vector<double>({anglevector[0], anglevector[1], anglevector[2]});
}

double MatrixProc::Chi2(const int& size, const std::vector<double>& res_data,
                        const std::vector<double>& S_data) {
  Eigen::VectorXd res(size);
  Eigen::MatrixXd S(size, size);
  for (int i = 0, iend = res_data.size(); i < iend; ++i) {
    res(i) = res_data[i];
  }
  for (int i = 0, iend = S_data.size(); i < iend; ++i) {
    S(i / size, i % size) = S_data[i];
  }
  return res.dot(S.llt().solve(res));
}
}
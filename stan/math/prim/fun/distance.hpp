#ifndef STAN_MATH_PRIM_FUN_DISTANCE_HPP
#define STAN_MATH_PRIM_FUN_DISTANCE_HPP

#include <stan/math/prim/err/check_finite.hpp>
#include <stan/math/prim/fun/abs.hpp>
#include <stan/math/prim/meta/return_type.hpp>
#include <stan/math/prim/fun/Eigen.hpp>
#include <stan/math/prim/fun/squared_distance.hpp>
#include <stan/math/prim/err/check_vector.hpp>
#include <stan/math/prim/err/check_matching_sizes.hpp>
#include <boost/math/tools/promotion.hpp>







namespace stan {
namespace math {

/**
 * Returns the distance between two scalars.
 *
 * @param x1 First scalar.
 * @param x2 Second scalar.
 * @return Distance between two scalars
 * @throw std::domain_error If the arguments are not finite.
 */
template <typename T1, typename T2>
inline typename return_type<T1, T2>::type distance(const T1& x1, const T2& x2) {
  check_finite("distance", "x1", x1);
  check_finite("distance", "x2", x2);
  return abs(x1 - x2);
}
}  // namespace math
}  // namespace stan










namespace stan {
namespace math {

/**
 * Returns the distance between the specified vectors.
 *
 * @param v1 First vector.
 * @param v2 Second vector.
 * @return Dot product of the vectors.
 * @throw std::domain_error If the vectors are not the same
 * size or if they are both not vector dimensioned.
 */
template <typename T1, int R1, int C1, typename T2, int R2, int C2>
inline typename boost::math::tools::promote_args<T1, T2>::type distance(
    const Eigen::Matrix<T1, R1, C1>& v1, const Eigen::Matrix<T2, R2, C2>& v2) {
  using std::sqrt;
  check_vector("distance", "v1", v1);
  check_vector("distance", "v2", v2);
  check_matching_sizes("distance", "v1", v1, "v2", v2);
  return sqrt(squared_distance(v1, v2));
}

}  // namespace math
}  // namespace stan
#endif
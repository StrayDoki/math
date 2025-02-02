#ifndef STAN_MATH_PRIM_SCAL_PROB_FRECHET_CDF_HPP
#define STAN_MATH_PRIM_SCAL_PROB_FRECHET_CDF_HPP

#include <stan/math/prim/meta.hpp>
#include <boost/random/weibull_distribution.hpp>
#include <boost/random/variate_generator.hpp>
#include <stan/math/prim/scal/err/check_consistent_sizes.hpp>
#include <stan/math/prim/scal/err/check_nonnegative.hpp>
#include <stan/math/prim/scal/err/check_not_nan.hpp>
#include <stan/math/prim/scal/err/check_positive.hpp>
#include <stan/math/prim/scal/err/check_positive_finite.hpp>
#include <stan/math/prim/scal/fun/size_zero.hpp>
#include <stan/math/prim/scal/fun/log1m.hpp>
#include <stan/math/prim/scal/fun/multiply_log.hpp>
#include <stan/math/prim/scal/fun/value_of.hpp>
#include <stan/math/prim/scal/fun/constants.hpp>
#include <cmath>

namespace stan {
namespace math {

template <typename T_y, typename T_shape, typename T_scale>
return_type_t<T_y, T_shape, T_scale> frechet_cdf(const T_y& y,
                                                 const T_shape& alpha,
                                                 const T_scale& sigma) {
  using T_partials_return = partials_return_t<T_y, T_shape, T_scale>;

  static const char* function = "frechet_cdf";

  using std::exp;
  using std::log;

  if (size_zero(y, alpha, sigma)) {
    return 1.0;
  }

  T_partials_return cdf(1.0);
  check_positive(function, "Random variable", y);
  check_positive_finite(function, "Shape parameter", alpha);
  check_positive_finite(function, "Scale parameter", sigma);

  operands_and_partials<T_y, T_shape, T_scale> ops_partials(y, alpha, sigma);

  scalar_seq_view<T_y> y_vec(y);
  scalar_seq_view<T_scale> sigma_vec(sigma);
  scalar_seq_view<T_shape> alpha_vec(alpha);
  size_t N = max_size(y, sigma, alpha);
  for (size_t n = 0; n < N; n++) {
    const T_partials_return y_dbl = value_of(y_vec[n]);
    const T_partials_return sigma_dbl = value_of(sigma_vec[n]);
    const T_partials_return alpha_dbl = value_of(alpha_vec[n]);
    const T_partials_return pow_ = pow(sigma_dbl / y_dbl, alpha_dbl);
    const T_partials_return cdf_ = exp(-pow_);

    cdf *= cdf_;

    if (!is_constant_all<T_y>::value) {
      ops_partials.edge1_.partials_[n] += pow_ * alpha_dbl / y_dbl;
    }
    if (!is_constant_all<T_shape>::value) {
      ops_partials.edge2_.partials_[n] += pow_ * log(y_dbl / sigma_dbl);
    }
    if (!is_constant_all<T_scale>::value) {
      ops_partials.edge3_.partials_[n] -= pow_ * alpha_dbl / sigma_dbl;
    }
  }

  if (!is_constant_all<T_y>::value) {
    for (size_t n = 0; n < stan::length(y); ++n) {
      ops_partials.edge1_.partials_[n] *= cdf;
    }
  }
  if (!is_constant_all<T_shape>::value) {
    for (size_t n = 0; n < stan::length(alpha); ++n) {
      ops_partials.edge2_.partials_[n] *= cdf;
    }
  }
  if (!is_constant_all<T_scale>::value) {
    for (size_t n = 0; n < stan::length(sigma); ++n) {
      ops_partials.edge3_.partials_[n] *= cdf;
    }
  }
  return ops_partials.build(cdf);
}

}  // namespace math
}  // namespace stan
#endif

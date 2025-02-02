#ifndef STAN_MATH_PRIM_MAT_FUN_MDIVIDE_RIGHT_TRI_HPP
#define STAN_MATH_PRIM_MAT_FUN_MDIVIDE_RIGHT_TRI_HPP

#include <stan/math/prim/mat/fun/Eigen.hpp>
#include <stan/math/prim/mat/fun/mdivide_left_tri.hpp>
#include <stan/math/prim/mat/fun/promote_common.hpp>
#include <stan/math/prim/mat/fun/transpose.hpp>
#include <stan/math/prim/mat/err/check_multiplicable.hpp>
#include <stan/math/prim/mat/err/check_square.hpp>
#include <stan/math/prim/scal/err/domain_error.hpp>

namespace stan {
namespace math {

/**
 * Returns the solution of the system Ax=b when A is triangular
 * @param A Triangular matrix.  Specify upper or lower with TriView
 * being Eigen::Upper or Eigen::Lower.
 * @param b Right hand side matrix or vector.
 * @return x = b A^-1, solution of the linear system.
 * @throws std::domain_error if A is not square or the rows of b don't
 * match the size of A.
 */
template <Eigen::UpLoType TriView, typename T1, typename T2, int R1, int C1,
          int R2, int C2>
inline Eigen::Matrix<return_type_t<T1, T2>, R1, C2> mdivide_right_tri(
    const Eigen::Matrix<T1, R1, C1> &b, const Eigen::Matrix<T2, R2, C2> &A) {
  check_square("mdivide_right_tri", "A", A);
  check_multiplicable("mdivide_right_tri", "b", b, "A", A);
  if (TriView != Eigen::Lower && TriView != Eigen::Upper) {
    domain_error("mdivide_left_tri",
                 "triangular view must be Eigen::Lower or Eigen::Upper", "",
                 "");
  }
  return promote_common<Eigen::Matrix<T1, R2, C2>, Eigen::Matrix<T2, R2, C2> >(
             A)
      .template triangularView<TriView>()
      .transpose()
      .solve(
          promote_common<Eigen::Matrix<T1, R1, C1>, Eigen::Matrix<T2, R1, C1> >(
              b)
              .transpose())
      .transpose();
}

/**
 * Returns the solution of the system Ax=b when A is triangular
 * and A and b are matrices of doubles.
 * @param A Triangular matrix.  Specify upper or lower with TriView
 * being Eigen::Upper or Eigen::Lower.
 * @param b Right hand side matrix or vector.
 * @return x = b A^-1, solution of the linear system.
 * @throws std::domain_error if A is not square or the rows of b don't
 * match the size of A.
 */
template <Eigen::UpLoType TriView, int R1, int C1, int R2, int C2>
inline Eigen::Matrix<double, R1, C2> mdivide_right_tri(
    const Eigen::Matrix<double, R1, C1> &b,
    const Eigen::Matrix<double, R2, C2> &A) {
  check_square("mdivide_right_tri", "A", A);
  check_multiplicable("mdivide_right_tri", "b", b, "A", A);
#ifdef STAN_OPENCL
  if (A.rows()
      >= opencl_context.tuning_opts().tri_inverse_size_worth_transfer) {
    matrix_cl<double> A_cl(A, from_eigen_uplo_type(TriView));
    matrix_cl<double> b_cl(b);
    matrix_cl<double> A_inv_cl = tri_inverse(A_cl);
    matrix_cl<double> C_cl = b_cl * A_inv_cl;
    return from_matrix_cl(C_cl);
  } else {
#endif
    return A.template triangularView<TriView>()
        .transpose()
        .solve(b.transpose())
        .transpose();
#ifdef STAN_OPENCL
  }
#endif
}

}  // namespace math
}  // namespace stan
#endif

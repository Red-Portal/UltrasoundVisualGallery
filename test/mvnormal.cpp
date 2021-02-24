
/*
 * Copyright (C) 2021  Ray Kim
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <catch2/catch.hpp>
#define BLAZE_USE_DEBUG_MODE 1

#include "../src/misc/linearalgebra.hpp"
#include "../src/misc/mvnormal.hpp"
#include "../src/misc/prng.hpp"
#include "statistical_test.hpp"

#include <cmath>

double const catch_eps = 1e-8;

TEST_CASE("Dense covariance multivariate normal density", "[mvnormal]")
{
  auto cov = blaze::DynamicMatrix<double>(
    {{3, 1, 1},
     {1, 3, 1},
     {1, 1, 3}});
  auto mean = blaze::DynamicVector<double>(
    {-0.20617401141446381,
     0.15186815822664115,
     -0.03498553786495774});
  auto x  = blaze::DynamicVector<double>(
    {0.9040983839157295,
     -0.29874050736604413,
     -1.2570687585683156});

  auto cov_chol = usvg::Cholesky<usvg::DenseChol>();
  REQUIRE_NOTHROW( cov_chol = usvg::cholesky_nothrow(cov).value() );

  double truth_p    = 0.0069349873998044214;
  REQUIRE( usvg::dmvnormal(x, mean, cov_chol) == Approx(truth_p) );

  double truth_logp = -4.971176042116139;
  REQUIRE( usvg::dmvnormal(x, mean, cov_chol, true) == Approx(truth_logp) );
}

TEST_CASE("Diagonal covariance multivariate normal density", "[mvnormal]")
{
  auto cov = blaze::DynamicVector<double>({1.0, 2.0, 3.0});
  auto mean  = blaze::DynamicVector<double>(
    {-0.20617401141446381,
     0.15186815822664115,
     -0.03498553786495774});
  auto x  = blaze::DynamicVector<double>(
    {0.9040983839157295,
     -0.29874050736604413,
     -1.2570687585683156});

  auto cov_chol = usvg::Cholesky<usvg::DiagonalChol>();
  REQUIRE_NOTHROW( cov_chol = usvg::cholesky_nothrow(cov).value() );

  double truth_p    = 0.010371181395210441;
  REQUIRE( usvg::dmvnormal(x, mean, cov_chol) == Approx(truth_p) );

  double truth_logp = -4.568724338908423;
  REQUIRE( usvg::dmvnormal(x, mean, cov_chol, true) == Approx(truth_logp) );
}

TEST_CASE("Unift multivariate normal density", "[mvnormal]")
{
  auto x  = blaze::DynamicVector<double>(
    {0.9040983839157295,
     -0.29874050736604413,
     -1.2570687585683156});

  double truth_p    = 0.01831112609097114;
  REQUIRE( usvg::dmvnormal(x) == Approx(truth_p) );

  double truth_logp = -4.000246420768439;
  REQUIRE( usvg::dmvnormal(x, true) == Approx(truth_logp) );
}

TEST_CASE("Multivariate unit normal sampling", "[mvnormal]")
{
  auto key         = GENERATE(range(0u, 8u));
  auto prng        = usvg::Random123(key);
  size_t n_samples = 512;
  size_t n_dims    = 16;
  auto samples     = blaze::DynamicMatrix<double>(n_dims, n_samples);

  for (size_t i = 0; i < n_samples; ++i)
  {
    blaze::column(samples, i) = usvg::rmvnormal(prng, n_dims);
  }

  size_t i = 0;
  auto row = blaze::row(samples, i);
  REQUIRE( !kolmogorov_smirnoff_test(0.01, normal_cdf, row.begin(), row.end()) );

  ++i;
  row = blaze::row(samples, i);
  REQUIRE( !kolmogorov_smirnoff_test(0.01, normal_cdf, row.begin(), row.end()) );

  ++i;
  row = blaze::row(samples, i);
  REQUIRE( !kolmogorov_smirnoff_test(0.01, normal_cdf, row.begin(), row.end()) );
}

TEST_CASE("Dense multivariate normal sampling", "[mvnormal]")
{
  auto cov = blaze::DynamicMatrix<double>(
    {{16,  1,  1},
     {1,  16,  1},
     {1,   1, 16}});
  auto mean = blaze::DynamicVector<double>(
    {1.0, 2.0, 3.0});

  auto key      = GENERATE(range(0u, 8u));
  auto prng     = usvg::Random123(key);
  auto cov_chol = usvg::Cholesky<usvg::DenseChol>();
  REQUIRE_NOTHROW( cov_chol = usvg::cholesky_nothrow(cov).value() );

  size_t n_samples = 512;
  size_t n_dims    = 3;
  auto samples     = blaze::DynamicMatrix<double>(n_dims, n_samples);
  for (size_t i = 0; i < n_samples; ++i)
  {
    blaze::column(samples, i) = usvg::rmvnormal(prng, mean, cov_chol);
  }

  size_t i = 0;
  auto row = blaze::row(samples, i);
  auto cdf = [&](double x){
    return normal_cdf((x -  mean[i]) / sqrt(cov(i,i)));
  };
  REQUIRE( !kolmogorov_smirnoff_test(0.01, cdf, row.begin(), row.end()) );

  ++i;
  row = blaze::row(samples, i);
  REQUIRE( !kolmogorov_smirnoff_test(0.01, cdf, row.begin(), row.end()) );

  ++i;
  row = blaze::row(samples, i);
  REQUIRE( !kolmogorov_smirnoff_test(0.01, cdf, row.begin(), row.end()) );
}

TEST_CASE("Diagonal multivariate normal sampling", "[mvnormal]")
{
  auto cov  = blaze::DynamicVector<double>({16, 16, 16});
  auto mean = blaze::DynamicVector<double>({1.0, 2.0, 3.0});

  auto key      = GENERATE(range(0u, 8u));
  auto prng     = usvg::Random123(key);
  auto cov_chol = usvg::Cholesky<usvg::DiagonalChol>();
  REQUIRE_NOTHROW( cov_chol = usvg::cholesky_nothrow(cov).value() );

  size_t n_samples = 512;
  size_t n_dims    = 3;
  auto samples     = blaze::DynamicMatrix<double>(n_dims, n_samples);
  for (size_t i = 0; i < n_samples; ++i)
  {
    blaze::column(samples, i) = usvg::rmvnormal(prng, mean, cov_chol);
  }

  size_t i = 0;
  auto row = blaze::row(samples, i);
  auto cdf = [&](double x){
    return normal_cdf((x -  mean[i]) / sqrt(cov[i]));
  };
  REQUIRE( !kolmogorov_smirnoff_test(0.01, cdf, row.begin(), row.end()) );

  ++i;
  row       = blaze::row(samples, i);
  REQUIRE( !kolmogorov_smirnoff_test(0.01, cdf, row.begin(), row.end()) );

  ++i;
  row       = blaze::row(samples, i);
  REQUIRE( !kolmogorov_smirnoff_test(0.01, cdf, row.begin(), row.end()) );
}

TEST_CASE("Laplace approximated normal sampling", "[mvnormal]")
{
  auto key  = GENERATE(range(0u, 8u));
  auto prng = usvg::Random123(key);
  auto K    = blaze::DynamicMatrix<double>(
    {{3.8908,    0.974802,  0.475912},
     {0.974802,  4.03892,   0.502967},
     {0.475912,  0.502967,  3.56278}});
  auto W = blaze::DynamicMatrix<double>(
    {{0.01,    0,    0},
     {   0, 0.01,    0},
     {   0,    0, 0.01}});

  auto Kinv = K;
  blaze::invert(Kinv);

  auto KinvpWinv = blaze::evaluate(Kinv + W);
  blaze::invert(KinvpWinv);

  auto mean = blaze::DynamicVector<double>(
    {-0.25005743001925373,
     0.5300156020399001,
     0.7143122346336731});

  auto cov_chol = usvg::Cholesky<usvg::DenseChol>();
  REQUIRE_NOTHROW( cov_chol = usvg::cholesky_nothrow(K).value() );

  auto id         = blaze::IdentityMatrix<double>(3);
  auto IpLBL      = id + blaze::trans(cov_chol.L)*W*cov_chol.L;
  auto IpLBL_chol = usvg::Cholesky<usvg::DenseChol>();
  REQUIRE_NOTHROW( IpLBL_chol = usvg::cholesky_nothrow(IpLBL).value() );

  auto dist = usvg::MvNormal<usvg::LaplaceNormal>{mean, cov_chol.L, IpLBL_chol.L};
  
  size_t n_samples = 512;
  size_t n_dims    = 3;
  auto samples     = blaze::DynamicMatrix<double>(n_dims, n_samples);
  for (size_t i = 0; i < n_samples; ++i)
  {
    auto z = usvg::rmvnormal(prng, n_dims);
    blaze::column(samples, i) = usvg::unwhiten(dist, z);
  }

  size_t i = 0;
  auto row = blaze::row(samples, i);
  auto cdf = [&](double x){
    return normal_cdf((x -  mean[i]) / sqrt(KinvpWinv(i,i)));
  };
  REQUIRE( !kolmogorov_smirnoff_test(0.01, cdf, row.begin(), row.end()) );

  ++i;
  row       = blaze::row(samples, i);
  REQUIRE( !kolmogorov_smirnoff_test(0.01, cdf, row.begin(), row.end()) );

  ++i;
  row       = blaze::row(samples, i);
  REQUIRE( !kolmogorov_smirnoff_test(0.01, cdf, row.begin(), row.end()) );
}

TEST_CASE("Laplace approximated normal density", "[mvnormal]")
{
  auto key  = GENERATE(range(0u, 8u));
  auto prng = usvg::Random123(key);
  auto K    = blaze::DynamicMatrix<double>(
    {{3.8908,    0.974802,  0.475912},
     {0.974802,  4.03892,   0.502967},
     {0.475912,  0.502967,  3.56278}});
  auto W = blaze::DynamicMatrix<double>(
    {{3.24731,   0.965769,  0.891059},
     {0.965769,  3.11808,   1.24221},
     {0.891059,  1.24221,   4.99718}});

  auto Kinv = K;
  blaze::invert(Kinv);

  auto KinvpWinv = blaze::evaluate(Kinv + W);
  blaze::invert(KinvpWinv);

  auto mean = blaze::DynamicVector<double>(
    {-0.25005743001925373,
     0.5300156020399001,
     0.7143122346336731});

  auto K_chol = usvg::Cholesky<usvg::DenseChol>();
  REQUIRE_NOTHROW( K_chol = usvg::cholesky_nothrow(K).value() );

  auto id         = blaze::IdentityMatrix<double>(3);
  auto IpUBL      = id + blaze::trans(K_chol.L)*W*K_chol.L;
  auto IpUBL_chol = usvg::Cholesky<usvg::DenseChol>();
  REQUIRE_NOTHROW( IpUBL_chol = usvg::cholesky_nothrow(IpUBL).value() );
  auto dist       = usvg::MvNormal<usvg::LaplaceNormal>{mean, K_chol.L, IpUBL_chol.L};

  auto cov_chol = usvg::Cholesky<usvg::DenseChol>();
  REQUIRE_NOTHROW( cov_chol = usvg::cholesky_nothrow(KinvpWinv).value() );
  auto true_dist =  usvg::MvNormal<usvg::DenseChol>{mean, cov_chol};

  auto x = usvg::rmvnormal(prng, 3);
  REQUIRE( dist.logpdf(x) == Approx(true_dist.logpdf(x)) );
}

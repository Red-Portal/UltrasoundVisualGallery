
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

#include "../src/misc/quadrature.hpp"

#include <cmath>
#include <numbers>

TEST_CASE("Integrating over Gaussian pdf", "[quadrature]")
{
  double sigma = 3.0;
  double mu    = -2.0;
  auto f = [=](double x){
    return sqrt(2)*sigma*x + mu;
  };
  REQUIRE( usdg::gauss_hermite(f) / sqrt(std::numbers::pi) == Approx(mu) );
}


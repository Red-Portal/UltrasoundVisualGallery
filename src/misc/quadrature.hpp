
/* 
 * Retrieved from http://www.mymathlib.com/quadrature/gauss_hermite.html 
 * Modified by Ray Kim
 */

#ifndef __US_GALLERY_QUADRATURE_HPP__
#define __US_GALLERY_QUADRATURE_HPP__

#include <blaze/math/DynamicVector.h>

namespace usdg
{
  template <typename Func>
  inline double
  gauss_hermite(Func f)
  {
    auto const x = blaze::DynamicVector<double>({
	-7.125813909830727, -6.40949814926966,
	-5.8122259495159145, -5.27555098651588,
	-4.777164503502596, -4.305547953351199,
	-3.8537554854714444, -3.4171674928185705,
	-2.992490825002374, -2.577249537732317,
	-2.1694991836061117, -1.7676541094632017,
	-1.3703764109528718, -0.9765004635896828,
	-0.5849787654359324, -0.19484074156939934,
	0.19484074156939934, 0.5849787654359324,
	0.9765004635896828, 1.3703764109528718,
	1.7676541094632017, 2.1694991836061117,
	2.577249537732317, 2.992490825002374,
	3.4171674928185705, 3.8537554854714444,
	4.305547953351199, 4.777164503502596,
	5.27555098651588, 5.8122259495159145,
	6.40949814926966, 7.125813909830727
      });
    auto const w = blaze::DynamicVector<double>({
	7.310676396298276e-23, 9.231736536518246e-19,
	1.1973440170928282e-15, 4.2150102113264933e-13,
	5.933291463396681e-11, 4.098832164770859e-9,
	1.5741677925455921e-7, 3.6505851295623934e-6,
	5.416584061819992e-5, 0.0005362683655279712,
	0.0036548903266544488, 0.01755342883157343,
	0.06045813095591266, 0.15126973407664218,
	0.2774581423025294, 0.3752383525928031,
	0.3752383525928031, 0.2774581423025294,
	0.15126973407664218, 0.06045813095591266,
	0.01755342883157343, 0.0036548903266544488,
	0.0005362683655279712, 5.416584061819992e-5,
	3.6505851295623934e-6, 1.5741677925455921e-7,
	4.098832164770859e-9, 5.933291463396681e-11,
	4.2150102113264933e-13, 1.1973440170928282e-15,
	9.231736536518246e-19, 7.310676396298276e-23
      });
    return blaze::dot(blaze::map(x, f), w);
  }
}

#endif

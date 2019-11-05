#ifndef OFEC_MATH_AUX_H
#define OFEC_MATH_AUX_H

#include <vector>
#include "../../../../core/definition.h"

namespace OFEC {
	namespace math_aux {
		// ASF(): achievement scalarization function
		real ASF(const std::vector<real> &objs, const std::vector<real> &weight);

		// guassian_elimination(): used to calculate the hyperplane
		//
		// Given a NxN matrix A and a Nx1 vector b, generate a Nx1 vector x
		// such that Ax = b.
		//
		// Use this to get a hyperplane for a given set of points.
		// Example.
		//
		// Given three points (-1, 1, 2), (2, 0, -3), and (5, 1, -2).
		// The equation of the hyperplane is ax+by+cz=d, or a'x +b'y + c'z = 1.
		// So we have
		//
		//    (-1)a' + b' + 2c' = 1.
		//    2a' - 3c' = 1.
		//    5a' + b' -2c' = 1.
		//
		// Let A be { {-1, 1, 2}, {2, 0, -3}, {5, 1, -2} } and b be {1, 1, 1}.
		// This function will generate x as {-0.4, 1.8, -0.6},
		// which means the equation is (-0.4)x + 1.8y - 0.6z = 1, or 2x-9y+3z+5=0.
		//
		// The intercepts are {1/-0.4, 0, 0}, {0, 1/1.8, 0}, and {0, 0, 1/-0.6}.
		//
		// Code example:
		//
		//    vector<real> x, b = {1, 1, 1};
		//    vector< vector<real> > A = { {-1, 1, 2}, {2, 0, -3}, {5, 1, -2}  };
		//
		//    GuassianElimination(x, A, b);
		//    cout << x[0] << ' ' << x[1] << ' ' << x[2] << endl;
		// ---------------------------------------------------------------------
		void guassian_elimination(std::vector<real> *px, std::vector< std::vector<real> > A, const std::vector<real> &b);

		// perpendicular_distance(): calculate the perpendicular distance from a point to a line
		//
		// Given a direction vector (w1, w2) and a point P(x1, y1),
		// we want to find a point Q(x2, y2) on the line connecting (0, 0)-(w1, w2)
		// such that (x1-x2, y1-y2) is perpendicular to (w1, w2).
		//
		// Since Q is on the line (0, 0)-(w1, w2), it should be (w1*k, w2*k).
		// (x1-w1*k, y1-w2*k).(w1, w2) = 0. (inner product)
		// => k(w1^2 + w2^2) = w1x1 + w2x2
		// => k = (w1x1 + w2x2)/(w1^2 +w2^2).
		//
		// After obtaining k, we have Q = (w1*k, w2*k) and the distance between P and Q.
		//
		// Code example:
		//    vector<real> dir{1, 3}, point{5.5, 1.5};
		//    cout << PerpendicularDistance(dir, point) << endl;
		// ---------------------------------------------------------------------
		real perpendicular_distance(const std::vector<real> &direction, const std::vector<real> &point);
	}
}

#endif // !OFEC_MATH_AUX_H

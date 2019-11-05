#include "UF05.h"

namespace OFEC {
	UF05::UF05(param_map & v) : UF05(v.at("proName"), v.at("numDim")) { //param_numDim = 30 is suggested

	}
	UF05::UF05(const std::string & name, size_t size_var) : problem(name, size_var, 2), UF(name, size_var, 2) {

	}
	void UF05::initialize() {
		std::vector<std::pair<real, real>> r;
		r.push_back(std::make_pair(0., 1.));
		for (int i = 1; i < m_variable_size; ++i) {
			r.push_back(std::make_pair(-1., 1.));
		}
		set_init_range(r);
		set_range(r);
		load_PF();
	}

	void UF05::evaluate_objective(real *x, std::vector<real> &obj) {
		int count1, count2;
		real sum1, sum2, yj, hj, N, E;
		sum1 = sum2 = 0.0;
		count1 = count2 = 0;
		N = 10.0; E = 0.1;
		for (int j = 2; j <= m_variable_size; j++) {
			yj = x[j - 1] - sin(6.0*OFEC_PI*x[0] + j * OFEC_PI / m_variable_size);
			hj = 2.0*yj*yj - cos(4.0*OFEC_PI*yj) + 1.0;
			if (j % 2 == 0) {
				sum2 += hj;
				count2++;
			}
			else {
				sum1 += hj;
				count1++;
			}
		}
		hj = (0.5 / N + E)*fabs(sin(2.0*N*OFEC_PI*x[0]));
		if (count1 == 0)
			obj[0] = x[0] + hj;
		else
			obj[0] = x[0] + hj + 2.0*sum1 / (real)count1;

		obj[1] = 1.0 - x[0] + hj + 2.0*sum2 / (real)count2;
	}

}
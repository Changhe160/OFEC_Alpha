#include "UF07.h"

namespace OFEC {
	UF07::UF07(param_map & v) : UF07(v.at("proName"), v.at("numDim")) { //param_numDim = 30 is suggested

	}
	UF07::UF07(const std::string & name, size_t size_var) : problem(name, size_var, 2), UF(name, size_var, 2) {

	}
	void UF07::initialize() {
		std::vector<std::pair<real, real>> r;
		r.push_back(std::make_pair(0., 1.));
		for (int i = 1; i < m_variable_size; ++i) {
			r.push_back(std::make_pair(-1., 1.));
		}
		set_init_range(r);
		set_range(r);
		load_PF();
	}
	void UF07::evaluate_objective(real *x, std::vector<real> &obj) {
		int count1, count2;
		real sum1, sum2, yj;
		sum1 = sum2 = 0.0;
		count1 = count2 = 0;
		for (int j = 2; j <= m_variable_size; j++) {
			yj = x[j - 1] - sin(6.0*OFEC_PI*x[0] + j * OFEC_PI / m_variable_size);
			if (j % 2 == 0)	{
				sum2 += yj * yj;
				count2++;
			}
			else {
				sum1 += yj * yj;
				count1++;
			}
		}
		yj = pow(x[0], 0.2);
		if (count1 == 0)
			obj[0] = yj;
		else
			obj[0] = yj + 2.0*sum1 / (real)count1;

		obj[1] = 1.0 - yj + 2.0*sum2 / (real)count2;
	}

}
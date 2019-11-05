#include "UF03.h"

namespace OFEC {
	UF03::UF03(param_map & v) : UF03(v.at("proName"), v.at("numDim")) { //param_numDim = 30 is suggested

	}
	UF03::UF03(const std::string & name, size_t size_var) : problem(name, size_var, 2), UF(name, size_var, 2) {

	}

	void UF03::evaluate_objective(real *x, std::vector<real> &obj) {
		int count1, count2;
		real sum1, sum2, prod1, prod2, yj, pj;
		sum1 = sum2 = 0.0;
		count1 = count2 = 0;
		prod1 = prod2 = 1.0;
		for (int j = 2; j <= m_variable_size; j++) {
			if (m_variable_size == 2)
				yj = 0;
			else
				yj = x[j - 1] - pow(x[0], 0.5*(1.0 + 3.0*(j - 2.0) / (m_variable_size - 2.0)));
			pj = cos(20.0*yj*OFEC_PI / std::sqrt(j + 0.0));
			if (j % 2 == 0){
				sum2 += yj * yj;
				prod2 *= pj;
				count2++;
			}
			else{
				sum1 += yj * yj;
				prod1 *= pj;
				count1++;
			}
		}
		if (m_variable_size == 2) {
			obj[0] = x[0];
			obj[1] = 1.0 - std::sqrt(x[0]);
		}
		else {
			obj[0] = x[0] + 2.0 * (4.0*sum1 - 2.0*prod1 + 2.0) / (real)count1;
			obj[1] = 1.0 - std::sqrt(x[0]) + 2.0*(4.0*sum2 - 2.0*prod2 + 2.0) / (real)count2;
		}

	}

}
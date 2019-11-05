#include "UF08.h"

namespace OFEC {
	UF08::UF08(param_map & v) : UF08(v.at("proName"), v.at("numDim")) { //param_numDim = 30 is suggested

	}
	UF08::UF08(const std::string & name, size_t size_var) : problem(name, size_var, 3), UF(name, size_var, 3) {

	}
	void UF08::initialize() {
		std::vector<std::pair<real, real>> r;
		r.push_back(std::make_pair(0., 1.));
		r.push_back(std::make_pair(0., 1.));
		for (int i = 2; i < m_variable_size; ++i) {
			r.push_back(std::make_pair(-2., 2.));
		}
		set_init_range(r);
		set_range(r);
		load_PF();
	}
	void UF08::evaluate_objective(real *x, std::vector<real> &obj) {
		int count1, count2, count3;
		real sum1, sum2, sum3, yj;
		sum1 = sum2 = sum3 = 0.0;
		count1 = count2 = count3 = 0;
		for (int j = 3; j <= m_variable_size; j++) {
			yj = x[j - 1] - 2.0*x[1] * sin(2.0*OFEC_PI*x[0] + j * OFEC_PI / m_variable_size);
			if (j % 3 == 1)
			{
				sum1 += yj * yj;
				count1++;
			}
			else if (j % 3 == 2)
			{
				sum2 += yj * yj;
				count2++;
			}
			else
			{
				sum3 += yj * yj;
				count3++;
			}
		}
		if (count1 == 0)
			obj[0] = cos(0.5*OFEC_PI*x[0])*cos(0.5*OFEC_PI*x[1]);
		else
			obj[0] = cos(0.5*OFEC_PI*x[0])*cos(0.5*OFEC_PI*x[1]) + 2.0*sum1 / (real)count1;

		if (count2 == 0)
			obj[1] = cos(0.5*OFEC_PI*x[0])*sin(0.5*OFEC_PI*x[1]);
		else
			obj[1] = cos(0.5*OFEC_PI*x[0])*sin(0.5*OFEC_PI*x[1]) + 2.0*sum2 / (real)count2;

		if (count3 == 0)
			obj[2] = sin(0.5*OFEC_PI*x[0]);
		else
			obj[2] = sin(0.5*OFEC_PI*x[0]) + 2.0*sum3 / (real)count3;

	}

}
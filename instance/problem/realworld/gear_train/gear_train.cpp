
#include "gear_train.h"
namespace OFEC {
	gear_train::gear_train(param_map &v) :problem((v.at("proName")), 4, 1), \
		function((v.at("proName")), 4, 1) {
		

	}

	gear_train::gear_train(const std::string &name, size_t size_var, size_t size_obj) :problem(name, 4, 1), \
		function(name, 4, 1) {
		
	}

	void gear_train::initialize() {
		set_tag(std::set<problem_tag>({ problem_tag::GOP, problem_tag::CONT }));
		m_variable_monitor = true;
		set_range(12, 60);
		set_init_range(12, 60);
		std::vector<real> v(m_variable_size, 0);
		v[0] = 15; v[1] = 20; v[2] = 57; v[3] = 59;

		set_original_global_opt(v.data());
		m_optima = m_original_optima;
	}


	void gear_train::evaluate__(real *x, std::vector<real>& obj) {

		int x1, x2, x3, x4;
		real s;

		x1 = (int)x[0]; x2 = (int)x[1]; x3 = (int)x[2]; x4 = (int)x[3];
		s = 1. / 6.931 - x1*x2 / (double)(x3*x4);

		obj[0] = s*s;

	}
}
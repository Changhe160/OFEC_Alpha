#include "michalewicz.h"
namespace OFEC {
	
	michalewicz::michalewicz(param_map &v) : problem((v[param_proName]), (v[param_numDim]), 1), \
		function((v[param_proName]), (v[param_numDim]), 1), m_m(20) {
		v[param_numDim] = 2;

		initialize();
	}
	michalewicz::michalewicz(const std::string &name, size_t size_var, size_t size_obj) : problem(name, size_var, size_obj), \
		function(name, size_var, size_obj), m_m(20) {

		initialize();
	}

	void michalewicz::initialize() {

		set_range(0, OFEC_PI);
		set_init_range(0, OFEC_PI);
		m_variable_accuracy = 1.e-3;
		m_objective_accuracy = 0.2;
		m_opt_mode[0] = optimization_mode::Maximization;

		m_original_optima.set_number_variable(2); //1 gopt + 1 lopt
		std::vector<std::vector<real>> var_data = { { 2.20291, 1.5708 },{ 2.20291, 2.71157 } };
		for (auto &i : var_data) {
			set_original_global_opt(i.data());
		}

		m_optima = m_original_optima;
		add_tag(problem_tag::MMP);
		//setObjSet();
	}
	void michalewicz::evaluate__(real *x, std::vector<real>& obj) {
		double s = 0;

		for (int i = 0; i < m_variable_size; ++i) {
			s += sin(x[i])*pow(sin((i + 1)*x[i] * x[i] / OFEC_PI), m_m);
		}
		obj[0] = s + m_bias;
	}
	
}
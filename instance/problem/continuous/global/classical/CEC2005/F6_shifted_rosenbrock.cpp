#include "F6_shifted_rosenbrock.h"

namespace OFEC {
	F6_shifted_rosenbrock::F6_shifted_rosenbrock(param_map &v) :problem((v[param_proName]), (v[param_numDim]), 1), \
		rosenbrock((v[param_proName]), (v[param_numDim]), 1) {
		
		initialize();
	}
	F6_shifted_rosenbrock::F6_shifted_rosenbrock(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
		rosenbrock(name, size_var, size_obj) {
		
		initialize();
	}

	F6_shifted_rosenbrock::~F6_shifted_rosenbrock() {
		//dtor
	}
	
	void F6_shifted_rosenbrock::initialize() {

		set_bias(390);
		m_translation.resize(m_variable_size);
		bool is_load = load_translation("instance/problem/continuous/global/classical/CEC2005/data/");  //data path
		if (!is_load) {
			std::vector<real> temp_var(m_variable_size);
			for (size_t i = 0; i < m_variable_size; ++i)
				temp_var[i] = m_original_optima.variable(0)[i];
			set_translation(temp_var);
		}
		clear_global_opt();
		set_global_opt(m_translation.data());
	}
	void F6_shifted_rosenbrock::evaluate__(real *x, vector<real>& obj) {
		
		rosenbrock::evaluate__(x, obj);
	}
}
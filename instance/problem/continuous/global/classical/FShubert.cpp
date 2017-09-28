#include "FShubert.h"

namespace OFEC {
	FShubert::FShubert(param_map &v) :problem((v[param_proName]), (v[param_numDim]), 1), \
		function((v[param_proName]), (v[param_numDim]), 1) {

		initialize();
	}
	FShubert::FShubert(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
		function(name, size_var, size_obj) {

		initialize();
	}

	FShubert::~FShubert() {
		//dtor
	}
	void FShubert::initialize() {
		std::vector<std::pair<real,real>> range;
		for (size_t i = 0; i < m_variable_size; ++i) {
			range.push_back(make_pair(10,-10));
		}
		set_range(range);
		m_variable_accuracy = 0.5;
		size_t num = m_variable_size*(size_t)pow(3, m_variable_size);
		m_original_optima.set_number_variable(num);
		if (m_variable_size == 2) {
			m_objective_accuracy = 1.e-4;
			
			//ifstream in;
			//stringstream ss;
			//ss << global::ms_arg[param_workingDir] << "Problem/FunctionOpt/Data/" << m_name << "_Opt_" << m_variable_size << "Dim.txt";
			//in.open(ss.str().c_str());
			//if (in.fail()) {
			//	throw myexcept("cannot open data file@FShubert::initialize()");
			//}
			//for (size_t i = 0; i < 18; ++i) {
			//	double x0, x1;
			//	in >> x0 >> x1;
			//	m_globalOpt[i].data().m_x[0] = x0; m_globalOpt[i].data().m_x[1] = x1; m_globalOpt[i].data().m_obj[0] = -186.7309;
			//}
			//in.close();
			for (size_t i = 0; i < num; ++i) {
				m_original_optima.append(-186.7309);
			}
		}
		else if (m_variable_size == 3) {
			m_objective_accuracy = 1.e-4;
			for (size_t i = 0; i < num; ++i) {
				m_original_optima.append(-2709.09);
			}
		}
		else if (m_variable_size == 4) {
			m_objective_accuracy = 1.e-4;
			for (size_t i = 0; i < num; ++i) {
				m_original_optima.append(-39303.6);
			}
		}
		else {
			
		}
		/*m_translation.resize(m_variable_size);
		if (m_name == "FUN_FShubert") {

		}
		else if (m_name == "FUN_S_FShubert") {

		}
		else if (m_name == "FUN_R_FShubert") {

		}
		else if (m_name == "FUN_SR_FShubert") {

		}*/
		m_optima = m_original_optima;
		add_tag(problem_tag::MMP);
		//setObjSet();
	}
	void FShubert::evaluate__(real *x, vector<real>& obj) {
		double s = 1;

		for (int j = 0; j < m_variable_size; ++j) {
			double a = 0;
			for (int i = 1; i <= 5; i++) {
				a += i*cos((i + 1)*x[j] + i);
			}
			s *= a;
		}
		obj[0] = s + m_bias;

	}
}
#include "schwefel_2_6.h"
namespace OFEC {
	
	schwefel_2_6::schwefel_2_6(param_map &v) :problem((v.at("proName")), (v.at("numDim")), 1), \
		function((v.at("proName")), (v.at("numDim")), 1), m_a(v.at("numDim"), std::vector<int>(v.at("numDim"))), m_b(v.at("numDim")) {

		
	}
	schwefel_2_6::schwefel_2_6(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
		function(name, size_var, size_obj), m_a(size_var, std::vector<int>(size_var)), m_b(size_var) {

		
	}

	void schwefel_2_6::initialize_problem() {    // To be continued ..
		set_tag(std::set<problem_tag>({ problem_tag::GOP, problem_tag::CONT }));
		m_variable_monitor = true;
		set_range(-100, 100);
		set_init_range(-100, 100);
	}

	void schwefel_2_6::load_data()
	{
		std::string sa;
		std::stringstream ss;
		ss << m_variable_size << "Dim.txt";
		sa = ss.str();
		sa.insert(0, m_name + "_a_");
		sa.insert(0, "instance/problem/continuous/global/classical/data/");
		//sa.insert(0, global::ms_arg.at("workingDir"));// data path

		std::ifstream in_a;
		in_a.open(sa.data());
		if (in_a.fail()) {
			for (int i = 0; i < m_variable_size; ++i) {
				for (int j = 0; j < m_variable_size; ++j) {
					m_a[i][j] = int(-500.0 + global::ms_global->m_uniform[caller::Problem]->next() * 1000);
				}
			}
			std::ofstream out(sa.c_str());
			for (int i = 0; i < m_variable_size; ++i) {
				for (int j = 0; j < m_variable_size; j++) {
					out << m_a[i][j] << " ";
				}
			}
			out.close();
		}
		else {
			for (int i = 0; i < m_variable_size; ++i) {
				for (int j = 0; j < m_variable_size; j++) {
					in_a >> m_a[i][j];
				}
			}
		}
		in_a.close();

		m_translation.resize(m_variable_size);
		std::string so;
		so = ss.str();
		so.insert(0, m_name + "_Shift_");

		so.insert(0, "instance/problem/continuous/global/classical/data/");
		//so.insert(0, global::ms_arg.at("workingDir"));// data path

		std::ifstream in;
		in.open(so.data());
		if (in.fail()) {
			set_translation(m_original_optima.variable(0).data());
			for (int i = 0; i < m_variable_size; ++i) {
				if (i < m_variable_size / 4) m_translation[i] = -100;
				else if (i >= m_variable_size * 3 / 4 - 1) m_translation[i] = 100;
			}

			std::ofstream out(so.c_str());
			for (int j = 0; j < m_variable_size; j++)        out << m_translation[j] << " ";
			out.close();
		}
		else {
			for (int j = 0; j < m_variable_size; j++) {
				in >> m_translation[j];
			}
		}
		in.close();

		for (int i = 0; i < m_variable_size; ++i) {
			m_b[i] = 0;
			for (int j = 0; j < m_variable_size; ++j) {
				m_b[i] += m_a[i][j] * m_translation[j];
			}
		}
	}

	void schwefel_2_6::evaluate__(real *x, std::vector<real>& obj) {
		// To be continued ..
	}
	
}
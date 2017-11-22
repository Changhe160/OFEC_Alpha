#include "F12_schwefel_2_13.h"

namespace OFEC {
#pragma warning(disable:4996)
	namespace CEC2005 {
		F12_schwefel_2_13::F12_schwefel_2_13(param_map &v) :problem((v[param_proName]), (v[param_numDim]), 1), \
			function((v[param_proName]), (v[param_numDim]), 1), m_a(v[param_numDim], std::vector<int>(v[param_numDim])), \
			m_b(v[param_numDim], std::vector<int>(v[param_numDim])), m_alpha(v[param_numDim]) {

			set_range(-OFEC_PI, OFEC_PI);
			set_init_range(-OFEC_PI, OFEC_PI);
			initialize();
		}
		F12_schwefel_2_13::F12_schwefel_2_13(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
			function(name, size_var, size_obj), m_a(size_var, std::vector<int>(size_var)), m_b(size_var, std::vector<int>(size_var)), \
			m_alpha(size_var) {
			set_range(-OFEC_PI, OFEC_PI);
			set_init_range(-OFEC_PI, OFEC_PI);
			initialize();
		}

		void F12_schwefel_2_13::initialize() {

			load_data("instance/problem/continuous/global/CEC2005/data/");
			set_original_global_opt();
			set_bias(-460);
			m_translation.resize(m_variable_size);
			for (size_t i = 0; i < m_variable_size; ++i) {
				m_translation[i] = m_alpha[i];
			}
			set_global_opt(m_translation.data());
			m_variable_accuracy = 1.0e-2;
		}

		void F12_schwefel_2_13::load_data(const std::string & path)
		{
			std::string sa;
			char astr[100];
			sprintf(astr, "%d", (int)m_variable_size);
			strcat(astr, "Dim.txt");
			sa = astr;
			sa.insert(0, m_name + "_a_");

			sa.insert(0, path);
			sa.insert(0, global::ms_arg[param_workingDir]);//probDataPath

			std::string sb;
			sprintf(astr, "%d", (int)m_variable_size);
			strcat(astr, "Dim.txt");
			sb = astr;
			sb.insert(0, m_name + "_b_");
			sb.insert(0, path);
			sb.insert(0, global::ms_arg[param_workingDir]);//probDataPath

			std::string salpha;
			sprintf(astr, "%d", (int)m_variable_size);
			strcat(astr, "Dim.txt");
			salpha = astr;
			salpha.insert(0, m_name + "_alpha_");

			salpha.insert(0, path);
			salpha.insert(0, global::ms_arg[param_workingDir]);//probDataPath

			std::ifstream in_a;
			in_a.open(sa.data());
			std::ifstream in_b;
			in_b.open(sb.data());
			std::ifstream in_alpha;
			in_alpha.open(salpha.data());
			if (in_a.fail()) {
				for (int i = 0; i < m_variable_size; ++i) {
					for (int j = 0; j < m_variable_size; ++j) {
						m_a[i][j] = int(-100.0 + global::ms_global->m_uniform[caller::Problem]->next() * 200);
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

			if (in_b.fail()) {
				for (int i = 0; i < m_variable_size; ++i) {
					for (int j = 0; j < m_variable_size; ++j) {
						m_b[i][j] = int(-100.0 + global::ms_global->m_uniform[caller::Problem]->next() * 200);
					}
				}
				std::ofstream out(sb.c_str());
				for (int i = 0; i < m_variable_size; ++i) {
					for (int j = 0; j < m_variable_size; j++) {
						out << m_b[i][j] << " ";
					}
				}
				out.close();
			}
			else {
				for (int i = 0; i < m_variable_size; ++i) {
					for (int j = 0; j < m_variable_size; j++) {
						in_b >> m_b[i][j];
					}
				}
			}
			in_b.close();

			if (in_alpha.fail()) {
				for (int i = 0; i < m_variable_size; ++i) {
					m_alpha[i] = -OFEC_PI + global::ms_global->m_uniform[caller::Problem]->next() * 2 * OFEC_PI;
				}
				std::ofstream out(salpha.c_str());
				for (int i = 0; i < m_variable_size; ++i) {
					out << m_alpha[i] << " ";
				}
				out.close();
			}
			else {
				for (int i = 0; i < m_variable_size; ++i) {
					in_alpha >> m_alpha[i];
				}
			}
			in_alpha.close();
		}



		void F12_schwefel_2_13::evaluate__(real *x, std::vector<real>& obj) {
			double result = 0;

			for (int i = 0; i < m_variable_size; ++i) {
				double A = 0;
				double B = 0;
				for (int j = 0; j < m_variable_size; ++j) {
					A += m_a[i][j] * sin(m_alpha[j]) + m_b[i][j] * cos(m_alpha[j]);
					B += m_a[i][j] * sin(x[j]) + m_b[i][j] * cos(x[j]);
				}
				result += pow((A - B), 2.0);
			}
			obj[0] = result + m_bias;
		}
	}
}
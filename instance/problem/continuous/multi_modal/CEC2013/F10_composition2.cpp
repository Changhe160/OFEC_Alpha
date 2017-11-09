

#include "F10_composition2.h"
#include "../../global/classical/griewank.h"
#include "../../global/classical/weierstrass.h"
#include "../../global/classical/sphere.h"
#include "../../global/classical/rastrigin.h"


namespace OFEC {
	namespace CEC2013 {
		F10_composition2::F10_composition2(param_map &v) :problem((v[param_proName]), (v[param_numDim]), 1), \
			composition((v[param_proName]), (v[param_numDim]), 1) {
			m_num_function = 8;
			m_function.resize(m_num_function);
			m_fmax.resize(m_num_function);
			m_stretch_severity.resize(m_num_function);
			m_converge_severity.resize(m_num_function);
			m_height.resize(m_num_function);
			initialize();
		}
		F10_composition2::F10_composition2(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
			composition(name, size_var, size_obj) {
			m_num_function = 8;
			m_function.resize(m_num_function);
			m_fmax.resize(m_num_function);
			m_stretch_severity.resize(m_num_function);
			m_converge_severity.resize(m_num_function);
			m_height.resize(m_num_function);
			initialize();
		}

		void F10_composition2::set_function() {
			basic_func f(4);
			f[0] = &create_function<rastrigin>;
			f[1] = &create_function<weierstrass>;
			f[2] = &create_function<griewank>;
			f[3] = &create_function<sphere>;

			for (size_t i = 0; i < m_num_function; ++i) {
				m_function[i] = dynamic_cast<function*>(f[i / 2]("", m_variable_size, m_objective_size));
				m_function[i]->set_bias(0);
			}

			m_stretch_severity[0] = 1;				m_stretch_severity[1] = 1;
			m_stretch_severity[2] = 10;				 m_stretch_severity[3] = 10;
			m_stretch_severity[4] = 1. / 10;			 m_stretch_severity[5] = 1. / 10;
			m_stretch_severity[6] = 1 / 7.;			m_stretch_severity[7] = 1 / 7.;

			for (int i = 0; i<m_num_function; i++) {
				m_function[i]->set_scale(m_stretch_severity[i]);
			}

			for (int i = 0; i<m_num_function; i++) {
				m_converge_severity[i] = 1;
			}

			for (int i = 0; i<m_num_function; i++) {
				m_height[i] = 0;
			}
		}
		void F10_composition2::initialize() {
			add_tag(problem_tag::MMP);
			set_function();

			load_rotation("instance/problem/continuous/multi_modal/CEC2013/data/");

			compute_fmax();

			load_translation("instance/problem/continuous/multi_modal/CEC2013/data/");  //data path

			for (auto &i : m_function) {
				i->get_optima().clear();
				i->set_global_opt(i->translation().data());
			}

			for (auto &i : m_function) {
				m_optima.append(i->get_optima().variable(0));
				m_optima.append(0);
			}

			m_variable_accuracy = 1.0e-2;
			m_objective_accuracy = 0.01;
		}

		void F10_composition2::evaluate__(real *x, std::vector<real>& obj) {
			composition::evaluate__(x, obj);

		}

		void F10_composition2::set_rotation() {
			for (auto i : m_function)
				i->rotation().identify();
		}
	}
}



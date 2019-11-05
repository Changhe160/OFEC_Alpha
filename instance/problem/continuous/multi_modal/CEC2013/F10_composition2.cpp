

#include "F10_composition2.h"
#include "../../global/classical/griewank.h"
#include "../../global/classical/weierstrass.h"
#include "../../global/classical/sphere.h"
#include "../../global/classical/rastrigin.h"
#include <numeric>

namespace OFEC {
	namespace CEC2013 {

		F10_composition2::F10_composition2(const std::string &name, size_t size_var) : problem(name, size_var, 1), composition(name, size_var, 1) {}

		F10_composition2::F10_composition2(param_map &v) : F10_composition2((v.at("proName")), (v.at("numDim"))) {}

		void F10_composition2::set_function() {
			basic_func f(4);
			f[0] = &create_function<rastrigin>;
			f[1] = &create_function<weierstrass>;
			f[2] = &create_function<griewank>;
			f[3] = &create_function<sphere>;

			for (size_t i = 0; i < m_num_function; ++i) {
				m_function[i] = dynamic_cast<function*>(f[i / 2]("", m_variable_size, m_objective_size));
				m_function[i]->initialize();
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
			set_range(-5., 5.);
			set_init_range(-5., 5.);
			m_opt_mode[0] = optimization_mode::Maximization;
			m_num_function = 8;
			m_function.resize(m_num_function);
			m_fmax.resize(m_num_function);
			m_stretch_severity.resize(m_num_function);
			m_converge_severity.resize(m_num_function);
			m_height.resize(m_num_function);
			
			set_function();

			load_rotation("instance/problem/continuous/multi_modal/CEC2013/data/");

			compute_fmax();

			load_translation("instance/problem/continuous/multi_modal/CEC2013/data/");  //data path
			for (auto &i : m_function) {
				i->get_optima().clear();
				i->set_global_opt(i->translation().data());
				m_optima.append(variable_vector<real>(i->translation()));
			}
			m_optima.set_flag_variable(true);

			m_objective_monitor = true;
			for (auto &i : m_function) {
				m_optima.append(0.);
			}
			if (global::ms_arg.find("objectiveAccuracy") == global::ms_arg.end())
				global::ms_arg.insert({ "objectiveAccuracy",(real)1.e-4 });
			m_objective_accuracy = global::ms_arg.at("objectiveAccuracy");
			m_variable_accuracy = 0.01;

			m_variable_partition.clear();
			m_variable_partition.push_back(std::vector<size_t>(m_variable_size));
			std::iota(m_variable_partition[0].begin(), m_variable_partition[0].end(), 0);
			m_initialized = true;
		}

		void F10_composition2::evaluate_objective(real *x, std::vector<real> &obj) {
			composition::evaluate_objective(x, obj);
			obj[0] = -obj[0];
		}

		void F10_composition2::set_rotation() {
			for (auto i : m_function)
				i->rotation().identify();
		}
	}
}



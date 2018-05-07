
#include "F9_composition1.h"
#include "../../global/classical/griewank.h"
#include "../../global/classical/weierstrass.h"
#include "../../global/classical/sphere.h"


namespace OFEC {
	namespace CEC2013 {

		F9_composition1::F9_composition1(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
			composition(name, size_var, size_obj) {}

		F9_composition1::F9_composition1(param_map &v) :F9_composition1((v.at("proName")), (v.at("numDim")), 1) {}

		void F9_composition1::set_function() {
			basic_func f(3);
			f[0] = &create_function<griewank>;
			f[1] = &create_function<weierstrass>;
			f[2] = &create_function<sphere>;
		
			for (size_t i = 0; i < m_num_function; ++i) {
				m_function[i] = dynamic_cast<function*>(f[i / 2]("", m_variable_size, m_objective_size));
				m_function[i]->initialize();
				m_function[i]->set_bias(0);
			}


			m_stretch_severity[0] = 1; m_stretch_severity[1] = 1;
			m_stretch_severity[2] = 8; m_stretch_severity[3] = 8;
			m_stretch_severity[4] = 1 / 5.; m_stretch_severity[5] = 1 / 5.;

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
		void F9_composition1::initialize() {
			m_num_function = 6;
			m_function.resize(m_num_function);
			m_fmax.resize(m_num_function);
			m_stretch_severity.resize(m_num_function);
			m_converge_severity.resize(m_num_function);
			m_height.resize(m_num_function);
			set_range(-5., 5.);
			set_init_range(-5., 5.);
			
			set_function();

			load_rotation("instance/problem/continuous/multi_modal/CEC2013/data/");

			compute_fmax();

			load_translation("instance/problem/continuous/multi_modal/CEC2013/data/");  //data path
			
			m_objective_monitor = true;
			for (auto &i : m_function) {
				m_optima.append(0.);
			}
			if (global::ms_arg.find("objectiveAccuracy") == global::ms_arg.end())
				global::ms_arg.insert({ "objectiveAccuracy",1.e-4 });
			m_objective_accuracy = global::ms_arg.at("objectiveAccuracy");
			m_variable_accuracy = 0.01;
		}

		void F9_composition1::evaluate__(real *x, std::vector<real>& obj) {
			composition::evaluate__(x, obj);
			
		}

		void F9_composition1::set_rotation() {
			for (auto i : m_function)
				i->rotation().identify();
		}
	}
}



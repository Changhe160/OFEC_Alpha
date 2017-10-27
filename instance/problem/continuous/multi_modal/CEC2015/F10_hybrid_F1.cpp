#include "F10_hybrid_F1.h"
#include "../../global/classical/modified_schwefel.h"
#include "../../global/classical/rastrigin.h"
#include "../../global/classical/elliptic.h"
namespace OFEC {
	namespace CEC2015 {
		F10_hybrid_F1::F10_hybrid_F1(param_map &v) :problem((v[param_proName]), (v[param_numDim]), 1), \
			function((v[param_proName]), (v[param_numDim]), 1), m_num_function(3), m_function(m_num_function), m_start(m_num_function), m_dim(m_num_function) {
			set_range(-100., 100.);
			initialize();
		}
		F10_hybrid_F1::F10_hybrid_F1(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
			function(name, size_var, size_obj), m_num_function(3), m_function(m_num_function), m_start(m_num_function), m_dim(m_num_function) {
			set_range(-100., 100.);
			initialize();
		}

		F10_hybrid_F1::~F10_hybrid_F1() {
			//dtor
			if (m_num_function > 0) {
				for (auto &i : m_function)
					if (i) delete i;
			}
		}
		void F10_hybrid_F1::initialize() {
			set_bias(1000);
			size_t i, tmp;
			double f_p[3] = { 0.3,0.3,0.4 };
			basic_func f(3);
			f[0] = &create_function<modified_schwefel>;
			f[1] = &create_function<rastrigin>;
			f[2] = &create_function<elliptic>;
			
			tmp = 0;
			for (i = 0; i<m_num_function - 1; ++i)
			{
				m_dim[i] = (size_t)ceil(f_p[i] * m_variable_size);
				tmp += m_dim[i];
			}
			m_dim[m_num_function - 1] = m_variable_size - tmp;
			m_start[0] = 0;
			for (i = 1; i<m_num_function; ++i)
			{
				m_start[i] = m_start[i - 1] + m_dim[i - 1];
			}
			for (size_t i = 0; i < m_num_function; ++i) {
				m_function[i] = dynamic_cast<function*>(f[i]("", m_dim[i], m_objective_size));
				m_function[i]->set_bias(0);
			}
			
			// Set optimal solution
			variable<real> temp_var(m_variable_size);
			objective<real> temp_obj(m_objective_size);
			std::vector<real> temp;
			for (auto &i : m_function) {
				for (auto &j : i->get_optima().variable(0)) {
					temp.push_back(j);
				}
			}
			temp_var = temp;
			solution<variable<real>, real> x(std::move(temp_var), std::move(temp_obj));

			m_optima.append(x.get_variable());

			evaluate_(x, caller::Problem, false, false);
			m_optima.append(x.get_objective());
			// end set
		}
		void F10_hybrid_F1::evaluate__(real *x, vector<real>& obj) {
			
			for (size_t i = 0; i < m_num_function; ++i) {
				variable<real> temp_var(m_dim[i]);
				objective<real> temp_obj(m_objective_size);
				std::copy(x + m_start[i], x + m_start[i] + m_dim[i], temp_var.begin());
				solution<variable<real>, real> temp(std::move(temp_var), std::move(temp_obj));
				m_function[i]->evaluate_(temp,caller::Problem,false,false);
				obj[0] += temp.get_objective()[0];
			}
			obj[0] += m_bias;
		}
	}
}
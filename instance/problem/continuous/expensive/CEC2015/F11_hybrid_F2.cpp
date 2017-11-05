#include "F11_hybrid_F2.h"
#include "../../global/classical/griewank.h"
#include "../../global/classical/weierstrass.h"
#include "../../global/classical/rosenbrock.h"
#include "../../global/classical/scaffer_F6.h"
namespace OFEC {
	namespace CEC2015 {
		F11_hybrid_F2::F11_hybrid_F2(param_map &v) :problem((v[param_proName]), (v[param_numDim]), 1), \
			hybrid((v[param_proName]), (v[param_numDim]), 1) {
			m_num_function = 4;
			m_function.resize(m_num_function);
			m_start.resize(m_num_function);
			m_dim.resize(m_num_function);
			set_range(-100., 100.);
			initialize();
		}
		F11_hybrid_F2::F11_hybrid_F2(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
			hybrid(name, size_var, size_obj) {
			m_num_function = 4;
			m_function.resize(m_num_function);
			m_start.resize(m_num_function);
			m_dim.resize(m_num_function);
			set_range(-100., 100.);
			initialize();
		}
		void F11_hybrid_F2::set_function() {
			size_t i, tmp;
			double f_p[4] = { 0.2, 0.2, 0.3, 0.3 };
			basic_func f(4);
			f[0] = &create_function<griewank>;
			f[1] = &create_function<weierstrass>;
			f[2] = &create_function<rosenbrock>;
			f[3] = &create_function<scaffer_F6>;

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
		}

		void F11_hybrid_F2::initialize() {
			//set_bias(1100);
			set_function();
			size_t count = 0;
			for (auto &i : m_random_perm)
				i = count++;
			global::ms_global->m_uniform[caller::Problem]->shuffle(m_random_perm.begin(), m_random_perm.end());
			// Set optimal solution
			variable<real> temp_var(m_variable_size);
			objective<real> temp_obj(m_objective_size);
			size_t num = 0;
			for (size_t i = 0; i < m_num_function;++i) {
				for (size_t j = 0; j < m_dim[i];++j)
				    temp_var[m_random_perm[num++]] = m_function[i]->get_optima().variable(0)[j];
			}
			solution<variable<real>, real> x(std::move(temp_var), std::move(temp_obj));
			
			m_optima.append(x.get_variable());

			evaluate_(x, caller::Problem, false, false);
			m_optima.append(x.get_objective());
			// end set
		}
		void F11_hybrid_F2::evaluate__(real *x, std::vector<real>& obj) {

			hybrid::evaluate__(x, obj);
			obj[0] += 1100;
		}
	}
}
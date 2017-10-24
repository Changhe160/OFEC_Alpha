#include "F12_hybrid_F3.h"
#include "../../global/classical/katsuura.h"
#include "../../global/classical/happy_cat.h"
#include "../../global/classical/griewank_rosenbrock.h"
#include "../../global/classical/modified_schwefel.h"
#include "../../global/classical/ackley.h"
namespace OFEC {
	namespace CEC2015 {
		F12_hybrid_F3::F12_hybrid_F3(param_map &v) :problem((v[param_proName]), (v[param_numDim]), 1), \
			function((v[param_proName]), (v[param_numDim]), 1), m_num_function(5), m_function(m_num_function), m_start(m_num_function), m_dim(m_num_function) {
			set_range(-100., 100.);
			initialize();
		}
		F12_hybrid_F3::F12_hybrid_F3(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
			function(name, size_var, size_obj), m_num_function(5), m_function(m_num_function), m_start(m_num_function), m_dim(m_num_function) {
			set_range(-100., 100.);
			initialize();
		}

		F12_hybrid_F3::~F12_hybrid_F3() {
			//dtor
			if (m_num_function > 0) {
				for (auto &i : m_function)
					if (i) delete i;
			}
		}
		void F12_hybrid_F3::initialize() {

			size_t i, tmp;
			double f_p[5] = {0.1, 0.2, 0.2, 0.2, 0.3 };
			basic_func f(5);
			f[0] = &create_function<katsuura>;
			f[1] = &create_function<happy_cat>;
			f[2] = &create_function<griewank_rosenbrock>;
			f[3] = &create_function<modified_schwefel>;
			f[4] = &create_function<ackley>;

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
		void F12_hybrid_F3::evaluate__(real *x, vector<real>& obj) {

			for (size_t i = 0; i < m_num_function; ++i) {
				variable<real> temp_var(m_dim[i]);
				objective<real> temp_obj(m_objective_size);
				std::copy(x + m_start[i], x + m_start[i] + m_dim[i], temp_var.begin());
				solution<variable<real>, real> temp(std::move(temp_var), std::move(temp_obj));
				m_function[i]->evaluate_(temp, caller::Problem, false, false);
				obj[0] += temp.get_objective()[0];
			}
			obj[0] += m_bias;
		}
	}
}
#include "DynDE.h"

namespace OFEC {
	namespace DE {
		DynDE::DynDE(param_map &v) :population(v.at("popSize"), global::ms_global->m_problem->variable_size()), \
			m_r_excl(v.at("exlRadius")), m_sub_population(v.at("popSize") / v.at("subPopSize"), v.at("subPopSize")){
			//set_default(v);

			//m_parameter << "M(N,Nbrownian):" << m_M << "(" << 5 << "," << 5 << "); Rexcl:" << m_r_excl << "; Rcloud:" << 0.2;
		}


		void DynDE::set_default(param_map &v) {
		
		}

		void DynDE::exclude() {
			for (size_t i = 0; i < m_sub_population.size(); ++i) {
				for (size_t j = i + 1; j < m_sub_population.size(); ++j) {
					if (m_sub_population[i].m_flag == false && m_sub_population[j].m_flag == false && m_sub_population[i].best()[0]->variable_distance(*m_sub_population[j].best()[0]) < m_r_excl) {
						if (m_sub_population[i].best()[0]->dominate(*m_sub_population[j].best()[0])) {
							m_sub_population[j].m_flag = true;
						}
						else {
							m_sub_population[i].m_flag = true;
						}
					}
				}
			}
		}
		evaluation_tag DynDE::run_() {
			evaluation_tag tag = evaluation_tag::Normal;

			std::vector<double> gopt(1);
			gopt = CONTINOUS_CAST->get_optima().multi_objective(0);
			while (tag != evaluation_tag::Terminate) {
				double best = problem::get_sofar_best<solution<>>(0)->get_objective()[0];
				double error = fabs(best - gopt[0]);
				int num_opt_found = CONTINOUS_CAST->num_optima_found();

				std::cout << m_iter << " " << CONTINOUS_CAST->total_evaluations() << " " << error << ' ' << num_opt_found << std::endl;
				//std::cout << m_iter << " " << CONTINOUS_CAST->total_evaluations() << " " << error << ' ' << std::endl;

				measure::ms_measure->record(global::ms_global.get(), m_iter, num_opt_found);

				for (size_t i = 0; i < m_sub_population.size(); ++i) {
					if (!m_sub_population[i].m_flag)       
						tag = m_sub_population[i].evolve();

					handle_evaluation_tag(tag);
				}

				if (tag == evaluation_tag::Terminate) break;
				//exclusion
				exclude();

				for (size_t i = 0; i < m_sub_population.size(); ++i) {
					{
						m_sub_population[i].initialize();
					}
					if (terminating()) break;
					m_sub_population[i].m_flag = false;
				}
				++m_iter;
			}
			// output objective found
			std::vector<solution< variable<real>, real >> test = CONTINOUS_CAST->get_optima_found();
			for (size_t i = 0; i < CONTINOUS_CAST->num_optima_found(); ++i) {
				std::cout << i + 1 << " " << CONTINOUS_CAST->get_optima_found()[i].get_objective()[0] << " " << std::endl;
				std::cout << " " << " " << CONTINOUS_CAST->get_optima_found()[i].get_variable()[0] << " " << CONTINOUS_CAST->get_optima_found()[i].get_variable()[1] << std::endl;
			}

			return tag;

		}
	}
}

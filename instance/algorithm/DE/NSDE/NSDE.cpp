#include "NSDE.h"

namespace OFEC {
	namespace DE {
		NSDE::NSDE(param_map &v) :population(v.at("popSize"), global::ms_global->m_problem->variable_size()), m_order_list(v.at("popSize")), \
			m_m(9), m_dis((v.at("popSize") / (m_m + 1)) + 1), m_seed((v.at("popSize") / (m_m + 1)) + 1) \
		{
			set_mutation_strategy(DE_rand_1);
			set_parmeter(0.7, 0.5);  // set CR=0.7, F=0.5
		}


		void NSDE::select_subpopulation() {
			int i = 0;
			sort();
			while (m_order_list.size() > 0) {
				m_seed[i] = m_order_list[0];
				for (size_t j = 0; j < m_order_list.size(); ++j) {
					std::pair<double, int> dis = std::make_pair(m_pop[m_seed[i]]->variable_distance(*m_pop[m_order_list[j]]), m_order_list[j]);
					auto it = m_dis[i].begin();
					while (it != m_dis[i].end() && it->first < dis.first) {
						it++;
					}
					if (m_dis[i].size() >= m_m + 1) {
						if (it != m_dis[i].end()) {
							m_dis[i].insert(it, dis);
							m_dis[i].pop_back();
						}
					}
					else {
						m_dis[i].insert(it, dis);
					}
				}
				for (auto it = m_dis[i].begin(); it != m_dis[i].end(); it++) {
					auto it_ = find(m_order_list.begin(), m_order_list.end(), it->second);
					m_order_list.erase(it_);
				}
				++i;
			}
		}
		void NSDE::sort() {
			std::vector<bool> flag;
			flag.resize(size());
			for (size_t i = 0; i < size(); ++i) {
				flag[i] = 0;
			}
			int index;
			for (size_t i = 0; i < size(); ++i) {
				size_t j = 0;
				while (j < size() && flag[j] == 1) ++j;
				index = j;

				for (j = index; j < size(); ++j)
					if (flag[j] == 0 && m_pop[j]->dominate(*m_pop[index]))	 index = j;

				m_order_list[i] = index;
				flag[index] = 1;
			}
			flag.clear();
		}

		evaluation_tag NSDE::evolve() {
			evaluation_tag tag = evaluation_tag::Normal;
			double u, l;
			for (size_t i = 0; i < m_dis.size(); ++i) {
				for (auto it = m_dis[i].begin(); it != m_dis[i].end(); it++) {
					mutate(it->second);
					m_pop[it->second]->recombine(m_CR);
					tag = m_pop[it->second]->select();
					if (m_seed[i] != it->second) {
						if (m_pop[m_seed[i]]->equal(*m_pop[it->second])) {
							for (size_t k = 0; k < m_pop[it->second]->variable().size(); ++k) {
								l = CONTINOUS_CAST->range(k).first;
								u = CONTINOUS_CAST->range(k).second;

								m_pop[it->second]->variable()[k] = global::ms_global->m_uniform[caller::Algorithm]->next_non_standard<double>(l, u);
							}
							m_pop[it->second]->evaluate();
						}
						else if (m_pop[it->second]->dominate(*m_pop[m_seed[i]])) {
							*m_pop[m_seed[i]] = *m_pop[it->second];
						}
					}
					if (tag != evaluation_tag::Normal) break;
				}
				if (tag != evaluation_tag::Normal) break;
			}


			++m_iter;


			return tag;
		}

		evaluation_tag NSDE::run_() {
			evaluation_tag tag = evaluation_tag::Normal;

			std::vector<double> gopt(1);
			gopt = CONTINOUS_CAST->get_optima().multi_objective(0);
			select_subpopulation();
			while (tag != evaluation_tag::Terminate) {
				//g_mutexStream.lock();

				double best = problem::get_sofar_best<solution<>>(0)->objective()[0];
				double error = fabs(best - gopt[0]);
				int num_opt_found = CONTINOUS_CAST->num_optima_found();

				std::cout << m_iter << " " << CONTINOUS_CAST->total_evaluations() << " " << error << ' ' << num_opt_found << std::endl;
				//std::cout << m_iter << " " << CONTINOUS_CAST->total_evaluations() << " " << error << ' ' << std::endl;

				//g_mutexStream.unlock();
				measure::get_measure()->record(global::ms_global.get(), m_iter, num_opt_found);
				tag = evolve();

			}

			// output objective found
			std::vector<solution< variable_vector<real>, real >> test = CONTINOUS_CAST->get_optima_found();
			for (size_t i = 0; i < CONTINOUS_CAST->num_optima_found(); ++i) {
					std::cout << i + 1 << " " << CONTINOUS_CAST->get_optima_found()[i].objective()[0] << " " << std::endl;
					std::cout << " " << " " << CONTINOUS_CAST->get_optima_found()[i].variable()[0] << " " << CONTINOUS_CAST->get_optima_found()[i].variable()[1] << std::endl;
			}
			/*std::cout << "global optimal solution:" << std::endl;
			for (size_t i = 0; i <CONTINOUS_CAST->get_optima().variable(0).size(); ++i) {
				std::cout << i + 1 << " " << CONTINOUS_CAST->get_optima().variable(0)[i] << " " << std::endl;
			}
			std::cout << "current optimal solution found:" << std::endl;
			for (size_t i = 0; i <problem::get_sofar_best<solution<>>(0)->variable().size(); ++i) {
				std::cout << i + 1 << " " << problem::get_sofar_best<solution<>>(0)->variable()[i] << " " << std::endl;
			}
			if (global::ms_global->m_problem->solved()) {
				std::cout << "global optimal solution found:" << std::endl;
				for (size_t i = 0; i < CONTINOUS_CAST->get_optima_found()[0].variable().size(); ++i) {
					std::cout << i + 1 << " " << CONTINOUS_CAST->get_optima_found()[0].variable()[i] << " " << std::endl;
				}
			}*/
			return tag;
		}
	}
}



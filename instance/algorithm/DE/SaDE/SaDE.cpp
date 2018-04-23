#include "SaDE.h"
namespace OFEC {
	namespace DE {
		SaDE::SaDE(param_map &v) :population(v.at("popSize"), global::ms_global->m_problem->variable_size()), m_F(v.at("popSize")), \
			m_CR(v.at("popSize"), std::vector<double>(m_num_strategy)), m_CRm(m_num_strategy, 0.5), m_probability(m_num_strategy, 1. / m_num_strategy), \
			m_strategy_selection(v.at("popSize")) {
			for (auto i = 0; i < m_probability.size(); ++i) {
				if (i > 0) m_probability[i] += m_probability[i - 1];
			}
		}


		evaluation_tag SaDE::evolve() {
			evaluation_tag tag = evaluation_tag::Normal;
			//std::vector<std::pair<char, double>> F;

			//int bidx = m_best[0];
			if (m_iter == 0) {
				update_best();
				update_archive(*m_best[0]);
			}
			for (int i = 0; i < size(); ++i) {
				double p = global::ms_global->m_uniform[caller::Algorithm]->next()*m_probability[m_num_strategy - 1];
				m_strategy_selection[i] = lower_bound(m_probability.begin(), m_probability.end(), p) - m_probability.begin();

				//std::vector<std::pair<solution<> *, solution<> *>> dif;

				mutate(m_strategy_selection[i]);
				m_pop[i]->recombine(m_CR[i][m_strategy_selection[i]]);
				if (m_pop[i]->trial().check_boundary_violation() != violation_type::None)
					m_pop[i]->trial().initialize();

			}

			for (int i = 0; i < size(); ++i) {
				tag = m_pop[i]->select();
				this->update_archive(*m_pop[i]);
				if (tag != evaluation_tag::Normal) break;
			}

			if (tag == evaluation_tag::Normal) {
				this->m_iter++;
			}

			return tag;
		}

		void SaDE::update_F() {
			for (auto i = 0; i < size(); i++) {
				m_F[i] = global::ms_global->m_normal[caller::Algorithm]->next_non_standard(0.5, 0.09);
			}
		}
		void SaDE::update_CR() {
			if (m_iter >= m_LP) {
				for (int k = 0; k < m_num_strategy; ++k) {
					std::vector<double> t;
					for (auto it = m_CRsuc.begin(); it != m_CRsuc.end(); ++it) {
						for (auto i : it->at(k)) t.push_back(i);
					}
					if (t.size() > 0) {
						nth_element(t.begin(), t.begin() + t.size() / 2, t.end());
						m_CRm[k] = t[t.size() / 2];
					}
				}
			}
			for (auto i = 0; i < size(); i++) {
				for (int k = 0; k < m_num_strategy; ++k) {
					do {
						m_CR[i][k] = global::ms_global->m_normal[caller::Algorithm]->next_non_standard(m_CRm[k], 0.01);
					} while (m_CR[i][k] < 0 || m_CR[i][k]>1);
				}
			}

		}

		void SaDE::update_memory() {

			std::vector < std::list<double>> curmem(m_num_strategy);
			std::vector<int> curSuc(m_num_strategy), curFail(m_num_strategy);
			for (auto i = 0; i < size(); i++) {
				if (m_pop[i]->is_improved()) {
					curmem[m_strategy_selection[i]].push_back(m_CR[i][m_strategy_selection[i]]);
					curSuc[m_strategy_selection[i]]++;
				}
				else {
					curFail[m_strategy_selection[i]]++;
				}
			}

			m_cnt_success.push_back(move(curSuc));
			m_CRsuc.push_back(move(curmem));
			m_cnt_fail.push_back(move(curFail));

			if (m_iter >= m_LP) {
				m_cnt_success.pop_front();
				m_CRsuc.pop_front();
				m_cnt_fail.pop_front();

				//update probability for all stategies
				double sum = 0;
				for (int k = 0; k < m_num_strategy; ++k) {
					m_probability[k] = 0;
					int fail = 0;
					for (auto &j : m_cnt_success) m_probability[k] += j[k];
					for (auto &j : m_cnt_fail) fail += j[k];

					m_probability[k] = m_probability[k] / (m_probability[k] + fail) + m_epsilon;
					if (k > 0) m_probability[k] += m_probability[k - 1];
				}

			}


		}
		evaluation_tag SaDE::run_() {
			evaluation_tag tag = evaluation_tag::Normal;

			std::vector<double> gopt(1);
			gopt = CONTINOUS_CAST->get_optima().multi_objective(0);
	
			do {
				//g_mutexStream.lock();
				//double error = fabs(gopt[0] - m_best[0]->get_objective()[0]); 
				double best = problem::get_sofar_best<solution<>>(0)->get_objective()[0];
				double error = fabs(gopt[0] - best);
				int num_obj_found = CONTINOUS_CAST->num_optima_found();
				std::cout << m_iter << ' ' << num_obj_found << ' ' << error << std::endl;
				//g_mutexStream.unlock();
				measure::ms_measure->record(global::ms_global.get(), m_iter, num_obj_found);
				update_CR();
				update_F();
				tag = evolve();
				update_memory();
			} while (tag != evaluation_tag::Terminate);
			return tag;
		}
	}
}

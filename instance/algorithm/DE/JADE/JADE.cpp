#include "JADE.h"

namespace OFEC {
	namespace DE {
		JADE::JADE(param_map &v) :population(v.at("popSize"), global::ms_global->m_problem->variable_size()), m_candi(3, nullptr), m_pcent_best_index(size()), \
			m_pcent_best(size()), mv_F(size()), mv_CR(size()) {

			m_archive_flag = 1;
			m_p = 0.2;
			m_c = 0.1;
		}

		void JADE::select_trial(int base_) {
			std::vector<int> candidate;
			for (size_t i = 0; i < size(); i++) {
				if (base_ != i) candidate.push_back(i);
			}
			int idx;
			do {
				idx = m_pcent_best_index[global::ms_global->m_uniform[caller::Algorithm]->next_non_standard<int>(0, (int)(size()*m_p))];
			} while (idx == base_);

			m_candi[0] = m_pop[idx].get();

			auto it = find(candidate.begin(), candidate.end(), idx);
			candidate.erase(it);

			idx = global::ms_global->m_uniform[caller::Algorithm]->next_non_standard<int>(0, (int)candidate.size());
			m_candi[1] = m_pop[candidate[idx]].get();
			candidate.erase(candidate.begin() + idx);

			idx = global::ms_global->m_uniform[caller::Algorithm]->next_non_standard<int>(0, (int)(candidate.size() + m_archive.size()));

			if (idx >= (int)candidate.size()) {
				m_candi[2] = &m_archive[idx - candidate.size()];
			}
			else {
				m_candi[2] = m_pop[candidate[idx]].get();
			}

		}

		evaluation_tag JADE::evolve() {
			evaluation_tag tag = evaluation_tag::Normal;
			for (auto i = 0; i < size(); ++i) {
				m_pcent_best[i] = m_pop[i]->get_objective()[0];
			}
			bool minf = global::ms_global->m_problem->opt_mode(0) == optimization_mode::Minimization ? true : false;
			quick_sort(m_pcent_best, (int)size(), m_pcent_best_index, minf);
			std::vector<int> candidate(3);

			for (int i = 0; i < size(); ++i) {
				select_trial(i);
				m_pop[i]->mutate(mv_F[i], m_pop[i].get(), m_candi[0], m_pop[i].get(), m_candi[1], m_candi[2]);
				m_pop[i]->recombine(mv_CR[i]);
			}

			for (int i = 0; i < size(); i++) {
				tag = m_pop[i]->select(m_archive);
				while (m_archive.size() > size()) {
					int ridx = global::ms_global->m_uniform[caller::Algorithm]->next_non_standard<int>(0, (int)m_archive.size());
					m_archive.erase(m_archive.begin() + ridx);
				}


				//update_archive(m_pop[i]);
				if (tag != evaluation_tag::Normal) break;

			}

			if (tag == evaluation_tag::Normal) {
				m_iter++;
			}


			return tag;
		}

		void JADE::update_F() {
			double mu = 0.5, mean = 0, mean2 = 0;
			if (m_iter > 0) {
				mean = 0;
				int cnt = 0;
				for (auto i = 0; i < size(); i++) {
					if (m_pop[i]->is_improved()) {
						cnt++;
						mean += mv_F[i];
						mean2 += mv_F[i] * mv_F[i];
					}
				}
				if (cnt > 0) {
					mean = mean2 / mean;
				}

				mu = (1 - m_c)*mu + m_c*mean;
			}
			for (auto i = 0; i < size(); i++) {
				do {
					mv_F[i] = (double)global::ms_global->m_cauchy[caller::Algorithm]->next_non_standard((real)mu, 0.01);
				} while (mv_F[i] <= 0);

				if (mv_F[i] > 1) mv_F[i] = 1;
			}
		}
		void JADE::update_CR() {
			double mu = 0.5, mean = 0;
			if (m_iter > 0) {
				mean = 0;
				int cnt = 0;
				for (auto i = 0; i < size(); i++) {
					if (m_pop[i]->is_improved()) {
						cnt++;
						mean += mv_CR[i];
					}
				}
				if (cnt > 0) {
					mean /= cnt;
				}

				mu = (1 - m_c)*mu + m_c*mean;
			}
			for (auto i = 0; i < size(); i++) {
				mv_CR[i] = (double)global::ms_global->m_normal[caller::Algorithm]->next_non_standard((real)mu, 0.01);
				if (mv_CR[i] < 0) mv_CR[i] = 0;
				else if (mv_CR[i] > 1) mv_CR[i] = 1;
			}
		}


		evaluation_tag JADE::run_() {
			evaluation_tag tag = evaluation_tag::Normal;

			while (tag != evaluation_tag::Terminate) {
				//g_mutexStream.lock();
				std::vector<real> gopt(1);
				gopt = CONTINOUS_CAST->get_optima().multi_objective(0);

				//update_best();
				double best = problem::get_sofar_best<solution<>>(0)->get_objective()[0];
				double error = fabs(best - gopt[0]);
				//std::cout << m_iter << " " << error <<" "<< m_best[0]->get_variable()[0] << " " << m_best[0]->get_variable()[1] << std::endl;
				std::cout << m_iter << " " << CONTINOUS_CAST->total_evaluations() << " " << CONTINOUS_CAST->num_optima_found() << std::endl;
				//g_mutexStream.unlock();
				measure::get_measure()->record(global::ms_global.get(), m_iter, error);
				update_CR();
				update_F();
				tag = evolve();
			}
			std::cout << m_iter + 1 << " " << CONTINOUS_CAST->num_optima_found() << std::endl;
			

			return tag;
		}
	}
}


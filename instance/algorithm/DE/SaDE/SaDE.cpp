#include "SaDE.h"
namespace OFEC {
	SaDE::SaDE(param_map &v) :DEpopulation(v[param_popSize], v[param_numDim]), mv_F(v[param_popSize]), \
		mv_CR(v[param_popSize], std::vector<double>(mc_numStr)), mv_CRm(mc_numStr, 0.5), mv_prob(mc_numStr, 1. / mc_numStr), \
		m_strSel(v[param_popSize]) {
		for (auto i = 0; i < mv_prob.size(); ++i) {
			if (i > 0) mv_prob[i] += mv_prob[i - 1];
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
			double p = global::ms_global->m_uniform[caller::Algorithm]->next()*mv_prob[mc_numStr - 1];
			m_strSel[i] = lower_bound(mv_prob.begin(), mv_prob.end(), p) - mv_prob.begin();

			//std::vector<std::pair<solution<> *, solution<> *>> dif;
		
			mutate(m_strSel[i]);
			m_pop[i]->recombine(mv_CR[i][m_strSel[i]]);
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
			mv_F[i] = global::ms_global->m_normal[caller::Algorithm]->next_non_standard(0.5, 0.09);
		}
	}
	void SaDE::update_CR() {
		if (m_iter >= m_LP) {
			for (int k = 0; k < mc_numStr; ++k) {
				std::vector<double> t;
				for (auto it = mv_CRsuc.begin(); it != mv_CRsuc.end(); ++it) {
					for (auto i : it->at(k)) t.push_back(i);
				}
				if (t.size() > 0) {
					nth_element(t.begin(), t.begin() + t.size() / 2, t.end());
					mv_CRm[k] = t[t.size() / 2];
				}
			}
		}
		for (auto i = 0; i < size(); i++) {
			for (int k = 0; k < mc_numStr; ++k) {
				do {
					mv_CR[i][k] = global::ms_global->m_normal[caller::Algorithm]->next_non_standard(mv_CRm[k], 0.01);
				} while (mv_CR[i][k] < 0 || mv_CR[i][k]>1);
			}
		}

	}

	void SaDE::update_memory() {

		std::vector < std::list<double>> curmem(mc_numStr);
		std::vector<int> curSuc(mc_numStr), curFail(mc_numStr);
		for (auto i = 0; i < size(); i++) {
			if (m_pop[i]->is_improved()) {
				curmem[m_strSel[i]].push_back(mv_CR[i][m_strSel[i]]);
				curSuc[m_strSel[i]]++;
			}
			else {
				curFail[m_strSel[i]]++;
			}
		}

		mv_cntSuc.push_back(move(curSuc));
		mv_CRsuc.push_back(move(curmem));
		mv_cntFail.push_back(move(curFail));

		if (m_iter >= m_LP) {
			mv_cntSuc.pop_front();
			mv_CRsuc.pop_front();
			mv_cntFail.pop_front();

			//update probability for all stategies
			double sum = 0;
			for (int k = 0; k < mc_numStr; ++k) {
				mv_prob[k] = 0;
				int fail = 0;
				for (auto &j : mv_cntSuc) mv_prob[k] += j[k];
				for (auto &j : mv_cntFail) fail += j[k];

				mv_prob[k] = mv_prob[k] / (mv_prob[k] + fail) + m_epsilon;
				if (k > 0) mv_prob[k] += mv_prob[k - 1];
			}

		}


	}
	evaluation_tag SaDE::run_() {
		evaluation_tag tag = evaluation_tag::Normal;

		std::vector<double> gopt(1);
		gopt = CONTINOUS_CAST->get_optima().multi_objective(0);
		//CONTINOUS_CAST->set_variable_monitor_true();
		do {
			//g_mutexStream.lock();
			//update_best();
			//double error = fabs(gopt[0] - m_best[0]->get_objective()[0]); 
			double best = problem::get_sofar_best<solution<>>(0)->get_objective()[0];
			double error = fabs(gopt[0] - best);
			int num_obj_found = CONTINOUS_CAST->num_optima_found();
			std::cout << m_iter << ' ' << num_obj_found << ' ' << error <<std::endl;
			//g_mutexStream.unlock();
			//measure::ms_measure->record(global::ms_global.get(), m_iter, num_obj_found);
			update_CR();
			update_F();
			tag = evolve();
			update_memory();
		} while (tag != evaluation_tag::Terminate);
		return tag;
	}
}

#include "jDE.h"

namespace OFEC {
	jDE::jDE(param_map &v) :DEpopulation(v[param_popSize], v[param_numDim]), \
		mv_F(v[param_popSize]), mv_CR(v[param_popSize]) {
		
		m_t1 = 0.1;
		m_t2 = 0.1;
		m_Fl = 0.1;
		m_Fu = 0.9;
	}

	evaluation_tag jDE::evolve() {
		evaluation_tag tag = evaluation_tag::Normal;

		std::vector<int> candidate(3);

		for (int i = 0; i < size(); ++i) {
			select(i,3, candidate);
			m_pop[i]->mutate(mv_F[i], &m_pop[candidate[0]]->self(), &m_pop[candidate[1]]->self(), &m_pop[candidate[2]]->self());
			m_pop[i]->recombine(mv_CR[i]);
			tag = m_pop[i]->select();
			if (tag != evaluation_tag::Normal) break;
		}

		if (tag == evaluation_tag::Normal) {
			++m_iter;
		}
		return tag;
	}

	void jDE::update_FandCR() {
		std::vector<double> rand(4);
		if (m_iter > 0) {
			for (auto i = 0; i < size(); ++i) {
				for (int j = 0; j < 4; ++j) 
					rand[j] = global::ms_global->m_uniform[caller::Algorithm]->next_non_standard<float>(0, 1);

				if (rand[1] < m_t1) mv_F[i] = m_Fl + rand[0] * m_Fu;
				if (rand[3] < m_t2) mv_CR[i] = rand[2];
			}
		}
		else {
			for (auto i = 0; i < size(); ++i) {
				for (int j = 0; j < 4; j++) 
					rand[j] = global::ms_global->m_uniform[caller::Algorithm]->next_non_standard<float>(0, 1);
				
				mv_F[i] = m_Fl + rand[0] * m_Fu;
				mv_CR[i] = rand[2];
			}
		}
	}




	evaluation_tag jDE::run_() {
		evaluation_tag tag = evaluation_tag::Normal;
		std::vector<double> gopt(1);
		gopt = CONTINOUS_CAST->get_optima().multi_objective(0);

		while (tag != evaluation_tag::Terminate) {
			//g_mutexStream.lock();
	
			//update_best();
			double best = problem::get_sofar_best<solution<>>(0)->get_objective()[0];
			double error = fabs(best - gopt[0]);
			//std::cout << m_iter << " " << error <<" "<< m_best[0]->get_variable()[0] << " " << m_best[0]->get_variable()[1] << std::endl;
			std::cout << m_iter << " " << CONTINOUS_CAST->total_evaluations() << " " << CONTINOUS_CAST->get_optima().num_optima_found() << std::endl;
			//g_mutexStream.unlock();
			measure::ms_measure->record(global::ms_global.get(), m_iter, error);
			update_FandCR();
			tag = evolve();
			//if (m_iter == 100) break;
		}
		//for (int i = 0; i < CONTINOUS_CAST->get_optima().num_objective_found(); ++i) {
		//	solution<> sol(CONTINOUS_CAST->get_optima().variable(i), CONTINOUS_CAST->get_optima().single_objective(i));
		//	CONTINOUS_CAST->evaluate_(sol,caller::Algorithm,false,false);
		//	std::cout << " " << sol.get_variable()[0] << " " << sol.get_variable()[1] << " " << sol.get_objective()[0] << std::endl;
		//}
		return tag;
	}
}



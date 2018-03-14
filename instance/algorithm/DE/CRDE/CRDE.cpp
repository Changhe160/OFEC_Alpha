#include "CRDE.h"

namespace OFEC {
	std::mutex m_mutexStream;
	CRDE::CRDE(param_map &v) :DEpopulation(v[param_popSize], v[param_numDim])
	{
		m_F = 0.5;
		m_CR = 0.9;
		m_mutStrategy = DE_rand_1;
	}

	evaluation_tag CRDE::evolve()
	{
		if (size() < 4) {
			throw myexcept("the population size cannot be smaller than 5@DEpopulation<TypeDEIndi>::evolve()");
		}

		evaluation_tag tag = evaluation_tag::Normal;

		for (int i = 0; i < size(); ++i) {
			
			mutate(i);
			m_pop[i]->recombine(m_CR);

			tag = m_pop[i]->trial().evaluate();
			if (tag != evaluation_tag::Normal) return tag;

			int idx = this->find_nearest(i);

			if (m_pop[i]->trial().dominate(*m_pop[idx])) {
				*m_pop[idx] = m_pop[i]->trial();
			}
			//update_archive(DEindividual<>(m_pop[i]->trial()));
		}
		m_iter++;
		
		return tag;
	}

	evaluation_tag CRDE::run_()
	{
		evaluation_tag tag = evaluation_tag::Normal;
		std::vector<real> gopt(1);
		gopt = CONTINOUS_CAST->get_optima().multi_objective(0);
		while (tag != evaluation_tag::Terminate)
		{
			m_mutexStream.lock();

			//update_best();
			double best = problem::get_sofar_best<solution<>>(0)->get_objective()[0];
			double error = fabs(best - gopt[0]);
			int num_opt_found = CONTINOUS_CAST->num_optima_found();
			//std::cout << m_iter << " " << error << " " << m_best[0]->get_variable()[0] << " " << m_best[0]->get_variable()[1] << std::endl;
			std::cout << m_iter << " " << CONTINOUS_CAST->total_evaluations() << " " << num_opt_found << std::endl;
			measure::ms_measure->record(global::ms_global.get(), m_iter, num_opt_found);
			m_mutexStream.unlock();

			tag = evolve();
		}
		measure::ms_measure->record(global::ms_global.get(), m_iter, CONTINOUS_CAST->num_optima_found());
		std::cout << m_iter << " " << CONTINOUS_CAST->total_evaluations() << " " << CONTINOUS_CAST->num_optima_found() << std::endl;
		std::cout << CONTINOUS_CAST->get_optima().number_variable() << std::endl;
		return tag;
	}
}
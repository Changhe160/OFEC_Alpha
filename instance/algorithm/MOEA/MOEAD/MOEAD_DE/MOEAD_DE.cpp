#include "MOEAD_DE.h"

namespace OFEC{
	MOEAD_DE_pop::MOEAD_DE_pop(size_t size_pop):DE::MOEA_DE_pop<>(size_pop){ }

	void MOEAD_DE_pop::initialize() {
		MOEA_DE_pop::initialize();
		MOEAD::initialize(this->m_inds);
	}

	evaluation_tag MOEAD_DE_pop::evolve() {
		evaluation_tag tag = evaluation_tag::Normal;
		tag = MOEAD::evolve();
		this->m_iter++;
		return tag;
	}

	evaluation_tag MOEAD_DE_pop::evolve_mo() {
		evaluation_tag tag = evaluation_tag::Normal;
		std::vector<int> perm(this->m_inds.size());
		for (int i(0); i < perm.size(); ++i) {
			perm[i] = i;
		}
		global::ms_global->m_uniform[caller::Algorithm]->shuffle(perm.begin(), perm.end());
		for (int i = 0; i < this->m_inds.size(); i++){
			int n = perm[i];
			// or int n = i;
			int type;
			double rnd = global::ms_global->m_uniform[caller::Algorithm]->next();
			// mating selection based on probability
			if (rnd < m_realb)    
				type = 1;   // neighborhood
			else             
				type = 2;   // whole population

			// select the indexes of mating parents
			std::vector<int> p;
			matingselection(p, n, 2, type, this->m_inds.size());  // neighborhood selection

			// produce a child Individual
			DE::individual child;
			std::vector<size_t> index(3);
			index[0] = n; index[1] = p[0]; index[2] = p[1];
			this->cross_mutate(index, child);
			tag = child.evaluate();
			if (tag != evaluation_tag::Normal) break;
			// update the reference points and other TypeIndivs in the neighborhood or the whole population
			update_reference(child);
			update_problem(this->m_inds, child, n, type);
		}
		return tag;
	}


	MOEAD_DE::MOEAD_DE(param_map &v) :algorithm(v.at("algName")), m_pop(v.at("popSize")) { }

	void MOEAD_DE::initialize() {
		m_pop.initialize();
	}

	void MOEAD_DE::record() {
		size_t evals = CONTINUOUS_CAST->evaluations();
		real IGD = CONTINUOUS_CAST->get_optima().IGD_to_PF(m_pop);
		measure::get_measure()->record(global::ms_global.get(), evals, IGD);
	}

	void MOEAD_DE::run_() {
		while (!terminating()) {
			m_pop.evolve();			
		}
	}
}

#include "MOEAD_SBX.h"

namespace OFEC {
	MOEAD_SBX_pop::MOEAD_SBX_pop(size_t size_pop) :SBX_pop<>(size_pop) { }

	void MOEAD_SBX_pop::initialize() {
		MOEAD_SBX_pop::initialize();
		MOEAD::initialize(this->m_inds);
	}

	evaluation_tag MOEAD_SBX_pop::evolve() {
		evaluation_tag tag = evaluation_tag::Normal;
		tag = MOEAD::evolve();
		this->m_iter++;
		return tag;
	}

	evaluation_tag MOEAD_SBX_pop::evolve_mo() {
		evaluation_tag tag = evaluation_tag::Normal;
		std::vector<int> perm(this->m_inds.size());
		for (int i(0); i < perm.size(); ++i) {
			perm[i] = i;
		}
		global::ms_global->m_uniform[caller::Algorithm]->shuffle(perm.begin(), perm.end());
		for (int i = 0; i < this->m_inds.size(); i += 2) {
			int n = perm[i];
			// or int n = i;
			int type = 1; //only select parent individual from neighborhood
			double rnd = global::ms_global->m_uniform[caller::Algorithm]->next();

			// select the indexes of mating parents
			std::vector<int> p;
			matingselection(p, n, 1, type, this->m_inds.size());  // neighborhood selection

			// produce a child Individual
			individual<> child1, child2;
			std::vector<size_t> index(2);
			index[0] = n; index[1] = p[0];
			this->crossover(index[0], index[1], child1, child2);
			mutate(child1);
			mutate(child2);
			tag = child1.evaluate();
			if (tag != evaluation_tag::Normal) break;
			tag = child2.evaluate();
			if (tag != evaluation_tag::Normal) break;
			// update the reference points and other TypeIndivs in the neighborhood or the whole population
			update_reference(child1);
			update_reference(child2);
			update_problem(this->m_inds, child1, n, type);
			update_problem(this->m_inds, child2, n, type);
		}
		return tag;
	}


	MOEAD_SBX::MOEAD_SBX(param_map &v) :algorithm(v.at("algName")), m_pop(v.at("popSize")) { }

	void MOEAD_SBX::initialize() {
		m_pop.initialize();
	}

	void MOEAD_SBX::record() {
		size_t evals = CONTINUOUS_CAST->evaluations();
		real IGD = CONTINUOUS_CAST->get_optima().IGD_to_PF(m_pop);
		measure::get_measure()->record(global::ms_global.get(), evals, IGD);
	}

	void MOEAD_SBX::run_() {
		while (!terminating()) {
			m_pop.evolve();
		}
	}
}


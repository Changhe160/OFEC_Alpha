#include "spso11.h"

//#ifdef OFEC_PLAYBACK
//#include <buffer/datum_inclusion.h>
//#endif // OFEC_PLAYBACK
//
//#ifdef OFEC_STATISTICS
//#include <record/datum_inclusion.h>
//#endif // OFEC_STATISTICS
#include "../../../../../../datum/datum_inclusion.h"

namespace ofec {
	void SPSO11::addInputParameters() {
		m_input_parameters.add("population size", new RangedSizeT(m_pop_size, 5, 10000, 20));
		m_input_parameters.add("weight", new RangedReal(m_weight, 0, 2, 0.721));
		m_input_parameters.add("accelerator1", new RangedReal(m_accelerator1, 0, 3, 1.193));
		m_input_parameters.add("accelerator2", new RangedReal(m_accelerator2, 0, 3, 1.193));
	}

	void SPSO11::initialize_(Environment *env) {
		Algorithm::initialize_(env);
		m_pop.reset(new Swarm<Particle11>(m_pop_size, env));
		m_pop->weight() = m_weight;
		m_pop->accelerator1() = m_accelerator1;
		m_pop->accelerator2() = m_accelerator2;
	}

	void SPSO11::run_(Environment *env) {
#ifdef OFEC_DATUM_OFFSPRING_H
		g_offspring.sols.clear();
		for (size_t i = 0; i < m_pop->size(); i++) {
			g_offspring.sols.push_back(&m_pop->at(i));
		}
#endif // OFEC_DATUM_OFFSPRING_H

		m_pop->initialize(env, m_random.get());
		m_pop->initVelocity(env, m_random.get());
		m_pop->evaluate(env);
		m_pop->initPbest(env);

#ifdef OFEC_DATUM_MULTI_POP_H
		g_multi_pop.pops.clear();
		g_multi_pop.pops.resize(1);
		for (size_t i = 0; i < m_pop->size(); ++i) {
			g_multi_pop.pops[0].push_back(&m_pop->at(i).pbest());
		}
		datumUpdated(env, g_multi_pop);
#endif
#ifdef OFEC_DATUM_OFFSPRING_H
		datumUpdated(env, g_offspring);
#endif // OFEC_DATUM_OFFSPRING_H
		while (!terminating()) {
			m_pop->evolve(env, m_random.get());
#ifdef OFEC_DATUM_OFFSPRING_H
			datumUpdated(env, g_offspring);
#endif // OFEC_DATUM_OFFSPRING_H
#ifdef OFEC_DATUM_MULTI_POP_H
			g_multi_pop.pops.clear();
			g_multi_pop.pops.resize(1);
			for (size_t i = 0; i < m_pop->size(); ++i) {
				g_multi_pop.pops[0].push_back(&m_pop->at(i).pbest());
			}
			datumUpdated(env, g_multi_pop);
#endif
		}
	}
}
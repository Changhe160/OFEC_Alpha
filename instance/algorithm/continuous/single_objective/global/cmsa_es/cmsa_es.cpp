#include "cmsa_es.h"
#include "cmsa_es_pop.h"

#ifdef OFEC_PLAYBACK
#include <buffer/datum_inclusion.h>
#endif // OFEC_PLAYBACK

#ifdef OFEC_STATISTICS
#include <record/datum_inclusion.h>
#endif // OFEC_STATISTICS

#include "../../../../../../datum/datum_inclusion.h"


namespace ofec {
	void CMSA_ES::addInputParameters() {
		m_input_parameters.add("population size", new RangedSizeT(m_pop_size, 2, 1000, 5));
	}

	void CMSA_ES::initialize_(Environment *env) {
		Algorithm::initialize_(env);
	}

	void CMSA_ES::run_(Environment *env) {
		PopCMSA_ES pop(m_pop_size, env);

		pop.initialize(env, m_random.get());
		pop.sampleNewPopulation(env, m_random.get());

#ifdef OFEC_DATUM_MULTI_POP_H
		g_multi_pop.pops.clear();
		g_multi_pop.pops.resize(1);
		for (size_t i = 0; i < m_pop_size; ++i) {
			g_multi_pop.pops[0].push_back(&pop[i]);
		}
		datumUpdated(env, g_multi_pop);
#endif


		while (!terminating()) {
			pop.evolve(env, m_random.get());

#ifdef OFEC_DATUM_MULTI_POP_H
			g_multi_pop.pops.clear();
			g_multi_pop.pops.resize(1);
			for (size_t i = 0; i < m_pop_size; ++i) {
				g_multi_pop.pops[0].push_back(&pop[i]);
			}
			datumUpdated(env, g_multi_pop);
#endif
		}
	}
}

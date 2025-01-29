#include "hill_vallea.h"
#include <numeric>
#include "../../../../../../core/problem/continuous/continuous.h"
#include "../../global/cmsa_es/cmsa_es_pop.h"
#include "../../../../../../utility/functional.h"

#ifdef OFEC_PLAYBACK
#include <buffer/datum_inclusion.h>
#endif // OFEC_PLAYBACK

#ifdef OFEC_STATISTICS
#include <record/datum_inclusion.h>
#endif // OFEC_STATISTICS

#include "../../../../../../datum/datum_inclusion.h"

namespace ofec {
	void HillVallEA::initialize_(Environment *env) {
		Algorithm::initialize_(env);
		m_E.clear();
		size_t d = env->problem()->numberVariables();
		m_N = 16 * d;
		m_N_c = 3 * sqrt(Real(d));
		m_tau = 0.5;
		m_TOL = 1e-5;
		m_restartTime = 0;
	}

	void HillVallEA::run_(Environment *env) {
		while (!terminating()) {

			// First phase - locating niches
			//if (m_maximum_evaluations > 0) {
			//	if (m_maximum_evaluations - m_evaluations < m_N) {
			//		break;
			//	}
			//}
			auto P = uniformlySample(m_N, env, m_random.get());
#ifdef OFEC_DATUM_NUM_SAMPLES_H
			g_num_samples.value = m_N;
			datumUpdated(env, g_num_samples);
#endif // OFEC_DATUM_NUM_SAMPLES_H
			if (terminating()) {
				break;
			}
			for (auto &e : m_E) { P.push_back(e); }
			auto S = truncationSelection(P, m_tau, env);
			auto K = clustering(S, env);
#ifdef OFEC_DATUM_MULTI_POP_H
			g_multi_pop.pops.clear();
			g_multi_pop.pops.resize(K.size());
			for (size_t k = 0; k < K.size(); k++) {
				for (size_t i = 0; i < K[k].size(); ++i) {
					g_multi_pop.pops[k].push_back(K[k][i].get());
				}
			}
			datumUpdated(env, g_multi_pop);
#endif

#ifdef OFEC_DATUM_HILL_VALLEA_POPINFO_H
			g_hillvallue_popinfo.m_popId = 0;
			g_hillvallue_popinfo.m_restart_id = m_restartTime;
			datumUpdated(env, g_hillvallue_popinfo);
#endif // OFEC_DATUM_HILL_VALLEA_POPINFO_H






			// Second phase - niche optimization
			bool elite_added = false;
			for (int idpop(0); idpop < K.size(); ++idpop) {
				auto& C = K[idpop];
				m_curpopid = idpop;
				if (duplicate(*best(C, env), m_E, env)) {
					continue;
				}
				auto x = coreSearch(C, m_N_c, env, m_random.get());
				if (terminating()) {
					break;
				}
				if (distinct(*x, m_E, env)) {
					if (betterThanAll(*x, m_E, m_TOL, env)) {
						m_E.clear();
					}
					m_E.push_back(x);
					elite_added = true;
				}
			}
			if (!elite_added) {
				m_N *= 2;
				m_N_c *= 1.2;
			}

			++m_restartTime;
		}
	}

	std::vector<std::shared_ptr<Solution<>>> HillVallEA::uniformlySample(
		size_t num, Environment *env, Random *rnd
	) {
		std::vector<std::shared_ptr<Solution<>>> sols;
		for (size_t i = 0; i < num && !terminating(); i++) {
			auto new_sol = dynamic_cast<Solution<>*>(env->problem()->createSolution());
			new_sol->initialize(env, rnd);
			new_sol->evaluate(env);
			sols.emplace_back(new_sol);
		}
		return sols;
	}

	std::vector<std::shared_ptr<Solution<>>> HillVallEA::truncationSelection(
		const std::vector<std::shared_ptr<Solution<>>> &sols, Real tau, Environment *env
	) {
		std::vector<std::shared_ptr<Solution<>>> sels;
		std::vector<size_t> seq(sols.size());
		std::iota(seq.begin(), seq.end(), 0);
		std::nth_element(seq.begin(), seq.begin() + tau * seq.size(), seq.end(),
			[&sols, env](size_t i, size_t j) { return dominate(*sols[i], *sols[j], env->problem()->optimizeMode()); });
		for (auto it = seq.begin(); it != seq.begin() + tau * seq.size(); it++) {
			sels.push_back(sols[*it]);
		}
		return sels;
	}

	std::vector<std::vector<std::shared_ptr<Solution<>>>> HillVallEA::clustering(
		const std::vector<std::shared_ptr<Solution<>>> &sols, Environment *env
	) {
		std::vector<std::shared_ptr<Solution<>>> sort_sols(sols); // sorted S
		std::sort(sort_sols.begin(), sort_sols.end(),
			[&sort_sols, env](std::shared_ptr<Solution<>> s1, std::shared_ptr<Solution<>> s2) {
				return dominate(*s1, *s2, env->problem()->optimizeMode());
			});
		size_t d = env->problem()->numberVariables();
		Real V_X = CAST_CONOP(env->problem())->domainVolume();
		Real EEL = pow(V_X / sort_sols.size(), 1.0 / d);
		std::vector<int> id_clu(sort_sols.size(), -1);
		id_clu[0] = 0;
		size_t num_clus = 1;
		for (size_t i = 1; i < sort_sols.size(); i++) {
			std::vector<Real> delta(i);
			for (size_t j = 0; j < i; j++) {
				delta[j] = sort_sols[i]->variableDistance(*sort_sols[j], env);
			}
			std::vector<size_t> seq(i);
			std::iota(seq.begin(), seq.end(), 0);
			std::vector<bool> clu_checked(num_clus, false);
			for (size_t j = 0; j < std::min(i, d + 1); j++) {
				std::nth_element(seq.begin(), seq.begin() + j, seq.end(),
					[&delta](size_t ii, size_t jj) {return delta[ii] < delta[jj]; });
				size_t k = *(seq.begin() + j);
				size_t N_t = 1 + floor(delta[k] / EEL);
				if (!clu_checked[id_clu[k]]) {
					if (test(*sort_sols[k], *sort_sols[i], N_t, env)) {
						id_clu[i] = id_clu[k];
						break;
					}
					clu_checked[id_clu[k]] = true;
				}
			}
			if (id_clu[i] == -1) {
				id_clu[i] = num_clus++;
			}
		}
		std::vector<std::vector<std::shared_ptr<Solution<>>>> clusters(num_clus);
		for (size_t i = 0; i < sort_sols.size(); i++) {
			clusters[id_clu[i]].push_back(sort_sols[i]);
		}
		return clusters;
	}

	bool HillVallEA::test(
		const Solution<> &s1, const Solution<> &s2, size_t num, Environment *env
	) {
		Vector x_left(s1.variable().vector()), x_right(s2.variable().vector());
		Solution<> m(s1);
		for (Real k = 1; k <= num; k++) {
			auto x_test = x_right + k * (x_left - x_right) / (num + 1);
			m.variable().vector() = x_test.vect();
			m.evaluate(env);
			if (dominate(s1, m, env->problem()->optimizeMode()) && dominate(s2, m, env->problem()->optimizeMode())) {
				return false;
			}
		}
		return true;
	}

	std::shared_ptr<Solution<>> HillVallEA::best(
		const std::vector<std::shared_ptr<Solution<>>> &sols, Environment *env
	) {
		std::shared_ptr<Solution<>> best_sol;
		for (auto &s : sols) {
			if (!best_sol || dominate(*s, *best_sol, env->problem()->optimizeMode())) {
				best_sol = s;
			}
		}
		return best_sol;
	}

	bool HillVallEA::duplicate(const Solution<> &s, 
		const std::list<std::shared_ptr<Solution<>>> &E, Environment *env
	) {
		for (auto &e : E) {
			if (s.same(*e, env)) {
				return true;
			}
		}
		return false;
	}

	std::shared_ptr<Solution<>> HillVallEA::coreSearch(
		const std::vector<std::shared_ptr<Solution<>>> &sols, 
		size_t pop_size, Environment *env, Random *rnd
	) {
		PopCMSA_ES pop(pop_size, env);
#ifdef OFEC_DATUM_MULTI_POP_H
		g_multi_pop.pops.clear();
		g_multi_pop.pops.resize(1);
		for (size_t i = 0; i < pop_size; ++i) {
			g_multi_pop.pops[0].push_back(&pop[i]);
		}
#endif


		pop.initializeBySamples(sols, env);
		pop.sampleNewPopulation(env, rnd);




#ifdef OFEC_DATUM_MULTI_POP_H
		datumUpdated(env, g_multi_pop);
#endif

#ifdef OFEC_DATUM_HILL_VALLEA_POPINFO_H
		g_hillvallue_popinfo.m_popId = m_curpopid;
		g_hillvallue_popinfo.m_restart_id = m_restartTime;
		datumUpdated(env, g_hillvallue_popinfo);
#endif // OFEC_DATUM_HILL_VALLEA_POPINFO_H
		std::list<Real> boei;	// best objective of each iteration
		size_t stallsize = 10 + floor(30.0 * env->problem()->numberVariables() / pop_size);
		size_t id_best = 0;
		while (!terminating()) {
			pop.evolve(env, rnd);
#ifdef OFEC_DATUM_MULTI_POP_H
			datumUpdated(env, g_multi_pop);
#endif

#ifdef OFEC_DATUM_HILL_VALLEA_POPINFO_H
			g_hillvallue_popinfo.m_popId = m_curpopid;
			g_hillvallue_popinfo.m_restart_id = m_restartTime;
			datumUpdated(env, g_hillvallue_popinfo);
#endif // OFEC_DATUM_HILL_VALLEA_POPINFO_H
			id_best = 0;
			for (size_t i = 1; i < pop.size(); ++i) {
				if (dominate(pop[i], pop[id_best], env->problem()->optimizeMode())) {
					id_best = i;
				}
			}
			boei.push_back(pop[id_best].objective(0));
			while (boei.size() > stallsize) {
				boei.pop_front();
			}
			if (boei.size() == stallsize &&
				*std::max_element(boei.begin(), boei.end()) - 
				*std::min_element(boei.begin(), boei.end()) < m_TOL)
			{
				break;
			}
		}
		return std::make_shared<Solution<>>(pop[id_best]);
	}

	bool HillVallEA::distinct(const Solution<> &s, 
		const std::list<std::shared_ptr<Solution<>>> &E, Environment *env
	) {
		for (auto &e : E) {
			if (test(*e, s, 5, env)) {
				return false;
			}
		}
		return true;
	}

	bool HillVallEA::betterThanAll(const Solution<> &s, 
		const std::list<std::shared_ptr<Solution<>>> &E, Real TOL, Environment *env
	) {
		for (auto &e : E) {
			if (!dominate(s, *e, env->problem()->optimizeMode()) || s.objectiveDistance(*e) < TOL) {
				return false;
			}
		}
		return true;
	}
}

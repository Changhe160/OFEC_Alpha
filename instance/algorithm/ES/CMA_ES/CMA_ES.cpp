#include "CMA_ES.h"
#include "cmaes_interface.h"
#include "../../../../core/problem/continuous/continuous.h"


namespace OFEC {
	CMA_ES::CMA_ES(param_map & v) : algorithm(v.at("algName")), m_inds(v.at("popSize")) {
		if (CONTINUOUS_CAST->has_tag(problem_tag::GOP))
			CONTINUOUS_CAST->set_eval_monitor_flag(true);
	}

	void CMA_ES::initialize() {
		size_pop = m_inds.size();
		size_var = CONTINUOUS_CAST->variable_size();
		for (auto& ind : m_inds) {
			ind.variable().resize(size_var);
			ind.resize_objective(1);
		}

		initialsFilePathName = (std::string)global::ms_arg.at("workingDir") + "instance/algorithm/ES/CMA_ES/initials.par";
		signalsFilePathName = (std::string)global::ms_arg.at("workingDir") + "instance/algorithm/ES/CMA_ES//signals.par";
		/* Initialize everything into the struct evo, 0 means default */
		arFunvals = cmaes_init(&m_evo, size_var, NULL, NULL, 0, size_pop, initialsFilePathName.c_str()); /* "cmaes_initials.par"*/
		/* printf("%s\n", cmaes_SayHello(&evo)); */
		cmaes_ReadSignals(&m_evo, signalsFilePathName.c_str());  /* "cmaes_signals.par" write header and initial values */
	}

	void CMA_ES::run_() {
		auto& init_range = CONTINUOUS_CAST->init_range();
		auto& range = CONTINUOUS_CAST->range();
		/* Iterate until stop criterion holds */
		while (!terminating()) {
			/* generate lambda new search points, sample population */
			pop = cmaes_SamplePopulation(&m_evo); /* do not change content of pop */
			for (size_t i = 0; i < size_pop; i++) {
				for (size_t j = 0; j < size_var; ++j) {
					if (init_range[j].limited)
						m_inds[i].variable()[j] = init_range[j].limit.first + (real)pop[i][j] * (init_range[j].limit.second - init_range[j].limit.first);
					else
						m_inds[i].variable()[j] = range[j].limit.first + (real)pop[i][j] * (range[j].limit.second - range[j].limit.first);
				}
			}

			/* resample the solution i until it satisfies given	box constraints (variable boundaries) */
			for (size_t i = 0; i < cmaes_Get(&m_evo, "popsize"); ++i) {
				int count = 0;
				while (CONTINUOUS_CAST->check_boundary_violation(m_inds[i]) != violation_type::None && ++count < size_pop)
					cmaes_ReSampleSingle(&m_evo, i);
				if (count < size_pop)
					for (size_t j = 0; j < size_var; ++j)
						if (init_range[j].limited)
							m_inds[i].variable()[j] = init_range[j].limit.first + (real)pop[i][j] * (init_range[j].limit.second - init_range[j].limit.first);
						else
							m_inds[i].variable()[j] = range[j].limit.first + (real)pop[i][j] * (range[j].limit.second - range[j].limit.first);
			}

			/* evalutate the solutions */
			for (size_t i = 0; i < size_pop; i++) {
				m_inds[i].evaluate();
				if (CONTINUOUS_CAST->opt_mode(0) == optimization_mode::Minimization)
					arFunvals[i] = m_inds[i].objective()[0];
				else
					arFunvals[i] = -m_inds[i].objective()[0];
			}
			/* update the search distribution used for cmaes_SamplePopulation() */
			cmaes_UpdateDistribution(&m_evo, arFunvals);

			/* read instructions for printing output or changing termination conditions */
			cmaes_ReadSignals(&m_evo, signalsFilePathName.c_str());//"cmaes_signals.par"
			fflush(stdout); /* useful in MinGW */

		}
		if (measure::get_measure()) record();
		/* get best estimator for the optimum, xmean */
		xfinal = cmaes_GetNew(&m_evo, "xmean"); /* "xbestever" might be used as well */
		cmaes_exit(&m_evo); /* release memory */

		/* do something with final solution and finally release memory */
		free(xfinal);
	}

	void CMA_ES::record() {
		if (CONTINUOUS_CAST->has_tag(problem_tag::MMOP)) {
			// ******* Multi-Modal Optimization *******
			size_t evals = CONTINUOUS_CAST->evaluations();
			size_t num_opt_found = CONTINUOUS_CAST->num_optima_found();
			size_t num_opt_known = CONTINUOUS_CAST->get_optima().number_objective();
			real peak_ratio = (real)num_opt_found / (real)num_opt_known;
			real success_rate = CONTINUOUS_CAST->solved() ? 1 : 0;
			measure::get_measure()->record(global::ms_global.get(), evals, peak_ratio, success_rate);
		}
		else if (CONTINUOUS_CAST->has_tag(problem_tag::GOP)) {
			// ******* Global Optimization ************
			size_t evals = CONTINUOUS_CAST->evaluations();
			real err = std::fabs(problem::get_sofar_best<solution<>>(0)->objective(0) - CONTINUOUS_CAST->get_optima().objective(0).at(0));
			measure::get_measure()->record(global::ms_global.get(), evals, err);
		}
	}
}
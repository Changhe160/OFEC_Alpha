#include "COGL.h"

namespace OFEC {
	COGL::COGL(param_map & v) : population(v[param_popSize], v[param_numDim]), multi_population(v[param_numSubPop], v[param_subPopSize]), m_tar(1.2)
	{
		v[param_numDim] = 1;
	}
	evaluation_tag COGL::run_() {
		evaluation_tag tag = evaluation_tag::Normal;
		int count = 1;
		while (tag != evaluation_tag::Terminate) {
			double best = problem::get_sofar_best<solution<variable_epanet>>(0)->get_objective()[0];
			int num_alternative_solution = 1;
			std::cout << m_iter << " " << CAST_RP_EPANET->phase() << " " << best << std::endl;
			measure::ms_measure->record(global::ms_global.get(), m_iter, best);
			if (CAST_RP_EPANET->phase() == 0)
				continue;
			tag = evolve();

			if (m_iter % 30 == 0) {
				CAST_RP_EPANET->set_phase(++count);
				if (CAST_RP_EPANET->is_time_out()) break;
			}

		}
		solution<variable_epanet>* best_var = problem::get_sofar_best<solution<variable_epanet>>(0);
		std::cout << "best solution: " << std::endl;
		std::cout << "location: " << best_var->get_variable().location() << std::endl;
		std::cout << "start time: " << best_var->get_variable().start_time() << std::endl;
		std::cout << "duration: " << best_var->get_variable().duration() << std::endl;
		std::cout << "multiplier: " << std::endl;
		for(auto &i: best_var->get_variable().multiplier())
			std::cout << i << std::endl;
		return tag;
	}
	evaluation_tag COGL::evolve() {
		evaluation_tag tag = evaluation_tag::Normal;
		int count = 0;
		for (size_t i = 0; i < m_sub.size(); ++i) {
			tag = m_sub[i]->evaluate();
			if (i>0 && is_feasible_population((*m_sub[i])))  
			for (size_t j = 0; j < (*m_sub[i]).size(); ++j) {
				
				
			}
		}
		++m_iter;
		if (m_iter == 5) return evaluation_tag::Terminate;
		return tag;
	}
	bool COGL::is_feasible_population(const population<individual<variable_epanet,real>> & subpopulation) {
		size_t num = CAST_RP_EPANET->phase()*CAST_RP_EPANET->interval();
		float temp = 0;
		for (size_t i = 0; i < num; ++i) {
			for (int j = 0; j < CAST_RP_EPANET->num_sensor(); ++j) {
			//	temp += pow(CAST_RP_EPANET->observation_concentration[j][i], 2);
			}
		}
		float benchmark = m_tar * sqrt(temp / (CAST_RP_EPANET->num_sensor()*num));

		size_t count_feasible = 0, count_infeasible = 0;
		for (size_t i = 0; i < subpopulation.size(); ++i) {
			if (subpopulation[i].get_objective()[0] <= benchmark) ++count_feasible;
			else ++count_infeasible;
		}
		return count_feasible >= count_infeasible;
	}
}
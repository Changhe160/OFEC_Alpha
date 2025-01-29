#include "nbn_grid_tree_division_mmop.h"
#include <vector>
#include "../../../core/problem/continuous/continuous.h"
#include "../../../utility/nondominated_sorting/fast_sort.h"
#include "../../../utility/nondominated_sorting/filter_sort.h"

namespace ofec {
	void NBN_GridTreeDivisionMMOP::generateSols()
	{
		std::vector<int> dim_div(m_dim, 0);
		int numSample_int = 1;
		for (auto& it : dim_div) {
			it = m_dimemsion_division;
			numSample_int *= it;
		}
	
		int fromSolId = m_sols.size();
		assignedSols(numSample_int);

		auto boundary = CAST_CONOP(m_environment->problem())->boundary();
		std::vector<double> range_div(m_dim,0);
		std::vector<double> range_from(m_dim,0);
		for (int idDim(0); idDim < m_dim; ++idDim) {
			auto& range_var = boundary[idDim];
			range_div[idDim] = (range_var.second - range_var.first) / double(dim_div[idDim]);
			range_from[idDim] = range_var.first;
		}

		
		std::vector<double> startPos(m_dim, 0);
		for (int idDim(0); idDim < m_dim; ++idDim) {
			startPos[idDim] = boundary[idDim].first;
		}
		

		auto pro = m_environment->problem();

		std::vector<int> curVec(m_dim);
		for (int nodeId(0); nodeId < numSample_int; ++nodeId) {
			auto& cur_sol(m_sols[nodeId+ fromSolId]);
			cur_sol.reset(new Solution<>(
				pro->numberObjectives(),
				pro->numberConstraints(),
				pro->numberVariables()));
			//	cur_sol->setId(solIdx);
			auto& curConSol(dynamic_cast<Solution<>&>(*cur_sol));

			idxToVec(nodeId, curVec, dim_div);

			for (int idDim(0); idDim < m_dim; ++idDim) {
				curConSol.variable()[idDim] = startPos[idDim] + curVec[idDim] * range_div[idDim] + range_div[idDim] * m_random->uniform.next();
			}
		}
	}
	void NBN_GridTreeDivisionMMOP::updateFitness()
	{
		auto pro = m_environment->problem();
		for (auto& it : m_sols) {
			it->evaluate(m_environment.get(),false);
		}
		
		std::vector<std::vector<Real>*> objs;
		for (size_t i = 0; i < m_sols.size(); ++i) {
			objs.emplace_back(&m_sols[i]->objective());
		}
		std::vector<int> rank;
		ofec::nd_sort::filterSortP<Real>(objs, rank, pro->optimizeMode());
		for (int idx(0); idx < m_sols.size(); ++idx) {
			m_fitness[idx] = -rank[idx];
			m_sols[idx]->setFitness(m_fitness[idx]);
			
		}
		
		
	}
}
#include "metrics_gop.h"
#include "../../../utility/functional.h"

namespace ofec {
	void MetricsGOP::updateCandidates(const SolutionBase &sol, std::list<std::unique_ptr<SolutionBase>> &candidates) const {
		if (candidates.empty()) {
			candidates.emplace_back(createSolution(sol));
		}
		else if (dominate(sol, *candidates.front(), m_optimize_mode)) {
			candidates.front().reset(createSolution(sol));
		}
	}
	
	bool MetricsGOP::isSolved(const std::list<std::unique_ptr<SolutionBase>> &candidates) const {
		if (!candidates.empty() &&
			candidates.front()->objectiveDistance(m_optima->objective(0)) < m_objective_accuracy
			) {
			return true;
		}
		else {
			return false;
		}
	}

	Real MetricsGOP::error(const std::list<std::unique_ptr<SolutionBase>> &candidates) const {
		return candidates.front()->objectiveDistance(m_optima->objective(0));
	}
}
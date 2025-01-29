#ifndef OFEC_METRICS_GOP_H
#define OFEC_METRICS_GOP_H

#include "../../../core/problem/problem.h"

namespace ofec{
	class MetricsGOP : virtual public Problem {
	protected:
		Real m_objective_accuracy;
	public:
		Real objectiveAccuracy() const { return m_objective_accuracy; }
		void updateCandidates(const SolutionBase &s, std::list<std::unique_ptr<SolutionBase>> &candidates) const;
		bool isSolved(const std::list<std::unique_ptr<SolutionBase>> &candidates) const;
		Real error(const std::list<std::unique_ptr<SolutionBase>> &candidates) const;
	};
}

#endif // !OFEC_METRICS_GOP_H

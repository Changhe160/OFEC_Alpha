#include "functional.h"

namespace ofec {
	bool dominate(const SolutionBase &a, const SolutionBase &b, const std::vector<OptimizeMode> &opt_mode) {
		return Dominance::kDominant == objectiveCompare(a.objective(), b.objective(), opt_mode);
	}
}

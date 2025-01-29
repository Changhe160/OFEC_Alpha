#include "problem.h"
#include "../environment/environment.h"

#ifdef OFEC_PLAYBACK
#include <playback/global.h>
#endif

namespace ofec {
	void Problem::reset() {
		resizeObjective(0);
		resizeConstraint(0);
		m_optima.reset();
	}

	void Problem::initialize(Environment *env) {
		m_initialized = false;
		initialize_(env);
		updateOptima(env);
		initializeAfter_(env);
		m_initialized = true;
	}

	void Problem::solutionToParameterVariants(
		const SolutionBase& sol, ParameterVariantStream& out
	) const {
		out << sol.objective() << sol.constraint()
			<< sol.timeEvaluate() << sol.fitness() << sol.type();
	}

	void Problem::parameterVariantsToSolution(
		ParameterVariantStream& in, SolutionBase& sol
	) const {
		in >> sol.objective() >> sol.constraint()
			>> sol.timeEvaluate() >> sol.fitness() >> sol.type();
	}
	
	void Problem::resizeObjective(size_t number_objectives) {
		m_number_objectives = number_objectives;
		m_optimize_mode.resize(number_objectives);
	}

	void Problem::resizeConstraint(size_t num_cons) {
		m_number_constraints = num_cons;
		m_constraint_type.resize(m_number_constraints);
	}

	bool Problem::isOptimumSolutionGiven() const {
		return m_optima && m_optima->isSolutionGiven();
	}

	size_t Problem::numberOptimumSolutions() const {
		return m_optima ? m_optima->numberSolutions() : 0;
	}

	bool Problem::isOptimumObjectiveGiven() const {
		return m_optima && m_optima->isObjectiveGiven();
	}

	size_t Problem::numberOptimumObjectives() const {
		return m_optima ? m_optima->numberObjectives() : 0;
	}
	
	Dominance Problem::objectiveCompare(const SolutionBase& sola, const SolutionBase& solb)const {
		auto& a = sola.objective();
		auto& b = solb.objective();
		const auto& mode = m_optimize_mode;
		if (a.size() < mode.size() || b.size() < mode.size())
			return Dominance::kNonComparable;

		int better = 0, worse = 0, equal = 0;
		for (size_t i = 0; i < mode.size(); ++i) {
			if (mode[i] == OptimizeMode::kMinimize) {
				if (a[i] < b[i]) {
					if (worse > 0)
						return Dominance::kNonDominated;
					else
						++better;
				}
				else if (a[i] > b[i]) {
					if (better > 0)
						return Dominance::kNonDominated;
					else
						++worse;
				}
				else {
					++equal;
				}
			}
			else {
				if (a[i] > b[i]) {
					if (worse > 0)
						return Dominance::kNonDominated;
					else
						++better;
				}
				else if (a[i] < b[i]) {
					if (better > 0)
						return Dominance::kNonDominated;
					else
						++worse;
				}
				else {
					++equal;
				}
			}
		}
		if (equal == mode.size()) return Dominance::kEqual;
		else if (worse == 0) return Dominance::kDominant;
		else if (better == 0) return Dominance::kDominated;
		else return Dominance::kNonDominated;
	}

}
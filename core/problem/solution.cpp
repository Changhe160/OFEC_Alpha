#include "solution.h"
#include "../environment/environment.h"

#ifdef OFEC_STATISTICS
#include <statistics/global.h>
#endif // OFEC_STATISTICS

#ifdef OFEC_PLAYBACK
#include <playback/global.h>
#endif // OFEC_PLAYBACK

namespace ofec {
	SolutionBase::SolutionBase(size_t num_objs, size_t num_cons) :
		m_variables(nullptr),
		m_objectives(num_objs),
		m_constraints(num_cons),
		m_time_evaluate(0), m_fitness(0) {}

	SolutionBase::SolutionBase(const SolutionBase &sol) :
		m_variables(nullptr),
		m_objectives(sol.m_objectives), 
		m_constraints(sol.m_constraints),
		m_time_evaluate(sol.m_time_evaluate), m_fitness(sol.m_fitness){}

	SolutionBase::SolutionBase(SolutionBase &&sol) noexcept :
		m_variables(sol.m_variables.release()),
		m_objectives(std::move(sol.m_objectives)),
		m_constraints(std::move(sol.m_constraints)),
		m_time_evaluate(sol.m_time_evaluate),m_fitness(sol.m_fitness){}

	SolutionBase& SolutionBase::operator=(const SolutionBase &sol) {
		if (this != &sol) {
			m_objectives = sol.m_objectives;
			m_constraints = sol.m_constraints;
			m_time_evaluate = sol.m_time_evaluate;
			m_fitness = sol.m_fitness;
			m_type = sol.m_type;
		}
		return *this;
	}

	SolutionBase& SolutionBase::operator=(SolutionBase &&sol) noexcept {
		if (this != &sol) {
			m_variables.reset(sol.m_variables.release());
			m_objectives = std::move(sol.m_objectives);
			m_constraints = std::move(sol.m_constraints);
			m_time_evaluate = sol.m_time_evaluate;
			m_fitness = sol.m_fitness;
			m_type = sol.m_type;
		}
		return *this;
	}

	void SolutionBase::initialize(Environment *env, Random *rnd) {
		env->problem()->initializeVariables(*m_variables, rnd);
	}

	bool SolutionBase::same(const SolutionBase& x, const Environment* env) const {
		return m_objectives == x.m_objectives
			&& m_constraints == x.m_constraints
			&& m_time_evaluate == x.m_time_evaluate
			&& m_fitness == x.m_fitness
			&& m_type == x.m_type
			&& env->problem()->same(*m_variables, x.variableBase());
	}

	bool SolutionBase::variableSame(const SolutionBase &x,const Environment *env) const {
		return env->problem()->same(*m_variables, x.variableBase());
	}

	Real SolutionBase::variableDistance(const SolutionBase &x, const Environment *env) const {
		return env->problem()->variableDistance(*m_variables, x.variableBase());
	}

	Real SolutionBase::variableDistance(const VariableBase &x, const Environment *env) const {
		return env->problem()->variableDistance(*m_variables, x);
	}

	Real SolutionBase::objectiveDistance(const std::vector<Real> &objs) const {
		auto& x1_ = m_objectives;
		auto& x2_ = objs;
		Real sum_pow = 0;
		for (size_t i = 0; i < m_objectives.size(); ++i) {
			sum_pow += pow(x1_[i] - x2_[i], 2);
		}
		return sqrt(sum_pow);
	}

	Real SolutionBase::objectiveDistance(const SolutionBase &sol) const {
		auto& x1_ = m_objectives;
		auto& x2_ = sol.m_objectives;
		Real sum_pow = 0;
		for (size_t i = 0; i < m_objectives.size(); ++i) {
			sum_pow += pow(x1_[i] - x2_[i], 2);
		}
		return sqrt(sum_pow);
	}

	size_t SolutionBase::numViolation() const {
		size_t count = 0;
		for (Real i : m_constraints) {
			if (i > 0) ++count;
		}
		return count;
	}

	void SolutionBase::setObjectiveInfinite(const Environment* env) {
		for (int i = 0; i < m_objectives.size(); ++i) {
			m_objectives[i] = (env->problem()->optimizeMode(i) == OptimizeMode::kMinimize) ?
				std::numeric_limits<Real>::max() : std::numeric_limits<Real>::min();
		}
	}

	int SolutionBase::evaluate(Environment *env, bool effective) {
		return env->evaluate(*this, effective);
	}
	
	void SolutionBase::validate(Environment *env, Validation mode, Random *rnd) {
		env->problem()->validateSolution(*this, mode, rnd);
	}

	bool SolutionBase::dominate(const SolutionBase& b, Environment* env) {
		return Dominance::kDominant ==  env->problem()->objectiveCompare(*this, b);
	}
}
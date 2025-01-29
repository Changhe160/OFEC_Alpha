#include "continuous.h"
#include "../../algorithm/algorithm.h"
#include "../solution.h"
#include <set>
#include <cmath>
#include <algorithm>

namespace ofec {
	void Continuous::reset() {
		ProblemVariableVector::reset();
		m_domain.clear();
	}

	void Continuous::evaluate(const VariableBase &vars, std::vector<Real> &objs, std::vector<Real> &cons) const {
		const VariableVector<Real> &x = dynamic_cast<const VariableType&>(vars);

		std::vector<Real> x_(x.begin(), x.end()); //for parallel running

		//if (isnan(x_.front())) {
		//	int stop = -1;
		//}
		if (boundaryViolated(vars)) {
			objs = m_default_objective;
			cons = m_default_contrait;
		}
		else {
			if (cons.empty()) {
				evaluateObjective(x_.data(), objs);
			}
			else {
				evaluateObjectiveAndConstraint(x_.data(), objs, cons);
			}
		}
	}

	void Continuous::initializeVariables(VariableBase &vars, Random *rnd) const {
		auto &x = dynamic_cast<VariableVector<Real>&>(vars);
		for (int i = 0; i < m_number_variables; ++i) {
			if (m_domain[i].limited) {    // If m_initial_domain is not given, then use problem boundary as initialization range
				x[i] = rnd->uniform.nextNonStd(m_domain[i].limit.first, m_domain[i].limit.second);
			}
			else {                        // Else if the problem function has no boundary 
				x[i] = rnd->uniform.nextNonStd(-std::numeric_limits<Real>::max(), std::numeric_limits<Real>::max());
			}
		}
	}

	void Continuous::resizeVariable(size_t num_vars) {
		ProblemVariableVector::resizeVariable(num_vars);
		m_domain.resize(m_number_variables);
	}

	bool Continuous::boundaryViolated(const VariableBase& baseX) const {
		const VariableVector<Real> &x = dynamic_cast<const VariableVector<Real>&>(baseX);
		for (int i = 0; i < m_number_variables; ++i) {
			if (m_domain[i].limited) {
				if (x[i]<m_domain[i].limit.first || x[i]>m_domain[i].limit.second)
					return true;
			}
		}
		return false;
	}

	void Continuous::validateSolution(SolutionBase &s, Validation mode, Random *rnd)const {
		VariableVector<Real> &x = dynamic_cast<Solution<>&>(s).variable();
		switch (mode) {
		case Validation::kIgnore:
			break;
		case Validation::kReinitialize:
			for (size_t j = 0; j < m_number_variables; ++j) {
				if (m_domain[j].limited)
					x[j] = rnd->uniform.nextNonStd(m_domain[j].limit.first, m_domain[j].limit.second);
			}
			break;
		case Validation::kRemap:
			for (size_t j = 0; j < m_number_variables; ++j) {

				if (std::isnan(x[j])) {
					if (m_domain[j].limited) x[j] = m_domain[j].limit.first;
					else x[j] = 0;
				}
				else {
					Real l = m_domain[j].limit.first, u = m_domain[j].limit.second;
					if (m_domain[j].limited) {
						if (x[j] < l)
							x[j] = l + (u - l) * (l - x[j]) / (u - x[j]);
						else if (x[j] > u)
							x[j] = l + (u - l) * (u - l) / (x[j] - l);
					}
				}
			}
			break;
		case Validation::kSetToBound:
			for (size_t j = 0; j < m_number_variables; ++j) {
				Real l = m_domain[j].limit.first, u = m_domain[j].limit.second;
				if (m_domain[j].limited) {
					if (x[j] < l)
						x[j] = l;
					else if (x[j] > u)
						x[j] = u;
				}

			}
			break;
		default:
			break;
		}
	}
	void Continuous::initializeAfter_(Environment* env){
		ProblemVariableVector<Real>::initializeAfter_(env);
		if (m_domain.isDomainLimited()) {
			// set the objective of solution at the edge as the default objective
			m_default_objective.resize(m_number_objectives);
			std::vector<Real> edge_x(m_number_variables);
			for (int idx(0); idx < m_number_variables; ++idx) {
				edge_x[idx] = m_domain[idx].limit.first;
			}

			if (m_number_constraints==0) {
				evaluateObjective(edge_x.data(), m_default_objective);
			}
			else {
				m_default_contrait.resize(m_number_constraints);
				evaluateObjectiveAndConstraint(edge_x.data(), m_default_objective, m_default_contrait);
			}
		}
		
	}
	void Continuous::setToBound(Real* x) {
		for (size_t j = 0; j < m_number_variables; ++j) {
			if (std::isnan(x[j])) {
				if (m_domain[j].limited) x[j] = m_domain[j].limit.first;
				else x[j] = 0;
			}
			else {
				Real l = m_domain[j].limit.first, u = m_domain[j].limit.second;
				if (m_domain[j].limited) {
					if (x[j] < l)
						x[j] = l;
					else if (x[j] > u)
						x[j] = u;
				}
			}
		}
	}

	std::vector<std::pair<Real, Real>> Continuous::boundary() const {
		std::vector<std::pair<Real, Real>> boundary(m_domain.size());
		for (size_t j = 0; j < m_domain.size(); ++j)
			boundary[j] = m_domain.range(j).limit;
		return boundary;
	}

	Real Continuous::domainArea() {
		return m_domain.area();
	}

	Real Continuous::domainVolume() {
		return m_domain.volume();
	}

	void Continuous::setDomain(Real l, Real u) {
		if (m_domain.size() != m_number_variables)
			m_domain.resize(m_number_variables);
		for (size_t i = 0; i < m_domain.size(); ++i)
			m_domain.setRange(l, u, i);
	}

	void Continuous::setDomain(const std::vector<std::pair<Real, Real>> &r) {
		size_t count = 0;
		for (auto &i : r) {
			m_domain.setRange(i.first, i.second, count++);
		}
	}

	void Continuous::setDomain(const Domain<Real> &domain) {
		m_domain = domain;
	}

	Real Continuous::maximumVariableDistance() const {
		VariableVector<Real> x1(m_number_variables, 0), x2(m_number_variables, 0);
		for (int idx(0); idx < m_number_variables; ++idx) {
			x1[idx] = m_domain.range(idx).limit.first;
			x2[idx] = m_domain.range(idx).limit.second;
		}
		Real sum_pow = 0;
		for (size_t i = 0; i < m_number_variables; ++i) {
			sum_pow += pow(x1[i] - x2[i], 2);
		}
		return sqrt(sum_pow);

	}

}

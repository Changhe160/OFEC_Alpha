/******************************************************************************
* Project:Open Frameworks for Evolutionary Computation (OFEC)
*******************************************************************************
* Author: Yiya Diao & Junchen Wang & Changhe Li
* Email: diaoyiyacug@gmail.com & wangjunchen.chris@gmail.com & changhe.lw@gmail.com
* Language: C++
*-------------------------------------------------------------------------------
*  This file is part of OFEC. This library is free software;
*  you can redistribute it and/or modify it under the terms of the
*  GNU General Public License as published by the Free Software
*  Foundation; either version 2, or (at your option) any later version.
*
*  see https://github.com/Changhe160/OFEC for more information
*
*-------------------------------------------------------------------------------
* class Continuous defines continuous optimization problems
*
*********************************************************************************/
#ifndef OFEC_CONTINUOUS_H
#define OFEC_CONTINUOUS_H

#include "../problem.h"
#include "../optima.h"
#include "../domain.h"
#include <vector>

#include "../../random/newran.h"
namespace ofec {
#define CAST_CONOP(pro) dynamic_cast<Continuous*>(pro)
	class Continuous : virtual public ProblemVariableVector<Real> {
	public:
		virtual ~Continuous() = default;

		void reset() override;
		virtual void evaluate(const VariableBase& vars, std::vector<Real> &objs, std::vector<Real> &cons) const override;
		void initializeVariables(VariableBase &vars, Random *rnd) const override;
		const Optima<>* optima() const { return dynamic_cast<Optima<>*>(m_optima.get()); }
		Optima<>* optima() { return dynamic_cast<Optima<>*>(m_optima.get()); }
		
		Real maximumVariableDistance() const override;
		bool boundaryViolated(const VariableBase&var) const ; // boundary check only
		bool boundaryViolated(const SolutionBase& s) const {
			return boundaryViolated(s.variableBase());
		}
		void validateSolution(SolutionBase &s, Validation mode, Random *rnd = nullptr) const override;

		void resizeVariable(size_t num_vars) override;

		/* Read-only methods */
		size_t numberVariables() const override { return m_number_variables; }
		const std::pair<Real, Real>& range(size_t i) const { return m_domain.range(i).limit; }
		std::vector<std::pair<Real, Real>> boundary() const;
		const Domain<Real>& domain() const { return m_domain; }


		/* Write methods */
		Real domainArea();
		Real domainVolume();
		void setDomain(Real l, Real u);
		void setDomain(const std::vector<std::pair<Real, Real>> &r);
		void setDomain(const Domain<Real> &domain);

	protected:

		virtual void initializeAfter_(Environment* env)override;
		void setToBound(Real* var);
		
		/**
		 * @brief Calculates the values of objectives based on the given decision variables.
		 * @param vars: The decision variables.
		 * @param objs: The values of objectives.
		 */
		virtual void evaluateObjective(Real *vars, std::vector<Real> &objs) const {}
		/**
		 * @brief Calculates the values of objectives and constraints based on the given decision variables.
		 * @param vars: The decision variables.
		 * @param objs: The values of objectives.
		 * @param cons: The values of constraints.
		 */
		virtual void evaluateObjectiveAndConstraint(Real *vars, std::vector<Real> &objs, std::vector<Real> &cons) const {}

	protected:
		Domain<Real> m_domain;		// search domain
		std::vector<Real> m_default_objective;// objectives of solution out of range
		std::vector<Real> m_default_contrait;// objectives of solution out of range
	};

}
#endif // !OFEC_CONTINUOUS_H
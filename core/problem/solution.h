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
* class solution is designed for any kind of user-defined solution representation.
* It takes a variable encoding and an objective value type as parameters to generate
* a solution.
*
*********************************************************************************/

#ifndef OFEC_SOLUTION_H
#define OFEC_SOLUTION_H

#include <utility>
#include "encoding.h"
#include "../random/newran.h"
#include "../enable_shared_ptr.h"

namespace  ofec {
	class Environment;

	class SolutionBase : public EnableSharedPtr<SolutionBase> {
	protected:
		std::unique_ptr<VariableBase> m_variables;
		std::vector<Real> m_objectives;
		std::vector<Real> m_constraints;
		size_t m_time_evaluate = 0;
		Real m_fitness = 0;
		int m_type = 0;
	
	public:
		SolutionBase() = default;
		/**
		 * @brief Constructs a `SolutionBase` object with the provided number of objectives and constraints.
		 * @param num_objs: The number of objectives.
		 * @param num_cons: The number of constraints.
		 */
		SolutionBase(size_t num_objs, size_t num_cons);
		SolutionBase(const SolutionBase &sol);
		SolutionBase(SolutionBase &&sol) noexcept;
		SolutionBase& operator=(const SolutionBase &sol);
		SolutionBase& operator=(SolutionBase &&sol) noexcept;

	public:
		virtual ~SolutionBase() = default;
		virtual void reset() {
			m_time_evaluate = 0;
			m_fitness = 0;
		}
		size_t objectiveSize() const noexcept { return m_objectives.size(); }
		size_t constraintSize() const noexcept { return m_constraints.size(); }
		const std::unique_ptr<VariableBase>& variablePointer()const { return m_variables; }
		const VariableBase& variableBase() const noexcept { return *m_variables; }
		const std::vector<Real>& objective() const noexcept { return m_objectives; }
		const std::vector<Real>& constraint() const noexcept { return m_constraints; }
		Real objective(size_t idx) const noexcept { return m_objectives[idx]; }
		Real constraint(size_t idx) const noexcept { return m_constraints[idx]; }
		size_t timeEvaluate() const noexcept { return m_time_evaluate; }
		size_t& timeEvaluate()  noexcept { return m_time_evaluate; }
		Real fitness() const noexcept { return m_fitness; }
		Real& fitness() noexcept { return m_fitness; }
		bool isFeasible() const noexcept {
			bool flag(true);
			for (auto &it : m_constraints) {
				if (it != 0) {
					flag = false;
					break;
				}
			}
			return flag;
		}


		virtual bool same(const SolutionBase& x, const Environment *env) const;

		virtual bool variableSame(const SolutionBase &x, const Environment *env) const;
		virtual Real variableDistance(const SolutionBase &sol, const Environment *env) const;
		virtual Real variableDistance(const VariableBase &x, const Environment *env) const;
		virtual Real objectiveDistance(const SolutionBase &sol) const;
		virtual Real objectiveDistance(const std::vector<Real> &objs) const;
		virtual size_t numViolation() const;

		virtual void initialize(Environment *env, Random *rnd);

		void resizeObjective(size_t n) { m_objectives.resize(n); }
		void resizeConstraint(size_t n) { m_constraints.resize(n); }
		VariableBase& variableBase() noexcept { return *m_variables; }
		std::vector<Real>& constraint() noexcept { return m_constraints; }
		std::vector<Real>& objective() noexcept { return m_objectives; }
		Real& objective(size_t idx) noexcept { return m_objectives[idx]; }
		Real& constraint(size_t idx) noexcept { return m_constraints[idx]; }
		virtual void setObjectiveInfinite(const Environment *env);
		void setFitness(Real value) noexcept { m_fitness = value; }
		int evaluate(Environment* env, bool effective = true);

		int type() const { return m_type; }
		int& type() { return m_type; }

		virtual void validate(Environment *env, Validation mode, Random *rnd = nullptr);
		bool dominate(const SolutionBase& b, Environment* env);

	};

	template<typename TVariable = VariableVector<Real>>
	class Solution : public SolutionBase {
	public:
		using VariableEncoding = TVariable;
	    using SolutionType = Solution<TVariable>;
		virtual ~Solution() {}
		Solution() : SolutionBase() {
			m_variables.reset(new VariableEncoding());
		};
		/**
		 * @brief Constructs a `Solution<TVariable>` object with the provided number of objectives and constraints and parameters of variables
		 * @param num_objs: The number of objectives.
		 * @param num_cons: The number of constraints.
		 * @param args: Parameters for constructing a `TVariable` object.
		 */
		template<typename ... Args>
		Solution(size_t num_objs, size_t num_cons, Args&& ... args) : SolutionBase(num_objs, num_cons) {
			m_variables.reset(new TVariable(std::forward<Args>(args)...));
		}
		Solution(const Solution &sol) : SolutionBase(sol) {
			m_variables.reset(new TVariable(dynamic_cast<const TVariable&>(*sol.m_variables)));
		}
		Solution(Solution &&sol) noexcept : SolutionBase(std::move(sol)) {}
		Solution& operator=(const Solution &sol) {
			if (this != &sol) {
				SolutionBase::operator=(sol);
				this->variable() = sol.variable();
				//m_variables.reset(new TVariable(dynamic_cast<const TVariable&>(*sol.m_variables)));
			}
			return *this;
		}
		Solution& operator=(Solution &&sol) noexcept {
			return dynamic_cast<Solution&>(SolutionBase::operator=(std::move(sol)));
		}
		explicit Solution(const SolutionBase &sol) : Solution(dynamic_cast<const Solution&>(sol)) {}
		explicit Solution(SolutionBase &&sol) : Solution(dynamic_cast<Solution&&>(sol)) {}
		const TVariable& variable() const noexcept { return dynamic_cast<const TVariable&>(*m_variables); }
		TVariable& variable() noexcept { return dynamic_cast<TVariable&>(*m_variables); }
		const Solution& solution() const { return *this; }
		Solution& solution() { return *this; }
	};


}

#endif // !OFEC_SOLUTION_H

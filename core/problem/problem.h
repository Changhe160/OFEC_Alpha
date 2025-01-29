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
* 
* the abstract of different types of problems
*
*********************************************************************************/

#ifndef OFEC_PROBLEM_H
#define OFEC_PROBLEM_H

#include <string>
#include <map>
#include <memory>
#include <set>
#include <array>
#include <list>
#include <vector>
#include "optima.h"
#include <sstream>

#include "../instance.h"

namespace ofec {
	class Environment;

	class Problem : public Instance {
		OFEC_ABSTRACT_INSTANCE(Problem)
	public:
		virtual void reset();
		void initialize(Environment *env);
		/**
		 * @brief Calculates the values of objectives and constraints based on the given decision variables.
		 * @param vars: The decision variables.
		 * @param objs: The values of objectives.
		 * @param cons: The values of constraints.
		 */
		virtual void evaluate(const VariableBase& vars, std::vector<Real>& objs, std::vector<Real>& cons) const = 0;

		/* Ready-only methods */
		size_t numberObjectives() const { return m_number_objectives; }
		size_t numberConstraints() const { return m_number_constraints; }
		virtual size_t numberVariables() const { return 0; }
		bool initialized() const { return m_initialized; }
		OptimizeMode optimizeMode(size_t idx) const { return m_optimize_mode[idx]; }
		const std::vector<OptimizeMode>& optimizeMode() const { return m_optimize_mode; }
		Constraint constraintType(size_t idx) const { return m_constraint_type[idx]; }
		const std::vector<Constraint>& constraintType() { return m_constraint_type; }
		bool isEqualityConstraint(size_t idx) const { return m_constraint_type[idx] == Constraint::kEquality; }
		const OptimaBase* optimaBase() const { return m_optima.get(); }
		virtual bool isOptimumSolutionGiven() const;
		virtual size_t numberOptimumSolutions() const;
		virtual bool isOptimumObjectiveGiven() const;
		virtual size_t numberOptimumObjectives() const;

		// for output to file 
		virtual void solutionToParameterVariants(const SolutionBase& sol, ParameterVariantStream& out)const;
		virtual void parameterVariantsToSolution(ParameterVariantStream& in, SolutionBase& sol)const;

		/* Write methods */
		void setOptimizeMode(OptimizeMode mode, size_t idx = 0) { m_optimize_mode[idx] = mode; }

		/* variables related methods */
		virtual VariableBase* createVariables() const = 0;
		virtual VariableBase* createVariables(const VariableBase &vars) const = 0;

		/* solution related methods */
		virtual SolutionBase* createSolution() const = 0;
		virtual SolutionBase* createSolution(const VariableBase& vars) const = 0;
		virtual SolutionBase* createSolution(const SolutionBase &sol) const = 0;

		/**
		* @brief Initializes decision variables using a random number generator.
		* @param vars: The decision variables to be initialized.
		* @param rnd: The random number generator used for initialization.
		*/
		virtual void initializeVariables(VariableBase& vars, Random* rnd) const = 0;
		/**
		* @brief Returns the distance between two given decision variables.
		* @param vars1: The first decision variables.
		* @param vars2: The second decision variables.
		*/
		virtual Real variableDistance(const VariableBase& vars1, const VariableBase& vars2) const = 0;

		virtual bool same(const VariableBase &vars1, const VariableBase &vars2) const {
			return variableDistance(vars1, vars2) == 0;
		}
		virtual Real normalizedVariableDistance(const VariableBase& vars1, const VariableBase& vars2) const {
			return variableDistance(vars1, vars2) / maximumVariableDistance();
		}
		virtual Real maximumVariableDistance()const { return 1.0; }
		virtual void resizeObjective(size_t num_objs);
		virtual void resizeConstraint(size_t num_cons);

		virtual void validateSolution(SolutionBase& sol, Validation mode, Random* rnd = nullptr) const {}

		//domination relationship between two objective vectors
		
		Dominance objectiveCompare(const SolutionBase& sola, const SolutionBase& solb) const;


	protected:
		void addInputParameters() {}
		virtual void initialize_(Environment *env) {}
		virtual void initializeAfter_(Environment* env) {}
		virtual void updateOptima(Environment *env) {}

		size_t m_number_objectives = 0;
		size_t m_number_constraints = 0;
		std::vector<OptimizeMode> m_optimize_mode;
		std::vector<Constraint> m_constraint_type;
		std::unique_ptr<OptimaBase> m_optima;

	private:
		Problem(const Problem&) = delete;
		Problem& operator=(const Problem&) = delete;

		bool m_initialized = false;		
	};


	template<typename TVariable>
	class ProblemVariable : virtual public Problem {
	public:
		using VariableType = TVariable;
		using SolutionType = Solution<TVariable>;

		VariableBase* createVariables() const override {
			return new VariableType();
		}

		VariableBase* createVariables(const VariableBase &vars) const override {
			return new VariableType(dynamic_cast<const VariableType&>(vars));
		}

		SolutionBase* createSolution() const override {
			return new SolutionType(m_number_objectives, m_number_constraints);
		}

		SolutionBase* createSolution(const VariableBase &vars) const override {
			return new SolutionType(m_number_objectives, m_number_constraints, dynamic_cast<const VariableType&>(vars));
		}

		SolutionBase *createSolution(const SolutionBase &sol) const override {
			return new SolutionType(dynamic_cast<const SolutionType&>(sol));
		}
	};

	template<typename TElement>
	class ProblemVariableVector : virtual public ProblemVariable<VariableVector<TElement>> {
	protected:
		size_t m_number_variables;

		void updateOptima(Environment *env) override {
			this->m_optima.reset(new Optima<VariableType>());
		}

	public:
		using VariableType = typename ProblemVariable<VariableVector<TElement>>::VariableType;
		using SolutionType = typename ProblemVariable<VariableVector<TElement>>::SolutionType;

		void reset() override {
			Problem::reset();
			m_number_variables = 0;
		}

		size_t numberVariables() const override { return m_number_variables; }

		virtual void resizeVariable(size_t num_vars) { m_number_variables = num_vars; }

		VariableBase* createVariables() const override {
			return new VariableType(m_number_variables);
		}

		SolutionBase* createSolution() const override {
			return new SolutionType(this->m_number_objectives, this->m_number_constraints, m_number_variables);
		}

		Real variableDistance(const VariableBase &vars1, const VariableBase &vars2) const override {
			auto &x1_ = dynamic_cast<const VariableType&>(vars1);
			auto &x2_ = dynamic_cast<const VariableType&>(vars2);
			Real sum_pow = 0;
			for (size_t i = 0; i < m_number_variables; ++i) {
				sum_pow += pow(x1_[i] - x2_[i], 2);
			}
			return sqrt(sum_pow);
		}

		const Optima<VariableType>* optima() const { return dynamic_cast<Optima<VariableType>*>(this->m_optima.get()); }

		// for output to file 
		virtual void solutionToParameterVariants(
			const SolutionBase& sol, ParameterVariantStream& stream) const override {
			if (isVariantMember<std::vector<TElement>, ParameterVariant>::value) {
				Problem::solutionToParameterVariants(sol, stream);
				auto& vars = dynamic_cast<const SolutionType&>(sol).variable().vector();
				stream << vars;
				
			}
			else {
				throw Exception("ProblemVariableVector::solutionToParameterVariants: not a ParameterVariant type\n");
			}
		}
		virtual void parameterVariantsToSolution(
			ParameterVariantStream& stream, SolutionBase& sol) const override {
	
			if (isVariantMember<std::vector<TElement>, ParameterVariant>::value) {
				Problem::parameterVariantsToSolution(stream,sol);
				auto& vars = dynamic_cast<SolutionType&>(sol).variable().vector();
				stream >> vars;
			}
			else {
				throw Exception("ProblemVariableVector::parameterVariantsToSolution: not a ParameterVariant type\n");
			}
		}

	};



}


#endif // !OFEC_PROBLEM_H

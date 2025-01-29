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

#ifndef OFEC_ENVIRONMENT_H
#define OFEC_ENVIRONMENT_H

#include <memory>
#include <string>
#include "../instance.h"
#include "../problem/problem.h"
#include "../algorithm/algorithm.h"

namespace  ofec {
	class Environment : public EnableSharedPtr<Environment>, public Instance {
		OFEC_CONCRETE_INSTANCE(Environment)
	protected:
		std::unique_ptr<Problem> m_problem = nullptr;
		std::unique_ptr<Algorithm> m_algorithm = nullptr;

		void addInputParameters() {}
		virtual void reset() {}
		virtual void initialize_() {}
		virtual int evaluate_(SolutionBase &sol);

	public:
		virtual ~Environment() = default;
		int evaluate(SolutionBase &sol, bool effective = true);

		void initialize(Real seed = 0);
		void initialize(const std::shared_ptr<Random> &random);

		Problem* problem() { return m_problem.get(); }
		const Problem* problem() const { return m_problem.get(); }
		void setProblem(Problem *pro) { m_problem.reset(pro); }
		void initializeProblem(Real seed = 0);
		void initializeProblem(const std::shared_ptr<Random> &random);

		Algorithm* algorithm() { return m_algorithm.get(); }
		const Algorithm* algorithm()const { return m_algorithm.get(); }
		void setAlgorithm(Algorithm *alg) { m_algorithm.reset(alg); }
		void initializeAlgorithm(Real seed = 0);
		void initializeAlgorithm(const std::shared_ptr<Random> &random);
		void runAlgorithm();

		virtual void clear();
	};
}
#endif // !OFEC_ENVIRONMENT_H

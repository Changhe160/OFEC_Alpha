/******************************************************************************
* Project:Open Frameworks for Evolutionary Computation (OFEC)
*******************************************************************************
* Author: Changhe Li and Li Zhou
* Email: changhe.lw@gmail.com, 441837060@qq.com
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
* class DESolution is a class for differential evolution of Solutions of an
* evolutionary computation algorithm.
*********************************************************************************/
// updated Mar 28, 2018 by Li Zhou

#ifndef OFEC_DE_INDIVIDUAL_H
#define OFEC_DE_INDIVIDUAL_H

#include "../../../../../core/problem/solution.h"
#include "definition.h"

namespace ofec {
	class IndividualDE : public Solution<> {
	protected:
		Solution<> m_pv;    // donor vector
		Solution<> m_pu;    // trial vector
		bool m_improved;
	public:
		IndividualDE() = default;
		IndividualDE(Environment *env);
		IndividualDE(size_t num_objs, size_t num_cons, size_t num_vars);
		explicit IndividualDE(const Solution<> &sol);
		bool isImproved() const { return m_improved; }
		void setImproved(bool flag) { m_improved = flag; }
		void mutate(Real F, 
			const Solution<>* s1,
			const Solution<>* s2,
			const Solution<>* s3,
			Environment *env,
			const Solution<>* s4 = nullptr,
			const Solution<>* s5 = nullptr,
			const Solution<>* s6 = nullptr,
			const Solution<>* s7 = nullptr);
		virtual void recombine(Real CR, de::RecombineStrategy RS, Random *rnd, Environment *env);
		virtual int select(Environment *env);
		Solution<>& trial() { return m_pu; }
		const Solution& trial() const { return m_pu; }
		Solution<>& donor() { return m_pv; }
		const Solution& donor() const { return m_pv; }
		void recombine(Real CR, const std::vector<int>& var, int I, Random *rnd);
		void mutate(Real F, 
			const std::vector<int>& var, 
			const Solution<>* s1,
			const Solution<>* s2,
			const Solution<>* s3,
			Environment *env,
			const Solution<>* s4 = nullptr,
			const Solution<>* s5 = nullptr,
			const Solution<>* s6 = nullptr,
			const Solution<>* s7 = nullptr);
		void mutate(Real K, Real F,
			const Solution<>* s1,
			const Solution<>* s2,
			const Solution<>* s3,
			const Solution<>* s4,
			const Solution<>* s5,
			Environment *env);
	};
}
#endif // !OFEC_DE_INDIVIDUAL_H

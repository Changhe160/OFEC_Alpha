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
* class DEindividual is a class for differential evolution of individuals of an
* evolutionary computation algorithm.
*********************************************************************************/
// updated Mar 28, 2018 by Li Zhou

#ifndef DE_INDIVIDUAL_H
#define DE_INDIVIDUAL_H

#include "../../../core/algorithm/individual.h"
#include "../../../core/problem/continuous/continuous.h"

namespace OFEC {
	namespace DE {
		// exp: exponential crossover
		// bin: binomial crossover
		// ari: Arithmetic Recombination
		enum class recombine_strategy { binomial, exponential };

		class individual : public OFEC::individual<> {
		protected:
			solution<> m_pv;    // donor vector
			solution<> m_pu;    // trial vector
		public:
			individual() = default;
			individual(size_t num_obj, size_t num_con, size_t size_var);
			individual(const individual&) = default;
			individual(individual &&p) noexcept = default;
			individual(const solution<>& sol);
			individual& operator=(const individual &other) = default;
			individual& operator=(individual &&other) noexcept = default;
			void initialize(int id) override;
			void mutate(real F, solution<> *r1,
				solution<> *r2,
				solution<> *r3,
				solution<> *r4 = nullptr,
				solution<> *r5 = nullptr);
			virtual void recombine(real CR, recombine_strategy rs);
			virtual evaluation_tag select();
			solution<>& trial();
			void recombine(real CR, const std::vector<int> &var, int I);
			void mutate(real F, const std::vector<int> &var, solution<> *r1,
				solution<> *r2,
				solution<> *r3,
				solution<> *r4 = nullptr,
				solution<> *r5 = nullptr);
			//evaluation_tag select(const std::vector<int> &var, solution<> &best);
		};
	}
}
#endif // !DE_INDIVIDUAL_H

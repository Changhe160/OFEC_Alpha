/*************************************************************************
* Project:Open Frameworks for Evolutionary Computation (OFEC)
*************************************************************************
* Author: Changhe Li
* Email: changhe.lw@gmail.com
* Language: C++
*************************************************************************
*  This file is part of OFEC. This library is free software;
*  you can redistribute it and/or modify it under the terms of the
*  GNU General Public License as published by the Free Software
*  Foundation; either version 2, or (at your option) any later version.
*************************************************************************/

#ifndef OFEC_CEC2015_HYBRID_H
#define OFEC_CEC2015_HYBRID_H

#include "../../../../../core/problem/continuous/continuous.h"
#include "../../../../../core/problem/continuous/function.h"

namespace OFEC {
	namespace CEC2015 {
		class hybrid : public continuous
		{
		public:
			hybrid(const std::string &name, size_t size_var, size_t size_obj = 1);
			~hybrid();
			size_t get_num_function();
			function* get_function(size_t num);
			std::vector<size_t>& dim() { return m_dim; }
			std::vector<size_t>& random_perm() {
				return m_random_perm;
			}
			std::vector<real>& hybrid_translation() {
				return m_hybrid_translation;
			}
		protected:
			virtual void initialize() = 0;
			virtual void evaluate__(real *x, std::vector<real>& obj);
			virtual void set_function() = 0;
		protected:
			size_t m_num_function;
			std::vector<function*> m_function;    // the functions
			std::vector<size_t> m_start, m_dim;
			std::vector<size_t> m_random_perm;
			std::vector<real> m_hybrid_translation;
		};

	}
}
#endif // ! OFEC_CEC2015_HYBRID_H


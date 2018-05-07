/*************************************************************************
* Project:Open Frameworks for Evolutionary Computation (OFEC)
*************************************************************************
* Author: Changhe Li and Junchen Wang
* Email: changhe.lw@gmail.com, wangjunchen.chris@gmail.com
* Language: C++
*************************************************************************
*  This file is part of OFEC. This library is free software;
*  you can redistribute it and/or modify it under the terms of the
*  GNU General Public License as published by the Free Software
*  Foundation; either version 2, or (at your option) any later version.
*************************************************************************
*  Paper: Benchmark Functions for CEC¡¯2013 Special Session and Competition on Niching
*  Methods for Multimodal Function Optimization.
*******************************************************************************************/

#ifndef F8_MODIFIED_RASTRIGIN_H
#define F8_MODIFIED_RASTRIGIN_H

#include "../../../../../core/problem/continuous/function.h"

namespace OFEC {
	namespace CEC2013 {
		// An inverted version of Modified Rastrigin function
		class F8_modified_rastrigin final : public function
		{
		public:
			F8_modified_rastrigin(param_map &v);
			F8_modified_rastrigin(const std::string &name, size_t size_var, size_t size_obj);
			void initialize();
		protected:		
			void evaluate__(real *x, std::vector<real>& obj);
		protected:
			std::vector<double> m_k;
		};
	}
}

#endif // !F8_MODIFIED_RASTRIGIN_H



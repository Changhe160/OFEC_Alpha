/*************************************************************************
* Project:Open Frameworks for Evolutionary Computation (OFEC)
*************************************************************************
* Author: Changhe Li and Li Zhou
* Email: changhe.lw@gmail.com, 441837060@qq.com
* Language: C++
*************************************************************************
*  This file is part of OFEC. This library is free software;
*  you can redistribute it and/or modify it under the terms of the
*  GNU General Public License as published by the Free Software
*  Foundation; either version 2, or (at your option) any later version.
*************************************************************************/


#ifndef OFEC_F5_SCHWEFEL_2_6_BOUND_H
#define OFEC_F5_SCHWEFEL_2_6_BOUND_H

#include "../../../../../core/problem/continuous/function.h"

namespace OFEC {
	namespace CEC2005 {
		class F5_schwefel_2_6_bound final : public function
		{
		public:
			F5_schwefel_2_6_bound(param_map &v);
			F5_schwefel_2_6_bound(const std::string &name, size_t size_var, size_t size_obj);
		protected:
			void initialize();
			void evaluate__(real *x, std::vector<real>& obj);
			void load_data(const std::string & path);

			std::vector<real> m_b;
			std::vector<std::vector<int>> m_a;
		private:
		};
	}
	using CEC2005_GOP_F5 = CEC2005::F5_schwefel_2_6_bound;
}
#endif // ! OFEC_F5_SCHWEFEL_2_6_BOUND_H

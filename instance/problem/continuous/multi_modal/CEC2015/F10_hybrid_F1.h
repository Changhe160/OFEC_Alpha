
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


#ifndef OFEC_F10_HYBRID_F1_H
#define OFEC_F10_HYBRID_F1_H

#include "../../../../../core/problem/continuous/function.h"

namespace OFEC {
	namespace CEC2015 {
		class F10_hybrid_F1 : public function
		{
		public:
			F10_hybrid_F1(param_map &v);
			F10_hybrid_F1(const std::string &name, size_t size_var, size_t size_obj);
			virtual ~F10_hybrid_F1();
		protected:
			void initialize();
			void evaluate__(real *x, std::vector<real>& obj);
		private:
			size_t m_num_function;
			std::vector<function*> m_function;    // the functions
			std::vector<size_t> m_start, m_dim;
		};
	}
}
#endif // ! OFEC_F10_HYBRID_F1_H



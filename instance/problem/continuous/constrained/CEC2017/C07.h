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


#ifndef OFEC_C07_H
#define OFEC_C07_H

#include "../../../../../core/problem/continuous/function.h"

namespace OFEC {
	namespace CEC2017 {
		class C07 final: public function
		{
		public:
			C07(param_map &v);
			C07(const std::string &name, size_t size_var, size_t size_obj);
			void initialize();
		protected:
			void evaluate_obj_nd_con(real *x, std::vector<real>& obj, std::vector<real> &con) override;
		private:

		};
	}
	using CEC2017_COP_F07 = CEC2017::C07;
}
#endif // ! OFEC_C07_H





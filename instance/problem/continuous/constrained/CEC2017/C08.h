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


#ifndef OFEC_C08_H
#define OFEC_C08_H

#include "../../../../../core/problem/continuous/function.h"

namespace OFEC {
	namespace CEC2017 {
		class C08 final: public function
		{
		public:
			C08(param_map &v);
			C08(const std::string &name, size_t size_var, size_t size_obj);
		protected:
			void initialize_problem();
			void evaluate__(real *x, std::vector<real>& obj, double & cons_value, std::vector<double> &cons_values);
		private:

		};
	}
	using CEC2017_COP_F08 = CEC2017::C08;
}
#endif // ! OFEC_C08_H






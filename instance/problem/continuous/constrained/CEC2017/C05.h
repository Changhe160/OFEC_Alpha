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


#ifndef OFEC_C05_H
#define OFEC_C05_H

#include "../../../../../core/problem/continuous/function.h"

namespace OFEC {
	namespace CEC2017 {
		class C05 final: public function
		{
		public:
			C05(param_map &v);
			C05(const std::string &name, size_t size_var, size_t size_obj);
		protected:
			void initialize();
			void evaluate__(real *x, std::vector<real>& obj, double & cons_value, std::vector<double> &cons_values);
			bool load_rotation_C05(const std::string &path);
			void load_rotation_C05_(const std::string &path);
			void set_rotation_C05();
			void rotate(real *x, size_t num);
		private:
			matrix m_mat1, m_mat2;
		};
	}
	using CEC2017_COP_F05 = CEC2017::C05;
}
#endif // ! OFEC_C05_H



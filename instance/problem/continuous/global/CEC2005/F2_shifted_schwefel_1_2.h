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


#ifndef OFEC_F2_SHIFTED_SCHWEFEL_1_2_H
#define OFEC_F2_SHIFTED_SCHWEFEL_1_2_H

#include "../classical/schwefel_1_2.h"

namespace OFEC {
	namespace CEC2005 {
		class F2_shifted_schwefel_1_2 final : public schwefel_1_2
		{
		public:
			F2_shifted_schwefel_1_2(param_map &v);
			F2_shifted_schwefel_1_2(const std::string &name, size_t size_var, size_t size_obj);
		protected:
			void initialize();
			void evaluate__(real *x, std::vector<real>& obj);
		private:
		};
	}
}
#endif // ! OFEC_F2_SHIFTED_SCHWEFEL_1_2_H

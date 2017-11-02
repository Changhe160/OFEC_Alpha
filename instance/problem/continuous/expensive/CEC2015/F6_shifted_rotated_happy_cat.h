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


#ifndef OFEC_F6_SHIFTED_ROTATED_HAPPY_CAT_H
#define OFEC_F6_SHIFTED_ROTATED_HAPPY_CAT_H

#include "../../global/classical/happy_cat.h"

namespace OFEC {
	namespace CEC2015 {
		class F6_shifted_rotated_happy_cat final : public happy_cat
		{
		public:
			F6_shifted_rotated_happy_cat(param_map &v);
			F6_shifted_rotated_happy_cat(const std::string &name, size_t size_var, size_t size_obj);
		protected:
			void initialize();
			void evaluate__(real *x, std::vector<real>& obj);
		private:
		};
	}
}
#endif // ! OFEC_F6_SHIFTED_ROTATED_HAPPY_CAT_H



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


#ifndef OFEC_F9_SHIFTED_ROTATED_SCAFFER_F6_H
#define OFEC_F9_SHIFTED_ROTATED_SCAFFER_F6_H

#include "../../global/classical/scaffer_F6.h"

namespace OFEC {
	namespace CEC2015 {
		class F9_shifted_rotated_scaffer_F6 final : public scaffer_F6
		{
		public:
			F9_shifted_rotated_scaffer_F6(param_map &v);
			F9_shifted_rotated_scaffer_F6(const std::string &name, size_t size_var, size_t size_obj);
		protected:
			void initialize();
			void evaluate__(real *x, std::vector<real>& obj);
		private:
		};
	}
}
#endif // ! OFEC_F9_SHIFTED_ROTATED_SCAFFER_F6_H


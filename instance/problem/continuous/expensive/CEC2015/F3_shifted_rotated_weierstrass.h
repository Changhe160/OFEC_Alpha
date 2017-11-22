
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


#ifndef OFEC_F3_SHIFTED_ROTATED_WEIERSTRASS_H
#define OFEC_F3_SHIFTED_ROTATED_WEIERSTRASS_H

#include "../../global/classical/weierstrass.h"

namespace OFEC {
	namespace CEC2015 {
		class F3_shifted_rotated_weierstrass final : public weierstrass
		{
		public:
			F3_shifted_rotated_weierstrass(param_map &v);
			F3_shifted_rotated_weierstrass(const std::string &name, size_t size_var, size_t size_obj);
		protected:
			void initialize();
			void evaluate__(real *x, std::vector<real>& obj);
		private:
		};
	}
	using CEC2015_EOP_F3 = CEC2015::F3_shifted_rotated_weierstrass;
}
#endif // ! OFEC_F3_SHIFTED_ROTATED_WEIERSTRASS_H


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


#ifndef OFEC_F7_SHIFTED_ROTATED_HGBAT_H
#define OFEC_F7_SHIFTED_ROTATED_HGBAT_H

#include "../../global/classical/HGBat.h"

namespace OFEC {
	namespace CEC2015 {
		class F7_shifted_rotated_HGBat final : public HGBat
		{
		public:
			F7_shifted_rotated_HGBat(param_map &v);
			F7_shifted_rotated_HGBat(const std::string &name, size_t size_var, size_t size_obj);
		protected:
			void initialize();
			void evaluate__(real *x, std::vector<real>& obj);
		private:
		};
	}
	using CEC2015_EOP_F7 = CEC2015::F7_shifted_rotated_HGBat;
}
#endif // ! OFEC_F7_SHIFTED_ROTATED_HGBAT_H


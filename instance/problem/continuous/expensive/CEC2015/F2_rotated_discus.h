
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


#ifndef OFEC_F2_ROTATED_DISCUS_H
#define OFEC_F2_ROTATED_DISCUS_H

#include "../../global/classical/discus.h"

namespace OFEC {
	namespace CEC2015 {
		class F2_rotated_discus final : public discus
		{
		public:
			F2_rotated_discus(param_map &v);
			F2_rotated_discus(const std::string &name, size_t size_var, size_t size_obj);
		protected:
			void initialize();
			void evaluate__(real *x, std::vector<real>& obj);
		private:
		};
	}
	using CEC2015_EOP_F2 = CEC2015::F2_rotated_discus;
}
#endif // ! OFEC_F2_ROTATED_DISCUS_H

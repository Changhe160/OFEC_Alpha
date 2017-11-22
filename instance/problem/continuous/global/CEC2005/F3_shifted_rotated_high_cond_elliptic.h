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


#ifndef OFEC_F3_SHIFTED_ROTATED_HIGH_COND_ELLIPTIC_H
#define OFEC_F3_SHIFTED_ROTATED_HIGH_COND_ELLIPTIC_H

#include "../classical/elliptic.h"

namespace OFEC {
	namespace CEC2005 {
		class F3_shifted_rotated_high_cond_elliptic final : public elliptic
		{
		public:
			F3_shifted_rotated_high_cond_elliptic(param_map &v);
			F3_shifted_rotated_high_cond_elliptic(const std::string &name, size_t size_var, size_t size_obj);
		protected:
			void initialize();
			void evaluate__(real *x, std::vector<real>& obj);
		private:
		};
	}
	using CEC2005_GOP_F3 = CEC2005::F3_shifted_rotated_high_cond_elliptic;
}
#endif // ! OFEC_F3_SHIFTED_ROTATED_HIGH_COND_ELLIPTIC_H

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


#ifndef OFEC_F8_SHIFTED_ROTATED_ACKLEY_BOUND_H
#define OFEC_F8_SHIFTED_ROTATED_ACKLEY_BOUND_H

#include "../classical/ackley.h"

namespace OFEC {
	namespace CEC2005 {
		class F8_shifted_rotated_ackley_bound final: public ackley
		{
		public:
			F8_shifted_rotated_ackley_bound(param_map &v);
			F8_shifted_rotated_ackley_bound(const std::string &name, size_t size_var, size_t size_obj);
		protected:
			void initialize();
			void evaluate__(real *x, std::vector<real>& obj);
		private:
		};
	}
	using CEC2005_GOP_F8 = CEC2005::F8_shifted_rotated_ackley_bound;
}
#endif // ! OFEC_F8_SHIFTED_ROTATED_ACKLEY_BOUND_H



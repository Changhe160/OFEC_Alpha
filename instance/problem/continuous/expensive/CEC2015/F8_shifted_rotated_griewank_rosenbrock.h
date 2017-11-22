
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


#ifndef OFEC_F8_SHIFTED_ROTATED_GRIEWANK_ROSENBROCK_H
#define OFEC_F8_SHIFTED_ROTATED_GRIEWANK_ROSENBROCK_H

#include "../../global/classical/griewank_rosenbrock.h"

namespace OFEC {
	namespace CEC2015 {
		class F8_shifted_rotated_griewank_rosenbrock : public griewank_rosenbrock
		{
		public:
			F8_shifted_rotated_griewank_rosenbrock(param_map &v);
			F8_shifted_rotated_griewank_rosenbrock(const std::string &name, size_t size_var, size_t size_obj);
		protected:
			void initialize();
			void evaluate__(real *x, std::vector<real>& obj);
		private:
		};
	}
	using CEC2015_EOP_F8 = CEC2015::F8_shifted_rotated_griewank_rosenbrock;
}
#endif // ! OFEC_F8_SHIFTED_ROTATED_GRIEWANK_ROSENBROCK_H


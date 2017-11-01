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


#ifndef OFEC_F6_SHIFTED_ROSENBROCK_H
#define OFEC_F6_SHIFTED_ROSENBROCK_H

#include "../classical/rosenbrock.h"

namespace OFEC {
	namespace CEC2005 {
		class F6_shifted_rosenbrock final : public rosenbrock
		{
		public:
			F6_shifted_rosenbrock(param_map &v);
			F6_shifted_rosenbrock(const std::string &name, size_t size_var, size_t size_obj);
		protected:
			void initialize();
			void evaluate__(real *x, std::vector<real>& obj);
		private:
		};
	}
}
#endif // ! OFEC_F6_SHIFTED_ROSENBROCK_H


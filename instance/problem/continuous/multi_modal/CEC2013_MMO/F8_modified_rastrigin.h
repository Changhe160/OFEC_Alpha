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
******************************************************************************************
*  Paper: A bi-criterion approach to multimodal optimization:
*         self-adaptive approach
*******************************************************************************************/

#ifndef OFEC_F8_FMODIFIED_RASTRIGIN_H
#define OFEC_F8_FMODIFIED_RASTRIGIN_H

#include "../classical/modified_rastrigin.h"

namespace OFEC {
	namespace CEC2013_MMO {
		class F8_modified_rastrigin final : public modified_rastrigin
		{
		public:
			F8_modified_rastrigin(param_map &v);
			F8_modified_rastrigin(const std::string &name, size_t size_var, size_t size_obj);
		protected:
			void evaluate__(real *x, std::vector<real>& obj);
		private:
			std::vector<double> m_k;
		};
	}
}
#endif // ! OFEC_F8_FMODIFIED_RASTRIGIN_H
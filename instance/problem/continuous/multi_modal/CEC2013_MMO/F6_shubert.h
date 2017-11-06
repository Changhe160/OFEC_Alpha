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


#ifndef OFEC_F6_SHUBERT_H
#define OFEC_F6_SHUBERT_H

#include "../classical/shubert.h"

namespace OFEC {
	namespace CEC2013_MMO {
		class F6_shubert final : public shubert
		{
		public:
			F6_shubert(param_map &v);
			F6_shubert(const std::string &name, size_t size_var, size_t size_obj);
		protected:
			void evaluate__(real *x, std::vector<real>& obj);
		private:
		};
	}
}
#endif // ! OFEC_F6_SHUBERT_H
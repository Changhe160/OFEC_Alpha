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
*  Paper: A sequential niching memetic algorithm for continuous multimodal
*		  Appled Mathematics and Computation 218(2012) 8242-8259
*******************************************************************************************/

#ifndef OFEC_F7_VINCENT_H
#define OFEC_F7_VINCENT_H

#include "../classical/vincent.h"

namespace OFEC {
	namespace CEC2013_MMO {
		class F7_vincent final : public vincent
		{
		public:
			F7_vincent(param_map &v);
			F7_vincent(const std::string &name, size_t size_var, size_t size_obj);
		protected:
			void evaluate__(real *x, std::vector<real>& obj);
		private:
		};
	}
}
#endif //!OFEC_F7_VINCENT_H
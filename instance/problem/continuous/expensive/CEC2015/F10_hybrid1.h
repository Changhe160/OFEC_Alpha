
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


#ifndef OFEC_F10_HYBRID1_H
#define OFEC_F10_HYBRID1_H

#include "hybrid.h"

namespace OFEC {
	namespace CEC2015 {
		class F10_hybrid1 final : public hybrid
		{
		public:
			F10_hybrid1(param_map &v);
			F10_hybrid1(const std::string &name, size_t size_var, size_t size_obj);
		protected:
			void initialize();
			void evaluate__(real *x, std::vector<real>& obj);
			void set_function();
		private:
		
		};
	}
	using CEC2015_EOP_F10 = CEC2015::F10_hybrid1;
}
#endif // ! OFEC_F10_HYBRID1_H



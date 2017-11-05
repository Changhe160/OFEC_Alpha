
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


#ifndef OFEC_F11_HYBRID_F2_H
#define OFEC_F11_HYBRID_F2_H

#include "hybrid.h"

namespace OFEC {
	namespace CEC2015 {
		class F11_hybrid_F2 final : public hybrid
		{
		public:
			F11_hybrid_F2(param_map &v);
			F11_hybrid_F2(const std::string &name, size_t size_var, size_t size_obj);
		protected:
			void initialize();
			void evaluate__(real *x, std::vector<real>& obj);
			void set_function();
		private:
			
		};
	}
}
#endif // ! OFEC_F11_HYBRID_F2_H





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


#ifndef OFEC_F12_HYBRID_F3_H
#define OFEC_F12_HYBRID_F3_H

#include "hybrid.h"

namespace OFEC {
	namespace CEC2015 {
		class F12_hybrid_F3 final : public hybrid
		{
		public:
			F12_hybrid_F3(param_map &v);
			F12_hybrid_F3(const std::string &name, size_t size_var, size_t size_obj);
		protected:
			void initialize();
			void evaluate__(real *x, std::vector<real>& obj);
			void set_function();
		private:
			
		};
	}
}
#endif // ! OFEC_F12_HYBRID_F3_H





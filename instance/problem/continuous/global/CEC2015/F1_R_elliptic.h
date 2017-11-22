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
*************************************************************************
*  Paper : Problem Definitions and Evaluation Criteria for the CEC2015 
*  Competition on Learning-based Real-Parameter Single Objective 
*  Optimization. 
************************************************************************/
#ifndef OFEC_F1_R_ELLIPTIC_H
#define OFEC_F1_R_ELLIPTIC_H

#include "../classical/elliptic.h"
namespace OFEC {
	namespace CEC2015 {
		class F1_R_elliptic final: public elliptic
		{
		public:
			F1_R_elliptic(param_map &v);
			F1_R_elliptic(const std::string &name, size_t size_var, size_t size_obj);
		protected:
			void initialize();
			void evaluate__(real *x, std::vector<real>& obj);

		private:
		};
	}
	using CEC2015_GOP_F1 = CEC2015::F1_R_elliptic;
}

#endif // !OFEC_F1_R_ELLIPTIC_H


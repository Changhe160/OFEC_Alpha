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
*  Paper: Problem Definitions and Evaluation Criteria for the CEC 2015
*  Competition on Single Objective Multi-Niche Optimization.
*******************************************************************************************/


#ifndef OFEC_CEC2015_F15_COMPOSITION7_H
#define OFEC_CEC2015_F15_COMPOSITION7_H

#include "../../expensive/CEC2015/composition_2015.h"


namespace OFEC {
	namespace CEC2015 {
		class F15_composition2015_C7 final : public composition_2015
		{
		public:
			F15_composition2015_C7(param_map &v);
			F15_composition2015_C7(const std::string &name, size_t size_var, size_t size_obj);
			real pre_opt_distance();
		protected:
			void initialize();
			void evaluate__(real *x, std::vector<real>& obj);
			void set_function();

			void rotate(size_t num, real *x);
			void scale(size_t num, real *x);

			bool load_translation(const std::string &path);
			void set_translation();
			void set_weight(std::vector<double>& weight, const std::vector<real>&x);
		private:
			real m_pre_opt_distance;
		};
	}
	using CEC2015_MMP_F15 = CEC2015::F15_composition2015_C7;
}
#endif // !OFEC_CEC2015_F15_COMPOSITION7_H












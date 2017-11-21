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

#ifndef OFEC_F10_GLOBAL_COMPOSITION2_H
#define OFEC_F10_GLOBAL_COMPOSITION2_H

#include "../../expensive/CEC2015/composition_2015.h"
#include "../../expensive/CEC2015/hybrid.h"

namespace OFEC {
	namespace CEC2015 {
		class F10_global_composition2 final : public composition_2015
		{
		public:
			F10_global_composition2(param_map &v);
			F10_global_composition2(const std::string &name, size_t size_var, size_t size_obj);
			~F10_global_composition2();
			hybrid* get_hybrid(size_t num); 
		protected:
			void initialize();
			void evaluate__(real *x, std::vector<real>& obj);
			void set_function();
			bool load_translation(const std::string &path);
			void set_translation();
			void set_weight(std::vector<double>& weight, const std::vector<real>&x);
		protected:
			std::vector<hybrid*> m_hybrid;
		};
	}
	using CEC2015_GOP_F10 = CEC2015::F10_global_composition2;
}
#endif // !OFEC_F10_GLOBAL_COMPOSITION2_H



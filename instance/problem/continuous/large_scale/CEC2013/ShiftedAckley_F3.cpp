/*************************************************************************
* Project:Open Frameworks for Evolutionary Computation (OFEC)
*************************************************************************
* Author: Li Zhou
* Email: 441837060@qq.com
* Language: C++
*************************************************************************
*  This file is part of OFEC. This library is free software;
*  you can redistribute it and/or modify it under the terms of the
*  GNU General Public License as published by the Free Software
*  Foundation; either version 2, or (at your option) any later version.
*************************************************************************/
// Created: 8 August 2017
// Last modified:
#include "ShiftedAckley_F3.h"
namespace OFEC {
	namespace CEC2013 {
		ShiftedAckley_F3::ShiftedAckley_F3(param_map &v) : problem((v[param_proName]), (v[param_numDim]), 1), \
			function_CEC2013((v[param_proName]), (v[param_numDim]), 1) \
		{
			set_range(-32, 32);
			set_init_range(-32, 32);
			initialize();
		}

		ShiftedAckley_F3::ShiftedAckley_F3(const std::string &name, size_t size_var, size_t size_obj) : problem(name, size_var, size_obj), \
			function_CEC2013(name, size_var, size_obj) \
		{
			set_range(-32, 32);
			set_init_range(-32, 32);
			initialize();
		}

		ShiftedAckley_F3::~ShiftedAckley_F3() {
			delete[] mp_Ovector;
			delete[] mp_anotherz;
		}

		void ShiftedAckley_F3::initialize() {
			ID = 3;
			mp_anotherz = new real[m_variable_size];

			// Ovector = createShiftVector(dimension,minX,maxX);
			mp_Ovector = readOvector();

			set_original_global_opt();
			set_global_opt(mp_Ovector);
		}

		void ShiftedAckley_F3::evaluate__(real *x, std::vector<real>& obj) {
			size_t    i;
			for (i = 0; i < m_variable_size; ++i) {
				mp_anotherz[i] = x[i] - mp_Ovector[i];
			}
			obj[0] = ackley(mp_anotherz, m_variable_size);
		}
	}
}
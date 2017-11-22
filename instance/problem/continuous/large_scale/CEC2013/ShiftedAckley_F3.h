#pragma once
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
#ifndef _SHIFTEDACKLEY_F3_H
#define _SHIFTEDACKLEY_F3_H

#include "function_CEC2013.h"
namespace OFEC {
	namespace CEC2013 {
		class ShiftedAckley_F3 final : public function_CEC2013 {
		public:
			ShiftedAckley_F3(param_map &v);
			ShiftedAckley_F3(const std::string &name, size_t size_var, size_t size_obj);
			void evaluate__(real *x, std::vector<real>& obj);
			~ShiftedAckley_F3();
		protected:
			void initialize();
		};
	}
	using CEC2013_LSOP_F3 = CEC2013::ShiftedAckley_F3;
}
#endif

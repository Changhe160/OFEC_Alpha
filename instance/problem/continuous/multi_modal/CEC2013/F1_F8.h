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
*  Paper: Benchmark Functions for CEC¡¯2013 Special Session and Competition on Niching
*  Methods for Multimodal Function Optimization.
*******************************************************************************************/


#ifndef OFEC_F1_F8_H
#define OFEC_F1_F8_H

#include "../classical/five_uneven_peak_trap.h"
#include "../classical/equal_maxima.h"
#include "../classical/uneven_de_maxima.h"
#include "../classical/himmenblau.h"
#include "../classical/six_hump_camel_back.h"
#include "../classical/shubert.h"
#include "../classical/vincent.h"
#include "../classical/modified_rastrigin.h"

namespace OFEC {
	namespace CEC2013 {
		using F1_five_uneven_peak_trap = five_uneven_peak_trap;
		using F2_equal_maxima = equal_maxima;
		using F3_uneven_de_maxima = uneven_de_maxima;
		using F4_himmenblau = himmenblau;
		using F5_six_hump_camel_back = six_hump_camel_back;
		using F6_shubert = shubert;
		using F7_vincent = vincent;
		using F8_modified_rastrigin = modified_rastrigin;
	}
	using CEC2013_MMP_F1 = CEC2013::F1_five_uneven_peak_trap;
	using CEC2013_MMP_F2 = CEC2013::F2_equal_maxima;
	using CEC2013_MMP_F3 = CEC2013::F3_uneven_de_maxima;
	using CEC2013_MMP_F4 = CEC2013::F4_himmenblau;
	using CEC2013_MMP_F5 = CEC2013::F5_six_hump_camel_back;
	using CEC2013_MMP_F6 = CEC2013::F6_shubert;
	using CEC2013_MMP_F7 = CEC2013::F7_vincent;
	using CEC2013_MMP_F8 = CEC2013::F8_modified_rastrigin;
}
#endif // !OFEC_F1_F8_H
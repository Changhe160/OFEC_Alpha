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
}
#endif // !OFEC_F1_F8_H
/*************************************************************************
* Project:Open Frameworks for Evolutionary Computation (OFEC)
*************************************************************************
* Author: Changhe Li
* Email: changhe.lw@gmail.com 
* Language: C++
*************************************************************************
*  This file is part of OFEC. This library is free software;
*  you can redistribute it and/or modify it under the terms of the
*  GNU General Public License as published by the Free Software
*  Foundation; either version 2, or (at your option) any later version.
*************************************************************************/

#include "FParEst_FMSoundWaves.h"
namespace OFEC {
	FParEst_FMSoundWaves::FParEst_FMSoundWaves(param_map &v) :problem((v[param_proName]), 6, 1), \
		function((v[param_proName]), 6, 1) {
		set_range(-6.4, 6.35);
		set_init_range(-6.4, 6.35);
		initialize();

	}
	FParEst_FMSoundWaves::FParEst_FMSoundWaves(const std::string &name, size_t size_var, size_t size_obj) :problem(name, 6, 1), \
		function(name, 6, 1) {
		set_range(-6.4, 6.35);
		set_init_range(-6.4, 6.35);
		initialize();
	}


	void FParEst_FMSoundWaves::initialize() {
		std::vector<real> v(m_variable_size);
		v[0] = 1.0; v[1] = 5.0; v[2] = 1.5; v[3] = 4.8; v[4] = 2.0; v[5] = 4.9;

		set_original_global_opt(v.data());
		m_optima = m_original_optima;
	}

	void FParEst_FMSoundWaves::evaluate__(real *x, std::vector<real>& obj) {

		double theta = 2 * OFEC_PI / 100.;
		real s = 0, t;

		for (int i = 0; i < 100; i++) {
			t = x[0] * sin(x[1] * i*theta + x[2] * sin(x[3] * i*theta + x[4] * sin(x[5] * i*theta))) - sin(5.*i*theta + 1.5*sin(4.8*i*theta + 2.0*sin(4.9*i*theta)));
			s += t*t;
		}
		obj[0] = s;

	}
}
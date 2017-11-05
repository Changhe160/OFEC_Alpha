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
******************************************************************************************
*  Paper; A sequential niching memetic algorithm for continuous multimodal
*		  Appled Mathematics and Computation 218(2012) 8242-8259
*******************************************************************************************/
/*******************************************************************************************
*  Paper: A sequential niching memetic algorithm for continuous multimodal
*		  Appled Mathematics and Computation 218(2012) 8242-8259
****************************************************************************************/

#include "modified_rastrigin.h"
namespace OFEC {
	
	modified_rastrigin::modified_rastrigin(param_map &v) : problem((v[param_proName]), (v[param_numDim]), 1), \
		function((v[param_proName]), (v[param_numDim]), 1), m_k(2) {
		v[param_numDim] = 2;
		set_range(0.0, 1.); // note
		initialize();
	}
	modified_rastrigin::modified_rastrigin(const std::string &name, size_t size_var, size_t size_obj) : problem(name, size_var, size_obj), \
		function(name, size_var, size_obj), m_k(2) {
		set_range(0.0, 1.0); // note
		initialize();
	}

	void modified_rastrigin::initialize() { // note
		m_k[0] = 3; m_k[1] = 4;
		m_objective_accuracy = 0.1;
		m_variable_accuracy = 1.e-5;

		m_original_optima.set_number_variable(12); //production of m_ki
		std::ifstream in;
		std::stringstream ss;
		std::vector<std::vector<real>> var_data(12, std::vector<real>(m_variable_size));
		ss << global::ms_arg[param_workingDir] << "instance/problem/continuous/global/classical/data/" << m_name << "_Opt_" << m_variable_size << "Dim.txt";
		in.open(ss.str());
		if (in.fail()) {
			throw myexcept("cannot open data file@modified_rastrigin::initialize()");
		}
		for (int i = 0; i < 12; ++i) {
			double x0, x1;
			in >> x0 >> x1;
			var_data[i][0] = x0;
			var_data[i][1] = x1;
		}
		in.close();
		for (auto &i : var_data) {
			set_original_global_opt(i.data());
		}
		m_optima = m_original_optima;
		add_tag(problem_tag::MMP);
		//setObjSet();
	}
	void modified_rastrigin::evaluate__(real *x, std::vector<real>& obj) {
		double s = 0;

		for (int i = 0; i < m_variable_size; ++i) {
			s += 10 + 9 * cos(2 * OFEC_PI*m_k[i] * x[i]);
		}
		obj[0] = s + m_bias;  // note

	}
	
}
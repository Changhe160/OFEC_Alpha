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
*  Paper: Multimodal Optimization by Means of a Topological Species Conservation Algorithm
*		  IEEE TRANSACTIONS ON EVOLUTIONARY COMPUTATION, VOL.14,NO.6,DECEMBER 2010
*******************************************************************************************/

#include "center_peak.h"
namespace OFEC {
	
	center_peak::center_peak(param_map &v) :problem((v[param_proName]), (v[param_numDim]), 1), \
		function((v[param_proName]), (v[param_numDim]), 1) {
		v[param_numDim] = 2;
		set_range(-2, 2);
		set_init_range(-2, 2);
		initialize();
	}
	center_peak::center_peak(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), \
		function(name, size_var, size_obj) {

		set_range(-2, 2);
		set_init_range(-2, 2);
		initialize();
	}

	void center_peak::initialize() {
		m_opt_mode[0] = optimization_mode::Maximization;

		m_original_optima.set_number_variable(5); //1 gopt + 4 lopt
		m_objective_accuracy = 0.2;
		m_variable_accuracy = 1.e-5;

		std::ifstream in;
		std::stringstream ss;
		std::vector<std::vector<real>> var_data(5, std::vector<real>(m_variable_size));
		ss << global::ms_arg[param_workingDir] << "instance/problem/continuous/global/classical/data/" << m_name << "_Opt_" << m_variable_size << "Dim.txt";
		in.open(ss.str().c_str());
		if (!in)		throw myexcept("cannot open data file@center_peak::initialize()");

		for (int i = 0; i < 5; ++i) {
			double x0, x1, obj;
			in >> x0 >> x1 >> obj;
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
	void center_peak::evaluate__(real *x, std::vector<real>& obj) {

		double s;
		s = 3 * sin(0.5*OFEC_PI*x[0] + 0.5*OFEC_PI)*(2 - sqrt(x[0] * x[0] + x[1] * x[1])) / 4;
		obj[0] = s + m_bias;

	}
	
}
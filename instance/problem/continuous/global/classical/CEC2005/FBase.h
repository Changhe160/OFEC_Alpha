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

#ifndef OFEC_FBASE_H
#define OFEC_FBASE_H

#include "../../../../../../core/problem/continuous/function.h"

namespace OFEC {

	class FBase : public function
	{
	public:
		FBase(const std::string &name, size_t size_var, size_t size_obj);
		~FBase();
		size_t get_num_function();
		function* get_function(size_t num);
	protected:
		virtual void initialize() = 0;
		void evaluate__(real *x, std::vector<real>& obj);
		void compute_fmax();
		virtual void set_function() = 0;
		virtual void set_weight(std::vector<double>& w, const std::vector<real>&x);
		
		bool load_translation(const string &path);
		virtual void set_translation();
		bool load_rotation(const string &path);
		virtual void set_rotation();
	protected:
		size_t m_num_function;                // number of basic functions, for hybrid functions
		std::vector<function*> m_function;    // the functions
		std::vector<real> m_height;
		std::vector<real> m_fmax;
		double m_height_normalize_severity;   // constant number for noralizing all basic function with similar height
		std::vector<double> m_converge_severity;     // severity of converge range for each function
		std::vector<double> m_stretch_severity;      // severity of stretching original function, greater than 1 for stretch

	};
	template<typename T> problem * create_function(const std::string &name, size_t size_var, size_t size_obj) {
		return new T(name, size_var, size_obj);
	}
	typedef problem *(*pFun)(const std::string &name, size_t size_var, size_t size_obj);
	typedef std::vector<pFun> basic_func;
}
#endif // ! OFEC_FBASE_H

/******************************************************************************
* Project:Open Frameworks for Evolutionary Computation (OFEC)
*******************************************************************************
* Author: Changhe Li
* Email: changhe.lw@gmail.com
* Language: C++
*-------------------------------------------------------------------------------
*  This file is part of OFEC. This library is free software;
*  you can redistribute it and/or modify it under the terms of the
*  GNU General Public License as published by the Free Software
*  Foundation; either version 2, or (at your option) any later version.
*
*  see https://github.com/Changhe160/OFEC for more information
*
*-------------------------------------------------------------------------------
* class function defines basic operations for function/numerical optimization problems
*
*********************************************************************************/

#ifndef OFEC_FUNCTION_H
#define OFEC_FUNCTION_H

#include "continuous.h"
#include "../../../utility/matrix.h"
namespace OFEC {

	class function :public continuous {
	public:
		function(const std::string &name, size_t size_var, size_t size_obj = 1);

		void set_bias(double val);
		void set_scale(double val);
		void set_rotation_flag(bool flag);
		void set_tranlation_flag(bool flag);

		real translation(size_t i) const;
		std::vector<real>& translation();
		matrix& rotation();
		double condition_number();
		void set_condition_number(double c);
		evaluation_tag evaluate_(base &s, caller call, bool effective_fes, bool constructed);
		optima<variable<real>, real>& get_original_optima();
	protected:
		virtual void clear();
		function& operator =(const function &);
		function& operator =(function &&);

		void translate_zero();		
		
		virtual void initialize() {};
		bool load_translation();
		virtual void load_translation_(const string &path);
		virtual void set_translation();

		bool load_rotation();
		virtual void load_rotation_(const string &path);
		virtual void set_rotation();
		virtual void evaluate__(real *x, vector<real>& obj) = 0;

		void resize_translation(size_t n);
		void resize_rotation(size_t n);

		void irregularize(real *x);
		void asyemmetricalize(real *x, double belta);		
		void translate(real *x);
		void rotate(real *x);
		void scale(real *x);
	
	protected:
		std::vector<real> m_translation;
		bool m_scale_flag=false, m_rotation_flag=false, m_translation_flag=false, m_noise_flag = false;
		real m_scale, m_bias;
		real m_condition_number;
		matrix m_rotation;
		optima<variable<real>, real> m_original_optima;
	};
}
#endif // !OFEC_FUNCTION_H

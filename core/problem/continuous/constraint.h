/******************************************************************************
* Project:Open Frameworks for Evolutionary Computation (OFEC)
*******************************************************************************
* Author: Changhe Li and Li Zhou
* Email: changhe.lw@gmail.com, 441837060@qq.com
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

#ifndef OFEC_CONSTRAINT_H
#define OFEC_CONSTRAINT_H

#include "continuous.h"
#include "../../../utility/matrix.h"
namespace OFEC {

	class constraint :public continuous {
	public:
		constraint(const std::string &name, size_t size_var, size_t size_obj = 1);

		
		void set_bias(double val);
		void set_scale(double val);
		void set_rotation_flag(bool flag);
		void set_tranlation_flag(bool flag);

		real translation(size_t i) const;
		std::vector<real>& translation();
		std::vector<matrix>& rotation();
		double condition_number();
		void set_condition_number(double c);
		evaluation_tag evaluate_(base &s, caller call, bool effective_fes, bool constructed);
		optima<variable<real>, real>& get_original_optima();
		void set_global_opt(real *tran = 0);
		void set_original_global_opt(real *opt = 0);
	protected:
		virtual void clear();
		constraint& operator =(const constraint &);
		constraint& operator =(constraint &&);

		void translate_zero();

		virtual void initialize() {};
		virtual bool load_translation(const string &path);
		virtual bool load_translation_(const string &path);
		virtual void set_translation(const std::vector<real>& opt_var);

		virtual bool load_rotation(const string &path);
		virtual void load_rotation_(const string &path);
		virtual void set_rotation();
		virtual void evaluate__(real *x, std::vector<real>& obj, double & cons) = 0;

		void resize_translation(size_t n);
		void resize_rotation(size_t n);

		void irregularize(real *x);
		void asyemmetricalize(real *x, double belta);
		void translate(real *x);
		void translate_origin(real *x);
		void rotate(real *x, size_t num=1);
		void scale(real *x);

		void translate_back(real *x);

	protected:
		std::vector<real> m_translation;
		bool m_scale_flag = false, m_rotation_flag = false, m_translation_flag = false, m_noise_flag = false;
		real m_scale, m_bias;
		real m_condition_number;
		std::vector<matrix> m_rotation;
		optima<variable<real>, real> m_original_optima;
		double m_epsilon = 1e-4;
	};
	
}
#endif // !OFEC_CONSTRAINT_H
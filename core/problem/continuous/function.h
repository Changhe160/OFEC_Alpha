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

	template<typename ObjetiveCompare = objective_compare<real>>
	class function :public continuous {
	public:
		function(const std::string &name, size_t size_var, size_t size_obj = 1);

		void set_bias(double val);
		void set_scale(double val);
		void set_rotation_flag(bool flag);
		void set_tranlation_flag(bool flag);

		real translation(size_t i) const;
		std::vector<real>& translation();
		matrix& rotation(size_t i = 0);
		double condition_number();
		void set_condition_number(double c);
		evaluation_tag evaluate_(base &s, caller call, bool effective_fes, bool constructed);
		optima<variable<real>, real>& get_original_optima();
	protected:
		virtual void clear();
		function& operator =(const function &);
		function& operator =(function &&);

		void translate_zero();		
		virtual void transform(double * x);

		virtual void initialize() {};
		bool load_translation();
		virtual void load_translation_(const string &path);
		virtual void set_translation();

		bool load_rotation();
		virtual void load_rotation_(const string &path);
		virtual void set_rotation();
		virtual void evaluate__(real *x, vector<real>& obj) = 0;

		void irregularize(real *x);
		void asyemmetricalize(real *x, double belta);
		
	protected:
		std::vector<real> m_translation;
		bool m_scale_flag=false, m_rotation_flag=false, m_translation_flag=false, m_noise_flag = false;
		real m_scale, m_bias;
		real m_condition_number;
		std::vector<matrix> m_rotation;
		optima<variable<real>, real> m_original_optima;
	};

	template<typename ObjetiveCompare>
	function<ObjetiveCompare>::function(const std::string &name, size_t size_var, size_t size_obj) :continuous(name, size_var, size_obj) {

	}
	template<typename ObjetiveCompare>
	void function<ObjetiveCompare>::set_bias(double val) {
		m_bias = val;
	}
	template<typename ObjetiveCompare>
	void function<ObjetiveCompare>::set_scale(double val) {
		m_scale = val;
	}
	template<typename ObjetiveCompare>
	void function<ObjetiveCompare>::set_rotation_flag(bool flag) {
		m_rotation_flag = flag;
	}
	template<typename ObjetiveCompare>
	void function<ObjetiveCompare>::set_tranlation_flag(bool flag) {
		m_translation_flag = flag;
	}
	template<typename ObjetiveCompare>
	real function<ObjetiveCompare>::translation(size_t i) const {
		return m_translation[i];
	}
	template<typename ObjetiveCompare>
	std::vector<real>&  function<ObjetiveCompare>::translation() {
		return m_translation;
	}
	template<typename ObjetiveCompare>
	matrix& function<ObjetiveCompare>::rotation(size_t i) {
		return m_rotation[i];
	}
	template<typename ObjetiveCompare>
	double function<ObjetiveCompare>::condition_number() {
		return m_condition_number;
	}
	template<typename ObjetiveCompare>
	void function<ObjetiveCompare>::set_condition_number(double c) {
		m_condition_number = c;
	}
	template<typename ObjetiveCompare>
	evaluation_tag function<ObjetiveCompare>::evaluate_(base &s, caller call, bool effective_fes, bool constructed) {
		variable<real> &x = dynamic_cast< solution<variable<real>, real, ObjetiveCompare> &>(s).get_variable();
		auto & obj= dynamic_cast< solution<variable<real>, real, ObjetiveCompare> &>(s).get_objective();

		double *x_ = new double[m_numDim]; //for parallel running
		copy(x.begin(), x.end(), x_);

		transform(x_);
		evaluate__(x_, obj);
		delete[] x_;
		x_ = 0;
		if (constructed) {
			if (effective_fes)		m_effective_eval++;

			if (m_variable_monitor) {
				m_optima.is_optimal_variable(dynamic_cast<solution<variable<real>, real, ObjetiveCompare> &>(s), m_variable_accuracy);
				if (m_optima.is_variable_found()) 
					m_solved = true;
			}
			if (m_objective_monitor) {
				m_optima.is_optimal_objective(obj, m_objective_accuracy);
				if (m_optima.is_objective_found())
					m_solved = true;
			}
			if (call == caller::Algorithm&& global::ms_global->m_algorithm&&global::ms_global->m_algorithm->terminating())
				return evaluation_tag::Terminate;

			//if (mode == Program_Algorithm&&Global::msp_global->mp_problem && !Global::msp_global->mp_problem->isProTag(MOP)) m_globalOpt.isFound(s, m_disAccuracy, m_accuracy);
			//if (Global::msp_global != nullptr&&Global::msp_global->mp_algorithm != nullptr&&Global::msp_global->mp_algorithm->ifTerminating()) { return Return_Terminate; }
		}
		return evaluation_tag::Normal;
	}

}
#endif // !OFEC_FUNCTION_H

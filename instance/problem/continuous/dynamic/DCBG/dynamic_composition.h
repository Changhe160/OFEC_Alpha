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
* 1. Changhe Li, Shengxiang Yang, D. Pelta. Benchmark Generator for CEC'2012 Competition on Evolutionary Computation for
* Dynamic Optimization Problems,Tech. Rep., the School of Computer Science, China University of Geosciences, Wuhan, China, 2012.
*
*********************************************************************************/
// Created: 11 May 2011
// Last modified: 4 August 2018

#ifndef COMPOSITIONDBG_H
#define COMPOSITIONDBG_H

#include"../real_DBG.h"
namespace OFEC {
	class composition_DBG final: public real_DBG
	{
	public:
		enum ComDBGFuncID { COMDBG_SPHERE = 0, COMDBG_Begin= COMDBG_SPHERE,COMDBG_RASTRIGIN, COMDBG_GRIEWANK, COMDBG_ACKLEY, COMDBG_HYBRID, COMDBG_End};
		enum BasicProblem { Sphere = 0, Rastrigin, Griewank, Ackley, Weierstrass };
	private:
		std::vector<domain<real>> m_com_domain;				// boundary of component functions
		std::vector<real> m_converge_severity;						// severity of converge range for each function
		std::vector<real> m_stretch_severity;							// severity of stretching original function, greater than 1 for stretch
															// less than 1 for compress the original function
		real m_height_normalize_severity;					// constant number for noralizing all basic function with similar height
		std::vector<BasicProblem> m_basic_function;						// which basic function used to compose the composition function

		std::vector<real> m_fmax;
		static const int ms_num_basic_funs = 5;					// number of basic functions
		static thread_local ComDBGFuncID ms_fun_idx;
	private:

		void set_basic_fun_domain();
		void get_basic_fun_domain(const BasicProblem &f, real &l, real &u, const int rDimIdx = 0)const;
		
	public:
		void initialize();
		composition_DBG(const std::string &name, int rDimNumber,  int rNumPeaks, int frequency, ChangeType rT,  ComDBGFuncID rF, \
			 real rChangingRatio,  bool rFlagDimChange,  bool rFlagNumPeakChange, \
			 int peakNumChangeMode,  bool flagNoise,  bool flagTimelinkage);
		composition_DBG(param_map &v);
		void set_rotation_matrix();					//randomly generate rotation matrx for each basic function	
		void set_stretch_severity();	
		void set_fun_idx(ComDBGFuncID idx);
		void evaluate_objective(real* x, std::vector<real>& obj);
	protected:
		void copy(const problem &rDP);
		void change_random();
		void change_small_step();
		void change_large_step();
		void change_recurrent();
		void change_chaotic();
		void change_recurrent_noisy();
		void change_dimension();
		void change_num_peak();		
		void update_parameters();
	private:
		void repair(const BasicProblem &f, real *x);					// make x within search range after rotation											// basic five functions
		real fSphere(real *x);
		real fRastrigin(real *x);
		real fWeierstrass(real *x);
		real fGriewank(real *x);
		real fAckley(real *x);
		real selectFun(const BasicProblem &f, real *x);
	};
}
#endif // COMPOSITIONDBG_H






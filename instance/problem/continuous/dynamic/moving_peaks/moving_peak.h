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
* J. Branke, Memory enhanced evolutionary algorithms for changing optimization
* problems, Proc. of the 1999 Congr. on Evol. Comput, pp. 1875-1882, 1999.
*
*********************************************************************************/

// Created: 16 May 2013
// Last modified: 1 Agust 2018
#ifndef MOVINGPEAK_H
#define MOVINGPEAK_H

#include "../dynamic_continuous.h"
namespace OFEC {
	class  moving_peak final : public dynamic_continuous	{
	private:
		int m_F;
		real m_vlength; /* distance by which the peaks are moved, severity */

		/* lambda determines whether there is a direction of the movement, or whether
		they are totally random. For lambda = 1.0 each move has the same direction,
		while for lambda = 0.0, each move has a random direction */
		real m_lambda;
		real  m_standardHeight;
		/* width chosen randomly when standardwidth = 0.0 */
		real  m_standardWidth;
		std::vector<real> m_shift;
		std::vector<std::vector<real>>  m_pre_movement;/* to store every peak's previous movement */

	private:
		/* the following basis functions are provided :*/
		real constant_basis_func(const real *gen);
		real five_peak_basis_func(const real *gen);
		/* the following peak functions are provided: */
		real peak_function1(const real *gen, int peak_number);
		real peakFunctionCone(const real *gen, const int &peak_number);
		real peak_function_hilly(const real *gen, int peak_number);
		real peak_function_twin(const real  *gen, int peak_number);
		real select_function(const real  *gen, const int &peak_number);
		real dummy_eval(const real *gen);
	protected:
		void calculate_current_peak(const real *gen);
		void change_random();
		void change_num_peak();
		void copy(const problem & rP);
		void set_severity();
		void update_parameters();
	public:
		moving_peak(const std::string &name,int rDimNumber, int rNumPeaks, real rChangingRatio, int fre, real vlength, bool rFlagDimChange = false,\
			 bool rFlagNumPeakChange = false, int peakNumChangeMode = 1, bool flagNoise = false,  bool flagTimelinkage = false);
		moving_peak(param_map &v);

		void change_stepsize_random();
		void change_stepsize_linear();
		int get_right_peak();
		void set_vlength(real s);
		real get_vlength();
		void evaluate_objective(real* x, std::vector<real>& obj);
		void initialize();
	};

}
#endif // MOVINGPEAK_H

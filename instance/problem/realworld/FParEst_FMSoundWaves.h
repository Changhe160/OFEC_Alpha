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

/************************************************************************
parameter estimation for frequency-modulated (FM) sound waves is to estimate
the parameters of a FM synthesizer.

S.~Das and P.~N. Suganthan, ``Problem definitions and evaluation criteria for
  cec 2011 competition on testing evolutionary algorithms on real world
  optimization problem,'' Dept. of Electronics and Telecommunication Engg.,
  Jadavpur University, Kolkata, India, Tech. Rep., 2011.

*****************************************************************************************/


#ifndef OFEC_FPAREST_FMSOUNDWAVES_H
#define OFEC_FPAREST_FMSOUNDWAVES_H

#include "../../../core/problem/continuous/function.h"
namespace OFEC {
	class FParEst_FMSoundWaves : public function
	{
	public:
		FParEst_FMSoundWaves(param_map &v);
		FParEst_FMSoundWaves(const std::string &name, size_t size_var, size_t size_obj);
	protected:
		void initialize();
		void evaluate__(real *x, std::vector<real>& obj);
	private:
	};
}
#endif // FPAREST_FMSOUNDWAVES_H

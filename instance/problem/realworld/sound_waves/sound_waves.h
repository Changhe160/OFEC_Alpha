/********* Begin Register Information **********
{
	"name": "FM-sound-waves",
	"identifier": "SoundWaves",
	"tags": [ "continuous", "single-objective" ]
}
*********** End Register Information **********/

/*************************************************************************
* Project:Open Frameworks for Evolutionary Computation (OFEC)
*************************************************************************
* Author: Changhe Li and Li Zhou
* Email: changhe.lw@gmail.com, 441837060@qq.com
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
cec 2011 competition on testing evolutionary algorithms on Real world
optimization problem,'' Dept. of Electronics and Telecommunication Engg.,
Jadavpur University, Kolkata, India, Tech. Rep., 2011.
*****************************************************************************************/

#ifndef OFEC_FPAREST_FMSOUNDWAVES_H
#define OFEC_FPAREST_FMSOUNDWAVES_H

#include "../../../../core/problem/continuous/continuous.h"

namespace ofec {
	class SoundWaves : virtual public Continuous {
		OFEC_CONCRETE_INSTANCE(SoundWaves)
	protected:
		virtual void initialize_(Environment* env) override;
		void addInputParameters();
		void updateOptima(Environment* env) override;
		void evaluateObjective(Real *x, std::vector<Real>& obj) const override;
	};
}
#endif // FPAREST_FMSOUNDWAVES_H
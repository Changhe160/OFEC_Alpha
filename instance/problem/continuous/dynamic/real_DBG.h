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
// Last modified: 30 July 2018

#ifndef REALDBG_H
#define REALDBG_H

#include "dynamic_continuous.h"
#include "../../../../utility/matrix.h"
namespace OFEC {
	class real_DBG : public dynamic_continuous
	{
	protected:
		bool m_prediction;						// the next change of function can be predictable or not
		std::vector<matrix> m_rotM;				// orthogonal rotation matrixes for each function
		std::vector<std::vector<std::vector<int>>> m_rotP;		// save the planes rotated during one periodicity
	public:
		real_DBG(const std::string &name,  int rDimNumber,  int rNumPeaks,  int numObj, ChangeType rT, bool rFlagDimChange, bool rFlagNumPeakChange
			, int peakNumChangeMode, bool flagNoise, bool flagTimelinkage);
		void set_period(int rPeriod) final;
	protected:
		void repair(real *x);
		void change_height();
		void change_location(real angle);
		void copy(const problem &rP);	
		real  basic_change( ChangeType T,  real min,  real max);				
		void restore();

	};
}
#endif // REALDBG_H

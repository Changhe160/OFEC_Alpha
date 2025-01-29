/******************************************************************************
* Project:Open Frameworks for Evolutionary Computation (OFEC)
*******************************************************************************
* Author: Yiya Diao & Junchen Wang & Changhe Li
* Email: diaoyiyacug@gmail.com & wangjunchen.chris@gmail.com & changhe.lw@gmail.com
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
* macros used in OFEC
*
*
*********************************************************************************/

#ifndef OFEC_DEFINITION_H
#define OFEC_DEFINITION_H

#include <cmath>

namespace ofec {

#if !defined(NOMINMAX) 
#  define NOMINMAX
#endif

#define OFEC_PI std::acos(-1.0)

#define OFEC_E std::exp(1.0)

#ifdef USING_FLOAT                     
	using Real = float;						// set precision type to float
#else
	using Real = double;				  // set precision type to double
#endif

	enum class Dominance { 
		kEqual, 
		kDominant, 
		kDominated, 
		kNonDominated,
		kNonComparable 
	};

	enum class OptimizeMode { 
		kMinimize, 
		kMaximize 
	};

	enum class Violation { 
		kConstraint, 
		kBoundary, 
		kNone 
	};

	enum class Constraint{ 
		kInequality,
		kEquality 
	};

	enum class Validation { 
		kIgnore, 
		kReinitialize,
		kRemap,
		kSetToBound 
	};

	/*using the binary coding to record the evaluation tag:
	   (cur_tag & kTerminate) == true, judge whether cur_tag has kTerminate tag;
	   cur_tag |= kTerminate, insert the kTerminate tag into cur_tag
	   cur_tag ^= kTerminate, erase  the kTerminate tag into cur_tag

	   */
	enum EvaluationTag {
		kNormalEval = 1 << 1,
		kTerminate = 1 << 2,
		kChangeNextEval = 1 << 3,
		kChangeCurEval = 1 << 4,
		kChangeObjectiveMemory = 1 << 5,
		kChangeVariableMemory = 1 << 6
	};
}
#endif // !OFEC_DEFINITION_H
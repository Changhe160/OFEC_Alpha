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
* macros used in OFEC
*
*
*********************************************************************************/

#ifndef OFEC_DEFINITION_H
#define OFEC_DEFINITION_H

namespace OFEC {
#define OFEC_PI acos(-1.0) 
#define OFEC_E exp(1.0)  

#define USING_DOUBLE                    // elements of type double
	//#define USING_FLOAT                   // elements of type float
	//#define OFEC_DEMON
    //#define USING_CONCURRENT
	#define OFEC_CONSOLE
	//#define OFEC_UNIT_TEST

#ifdef USING_FLOAT                      // set precision type to float
	using real = float;
#endif

#ifdef USING_DOUBLE                     // set precision type to double
	using real = double;
#endif

	enum class dominationship { Equal, Dominating, Dominated, Non_dominated, Non_comparable };
	enum class optimization_mode { Minimization, Maximization };
	enum class caller { Problem, Algorithm, Demontration };
	enum class violation_type { Constraint, Boundary, None };
	enum class evaluation_tag {
		Normal, Change, Terminate, Change_next_eval, Change_timelinkage,
		Change_dimension, Infeasible
	};
	//SOP: single objective problem
	//MOP: multi-objective problem
	//DOP: dynamic optimization problem
	//MMOP: multi-modal optimization problem
	//GOP: global optimization problem
	//ROOT: robust optimzation problem
	//ConOP: continuous optimization problem
	//ComOP: combinatorial optimization problem
	//TSP: travelling salesman problem
	//COP: constraint optimization problem
	//VRP: vehicle routing problem
	//TTP: timetabling problem
	//JSP: job shop problem
	//KOP: knapsack optimization problem
	//SAT: boolean satisfiability problem
	//ONEMAX: one max problem
	//QAP: quadratic assignment problem
	//MKP: multi-dimensional knapsack problem
	//EOP: expensive optimization problem
	//LSOP: large scale optimization problem
	enum class problem_tag {null_tag,
		SOP, MOP, DOP, MMOP, GOP, ROOT, ConOP, ComOP, TSP, COP, VRP, TTP, JSP,
		KOP, SAT, ONEMAX, QAP, MKP, EOP, LSOP, epanet
	};
}
#endif // !OFEC_DEFINITION_H
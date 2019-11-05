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

#include <cmath>

namespace OFEC {
#define OFEC_PI std::acos(-1.0)
#define OFEC_E std::exp(1.0)

    //#define USING_FLOAT                   // elements of type float
    //#define USING_CONCURRENT

#ifdef USING_FLOAT                     
	using real = float;						// set precision type to float
#else
	using real = double;				  // set precision type to double
#endif

	enum class dominationship { Equal, Dominating, Dominated, Non_dominated, Non_comparable };
	enum class optimization_mode { Minimization, Maximization };
	enum class caller { Begin, Problem= Begin, Algorithm, Demon, End};
	enum class violation_type { Constraint, Boundary, None };
	enum class constraint_type{ Inequality, Equality };
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
		KOP, SAT, ONEMAX, QAP, MKP, EOP, LSOP, epanet, DVRP
	};
	//for epanet
	enum class init_type { random, distance, k_means, be_visited };
}
#endif // !OFEC_DEFINITION_H

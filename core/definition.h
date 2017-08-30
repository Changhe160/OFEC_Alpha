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
	#define USING_EXCEPTION                     // for debug
	#define USING_DOUBLE                    // elements of type double
	//#define USING_FLOAT                   // elements of type float
	//#define USING_GPU
	//#define OFEC_DEMON
	#define OFEC_CONSOLE

#ifdef USING_GPU
	#undef USING_DOUBLE
	#define USING_FLOAT
#endif // USING_GPU

#ifdef USING_FLOAT                      // set precision type to float
	using real = float;
#endif

#ifdef USING_DOUBLE                     // set precision type to double
	using real = double;
#endif

	enum class dominationship { Equal, Dominating, Dominated, Non_dominated, Non_comparable };
	enum class optimization_mode {Minimization, Maximization};
	enum class caller {Problem, Algorithm,Demontration};
	enum class violation_type {Constraint, Boundary, None};
	enum class evaluation_tag {Normal, Problem_change, Terminate, Problem_change_next_eval, Change_timelinkage, 
		Change_dimension,Infeasible};
	enum class problem_tag { SOP, MOP, DOP, MMP, SMP, ROOT, CONT, COMB, TSP, COP, VRP, TTP, JSP, KOP, SAT, ONEMAX, QAP, MKP };
	//SOP: single objective problem
	//MOP: multi-objective problem
	//DOP: dynamic optimization problem
	//MMP: multi-modal problem
	//SMP: single(uni) modal problem
	//ROOT: robust optimzation problem
	//CONT: continuous optimization problem
	//COMB: combinatorial optimization problem
	//TSP: travelling salesman problem
	//COP: constraint optimization problem
	//VRP: vehicle routing problem
	//TTP: timetabling problem
	//JSP: job shop problem
	//KOP: knapsack optimization problem
	//SAT: boolean satisfiability problem
	//ONEMAX: One Max problem

}
#endif // !OFEC_DEFINITION_H


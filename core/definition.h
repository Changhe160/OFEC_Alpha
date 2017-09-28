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
	enum class problem_tag { SOP, MOP, DOP, MMP, GOP, ROOT, CONT, COMB, TSP, COP, VRP, TTP, JSP, KOP, SAT, ONEMAX, QAP, MKP };

	//SOP: single objective problem
	//MOP: multi-objective problem
	//DOP: dynamic optimization problem
	//MMP: multi-modal problem
	//GOP: global optimization problem
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

	enum parameter {
		param_numDim, param_numPeak, param_proName, param_algName, param_maxEvals, param_shiftLength, param_changeType, param_proId, \
		param_runId, param_algId, param_changeRatio, param_flagNumPeakChange, param_flagNumDimChange, param_peakNumChangeMode, param_flagNoise, \
		param_flagTimeLinkage, param_comDBGFunID, param_changeFre, param_noiseSeverity, param_timelinkageSeverity, param_popSize, param_evalCountFlag, \
		param_stepIndi, param_trainingTime, param_subPopSize, param_overlapDgre, param_convThreshold, param_timeWindow, param_sampleFre, param_gOptFlag, \
		param_workingDir, param_convFactor, param_numRun, param_numTask, param_exlRadius, param_solutionValidationMode, param_populationInitialMethod, \
		param_hibernatingRadius, param_minNumPopSize, param_numChange, param_numObj, param_case, param_resource4BestPop, param_dataFile1, \
		param_xoverProbability, param_mutProbability, param_proTag, param_numGOpt, param_noiseFlag, param_predicFlag, param_changeType2, \
		param_peaksPerBox, param_interTest1, param_interTest2, param_interTest3, param_interTest4, param_numBox, param_heightConfigMode, param_peakCenter, \
		param_numParetoRegion, param_validRadius, param_radius, param_jumpHeight, param_variableRelation, param_peakShape, param_divisionMode, \
		param_peakOffset, param_flagIrregular, param_flagAsymmetry, param_flagRotation, param_dataFile2, param_dataFile3, param_dataDirectory1, \
		param_maxIter, param_maxSucIter, param_epsilon, param_mutationSchemeDE, param_updateSchemeProbabilityLearning, param_beta, param_alpha, param_gamma,
		param_glstrture,param_maxRunTime
	};
}
#endif // !OFEC_DEFINITION_H


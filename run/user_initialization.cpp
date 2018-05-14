#include "user_initialization.h"

namespace OFEC {

	void register_parameter() {
		// ******* user-defined parameters ***

		// ******* default parameters ********
		global::ms_param.insert({ "AID","algId" });
		global::ms_param.insert({ "al","alpha" });
		global::ms_param.insert({ "AN","algName" });
		global::ms_param.insert({ "be","beta" });
		global::ms_param.insert({ "ca","case" });
		global::ms_param.insert({ "CDBGFID","comDBGFunID" });
		global::ms_param.insert({ "CF","changeFre" });
		global::ms_param.insert({ "CoF","convFactor" });
		global::ms_param.insert({ "CR","changeRatio" });
		global::ms_param.insert({ "CT","changeType" });
		global::ms_param.insert({ "CT2","changeType2" });
		global::ms_param.insert({ "CTH","convThreshold" });
		global::ms_param.insert({ "DD1","dataDirectory1" });
		global::ms_param.insert({ "DF1","dataFile1" });
		global::ms_param.insert({ "DF2","dataFile2" });
		global::ms_param.insert({ "DF3","dataFile3" });
		global::ms_param.insert({ "DM","divisionMode" });
		global::ms_param.insert({ "ECF","evalCountFlag" });
		global::ms_param.insert({ "ep","epsilon" });
		global::ms_param.insert({ "ER","exlRadius" });
		global::ms_param.insert({ "FA","flagAsymmetry" });
		global::ms_param.insert({ "FI","flagIrregular" });
		global::ms_param.insert({ "FN","flagNoise" });
		global::ms_param.insert({ "FNDC","flagNumDimChange" });
		global::ms_param.insert({ "FNPC","flagNumPeakChange" });
		global::ms_param.insert({ "FR","flagRotation" });
		global::ms_param.insert({ "FTL","flagTimeLinkage" });
		global::ms_param.insert({ "ga","gamma" });
		global::ms_param.insert({ "gOptFlag","gOptFlag" });
		global::ms_param.insert({ "GS","glstrture" });
		global::ms_param.insert({ "HCM","heightConfigMode" });
		global::ms_param.insert({ "HR","hibernatingRadius" });
		global::ms_param.insert({ "IT1","interTest1" });
		global::ms_param.insert({ "IT2","interTest2" });
		global::ms_param.insert({ "IT3","interTest3" });
		global::ms_param.insert({ "IT4","interTest4" });
		global::ms_param.insert({ "JH","jumpHeight" });
		global::ms_param.insert({ "ME","maxEvals" });
		global::ms_param.insert({ "MI","maxIter" });
		global::ms_param.insert({ "MNPS","minNumPopSize" });
		global::ms_param.insert({ "MP","mutProbability" });
		global::ms_param.insert({ "MSDE","mutationSchemeDE" });
		global::ms_param.insert({ "MSI","maxSucIter" });
		global::ms_param.insert({ "NB","numBox" });
		global::ms_param.insert({ "NC","numChange" });
		global::ms_param.insert({ "ND","numDim" });
		global::ms_param.insert({ "NF","noiseFlag" });
		global::ms_param.insert({ "NGO","numGOpt" });
		global::ms_param.insert({ "NO","numObj" });
		global::ms_param.insert({ "NP","numPeak" });
		global::ms_param.insert({ "NPR","numParetoRegion" });
		global::ms_param.insert({ "NR","numRun" });
		global::ms_param.insert({ "NS","noiseSeverity" });
		global::ms_param.insert({ "NSP","numSubPop" });
		global::ms_param.insert({ "NT","numTask" });
		global::ms_param.insert({ "OLD","overlapDgre" });
		global::ms_param.insert({ "PC","peakCenter" });
		global::ms_param.insert({ "PF","predicFlag" });
		global::ms_param.insert({ "PID","proId" });
		global::ms_param.insert({ "PIM","populationInitialMethod" });
		global::ms_param.insert({ "PkS","peakShape" });
		global::ms_param.insert({ "PN","proName" });
		global::ms_param.insert({ "PNCM","peakNumChangeMode" });
		global::ms_param.insert({ "POS","peakOffset" });
		global::ms_param.insert({ "PPB","peaksPerBox" });
		global::ms_param.insert({ "PS","popSize" });
		global::ms_param.insert({ "PT","proTag" });
		global::ms_param.insert({ "ra","radius" });
		global::ms_param.insert({ "RBP","resource4BestPop" });
		global::ms_param.insert({ "RID","runId" });
		global::ms_param.insert({ "SF","sampleFre" });
		global::ms_param.insert({ "SI","stepIndi" });
		global::ms_param.insert({ "SL","shiftLength" });
		global::ms_param.insert({ "SPS","subPopSize" });
		global::ms_param.insert({ "SVM","solutionValidationMode" });
		global::ms_param.insert({ "TLS","timelinkageSeverity" });
		global::ms_param.insert({ "TT","trainingTime" });
		global::ms_param.insert({ "TW","timeWindow" });
		global::ms_param.insert({ "USPL","updateSchemeProbabilityLearning" });
		global::ms_param.insert({ "VR","validRadius" });
		global::ms_param.insert({ "VR","variableRelation" });
		global::ms_param.insert({ "WD","workingDir" });
		global::ms_param.insert({ "XP","xoverProbability" });
	}

	void register_problem() {
		/*combination*/
		RIGIESTER(problem, multi_dimensional_knapsack, "MKP", std::set<problem_tag>({ problem_tag::MKP, problem_tag::ComOP }));
		RIGIESTER(problem, one_max, "One_Max", std::set<problem_tag>({ problem_tag::ONEMAX, problem_tag::ComOP }));
		RIGIESTER(problem, quadratic_assignment, "QAP", std::set<problem_tag>({ problem_tag::QAP, problem_tag::ComOP }));
		RIGIESTER(problem, travelling_salesman, "TSP", std::set<problem_tag>({ problem_tag::TSP, problem_tag::ComOP }));
		/*multi_objective*/
		RIGIESTER(problem, ZDT1, "ZDT1", std::set<problem_tag>({ problem_tag::MOP,problem_tag::ConOP }));
		RIGIESTER(problem, ZDT2, "ZDT2", std::set<problem_tag>({ problem_tag::MOP,problem_tag::ConOP }));
		RIGIESTER(problem, ZDT3, "ZDT3", std::set<problem_tag>({ problem_tag::MOP,problem_tag::ConOP }));
		RIGIESTER(problem, ZDT4, "ZDT4", std::set<problem_tag>({ problem_tag::MOP,problem_tag::ConOP }));
		RIGIESTER(problem, ZDT6, "ZDT6", std::set<problem_tag>({ problem_tag::MOP,problem_tag::ConOP }));
		RIGIESTER(problem, DTLZ1, "DTLZ1", std::set<problem_tag>({ problem_tag::MOP,problem_tag::ConOP }));
		RIGIESTER(problem, DTLZ2, "DTLZ2", std::set<problem_tag>({ problem_tag::MOP,problem_tag::ConOP }));
		RIGIESTER(problem, DTLZ3, "DTLZ3", std::set<problem_tag>({ problem_tag::MOP,problem_tag::ConOP }));
		RIGIESTER(problem, DTLZ4, "DTLZ4", std::set<problem_tag>({ problem_tag::MOP,problem_tag::ConOP }));
		RIGIESTER(problem, MOEA_F1, "MOEA_F1", std::set<problem_tag>({ problem_tag::MOP,problem_tag::ConOP }));
		RIGIESTER(problem, MOEA_F2, "MOEA_F2", std::set<problem_tag>({ problem_tag::MOP,problem_tag::ConOP }));
		RIGIESTER(problem, MOEA_F3, "MOEA_F3", std::set<problem_tag>({ problem_tag::MOP,problem_tag::ConOP }));
		RIGIESTER(problem, MOEA_F4, "MOEA_F4", std::set<problem_tag>({ problem_tag::MOP,problem_tag::ConOP }));
		RIGIESTER(problem, MOEA_F5, "MOEA_F5", std::set<problem_tag>({ problem_tag::MOP,problem_tag::ConOP }));
		RIGIESTER(problem, MOEA_F6, "MOEA_F6", std::set<problem_tag>({ problem_tag::MOP,problem_tag::ConOP }));
		RIGIESTER(problem, MOEA_F7, "MOEA_F7", std::set<problem_tag>({ problem_tag::MOP,problem_tag::ConOP }));
		RIGIESTER(problem, MOEA_F8, "MOEA_F8", std::set<problem_tag>({ problem_tag::MOP,problem_tag::ConOP }));
		RIGIESTER(problem, MOEA_F9, "MOEA_F9", std::set<problem_tag>({ problem_tag::MOP,problem_tag::ConOP }));
		/*BBOB*/
		RIGIESTER(problem, BBOB_F1, "BBOB_F01_Sphere", std::set<problem_tag>({ problem_tag::GOP,problem_tag::ConOP }));
		RIGIESTER(problem, BBOB_F2, "BBOB_F02_Ellipsoidal", std::set<problem_tag>({ problem_tag::GOP,problem_tag::ConOP }));
		RIGIESTER(problem, BBOB_F3, "BBOB_F03_Rastrigin", std::set<problem_tag>({ problem_tag::GOP,problem_tag::ConOP }));
		RIGIESTER(problem, BBOB_F4, "BBOB_F04_BucheRastrigin", std::set<problem_tag>({ problem_tag::GOP,problem_tag::ConOP }));
		RIGIESTER(problem, BBOB_F5, "BBOB_F05_Slope", std::set<problem_tag>({ problem_tag::GOP,problem_tag::ConOP }));
		RIGIESTER(problem, BBOB_F6, "BBOB_F06_Sector", std::set<problem_tag>({ problem_tag::GOP,problem_tag::ConOP }));
		RIGIESTER(problem, BBOB_F7, "BBOB_F07_StepEllipsoid", std::set<problem_tag>({ problem_tag::GOP,problem_tag::ConOP }));
		RIGIESTER(problem, BBOB_F8, "BBOB_F08_OriginalRosenbrock", std::set<problem_tag>({ problem_tag::GOP,problem_tag::ConOP }));
		RIGIESTER(problem, BBOB_F9, "BBOB_F09_RotatedRosenbrock", std::set<problem_tag>({ problem_tag::GOP,problem_tag::ConOP }));
		RIGIESTER(problem, BBOB_F10, "BBOB_F10_NonseparableEllipsoid", std::set<problem_tag>({ problem_tag::GOP,problem_tag::ConOP }));
		RIGIESTER(problem, BBOB_F11, "BBOB_F11_Discus", std::set<problem_tag>({ problem_tag::GOP,problem_tag::ConOP }));
		RIGIESTER(problem, BBOB_F12, "BBOB_F12_BentCigar", std::set<problem_tag>({ problem_tag::GOP,problem_tag::ConOP }));
		RIGIESTER(problem, BBOB_F13, "BBOB_F13_SharpRidge", std::set<problem_tag>({ problem_tag::GOP,problem_tag::ConOP }));
		RIGIESTER(problem, BBOB_F14, "BBOB_F14_DifferentPowers", std::set<problem_tag>({ problem_tag::GOP,problem_tag::ConOP }));
		RIGIESTER(problem, BBOB_F15, "BBOB_F15_NonseparableRastrigin", std::set<problem_tag>({ problem_tag::GOP,problem_tag::ConOP }));
		RIGIESTER(problem, BBOB_F16, "BBOB_F16_Weierstrass", std::set<problem_tag>({ problem_tag::GOP,problem_tag::ConOP }));
		RIGIESTER(problem, BBOB_F17, "BBOB_F17_SchaffersF7", std::set<problem_tag>({ problem_tag::GOP,problem_tag::ConOP }));
		RIGIESTER(problem, BBOB_F18, "BBOB_F18_IllconditionedSchaffersF7", std::set<problem_tag>({ problem_tag::GOP,problem_tag::ConOP }));
		RIGIESTER(problem, BBOB_F19, "BBOB_F19_CompositeGriewankRosenbrock", std::set<problem_tag>({ problem_tag::GOP,problem_tag::ConOP }));
		RIGIESTER(problem, BBOB_F20, "BBOB_F20_Schwefel", std::set<problem_tag>({ problem_tag::GOP,problem_tag::ConOP }));
		RIGIESTER(problem, BBOB_F21, "BBOB_F21_GallagherGaussian101mePeaks", std::set<problem_tag>({ problem_tag::GOP,problem_tag::ConOP }));
		RIGIESTER(problem, BBOB_F22, "BBOB_F22_GallagherGaussian21hiPeaks", std::set<problem_tag>({ problem_tag::GOP,problem_tag::ConOP }));
		RIGIESTER(problem, BBOB_F23, "BBOB_F23_Katsuura", std::set<problem_tag>({ problem_tag::GOP,problem_tag::ConOP }));
		RIGIESTER(problem, BBOB_F24, "BBOB_F24_LunacekBiRastrigin", std::set<problem_tag>({ problem_tag::GOP,problem_tag::ConOP }));
		/*classical_GOP*/
		RIGIESTER(problem, ackley, "ackley", std::set<problem_tag>({ problem_tag::GOP,problem_tag::ConOP }));
		RIGIESTER(problem, bent_cigar, "bent_cigar", std::set<problem_tag>({ problem_tag::GOP,problem_tag::ConOP }));
		RIGIESTER(problem, different_powers, "different_powers", std::set<problem_tag>({ problem_tag::GOP,problem_tag::ConOP }));
		RIGIESTER(problem, discus, "discus", std::set<problem_tag>({ problem_tag::GOP,problem_tag::ConOP }));
		RIGIESTER(problem, elliptic, "elliptic", std::set<problem_tag>({ problem_tag::GOP,problem_tag::ConOP }));
		RIGIESTER(problem, griewank, "griewank", std::set<problem_tag>({ problem_tag::GOP,problem_tag::ConOP }));
		RIGIESTER(problem, griewank_rosenbrock, "griewank_rosenbrock", std::set<problem_tag>({ problem_tag::GOP,problem_tag::ConOP }));
		RIGIESTER(problem, happy_cat, "happy_cat", std::set<problem_tag>({ problem_tag::GOP,problem_tag::ConOP }));
		RIGIESTER(problem, HGBat, "HGBat", std::set<problem_tag>({ problem_tag::GOP,problem_tag::ConOP }));
		RIGIESTER(problem, katsuura, "katsuura", std::set<problem_tag>({ problem_tag::GOP,problem_tag::ConOP }));
		RIGIESTER(problem, modified_schwefel, "modified_schwefel", std::set<problem_tag>({ problem_tag::GOP,problem_tag::ConOP }));
		RIGIESTER(problem, non_continuous_rastrigin, "non_continuous_rastrigin", std::set<problem_tag>({ problem_tag::GOP,problem_tag::ConOP }));
		RIGIESTER(problem, non_continuous_scaffer_F6, "non_continuous_scaffer_F6", std::set<problem_tag>({ problem_tag::GOP,problem_tag::ConOP }));
		RIGIESTER(problem, penalized_1, "penalized_1", std::set<problem_tag>({ problem_tag::GOP,problem_tag::ConOP }));
		RIGIESTER(problem, penalized_2, "penalized_2", std::set<problem_tag>({ problem_tag::GOP,problem_tag::ConOP }));
		RIGIESTER(problem, quartic_noisy, "quartic_noisy", std::set<problem_tag>({ problem_tag::GOP,problem_tag::ConOP }));
		RIGIESTER(problem, rastrigin, "rastrigin", std::set<problem_tag>({ problem_tag::GOP,problem_tag::ConOP }));
		RIGIESTER(problem, rosenbrock, "rosenbrock", std::set<problem_tag>({ problem_tag::GOP,problem_tag::ConOP }));
		RIGIESTER(problem, rotated_bent_cigar, "rotated_bent_cigar", std::set<problem_tag>({ problem_tag::GOP,problem_tag::ConOP }));
		RIGIESTER(problem, rotated_discus, "rotated_discus", std::set<problem_tag>({ problem_tag::GOP,problem_tag::ConOP }));
		RIGIESTER(problem, rotated_rosenbrock, "rotated_rosenbrock", std::set<problem_tag>({ problem_tag::GOP,problem_tag::ConOP }));
		RIGIESTER(problem, rotated_scaffer_F6, "rotated_scaffer_F6", std::set<problem_tag>({ problem_tag::GOP,problem_tag::ConOP }));
		RIGIESTER(problem, scaffer_F6, "scaffer_F6", std::set<problem_tag>({ problem_tag::GOP,problem_tag::ConOP }));
		RIGIESTER(problem, schwefel, "schwefel", std::set<problem_tag>({ problem_tag::GOP,problem_tag::ConOP }));
		RIGIESTER(problem, schwefel_1_2, "schwefel_1_2", std::set<problem_tag>({ problem_tag::GOP,problem_tag::ConOP }));
		RIGIESTER(problem, schwefel_2_6, "schwefel_2_6", std::set<problem_tag>({ problem_tag::GOP,problem_tag::ConOP }));
		RIGIESTER(problem, sphere, "sphere", std::set<problem_tag>({ problem_tag::GOP,problem_tag::ConOP }));
		RIGIESTER(problem, sphere_noisy, "sphere_noisy", std::set<problem_tag>({ problem_tag::GOP,problem_tag::ConOP }));
		RIGIESTER(problem, step, "step", std::set<problem_tag>({ problem_tag::GOP,problem_tag::ConOP }));
		RIGIESTER(problem, weierstrass, "weierstrass", std::set<problem_tag>({ problem_tag::GOP,problem_tag::ConOP }));
		/*CEC2005_GOP*/
		RIGIESTER(problem, CEC2005_GOP_F1, "F1_shifted_sphere", std::set<problem_tag>({ problem_tag::GOP,problem_tag::ConOP }));
		RIGIESTER(problem, CEC2005_GOP_F2, "F2_shifted_schwefel_1_2", std::set<problem_tag>({ problem_tag::GOP,problem_tag::ConOP }));
		RIGIESTER(problem, CEC2005_GOP_F3, "F3_shifted_rotated_high_cond_elliptic", std::set<problem_tag>({ problem_tag::GOP,problem_tag::ConOP }));
		RIGIESTER(problem, CEC2005_GOP_F4, "F4_shifted_schwefel_1_2_noisy", std::set<problem_tag>({ problem_tag::GOP,problem_tag::ConOP }));
		RIGIESTER(problem, CEC2005_GOP_F5, "F5_schwefel_2_6_bound", std::set<problem_tag>({ problem_tag::GOP,problem_tag::ConOP }));
		RIGIESTER(problem, CEC2005_GOP_F6, "F6_shifted_rosenbrock", std::set<problem_tag>({ problem_tag::GOP,problem_tag::ConOP }));
		RIGIESTER(problem, CEC2005_GOP_F7, "F7_shifted_rotated_griewank_no_bound", std::set<problem_tag>({ problem_tag::GOP,problem_tag::ConOP }));
		RIGIESTER(problem, CEC2005_GOP_F8, "F8_shifted_rotated_ackley_bound", std::set<problem_tag>({ problem_tag::GOP,problem_tag::ConOP }));
		RIGIESTER(problem, CEC2005_GOP_F9, "F9_shifted_rastrigin", std::set<problem_tag>({ problem_tag::GOP,problem_tag::ConOP }));
		RIGIESTER(problem, CEC2005_GOP_F10, "F10_shifted_rotated_rastrigin", std::set<problem_tag>({ problem_tag::GOP,problem_tag::ConOP }));
		RIGIESTER(problem, CEC2005_GOP_F11, "F11_shifted_rotated_weierstrass", std::set<problem_tag>({ problem_tag::GOP,problem_tag::ConOP }));
		RIGIESTER(problem, CEC2005_GOP_F12, "F12_schwefel_2_13", std::set<problem_tag>({ problem_tag::GOP,problem_tag::ConOP }));
		RIGIESTER(problem, CEC2005_GOP_F13, "F13_shifted_expanded_griewank_rosenbrock", std::set<problem_tag>({ problem_tag::GOP,problem_tag::ConOP }));
		RIGIESTER(problem, CEC2005_GOP_F14, "F14_shifted_rotated_expanded_scaffer_F6", std::set<problem_tag>({ problem_tag::GOP,problem_tag::ConOP }));
		RIGIESTER(problem, CEC2005_GOP_F15, "F15_hybrid_composition", std::set<problem_tag>({ problem_tag::GOP,problem_tag::ConOP }));
		RIGIESTER(problem, CEC2005_GOP_F16, "F16_rotated_hybrid_composition_F15", std::set<problem_tag>({ problem_tag::GOP,problem_tag::ConOP }));
		RIGIESTER(problem, CEC2005_GOP_F17, "F17_hybrid_composition_noisy", std::set<problem_tag>({ problem_tag::GOP,problem_tag::ConOP }));
		RIGIESTER(problem, CEC2005_GOP_F18, "F18_rotated_hybrid_composition", std::set<problem_tag>({ problem_tag::GOP,problem_tag::ConOP }));
		RIGIESTER(problem, CEC2005_GOP_F19, "F19_rotated_hybrid_narrow_basin", std::set<problem_tag>({ problem_tag::GOP,problem_tag::ConOP }));
		RIGIESTER(problem, CEC2005_GOP_F20, "F20_rotated_hybrid_bound", std::set<problem_tag>({ problem_tag::GOP,problem_tag::ConOP }));
		RIGIESTER(problem, CEC2005_GOP_F21, "F21_rotated_hybrid_composition", std::set<problem_tag>({ problem_tag::GOP,problem_tag::ConOP }));
		RIGIESTER(problem, CEC2005_GOP_F22, "F22_rotated_hybrid_high_condition_number_matrix", std::set<problem_tag>({ problem_tag::GOP,problem_tag::ConOP }));
		RIGIESTER(problem, CEC2005_GOP_F23, "F23_non_continuous_rotated_hybrid", std::set<problem_tag>({ problem_tag::GOP,problem_tag::ConOP }));
		RIGIESTER(problem, CEC2005_GOP_F24, "F24_rotated_hybrid_composition", std::set<problem_tag>({ problem_tag::GOP,problem_tag::ConOP }));
		RIGIESTER(problem, CEC2005_GOP_F25, "F25_rotated_hybrid_no_bound", std::set<problem_tag>({ problem_tag::GOP,problem_tag::ConOP }));
		/*CEC2013_large_scale*/
		RIGIESTER(problem, CEC2013_LSOP_F1, "ShiftedElliptic_F1", std::set<problem_tag>({ problem_tag::LSOP,problem_tag::ConOP }));
		RIGIESTER(problem, CEC2013_LSOP_F2, "ShiftedRastrigin_F2", std::set<problem_tag>({ problem_tag::LSOP,problem_tag::ConOP }));
		RIGIESTER(problem, CEC2013_LSOP_F3, "ShiftedAckley_F3", std::set<problem_tag>({ problem_tag::LSOP,problem_tag::ConOP }));
		RIGIESTER(problem, CEC2013_LSOP_F4, "N7S1_SR_Elliptic_F4", std::set<problem_tag>({ problem_tag::LSOP,problem_tag::ConOP }));
		RIGIESTER(problem, CEC2013_LSOP_F5, "N7S1_SR_Rastrigin_F5", std::set<problem_tag>({ problem_tag::LSOP,problem_tag::ConOP }));
		RIGIESTER(problem, CEC2013_LSOP_F6, "N7S1_SR_Ackley_F6", std::set<problem_tag>({ problem_tag::LSOP,problem_tag::ConOP }));
		RIGIESTER(problem, CEC2013_LSOP_F7, "N7S1_ShiftedSchwefel_F7", std::set<problem_tag>({ problem_tag::LSOP,problem_tag::ConOP }));
		RIGIESTER(problem, CEC2013_LSOP_F8, "N20_SR_Elliptic_F8", std::set<problem_tag>({ problem_tag::LSOP,problem_tag::ConOP }));
		RIGIESTER(problem, CEC2013_LSOP_F9, "N20_SR_Rastrigin_F9", std::set<problem_tag>({ problem_tag::LSOP,problem_tag::ConOP }));
		RIGIESTER(problem, CEC2013_LSOP_F10, "N20_SR_Ackley_F10", std::set<problem_tag>({ problem_tag::LSOP,problem_tag::ConOP }));
		RIGIESTER(problem, CEC2013_LSOP_F11, "N20_ShiftedSchwefel_F11", std::set<problem_tag>({ problem_tag::LSOP,problem_tag::ConOP }));
		RIGIESTER(problem, CEC2013_LSOP_F12, "ShiftedRosenbrock_F12", std::set<problem_tag>({ problem_tag::LSOP,problem_tag::ConOP }));
		RIGIESTER(problem, CEC2013_LSOP_F13, "ConformingOS_ShiftedSchwefel_F13", std::set<problem_tag>({ problem_tag::LSOP,problem_tag::ConOP }));
		RIGIESTER(problem, CEC2013_LSOP_F14, "ConflictingOS_ShiftedSchwefel_F14", std::set<problem_tag>({ problem_tag::LSOP,problem_tag::ConOP }));
		RIGIESTER(problem, CEC2013_LSOP_F15, "ShiftedSchwefel_F15", std::set<problem_tag>({ problem_tag::LSOP,problem_tag::ConOP }));
		/*CEC2015_GOP*/
		RIGIESTER(problem, CEC2015_GOP_F1, "F1_R_elliptic", std::set<problem_tag>({ problem_tag::GOP,problem_tag::ConOP }));
		RIGIESTER(problem, CEC2015_GOP_F2, "F2_R_cigar", std::set<problem_tag>({ problem_tag::GOP,problem_tag::ConOP }));
		RIGIESTER(problem, CEC2015_GOP_F3, "F3_SR_ackley", std::set<problem_tag>({ problem_tag::GOP,problem_tag::ConOP }));
		RIGIESTER(problem, CEC2015_GOP_F4, "F4_SR_rastrigin", std::set<problem_tag>({ problem_tag::GOP,problem_tag::ConOP }));
		RIGIESTER(problem, CEC2015_GOP_F5, "F5_SR_schwefel", std::set<problem_tag>({ problem_tag::GOP,problem_tag::ConOP }));
		RIGIESTER(problem, CEC2015_GOP_F6, "F6_hybrid1", std::set<problem_tag>({ problem_tag::GOP,problem_tag::ConOP }));
		RIGIESTER(problem, CEC2015_GOP_F7, "F7_hybrid2", std::set<problem_tag>({ problem_tag::GOP,problem_tag::ConOP }));
		RIGIESTER(problem, CEC2015_GOP_F8, "F8_hybrid3", std::set<problem_tag>({ problem_tag::GOP,problem_tag::ConOP }));
		RIGIESTER(problem, CEC2015_GOP_F9, "F9_global_composition1", std::set<problem_tag>({ problem_tag::GOP,problem_tag::ConOP }));
		RIGIESTER(problem, CEC2015_GOP_F10, "F10_global_composition2", std::set<problem_tag>({ problem_tag::GOP,problem_tag::ConOP }));
		RIGIESTER(problem, CEC2015_GOP_F11, "F11_global_composition3", std::set<problem_tag>({ problem_tag::GOP,problem_tag::ConOP }));
		RIGIESTER(problem, CEC2015_GOP_F12, "F12_global_composition4", std::set<problem_tag>({ problem_tag::GOP,problem_tag::ConOP }));
		RIGIESTER(problem, CEC2015_GOP_F13, "F13_global_composition5", std::set<problem_tag>({ problem_tag::GOP,problem_tag::ConOP }));
		RIGIESTER(problem, CEC2015_GOP_F14, "F14_global_composition6", std::set<problem_tag>({ problem_tag::GOP,problem_tag::ConOP }));
		RIGIESTER(problem, CEC2015_GOP_F15, "F15_global_composition7", std::set<problem_tag>({ problem_tag::GOP,problem_tag::ConOP }));
		/*CEC2015_EOP*/
		RIGIESTER(problem, CEC2015_EOP_F1, "F1_rotated_bent_cigar", std::set<problem_tag>({ problem_tag::EOP,problem_tag::ConOP }));
		RIGIESTER(problem, CEC2015_EOP_F2, "F2_rotated_discus", std::set<problem_tag>({ problem_tag::EOP,problem_tag::ConOP }));
		RIGIESTER(problem, CEC2015_EOP_F3, "F3_shifted_rotated_weierstrass", std::set<problem_tag>({ problem_tag::EOP,problem_tag::ConOP }));
		RIGIESTER(problem, CEC2015_EOP_F4, "F4_shifted_rotated_schwefel", std::set<problem_tag>({ problem_tag::EOP,problem_tag::ConOP }));
		RIGIESTER(problem, CEC2015_EOP_F5, "F5_shifted_rotated_katsuura", std::set<problem_tag>({ problem_tag::EOP,problem_tag::ConOP }));
		RIGIESTER(problem, CEC2015_EOP_F6, "F6_shifted_rotated_happy_cat", std::set<problem_tag>({ problem_tag::EOP,problem_tag::ConOP }));
		RIGIESTER(problem, CEC2015_EOP_F7, "F7_shifted_rotated_HGBat", std::set<problem_tag>({ problem_tag::EOP,problem_tag::ConOP }));
		RIGIESTER(problem, CEC2015_EOP_F8, "F8_shifted_rotated_griewank_rosenbrock", std::set<problem_tag>({ problem_tag::EOP,problem_tag::ConOP }));
		RIGIESTER(problem, CEC2015_EOP_F9, "F9_shifted_rotated_scaffer_F6", std::set<problem_tag>({ problem_tag::EOP,problem_tag::ConOP }));
		RIGIESTER(problem, CEC2015_EOP_F10, "F10_hybrid1", std::set<problem_tag>({ problem_tag::EOP,problem_tag::ConOP }));
		RIGIESTER(problem, CEC2015_EOP_F11, "F11_hybrid2", std::set<problem_tag>({ problem_tag::EOP,problem_tag::ConOP }));
		RIGIESTER(problem, CEC2015_EOP_F12, "F12_hybrid3", std::set<problem_tag>({ problem_tag::EOP,problem_tag::ConOP }));
		RIGIESTER(problem, CEC2015_EOP_F13, "F13_expensive_composition1", std::set<problem_tag>({ problem_tag::EOP,problem_tag::ConOP }));
		RIGIESTER(problem, CEC2015_EOP_F14, "F14_expensive_composition2", std::set<problem_tag>({ problem_tag::EOP,problem_tag::ConOP }));
		RIGIESTER(problem, CEC2015_EOP_F15, "F15_expensive_composition3", std::set<problem_tag>({ problem_tag::EOP,problem_tag::ConOP }));
		/*CEC2017_COP*/
		RIGIESTER(problem, CEC2017_COP_F01, "C01", std::set<problem_tag>({ problem_tag::COP,problem_tag::ConOP }));
		RIGIESTER(problem, CEC2017_COP_F02, "C02", std::set<problem_tag>({ problem_tag::COP,problem_tag::ConOP }));
		RIGIESTER(problem, CEC2017_COP_F03, "C03", std::set<problem_tag>({ problem_tag::COP,problem_tag::ConOP }));
		RIGIESTER(problem, CEC2017_COP_F04, "C04", std::set<problem_tag>({ problem_tag::COP,problem_tag::ConOP }));
		RIGIESTER(problem, CEC2017_COP_F05, "C05", std::set<problem_tag>({ problem_tag::COP,problem_tag::ConOP }));
		RIGIESTER(problem, CEC2017_COP_F06, "C06", std::set<problem_tag>({ problem_tag::COP,problem_tag::ConOP }));
		RIGIESTER(problem, CEC2017_COP_F07, "C07", std::set<problem_tag>({ problem_tag::COP,problem_tag::ConOP }));
		RIGIESTER(problem, CEC2017_COP_F08, "C08", std::set<problem_tag>({ problem_tag::COP,problem_tag::ConOP }));
		RIGIESTER(problem, CEC2017_COP_F09, "C09", std::set<problem_tag>({ problem_tag::COP,problem_tag::ConOP }));
		RIGIESTER(problem, CEC2017_COP_F10, "C10", std::set<problem_tag>({ problem_tag::COP,problem_tag::ConOP }));
		RIGIESTER(problem, CEC2017_COP_F11, "C11", std::set<problem_tag>({ problem_tag::COP,problem_tag::ConOP }));
		RIGIESTER(problem, CEC2017_COP_F12, "C12", std::set<problem_tag>({ problem_tag::COP,problem_tag::ConOP }));
		RIGIESTER(problem, CEC2017_COP_F13, "C13", std::set<problem_tag>({ problem_tag::COP,problem_tag::ConOP }));
		RIGIESTER(problem, CEC2017_COP_F14, "C14", std::set<problem_tag>({ problem_tag::COP,problem_tag::ConOP }));
		RIGIESTER(problem, CEC2017_COP_F15, "C15", std::set<problem_tag>({ problem_tag::COP,problem_tag::ConOP }));
		RIGIESTER(problem, CEC2017_COP_F16, "C16", std::set<problem_tag>({ problem_tag::COP,problem_tag::ConOP }));
		RIGIESTER(problem, CEC2017_COP_F17, "C17", std::set<problem_tag>({ problem_tag::COP,problem_tag::ConOP }));
		RIGIESTER(problem, CEC2017_COP_F18, "C18", std::set<problem_tag>({ problem_tag::COP,problem_tag::ConOP }));
		RIGIESTER(problem, CEC2017_COP_F19, "C19", std::set<problem_tag>({ problem_tag::COP,problem_tag::ConOP }));
		RIGIESTER(problem, CEC2017_COP_F20, "C20", std::set<problem_tag>({ problem_tag::COP,problem_tag::ConOP }));
		RIGIESTER(problem, CEC2017_COP_F21, "C21", std::set<problem_tag>({ problem_tag::COP,problem_tag::ConOP }));
		RIGIESTER(problem, CEC2017_COP_F22, "C22", std::set<problem_tag>({ problem_tag::COP,problem_tag::ConOP }));
		RIGIESTER(problem, CEC2017_COP_F23, "C23", std::set<problem_tag>({ problem_tag::COP,problem_tag::ConOP }));
		RIGIESTER(problem, CEC2017_COP_F24, "C24", std::set<problem_tag>({ problem_tag::COP,problem_tag::ConOP }));
		RIGIESTER(problem, CEC2017_COP_F25, "C25", std::set<problem_tag>({ problem_tag::COP,problem_tag::ConOP }));
		RIGIESTER(problem, CEC2017_COP_F26, "C26", std::set<problem_tag>({ problem_tag::COP,problem_tag::ConOP }));
		RIGIESTER(problem, CEC2017_COP_F27, "C27", std::set<problem_tag>({ problem_tag::COP,problem_tag::ConOP }));
		RIGIESTER(problem, CEC2017_COP_F28, "C28", std::set<problem_tag>({ problem_tag::COP,problem_tag::ConOP }));
		/*classical_MMP*/
		RIGIESTER(problem, beasley, "beasley", std::set<problem_tag>({ problem_tag::MMOP,problem_tag::ConOP }));
		RIGIESTER(problem, branin_rcos, "branin_rcos", std::set<problem_tag>({ problem_tag::MMOP,problem_tag::ConOP }));
		RIGIESTER(problem, center_peak, "center_peak", std::set<problem_tag>({ problem_tag::MMOP,problem_tag::ConOP }));
		RIGIESTER(problem, equal_maxima, "equal_maxima", std::set<problem_tag>({ problem_tag::MMOP,problem_tag::ConOP }));
		RIGIESTER(problem, expanded_decreasing_minima, "expanded_decreasing_minima", std::set<problem_tag>({ problem_tag::MMOP,problem_tag::ConOP }));
		RIGIESTER(problem, expanded_equal_maxima, "expanded_equal_maxima", std::set<problem_tag>({ problem_tag::MMOP,problem_tag::ConOP }));
		RIGIESTER(problem, expanded_five_uneven_peak_trap, "expanded_five_uneven_peak_trap", std::set<problem_tag>({ problem_tag::MMOP,problem_tag::ConOP }));
		RIGIESTER(problem, expanded_himmelblau, "expanded_himmelblau", std::set<problem_tag>({ problem_tag::MMOP,problem_tag::ConOP }));
		RIGIESTER(problem, expanded_six_hump_camel_back, "expanded_six_hump_camel_back", std::set<problem_tag>({ problem_tag::MMOP,problem_tag::ConOP }));
		RIGIESTER(problem, expanded_two_peak_trap, "expanded_two_peak_trap", std::set<problem_tag>({ problem_tag::MMOP,problem_tag::ConOP }));
		RIGIESTER(problem, expanded_uneven_minima, "expanded_uneven_minima", std::set<problem_tag>({ problem_tag::MMOP,problem_tag::ConOP }));
		RIGIESTER(problem, FIBA, "FIBA", std::set<problem_tag>({ problem_tag::MMOP,problem_tag::ConOP }));
		RIGIESTER(problem, five_hills, "five_hills", std::set<problem_tag>({ problem_tag::MMOP,problem_tag::ConOP }));
		RIGIESTER(problem, five_uneven_peak_trap, "five_uneven_peak_trap", std::set<problem_tag>({ problem_tag::MMOP,problem_tag::ConOP }));
		RIGIESTER(problem, himmenblau, "himmenblau", std::set<problem_tag>({ problem_tag::MMOP,problem_tag::ConOP }));
		RIGIESTER(problem, keane_bump, "keane_bump", std::set<problem_tag>({ problem_tag::MMOP,problem_tag::ConOP }));
		RIGIESTER(problem, max_global, "max_global", std::set<problem_tag>({ problem_tag::MMOP,problem_tag::ConOP }));
		RIGIESTER(problem, michalewicz, "michalewicz", std::set<problem_tag>({ problem_tag::MMOP,problem_tag::ConOP }));
		RIGIESTER(problem, modified_rastrigin, "modified_rastrigin", std::set<problem_tag>({ problem_tag::MMOP,problem_tag::ConOP }));
		RIGIESTER(problem, modified_shekel, "modified_shekel", std::set<problem_tag>({ problem_tag::MMOP,problem_tag::ConOP }));
		RIGIESTER(problem, modified_vincent, "modified_vincent", std::set<problem_tag>({ problem_tag::MMOP,problem_tag::ConOP }));
		RIGIESTER(problem, shaffer, "shaffer", std::set<problem_tag>({ problem_tag::MMOP,problem_tag::ConOP }));
		RIGIESTER(problem, shubert, "shubert", std::set<problem_tag>({ problem_tag::MMOP,problem_tag::ConOP }));
		RIGIESTER(problem, six_hump_camel_back, "six_hump_camel_back", std::set<problem_tag>({ problem_tag::MMOP,problem_tag::ConOP }));
		RIGIESTER(problem, szu, "szu", std::set<problem_tag>({ problem_tag::MMOP,problem_tag::ConOP }));
		RIGIESTER(problem, uneven_de_maxima, "uneven_de_maxima", std::set<problem_tag>({ problem_tag::MMOP,problem_tag::ConOP }));
		RIGIESTER(problem, valleys, "valleys", std::set<problem_tag>({ problem_tag::MMOP,problem_tag::ConOP }));
		RIGIESTER(problem, vincent, "vincent", std::set<problem_tag>({ problem_tag::MMOP,problem_tag::ConOP }));
		RIGIESTER(problem, waves, "waves", std::set<problem_tag>({ problem_tag::MMOP,problem_tag::ConOP }));
		/*CEC2013_MMP*/
		RIGIESTER(problem, CEC2013_MMP_F1, "F1_five_uneven_peak_trap", std::set<problem_tag>({ problem_tag::MMOP,problem_tag::ConOP }));
		RIGIESTER(problem, CEC2013_MMP_F2, "F2_equal_maxima", std::set<problem_tag>({ problem_tag::MMOP,problem_tag::ConOP }));
		RIGIESTER(problem, CEC2013_MMP_F3, "F3_uneven_de_maxima", std::set<problem_tag>({ problem_tag::MMOP,problem_tag::ConOP }));
		RIGIESTER(problem, CEC2013_MMP_F4, "F4_himmenblau", std::set<problem_tag>({ problem_tag::MMOP,problem_tag::ConOP }));
		RIGIESTER(problem, CEC2013_MMP_F5, "F5_six_hump_camel_back", std::set<problem_tag>({ problem_tag::MMOP,problem_tag::ConOP }));
		RIGIESTER(problem, CEC2013_MMP_F6, "F6_shubert", std::set<problem_tag>({ problem_tag::MMOP,problem_tag::ConOP }));
		RIGIESTER(problem, CEC2013_MMP_F7, "F7_vincent", std::set<problem_tag>({ problem_tag::MMOP,problem_tag::ConOP }));
		RIGIESTER(problem, CEC2013_MMP_F8, "F8_modified_rastrigin", std::set<problem_tag>({ problem_tag::MMOP,problem_tag::ConOP }));
		RIGIESTER(problem, CEC2013_MMP_F9, "F9_composition1", std::set<problem_tag>({ problem_tag::MMOP,problem_tag::ConOP }));
		RIGIESTER(problem, CEC2013_MMP_F10, "F10_composition2", std::set<problem_tag>({ problem_tag::MMOP,problem_tag::ConOP }));
		RIGIESTER(problem, CEC2013_MMP_F11, "F11_composition3", std::set<problem_tag>({ problem_tag::MMOP,problem_tag::ConOP }));
		RIGIESTER(problem, CEC2013_MMP_F12, "F12_composition4", std::set<problem_tag>({ problem_tag::MMOP,problem_tag::ConOP }));
		/*CEC2015_MMP*/
		RIGIESTER(problem, CEC2015_MMP_F1, "F1_SR_expanded_two_peak_trap", std::set<problem_tag>({ problem_tag::MMOP,problem_tag::ConOP }));
		RIGIESTER(problem, CEC2015_MMP_F2, "F2_SR_expanded_five_uneven_peak_trap", std::set<problem_tag>({ problem_tag::MMOP,problem_tag::ConOP }));
		RIGIESTER(problem, CEC2015_MMP_F3, "F3_SR_expanded_equal_minima", std::set<problem_tag>({ problem_tag::MMOP,problem_tag::ConOP }));
		RIGIESTER(problem, CEC2015_MMP_F4, "F4_SR_expanded_decreasing_minima", std::set<problem_tag>({ problem_tag::MMOP,problem_tag::ConOP }));
		RIGIESTER(problem, CEC2015_MMP_F5, "F5_SR_expanded_uneven_minima", std::set<problem_tag>({ problem_tag::MMOP,problem_tag::ConOP }));
		RIGIESTER(problem, CEC2015_MMP_F6, "F6_SR_expanded_himmelblau", std::set<problem_tag>({ problem_tag::MMOP,problem_tag::ConOP }));
		RIGIESTER(problem, CEC2015_MMP_F7, "F7_SR_expanded_six_hump_camel_back", std::set<problem_tag>({ problem_tag::MMOP,problem_tag::ConOP }));
		RIGIESTER(problem, CEC2015_MMP_F8, "F8_SR_modified_vincent", std::set<problem_tag>({ problem_tag::MMOP,problem_tag::ConOP }));
		RIGIESTER(problem, CEC2015_MMP_F9, "F9_composition2015_C1", std::set<problem_tag>({ problem_tag::MMOP,problem_tag::ConOP }));
		RIGIESTER(problem, CEC2015_MMP_F10, "F10_composition2015_C2", std::set<problem_tag>({ problem_tag::MMOP,problem_tag::ConOP }));
		RIGIESTER(problem, CEC2015_MMP_F11, "F11_composition2015_C3", std::set<problem_tag>({ problem_tag::MMOP,problem_tag::ConOP }));
		RIGIESTER(problem, CEC2015_MMP_F12, "F12_composition2015_C4", std::set<problem_tag>({ problem_tag::MMOP,problem_tag::ConOP }));
		RIGIESTER(problem, CEC2015_MMP_F13, "F13_composition2015_C5", std::set<problem_tag>({ problem_tag::MMOP,problem_tag::ConOP }));
		RIGIESTER(problem, CEC2015_MMP_F14, "F14_composition2015_C6", std::set<problem_tag>({ problem_tag::MMOP,problem_tag::ConOP }));
		RIGIESTER(problem, CEC2015_MMP_F15, "F15_composition2015_C7", std::set<problem_tag>({ problem_tag::MMOP,problem_tag::ConOP }));
		/*realworld*/
		RIGIESTER(problem, gear_train, "gear_Train", std::set<problem_tag>({ problem_tag::GOP,problem_tag::ConOP }));
		RIGIESTER(problem, par_est_FM_sound_waves, "par_est_FM_sound_waves", std::set<problem_tag>({ problem_tag::GOP,problem_tag::ConOP }));
		RIGIESTER(problem, epanet, "epanet", std::set<problem_tag>({ problem_tag::GOP }));
	}

	void register_algorithm() {
		RIGIESTER(algorithm, NSGAII, "NSGAII", std::set<problem_tag>({ problem_tag::MOP,problem_tag::ConOP }));
		RIGIESTER(algorithm, JADE, "JADE", std::set<problem_tag>({ problem_tag::GOP,problem_tag::ConOP }));
		RIGIESTER(algorithm, CRDE, "CRDE", std::set<problem_tag>({ problem_tag::GOP,problem_tag::ConOP }));
		RIGIESTER(algorithm, DynDE, "DynDE", std::set<problem_tag>({ problem_tag::GOP,problem_tag::ConOP }));
		RIGIESTER(algorithm, jDE, "jDE", std::set<problem_tag>({ problem_tag::GOP,problem_tag::ConOP }));
		RIGIESTER(algorithm, NCDE, "NCDE", std::set<problem_tag>({ problem_tag::MMOP,problem_tag::ConOP }));
		RIGIESTER(algorithm, NSDE, "NSDE", std::set<problem_tag>({ problem_tag::MMOP,problem_tag::ConOP }));
		RIGIESTER(algorithm, SaDE, "SaDE", std::set<problem_tag>({ problem_tag::MMOP,problem_tag::ConOP }));
		//RIGIESTER(algorithm, COGL, "COGL", std::set<problem_tag>({ problem_tag::MMOP, problem_tag::DOP, problem_tag::LSOP}));
		RIGIESTER(algorithm, GL_cont, "GLC", std::set<problem_tag>({ problem_tag::GOP,problem_tag::MMOP,problem_tag::ConOP }));

	}

	int before_run() {
		//setup result format as: "XX","XX", ... 
		measure::initialize(global::ms_arg.at("numRun"), std::vector<std::string>({ "Evaluations","Error"}));
		int num_task;
		if (global::ms_arg.find("numTask") != global::ms_arg.end())
			num_task = global::ms_arg.at("numTask");
		else 
			num_task = std::thread::hardware_concurrency();
		
		if (num_task > (int) global::ms_arg.at("numRun"))
			num_task = global::ms_arg.at("numRun");
		
		return num_task;
	}

	void after_run() {
		measure::get_measure()->output_progr();
		measure::get_measure()->output_final();
	}
}
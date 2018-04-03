#include "initialize.h"
#include "../core/global.h"
#include <thread>

namespace OFEC {
	
	std::mutex g_mutex;
	std::mutex g_mutexStream;


	void register_paramter() {
		global::ms_param["param_ND"] = param_numDim;
		global::ms_param["param_NP"] = param_numPeak;
		global::ms_param["param_PN"] = param_proName;
		global::ms_param["param_AN"] = param_algName;
		global::ms_param["param_ME"] = param_maxEvals;
		global::ms_param["param_SL"] = param_shiftLength;
		global::ms_param["param_CT"] = param_changeType;
		global::ms_param["param_CR"] = param_changeRatio;
		global::ms_param["param_RID"] = param_runId;
		global::ms_param["param_AID"] = param_algId;
		global::ms_param["param_PID"] = param_proId;
		global::ms_param["param_FNDC"] = param_flagNumDimChange;
		global::ms_param["param_FNPC"] = param_flagNumPeakChange;
		global::ms_param["param_PNCM"] = param_peakNumChangeMode;
		global::ms_param["param_FN"] = param_flagNoise;
		global::ms_param["param_FTL"] = param_flagTimeLinkage;
		global::ms_param["param_CDBGFID"] = param_comDBGFunID;
		global::ms_param["param_NS"] = param_noiseSeverity;
		global::ms_param["param_TLS"] = param_timelinkageSeverity;
		global::ms_param["param_PS"] = param_popSize;
		global::ms_param["param_ECF"] = param_evalCountFlag;
		global::ms_param["param_SI"] = param_stepIndi;
		global::ms_param["param_TT"] = param_trainingTime;
		global::ms_param["param_SPS"] = param_subPopSize;
		global::ms_param["param_OLD"] = param_overlapDgre;
		global::ms_param["param_CTH"] = param_convThreshold;
		global::ms_param["param_TW"] = param_timeWindow;
		global::ms_param["param_SF"] = param_sampleFre;
		global::ms_param["param_gOptFlag"] = param_gOptFlag;
		global::ms_param["param_WD"] = param_workingDir;
		global::ms_param["param_CF"] = param_changeFre;
		global::ms_param["param_CoF"] = param_convFactor;
		global::ms_param["param_NR"] = param_numRun;
		global::ms_param["param_NT"] = param_numTask;
		global::ms_param["param_ER"] = param_exlRadius;
		global::ms_param["param_SVM"] = param_solutionValidationMode;
		global::ms_param["param_PIM"] = param_populationInitialMethod;
		global::ms_param["param_HR"] = param_hibernatingRadius;
		global::ms_param["param_MNPS"] = param_minNumPopSize;
		global::ms_param["param_NC"] = param_numChange;
		global::ms_param["param_NO"] = param_numObj;
		global::ms_param["param_ca"] = param_case;
		global::ms_param["param_RBP"] = param_resource4BestPop;
		global::ms_param["param_DF1"] = param_dataFile1;
		global::ms_param["param_XP"] = param_xoverProbability;
		global::ms_param["param_MP"] = param_mutProbability;
		global::ms_param["param_PT"] = param_proTag;
		global::ms_param["param_NGO"] = param_numGOpt;
		global::ms_param["param_NF"] = param_noiseFlag;
		global::ms_param["param_PF"] = param_predicFlag;
		global::ms_param["param_CT2"] = param_changeType2;
		global::ms_param["param_PPB"] = param_peaksPerBox;
		global::ms_param["param_IT1"] = param_interTest1;
		global::ms_param["param_IT2"] = param_interTest2;
		global::ms_param["param_IT3"] = param_interTest3;
		global::ms_param["param_IT4"] = param_interTest4;
		global::ms_param["param_NB"] = param_numBox;
		global::ms_param["param_HCM"] = param_heightConfigMode;
		global::ms_param["param_PC"] = param_peakCenter;
		global::ms_param["param_NPR"] = param_numParetoRegion;
		global::ms_param["param_VR"] = param_validRadius;
		global::ms_param["param_ra"] = param_radius;
		global::ms_param["param_JH"] = param_jumpHeight;
		global::ms_param["param_VR"] = param_variableRelation;
		global::ms_param["param_PkS"] = param_peakShape;
		global::ms_param["param_DM"] = param_divisionMode;
		global::ms_param["param_POS"] = param_peakOffset;
		global::ms_param["param_FI"] = param_flagIrregular;
		global::ms_param["param_FA"] = param_flagAsymmetry;
		global::ms_param["param_DF2"] = param_dataFile2;
		global::ms_param["param_DF3"] = param_dataFile3;
		global::ms_param["param_FR"] = param_flagRotation;
		global::ms_param["param_DD1"] = param_dataDirectory1;
		global::ms_param["param_MI"] = param_maxIter;
		global::ms_param["param_MSI"] = param_maxSucIter;
		global::ms_param["param_ep"] = param_epsilon;
		global::ms_param["param_MSDE"] = param_mutationSchemeDE;
		global::ms_param["param_USPL"] = param_updateSchemeProbabilityLearning;
		global::ms_param["param_be"] = param_beta;
		global::ms_param["param_al"] = param_alpha;
		global::ms_param["param_ga"] = param_gamma;
		global::ms_param["param_GS"] = param_glstrture;
		global::ms_param["param_NSP"] = param_numSubPop;
	}

	void register_problem() {
		RIGIESTER(problem, ZDT1, "ZDT1", std::set<problem_tag>({ problem_tag::MOP,problem_tag::CONT }));
		RIGIESTER(problem, ZDT2, "ZDT2", std::set<problem_tag>({ problem_tag::MOP,problem_tag::CONT }));
		RIGIESTER(problem, ZDT3, "ZDT3", std::set<problem_tag>({ problem_tag::MOP,problem_tag::CONT }));
		RIGIESTER(problem, ZDT4, "ZDT4", std::set<problem_tag>({ problem_tag::MOP,problem_tag::CONT }));
		RIGIESTER(problem, ZDT6, "ZDT6", std::set<problem_tag>({ problem_tag::MOP,problem_tag::CONT }));
		RIGIESTER(problem, DTLZ1, "DTLZ1", std::set<problem_tag>({ problem_tag::MOP,problem_tag::CONT }));
		RIGIESTER(problem, DTLZ2, "DTLZ2", std::set<problem_tag>({ problem_tag::MOP,problem_tag::CONT }));
		RIGIESTER(problem, DTLZ3, "DTLZ3", std::set<problem_tag>({ problem_tag::MOP,problem_tag::CONT }));
		RIGIESTER(problem, DTLZ4, "DTLZ4", std::set<problem_tag>({ problem_tag::MOP,problem_tag::CONT }));
		RIGIESTER(problem, MOEA_F1, "MOEA_F1", std::set<problem_tag>({ problem_tag::MOP,problem_tag::CONT }));
		RIGIESTER(problem, MOEA_F2, "MOEA_F2", std::set<problem_tag>({ problem_tag::MOP,problem_tag::CONT }));
		RIGIESTER(problem, MOEA_F3, "MOEA_F3", std::set<problem_tag>({ problem_tag::MOP,problem_tag::CONT }));
		RIGIESTER(problem, MOEA_F4, "MOEA_F4", std::set<problem_tag>({ problem_tag::MOP,problem_tag::CONT }));
		RIGIESTER(problem, MOEA_F5, "MOEA_F5", std::set<problem_tag>({ problem_tag::MOP,problem_tag::CONT }));
		RIGIESTER(problem, MOEA_F6, "MOEA_F6", std::set<problem_tag>({ problem_tag::MOP,problem_tag::CONT }));
		RIGIESTER(problem, MOEA_F7, "MOEA_F7", std::set<problem_tag>({ problem_tag::MOP,problem_tag::CONT }));
		RIGIESTER(problem, MOEA_F8, "MOEA_F8", std::set<problem_tag>({ problem_tag::MOP,problem_tag::CONT }));
		RIGIESTER(problem, MOEA_F9, "MOEA_F9", std::set<problem_tag>({ problem_tag::MOP,problem_tag::CONT }));
		/*BBOB*/
		RIGIESTER(problem, BBOB_F1, "BBOB_F01_Sphere", std::set<problem_tag>({ problem_tag::GOP,problem_tag::CONT }));
		RIGIESTER(problem, BBOB_F2, "BBOB_F02_Ellipsoidal", std::set<problem_tag>({ problem_tag::GOP,problem_tag::CONT }));
		RIGIESTER(problem, BBOB_F3, "BBOB_F03_Rastrigin", std::set<problem_tag>({ problem_tag::GOP,problem_tag::CONT }));
		RIGIESTER(problem, BBOB_F4, "BBOB_F04_BucheRastrigin", std::set<problem_tag>({ problem_tag::GOP,problem_tag::CONT }));
		RIGIESTER(problem, BBOB_F5, "BBOB_F05_Slope", std::set<problem_tag>({ problem_tag::GOP,problem_tag::CONT }));
		RIGIESTER(problem, BBOB_F6, "BBOB_F06_Sector", std::set<problem_tag>({ problem_tag::GOP,problem_tag::CONT }));
		RIGIESTER(problem, BBOB_F7, "BBOB_F07_StepEllipsoid", std::set<problem_tag>({ problem_tag::GOP,problem_tag::CONT }));
		RIGIESTER(problem, BBOB_F8, "BBOB_F08_OriginalRosenbrock", std::set<problem_tag>({ problem_tag::GOP,problem_tag::CONT }));
		RIGIESTER(problem, BBOB_F9, "BBOB_F09_RotatedRosenbrock", std::set<problem_tag>({ problem_tag::GOP,problem_tag::CONT }));
		RIGIESTER(problem, BBOB_F10, "BBOB_F10_NonseparableEllipsoid", std::set<problem_tag>({ problem_tag::GOP,problem_tag::CONT }));
		RIGIESTER(problem, BBOB_F11, "BBOB_F11_Discus", std::set<problem_tag>({ problem_tag::GOP,problem_tag::CONT }));
		RIGIESTER(problem, BBOB_F12, "BBOB_F12_BentCigar", std::set<problem_tag>({ problem_tag::GOP,problem_tag::CONT }));
		RIGIESTER(problem, BBOB_F13, "BBOB_F13_SharpRidge", std::set<problem_tag>({ problem_tag::GOP,problem_tag::CONT }));
		RIGIESTER(problem, BBOB_F14, "BBOB_F14_DifferentPowers", std::set<problem_tag>({ problem_tag::GOP,problem_tag::CONT }));
		RIGIESTER(problem, BBOB_F15, "BBOB_F15_NonseparableRastrigin", std::set<problem_tag>({ problem_tag::GOP,problem_tag::CONT }));
		RIGIESTER(problem, BBOB_F16, "BBOB_F16_Weierstrass", std::set<problem_tag>({ problem_tag::GOP,problem_tag::CONT }));
		RIGIESTER(problem, BBOB_F17, "BBOB_F17_SchaffersF7", std::set<problem_tag>({ problem_tag::GOP,problem_tag::CONT }));
		RIGIESTER(problem, BBOB_F18, "BBOB_F18_IllconditionedSchaffersF7", std::set<problem_tag>({ problem_tag::GOP,problem_tag::CONT }));
		RIGIESTER(problem, BBOB_F19, "BBOB_F19_CompositeGriewankRosenbrock", std::set<problem_tag>({ problem_tag::GOP,problem_tag::CONT }));
		RIGIESTER(problem, BBOB_F20, "BBOB_F20_Schwefel", std::set<problem_tag>({ problem_tag::GOP,problem_tag::CONT }));
		RIGIESTER(problem, BBOB_F21, "BBOB_F21_GallagherGaussian101mePeaks", std::set<problem_tag>({ problem_tag::GOP,problem_tag::CONT }));
		RIGIESTER(problem, BBOB_F22, "BBOB_F22_GallagherGaussian21hiPeaks", std::set<problem_tag>({ problem_tag::GOP,problem_tag::CONT }));
		RIGIESTER(problem, BBOB_F23, "BBOB_F23_Katsuura", std::set<problem_tag>({ problem_tag::GOP,problem_tag::CONT }));
		RIGIESTER(problem, BBOB_F24, "BBOB_F24_LunacekBiRastrigin", std::set<problem_tag>({ problem_tag::GOP,problem_tag::CONT }));
		/*classical_GOP*/
		RIGIESTER(problem, ackley, "ackley", std::set<problem_tag>({ problem_tag::GOP,problem_tag::CONT }));
		RIGIESTER(problem, bent_cigar, "bent_cigar", std::set<problem_tag>({ problem_tag::GOP,problem_tag::CONT }));
		RIGIESTER(problem, different_powers, "different_powers", std::set<problem_tag>({ problem_tag::GOP,problem_tag::CONT }));
		RIGIESTER(problem, discus, "discus", std::set<problem_tag>({ problem_tag::GOP,problem_tag::CONT }));
		RIGIESTER(problem, elliptic, "elliptic", std::set<problem_tag>({ problem_tag::GOP,problem_tag::CONT }));
		RIGIESTER(problem, griewank, "griewank", std::set<problem_tag>({ problem_tag::GOP,problem_tag::CONT }));
		RIGIESTER(problem, griewank_rosenbrock, "griewank_rosenbrock", std::set<problem_tag>({ problem_tag::GOP,problem_tag::CONT }));
		RIGIESTER(problem, happy_cat, "happy_cat", std::set<problem_tag>({ problem_tag::GOP,problem_tag::CONT }));
		RIGIESTER(problem, HGBat, "HGBat", std::set<problem_tag>({ problem_tag::GOP,problem_tag::CONT }));
		RIGIESTER(problem, katsuura, "katsuura", std::set<problem_tag>({ problem_tag::GOP,problem_tag::CONT }));
		RIGIESTER(problem, modified_schwefel, "modified_schwefel", std::set<problem_tag>({ problem_tag::GOP,problem_tag::CONT }));
		RIGIESTER(problem, non_continuous_rastrigin, "non_continuous_rastrigin", std::set<problem_tag>({ problem_tag::GOP,problem_tag::CONT }));
		RIGIESTER(problem, non_continuous_scaffer_F6, "non_continuous_scaffer_F6", std::set<problem_tag>({ problem_tag::GOP,problem_tag::CONT }));
		RIGIESTER(problem, penalized_1, "penalized_1", std::set<problem_tag>({ problem_tag::GOP,problem_tag::CONT }));
		RIGIESTER(problem, penalized_2, "penalized_2", std::set<problem_tag>({ problem_tag::GOP,problem_tag::CONT }));
		RIGIESTER(problem, quartic_noisy, "quartic_noisy", std::set<problem_tag>({ problem_tag::GOP,problem_tag::CONT }));
		RIGIESTER(problem, rastrigin, "rastrigin", std::set<problem_tag>({ problem_tag::GOP,problem_tag::CONT }));
		RIGIESTER(problem, rosenbrock, "rosenbrock", std::set<problem_tag>({ problem_tag::GOP,problem_tag::CONT }));
		RIGIESTER(problem, rotated_bent_cigar, "rotated_bent_cigar", std::set<problem_tag>({ problem_tag::GOP,problem_tag::CONT }));
		RIGIESTER(problem, rotated_discus, "rotated_discus", std::set<problem_tag>({ problem_tag::GOP,problem_tag::CONT }));
		RIGIESTER(problem, rotated_rosenbrock, "rotated_rosenbrock", std::set<problem_tag>({ problem_tag::GOP,problem_tag::CONT }));
		RIGIESTER(problem, rotated_scaffer_F6, "rotated_scaffer_F6", std::set<problem_tag>({ problem_tag::GOP,problem_tag::CONT }));
		RIGIESTER(problem, scaffer_F6, "scaffer_F6", std::set<problem_tag>({ problem_tag::GOP,problem_tag::CONT }));
		RIGIESTER(problem, schwefel, "schwefel", std::set<problem_tag>({ problem_tag::GOP,problem_tag::CONT }));
		RIGIESTER(problem, schwefel_1_2, "schwefel_1_2", std::set<problem_tag>({ problem_tag::GOP,problem_tag::CONT }));
		RIGIESTER(problem, schwefel_2_6, "schwefel_2_6", std::set<problem_tag>({ problem_tag::GOP,problem_tag::CONT }));
		RIGIESTER(problem, sphere, "sphere", std::set<problem_tag>({ problem_tag::GOP,problem_tag::CONT }));
		RIGIESTER(problem, sphere_noisy, "sphere_noisy", std::set<problem_tag>({ problem_tag::GOP,problem_tag::CONT }));
		RIGIESTER(problem, step, "step", std::set<problem_tag>({ problem_tag::GOP,problem_tag::CONT }));
		RIGIESTER(problem, weierstrass, "weierstrass", std::set<problem_tag>({ problem_tag::GOP,problem_tag::CONT }));
		/*CEC2005_GOP*/
		RIGIESTER(problem, CEC2005_GOP_F1, "F1_shifted_sphere", std::set<problem_tag>({ problem_tag::GOP,problem_tag::CONT }));
		RIGIESTER(problem, CEC2005_GOP_F2, "F2_shifted_schwefel_1_2", std::set<problem_tag>({ problem_tag::GOP,problem_tag::CONT }));
		RIGIESTER(problem, CEC2005_GOP_F3, "F3_shifted_rotated_high_cond_elliptic", std::set<problem_tag>({ problem_tag::GOP,problem_tag::CONT }));
		RIGIESTER(problem, CEC2005_GOP_F4, "F4_shifted_schwefel_1_2_noisy", std::set<problem_tag>({ problem_tag::GOP,problem_tag::CONT }));
		RIGIESTER(problem, CEC2005_GOP_F5, "F5_schwefel_2_6_bound", std::set<problem_tag>({ problem_tag::GOP,problem_tag::CONT }));
		RIGIESTER(problem, CEC2005_GOP_F6, "F6_shifted_rosenbrock", std::set<problem_tag>({ problem_tag::GOP,problem_tag::CONT }));
		RIGIESTER(problem, CEC2005_GOP_F7, "F7_shifted_rotated_griewank_no_bound", std::set<problem_tag>({ problem_tag::GOP,problem_tag::CONT }));
		RIGIESTER(problem, CEC2005_GOP_F8, "F8_shifted_rotated_ackley_bound", std::set<problem_tag>({ problem_tag::GOP,problem_tag::CONT }));
		RIGIESTER(problem, CEC2005_GOP_F9, "F9_shifted_rastrigin", std::set<problem_tag>({ problem_tag::GOP,problem_tag::CONT }));
		RIGIESTER(problem, CEC2005_GOP_F10, "F10_shifted_rotated_rastrigin", std::set<problem_tag>({ problem_tag::GOP,problem_tag::CONT }));
		RIGIESTER(problem, CEC2005_GOP_F11, "F11_shifted_rotated_weierstrass", std::set<problem_tag>({ problem_tag::GOP,problem_tag::CONT }));
		RIGIESTER(problem, CEC2005_GOP_F12, "F12_schwefel_2_13", std::set<problem_tag>({ problem_tag::GOP,problem_tag::CONT }));
		RIGIESTER(problem, CEC2005_GOP_F13, "F13_shifted_expanded_griewank_rosenbrock", std::set<problem_tag>({ problem_tag::GOP,problem_tag::CONT }));
		RIGIESTER(problem, CEC2005_GOP_F14, "F14_shifted_rotated_expanded_scaffer_F6", std::set<problem_tag>({ problem_tag::GOP,problem_tag::CONT }));
		RIGIESTER(problem, CEC2005_GOP_F15, "F15_hybrid_composition", std::set<problem_tag>({ problem_tag::GOP,problem_tag::CONT }));
		RIGIESTER(problem, CEC2005_GOP_F16, "F16_rotated_hybrid_composition_F15", std::set<problem_tag>({ problem_tag::GOP,problem_tag::CONT }));
		RIGIESTER(problem, CEC2005_GOP_F17, "F17_hybrid_composition_noisy", std::set<problem_tag>({ problem_tag::GOP,problem_tag::CONT }));
		RIGIESTER(problem, CEC2005_GOP_F18, "F18_rotated_hybrid_composition", std::set<problem_tag>({ problem_tag::GOP,problem_tag::CONT }));
		RIGIESTER(problem, CEC2005_GOP_F19, "F19_rotated_hybrid_narrow_basin", std::set<problem_tag>({ problem_tag::GOP,problem_tag::CONT }));
		RIGIESTER(problem, CEC2005_GOP_F20, "F20_rotated_hybrid_bound", std::set<problem_tag>({ problem_tag::GOP,problem_tag::CONT }));
		RIGIESTER(problem, CEC2005_GOP_F21, "F21_rotated_hybrid_composition", std::set<problem_tag>({ problem_tag::GOP,problem_tag::CONT }));
		RIGIESTER(problem, CEC2005_GOP_F22, "F22_rotated_hybrid_high_condition_number_matrix", std::set<problem_tag>({ problem_tag::GOP,problem_tag::CONT }));
		RIGIESTER(problem, CEC2005_GOP_F23, "F23_non_continuous_rotated_hybrid", std::set<problem_tag>({ problem_tag::GOP,problem_tag::CONT }));
		RIGIESTER(problem, CEC2005_GOP_F24, "F24_rotated_hybrid_composition", std::set<problem_tag>({ problem_tag::GOP,problem_tag::CONT }));
		RIGIESTER(problem, CEC2005_GOP_F25, "F25_rotated_hybrid_no_bound", std::set<problem_tag>({ problem_tag::GOP,problem_tag::CONT }));
		/*CEC2013_large_scale*/
		RIGIESTER(problem, CEC2013_LSOP_F1, "ShiftedElliptic_F1", std::set<problem_tag>({ problem_tag::LSOP,problem_tag::CONT }));
		RIGIESTER(problem, CEC2013_LSOP_F2, "ShiftedRastrigin_F2", std::set<problem_tag>({ problem_tag::LSOP,problem_tag::CONT }));
		RIGIESTER(problem, CEC2013_LSOP_F3, "ShiftedAckley_F3", std::set<problem_tag>({ problem_tag::LSOP,problem_tag::CONT }));
		RIGIESTER(problem, CEC2013_LSOP_F4, "N7S1_SR_Elliptic_F4", std::set<problem_tag>({ problem_tag::LSOP,problem_tag::CONT }));
		RIGIESTER(problem, CEC2013_LSOP_F5, "N7S1_SR_Rastrigin_F5", std::set<problem_tag>({ problem_tag::LSOP,problem_tag::CONT }));
		RIGIESTER(problem, CEC2013_LSOP_F6, "N7S1_SR_Ackley_F6", std::set<problem_tag>({ problem_tag::LSOP,problem_tag::CONT }));
		RIGIESTER(problem, CEC2013_LSOP_F7, "N7S1_ShiftedSchwefel_F7", std::set<problem_tag>({ problem_tag::LSOP,problem_tag::CONT }));
		RIGIESTER(problem, CEC2013_LSOP_F8, "N20_SR_Elliptic_F8", std::set<problem_tag>({ problem_tag::LSOP,problem_tag::CONT }));
		RIGIESTER(problem, CEC2013_LSOP_F9, "N20_SR_Rastrigin_F9", std::set<problem_tag>({ problem_tag::LSOP,problem_tag::CONT }));
		RIGIESTER(problem, CEC2013_LSOP_F10, "N20_SR_Ackley_F10", std::set<problem_tag>({ problem_tag::LSOP,problem_tag::CONT }));
		RIGIESTER(problem, CEC2013_LSOP_F11, "N20_ShiftedSchwefel_F11", std::set<problem_tag>({ problem_tag::LSOP,problem_tag::CONT }));
		RIGIESTER(problem, CEC2013_LSOP_F12, "ShiftedRosenbrock_F12", std::set<problem_tag>({ problem_tag::LSOP,problem_tag::CONT }));
		RIGIESTER(problem, CEC2013_LSOP_F13, "ConformingOS_ShiftedSchwefel_F13", std::set<problem_tag>({ problem_tag::LSOP,problem_tag::CONT }));
		RIGIESTER(problem, CEC2013_LSOP_F14, "ConflictingOS_ShiftedSchwefel_F14", std::set<problem_tag>({ problem_tag::LSOP,problem_tag::CONT }));
		RIGIESTER(problem, CEC2013_LSOP_F15, "ShiftedSchwefel_F15", std::set<problem_tag>({ problem_tag::LSOP,problem_tag::CONT }));
		/*CEC2015_GOP*/
		RIGIESTER(problem, CEC2015_GOP_F1, "F1_R_elliptic", std::set<problem_tag>({ problem_tag::GOP,problem_tag::CONT }));
		RIGIESTER(problem, CEC2015_GOP_F2, "F2_R_cigar", std::set<problem_tag>({ problem_tag::GOP,problem_tag::CONT }));
		RIGIESTER(problem, CEC2015_GOP_F3, "F3_SR_ackley", std::set<problem_tag>({ problem_tag::GOP,problem_tag::CONT }));
		RIGIESTER(problem, CEC2015_GOP_F4, "F4_SR_rastrigin", std::set<problem_tag>({ problem_tag::GOP,problem_tag::CONT }));
		RIGIESTER(problem, CEC2015_GOP_F5, "F5_SR_schwefel", std::set<problem_tag>({ problem_tag::GOP,problem_tag::CONT }));
		RIGIESTER(problem, CEC2015_GOP_F6, "F6_hybrid1", std::set<problem_tag>({ problem_tag::GOP,problem_tag::CONT }));
		RIGIESTER(problem, CEC2015_GOP_F7, "F7_hybrid2", std::set<problem_tag>({ problem_tag::GOP,problem_tag::CONT }));
		RIGIESTER(problem, CEC2015_GOP_F8, "F8_hybrid3", std::set<problem_tag>({ problem_tag::GOP,problem_tag::CONT }));
		RIGIESTER(problem, CEC2015_GOP_F9, "F9_global_composition1", std::set<problem_tag>({ problem_tag::GOP,problem_tag::CONT }));
		RIGIESTER(problem, CEC2015_GOP_F10, "F10_global_composition2", std::set<problem_tag>({ problem_tag::GOP,problem_tag::CONT }));
		RIGIESTER(problem, CEC2015_GOP_F11, "F11_global_composition3", std::set<problem_tag>({ problem_tag::GOP,problem_tag::CONT }));
		RIGIESTER(problem, CEC2015_GOP_F12, "F12_global_composition4", std::set<problem_tag>({ problem_tag::GOP,problem_tag::CONT }));
		RIGIESTER(problem, CEC2015_GOP_F13, "F13_global_composition5", std::set<problem_tag>({ problem_tag::GOP,problem_tag::CONT }));
		RIGIESTER(problem, CEC2015_GOP_F14, "F14_global_composition6", std::set<problem_tag>({ problem_tag::GOP,problem_tag::CONT }));
		RIGIESTER(problem, CEC2015_GOP_F15, "F15_global_composition7", std::set<problem_tag>({ problem_tag::GOP,problem_tag::CONT }));
		/*CEC2015_EOP*/
		RIGIESTER(problem, CEC2015_EOP_F1, "F1_rotated_bent_cigar", std::set<problem_tag>({ problem_tag::EOP,problem_tag::CONT }));
		RIGIESTER(problem, CEC2015_EOP_F2, "F2_rotated_discus", std::set<problem_tag>({ problem_tag::EOP,problem_tag::CONT }));
		RIGIESTER(problem, CEC2015_EOP_F3, "F3_shifted_rotated_weierstrass", std::set<problem_tag>({ problem_tag::EOP,problem_tag::CONT }));
		RIGIESTER(problem, CEC2015_EOP_F4, "F4_shifted_rotated_schwefel", std::set<problem_tag>({ problem_tag::EOP,problem_tag::CONT }));
		RIGIESTER(problem, CEC2015_EOP_F5, "F5_shifted_rotated_katsuura", std::set<problem_tag>({ problem_tag::EOP,problem_tag::CONT }));
		RIGIESTER(problem, CEC2015_EOP_F6, "F6_shifted_rotated_happy_cat", std::set<problem_tag>({ problem_tag::EOP,problem_tag::CONT }));
		RIGIESTER(problem, CEC2015_EOP_F7, "F7_shifted_rotated_HGBat", std::set<problem_tag>({ problem_tag::EOP,problem_tag::CONT }));
		RIGIESTER(problem, CEC2015_EOP_F8, "F8_shifted_rotated_griewank_rosenbrock", std::set<problem_tag>({ problem_tag::EOP,problem_tag::CONT }));
		RIGIESTER(problem, CEC2015_EOP_F9, "F9_shifted_rotated_scaffer_F6", std::set<problem_tag>({ problem_tag::EOP,problem_tag::CONT }));
		RIGIESTER(problem, CEC2015_EOP_F10, "F10_hybrid1", std::set<problem_tag>({ problem_tag::EOP,problem_tag::CONT }));
		RIGIESTER(problem, CEC2015_EOP_F11, "F11_hybrid2", std::set<problem_tag>({ problem_tag::EOP,problem_tag::CONT }));
		RIGIESTER(problem, CEC2015_EOP_F12, "F12_hybrid3", std::set<problem_tag>({ problem_tag::EOP,problem_tag::CONT }));
		RIGIESTER(problem, CEC2015_EOP_F13, "F13_expensive_composition1", std::set<problem_tag>({ problem_tag::EOP,problem_tag::CONT }));
		RIGIESTER(problem, CEC2015_EOP_F14, "F14_expensive_composition2", std::set<problem_tag>({ problem_tag::EOP,problem_tag::CONT }));
		RIGIESTER(problem, CEC2015_EOP_F15, "F15_expensive_composition3", std::set<problem_tag>({ problem_tag::EOP,problem_tag::CONT }));
		/*CEC2017_COP*/
		RIGIESTER(problem, CEC2017_COP_F01, "C01", std::set<problem_tag>({ problem_tag::COP,problem_tag::CONT }));
		RIGIESTER(problem, CEC2017_COP_F02, "C02", std::set<problem_tag>({ problem_tag::COP,problem_tag::CONT }));
		RIGIESTER(problem, CEC2017_COP_F03, "C03", std::set<problem_tag>({ problem_tag::COP,problem_tag::CONT }));
		RIGIESTER(problem, CEC2017_COP_F04, "C04", std::set<problem_tag>({ problem_tag::COP,problem_tag::CONT }));
		RIGIESTER(problem, CEC2017_COP_F05, "C05", std::set<problem_tag>({ problem_tag::COP,problem_tag::CONT }));
		RIGIESTER(problem, CEC2017_COP_F06, "C06", std::set<problem_tag>({ problem_tag::COP,problem_tag::CONT }));
		RIGIESTER(problem, CEC2017_COP_F07, "C07", std::set<problem_tag>({ problem_tag::COP,problem_tag::CONT }));
		RIGIESTER(problem, CEC2017_COP_F08, "C08", std::set<problem_tag>({ problem_tag::COP,problem_tag::CONT }));
		RIGIESTER(problem, CEC2017_COP_F09, "C09", std::set<problem_tag>({ problem_tag::COP,problem_tag::CONT }));
		RIGIESTER(problem, CEC2017_COP_F10, "C10", std::set<problem_tag>({ problem_tag::COP,problem_tag::CONT }));
		RIGIESTER(problem, CEC2017_COP_F11, "C11", std::set<problem_tag>({ problem_tag::COP,problem_tag::CONT }));
		RIGIESTER(problem, CEC2017_COP_F12, "C12", std::set<problem_tag>({ problem_tag::COP,problem_tag::CONT }));
		RIGIESTER(problem, CEC2017_COP_F13, "C13", std::set<problem_tag>({ problem_tag::COP,problem_tag::CONT }));
		RIGIESTER(problem, CEC2017_COP_F14, "C14", std::set<problem_tag>({ problem_tag::COP,problem_tag::CONT }));
		RIGIESTER(problem, CEC2017_COP_F15, "C15", std::set<problem_tag>({ problem_tag::COP,problem_tag::CONT }));
		RIGIESTER(problem, CEC2017_COP_F16, "C16", std::set<problem_tag>({ problem_tag::COP,problem_tag::CONT }));
		RIGIESTER(problem, CEC2017_COP_F17, "C17", std::set<problem_tag>({ problem_tag::COP,problem_tag::CONT }));
		RIGIESTER(problem, CEC2017_COP_F18, "C18", std::set<problem_tag>({ problem_tag::COP,problem_tag::CONT }));
		RIGIESTER(problem, CEC2017_COP_F19, "C19", std::set<problem_tag>({ problem_tag::COP,problem_tag::CONT }));
		RIGIESTER(problem, CEC2017_COP_F20, "C20", std::set<problem_tag>({ problem_tag::COP,problem_tag::CONT }));
		RIGIESTER(problem, CEC2017_COP_F21, "C21", std::set<problem_tag>({ problem_tag::COP,problem_tag::CONT }));
		RIGIESTER(problem, CEC2017_COP_F22, "C22", std::set<problem_tag>({ problem_tag::COP,problem_tag::CONT }));
		RIGIESTER(problem, CEC2017_COP_F23, "C23", std::set<problem_tag>({ problem_tag::COP,problem_tag::CONT }));
		RIGIESTER(problem, CEC2017_COP_F24, "C24", std::set<problem_tag>({ problem_tag::COP,problem_tag::CONT }));
		RIGIESTER(problem, CEC2017_COP_F25, "C25", std::set<problem_tag>({ problem_tag::COP,problem_tag::CONT }));
		RIGIESTER(problem, CEC2017_COP_F26, "C26", std::set<problem_tag>({ problem_tag::COP,problem_tag::CONT }));
		RIGIESTER(problem, CEC2017_COP_F27, "C27", std::set<problem_tag>({ problem_tag::COP,problem_tag::CONT }));
		RIGIESTER(problem, CEC2017_COP_F28, "C28", std::set<problem_tag>({ problem_tag::COP,problem_tag::CONT }));
		/*classical_MMP*/
		RIGIESTER(problem, beasley, "beasley", std::set<problem_tag>({ problem_tag::MMP,problem_tag::CONT }));
		RIGIESTER(problem, branin_rcos, "branin_rcos", std::set<problem_tag>({ problem_tag::MMP,problem_tag::CONT }));
		RIGIESTER(problem, center_peak, "center_peak", std::set<problem_tag>({ problem_tag::MMP,problem_tag::CONT }));
		RIGIESTER(problem, equal_maxima, "equal_maxima", std::set<problem_tag>({ problem_tag::MMP,problem_tag::CONT }));
		RIGIESTER(problem, expanded_decreasing_minima, "expanded_decreasing_minima", std::set<problem_tag>({ problem_tag::MMP,problem_tag::CONT }));
		RIGIESTER(problem, expanded_equal_maxima, "expanded_equal_maxima", std::set<problem_tag>({ problem_tag::MMP,problem_tag::CONT }));
		RIGIESTER(problem, expanded_five_uneven_peak_trap, "expanded_five_uneven_peak_trap", std::set<problem_tag>({ problem_tag::MMP,problem_tag::CONT }));
		RIGIESTER(problem, expanded_himmelblau, "expanded_himmelblau", std::set<problem_tag>({ problem_tag::MMP,problem_tag::CONT }));
		RIGIESTER(problem, expanded_six_hump_camel_back, "expanded_six_hump_camel_back", std::set<problem_tag>({ problem_tag::MMP,problem_tag::CONT }));
		RIGIESTER(problem, expanded_two_peak_trap, "expanded_two_peak_trap", std::set<problem_tag>({ problem_tag::MMP,problem_tag::CONT }));
		RIGIESTER(problem, expanded_uneven_minima, "expanded_uneven_minima", std::set<problem_tag>({ problem_tag::MMP,problem_tag::CONT }));
		RIGIESTER(problem, FIBA, "FIBA", std::set<problem_tag>({ problem_tag::MMP,problem_tag::CONT }));
		RIGIESTER(problem, five_hills, "five_hills", std::set<problem_tag>({ problem_tag::MMP,problem_tag::CONT }));
		RIGIESTER(problem, five_uneven_peak_trap, "five_uneven_peak_trap", std::set<problem_tag>({ problem_tag::MMP,problem_tag::CONT }));
		RIGIESTER(problem, himmenblau, "himmenblau", std::set<problem_tag>({ problem_tag::MMP,problem_tag::CONT }));
		RIGIESTER(problem, keane_bump, "keane_bump", std::set<problem_tag>({ problem_tag::MMP,problem_tag::CONT }));
		RIGIESTER(problem, max_global, "max_global", std::set<problem_tag>({ problem_tag::MMP,problem_tag::CONT }));
		RIGIESTER(problem, michalewicz, "michalewicz", std::set<problem_tag>({ problem_tag::MMP,problem_tag::CONT }));
		RIGIESTER(problem, modified_rastrigin, "modified_rastrigin", std::set<problem_tag>({ problem_tag::MMP,problem_tag::CONT }));
		RIGIESTER(problem, modified_shekel, "modified_shekel", std::set<problem_tag>({ problem_tag::MMP,problem_tag::CONT }));
		RIGIESTER(problem, modified_vincent, "modified_vincent", std::set<problem_tag>({ problem_tag::MMP,problem_tag::CONT }));
		RIGIESTER(problem, shaffer, "shaffer", std::set<problem_tag>({ problem_tag::MMP,problem_tag::CONT }));
		RIGIESTER(problem, shubert, "shubert", std::set<problem_tag>({ problem_tag::MMP,problem_tag::CONT }));
		RIGIESTER(problem, six_hump_camel_back, "six_hump_camel_back", std::set<problem_tag>({ problem_tag::MMP,problem_tag::CONT }));
		RIGIESTER(problem, szu, "szu", std::set<problem_tag>({ problem_tag::MMP,problem_tag::CONT }));
		RIGIESTER(problem, uneven_de_maxima, "uneven_de_maxima", std::set<problem_tag>({ problem_tag::MMP,problem_tag::CONT }));
		RIGIESTER(problem, valleys, "valleys", std::set<problem_tag>({ problem_tag::MMP,problem_tag::CONT }));
		RIGIESTER(problem, vincent, "vincent", std::set<problem_tag>({ problem_tag::MMP,problem_tag::CONT }));
		RIGIESTER(problem, waves, "waves", std::set<problem_tag>({ problem_tag::MMP,problem_tag::CONT }));
		/*CEC2013_MMP*/
		RIGIESTER(problem, CEC2013_MMP_F1, "F1_five_uneven_peak_trap", std::set<problem_tag>({ problem_tag::MMP,problem_tag::CONT }));
		RIGIESTER(problem, CEC2013_MMP_F2, "F2_equal_maxima", std::set<problem_tag>({ problem_tag::MMP,problem_tag::CONT }));
		RIGIESTER(problem, CEC2013_MMP_F3, "F3_uneven_de_maxima", std::set<problem_tag>({ problem_tag::MMP,problem_tag::CONT }));
		RIGIESTER(problem, CEC2013_MMP_F4, "F4_himmenblau", std::set<problem_tag>({ problem_tag::MMP,problem_tag::CONT }));
		RIGIESTER(problem, CEC2013_MMP_F5, "F5_six_hump_camel_back", std::set<problem_tag>({ problem_tag::MMP,problem_tag::CONT }));
		RIGIESTER(problem, CEC2013_MMP_F6, "F6_shubert", std::set<problem_tag>({ problem_tag::MMP,problem_tag::CONT }));
		RIGIESTER(problem, CEC2013_MMP_F7, "F7_vincent", std::set<problem_tag>({ problem_tag::MMP,problem_tag::CONT }));
		RIGIESTER(problem, CEC2013_MMP_F8, "F8_modified_rastrigin", std::set<problem_tag>({ problem_tag::MMP,problem_tag::CONT }));
		RIGIESTER(problem, CEC2013_MMP_F9, "F9_composition1", std::set<problem_tag>({ problem_tag::MMP,problem_tag::CONT }));
		RIGIESTER(problem, CEC2013_MMP_F10, "F10_composition2", std::set<problem_tag>({ problem_tag::MMP,problem_tag::CONT }));
		RIGIESTER(problem, CEC2013_MMP_F11, "F11_composition3", std::set<problem_tag>({ problem_tag::MMP,problem_tag::CONT }));
		RIGIESTER(problem, CEC2013_MMP_F12, "F12_composition4", std::set<problem_tag>({ problem_tag::MMP,problem_tag::CONT }));
		/*CEC2015_MMP*/
		RIGIESTER(problem, CEC2015_MMP_F1, "F1_SR_expanded_two_peak_trap", std::set<problem_tag>({ problem_tag::MMP,problem_tag::CONT }));
		RIGIESTER(problem, CEC2015_MMP_F2, "F2_SR_expanded_five_uneven_peak_trap", std::set<problem_tag>({ problem_tag::MMP,problem_tag::CONT }));
		RIGIESTER(problem, CEC2015_MMP_F3, "F3_SR_expanded_equal_minima", std::set<problem_tag>({ problem_tag::MMP,problem_tag::CONT }));
		RIGIESTER(problem, CEC2015_MMP_F4, "F4_SR_expanded_decreasing_minima", std::set<problem_tag>({ problem_tag::MMP,problem_tag::CONT }));
		RIGIESTER(problem, CEC2015_MMP_F5, "F5_SR_expanded_uneven_minima", std::set<problem_tag>({ problem_tag::MMP,problem_tag::CONT }));
		RIGIESTER(problem, CEC2015_MMP_F6, "F6_SR_expanded_himmelblau", std::set<problem_tag>({ problem_tag::MMP,problem_tag::CONT }));
		RIGIESTER(problem, CEC2015_MMP_F7, "F7_SR_expanded_six_hump_camel_back", std::set<problem_tag>({ problem_tag::MMP,problem_tag::CONT }));
		RIGIESTER(problem, CEC2015_MMP_F8, "F8_SR_modified_vincent", std::set<problem_tag>({ problem_tag::MMP,problem_tag::CONT }));
		RIGIESTER(problem, CEC2015_MMP_F9, "F9_composition2015_C1", std::set<problem_tag>({ problem_tag::MMP,problem_tag::CONT }));
		RIGIESTER(problem, CEC2015_MMP_F10, "F10_composition2015_C2", std::set<problem_tag>({ problem_tag::MMP,problem_tag::CONT }));
		RIGIESTER(problem, CEC2015_MMP_F11, "F11_composition2015_C3", std::set<problem_tag>({ problem_tag::MMP,problem_tag::CONT }));
		RIGIESTER(problem, CEC2015_MMP_F12, "F12_composition2015_C4", std::set<problem_tag>({ problem_tag::MMP,problem_tag::CONT }));
		RIGIESTER(problem, CEC2015_MMP_F13, "F13_composition2015_C5", std::set<problem_tag>({ problem_tag::MMP,problem_tag::CONT }));
		RIGIESTER(problem, CEC2015_MMP_F14, "F14_composition2015_C6", std::set<problem_tag>({ problem_tag::MMP,problem_tag::CONT }));
		RIGIESTER(problem, CEC2015_MMP_F15, "F15_composition2015_C7", std::set<problem_tag>({ problem_tag::MMP,problem_tag::CONT }));
		/*realworld*/
		RIGIESTER(problem, gear_train, "gear_Train", std::set<problem_tag>({ problem_tag::GOP,problem_tag::CONT }));
		RIGIESTER(problem, par_est_FM_sound_waves, "par_est_FM_sound_waves", std::set<problem_tag>({ problem_tag::GOP,problem_tag::CONT }));
		RIGIESTER(problem, epanet, "epanet", std::set<problem_tag>({ problem_tag::GOP}));
	}

	void register_algorithm() {
		RIGIESTER(algorithm, FNS_NSGAII, "NSGAII", std::set<problem_tag>({ problem_tag::MOP,problem_tag::CONT }));
		RIGIESTER(algorithm, JADE, "JADE", std::set<problem_tag>({ problem_tag::GOP,problem_tag::CONT }));
		RIGIESTER(algorithm, CRDE, "CRDE", std::set<problem_tag>({ problem_tag::GOP,problem_tag::CONT }));
		RIGIESTER(algorithm, DynDE, "DynDE", std::set<problem_tag>({ problem_tag::GOP,problem_tag::CONT }));
		RIGIESTER(algorithm, jDE, "jDE", std::set<problem_tag>({ problem_tag::GOP,problem_tag::CONT }));
		RIGIESTER(algorithm, NCDE, "NCDE", std::set<problem_tag>({ problem_tag::MMP,problem_tag::CONT }));
		RIGIESTER(algorithm, NSDE, "NSDE", std::set<problem_tag>({ problem_tag::MMP,problem_tag::CONT }));
		RIGIESTER(algorithm, SaDE, "SaDE", std::set<problem_tag>({ problem_tag::MMP,problem_tag::CONT }));
		//RIGIESTER(algorithm, COGL, "COGL", std::set<problem_tag>({ problem_tag::MMP, problem_tag::DOP, problem_tag::LSOP}));
	}

	void set_global_parameters(int argn, char * argv[]) {
		//format of argument list from main: param_proName=FUN_Sphere param_numDim=5  param_numPeak=10 
		register_paramter(); //register all system parameters first
		std::string letter;
		for (auto i = 'a'; i <= 'z'; i++) {
			letter += i;
			letter += i - 32;
		}
		std::string remove = "\r\t\n\b\v";
		letter += "\\/:";
		for (int i = 1; i<argn; i++) {
			std::string par = argv[i];
			while (size_t pos = par.find_first_of(remove)) {
				if (std::string::npos == pos) break;
				par.erase(par.begin() + pos);
			}
			size_t pos = par.find('=');
			if (pos == std::string::npos)  throw myexcept("Invalid argument:gSetGlobalParameters()");
			std::string value = par.substr(pos + 1, par.size() - 1), name = par.substr(0, pos);
			name.insert(0, "param_");
			if (global::ms_param.find(name) == global::ms_param.end()) throw myexcept("Invalid argument:gSetGlobalParameters()");
			if (value.compare("true") == 0) {
				global::ms_arg[global::ms_param[name]] = true;
			}
			else if (value.compare("false") == 0) {
				global::ms_arg[global::ms_param[name]] = false;
			}
			else if (value.find_first_of(letter) != std::string::npos) {
				if (value.size() == 1)
					global::ms_arg[global::ms_param[name]] = value[0];
				else
					global::ms_arg[global::ms_param[name]] = value;
			}
			else if (std::string::npos != par.find('.', pos + 1)) {
				double val = atof(value.c_str());
				global::ms_arg[global::ms_param[name]] = val;
			}
			else {
				int val = atoi(value.c_str());
				global::ms_arg[global::ms_param[name]] = val;
			}
		}
#ifdef OFEC_CONSOLE
		if (global::ms_arg.find(param_sampleFre) == global::ms_arg.end()) global::ms_arg[param_sampleFre] = 2;
		if (global::ms_arg.find(param_workingDir) == global::ms_arg.end())  global::ms_arg[param_workingDir] = std::string("./iofile/");
#endif
		if (global::ms_arg.find(param_dataFile1) != global::ms_arg.end())
		{
			std::string str = global::ms_arg[param_dataFile1];
			size_t i;
			if ((i = str.find(".tsp")) == std::string::npos) return;
			str.erase(i, 4);
			for (i = str.size() - 1; i >= 0; i--)
			{
				if (str[i] >= '0'&&str[i] <= '9')
					continue;
				else break;
			}
			std::string s(str, i + 1);
			if (global::ms_arg.find(param_numDim) == global::ms_arg.end())
				global::ms_arg[param_numDim] = atoi(s.c_str());
			if (global::ms_arg.find(param_popSize) == global::ms_arg.end())
				global::ms_arg[param_popSize] = atoi(s.c_str());
		}
	}

	void run() {
		//std::vector<std::string> headers({ "Evals","IGD", "numComp" });
		std::vector<std::string> headers({ "Iter", "NumOptFound" });
		
		measure::ms_measure.reset(new measure((int)(global::ms_arg[param_numRun]), headers));

		//int numTask = std::thread::hardware_concurrency();
		int numTask = (int)global::ms_arg[param_numTask];
		std::vector<std::thread> atrd;
		int rest = (int)(global::ms_arg[param_numRun]) % numTask;
		int id1 = 0, id2 = id1 + (int)(global::ms_arg[param_numRun]) / numTask - 1 + (rest-->0 ? 1 : 0);
		for (size_t i = 0; i < numTask; i++) {
			std::vector<int> runs;
			for (int r = id1; r <= id2; r++) runs.push_back(r);
			id1 = id2 + 1;
			id2 = id1 + (int)(global::ms_arg[param_numRun]) / numTask - 1 + (rest-->0 ? 1 : 0);
			atrd.push_back(std::thread(go, runs));
		}
		for (auto&t : atrd) t.join();
		measure::ms_measure->output();
	}

	int go(std::vector<int> runIds) {
		for (auto & runId : runIds) {
			try {
				g_mutex.lock();
				std::cout << "runId: " << runId << std::endl;
				global::ms_global.reset(new global(runId, 1. / 7, (runId + 1.) / ((int)(global::ms_arg[param_numRun]) + 1.)));
				global::ms_global->m_problem.reset(factory<problem>::produce(global::ms_arg[param_proName], global::ms_arg));
				global::ms_global->m_algorithm.reset(factory<algorithm>::produce(global::ms_arg[param_algName], global::ms_arg));
				g_mutex.unlock();
				global::ms_global->m_algorithm->run();
			}
			catch (std::exception &e) {
				g_mutexStream.lock();
				std::cout << "exception, runId " << runId << ": " << e.what() << std::endl;
				g_mutexStream.unlock();
			}
		}
		return 0;
	}
}
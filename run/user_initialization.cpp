#include "user_initialization.h"
//#include "include_algorithm.h"
//#include "include_problem.h"
#include "../core/measure/measure.h"

namespace OFEC {

	void register_parameter() {
		// ******* user-defined parameters ***
		global::ms_param.insert({ "OA","objectiveAccuracy" });
		global::ms_param.insert({ "UVP","useVariablePartition" });
		global::ms_param.insert({ "F","scalingFactor" });
		global::ms_param.insert({ "Cr","crossoverRate" });
		global::ms_param.insert({ "hlcNS","hlcNumSegment" });

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
		global::ms_param.insert({ "et","eta" });
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

		global::ms_param.insert({ "DFR","dataFileResult" }); 
		global::ms_param.insert({ "MRT","maxRunTime" }); 
	}

	void before_run() {
		//setup the headers of format as: "XX","XX", ... 
		std::vector<std::string> headers;
		const auto& pro_tags = global::ms_reg_problem.get().at(global::ms_arg.at("proName")).second;

		if (pro_tags.find(problem_tag::GOP) != pro_tags.end())
			headers = { "Evaluations", "Error" };
		else if (pro_tags.find(problem_tag::MMOP) != pro_tags.end())
			headers = { "Evaluations", "Peak ratio", "Success rate" };
		else if (pro_tags.find(problem_tag::MOP) != pro_tags.end())
			headers = { "Evaluations", "IGD" };

		measure::initialize(global::ms_arg.at("numRun"), headers);
	}

	void after_run() {
		measure::get_measure()->output_progr();
		measure::get_measure()->output_final();
	}
}
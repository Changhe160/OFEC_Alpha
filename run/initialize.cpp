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
	}

	void register_class() {
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


		RIGIESTER(algorithm, FNS_NSGAII, "NSGAII", std::set<problem_tag>({ problem_tag::MOP,problem_tag::CONT }));
		RIGIESTER(algorithm, CS_NSGAII, "CS_NSGAII", std::set<problem_tag>({ problem_tag::MOP,problem_tag::CONT }));
		RIGIESTER(algorithm, DG_NSGAII, "DG_NSGAII", std::set<problem_tag>({ problem_tag::MOP,problem_tag::CONT }));
		RIGIESTER(algorithm, T_ENS_NSGAII, "T_ENS_NSGAII", std::set<problem_tag>({ problem_tag::MOP,problem_tag::CONT }));
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
			int i;
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
		std::vector<std::string> headers({ "Evals","IGD", "numComp" });
		measure::ms_measure.reset(new measure((int)(global::ms_arg[param_numRun]), headers));

		size_t numTask = std::thread::hardware_concurrency();
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

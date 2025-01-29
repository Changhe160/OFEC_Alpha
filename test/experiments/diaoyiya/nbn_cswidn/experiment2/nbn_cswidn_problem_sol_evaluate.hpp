
#include <filesystem>
#include <string>
#include <vector>
#include <iostream>
#include <limits>
#include <algorithm>
#include <iostream>
#include <random>
#include <string>
#include <vector>
#include "../utility/hnsw/hnsw_solution_multithread/hnsw_sol_model.h"
#include "../core/global.h"
#include "../core/problem/optima.h"
#include "../core/definition.h"
#include "../core/global.h"
#include "../utility/nbn_visualization/visualization/tree_graph_simple.h"
#include "../utility/nbn_visualization/calculate_algorithm/nbn_grid_tree_division.h"
#include "../utility/nbn_visualization/calculate_algorithm/nbn_grid_tree_division_allSize.h"
#include  "../utility/function/custom_function.h"
#include "../utility/function/general_function.h"
#include "../utility/nbn_visualization/nbn_calculator/nearest_better_calculator.h"
#include "interface.h"
#include <queue>
#include "../core/definition.h"
#include <deque>
#include <chrono>  

#include "../utility/hnsw/hnsw_nbn/neighbor_info.h"
#include "../utility/hnsw/hnsw_nbn/select_policy.h"
#include "../utility/hnsw/hnsw_nbn/node.h"
#include "../utility/hnsw/hnsw_nbn/hnsw_model.h"
#include "../utility/hnsw/hnsw_nbn/hnsw_nbn.h"
#include "../core/environment/environment.h"
#include "../core/parameter/variants_to_stream.h"
#include<filesystem>
#include<queue>
#include <set>
#include "../core/parameter/variants_to_stream.h"
#include <iostream>
#include <iomanip>
#include <ctime>
#include <sstream>

//#include "../utility/visualization/idee_trait.h"
#include "../instance/problem/realworld/csiwdn/csiwdn.h"
#include "../utility/nbn_visualization/instance/cswidn/cswidn_utility.h"


#include "../utility/function/custom_function.h"
#include "../utility/function/general_function.h"
#include "../utility/function/stream_function.h"




void createEnvironment(std::shared_ptr<ofec::Environment>& env,
	int stage,
	const std::string& processname) {
	using namespace std;
	using namespace ofec;
	ofec::ParameterMap params;


	std::string proname = "CSIWDN";
	params["problem name"] = std::string("CSIWDN");
	params["net-sensor-source case"] = std::string("Net2/sensor_case1/source/case11.txt");

	//	params["dataFile2"] = std::string("case1");
	//	params["dataFile3"] = std::string("case11");
	//	params["use LSTM"] = false;
//	std::shared_ptr<Environment> env;
	env.reset(Environment::create());
	env->recordInputParameters();
	env->initialize();
	env->setProblem(ofec::Factory<ofec::Problem>::produce(proname));
	env->problem()->inputParameters().input(params);
	env->problem()->recordInputParameters();


	auto cswidnPro = CAST_CSIWDN(env->problem());
	cswidnPro->setProcessName(processname);
	env->initializeProblem(0.5);

	cswidnPro->setDistanceType(ofec::CSIWDN::DistanceType::Mix);
	cswidnPro->setPhase(stage);


}

struct proInfo {

	const int m_stage = 3;
	const int m_numSample = 1e6;
	ofec::CSIWDN::DistanceType m_initType = ofec::CSIWDN::DistanceType::Mix;
	double m_initRadius = 1.0;
	double m_aroundConCenterRadius = 0;
	int m_aoundIntCenterRaduis = 0;
	std::shared_ptr<ofec::Environment> m_env;
	std::shared_ptr<ofec::Random> m_rnd;
	std::shared_ptr<ofec::SolutionBase> m_center;
	void initialize(const std::string& processname) {
		//m_initRadius = pow(0.1, m_precision);
		modify();
		m_rnd.reset(new ofec::Random(0.5));
		createEnvironment(m_env, m_stage, processname);
		createCenterSolution();

	}




	std::string getFileName()const {
		std::string nbnType = "Mix";
		std::string aourndRadiusName;

		switch (m_initType)
		{
		case ofec::CSIWDN::DistanceType::Mix: {
			nbnType = "Mix";
			aourndRadiusName = "0";

			break;
		}
		case ofec::CSIWDN::DistanceType::Integer:
		{
			nbnType = "Integer";
			aourndRadiusName = std::to_string(int(m_aroundConCenterRadius * 1e6));
			break;
		}
		case ofec::CSIWDN::DistanceType::Continous:
		{
			nbnType = "Continous";
			aourndRadiusName = std::to_string(m_aoundIntCenterRaduis);
			break;
		}
		default:
			break;
		}

		return "CSIWDN_" + nbnType + "_aroundRadius_" + aourndRadiusName + "_initRadiusPrecision_" + std::to_string(int(m_initRadius * 1e6));
	}

	void modify() {
		switch (m_initType)
		{
		case ofec::CSIWDN::DistanceType::Mix: {
			m_aroundConCenterRadius = 0;
			m_aoundIntCenterRaduis = 0;
			break;
		}
		case ofec::CSIWDN::DistanceType::Integer:
		{
			m_aoundIntCenterRaduis = 0;

			break;
		}
		case ofec::CSIWDN::DistanceType::Continous:
		{
			m_aroundConCenterRadius = 0;
			break;
		}
		default:
			break;
		}
	}



	void createCenterSolution() {
		using namespace ofec;
		auto cswidnPro = CAST_CSIWDN(m_env->problem());
		cswidnPro->setPhase(cswidnPro->totalPhase());
		std::vector<std::shared_ptr<ofec::SolutionBase>> sols(1e6);
		auto& optBase = cswidnPro->optimaBase()->solutionBase(0);
		m_center.reset(cswidnPro->createSolution(optBase));
		auto& cursol = dynamic_cast<ofec::CSIWDN::solutionType&>(*m_center);
		cursol.variable().flagLocation() = false;
		if (m_aoundIntCenterRaduis > 0) {
			ofec::nbn::cswidn::generateSolIntRasiu(m_center, optBase, m_aoundIntCenterRaduis, m_env.get(), m_rnd.get());
		}
		if (m_aroundConCenterRadius > 0) {
			ofec::nbn::cswidn::generateSolRasiu(m_center, optBase, m_aroundConCenterRadius, m_env.get(), m_rnd.get());
		}

	}






	void genereateSols(std::vector<std::shared_ptr<ofec::SolutionBase>>& sols) {
		sols.resize(m_numSample);
		if (m_initType == ofec::CSIWDN::DistanceType::Continous) {
			ofec::nbn::cswidn::generateSolutionsConRadius(*m_center, m_initRadius, sols, m_env.get(), m_rnd.get());
		}
		else if (m_initType == ofec::CSIWDN::DistanceType::Integer) {
			ofec::nbn::cswidn::generateSolutionsInts(*m_center, sols, m_env.get(), m_rnd.get());
		}
		else {
			UTILITY::generateRandomSolutionsMultiThreads(sols, m_env.get(), m_rnd.get());
		}
	}

};


bool waitForFile(const std::string& saveDir, const std::string& filename, const std::string& process_name) {
	std::vector<std::filesystem::path> results;
	if (ofec::utility::file_exists_with_prefix(results, saveDir, filename)) {
		return false;
	}
	auto testfilename = filename + process_name + ".dat";
	{
		std::ofstream out(saveDir + testfilename);
		out << "yes" << std::endl;
		out.close();
	}
	// 获取当前时间
	auto now = std::chrono::steady_clock::now();
	// 设置2分钟后的时间
	auto wait_until = now + std::chrono::minutes(2);
	// 让当前线程等待，直到 wait_until
	std::this_thread::sleep_until(wait_until);
	ofec::utility::file_exists_with_prefix(results, saveDir, filename);
	std::sort(results.begin(), results.end());
	if (results.front() == testfilename) {
		return true;
	}
	return false;
}


void runTotalTasks(const std::string& saveDir, const std::string& processName) {

	using namespace ofec;
	using namespace std;


	std::list<proInfo>infos;

	std::vector<int> phases = { 3 };
	// 时间相关
//	phases = { 1 };
	double curConRatio = 0;
	curConRatio = 1.0;
	for (auto& curphase : phases) {
		proInfo localInfo;
		localInfo.m_initType = ofec::CSIWDN::DistanceType::Continous;
		localInfo.m_initRadius = curConRatio;
		infos.push_back(localInfo);
	}

	for (auto& curphase : phases) {
		proInfo localInfo;
		localInfo.m_initType = ofec::CSIWDN::DistanceType::Integer;
		infos.push_back(localInfo);
	}

	{
		proInfo localInfo;
		localInfo.m_initType = ofec::CSIWDN::DistanceType::Mix;
		infos.push_back(localInfo);
	}


	// 迁移相关

	std::vector<double> disRange = { 1e-1,1e-2,1e-3, 1e-4 ,1e-5,1e-6 };
	double defaultStage = 28;

	for (auto& curRoundDis : disRange) {
		proInfo localInfo;
		localInfo.m_initType = ofec::CSIWDN::DistanceType::Integer;
		localInfo.m_aroundConCenterRadius = curRoundDis;
		infos.push_back(localInfo);
	}

	std::vector<int> intRange = { 0, 1,2, 3 };

	//curConRatio = 0.01;
	//for (auto& intDis : intRange) {
	//	proInfo localInfo;
	//	localInfo.m_initType = ofec::CSIWDN::DistanceType::Continous;
	//	localInfo.m_aoundIntCenterRaduis = intDis;
	//	infos.push_back(localInfo);
	//}
	curConRatio = 1.0;
	for (auto& intDis : intRange) {
		proInfo localInfo;
		localInfo.m_initType = ofec::CSIWDN::DistanceType::Continous;
		localInfo.m_aoundIntCenterRaduis = intDis;
		infos.push_back(localInfo);
	}

	// 局部 实验 
	std::vector<double> initDis = { 0.1,0.01,0.001,0.0001,0.00001,1e-6 };
	for (auto& curinit : initDis) {
		proInfo localInfo;
		localInfo.m_initType = ofec::CSIWDN::DistanceType::Continous;
		localInfo.m_initRadius = curinit;
		infos.push_back(localInfo);
	}


	for (auto& it : infos) {

		it.initialize(processName);
		std::vector<std::shared_ptr<SolutionBase>> sols;
		it.genereateSols(sols);
		std::vector<ofec::SolutionBase*> solbases;
		for (auto& it : sols) {
			solbases.push_back(it.get());
		}
		ofec::utility::outputSolutions(saveDir, it.getFileName(), solbases, it.m_env.get());

		//globalInfo.m_proInfos.push_back(it);
	}

}

void evaluateSolution(const std::string& readdir, const std::string& saveDir, const std::string& processname) {


	std::cout << "evaluate \t" << processname << std::endl;
	using namespace ofec;
	const std::string extension = "_solutions.txt";
	auto filenames = ofec::utility::getFilesNamesWithExtension(readdir, extension);
	for (auto& it : filenames) {
		it = ofec::utility::removeSuffix(it, extension);
		//auto infos = UTILITY::split(it, "_");
		//std::cout << infos << std::endl;
		std::cout << it << std::endl;
	}
	std::shared_ptr<ofec::Environment> env;
	createEnvironment(env, 3, processname);
	auto pro = env->problem();
	auto cswidnPro = CAST_CSIWDN(env->problem());

	//	std::reverse(filenames.begin(), filenames.end());

	std::vector<int> phases = { 3,10,11,17,18,48,7,8,15,16,31,32, 39,40 };
	//	std::reverse(phases.begin(), phases.end());
	ofec::Real pos = (pro->optimizeMode(0) == ofec::OptimizeMode::kMaximize) ? 1 : -1;



	for (auto& curphase : phases) {
		for (auto& inputName : filenames) {


			cswidnPro->setPhase(curphase);
			std::vector<std::shared_ptr<ofec::SolutionBase>> sols;
			ofec::utility::inputSolutions(readdir, inputName, sols, env.get());
			auto curfilename = inputName + "_phase_" + std::to_string(curphase);

			if (waitForFile(saveDir, curfilename, processname)) {
				std::cout << "running\t" << curfilename << std::endl;
				for (auto& it : sols) {
					it->evaluate(env.get(), false);
					it->setFitness(pos * it->objective(0));
				}

				std::vector<double>fitness;
				for (auto& it : sols) {
					fitness.push_back(it->fitness());

				}

				{
					ofec::ParameterVariantStream paramsStream;
					paramsStream << fitness;
					ofec::variants_stream::outputToFile(paramsStream, saveDir + curfilename + "_fitness.txt");
				}
			}
		}
	}


}



void evaluateSolutionSelectTask(const std::string& readdir, const std::string& saveDir, const std::string& processname) {

	std::cout << "evaluateSolutionSelectTask" << std::endl;
	std::cout << "evaluate \t" << processname << std::endl;
	using namespace ofec;
	const std::string extension = "_solutions.txt";
	//auto filenames = ofec::utility::getFilesNamesWithExtension(readdir, extension);
	//for (auto& it : filenames) {
	//	it = ofec::utility::removeSuffix(it, extension);
	//	//auto infos = UTILITY::split(it, "_");
	//	//std::cout << infos << std::endl;
	//	std::cout << it << std::endl;
	//}

	std::vector<std::string> filenames = { };

	filenames = { "CSIWDN_Integer_aroundRadius_0_initRadiusPrecision_1000000" };
	filenames = {
		"CSIWDN_Continous_aroundRadius_0_initRadiusPrecision_1",
		"CSIWDN_Integer_aroundRadius_0_initRadiusPrecision_1000000",
		"CSIWDN_Mix_aroundRadius_0_initRadiusPrecision_1000000",
		"CSIWDN_Continous_aroundRadius_0_initRadiusPrecision_1000000"
	};


	std::shared_ptr<ofec::Environment> env;
	createEnvironment(env, 3, processname);
	auto pro = env->problem();
	auto cswidnPro = CAST_CSIWDN(env->problem());

	std::reverse(filenames.begin(), filenames.end());
	// 
	// 

	//	std::reverse(phases.begin(), phases.end());
	ofec::Real pos = (pro->optimizeMode(0) == ofec::OptimizeMode::kMaximize) ? 1 : -1;

	double phaseStep = (48 - 3 + 1) / 10;
	for (auto& inputName : filenames) {

		/*for (int idx(48); idx >= 3; --idx)*/ {
			int idx = 4;
			double phaseRound = idx;
			int curphase = std::round(phaseRound);
			if (curphase >= 48) curphase = 48;


			cswidnPro->setPhase(curphase);

			auto curfilename = inputName + "_phase_" + std::to_string(curphase);

			/*if (waitForFile(saveDir, curfilename, processname))*/ {

				std::cout << "running\t" << curfilename << std::endl;
				std::vector<std::shared_ptr<ofec::SolutionBase>> sols;
				ofec::utility::inputSolutions(readdir, inputName, sols, env.get());
				for (auto& it : sols) {
					it->evaluate(env.get(), false);
					it->setFitness(pos * it->objective(0));
				}

				std::vector<double>fitness;
				for (auto& it : sols) {
					fitness.push_back(it->fitness());

				}

				{
					ofec::ParameterVariantStream paramsStream;
					paramsStream << fitness;
					ofec::variants_stream::outputToFile(paramsStream, saveDir + curfilename + "_fitness.txt");
				}
			}
		}
	}


}





void runOFECshell(const std::string& filepath) {
	std::ofstream out(filepath);

	for (int idx(80); idx < 80 + 35; ++idx) {
		out << "param" << idx << "=\"process" << idx << " " << idx << "\"\n";
		out << "./OFEC $param" << idx << " & " << std::endl;
	}

	out << "wait" << std::endl;
	out.close();
}



void evaluateSolTaskProcess(const std::string processName) {
	std::cout << "evaluateSolTask" << std::endl;
	ofec::g_working_directory = "//172.24.207.203/share/2018/diaoyiya/ofec-data";
	ofec::g_working_directory = "/home/lab408/share/2018/diaoyiya/ofec-data";
	//ofec::g_working_directory = "//172.29.204.96/share/2018/diaoyiya/ofec-data";
	//ofec::g_working_directory = "//172.29.203.176/e/DiaoYiya/code/data/ofec-data";
	//ofec::g_working_directory = "E:/DiaoYiya/code/data/ofec-data";
	//ofec::g_working_directory = "/mnt/Data/Student/2018/YiyaDiao/code_total/data";
	//ofec::g_working_directory = "//172.24.242.8/share/Student/2018/YiyaDiao/code_total/data";
	auto saveDir = ofec::g_working_directory + "/paper_cswidn/cswidn_sampling_phase/";
	auto saveDir2 = ofec::g_working_directory + "/paper_cswidn/cswidn_sampling_phase_fitness/";
	std::filesystem::create_directories(saveDir2);
	std::cout << saveDir2 << std::endl;
	//runOFECshell(saveDir+"run_multiple_ofec40-80.sh");
	evaluateSolutionSelectTask(saveDir, saveDir2, processName);
}






namespace ofec {

	void registerParamAbbr() {}
	void customizeFileName() {}
	void run(int argc, char* argv[]) {

		using namespace ofec;
		using namespace std;

		registerInstance();


		std::string filename = "run_test";
		std::cout << "sampling in CSWIDN" << std::endl;
		// 打印参数个数和每个参数的值
		std::cout << "Number of command-line arguments: " << argc << std::endl;
		for (int i = 0; i < argc; ++i) {
			std::cout << "Argument " << i << ": " << argv[i] << std::endl;
			std::string filename = std::string(argv[i]);
			std::cout << "process name\t" << filename << std::endl;
		}


		//runOFECshell("run_multiple_80_35.sh");



		evaluateSolTaskProcess(argv[1]);


		//runTotalTasks(saveDir,"PROCESS2");




		//std::cout << "process name\t" << filename << std::endl;
		//runTask();
		//runTotalTasks(filename);


	}


}
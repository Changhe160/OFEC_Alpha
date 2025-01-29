
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
#include "../utility/function/custom_function.h"
#include "../utility/function/general_function.h"
#include "../utility/nbn_visualization/nbn_calculator/nearest_better_calculator.h"
//#include "../instance/problem/combination/travelling_salesman/travelling_salesman.h"
#include "interface.h"
//#include "../utility/function/custom_function.h"
//#include "../core/algorithm/population.h"
//#include "../utility/nondominated_sorting/filter_sort.h"
//#include "../instance/algorithm/visualize/sampling/instance/sampling_eax_tsp.h"
//#include "../instance/algorithm/visualize/sampling/sampling_multiThread.h"
//#include "../instance/problem/combination/travelling_salesman/travelling_salesman.h"
#include <queue>
#include "../core/definition.h"
#include <deque>
#include <chrono>   
//#include "../utility/nbn_visualization/nbn_calculator/nearest_better_calculator_multiThead.h"

#include "../utility/hnsw/hnsw_nbn/neighbor_info.h"
#include "../utility/hnsw/hnsw_nbn/select_policy.h"
#include "../utility/hnsw/hnsw_nbn/node.h"
#include "../utility/hnsw/hnsw_nbn/hnsw_model.h"
#include "../utility/hnsw/hnsw_nbn/hnsw_nbn.h"
#include "../core/environment/environment.h"
#include "../core/parameter/variants_to_stream.h"

//#include "../instance/algorithm/combination/lkh/lkh_algorithm.h"
#include <filesystem>
#include <queue>
#include <set>

#include "../core/parameter/variants_to_stream.h"
#include <iostream>
#include <iomanip>
#include <ctime>
#include <sstream>

#include "../instance/algorithm/visualize/sampling/sampling_multiThread.h"
#include "../instance/algorithm/visualize/sampling/sampling_algorithm_template.h"
#include "../instance/algorithm/visualize/sampling/instance/cswidn/sampling_cswidn_amp_cc.h"

#include "../utility/nbn_visualization/instance/travelling_salemans_problem/nbn_modify_tsp.h"
#include "../utility/nbn_visualization/calculate_algorithm/nbn_iteration_multithread.h"
#include "../utility/nbn_visualization/nbn_calculator/nearest_better_calculator.h"
#include "../utility/nbn_visualization/nbn_fla/nbn_fla.h"
#include "../utility/nbn_visualization/nbn_fla/nbn_fla_utility.h"


#include "../utility/hnsw/hnsw_nbn/neighbor_info.h"
#include "../utility/hnsw/hnsw_nbn/select_policy.h"
#include "../utility/hnsw/hnsw_nbn/node.h"
#include "../utility/hnsw/hnsw_nbn/hnsw_model.h"
#include "../utility/hnsw/hnsw_nbn/hnsw_nbn.h"
#include "../core/environment/environment.h"
#include "../core/parameter/variants_to_stream.h"
#include "../utility/function/custom_function.h"
#include "../utility/function/stream_function.h"
#include "../core/parameter/variants_to_stream.h"

#include "../utility/matlab/matlab_utility.h"


#include <string>
#include <string_view>


void modifyNBN(
	ofec::nbn::NBNinfo& nbnInfo,
	const std::string& saveDir,
	const std::string& saveDir2,
	const std::string& filename,
	ofec::Environment* env,
	ofec::Random* rnd
) {

	auto pro = env->problem();
	std::cout << "intputNBNinfo" << std::endl;
	nbnInfo.inputNBNinfo(saveDir, filename);
	//	nbnInfo.outputVecSolutions<int>(saveDir, filename, env);


	{


		nbnInfo.m_vFitness = UTILITY::valuesToSortedValues(nbnInfo.m_vFitness);

		//	auto filename = tspname + "_randomSamples" + "_neighborK_" + std::to_string(neighborK);
		std::vector<int> solIds(nbnInfo.m_belong.size());
		for (int idx(0); idx < solIds.size(); ++idx) {
			solIds[idx] = idx;
		}
		std::vector<ofec::TreeGraphSimple::NBNdata> nbn_data;
		ofec::transferNBNData(nbn_data, solIds, nbnInfo.m_belong, nbnInfo.m_dis2parent, nbnInfo.m_vFitness);
		std::cout << "setNBNdata" << std::endl;
		ofec::TreeGraphSimple nbn_graph;
		nbn_graph.setNBNdata(nbn_data);

		std::cout << "calNetwork" << std::endl;
		//nbn_graph.modifyBestSols(rnd.get(), env->problem(), nbnInfo.solbases);
		nbn_graph.calNetwork();
		std::string savepath = saveDir2 + filename + "_network.txt";
		std::ofstream out(savepath);
		nbn_graph.outputNBNnetwork(out);
		out.close();
		ouputNBNdata(saveDir2 + filename + "_nbn.txt", nbn_graph.get_nbn_data());
	}
}


void calculateNBN(
	ofec::nbn::NBNinfo& nbnInfo,
	const std::string& saveDir,
	const std::string& saveDir2,
	const std::string& filename,
	ofec::Environment* env,
	ofec::Random* rnd
) {

	//std::string proname = "TSP";
	auto pro = env->problem();
	//auto rnd = std::make_shared<ofec::Random>(0.5);
	//auto tsp_pro = dynamic_cast<ofec::TravellingSalesman*>(pro);

	nbnInfo.calculateNBNhnswEqualBetterRandom(env, rnd);



	//udpateNBNinfoIteration(nbnInfo,
	//	1e3, env, rnd);
	std::cout << "outputNBNinfo" << std::endl;
	nbnInfo.outputNBNinfo(saveDir, filename);
	//	nbnInfo.outputVecSolutions<int>(saveDir, filename, env);

	{


		std::cout << "output solutions" << std::endl;
		ofec::ParameterVariantStream paramsStream;
		paramsStream << nbnInfo.m_solbases.size();
		for (auto& it : nbnInfo.m_solbases) {
			pro->solutionToParameterVariants(*it, paramsStream);
		}
		ofec::variants_stream::outputToFile(paramsStream, saveDir + filename + ".txt");

	}

	{


		nbnInfo.m_vFitness = UTILITY::valuesToSortedValues(nbnInfo.m_vFitness);

		//	auto filename = tspname + "_randomSamples" + "_neighborK_" + std::to_string(neighborK);
		std::vector<int> solIds(nbnInfo.m_belong.size());
		for (int idx(0); idx < solIds.size(); ++idx) {
			solIds[idx] = idx;
		}
		std::vector<ofec::TreeGraphSimple::NBNdata> nbn_data;
		ofec::transferNBNData(nbn_data, solIds, nbnInfo.m_belong, nbnInfo.m_dis2parent, nbnInfo.m_vFitness);
		std::cout << "setNBNdata" << std::endl;
		ofec::TreeGraphSimple nbn_graph;
		nbn_graph.setNBNdata(nbn_data);

		std::cout << "calNetwork" << std::endl;
		//nbn_graph.modifyBestSols(rnd.get(), env->problem(), nbnInfo.solbases);
		nbn_graph.calNetwork();


		std::string savepath = saveDir2 + filename + "_network.txt";
		std::ofstream out(savepath);
		nbn_graph.outputNBNnetwork(out);
		out.close();
		ouputNBNdata(saveDir2 + filename + "_nbn.txt", nbn_graph.get_nbn_data());
	}



}


void genenratePro(
	const std::string& proname,
	ofec::ParameterMap& params,
	std::shared_ptr<ofec::Environment>& env) {
	using namespace ofec;


	env.reset(Environment::create());
	env->recordInputParameters();
	env->initialize(0.5);
	env->setProblem(ofec::Factory<ofec::Problem>::produce(proname));
	env->problem()->inputParameters().input(params);
	env->problem()->setName(proname);
	env->problem()->recordInputParameters();
	env->initializeProblem(0.5);
}


void createCSWIDN_environments(std::shared_ptr<ofec::Environment>& env, const std::string& processName) {
	using namespace ofec;
	using namespace std;
	ofec::ParameterMap params;
	std::string algName = "AMP-GL-SaDE-sampling";
	std::string proname = "CSIWDN";
	params["problem name"] = std::string("CSIWDN");
	params["net-sensor-source case"] = std::string("Net2/sensor_case1/source/case11.txt");
	params["maximum evaluations"] = int(200000);
	params["process name"] = processName;
	int numRun = 30;

	genenratePro(proname, params, env);
}

std::string removePhaseStr(const std::string& str, int& curphase) {

	std::string ret = str;
	size_t pos = str.find("_phase_");
	std::string phaseStr;
	size_t pos2 = str.find_last_of("_");
	phaseStr = str.substr(pos2 + 1, str.size());

	curphase = std::stoi(phaseStr);
	if (pos != std::string::npos) {
		ret = ret.substr(0, pos);
	}
	return ret;
}

void calNBNallTasks(const std::string& solDir, const std::string& fitDir, const std::string& saveDir) {
	using namespace ofec;
	using namespace ofec::utility;

	const std::string fitExtension = "_fitness.txt";
	auto filenames = getFilesNamesWithExtension(fitDir, fitExtension);
	for (auto& it : filenames) {
		it = removeSuffix(it, fitExtension);
		//auto infos = UTILITY::split(it, "_");
		//std::cout << infos << std::endl;
		std::cout << it << std::endl;
	}
	std::shared_ptr<ofec::Environment> env;
	createCSWIDN_environments(env, "process_1");
	auto rnd = std::make_shared<ofec::Random>(0.5);
	auto pro = env->problem();

	auto cswidnPro = CAST_CSIWDN(pro);
	for (auto& it : filenames) {
		auto& curfilename = it;

		std::cout << curfilename << std::endl;
		int curphase(0);
		auto solFilename = removePhaseStr(curfilename, curphase);
		cswidnPro->setPhase(curphase);
		std::vector<std::shared_ptr<ofec::SolutionBase>> sols;
		{
			utility::inputSolutions(solDir, solFilename, sols, env.get());
			//{
			//	ofec::ParameterVariantStream paramsStream;
			//	ofec::variants_stream::inputFromFile(paramsStream, solDir + solFilename + "_solutions.txt");
			//	size_t inputSize(0);
			//	paramsStream >> inputSize;
			//	sols.resize(inputSize);
			//	for (auto& it : sols) {
			//		it.reset(env->problem()->createSolution());
			//		pro->parameterVariantsToSolution(paramsStream, *it);
			//	}

			//}

			std::vector<double> fits;
			{
				ofec::ParameterVariantStream paramsStream;
				ofec::variants_stream::inputFromFile(paramsStream, fitDir + curfilename + fitExtension);
				paramsStream >> fits;

			}

			for (int idx(0); idx < sols.size(); ++idx) {
				sols[idx]->setFitness(fits[idx]);
			}
		}
		auto pro = env->problem();
		ofec::nbn::NBNinfo nbnInfo;
		nbnInfo.m_sols = sols;
		for (auto& it : nbnInfo.m_sols) {
			nbnInfo.m_solbases.push_back(it.get());
		}
		calculateNBN(nbnInfo, saveDir, saveDir, curfilename, env.get(), rnd.get());
	}
}

void runOFECshell(const std::string& filepath) {
	std::ofstream out(filepath);
	for (int idx(0); idx <= 5; ++idx) {
		out << "param" << idx << "=\"process" << idx << " " << idx << "\"\n";
		out << "./OFEC $param" << idx << " & " << std::endl;
	}

	out << "wait" << std::endl;
	out.close();
}



void outputCSWIDNpro_figureData(const std::string& saveDir) {

	using namespace ofec;

	std::shared_ptr<ofec::Environment> env;
	createCSWIDN_environments(env, "process_1");
	auto rnd = std::make_shared<ofec::Random>(0.5);
	auto pro = env->problem();

	auto cswidnPro = CAST_CSIWDN(pro);
	//	auto nodes= 


	auto& nodeNames = cswidnPro->nodeName();
	auto nodeIds = cswidnPro->nodeId();
	auto& cor = cswidnPro->getCoordinates();
	auto& aja = cswidnPro->getDirectedAdjmatrix();

	std::vector<std::vector<int>> edges;
	for (int idx(0); idx < aja.size(); ++idx) {
		for (int idy(0); idy < aja[idx].size(); ++idy) {
			if (aja[idx][idy]) {
				edges.push_back({ idx, idy });
			}
		}
	}


	std::vector<std::string> lake_name = { "Lake" };
	std::vector<std::string> river_name = { "River" };
	std::vector<std::string> tank_name = { "1", "2", "3" };

	std::vector<int> lakeIds;
	for (auto& it : lake_name) {
		lakeIds.push_back(nodeIds[it]);
	}
	for (auto& it : lake_name) {
		nodeIds.erase(it);
	}

	std::vector<int> riverIds;
	for (auto& it : river_name) {
		riverIds.push_back(nodeIds[it]);
	}

	for (auto& it : river_name) {
		nodeIds.erase(it);
	}

	std::vector<int> tankIds;
	for (auto& it : tank_name) {
		tankIds.push_back(nodeIds[it]);
	}
	for (auto& it : tank_name) {
		nodeIds.erase(it);
	}


	std::vector<int> junctionIds;
	for (auto& it : nodeIds) {
		junctionIds.push_back(it.second);
	}


	{
		std::ofstream out(saveDir + "edges.txt");
		ofec::matlab::outputMatrix(out, edges);
		out.close();
	}
	{
		std::ofstream out(saveDir + "positions.txt");
		ofec::matlab::outputMatrix(out, cor);
		out.close();
	}
	{
		std::ofstream out(saveDir + "lakeIds.txt");
		ofec::matlab::outputVector(out, lakeIds);
		out.close();
	}
	{
		std::ofstream out(saveDir + "tankIds.txt");
		ofec::matlab::outputVector(out, tankIds);
		out.close();
	}


	{
		std::ofstream out(saveDir + "riverIds.txt");
		ofec::matlab::outputVector(out, riverIds);
		out.close();
	}

	{
		std::ofstream out(saveDir + "junctionIds.txt");
		ofec::matlab::outputVector(out, junctionIds);
		out.close();
	}
	{
		auto sensorIds = cswidnPro->sensorIds();
		std::ofstream out(saveDir + "sensorIds.txt");
		ofec::matlab::outputVector(out, sensorIds);
		out.close();

	}


	std::vector<int> optIds;

	{
		auto& opt = cswidnPro->optima()->solution(0);
		for (int idx(0); idx < cswidnPro->numSource(); ++idx) {
			optIds.push_back(opt.variable().index(idx) - 1);
		}
	}
	{
		std::ofstream out(saveDir + "optIds.txt");
		ofec::matlab::outputVector(out, optIds);
		out.close();

	}


	int stop = -1;
}



void copyFiles(const std::string& fromDir, const std::string& toDir) {
	using namespace ofec;
	using namespace ofec::utility;
	std::string filetype = "_nbn.txt";
	std::string phasetype = "_phase_3";
	auto filenames = getFilesNamesWithExtension(fromDir, phasetype + filetype);

	for (auto& it : filenames) {
		it = removeSuffix(it, filetype);
		std::filesystem::copy(fromDir + it + "_nbn.txt", toDir + it + "_nbn.txt");
		std::filesystem::copy(fromDir + it + "_nbnInfo.txt", toDir + it + "_nbnInfo.txt");
	}
}

void copyFilesPhase3(const std::string& fromDir, const std::string& toDir) {
	using namespace ofec;
	using namespace ofec::utility;

	std::string filetype = "_network.txt";
	std::string phasetype = "_phase_3";
	auto filenames = getFilesNamesWithExtension(fromDir, phasetype + filetype);

	for (auto& it : filenames) {
		it = removeSuffix(it, filetype);
		std::cout << it << std::endl;
		if (!std::filesystem::exists(toDir + it + filetype)) {
			std::filesystem::copy(fromDir + it + filetype, toDir + it + filetype);
		}

		//std::filesystem::copy(fromDir + it + "_nbnInfo.txt", toDir + it + "_nbnInfo.txt");
	}
}



void copyFilesPhase3_phase(const std::string& fromDir, const std::string& toDir) {
	using namespace ofec;
	using namespace ofec::utility;

	std::string filetype = "_network.txt";
	std::string phasetype = "_phase_3";
	auto filenames = getFilesNamesWithExtension(fromDir, phasetype + filetype);

	for (auto& it : filenames) {
		it = removeSuffix(it, filetype);
		std::cout << it << std::endl;
		if (!std::filesystem::exists(toDir + it + filetype)) {
			std::filesystem::copy(fromDir + it + filetype, toDir + it + filetype);
		}

		//std::filesystem::copy(fromDir + it + "_nbnInfo.txt", toDir + it + "_nbnInfo.txt");
	}
}


void copyTask() {

	const std::string& fromdir = "//172.24.34.11/share/2018/diaoyiya/ofec-data/paper_cswidn/cswidn_sampling_phase_fitness_network/";
	const std::string& todir = "//172.24.24.151/f/DiaoYiya/cswidn_exp/cswidn_sampling_phase_fitness_network_phase3/";
	std::filesystem::create_directories(todir);
	//copyFiles(fromdir, todir);


	copyFilesPhase3(fromdir, todir);
}



void calNBNtaskSelectTask() {


	std::cout << "calNBNtaskSelectTask" << std::endl;
	using namespace ofec;
	using namespace ofec::utility;


	//ofec::g_working_directory = "/mnt/Data/Student/2018/YiyaDiao/code_total/data";
	ofec::g_working_directory = "//172.24.242.8/share/Student/2018/YiyaDiao/code_total/data";
	ofec::g_working_directory = "/home/lab408/share/2018/diaoyiya/ofec-data";
	auto solDir = ofec::g_working_directory + "/paper_cswidn/cswidn_sampling_phase/";
	auto fitDir = ofec::g_working_directory + "/paper_cswidn/cswidn_sampling_phase_fitness/";
	auto networkDir = ofec::g_working_directory + "/paper_cswidn/cswidn_sampling_phase_fitness_network/";

	std::filesystem::create_directories(networkDir);

	auto processname = "processUnique";
	std::cout << "evaluate \t" << processname << std::endl;
	using namespace ofec;
	const std::string solExtension = "_solutions.txt";
	auto filenames = getFilesNamesWithExtension(solDir, solExtension);
	for (auto& it : filenames) {
		it = removeSuffix(it, solExtension);
		//auto infos = UTILITY::split(it, "_");
		//std::cout << infos << std::endl;
		std::cout << it << std::endl;
	}
	std::shared_ptr<ofec::Environment> env;
	createCSWIDN_environments(env, processname);
	auto rnd = std::make_shared<ofec::Random>(0.5);
	auto pro = env->problem();


	std::vector<int> phases = { 3,7,8 };
	//	std::reverse(phases.begin(), phases.end());
	ofec::Real pos = (pro->optimizeMode(0) == ofec::OptimizeMode::kMaximize) ? 1 : -1;

	auto fitExten = "_fitness.txt";
	auto networkExten = "_network.txt";


	auto cswidnPro = CAST_CSIWDN(pro);

	for (auto& curphase : phases) {
		for (auto& inputName : filenames) {
			auto curfilename = inputName + "_phase_" + std::to_string(curphase);
			std::cout << "filename\t" << networkDir + curfilename + networkExten << std::endl;
			if (!std::filesystem::exists(networkDir + curfilename + networkExten)) {

				std::cout << "filename\t" << solDir + inputName + solExtension << std::endl;
				std::cout << "filename\t" << fitDir + curfilename + fitExten << std::endl;
				if (std::filesystem::exists(solDir + inputName + solExtension) && std::filesystem::exists(fitDir + curfilename + fitExten)) {
					cswidnPro->setPhase(curphase);
					std::vector<std::shared_ptr<ofec::SolutionBase>> sols;
					{




						utility::inputSolutions(solDir, inputName, sols, env.get());
						std::vector<double> fits;
						{
							ofec::ParameterVariantStream paramsStream;
							ofec::variants_stream::inputFromFile(paramsStream, fitDir + curfilename + fitExten);
							paramsStream >> fits;

						}

						for (int idx(0); idx < sols.size(); ++idx) {
							sols[idx]->setFitness(fits[idx]);
						}
					}
					auto pro = env->problem();
					ofec::nbn::NBNinfo nbnInfo;
					nbnInfo.m_sols = sols;
					for (auto& it : nbnInfo.m_sols) {
						nbnInfo.m_solbases.push_back(it.get());
					}
					calculateNBN(nbnInfo, networkDir, networkDir, curfilename, env.get(), rnd.get());

				}
			}
		}
	}


}



std::string removePhaseSuffix(const std::string& str, int& curphase) {
	std::string result = str;
	size_t pos = result.find("_phase_");
	size_t phasePos = result.find_last_of("_");
	auto phaseStr = result.substr(phasePos + 1, result.size());
	std::cout << "curPhase\t" << phaseStr << std::endl;
	curphase = std::stoi(phaseStr);
	if (pos != std::string::npos) {
		result.erase(pos);
	}
	return result;
}






std::vector<std::string> findFilesWithPrefix(const std::string& directoryStr, const std::string& prefix) {
	namespace fs = std::filesystem;
	fs::path directory = directoryStr; // 这里可以设置你要查找的文件夹路径
	std::vector<std::string> result;
	for (const auto& entry : fs::directory_iterator(directory)) {
		if (fs::is_regular_file(entry) && entry.path().filename().string().starts_with(prefix)) {
			result.push_back(entry.path().filename().string());
		}
	}
	return result;
}



void calNBNinfo() {


	std::cout << "calNBNinfo" << std::endl;
	using namespace ofec;
	using namespace ofec::utility;


	ofec::g_working_directory = "/mnt/Data/Student/2018/YiyaDiao/code_total/data";
	ofec::g_working_directory = "//172.24.242.8/share/Student/2018/YiyaDiao/code_total/data";
	//ofec::g_working_directory = "/home/lab408/share/2018/diaoyiya/ofec-data";
	//auto solDir = ofec::g_working_directory + "/paper_cswidn/cswidn_sampling_phase/";
	//auto fitDir = ofec::g_working_directory + "/paper_cswidn/cswidn_sampling_phase_fitness/";
	auto networkDir = ofec::g_working_directory + "/paper_cswidn/cswidn_algorithm_data_problemPhase_network/";
	networkDir = "//172.24.24.151/f/DiaoYiya/cswidn_exp/CSIWDN_Mix_aroundRadius_0_initRadiusPrecision_1000000/";
	auto analsisData = ofec::g_working_directory + "/paper_cswidn/cswidn_sampling_phase_ananlysis/";
	std::filesystem::create_directories(analsisData);
	auto processname = "processUnique";
	std::cout << "evaluate \t" << processname << std::endl;
	using namespace ofec;
	const std::string fitExtension = "_fitness.txt";
	auto fitExten = "_fitness.txt";
	auto networkExten = "_network.txt";
	const std::string solExtension = "_solutions.txt";




	auto filenames = getFilesNamesWithExtension(networkDir, networkExten);
	for (auto& it : filenames) {
		it = removeSuffix(it, networkExten);
		//auto infos = UTILITY::split(it, "_");
		//std::cout << infos << std::endl;
		std::cout << it << std::endl;
	}
	std::shared_ptr<ofec::Environment> env;
	createCSWIDN_environments(env, processname);
	auto rnd = std::make_shared<ofec::Random>(0.5);
	auto pro = env->problem();


	//std::vector<int> phases = { 3,7,8 };
	//	std::reverse(phases.begin(), phases.end());
	ofec::Real pos = (pro->optimizeMode(0) == ofec::OptimizeMode::kMaximize) ? 1 : -1;



	std::ofstream out(analsisData + "CSIWDN_Mix_aroundRadius_0_initRadiusPrecision_1000000_analysis3.txt");


	auto cswidnPro = CAST_CSIWDN(pro);

	for (auto& curfilename : filenames) {

		/*if (curfilename.starts_with("CSIWDN_Continous_aroundRadius_0_initRadiusPrecision_1_"))*/ {

			std::cout << "filename\t" << networkDir + curfilename + networkExten << std::endl;

			std::cout << "inputNBNinfo" << std::endl;
			ofec::nbn::NBNinfo nbnInfo;
			nbnInfo.inputNBNinfo(networkDir, curfilename);
			//	nbnInfo.outputVecSolutions<int>(saveDir, filename, env);
			std::vector<double> staInfo;
			staInfo.push_back(ofec::nbn::calculateNBDstd(nbnInfo));
			staInfo.push_back(ofec::nbn::calculateNeutralityArea(nbnInfo));

			double meanSTD = ofec::nbn::calculateMeanNBD(nbnInfo);
			staInfo.push_back(meanSTD);
			double maxSTD = ofec::nbn::calculateMaxNBD(nbnInfo);
			staInfo.push_back(maxSTD);

			std::vector<int> optIds;

			ofec::nbn::filterBestSolutions(optIds, nbnInfo, 0.99999, 20);
			staInfo.push_back(optIds.size());
			ofec::nbn::filterBestSolutions(optIds, nbnInfo, 0.9999, 20);
			staInfo.push_back(optIds.size());
			ofec::nbn::filterBestSolutions(optIds, nbnInfo, 0.999, 20);
			staInfo.push_back(optIds.size());

			ofec::nbn::filterBestSolutions(optIds, nbnInfo, 0.995, 20);
			staInfo.push_back(optIds.size());
			ofec::nbn::filterBestSolutions(optIds, nbnInfo, 0.99, 20);
			staInfo.push_back(optIds.size());


			ofec::nbn::filterBestSolutions(optIds, nbnInfo, 0.99, 10);
			staInfo.push_back(optIds.size());

			ofec::nbn::filterBestSolutions(optIds, nbnInfo);
			staInfo.push_back(optIds.size());

			ofec::nbn::getOptimumIdsByDistance(optIds, nbnInfo, meanSTD * 10);
			staInfo.push_back(optIds.size());
			ofec::nbn::getOptimumIdsByDistance(optIds, nbnInfo, meanSTD * 5);
			staInfo.push_back(optIds.size());

			ofec::nbn::getOptimumIdsByDistance(optIds, nbnInfo, meanSTD * 4);
			staInfo.push_back(optIds.size());
			ofec::nbn::getOptimumIdsByDistance(optIds, nbnInfo, meanSTD * 3);
			staInfo.push_back(optIds.size());
			ofec::nbn::getOptimumIdsByDistance(optIds, nbnInfo, meanSTD * 2);
			staInfo.push_back(optIds.size());
			ofec::nbn::getOptimumIdsByDistance(optIds, nbnInfo, meanSTD * 1.5);
			staInfo.push_back(optIds.size());

			auto info = utility::split(curfilename, "_");

			for (auto& it : info) {
				out << it << "\t";
			}
			for (auto& it : staInfo) {
				out << it << "\t";
			}
			out << std::endl;
		}
	}

	out.close();
}

std::string merStr(const std::vector<std::string>& infos) {
	std::string cur;
	if (!infos.empty()) {
		cur = infos.front();
		for (size_t idx(1); idx < infos.size(); ++idx) {
			cur += "_" + infos[idx];
		}
	}

	return cur;
}

void calNBNinfoScalbilityDiff() {


	std::cout << "calNBNinfo" << std::endl;
	using namespace ofec;
	using namespace ofec::utility;


	ofec::g_working_directory = "/mnt/Data/Student/2018/YiyaDiao/code_total/data";
	ofec::g_working_directory = "//172.24.242.8/share/Student/2018/YiyaDiao/code_total/data";
	//ofec::g_working_directory = "/home/lab408/share/2018/diaoyiya/ofec-data";
	//auto solDir = ofec::g_working_directory + "/paper_cswidn/cswidn_sampling_phase/";
	//auto fitDir = ofec::g_working_directory + "/paper_cswidn/cswidn_sampling_phase_fitness/";
	auto networkDir = ofec::g_working_directory + "/paper_cswidn/cswidn_sampling_phase_fitness_network/";
	//	networkDir = "//172.24.24.151/f/DiaoYiya/cswidn_exp/CSIWDN_Mix_aroundRadius_0_initRadiusPrecision_1000000/";
	auto analsisData = ofec::g_working_directory + "/paper_cswidn/cswidn_sampling_phase_ananlysis/";
	std::filesystem::create_directories(analsisData);
	auto processname = "processUnique";
	std::cout << "evaluate \t" << processname << std::endl;
	using namespace ofec;
	const std::string fitExtension = "_fitness.txt";
	auto fitExten = "_fitness.txt";
	auto networkExten = "_network.txt";
	const std::string solExtension = "_solutions.txt";




	auto filenames = getFilesNamesWithExtension(networkDir, networkExten);
	for (auto& it : filenames) {
		it = removeSuffix(it, networkExten);
		//auto infos = UTILITY::split(it, "_");
		//std::cout << infos << std::endl;
		std::cout << it << std::endl;
	}
	std::shared_ptr<ofec::Environment> env;
	createCSWIDN_environments(env, processname);
	auto rnd = std::make_shared<ofec::Random>(0.5);
	auto pro = env->problem();


	//std::vector<int> phases = { 3,7,8 };
	//	std::reverse(phases.begin(), phases.end());
	ofec::Real pos = (pro->optimizeMode(0) == ofec::OptimizeMode::kMaximize) ? 1 : -1;



	std::ofstream out(analsisData + "nbn_diff_info_INT.txt");


	auto cswidnPro = CAST_CSIWDN(pro);
	std::vector<std::vector<std::string>> contInfo;
	std::vector<std::vector<std::string>> intInfo;

	std::set<std::string> setInfos;
	for (auto& it : filenames) {
		setInfos.insert(it);
	}
	for (auto& it : filenames) {
		auto info = split(it, "_");
		if (info[3] == "0") {
			if (info[1] == "Continous") {
				contInfo.push_back(info);
			}
			else {
				intInfo.push_back(info);
			}
		}
	}

	for (auto curinfo : intInfo) {
		auto curfilename = merStr(curinfo);
		std::cout << "filename\t" << networkDir + curfilename + networkExten << std::endl;

		std::vector<double> staInfo;
		ofec::nbn::NBNinfo nbnInfo;
		nbnInfo.inputNBNinfo(networkDir, curfilename);

		std::vector<double> range = { 1,1e1,1e2,1e3,1e4,1e5 };
		for (auto& currange : range) {
			//for (int idx(1); idx <= 3; ++idx) {
			int idx = currange;
			curinfo[3] = std::to_string(idx);
			auto otherfilename = merStr(curinfo);
			if (setInfos.find(otherfilename) == setInfos.end()) {
				staInfo.push_back(-1);
			}
			else {
				ofec::nbn::NBNinfo otherInfo;
				otherInfo.inputNBNinfo(networkDir, otherfilename);
				staInfo.push_back(ofec::nbn::diffNBNRatio(nbnInfo, otherInfo));

			}
		}
		for (auto& it : curinfo) {
			out << it << "\t";
		}
		for (auto& it : staInfo) {
			out << it << "\t";
		}

		out << std::endl;
	}

	out.close();
}


void calNBNinfoChangeDiff() {


	std::cout << "calNBNinfo" << std::endl;
	using namespace ofec;
	using namespace ofec::utility;


	ofec::g_working_directory = "/mnt/Data/Student/2018/YiyaDiao/code_total/data";
	ofec::g_working_directory = "//172.24.242.8/share/Student/2018/YiyaDiao/code_total/data";
	//ofec::g_working_directory = "/home/lab408/share/2018/diaoyiya/ofec-data";
	//auto solDir = ofec::g_working_directory + "/paper_cswidn/cswidn_sampling_phase/";
	//auto fitDir = ofec::g_working_directory + "/paper_cswidn/cswidn_sampling_phase_fitness/";
	auto networkDir = ofec::g_working_directory + "/paper_cswidn/cswidn_sampling_phase_fitness_network/";
	//	networkDir = "//172.24.24.151/f/DiaoYiya/cswidn_exp/CSIWDN_Mix_aroundRadius_0_initRadiusPrecision_1000000/";
	auto analsisData = ofec::g_working_directory + "/paper_cswidn/cswidn_sampling_phase_ananlysis/";
	std::filesystem::create_directories(analsisData);
	auto processname = "processUnique";
	std::cout << "evaluate \t" << processname << std::endl;
	using namespace ofec;
	const std::string fitExtension = "_fitness.txt";
	auto fitExten = "_fitness.txt";
	auto networkExten = "_network.txt";
	const std::string solExtension = "_solutions.txt";


	std::vector<std::string> filenames = {
	"CSIWDN_Continous_aroundRadius_0_initRadiusPrecision_1",
	"CSIWDN_Integer_aroundRadius_0_initRadiusPrecision_1000000",
	"CSIWDN_Mix_aroundRadius_0_initRadiusPrecision_1000000",
	"CSIWDN_Continous_aroundRadius_0_initRadiusPrecision_1000000"
	};



	std::ofstream out(analsisData + "nbn_diff_dynamic_change2.txt");

	for (auto& curname : filenames) {

		out << curname << "\t";
		ofec::nbn::NBNinfo nbnInfo;
		nbnInfo.inputNBNinfo(networkDir, curname + "_phase_" + std::to_string(5));
		for (int idx(6); idx < 48; ++idx) {

			ofec::nbn::NBNinfo otherNBNinfo;
			otherNBNinfo.inputNBNinfo(networkDir, curname + "_phase_" + std::to_string(idx));
			out << ofec::nbn::diffNBNRatio(nbnInfo, otherNBNinfo) << "\t";
		}
		out << std::endl;
	}

	out.close();




}




void calNBNtaskSelectTaskFit() {


	std::cout << "calNBNtaskSelectTaskFit" << std::endl;
	using namespace ofec;
	using namespace ofec::utility;


	//ofec::g_working_directory = "/mnt/Data/Student/2018/YiyaDiao/code_total/data";
	ofec::g_working_directory = "//172.24.242.8/share/Student/2018/YiyaDiao/code_total/data";
	ofec::g_working_directory = "/home/lab408/share/2018/diaoyiya/ofec-data";
	auto solDir = ofec::g_working_directory + "/paper_cswidn/cswidn_sampling_phase/";
	auto fitDir = ofec::g_working_directory + "/paper_cswidn/cswidn_sampling_phase_fitness_con_1/";
	auto networkDir = ofec::g_working_directory + "/paper_cswidn/cswidn_sampling_phase_fitness_network/";

	std::filesystem::create_directories(networkDir);

	auto processname = "processUnique";
	std::cout << "evaluate \t" << processname << std::endl;
	using namespace ofec;
	const std::string fitExtension = "_fitness.txt";
	auto fitExten = "_fitness.txt";
	auto networkExten = "_network.txt";
	const std::string solExtension = "_solutions.txt";




	auto filenames = getFilesNamesWithExtension(fitDir, fitExten);
	for (auto& it : filenames) {
		it = removeSuffix(it, fitExten);
		//auto infos = UTILITY::split(it, "_");
		//std::cout << infos << std::endl;
		std::cout << it << std::endl;
	}
	std::shared_ptr<ofec::Environment> env;
	createCSWIDN_environments(env, processname);
	auto rnd = std::make_shared<ofec::Random>(0.5);
	auto pro = env->problem();
	//std::vector<int> phases = { 3,7,8 };
	//	std::reverse(phases.begin(), phases.end());




	auto cswidnPro = CAST_CSIWDN(pro);

	for (auto& curfilename : filenames) {
		/*for (auto& inputName : filenames)*/ {
			//	auto curfilename = inputName + "_phase_" + std::to_string(curphase);
			int curphase(0);
			auto inputName = removePhaseSuffix(curfilename, curphase);
			std::cout << "filename\t" << networkDir + curfilename + networkExten << std::endl;
			if (!std::filesystem::exists(networkDir + curfilename + networkExten)) {

				std::cout << "filename\t" << solDir + inputName + solExtension << std::endl;
				std::cout << "filename\t" << fitDir + curfilename + fitExten << std::endl;
				if (std::filesystem::exists(solDir + inputName + solExtension) && std::filesystem::exists(fitDir + curfilename + fitExten)) {
					cswidnPro->setPhase(curphase);
					std::vector<std::shared_ptr<ofec::SolutionBase>> sols;
					{




						utility::inputSolutions(solDir, inputName, sols, env.get());
						std::vector<double> fits;
						{
							ofec::ParameterVariantStream paramsStream;
							ofec::variants_stream::inputFromFile(paramsStream, fitDir + curfilename + fitExten);
							paramsStream >> fits;

						}

						for (int idx(0); idx < sols.size(); ++idx) {
							sols[idx]->setFitness(fits[idx]);
						}
					}
					auto pro = env->problem();
					ofec::nbn::NBNinfo nbnInfo;
					nbnInfo.m_sols = sols;
					for (auto& it : nbnInfo.m_sols) {
						nbnInfo.m_solbases.push_back(it.get());
					}
					calculateNBN(nbnInfo, networkDir, networkDir, curfilename, env.get(), rnd.get());

				}
			}
		}
	}


}


void calNBN_tasks(std::vector<std::string>& filenames) {


	std::cout << "calNBNtaskSelectTaskFit" << std::endl;
	using namespace ofec;
	using namespace ofec::utility;

	auto solDir = ofec::g_working_directory + "/paper_cswidn/cswidn_sampling_phase/";
	auto fitDir = ofec::g_working_directory + "/paper_cswidn/cswidn_sampling_phase_fitness/";
	auto networkDir = ofec::g_working_directory + "/paper_cswidn/cswidn_sampling_phase_fitness_network/";

	std::filesystem::create_directories(networkDir);

	auto processname = "processUnique";
	std::cout << "evaluate \t" << processname << std::endl;
	using namespace ofec;
	const std::string fitExtension = "_fitness.txt";
	auto fitExten = "_fitness.txt";
	auto networkExten = "_network.txt";
	const std::string solExtension = "_solutions.txt";



	std::shared_ptr<ofec::Environment> env;
	createCSWIDN_environments(env, processname);
	auto rnd = std::make_shared<ofec::Random>(0.5);
	auto pro = env->problem();
	auto cswidnPro = CAST_CSIWDN(pro);

	for (auto& curfilename : filenames) {
		/*for (auto& inputName : filenames)*/ {
			//	auto curfilename = inputName + "_phase_" + std::to_string(curphase);
			int curphase(0);
			auto inputName = removePhaseSuffix(curfilename, curphase);
			std::cout << "filename\t" << networkDir + curfilename + networkExten << std::endl;
			if (!std::filesystem::exists(networkDir + curfilename + networkExten)) {

				std::cout << "filename\t" << solDir + inputName + solExtension << std::endl;
				std::cout << "filename\t" << fitDir + curfilename + fitExten << std::endl;
				if (std::filesystem::exists(solDir + inputName + solExtension) && std::filesystem::exists(fitDir + curfilename + fitExten)) {
					cswidnPro->setPhase(curphase);
					std::vector<std::shared_ptr<ofec::SolutionBase>> sols;
					{




						utility::inputSolutions(solDir, inputName, sols, env.get());
						std::vector<double> fits;
						{
							ofec::ParameterVariantStream paramsStream;
							ofec::variants_stream::inputFromFile(paramsStream, fitDir + curfilename + fitExten);
							paramsStream >> fits;

						}

						for (int idx(0); idx < sols.size(); ++idx) {
							sols[idx]->setFitness(fits[idx]);
						}
					}
					auto pro = env->problem();
					ofec::nbn::NBNinfo nbnInfo;
					nbnInfo.m_sols = sols;
					for (auto& it : nbnInfo.m_sols) {
						nbnInfo.m_solbases.push_back(it.get());
					}
					calculateNBN(nbnInfo, networkDir, networkDir, curfilename, env.get(), rnd.get());

				}
			}
		}
	}


}


void calNBNwithTask() {

	ofec::g_working_directory = "/mnt/Data/Student/2018/YiyaDiao/code_total/data";
	//ofec::g_working_directory = "//172.24.242.8/share/Student/2018/YiyaDiao/code_total/data";
	ofec::g_working_directory = "/home/lab408/share/2018/diaoyiya/ofec-data";
	std::vector<std::string> filenames = { "CSIWDN_Continous_aroundRadius_0_initRadiusPrecision_1" };

	filenames = { "CSIWDN_Continous_aroundRadius_0_initRadiusPrecision_1" ,"CSIWDN_Integer_aroundRadius_0_initRadiusPrecision_1000000" };
	//	filenames = { "CSIWDN_Mix_aroundRadius_0_initRadiusPrecision_1000000" };



	filenames = {
	"CSIWDN_Continous_aroundRadius_0_initRadiusPrecision_1",
	"CSIWDN_Integer_aroundRadius_0_initRadiusPrecision_1000000",
	"CSIWDN_Mix_aroundRadius_0_initRadiusPrecision_1000000",
	"CSIWDN_Continous_aroundRadius_0_initRadiusPrecision_1000000"
	};

	//	std::reverse(filenames.begin(), filenames.end());



	std::vector<std::string> totalTaskNames;
	for (int idx(3); idx <= 48; ++idx) {
		//for (int idx(48); idx >=3; --idx) {

		double phaseRound = idx;
		int curphase = std::round(phaseRound);
		if (curphase >= 48) curphase = 48;
		for (auto& inputName : filenames) {
			auto curfilename = inputName + "_phase_" + std::to_string(curphase);
			totalTaskNames.push_back(curfilename);
		}
	}

	calNBN_tasks(totalTaskNames);


}





void modifyNBNvalue(const std::string& processname) {

	using namespace ofec;

	ofec::g_working_directory = "//172.24.242.8/share/Student/2018/YiyaDiao/code_total/data";
	//ofec::g_working_directory = "/home/lab408/share/2018/diaoyiya/ofec-data";
	auto solDir = ofec::g_working_directory + "/paper_cswidn/cswidn_sampling_phase/";
	auto fitDir = ofec::g_working_directory + "/paper_cswidn/cswidn_sampling_phase_fitness/";
	auto networkDir = ofec::g_working_directory + "/paper_cswidn/cswidn_sampling_phase_fitness_network/";


	std::shared_ptr<ofec::Environment> env;
	createCSWIDN_environments(env, processname);
	auto rnd = std::make_shared<ofec::Random>(0.5);
	auto pro = env->problem();
	auto cswidnPro = CAST_CSIWDN(pro);

	const std::string filename = "CSIWDN_Continous_aroundRadius_0_initRadiusPrecision_1_phase_3";

	ofec::nbn::NBNinfo nbnInfo;
	nbnInfo.inputNBNinfo(networkDir, filename);
	modifyNBN(nbnInfo, networkDir, networkDir, filename, env.get(), rnd.get());

}




void copyFiles() {


	std::cout << "calNBNinfo" << std::endl;
	using namespace ofec;
	using namespace ofec::utility;


	ofec::g_working_directory = "/mnt/Data/Student/2018/YiyaDiao/code_total/data";
	ofec::g_working_directory = "//172.24.242.8/share/Student/2018/YiyaDiao/code_total/data";
	//ofec::g_working_directory = "/home/lab408/share/2018/diaoyiya/ofec-data";
	//auto solDir = ofec::g_working_directory + "/paper_cswidn/cswidn_sampling_phase/";
	//auto fitDir = ofec::g_working_directory + "/paper_cswidn/cswidn_sampling_phase_fitness/";
	auto fromDir = ofec::g_working_directory + "/paper_cswidn/cswidn_sampling_phase_fitness_network/";



	std::string toDirDir = "//172.24.24.151/f/DiaoYiya/cswidn_exp/";
	std::string dirname = toDirDir + "CSIWDN_Continous_aroundRadius_0_initRadiusPrecision_1/";
	std::filesystem::create_directories(dirname);



	auto processname = "processUnique";
	std::cout << "copy files \t" << processname << std::endl;
	using namespace ofec;
	const std::string fitExtension = "_fitness.txt";
	auto fitExten = "_fitness.txt";
	auto networkExten = "_network.txt";
	const std::string solExtension = "_solutions.txt";




	auto filenames = getFilesNamesWithExtension(fromDir, networkExten);
	for (auto& it : filenames) {
		it = removeSuffix(it, networkExten);
	}
	std::shared_ptr<ofec::Environment> env;
	createCSWIDN_environments(env, processname);
	auto rnd = std::make_shared<ofec::Random>(0.5);
	auto pro = env->problem();


	for (auto& it : filenames) {
		if (it.starts_with("CSIWDN_Continous_aroundRadius_0_initRadiusPrecision_1_")) {

			std::cout << fromDir + it + networkExten << std::endl;
			std::cout << dirname + it + networkExten << std::endl;
			std::cout << it << std::endl;
			try {
				std::filesystem::copy(fromDir + it + networkExten, dirname + it + networkExten);
			}
			catch (const std::exception& e) {
				std::cout << e.what() << std::endl;
			}
		}
	}



}








namespace ofec {

	void registerParamAbbr() {}
	void customizeFileName() {}
	void run(int argc, char* argv[]) {


		registerInstance();

		// 打印参数个数和每个参数的值
		std::cout << "Number of command-line arguments: " << argc << std::endl;
		for (int i = 0; i < argc; ++i) {
			std::cout << "Argument " << i << ": " << argv[i] << std::endl;
			//std::string filename = std::string(argv[i]) + "_yes";
			//std::cout << "filename\t" << filename << std::endl;
		}



		using namespace ofec;
		using namespace std;
		ofec::g_working_directory = "//172.24.34.11/share/2018/diaoyiya/ofec-data";
		ofec::g_working_directory = "/home/lab408/share/2018/diaoyiya/ofec-data";
		//ofec::g_working_directory = "//172.29.41.69/share/2018/diaoyiya/ofec-data";
		//ofec::g_working_directory = "//172.29.203.176/e/DiaoYiya/code/data/ofec-data";
		//ofec::g_working_directory = "E:/DiaoYiya/code/data/ofec-data";
		//ofec::g_working_directory = "//172.29.203.176/e/DiaoYiya/code/data/ofec-data";
		//ofec::g_working_directory = "/mnt/Data/Student/2018/YiyaDiao/code_total/data";
		//ofec::g_working_directory = "//172.24.242.8/share/Student/2018/YiyaDiao/code_total/data";
		//auto saveDir = ofec::g_working_directory + "/cswidn_algorithm_runProcess_2/";
		//auto solDir = ofec::g_working_directory + "/cswidn_sampling_phase/";
		//auto fitDir = ofec::g_working_directory + "/cswidn_sampling_phase_fitness_con_1/";
		//auto networkDir = ofec::g_working_directory + "/cswidn_sampling_phase_fitness_network/";
		//calNBNtaskSelectTaskFit();

		//calNBNinfoChangeDiff();
		copyFiles();


		//modifyNBNvalue("process_dyy1");


		//calNBNinfo();
		//calNBNwithTask();



		//auto cswidnProInfoDir = ofec::g_working_directory + "/cswidn_pro_info/";
		//std::filesystem::create_directories(cswidnProInfoDir);
		//outputCSWIDNpro_figureData(cswidnProInfoDir);


		//calNBNallTasks(solDir, fitDir, networkDir);

		//std::cout << "runOneAlg\t" << argv[1] << "\t" << argv[2] << std::endl;
		//runOneAlg(saveDir, argv[1], std::stoi(argv[2]), 30);

		//std::cout << "ver5 evaluateAllAlg\t" << argv[1]  << std::endl;
		//evaluateAllAlg(saveDir, argv[1], 30);

	//	std::string avg = "run1";

		//runOFECshell(saveDir2 + "run_multiple_ofec.sh");



	  //  std::cout << "evaluateProOnePhase with cout\t" << argv[1]<<"\t" <<stoi(argv[2]) << std::endl;
	//	evaluateProOnePhase(saveDir, saveDir2, argv[1], stoi(argv[2]));


		//std::cout << "evaluateProOnePhase with cout\t" << argv[1]  << std::endl;
		//evaluateProOnePhaseMulti(saveDir, saveDir2, argv[1]);

	}


}
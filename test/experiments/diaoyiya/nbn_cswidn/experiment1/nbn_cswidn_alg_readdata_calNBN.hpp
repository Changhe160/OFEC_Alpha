
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
#include "../core/parameter/variants_to_stream.h"

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



void createCSWIDN_Environment(std::shared_ptr<ofec::Environment>& envCopy, ofec::Environment* env) {
	using namespace std;
	using namespace ofec;
	ofec::ParameterMap params;

	env->problem()->inputParameters().output(params);

	envCopy.reset(Environment::create());
	envCopy->recordInputParameters();
	envCopy->initialize();
	envCopy->setProblem(ofec::Factory<ofec::Problem>::produce(env->problem()->name()));
	envCopy->problem()->inputParameters().input(params);
	envCopy->problem()->recordInputParameters();
	envCopy->initializeProblem(0.5);
	auto cswidnPro = CAST_CSIWDN(env->problem());
	auto cswidnProCopy = CAST_CSIWDN(envCopy->problem());

	//std::string filernd = UTILITY::getCurrentSystemTime() + "_" + UTILITY::createRandomString(50, m_rnd.get());
	//auto filernd = m_winName + "_" + std::to_string(total_file++);
	//cswidnPro->setFileRnd(filernd);

	cswidnProCopy->setDistanceType(cswidnPro->distanceType());
	cswidnProCopy->setPhase(cswidnPro->phase());
	cswidnProCopy->setSourceIdx(cswidnPro->sourceIdx());
	cswidnProCopy->setProcessName(cswidnPro->processName());



}




void evaluateSolsSubTask(std::vector<ofec::SolutionBase*>& sols, int from, int to, ofec::Environment* env, ofec::Random* rnd) {

	auto pro = env->problem();
	ofec::Real pos = (pro->optimizeMode(0) == ofec::OptimizeMode::kMaximize) ? 1 : -1;

	for (int idx(from); idx < to; ++idx) {
		auto& cursol = dynamic_cast<ofec::CSIWDN::solutionType&>(*sols[idx]);
		cursol.variable().flagLocation() = false;
		sols[idx]->evaluate(env, false);
		sols[idx]->setFitness(pos * sols[idx]->objective(0));
	}
}

void evaluateSols(std::vector<ofec::SolutionBase*>& sols, ofec::Environment* env, ofec::Random* rnd) {
	using namespace std;
	using namespace ofec;

	int num_task = std::thread::hardware_concurrency();
	num_task = 1;
	std::vector<std::shared_ptr<Environment>> envs(num_task);
	std::vector<std::shared_ptr<Random>> rnds(num_task);
	for (auto& it : envs) {
		createCSWIDN_Environment(it, env);
	}
	for (auto& it : rnds) {
		it.reset(new Random(rnd->uniform.next()));
	}

	std::vector<int> tasks;
	std::vector<std::thread> thrds;

	UTILITY::assignThreads(sols.size(), num_task, tasks);
	std::pair<int, int> from_to;
	for (size_t i = 0; i < num_task; ++i) {
		from_to.first = tasks[i];
		from_to.second = tasks[i + 1];

		thrds.push_back(std::thread(
			evaluateSolsSubTask, std::ref(sols),
			tasks[i], tasks[i + 1], envs[i].get(), rnds[i].get()));
	}
	for (auto& thrd : thrds)
		thrd.join();
}




bool file_exists_with_prefix(std::vector<std::filesystem::path>& results, const std::string& savedir, const std::string& prefix) {
	namespace fs = std::filesystem;
	const std::string extension = ".txt";
	std::error_code ec;
	results.clear();

	// 使用递归目录迭代器遍历当前目录及其子目录
	for (const auto& entry : fs::recursive_directory_iterator(savedir)) {
		if (entry.is_regular_file() && entry.path().filename().string().rfind(prefix, 0) == 0) {
			results.push_back(entry.path().filename());
		}
	}

	// 如果结果不为空，说明找到了至少一个符合条件的文件
	return !results.empty();
}



bool waitForFile(const std::string& saveDir, const std::string& filename, const std::string& process_name) {
	std::vector<std::filesystem::path> results;
	if (file_exists_with_prefix(results, saveDir, filename)) {
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
	file_exists_with_prefix(results, saveDir, filename);
	std::sort(results.begin(), results.end());
	if (results.front() == testfilename) {
		return true;
	}
	return false;
}


struct TaskInfo {


	std::string m_filename;

	ofec::CSIWDN::DistanceType m_disType = ofec::CSIWDN::DistanceType::Mix;
	std::pair<int, int> m_source_idx;
	int m_current_phase = 0;


	std::vector<ofec::SolutionBase*> algSols;
	std::vector<ofec::AMP_GL_SaDE_Sampling::SolInfo*> algSolInfos;



	ofec::Environment* env = nullptr;
	ofec::Random* rnd = nullptr;

	TaskInfo(ofec::Environment* tenv, ofec::Random* trnd) {
		env = tenv;
		rnd = trnd;
	}

	void getInfo(const ofec::AMP_GL_SaDE_Sampling::SolInfo& info) {
		m_source_idx = info.m_source_idx;
		m_current_phase = info.m_current_phase;
	}

	std::string getFileName() const {
		return "CSWIDN_SourceIds_" + std::to_string(m_source_idx.first) + "_" + std::to_string(m_source_idx.second) + "_phase_" + std::to_string(m_current_phase);
	}

	void modifyEnv(ofec::Environment* env) const {
		using namespace ofec;
		auto pro = env->problem();
		auto cswidnPro = CAST_CSIWDN(pro);
		cswidnPro->setDistanceType(m_disType);
		//	cswidnPro->setSourceIdx(m_source_idx);
		cswidnPro->setPhase(m_current_phase);

	}


};




void evaluateFilterSolAndOutputInfo(const std::string& saveDir,
	const std::string& saveDir2, const std::string& processName, TaskInfo& curTask) {
	using namespace ofec;
	auto env = curTask.env;
	auto rnd = curTask.rnd;
	auto& algSols = curTask.algSols;

	auto filename = curTask.getFileName() + "_algSol_infos";
	auto pro = env->problem();
	auto cswidnPro = CAST_CSIWDN(pro);
	filename = curTask.m_filename;
	/*if (waitForFile(saveDir, filename, processName)) */ {
		//auto& algSols = sub_sols[proId];
		curTask.modifyEnv(env);

		//cswidnPro->setDistanceType(CSIWDN::DistanceType::Mix);
		//cswidnPro->setSourceIdx(curTask.m_source_idx);
		//evaluateSols(algSols, env, rnd);
		std::vector<int> mapSolId;
		//	fitlerSameSolutions(algSols, mapSolId, env);
			// output solutions infos;
		{
			ofec::ParameterVariantStream paramsStream;
			ofec::variants_stream::inputFromFile(paramsStream, saveDir + filename + ".txt");
			std::vector<ofec::Real> fitness;


			paramsStream >> fitness;
			paramsStream >> mapSolId;

			//for (auto& it : algSols) {
			//	fitness.push_back(it->fitness());
			//}

			for (int idx(0); idx < algSols.size(); ++idx) {
				algSols[idx]->setFitness(fitness[idx]);
			}
			ofec::nbn::NBNinfo nbnInfo;
			nbnInfo.m_solbases = algSols;
			//std::vector<std::shared_ptr<ofec::SolutionBase>> optSols;
			//std::vector<std::shared_ptr<ofec::SolutionInfo>> optSolInfos;
			//if (pro->optimaBase()->isSolutionGiven()) {
			//	for (int idx(0); idx < pro->optimaBase()->numberSolutions(); ++idx) {
			//		optSols.emplace_back(pro->createSolution(pro->optimaBase()->solutionBase(idx)));
			//	}
			//}
			//for (auto& it : optSols) {
			//	nbnInfo.m_solbases.push_back(it.get());

			//}
			std::cout << "fitlerSameSolutions" << std::endl;



			std::vector<int> mapSolIds;

			{
				cswidnPro->fitlerSameSolutions(algSols, nbnInfo.m_solbases, mapSolIds);
			}

			{
				ofec::ParameterVariantStream paramsStream;
				paramsStream << mapSolId;
				ofec::variants_stream::outputToFile(paramsStream, saveDir + filename + "_mapSolId.txt");
			}


			calculateNBN(nbnInfo, saveDir2, saveDir2, filename, env, rnd);
			//	evaluateFilterSolAndOutputInfo()
		}

	}
}


void evaluateSolAndOutputInfo(const std::string& saveDir, const std::string& processName, TaskInfo& curTask) {
	auto env = curTask.env;
	auto rnd = curTask.rnd;
	auto& algSols = curTask.algSols;

	auto filename = curTask.getFileName() + "_algSol_fitness";

	filename = curTask.m_filename;
	if (waitForFile(saveDir, filename, processName)) {
		//auto& algSols = sub_sols[proId];
		curTask.modifyEnv(env);

		//cswidnPro->setDistanceType(CSIWDN::DistanceType::Mix);
		//cswidnPro->setSourceIdx(curTask.m_source_idx);
		//evaluateSols(algSols, env, rnd);
		//std::vector<int> mapSolId;
		//fitlerSameSolutions(algSols, mapSolId, env);
		// output solutions infos;
		{
			ofec::ParameterVariantStream paramsStream;
			ofec::variants_stream::inputFromFile(paramsStream, saveDir + filename + ".txt");
			std::vector<ofec::Real> fitness;

			paramsStream >> fitness;
			for (int idx(0); idx < fitness.size(); ++idx) {
				algSols[idx]->setFitness(fitness[idx]);
			}
			//for (auto& it : algSols) {
			//	it->setFitness()
			//	fitness.push_back(it->fitness());
			//}


			//paramsStream << mapSolId;



		}

	}
}



void outputSols(const std::string& saveDir, const std::string& processName, TaskInfo& curTask) {
	using namespace ofec;
	auto filename = curTask.getFileName() + "_algSol_sols";

	filename = curTask.m_filename;
	if (waitForFile(saveDir, filename, processName)) {
		ofec::ParameterVariantStream paramsStream;
		auto env = curTask.env;
		auto pro = env->problem();
		auto cswidnPro = CAST_CSIWDN(pro);


		paramsStream << curTask.algSols.size();
		for (auto& it : curTask.algSols) {
			pro->solutionToParameterVariants(*it, paramsStream);
		}
		paramsStream << curTask.algSolInfos.size();
		for (auto& it : curTask.algSolInfos) {
			it->toParameterVariants(paramsStream);
		}


		ofec::variants_stream::outputToFile(paramsStream, saveDir + filename + ".txt");

	}
}



void runAlg(const std::string& saveDir, const std::string& processName) {
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


	std::shared_ptr<ofec::Environment> env;
	genenratePro(proname, params, env);
	auto pro = env->problem();
	auto cswidnPro = CAST_CSIWDN(pro);
	auto rnd = make_shared<ofec::Random>(0.5);
	cswidnPro->setProcessName(processName);


	std::vector<std::shared_ptr<ofec::SolutionBase>> sols;
	std::vector<std::shared_ptr<ofec::SolutionInfo>> solInfos;
	ofec::SamplingMutithread::runAlgMultiTask(sols, solInfos, 0.5, 0.5, proname, params, algName, params, numRun);

	// output solutions



	{
		// output pro type
		std::vector<std::vector<int>> vecProTypes;
		std::vector<int> curProType(3);


		std::set<std::vector<int>> proTypes;

		for (auto& info : solInfos) {
			auto& curInfo = dynamic_cast<AMP_GL_SaDE_Sampling::SolInfo&>(*info);
			curProType[0] = curInfo.m_current_phase;
			curProType[1] = curInfo.m_source_idx.first;
			curProType[2] = curInfo.m_source_idx.second;
			proTypes.insert(curProType);
		}

		for (auto& info : proTypes) {
			vecProTypes.push_back(info);
		}
		std::string filename = "ProType";
		if (waitForFile(saveDir, filename, processName)) {
			{
				ofec::ParameterVariantStream paramsStream;
				paramsStream << vecProTypes.size();
				for (auto& it : vecProTypes) {
					paramsStream << it;
				}

				ofec::variants_stream::outputToFile(paramsStream, saveDir + filename + ".txt");

				//std::stringstream buf;
				//ofec::variants_stream::parameterStream2stringstream(paramsStream, buf);
				//std::ofstream out(saveDir + filename + ".txt");
				//out << buf.rdbuf();
				//out.close();
			}
		}
	}


	// output pro type
	std::vector<std::vector<int>> vecProTypes;
	std::vector<int> curProType(2);
	{
		std::set<std::vector<int>> proTypes;

		for (auto& info : solInfos) {
			auto& curInfo = dynamic_cast<AMP_GL_SaDE_Sampling::SolInfo&>(*info);
			//curProType[0] = curInfo.m_current_phase;
			curProType[0] = curInfo.m_source_idx.first;
			curProType[1] = curInfo.m_source_idx.second;
			proTypes.insert(curProType);
		}

		for (auto& info : proTypes) {
			vecProTypes.push_back(info);
		}
	}


	std::map<std::vector<int>, int> proTypeId;
	for (int idx(0); idx < vecProTypes.size(); ++idx) {
		proTypeId[vecProTypes[idx]] = idx;
	}


	std::vector<std::set<int>> proPhases(proTypeId.size());
	std::vector<std::vector<SolutionBase*>> sub_sols(proTypeId.size());
	std::vector<std::vector<AMP_GL_SaDE_Sampling::SolInfo*>> sub_solInfos(proTypeId.size());


	std::shared_ptr<AMP_GL_SaDE_Sampling::SolInfo> optInfo(new AMP_GL_SaDE_Sampling::SolInfo);
	optInfo->m_runId = 1e9;
	std::vector<std::shared_ptr<SolutionBase>> optSols;

	for (int idx(0); idx < pro->optimaBase()->numberSolutions(); ++idx) {
		optSols.emplace_back(pro->createSolution(pro->optimaBase()->solutionBase(idx)));
		//	auto optSol = pro->optimaBase()->solutionBase(idx);
		//	proSol.push_back(optSol);

	}
	{
		for (int idPro(0); idPro < sub_sols.size(); ++idPro) {
			auto& proSol = sub_sols[idPro];
			auto& proSolInfo = sub_solInfos[idPro];
			for (int idx(0); idx < optSols.size(); ++idx) {
				proSol.push_back(optSols[idx].get());
				proSolInfo.push_back(optInfo.get());

			}
		}

	}

	for (int idx(0); idx < sols.size(); ++idx) {
		auto curSol = dynamic_cast<SolutionBase*>(sols[idx].get());
		auto curInfo = dynamic_cast<AMP_GL_SaDE_Sampling::SolInfo*>(solInfos[idx].get());
		//curProType[0] = curInfo->m_current_phase;
		//curProType[1] = curInfo->m_source_idx.first;
		//curProType[2] = curInfo->m_source_idx.second;

		curProType[0] = curInfo->m_source_idx.first;
		curProType[1] = curInfo->m_source_idx.second;

		int proId = proTypeId[curProType];
		sub_sols[proId].push_back(curSol);
		sub_solInfos[proId].push_back(curInfo);

		proPhases[proId].insert(curInfo->m_current_phase);

	}


	TaskInfo curTask(env.get(), rnd.get());
	curTask.m_disType = ofec::CSIWDN::DistanceType::Mix;
	for (int proId(0); proId < proPhases.size(); ++proId) {

		curTask.getInfo(*sub_solInfos[proId].front());
		curTask.algSolInfos = sub_solInfos[proId];
		curTask.algSols = sub_sols[proId];
		curTask.m_current_phase = 0;
		curTask.m_filename = curTask.getFileName() + "_algSol_sols";

		outputSols(saveDir, processName, curTask);


		//for (auto& curphase : proPhases[proId]) {
		//	curTask.m_current_phase = curphase;
		//	evaluateSolAndOutputTask(saveDir, processName, curTask);
		//
		//  auto filename = curTask.getFileName() + "_algSol_infos";

		//filename = curTask.m_filename;
		//}
	}

}


void runOneAlg(const std::string& saveDir, const std::string& processName, int curRun, int numRun) {


	using namespace ofec;
	using namespace std;
	ofec::ParameterMap params;
	std::string algName = "AMP-GL-SaDE-sampling";
	std::string proname = "CSIWDN";
	params["problem name"] = std::string("CSIWDN");
	params["net-sensor-source case"] = std::string("Net2/sensor_case1/source/case11.txt");
	params["maximum evaluations"] = int(200000);
	params["process name"] = processName;
	//int numRun = 30;


	std::shared_ptr<ofec::Environment> env;
	genenratePro(proname, params, env);
	auto pro = env->problem();
	auto cswidnPro = CAST_CSIWDN(pro);
	auto rnd = make_shared<ofec::Random>(0.5);
	cswidnPro->setProcessName(processName);

	double algSeed = double(curRun + 1) / double(numRun + 1);;


	env->setAlgorithm(Factory<Algorithm>::produce(algName));
	env->algorithm()->inputParameters().input(params);
	env->algorithm()->recordInputParameters();
	env->initializeAlgorithm(algSeed);
	auto samplingAlg = CAST_SAMPLING_ALG(env->algorithm());

	samplingAlg->samplingDuringRun(env.get());

	auto& sols = samplingAlg->getSols();
	auto& solInfos = samplingAlg->getSolInfos();



	std::string filename = proname + "_algorithm_" + std::to_string(curRun);


	ofec::ParameterVariantStream paramsStream;


	paramsStream << sols.size();
	for (auto& it : sols) {
		pro->solutionToParameterVariants(*it, paramsStream);
	}
	paramsStream << solInfos.size();
	for (auto& it : solInfos) {
		it->toParameterVariants(paramsStream);
	}


	ofec::variants_stream::outputToFile(paramsStream, saveDir + filename + ".txt");


}


void getProIds(const std::string& saveDir, std::map<std::vector<int>, int>& proTypeId, std::vector<std::set<int>>& proPhases) {
	proTypeId.clear();
	proPhases.clear();

	std::vector<std::vector<int>> vecProTypes;
	std::vector<int> curProType(3);
	{
		// output pro type
		std::string filename = "ProType";
		/*if (waitForFile(saveDir, filename, processName))*/ {
			{

				ofec::ParameterVariantStream paramsStream;
				ofec::variants_stream::inputFromFile(paramsStream, saveDir + filename + ".txt");
				auto vSize = vecProTypes.size();
				paramsStream >> vSize;
				vecProTypes.resize(vSize);
				for (auto& it : vecProTypes) {
					paramsStream >> it;
				}



				//std::stringstream buf;
				//ofec::variants_stream::parameterStream2stringstream(paramsStream, buf);
				//std::ofstream out(saveDir + filename + ".txt");
				//out << buf.rdbuf();
				//out.close();
			}
		}
	}


	;
	{
		std::set<std::vector<int>> proTypeSet;
		curProType.resize(2);
		for (int idx(0); idx < vecProTypes.size(); ++idx) {

			curProType[0] = vecProTypes[idx][1];
			curProType[1] = vecProTypes[idx][2];
			proTypeSet.insert(curProType);
			//proTypeId[vecProTypes[idx]] = idx;
		}
		std::vector<std::vector<int>> vecProTypes2;
		for (auto& info : proTypeSet) {
			vecProTypes2.push_back(info);
		}
		for (int idx(0); idx < vecProTypes2.size(); ++idx) {
			proTypeId[vecProTypes2[idx]] = idx;
		}

	}

	proPhases.resize(proTypeId.size());
	curProType.resize(2);
	for (auto& it : vecProTypes) {
		curProType[0] = it[1];
		curProType[1] = it[2];
		int proId = proTypeId[curProType];
		proPhases[proId].insert(it[0]);
	}

}



#include <string>

//std::vector<std::vector<ofec::AMP_GL_SaDE_Sampling::SolInfo>> copyptr_solInfos;
void evaluateAllAlg(const std::string& saveDir,
	const std::string& saveDir2,
	const std::string& processName, int numRun) {




	using namespace ofec;
	using namespace std;
	ofec::ParameterMap params;
	std::string algName = "AMP-GL-SaDE-sampling";
	std::string proname = "CSIWDN";
	params["problem name"] = std::string("CSIWDN");
	params["net-sensor-source case"] = std::string("Net2/sensor_case1/source/case11.txt");
	params["maximum evaluations"] = int(200000);
	params["process name"] = processName;
	//int numRun = 30;


	std::shared_ptr<ofec::Environment> env;
	genenratePro(proname, params, env);
	auto pro = env->problem();
	auto cswidnPro = CAST_CSIWDN(pro);
	auto rnd = make_shared<ofec::Random>(0.5);
	cswidnPro->setProcessName(processName);


	std::vector<std::vector<std::shared_ptr<SolutionBase>>> ptr_sols(numRun);
	std::vector<std::vector<AMP_GL_SaDE_Sampling::SolInfo>> ptr_solInfos(numRun);



	for (int curRun(0); curRun < numRun; ++curRun) {
		std::string filename = proname + "_algorithm_" + std::to_string(curRun);
		auto& cursols = ptr_sols[curRun];
		auto& cursolInfos = ptr_solInfos[curRun];

		ofec::ParameterVariantStream paramsStream;
		ofec::variants_stream::inputFromFile(paramsStream, saveDir + filename + ".txt");
		size_t inputSize(0);
		paramsStream >> inputSize;
		cursols.resize(inputSize);
		for (auto& it : cursols) {
			it.reset(pro->createSolution());
			pro->parameterVariantsToSolution(paramsStream, *it);
		}
		paramsStream >> inputSize;
		cursolInfos.resize(inputSize);
		for (auto& it : cursolInfos) {
			it.fromParameterVariants(paramsStream);
			//it->toParameterVariants(paramsStream);
		}



	}


	//copyptr_solInfos = ptr_solInfos;
	std::map<std::vector<int>, int> proTypeId;
	std::vector<std::set<int>> proPhases;

	getProIds(saveDir, proTypeId, proPhases);


	std::vector<SolutionBase*> sols;
	std::vector<AMP_GL_SaDE_Sampling::SolInfo*> solInfos;
	for (auto& it : ptr_sols) {
		for (auto& it2 : it) {
			sols.push_back(it2.get());
		}
	}

	for (auto& it : ptr_solInfos) {
		for (auto& it2 : it) {
			solInfos.push_back(&it2);
		}
	}


	std::vector<std::vector<SolutionBase*>> sub_sols(proTypeId.size());
	std::vector<std::vector<AMP_GL_SaDE_Sampling::SolInfo*>> sub_solInfos(proTypeId.size());


	std::shared_ptr<AMP_GL_SaDE_Sampling::SolInfo> optInfo(new AMP_GL_SaDE_Sampling::SolInfo);
	optInfo->m_runId = 1e9;
	std::vector<std::shared_ptr<SolutionBase>> optSols;

	for (int idx(0); idx < pro->optimaBase()->numberSolutions(); ++idx) {
		optSols.emplace_back(pro->createSolution(pro->optimaBase()->solutionBase(idx)));
		//	auto optSol = pro->optimaBase()->solutionBase(idx);
		//	proSol.push_back(optSol);

	}
	{
		for (int idPro(0); idPro < sub_sols.size(); ++idPro) {
			auto& proSol = sub_sols[idPro];
			auto& proSolInfo = sub_solInfos[idPro];
			for (int idx(0); idx < optSols.size(); ++idx) {
				proSol.push_back(optSols[idx].get());
				proSolInfo.push_back(optInfo.get());

			}
		}

	}

	std::vector<int> curProType(2);

	for (int idx(0); idx < sols.size(); ++idx) {
		auto curSol = dynamic_cast<SolutionBase*>(sols[idx]);
		auto curInfo = dynamic_cast<AMP_GL_SaDE_Sampling::SolInfo*>(solInfos[idx]);
		//curProType[0] = curInfo->m_current_phase;
		//curProType[1] = curInfo->m_source_idx.first;
		//curProType[2] = curInfo->m_source_idx.second;

		curProType[0] = curInfo->m_source_idx.first;
		curProType[1] = curInfo->m_source_idx.second;

		int proId = proTypeId[curProType];
		sub_sols[proId].push_back(curSol);
		sub_solInfos[proId].push_back(curInfo);

		//proPhases[proId].insert(curInfo->m_current_phase);

	}


	TaskInfo curTask(env.get(), rnd.get());
	curTask.env = env.get();
	curTask.m_disType = ofec::CSIWDN::DistanceType::Mix;
	for (int proId(0); proId < proPhases.size(); ++proId) {

		curTask.getInfo(*sub_solInfos[proId].back());
		curTask.algSolInfos = sub_solInfos[proId];
		curTask.algSols = sub_sols[proId];
		curTask.m_current_phase = 0;


		//outputSols(saveDir, processName, curTask);


		for (auto& curphase : proPhases[proId]) {


			curTask.m_current_phase = curphase;
			curTask.m_filename = curTask.getFileName() + "_algSol_sols";
			std::cout << "curphase\t" << curphase << std::endl;
			std::cout << "filename\t" << curTask.m_filename << std::endl;
			evaluateFilterSolAndOutputInfo(saveDir, saveDir2, processName, curTask);
			//curTask.m_filename = curTask.getFileName() + "_algSol_infos";

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
			std::string filename = std::string(argv[i]) + "_yes";
			std::cout << "filename\t" << filename << std::endl;
		}



		using namespace ofec;
		using namespace std;
		ofec::g_working_directory = "//172.24.207.203/share/2018/diaoyiya/ofec-data";
		ofec::g_working_directory = "/home/lab408/share/2018/diaoyiya/ofec-data";
		//ofec::g_working_directory = "//172.29.41.69/share/2018/diaoyiya/ofec-data";
		//ofec::g_working_directory = "//172.29.203.176/e/DiaoYiya/code/data/ofec-data";
		//ofec::g_working_directory = "E:/DiaoYiya/code/data/ofec-data";
		//ofec::g_working_directory = "//172.29.203.176/e/DiaoYiya/code/data/ofec-data";
		ofec::g_working_directory = "/mnt/Data/Student/2018/YiyaDiao/code_total/data";
		//	ofec::g_working_directory = "//172.24.242.8/share/Student/2018/YiyaDiao/code_total/data";


		auto saveDir = ofec::g_working_directory + "/paper_cswidn/cswidn_algorithm_runProcess_2/";
		auto saveDir2 = ofec::g_working_directory + "/paper_cswidn/cswidn_algorithm_runProcess_network4/";

		std::filesystem::create_directories(saveDir);
		std::filesystem::create_directories(saveDir2);
		//std::cout << "runOneAlg\t" << argv[1] << "\t" << argv[2] << std::endl;
		//runOneAlg(saveDir, argv[1], std::stoi(argv[2]), 30);

		//std::cout << "ver5 evaluateAllAlg\t" << argv[1]  << std::endl;
		//evaluateAllAlg(saveDir, argv[1], 30);

		std::string avg = "run1";


		std::cout << "ver5 evaluateAllAlg with cout\t" << avg << std::endl;
		evaluateAllAlg(saveDir, saveDir2, avg, 30);

		//evaluateAllAlg(saveDir, "test", 3);

		//runOneAlg(saveDir, "test", 1, 30);

		//runTask();
		//runTotalTasks(std::string(argv[1]));
		//runTotalTasks("test");


	}


}
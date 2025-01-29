
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
#include "../utility/nbn_visualization/tree_graph_simple.h"
#include "../utility/nbn_visualization/nbn_grid_tree_division.h"
#include "../utility/nbn_visualization/nbn_grid_tree_division_allSize.h"
#include "../utility/function/custom_function.h"
#include "../utility/function/general_function.h"
#include "../utility/nbn_visualization/nbn_calculator/nearest_better_calculator.h"
//#include "../instance/problem/combination/travelling_salesman/travelling_salesman.h"
#include "interface.h"
//#include "../utility/function/custom_function.h"
#include "../utility/nbn_visualization/tree_graph_simple.h"
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
#include "../instance/algorithm/visualize/sampling/instance/cswidn/sampling_cswidn_amp_cc.h"




void outputToFile(ofec::ParameterVariantStream& paramsStream, const std::string& filepath) {
	std::stringstream buf;
	ofec::variants_stream::parameterStream2stringstream(paramsStream, buf);
	std::ofstream out(filepath);
	out << buf.rdbuf();
	out.close();
}


void inputFromFile(ofec::ParameterVariantStream& paramsStream, const std::string& filepath) {
	std::stringstream buf;
	std::ifstream in(filepath);
	buf << in.rdbuf();
	in.close();
	ofec::variants_stream::stringstream2parameterStream(buf, paramsStream);

}

struct NBNinfo {

	std::vector<std::shared_ptr<ofec::SolutionBase>> m_sols;

	std::vector<ofec::SolutionBase*> solbases;
	std::vector<int> belong;
	std::vector<double> dis2parent;
	std::vector<double> vFitness;


	void outputNBNinfo(const std::string& saveDir, const std::string& filename) {


		ofec::ParameterVariantStream paramsStream;
		paramsStream << belong;
		paramsStream << dis2parent;
		paramsStream << vFitness;
		auto filepath = saveDir + filename + "_nbnInfo.txt";
		outputToFile(paramsStream, filepath);

	}

	void inputNBNinfo(const std::string& saveDir, const std::string& filename) {
		ofec::ParameterVariantStream paramsStream;
		auto filepath = saveDir + filename + "_nbnInfo.txt";
		inputFromFile(paramsStream, filepath);
		paramsStream >> belong;
		paramsStream >> dis2parent;
		paramsStream >> vFitness;
	}
	static std::string getTspSolutionFileName(const std::string& saveDir, const std::string& filename) {
		return saveDir + filename + "_solVariables.txt";
	}


};


void calculateHnswModel(NBNinfo& info, ofec::Environment* env, ofec::Random* rnd, nbn::HnswModel& hnswModel) {
	auto& solbases = info.solbases;

	hnswModel.initialize(env, rnd, std::thread::hardware_concurrency());
	std::vector<int> solIds;
	//	new_datum->m_hnswModel2.setNumberThreads(1);
	hnswModel.addDataMutliThread(solbases, solIds);
}

void calculateNBN(NBNinfo& info, ofec::Environment* env, ofec::Random* rnd, nbn::HnswModel& hnswModel) {
	auto& solbases = info.solbases;
	//nbn::HnswModel hnswModel;
	auto& belong = info.belong;
	auto& dis2parent = info.dis2parent;
	auto& vFitness = info.vFitness;
	vFitness.clear();

	for (auto& it : solbases) {
		vFitness.push_back(it->fitness());
	}

	nbn::HnswModelNBN model2;
	model2.copy(hnswModel);
	//model2.setNumberThreads(1);
	model2.updateFitness(vFitness);
	model2.calculateNBN(belong, dis2parent);

}




void calculateNBNsingleThread(NBNinfo& info, ofec::Environment* env, ofec::Random* rnd) {

	auto pro = env->problem();

	auto& solbases = info.solbases;
	nbn::HnswModel hnswModel;
	hnswModel.initialize(env, rnd, std::thread::hardware_concurrency());
	std::vector<int> solIds;
	//	new_datum->m_hnswModel2.setNumberThreads(1);
	hnswModel.addDataMutliThread(solbases, solIds);


	auto& belong = info.belong;
	auto& dis2parent = info.dis2parent;
	auto& vFitness = info.vFitness;
	vFitness.clear();

	for (auto& it : solbases) {
		vFitness.push_back(it->fitness());
	}


	//for (auto& it : solbases) {
	//	vFitness.push_back(it->fitness());
	//}


	std::vector<std::vector<int>> model_neighbors(solbases.size());
	for (int idx(0); idx < solbases.size(); ++idx) {
		auto& nei = hnswModel.getNeighbors(idx);
		auto& curnei = model_neighbors[idx];
		for (auto& neiId : nei.neighbors()) {
			curnei.push_back(neiId.nodeId());
		}
	}



	//for (int idx(0); idx < solbases.size(); ++idx) {
	//	auto& nei = model_neighbors[idx][0];
	//	auto& curnei = neighbors[idx];
	//	curnei = nei;
	//}

	{
		std::cout << "begin nbn" << std::endl;
		auto start = std::chrono::system_clock::now();
		ofec::NBN_NearestBetterCalculator::calculate(solbases, vFitness, model_neighbors,
			belong, dis2parent,
			pro, rnd);
		auto end = std::chrono::system_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
		std::cout << "nbn calculate costs"
			<< double(duration.count()) * std::chrono::microseconds::period::num / std::chrono::microseconds::period::den
			<< "seconds" << std::endl;
	}
}


void calculateNetwork(NBNinfo& nbnInfo, ofec::TreeGraphSimple& nbn_graph, ofec::Environment* env, ofec::Random* rnd) {
	std::vector<int> solIds(nbnInfo.solbases.size());
	for (int idx(0); idx < solIds.size(); ++idx) {
		solIds[idx] = idx;
	}
	std::vector<ofec::TreeGraphSimple::NBNdata> nbn_data;
	ofec::transferNBNData(nbn_data, solIds, nbnInfo.belong, nbnInfo.dis2parent, nbnInfo.vFitness);

	;
	nbn_graph.setNBNdata(nbn_data);
	nbn_graph.modifyBestSols(rnd, env->problem(), nbnInfo.solbases);
	nbn_graph.calNetwork(1.0);
}



void nbnCalculator(
	const std::string& saveDir, const std::string& filename,
	std::vector<std::shared_ptr<ofec::SolutionBase>>& sols, ofec::Environment* env, ofec::Random* rnd) {


	ofec::ParameterVariantStream paramsStream;
	auto filepath = saveDir + filename + "_nbnfit.txt";
	inputFromFile(paramsStream, filepath);
	std::vector<double> vFitness;
	paramsStream >> vFitness;

	NBNinfo nbnInfo;
	nbnInfo.m_sols = sols;
	for (auto& it : nbnInfo.m_sols) {
		nbnInfo.solbases.push_back(it.get());
	}
	nbnInfo.vFitness = vFitness;
	for (int idx(0); idx < nbnInfo.solbases.size(); ++idx) {
		nbnInfo.solbases[idx]->setFitness(vFitness[idx]);
	}
	calculateNBNsingleThread(nbnInfo, env, rnd);
	//calculateNBN(nbnInfo, env, rnd);

	ofec::TreeGraphSimple nbn_graph;
	calculateNetwork(nbnInfo, nbn_graph, env, rnd);

	{
		std::string savepath = saveDir + filename + "_network.txt";
		std::ofstream out(savepath);
		nbn_graph.outputNBNnetwork(out);
		out.close();
		ofec::ouputNBNdata(saveDir + filename + "_nbn.txt", nbn_graph.get_nbn_data());
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



void createEnvironment(std::shared_ptr<ofec::Environment>& envCopy, ofec::Environment* env) {
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
	std::vector<std::shared_ptr<Environment>> envs(num_task);
	std::vector<std::shared_ptr<Random>> rnds(num_task);
	for (auto& it : envs) {
		createEnvironment(it, env);
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
		cswidnPro->setSourceIdx(m_source_idx);
		cswidnPro->setPhase(m_current_phase);

	}


};


void fitlerSameSolutions(std::vector<ofec::SolutionBase*>& sols, std::vector<int>& mapSolId, ofec::Environment* env) {
	using namespace ofec;
	auto pro = env->problem();
	auto cswidnPro = CAST_CSIWDN(pro);

	mapSolId.resize(sols.size());
	for (int idx(0); idx < mapSolId.size(); ++idx) {
		mapSolId[idx] = idx;
	}
	std::vector<int> sortedIds(sols.size());
	for (int idx(0); idx < sortedIds.size(); ++idx) {
		sortedIds[idx] = idx;
	}

	std::sort(sortedIds.begin(), sortedIds.end(), [&](int a, int b) {
		return sols[a]->fitness() < sols[b]->fitness();
		});

	std::vector<int> beforeSolIds;
	for (auto& solId : sortedIds) {
		if (beforeSolIds.empty()) {
			beforeSolIds.push_back(solId);
		}
		else if (sols[beforeSolIds.front()]->fitness() == sols[solId]->fitness()) {
			auto& cursol = sols[solId];
			for (auto& otherId : beforeSolIds) {
				auto& otherSol = sols[otherId];
				if (pro->same(cursol->variableBase(), otherSol->variableBase())) {
					mapSolId[solId] = otherId;
				}
			}
		}
		else {
			beforeSolIds.clear();
			beforeSolIds.push_back(solId);
		}
	}

}



void fitlerSameSolutions2(std::vector<ofec::SolutionBase*>& sols, std::vector<int>& mapSolId, ofec::Environment* env) {
	using namespace ofec;
	auto pro = env->problem();
	auto cswidnPro = CAST_CSIWDN(pro);

	mapSolId.resize(sols.size());
	for (int idx(0); idx < mapSolId.size(); ++idx) {
		mapSolId[idx] = idx;
	}
	std::vector<int> sortedIds(sols.size());
	for (int idx(0); idx < sortedIds.size(); ++idx) {
		sortedIds[idx] = idx;
	}

	std::vector<ofec::CSIWDN::solutionType*> cswidn_sols;
	for (auto& it : sols) {
		cswidn_sols.push_back(dynamic_cast<ofec::CSIWDN::solutionType*>(it));
	}

	std::sort(sortedIds.begin(), sortedIds.end(), [&](int a, int b) {
		if (cswidn_sols[a]->fitness() == cswidn_sols[b]->fitness()) {
			bool flag = false;
			for(int idx(0);idx<)
		}
		else return cswidn_sols[a]->fitness() < cswidn_sols[b]->fitness();
		});

	std::vector<int> beforeSolIds;
	for (auto& solId : sortedIds) {
		if (beforeSolIds.empty()) {
			beforeSolIds.push_back(solId);
		}
		else if (sols[beforeSolIds.front()]->fitness() == sols[solId]->fitness()) {
			auto& cursol = sols[solId];
			for (auto& otherId : beforeSolIds) {
				auto& otherSol = sols[otherId];
				if (pro->same(cursol->variableBase(), otherSol->variableBase())) {
					mapSolId[solId] = otherId;
				}
			}
		}
		else {
			beforeSolIds.clear();
			beforeSolIds.push_back(solId);
		}
	}

}


void evaluateSolAndOutputTask(const std::string& saveDir, const std::string& processName, TaskInfo& curTask) {
	auto env = curTask.env;
	auto rnd = curTask.rnd;
	auto& algSols = curTask.algSols;

	auto filename = curTask.getFileName() + "_algSol_Info";
	if (waitForFile(saveDir, filename, processName)) {
		//auto& algSols = sub_sols[proId];
		curTask.modifyEnv(env);

		//cswidnPro->setDistanceType(CSIWDN::DistanceType::Mix);
		//cswidnPro->setSourceIdx(curTask.m_source_idx);
		evaluateSols(algSols, env, rnd);
		std::vector<int> mapSolId;
		fitlerSameSolutions(algSols, mapSolId, env);
		// output solutions infos;
		{
			std::vector<ofec::Real> fitness;
			for (auto& it : algSols) {
				fitness.push_back(it->fitness());
			}
			ofec::ParameterVariantStream paramsStream;
			paramsStream << fitness;
			paramsStream << mapSolId;

			outputToFile(paramsStream, saveDir + filename + ".txt");

		}

	}
}


void outputTotalSolInfo(const std::string& saveDir, const std::string& processName, TaskInfo& curTask) {
	using namespace ofec;
	auto filename = curTask.getFileName() + "_algSol_Sol";
	/*if (waitForFile(saveDir, filename, processName)) */ {
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


		outputToFile(paramsStream, saveDir + filename + ".txt");

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
	int numRun = 1;


	std::shared_ptr<ofec::Environment> env;
	genenratePro(proname, params, env);
	auto pro = env->problem();
	auto cswidnPro = CAST_CSIWDN(pro);
	auto rnd = make_shared<ofec::Random>(0.5);
	cswidnPro->setProcessName(processName);


	std::vector<std::shared_ptr<ofec::SolutionBase>> sols;
	std::vector<std::shared_ptr<ofec::SolutionInfo>> solInfos;


	{
		time_t timer_start, timer_end;
		time(&timer_start);
		ofec::SamplingMutithread::runAlgMultiTask(sols, solInfos, 0.5, 0.5, proname, params, algName, params, numRun);
		time(&timer_end);
		std::cout << "runAlgMultiTask Time used: " << difftime(timer_end, timer_start) << " seconds" << std::endl;
	}
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

				outputToFile(paramsStream, saveDir + filename + ".txt");

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


	TaskInfo curTask;
	curTask.m_disType = ofec::CSIWDN::DistanceType::Mix;
	for (int proId(0); proId < proPhases.size(); ++proId) {
		curTask.getInfo(*sub_solInfos[proId].front());
		curTask.algSolInfos = sub_solInfos[proId];
		curTask.algSols = sub_sols[proId];
		curTask.m_current_phase = 0;

		outputTotalSolInfo(saveDir, processName, curTask);


		//for (auto& curphase : proPhases[proId]) {
		//	curTask.m_current_phase = curphase;
		//	evaluateSolAndOutputTask(saveDir, processName, curTask);
		//
		//}
	}

}






void runAlg2(const std::string& saveDir, const std::string& processName) {



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


	{
		time_t timer_start, timer_end;
		time(&timer_start);
		ofec::SamplingMutithread::runAlgMultiTask(sols, solInfos, 0.5, 0.5, proname, params, algName, params, numRun);
		time(&timer_end);
		std::cout << "runAlgMultiTask Time used: " << difftime(timer_end, timer_start) << " seconds" << std::endl;
	}
	// output solutions



	std::vector<std::vector<int>> vecProTypes;
	std::vector<int> curProType(3);
	{
		// output pro type
		std::string filename = "ProType";
		/*if (waitForFile(saveDir, filename, processName))*/ {
			{

				ofec::ParameterVariantStream paramsStream;
				inputFromFile(paramsStream, saveDir + filename + ".txt");
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


	std::map<std::vector<int>, int> proTypeId;
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

	std::vector<std::set<int>> proPhases(proTypeId.size());
	curProType.resize(2);
	for (auto& it : vecProTypes) {
		curProType[0] = it[1];
		curProType[1] = it[2];
		int proId = proTypeId[curProType];
		proPhases[proId].insert(it[0]);
	}


	//std::vector<std::set<int>> proPhases(proTypeId.size());
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

		//	proPhases[proId].insert(curInfo->m_current_phase);

	}


	TaskInfo curTask;
	curTask.m_disType = ofec::CSIWDN::DistanceType::Mix;
	for (int proId(0); proId < proPhases.size(); ++proId) {
		curTask.getInfo(*sub_solInfos[proId].front());
		curTask.algSolInfos = sub_solInfos[proId];
		curTask.algSols = sub_sols[proId];
		curTask.m_current_phase = 0;

		outputTotalSolInfo(saveDir, processName, curTask);


		//for (auto& curphase : proPhases[proId]) {
		//	curTask.m_current_phase = curphase;
		//	evaluateSolAndOutputTask(saveDir, processName, curTask);
		//
		//}
	}

	//int stop = -1;

}



void runTotalTasks(const std::string& processName) {

	using namespace ofec;
	using namespace std;
	ofec::g_working_directory = "//172.24.207.203/share/2018/diaoyiya/ofec-data";
	//ofec::g_working_directory = "/home/lab408/share/2018/diaoyiya/ofec-data";
	ofec::g_working_directory = "//172.29.41.69/share/2018/diaoyiya/ofec-data";
	//ofec::g_working_directory = "//172.29.203.176/e/DiaoYiya/code/data/ofec-data";
	//ofec::g_working_directory = "E:/DiaoYiya/code/data/ofec-data";
	//ofec::g_working_directory = "//172.29.203.176/e/DiaoYiya/code/data/ofec-data";
	//ofec::g_working_directory = "/mnt/Data/Student/2018/YiyaDiao/code_total/data";
	//ofec::g_working_directory = "//172.24.242.8/share/Student/2018/YiyaDiao/code_total/data";


	auto saveDir = ofec::g_working_directory + "/cswidn_algorithm_total_test/";

	std::filesystem::create_directories(saveDir);
	//runAlg(saveDir, processName);
	runAlg2(saveDir, processName);

	//runAlg(saveDir);




	//GlobalInfo globalInfo;
	//globalInfo.saveDir = saveDir;
	//globalInfo.m_win_name = "_winServer_1";
	//globalInfo.m_win_name = "winLocal";
	////std::list<ProInfo>infos;
	//auto& infos = globalInfo.m_proInfos;
	//std::vector<std::string> proNames = { "gear-train", "FM-sound-waves", "GTOP_Cassini2" ,"RW_CEC2011_T2" };

	//std::vector<int> precisions = { 0,1,2,3,4,5,6,7 };


	////double curConRatio = 0.01;
	//for (auto& proName : proNames) {
	//	for (auto& curphase : precisions) {
	//		ProInfo localInfo;
	//		localInfo.m_initPre = curphase;
	//		localInfo.m_proName = proName;
	//		infos.push_back(localInfo);
	//	}
	//}


	//runTask(globalInfo);


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


		//runTask();
		//runTotalTasks(std::string(argv[1]));

		runTotalTasks("test");


	}


}
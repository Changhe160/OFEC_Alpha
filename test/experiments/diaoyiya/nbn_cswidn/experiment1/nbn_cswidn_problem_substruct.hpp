
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
//#include "../utility/nbn_visualization/nbn_calculator/nearest_better_calculator_multiThead.h"

#include "../utility/hnsw/hnsw_nbn/neighbor_info.h"
#include "../utility/hnsw/hnsw_nbn/select_policy.h"
#include "../utility/hnsw/hnsw_nbn/node.h"
#include "../utility/hnsw/hnsw_nbn/hnsw_model.h"
#include "../utility/hnsw/hnsw_nbn/hnsw_nbn.h"
#include "../core/environment/environment.h"
#include "../core/parameter/variants_to_stream.h"

//#include "../instance/algorithm/combination/lkh/lkh_algorithm.h"
#include<filesystem>
#include<queue>
#include <set>

#include "../core/parameter/variants_to_stream.h"
//#include "../instance/algorithm/combination/eax_tsp/eax_tsp_test/eax_tsp_alg.h"
//#include "../instance/problem/combination/travelling_salesman/tsp_offline_data/travelling_salesman_offline_data.h"

//#include "../utility/nbn_visualization/nbn_fla/nbn_fla.h"
//#include "../instance/algorithm/combination/eax_tsp/eax_tsp_basin/eax_tsp_basin_multipop.h"
//#include "../utility/nbn_visualization/nbn_fla/nbn_fla_utility.h"
//#include "../instance/algorithm/combination/LKH_origin/INCLUDE/LKH.h"
#include <iostream>
#include <iomanip>
#include <ctime>
#include <sstream>

//#include "../utility/visualization/idee_trait.h"
#include "../instance/problem/realworld/csiwdn/csiwdn.h"

#include "../utility/nbn_visualization/nbn_fla/nbn_info.h"

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


void generateSolRasiu(std::shared_ptr<ofec::SolutionBase>& initSol,
	const ofec::SolutionBase& centerSol,
	double sampleRadius,
	ofec::Environment* env,
	ofec::Random* rnd) {

	using namespace std;
	using namespace ofec;
	auto cswidnPro = CAST_CSIWDN(env->problem());
	auto& curCsol = dynamic_cast<const ofec::CSIWDN::solutionType&>(centerSol);

	double halfRadius = sampleRadius / 2.0 * (cswidnPro->maxMultiplier() - cswidnPro->minMultiplier());

	std::pair<double, double> proRange = { cswidnPro->minMultiplier(), cswidnPro->maxMultiplier() };

	std::vector<std::vector<std::pair<double, double>>> targetRange(cswidnPro->numSource());
	for (int idx(0); idx < targetRange.size(); ++idx) {
		targetRange[idx].resize(curCsol.variable().multiplier(idx).size());

		for (int idy(0); idy < targetRange[idx].size(); ++idy) {
			targetRange[idx][idy].first = curCsol.variable().multiplier(idx)[idy] - halfRadius;
			targetRange[idx][idy].second = curCsol.variable().multiplier(idx)[idy] + halfRadius;
		}
	}

	for (auto& it : targetRange) {
		for (auto& it2 : it) {
			if (it2.first < proRange.first) {
				double left = proRange.first - it2.first;
				it2.first = proRange.first;
				it2.second += left;
			}
			else if (it2.second > proRange.second) {
				double left = it2.second - proRange.second;
				it2.second = proRange.second;
				it2.first -= left;
			}
		}
	}



	//	initSol.reset(env->problem()->createSolution(centerSol));
	auto& cursol = dynamic_cast<ofec::CSIWDN::solutionType&>(*initSol);
	//	cursol.variable().flagLocation() = false;
		//cswidnPro->initSolutionMultiplier(cursol, targetRange, rnd);

	VarCSIWDN& var = cursol.variable();
	for (size_t z = 0; z < cswidnPro->numSource(); z++) {
		for (int j(0); j < var.multiplier(z).size(); ++j) {
			if (rnd->uniform.next() < 0.5) {
				var.multiplier(z)[j] = targetRange[z][j].first;
			}
			else var.multiplier(z)[j] = targetRange[z][j].second;
			//var.multiplier(z)[j] = rnd->uniform.nextNonStd<double>(range[z][j].first, range[z][j].second);
		}
		//for (auto& j : var.multiplier(z)) {
		//	j = rnd->uniform.nextNonStd<float>(m_min_multiplier, m_max_multiplier);
		//}
	}
}



void generateSolIntRasiu(std::shared_ptr<ofec::SolutionBase>& initSol,
	const ofec::SolutionBase& centerSol,
	int sampleRadius,
	ofec::Environment* env,
	ofec::Random* rnd) {

	using namespace std;
	using namespace ofec;
	auto cswidnPro = CAST_CSIWDN(env->problem());
	auto& curCsol = dynamic_cast<const ofec::CSIWDN::solutionType&>(centerSol);

	auto numSource = cswidnPro->numberSource();
	std::vector<int> shuffleIds(numSource);
	for (int idx(0); idx < shuffleIds.size(); ++idx) {
		shuffleIds[idx] = idx;
	}
	rnd->uniform.shuffle(shuffleIds.begin(), shuffleIds.end());
	shuffleIds.resize(sampleRadius);

	auto& cursol = dynamic_cast<ofec::CSIWDN::solutionType&>(*initSol);
	//cursol.variable().flagLocation() = false;


	for (auto z : shuffleIds) {
		int beforeNode = cursol.variable().index(z);
		cursol.variable().index(z) = rnd->uniform.nextNonStd<int>(1, cswidnPro->numberNode());
		//   if (cursol.variable().index(z) >= beforeNode) ++cursol.variable().index(z);
	}

}





// 函数用于将十进制数转换为M进制数
void decimalToM(int decimal, int M, int numSource, std::vector<int>& data) {

	data.clear();
	// 持续除以M，直到decimal为0
	while (numSource--) {
		int remainder = decimal % M;
		data.push_back(remainder);
		//char symbol = (remainder < 10) ? ('0' + remainder) : ('A' + (remainder - 10));
		//result = symbol + result; // 将余数添加到结果的前面
		decimal /= M; // 更新十进制数
	}

}






void generateSolutionsIntsSub(
	const ofec::SolutionBase& centerSol,
	std::vector<std::shared_ptr<ofec::SolutionBase>>& sols, int from, int to, ofec::Environment* env, ofec::Random* rnd) {
	using namespace std;
	using namespace ofec;

	auto cswidnPro = CAST_CSIWDN(env->problem());

	std::vector<int> data;
	for (int idx(from); idx < to; ++idx) {
		sols[idx].reset(env->problem()->createSolution(centerSol));

		auto& cursol = dynamic_cast<ofec::CSIWDN::solutionType&>(*sols[idx]);
		cursol.variable().flagLocation() = false;

		decimalToM(idx, cswidnPro->numberNode(), cswidnPro->numSource(), data);
		for (auto& it : data) {
			++it;
		}
		for (size_t z = 0; z < cswidnPro->numSource(); z++) {
			cursol.variable().index(z) = data[z];
		}

		//	cursol.evaluate(env, false);
	}

}



void generateSolutionsConsRandomSub(
	const ofec::SolutionBase& centerSol,
	std::vector<std::shared_ptr<ofec::SolutionBase>>& sols, int from, int to, ofec::Environment* env, ofec::Random* rnd) {
	using namespace std;
	using namespace ofec;

	auto cswidnPro = CAST_CSIWDN(env->problem());

	std::vector<int> data;
	for (int idx(from); idx < to; ++idx) {
		sols[idx].reset(env->problem()->createSolution(centerSol));
		auto& cursol = dynamic_cast<ofec::CSIWDN::solutionType&>(*sols[idx]);
		cursol.variable().flagLocation() = false;
		cswidnPro->initSolutionMultiplier(cursol, rnd);
		//			CAST_CSIWDN(pro)->initSolutionMultiplier(cursol, rnd);
	}

}



void generateSolutionsConsRadiusSub(
	const ofec::SolutionBase& centerSol,
	double sampleRadius,
	std::vector<std::shared_ptr<ofec::SolutionBase>>& sols, int from, int to, ofec::Environment* env, ofec::Random* rnd) {
	using namespace std;
	using namespace ofec;
	auto cswidnPro = CAST_CSIWDN(env->problem());
	auto& curCsol = dynamic_cast<const ofec::CSIWDN::solutionType&>(centerSol);

	auto& optSol = cswidnPro->optima()->solution(0);

	double halfRadius = sampleRadius / 2.0 * (cswidnPro->maxMultiplier() - cswidnPro->minMultiplier());

	std::pair<double, double> proRange = { cswidnPro->minMultiplier(), cswidnPro->maxMultiplier() };

	std::vector<std::vector<std::pair<double, double>>> targetRange(cswidnPro->numSource());
	for (int idx(0); idx < targetRange.size(); ++idx) {
		targetRange[idx].resize(optSol.variable().multiplier(idx).size());

		for (int idy(0); idy < targetRange[idx].size(); ++idy) {
			targetRange[idx][idy].first = optSol.variable().multiplier(idx)[idy] - halfRadius;
			targetRange[idx][idy].second = optSol.variable().multiplier(idx)[idy] + halfRadius;
		}
	}

	for (auto& it : targetRange) {
		for (auto& it2 : it) {
			if (it2.first < proRange.first) {
				//	double left = proRange.first - it2.first;
				it2.first = proRange.first;
				//	it2.second += left;
			}
			else if (it2.second > proRange.second) {
				//	double left = it2.second - proRange.second;
				it2.second = proRange.second;
				//	it2.first -= left;
			}
		}
	}



	std::vector<int> data;
	for (int idx(from); idx < to; ++idx) {
		sols[idx].reset(env->problem()->createSolution(centerSol));
		auto& cursol = dynamic_cast<ofec::CSIWDN::solutionType&>(*sols[idx]);
		cursol.variable().flagLocation() = false;
		cswidnPro->initSolutionMultiplier(cursol, targetRange, rnd);
		//			CAST_CSIWDN(pro)->initSolutionMultiplier(cursol, rnd);
	}

}




struct proInfo {
	ofec::CSIWDN::DistanceType m_initType = ofec::CSIWDN::DistanceType::Mix;
	int m_stage = 48;
	double m_initRadius = 1.0;
	double m_aroundConCenterRadius = 0;
	int m_aoundIntCenterRaduis = 0;

	int m_precision = 0;



	std::shared_ptr<ofec::Environment> m_env;
	std::shared_ptr<ofec::Random> m_rnd;
	std::shared_ptr<ofec::SolutionBase> m_center;

	int total_file = 0;
	std::string m_winName = "";
	void initialize(const std::string& winName) {

		//m_initRadius = pow(0.1, m_precision);
		m_winName = winName;
		total_file = 0;
		modify();
		m_rnd.reset(new ofec::Random(0.5));
		createEnvironment(m_env);

		createCenterSolution();

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
	// 定义比较运算符 < ，以便 std::set 使用
	bool operator<(const proInfo& other) const {
		if (m_initType == other.m_initType) {
			if (m_stage == other.m_stage) {
				if (m_initRadius == other.m_initRadius) {
					if (m_aroundConCenterRadius == other.m_aroundConCenterRadius) {
						return m_aoundIntCenterRaduis < other.m_aoundIntCenterRaduis;
					}
					else return m_aroundConCenterRadius < other.m_aroundConCenterRadius;
				}
				else return m_initRadius < other.m_initRadius;
			}
			else return m_stage < other.m_stage;
		}
		else m_initType < other.m_initType;

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

		return "CSIWDN_" + nbnType + "_stage_" + std::to_string(m_stage) + "_aroundRadius_" + aourndRadiusName + "_initRadiusPrecision_" + std::to_string(int(m_initRadius * 1e6));
	}
	void createEnvironment(std::shared_ptr<ofec::Environment>& env) {
		using namespace std;
		using namespace ofec;
		ofec::ParameterMap params;


		std::string proname = "CSIWDN";
		params["problem name"] = std::string("CSIWDN");
		params["net-sensor-source case"] = std::string("Net2/sensor_case1/source/case11.txt");

		auto filernd = m_winName + "_" + std::to_string(total_file++);
		params["process name"] = m_winName;
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
		//std::string filernd = UTILITY::getCurrentSystemTime() + "_" + UTILITY::createRandomString(50, m_rnd.get());

		cswidnPro->setProcessName(filernd);
		env->initializeProblem(0.5);

		cswidnPro->setDistanceType(m_initType);
		cswidnPro->setPhase(m_stage);


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
			generateSolIntRasiu(m_center, optBase, m_aoundIntCenterRaduis, m_env.get(), m_rnd.get());
		}
		if (m_aroundConCenterRadius > 0) {
			generateSolRasiu(m_center, optBase, m_aroundConCenterRadius, m_env.get(), m_rnd.get());
		}

	}






	void generateSolutionsInts(
		const ofec::SolutionBase& centerSol,
		std::vector<std::shared_ptr<ofec::SolutionBase>>& sols) {
		using namespace std;
		using namespace ofec;

		auto env = m_env.get();
		auto rnd = m_rnd.get();
		auto cswidnPro = CAST_CSIWDN(env->problem());
		sols.resize(pow(cswidnPro->numberNode(), cswidnPro->numSource()));



		int num_task = std::thread::hardware_concurrency();
		// for test 
		//num_task = 1;
		std::cout << "generateSolutionsInts num task\t" << num_task << std::endl;
		std::vector<std::shared_ptr<Environment>> envs(num_task);
		std::vector<std::shared_ptr<Random>> rnds(200);
		for (auto& it : envs) {
			createEnvironment(it);
			auto curpro = CAST_CSIWDN(it->problem());
			curpro->setPhase(cswidnPro->phase());
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
				generateSolutionsIntsSub, std::cref(centerSol), std::ref(sols),
				tasks[i], tasks[i + 1], envs[i].get(), rnds[i].get()));
		}
		for (auto& thrd : thrds)
			thrd.join();


	}
	void generateSolutionsConRandom(
		const ofec::SolutionBase& centerSol,
		std::vector<std::shared_ptr<ofec::SolutionBase>>& sols
	) {
		using namespace std;
		using namespace ofec;
		auto env = m_env.get();
		auto rnd = m_rnd.get();
		auto cswidnPro = CAST_CSIWDN(env->problem());
		sols.resize(1e6);



		int num_task = std::thread::hardware_concurrency();

		std::vector<std::shared_ptr<Environment>> envs(num_task);
		std::vector<std::shared_ptr<Random>> rnds(200);
		for (auto& it : envs) {
			createEnvironment(it);
			auto curpro = CAST_CSIWDN(it->problem());
			curpro->setPhase(cswidnPro->phase());
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
				generateSolutionsConsRandomSub, std::cref(centerSol), std::ref(sols),
				tasks[i], tasks[i + 1], envs[i].get(), rnds[i].get()));
		}
		for (auto& thrd : thrds)
			thrd.join();
	}

	void generateSolutionsConRadius(
		const ofec::SolutionBase& centerSol,
		double sampleRadius,
		std::vector<std::shared_ptr<ofec::SolutionBase>>& sols
	) {
		using namespace std;
		using namespace ofec;
		auto env = m_env.get();
		auto rnd = m_rnd.get();
		auto cswidnPro = CAST_CSIWDN(env->problem());
		sols.resize(1e6);



		int num_task = std::thread::hardware_concurrency();
		std::cout << "generateSolutionsConRadius num task\t" << num_task << std::endl;
		std::vector<std::shared_ptr<Environment>> envs(num_task);
		std::vector<std::shared_ptr<Random>> rnds(200);
		for (auto& it : envs) {
			createEnvironment(it);
			auto curpro = CAST_CSIWDN(it->problem());
			curpro->setPhase(cswidnPro->phase());
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
				generateSolutionsConsRadiusSub, std::cref(centerSol), sampleRadius, std::ref(sols),
				tasks[i], tasks[i + 1], envs[i].get(), rnds[i].get()));
		}
		for (auto& thrd : thrds)
			thrd.join();
	}


	void genereateSols(std::vector<std::shared_ptr<ofec::SolutionBase>>& sols) {
		if (m_initType == ofec::CSIWDN::DistanceType::Continous) {
			generateSolutionsConRadius(*m_center, m_initRadius, sols);
		}
		else if (m_initType == ofec::CSIWDN::DistanceType::Integer) {
			generateSolutionsInts(*m_center, sols);
		}
	}

};




void evaluateSolsSubTask(std::vector<std::shared_ptr<ofec::SolutionBase>>& sols, int from, int to, ofec::Environment* env, ofec::Random* rnd) {

	auto pro = env->problem();
	ofec::Real pos = (pro->optimizeMode(0) == ofec::OptimizeMode::kMaximize) ? 1 : -1;

	for (int idx(from); idx < to; ++idx) {
		auto& cursol = dynamic_cast<ofec::CSIWDN::solutionType&>(*sols[idx]);
		cursol.variable().flagLocation() = false;
		sols[idx]->evaluate(env, false);
		sols[idx]->setFitness(pos * sols[idx]->objective(0));
	}
}

void evaluateSols(std::vector<std::shared_ptr<ofec::SolutionBase>>& sols, proInfo& info) {
	using namespace std;
	using namespace ofec;

	int num_task = std::thread::hardware_concurrency();
	std::vector<std::shared_ptr<Environment>> envs(num_task);
	std::vector<std::shared_ptr<Random>> rnds(num_task);
	for (auto& it : envs) {
		info.createEnvironment(it);
	}
	auto rnd = info.m_rnd.get();
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







struct GlobalInfo {
	//std::string m_win_name = "winLocal";
	std::list<proInfo> m_proInfos;
	std::string nbnSaveDir;
	std::string networkSaveDir;

	std::mutex info_mtx;
};


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


void outputTotalSolInfo(const std::string& saveDir, const std::string& filename,
	std::vector<ofec::SolutionBase*> sols, ofec::Environment* env) {
	using namespace ofec;

	auto pro = env->problem();
	/*if (waitForFile(saveDir, filename, processName)) */ {
		ofec::ParameterVariantStream paramsStream;

		paramsStream << sols.size();
		for (auto& it : sols) {
			pro->solutionToParameterVariants(*it, paramsStream);
		}

		ofec::variants_stream::outputToFile(paramsStream, saveDir + filename + "_solutions.txt");
		//ofecoutputToFile(paramsStream, saveDir + filename + ".txt");

	}
}


void runTask(GlobalInfo& info) {

	bool hashTask = true;

	proInfo localInfo;

	while (hashTask) {
		hashTask = false;
		{
			info.info_mtx.lock();
			if (!info.m_proInfos.empty()) {
				localInfo = info.m_proInfos.front();
				info.m_proInfos.pop_front();
				hashTask = true;
			}
			info.info_mtx.unlock();
		}


		if (hashTask) {
			auto filename = localInfo.getFileName();
			std::vector<std::filesystem::path> results;
			if (file_exists_with_prefix(results, info.networkSaveDir, filename)) {
				continue;
			}
			auto testfilename = filename + localInfo.m_winName + ".dat";
			{
				std::ofstream out(info.networkSaveDir + testfilename);
				out << "yes" << std::endl;
				out.close();
			}
			// 获取当前时间
			auto now = std::chrono::steady_clock::now();
			// 设置2分钟后的时间
			auto wait_until = now + std::chrono::minutes(2);
			// 让当前线程等待，直到 wait_until
			std::this_thread::sleep_until(wait_until);
			file_exists_with_prefix(results, info.networkSaveDir, filename);
			std::sort(results.begin(), results.end());
			if (results.front() == testfilename) {

				std::cout << "runnig\t" << testfilename << std::endl;
				localInfo.initialize(localInfo.m_winName);
				std::vector<std::shared_ptr<ofec::SolutionBase>> sols;
				localInfo.genereateSols(sols);

				evaluateSols(sols, localInfo);

				std::vector<ofec::SolutionBase*> solbases;
				for (auto& it : sols) {
					solbases.push_back(it.get());
				}
				outputTotalSolInfo(info.nbnSaveDir, filename, solbases, localInfo.m_env.get());


				//std::vector<double> vFitness;
				//for (auto& it : sols) {
				//	vFitness.push_back(it->fitness());
				//}


				//ofec::ParameterVariantStream paramsStream;
				//paramsStream << vFitness;
				//auto filepath = info.nbnSaveDir + filename + "_nbnfit.txt";
				//outputToFile(paramsStream, filepath);




			}


		}


	}
}






void runNBNTask(GlobalInfo& info) {

	bool hashTask = true;

	proInfo localInfo;

	while (hashTask) {
		hashTask = false;
		{
			//info.info_mtx.lock();
			if (!info.m_proInfos.empty()) {
				localInfo = info.m_proInfos.front();
				info.m_proInfos.pop_front();
				hashTask = true;
			}
			//info.info_mtx.unlock();
		}


		if (hashTask) {
			auto filename = localInfo.getFileName();


			std::cout << "runnig\t" << filename << std::endl;
			localInfo.initialize(localInfo.m_winName);
			std::vector<std::shared_ptr<ofec::SolutionBase>> sols;
			localInfo.genereateSols(sols);
			//std::vector<std::filesystem::path> results;
			//if (file_exists_with_prefix(results, info.saveDir, filename+"_nbnfit.txt")) {
			//	continue;
			//}



			ofec::nbn::NBNinfo nbnInfo;
			for (auto& it : sols) {
				nbnInfo.m_solbases.push_back(it.get());
			}


			calculateNBN(nbnInfo, info.nbnSaveDir, info.networkSaveDir, filename, localInfo.m_env.get(), localInfo.m_rnd.get());


			//if (localInfo.m_initType == ofec::CSIWDN::DistanceType::Integer) {
			//	nbnCalculatorInt(info.saveDir, filename, sols, localInfo.m_env.get(), localInfo.m_rnd.get());
			//}
			//else if (localInfo.m_initType == ofec::CSIWDN::DistanceType::Continous) {
			//	nbnCalculator(info.saveDir, filename, sols, localInfo.m_env.get(), localInfo.m_rnd.get());
			//}
			//evaluateSols(sols, localInfo);
			//std::vector<double> vFitness;
			//for (auto& it : sols) {
			//	vFitness.push_back(it->fitness());
			//}

			//ofec::ParameterVariantStream paramsStream;
			//paramsStream << vFitness;
			//auto filepath = info.saveDir + filename + "_nbnfit.txt";
			//outputToFile(paramsStream, filepath);

		}


	}
}


void runTotalTasks(const std::string& processName) {

	using namespace ofec;
	using namespace std;
	ofec::g_working_directory = "//172.24.207.203/share/2018/diaoyiya/ofec-data";
	ofec::g_working_directory = "/home/lab408/share/2018/diaoyiya/ofec-data";
	//ofec::g_working_directory = "//172.29.204.96/share/2018/diaoyiya/ofec-data";
	//ofec::g_working_directory = "//172.29.203.176/e/DiaoYiya/code/data/ofec-data";
	//ofec::g_working_directory = "E:/DiaoYiya/code/data/ofec-data";
	//ofec::g_working_directory = "/mnt/Data/Student/2018/YiyaDiao/code_total/data";
	//ofec::g_working_directory = "//172.24.242.8/share/Student/2018/YiyaDiao/code_total/data";


	auto saveDir = ofec::g_working_directory + "/paper_cswidn/cswidn_problem_nbn/";
	auto saveDir2 = ofec::g_working_directory + "/paper_cswidn/cswidn_problem_network/";
	std::filesystem::create_directories(saveDir);
	std::filesystem::create_directories(saveDir2);

	GlobalInfo globalInfo;
	globalInfo.nbnSaveDir = saveDir;
	globalInfo.networkSaveDir = saveDir2;
	//globalInfo.m_win_name = processName;
	//	globalInfo.m_win_name = "win_2";
	std::list<proInfo>infos;

	std::vector<int> phases = { 15,16,20,24,25,26,27,28,31,32 };
	// 时间相关
//	phases = { 1 };
	double curConRatio = 0.01;
	//for (auto& curphase : phases) {
	//	proInfo localInfo;
	//	localInfo.m_stage = curphase;
	//	localInfo.m_initType = ofec::CSIWDN::DistanceType::Continous;
	//	localInfo.m_initRadius = curConRatio;
	//	infos.push_back(localInfo);
	//}
	curConRatio = 1.0;
	for (auto& curphase : phases) {
		proInfo localInfo;
		localInfo.m_winName = processName;
		localInfo.m_stage = curphase;
		localInfo.m_initType = ofec::CSIWDN::DistanceType::Continous;
		localInfo.m_initRadius = curConRatio;
		infos.push_back(localInfo);
	}

	for (auto& curphase : phases) {
		proInfo localInfo;
		localInfo.m_winName = processName;
		localInfo.m_stage = curphase;
		localInfo.m_initType = ofec::CSIWDN::DistanceType::Integer;
		infos.push_back(localInfo);
	}


	// 迁移相关

	std::vector<double> disRange = { 0.1,0.01,0.001 };
	double defaultStage = 28;

	for (auto& curRoundDis : disRange) {
		proInfo localInfo;
		localInfo.m_winName = processName;
		localInfo.m_stage = defaultStage;
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
		localInfo.m_stage = defaultStage;
		localInfo.m_winName = processName;
		localInfo.m_initType = ofec::CSIWDN::DistanceType::Continous;
		localInfo.m_aoundIntCenterRaduis = intDis;
		infos.push_back(localInfo);
	}

	// 局部 实验 
	std::vector<double> initDis = { 0.01,0.0001,0.000001 };
	for (auto& curinit : initDis) {
		proInfo localInfo;
		localInfo.m_winName = processName;
		localInfo.m_stage = defaultStage;
		localInfo.m_initType = ofec::CSIWDN::DistanceType::Continous;
		localInfo.m_initRadius = curinit;
		infos.push_back(localInfo);
	}


	for (auto& it : infos) {
		globalInfo.m_proInfos.push_back(it);
	}
	//	std::reverse(globalInfo.m_proInfos.begin(), globalInfo.m_proInfos.end());

		//runTask(globalInfo);
	runTask(globalInfo);
	//runNBNTask(globalInfo);


}



namespace ofec {

	void registerParamAbbr() {}
	void customizeFileName() {}
	void run(int argc, char* argv[]) {

		using namespace ofec;
		using namespace std;

		registerInstance();


		std::string filename = "run_test";
		std::cout << "calculate NBN problem fitness task" << std::endl;
		// 打印参数个数和每个参数的值
		std::cout << "Number of command-line arguments: " << argc << std::endl;
		for (int i = 0; i < argc; ++i) {
			std::cout << "Argument " << i << ": " << argv[i] << std::endl;
			std::string filename = std::string(argv[i]);
			std::cout << "process name\t" << filename << std::endl;
		}
		runTotalTasks(argv[1]);

		//std::cout << "process name\t" << filename << std::endl;
		//runTask();
		//runTotalTasks(filename);


	}


}
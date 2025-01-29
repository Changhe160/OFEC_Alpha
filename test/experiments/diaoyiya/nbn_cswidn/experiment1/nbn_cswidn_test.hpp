
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

//#include "../instance/problem/combination/travelling_salesman/travelling_salesman.h"
#include "interface.h"
//#include "../utility/function/custom_function.h"
#include "../utility/nbn_visualization/visualization/tree_graph_simple.h"
#include "../core/algorithm/population.h"
//#include "../utility/nondominated_sorting/filter_sort.h"
#include "../instance/algorithm/visualize/sampling/instance/tsp/sampling_eax_tsp.h"
#include "../instance/algorithm/visualize/sampling/sampling_multiThread.h"
#include "../instance/problem/combination/travelling_salesman/travelling_salesman.h"
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
#include "../instance/problem/combination/travelling_salesman/tsp_offline_data/travelling_salesman_offline_data.h"


#include "../instance/algorithm/combination/LKH_origin/INCLUDE/LKH.h"
#include <iostream>
#include <iomanip>
#include <ctime>
#include <sstream>

#include "../utility/idee/idee_trait.h"
#include "../utility/nbn_visualization/instance/travelling_salemans_problem/nbn_modify_tsp.h"
#include "../utility/nbn_visualization/calculate_algorithm/nbn_iteration_multithread.h"
#include "../utility/nbn_visualization/nbn_calculator/nearest_better_calculator.h"
#include "../utility/nbn_visualization/nbn_fla/nbn_fla.h"
#include "../utility/nbn_visualization/nbn_fla/nbn_fla_utility.h"


void genenrateTSPenv(
	const std::string& readDir,
	const std::string& tspfilename,
	std::shared_ptr<ofec::Environment>& env) {
	using namespace ofec;
	ofec::ParameterMap params;
	//params["problem name"] = std::string("TSP");
	params["dataFile1"] = tspfilename;
	params["dataDirectory"] = readDir;

	//params["algorithm name"] = std::string("EAX-TSP-sampling");
	std::string proname = "TSP";

	env.reset(Environment::create());
	env->recordInputParameters();
	env->initialize();
	env->setProblem(ofec::Factory<ofec::Problem>::produce(proname));
	env->problem()->inputParameters().input(params);
	env->problem()->recordInputParameters();
	env->initializeProblem(0.5);
}


void runEax(const std::string& readDir,
	const std::string& tspfilename,
	std::vector<std::vector<std::vector<std::shared_ptr<ofec::SolutionBase>>>>& sols, int numRun = 30) {
	using namespace ofec;
	using namespace std;
	using namespace chrono;


	std::string proname = "TSP";
	std::string algName = "EAX-TSP-sampling";

	ofec::ParameterMap params;
	//params["problem name"] = std::string("TSP");
	params["dataFile1"] = tspfilename;
	params["dataDirectory"] = readDir;

	//params["algorithm name"] = std::string("EAX-TSP-sampling");


	//solbases.clear();
	std::vector<std::shared_ptr<ofec::SolutionBase>> tmp;
	std::vector<std::shared_ptr<ofec::SolutionInfo>> solInfos;
	SamplingMutithread::runAlgMultiTask(tmp, solInfos, 0.5, 0.5, proname, params, algName, params, numRun);
	sols.clear();
	sols.resize(numRun);
	for (int idx(0); idx < tmp.size(); ++idx) {
		auto& cursol = tmp[idx];
		auto& curinfo = solInfos[idx];
		if (sols[curinfo->m_runId].size() <= curinfo->m_iter) {
			sols[curinfo->m_runId].resize(curinfo->m_iter + 1);
		}
		if (sols[curinfo->m_runId][curinfo->m_iter].size() <= curinfo->m_indiId) {
			sols[curinfo->m_runId][curinfo->m_iter].resize(curinfo->m_indiId + 1);
		}
		sols[curinfo->m_runId][curinfo->m_iter][curinfo->m_indiId] = cursol;
	}




	//sols = ofec::SamplingMutithread::runAlgMultiTask(0.5, 0.5, proname, params, algName, params, numRun);

}


struct LKH_RunInfo {
	std::vector<unsigned> m_seeds;
	std::vector<std::vector<std::vector<std::vector<int>>>> m_sols;
	std::vector<double> m_costs;
	int m_curId = 0;
	std::mutex m_mutex;


	void resetSize(int numRun) {
		m_seeds.resize(numRun);
		m_sols.resize(numRun);
	}

};


void runLKH(const std::string& readDir,
	const std::string& tspfilename,
	LKH_RunInfo& curInfo) {

	int curRunId = 0;
	while (true) {
		{
			std::unique_lock lock(curInfo.m_mutex);
			if (curInfo.m_curId == curInfo.m_seeds.size())return;
			curRunId = curInfo.m_curId++;
		}


		auto& Seed = curInfo.m_seeds[curRunId];
		auto& totalSols = curInfo.m_sols[curRunId];
		LKH::LKHAlg lkh_alg;

		lkh_alg.SRandom(Seed);
		std::vector<double> costs;
		lkh_alg.run(readDir, tspfilename, totalSols);

		//bool correctRecord = true;


		//for (auto& it : curInfo.m_costs) {
		//	if (lkh_alg.BestCost < it) {
		//		correctRecord = false;
		//	}
		//}

		//if (!correctRecord) {
		//	std::cout << "error at" << tspfilename << std::endl;
		//}

		for (auto& it : totalSols) {
			for (auto& it2 : it) {
				for (auto& it3 : it2) {
					--it3;
				}
			}
		}
	}




}


void runLKHmultiThread(
	const std::string& readDir,
	const std::string& tspfilename,
	LKH_RunInfo& totalInfo) {
	using namespace ofec;
	int numRuns(30);
	//numRuns = 1;
	totalInfo.resetSize(numRuns);

	unsigned initSeed = 1;
	unsigned SeedStep = 1e4;

	ofec::Random rnd(0.5);
	for (auto& it : totalInfo.m_seeds) {
		it = initSeed;
		initSeed += SeedStep;
		it = rnd.uniform.nextNonStd<unsigned>(1, std::numeric_limits<unsigned>::max());
	}

	int num_task = std::thread::hardware_concurrency();
	std::vector<std::thread> thrds;
	for (size_t i = 0; i < num_task; ++i) {
		thrds.push_back(std::thread(
			runLKH,
			std::cref(readDir), std::cref(tspfilename), std::ref(totalInfo)));
	}
	for (auto& thrd : thrds)
		thrd.join();


}


void udpateNBNinfoIteration(
	ofec::nbn::NBNinfo& m_nbnInfo,
	int solSize,
	ofec::Environment* env, ofec::Random* rnd) {


	std::vector<bool> updateIds;
	int from(m_nbnInfo.m_belong.size() - solSize);
	int to = m_nbnInfo.m_belong.size();
	std::vector<int> sortedIds(m_nbnInfo.m_belong.size());
	for (int idx(0); idx < sortedIds.size(); ++idx) {
		sortedIds[idx] = idx;
	}
	std::sort(sortedIds.begin(), sortedIds.end(), [&](int a, int b) {
		return m_nbnInfo.m_vFitness[a] > m_nbnInfo.m_vFitness[b];
		});


	sortedIds.resize(solSize);


	auto oldDis = m_nbnInfo.m_dis2parent;

	ofec::nbn::updateNearsetBetterSolutions(
		sortedIds, m_nbnInfo.m_solbases, m_nbnInfo.m_belong, m_nbnInfo.m_dis2parent, m_nbnInfo.m_vFitness, env, rnd);
	//	m_nbnInfo, updateIds, from, to, sortedIds, env, rnd);env, rnd


	/*
	const std::vector<int>& solIds,
			const std::vector<ofec::SolutionBase*>& sols,
			std::vector<int>& belong,
			std::vector<double>& dis2parent,
			const std::vector<double>& fitness,
			ofec::Environment* env,
			ofec::Random* rnd*/
	double improveAccuracy(0);

	double maxImprove(0);

	for (int idx(0); idx < m_nbnInfo.m_dis2parent.size(); ++idx) {
		if (m_nbnInfo.m_dis2parent[idx] < oldDis[idx]) {
			++improveAccuracy;
			maxImprove = std::max(maxImprove, oldDis[idx] - m_nbnInfo.m_dis2parent[idx]);
		}

		//	if (updateIds[idx])
	}
	std::cout << "improveAccuracy\t" << improveAccuracy << "\tMax Imrprove Dis\t" << maxImprove << std::endl;


}
#include "../utility/function/custom_function.h"
void calculateNBNtsp(
	ofec::nbn::NBNinfo& nbnInfo,
	const std::string& saveDir,
	const std::string& saveDir2,
	const std::string& filename,
	ofec::Environment* env,
	ofec::Random* rnd
) {

	std::string proname = "TSP";
	auto pro = env->problem();
	//auto rnd = std::make_shared<ofec::Random>(0.5);
	auto tsp_pro = dynamic_cast<ofec::TravellingSalesman*>(pro);
	nbnInfo.calculateNBNhnswEqualBetterRandom(env, rnd);

	//udpateNBNinfoIteration(nbnInfo,
	//	1e3, env, rnd);

	nbnInfo.outputNBNinfo(saveDir, filename);
	nbnInfo.outputVecSolutions<int>(saveDir, filename, env);


	{


		nbnInfo.m_vFitness = UTILITY::valuesToSortedValues(nbnInfo.m_vFitness);

		//	auto filename = tspname + "_randomSamples" + "_neighborK_" + std::to_string(neighborK);
		std::vector<int> solIds(nbnInfo.m_belong.size());
		for (int idx(0); idx < solIds.size(); ++idx) {
			solIds[idx] = idx;
		}
		std::vector<ofec::TreeGraphSimple::NBNdata> nbn_data;
		ofec::transferNBNData(nbn_data, solIds, nbnInfo.m_belong, nbnInfo.m_dis2parent, nbnInfo.m_vFitness);

		ofec::TreeGraphSimple nbn_graph;
		nbn_graph.setNBNdata(nbn_data);
		//nbn_graph.modifyBestSols(rnd.get(), env->problem(), nbnInfo.solbases);
		nbn_graph.calNetwork(tsp_pro->numberVariables());


		std::string savepath = saveDir2 + filename + "_network.txt";
		std::ofstream out(savepath);
		nbn_graph.outputNBNnetwork(out);
		out.close();
		ouputNBNdata(saveDir2 + filename + "_nbn.txt", nbn_graph.get_nbn_data());
	}



}



void calculateNBNtest(
	ofec::nbn::NBNinfo& nbnInfo,
	const std::string& saveDir,
	const std::string& saveDir2,
	const std::string& filename,
	ofec::Random* rnd
) {

	//std::string proname = "TSP";
	//auto pro = env->problem();
	//auto rnd = std::make_shared<ofec::Random>(0.5);
	//auto tsp_pro = dynamic_cast<ofec::TravellingSalesman*>(pro);
	//nbnInfo.calculateNBNhnswEqualBetterRandom(env, rnd);

	//udpateNBNinfoIteration(nbnInfo,
	//	1e3, env, rnd);


	nbnInfo.inputNBNinfo(saveDir, filename);

	//nbnInfo.outputNBNinfo(saveDir, filename);
	//nbnInfo.outputVecSolutions<int>(saveDir, filename, env);

	{


		nbnInfo.m_vFitness = UTILITY::valuesToSortedValues(nbnInfo.m_vFitness);

		//	auto filename = tspname + "_randomSamples" + "_neighborK_" + std::to_string(neighborK);
		std::vector<int> solIds(nbnInfo.m_belong.size());
		for (int idx(0); idx < solIds.size(); ++idx) {
			solIds[idx] = idx;
		}
		std::vector<ofec::TreeGraphSimple::NBNdata> nbn_data;
		ofec::transferNBNData(nbn_data, solIds, nbnInfo.m_belong, nbnInfo.m_dis2parent, nbnInfo.m_vFitness);

		ofec::TreeGraphSimple nbn_graph;
		nbn_graph.setNBNdata(nbn_data);
		//nbn_graph.modifyBestSols(rnd.get(), env->problem(), nbnInfo.solbases);
		nbn_graph.calNetwork();


		std::string savepath = saveDir2 + filename + "_network.txt";
		std::ofstream out(savepath);
		nbn_graph.outputNBNnetwork(out);
		out.close();
		//	ouputNBNdata(saveDir2 + filename + "_nbn.txt", nbn_graph.get_nbn_data());
	}



}


struct TaskInfo {
	std::ofstream m_out;
	std::set<int> m_lkh_suc;
	std::set<int> m_eax_suc;

};

void runEAX_LKH(const std::string& readDir,
	const std::string& saveDir,
	const std::string& tspfilename, TaskInfo& globalInfo) {
	using namespace ofec;
	using namespace std;
	using namespace chrono;
	auto tspname = tspfilename.substr(0, tspfilename.find_first_of("."));
	std::cout << "calculating filename\t" << tspname << std::endl;
	std::shared_ptr<Environment> env;
	genenrateTSPenv(readDir, tspfilename, env);

	std::string proname = "TSP";
	auto pro = env->problem();
	auto tsp_pro = dynamic_cast<ofec::TravellingSalesman*>(pro);
	auto rnd = std::make_shared<ofec::Random>(0.5);


	std::vector<std::vector<std::vector<std::shared_ptr<ofec::SolutionBase>>>> sols;
	runEax(readDir, tspfilename, sols);

	std::vector<std::vector<std::vector<int>>>lkh_ids;
	std::vector<std::vector<std::vector<int>>> eax_ids;

	std::vector<SolutionBase*> totalSols;
	//	int solId(0);
	eax_ids.resize(sols.size());
	for (int idx(0); idx < eax_ids.size(); ++idx) {
		eax_ids[idx].resize(sols[idx].size());
		for (int idy(0); idy < eax_ids[idx].size(); ++idy) {
			eax_ids[idx][idy].resize(sols[idx][idy].size());
			for (int idz(0); idz < eax_ids[idx][idy].size(); ++idz) {
				eax_ids[idx][idy][idz] = totalSols.size();
				totalSols.push_back(sols[idx][idy][idz].get());

			}
		}
	}


	std::vector<double> lkh_searchDis;



	for (auto& it : eax_ids) {

		for (int idx(1); idx < it.size(); ++idx) {
			for (int idy(0); idy < it[idx].size(); ++idy) {
				double dis = totalSols[it[idx - 1][idy]]->variableDistance(*totalSols[it[idx][idy]], env.get());
				if (dis > 0)
					lkh_searchDis.push_back(dis);
			}
		}

		//for (auto& it2 : it) {
		//	for (int idx(0); idx + 1 < it2.size(); ++idx) {
		//		double dis = totalSols[it2[idx]]->variableDistance(*totalSols[it2[idx + 1]], env.get());
		//		lkh_searchDis.push_back(dis);
		//	}
		//}
	}




	std::vector<std::string> heads = { "max","min", "mean", "std" };
	std::vector<double> values;

	double curValue, curValue2;
	ofec::calMax(lkh_searchDis, curValue);
	values.push_back(curValue);
	ofec::calMean(lkh_searchDis, curValue);
	values.push_back(curValue);
	ofec::calMeanAndStd(lkh_searchDis, curValue, curValue2);
	values.push_back(curValue);
	values.push_back(curValue2);

	std::ofstream out(saveDir + tspname + "_eax_searchDis.txt");
	for (auto& it : heads) {
		out << it << "\t";
	}
	out << std::endl;
	for (auto& it : values) {
		out << it << "\t";
	}
	out << std::endl;
	out.close();


	return;




}


void runTask() {
	using namespace ofec;
	using namespace std;
	ofec::g_working_directory = "//172.24.207.203/share/2018/diaoyiya/ofec-data";
	ofec::g_working_directory = "//172.29.41.69/share/2018/diaoyiya/ofec-data";
	ofec::g_working_directory = "//172.29.204.109/share/Student/2018/YiyaDiao/code_total/data";
	ofec::g_working_directory = "/home/lab408/share/2018/diaoyiya/ofec-data";
	//ofec::g_working_directory = "/mnt/Data/Student/2018/YiyaDiao/code_total/data";
	ofec::g_working_directory = "//172.24.24.151/e/DiaoYiya/code/data/ofec-data";

	//ofec::g_working_directory = "E:/DiaoYiya/code/data/ofec-data/";

	std::string saveDir = ofec::g_working_directory + "/paper_com_experiment_data/tsp_typical_comparison_remote/";
	saveDir = ofec::g_working_directory + "/paper_com_experiment_data/tsp_typical_comparison_exp3/";
	saveDir = ofec::g_working_directory + "/paper_com_experiment_data/totalTsp/nbn_eax_analysis/";

	std::filesystem::create_directories(saveDir);


	std::string dir1 = ofec::g_working_directory + "/instance/problem/combination/travelling_salesman/";
	std::string dir2 = ofec::g_working_directory + "/instance/problem/combination/travelling_salesman_origin/";
	std::string dir3 = ofec::g_working_directory + "/instance/problem/combination/travelling_salesman_origin2/";
	std::vector<std::string> filenames;
	//{ "2202.tsp","1281.tsp" ,"u574.tsp" ,"6717.tsp" , "6702.tsp" , "7310.tsp", "9225.tsp", "5955.tsp", };


	filenames = { "2202.tsp","u574.tsp" , "5955.tsp", };


	//readTspInstance2(ofec::g_working_directory, filenames);


	//readDirFun(dir1, filenames);

	//std::reverse(filenames.begin(), filenames.end());


	std::cout << "run EAX vs LKH run compareResult hnsw equal better" << std::endl;

	//filenames.resize(60);
	//filenames.erase(filenames.begin());
	//	filenames.clear();
	//	filenames = { "2202.tsp" };

		//	std::reverse(filenames.begin(), filenames.end());
	//size_t K = 11;

	//if (K <= filenames.size()) {
	//	filenames.erase(filenames.begin(), std::next(filenames.begin(), K));
	//}

	//std::reverse(filenames.begin(), filenames.end());


	TaskInfo globalInfo;
	globalInfo.m_out.open(saveDir + "tsp_instance2.txt");

	for (auto& it : filenames) {
		if (it == "u574.tsp") {
			runEAX_LKH(dir2, saveDir, it, globalInfo);
		}
		else {
			runEAX_LKH(dir1, saveDir, it, globalInfo);
		}
	}


	globalInfo.m_out.close();



}



void testNBN() {
	auto rnd = std::make_shared<ofec::Random>(0.5);
	ofec::nbn::NBNinfo nbnInfo;
	auto saveDir = "//172.24.34.11/share/2018/diaoyiya/ofec-data/paper_cswidn/cswidn_algorithm_runProcess_network/";

	auto filename = "CSWIDN_SourceIds_0_0_phase_3_algSol_sols";
	calculateNBNtest(
		nbnInfo,
		saveDir,
		saveDir,
		filename,
		rnd.get()
	);


}

namespace ofec {

	void registerParamAbbr() {}
	void customizeFileName() {}
	void run(int argc, char* argv[]) {

		using namespace ofec;
		using namespace std;

		registerInstance();

		//runTask();

		testNBN();
	}


}
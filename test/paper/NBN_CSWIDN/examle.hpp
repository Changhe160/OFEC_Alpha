#ifndef OFEC_CUSTOM_METHOD_HPP
#define OFEC_CUSTOM_METHOD_HPP

#include "../core/global.h"
#include "interface.h"
#include "../instance/problem/continuous/single_objective/global/cec2005/f1_shifted_sphere.h"
#include "../instance/algorithm/continuous/single_objective/global/canonical_de/canonical_de.h"


#include "../instance/problem/realworld/csiwdn/csiwdn.h"
#include "../utility/nbn_visualization//nbn_fla/nbn_info.h"
#include "../utility/hnsw/hnsw_mt/hnsw_model.h"
#include "../utility/nbn_visualization/visualization/tree_graph_simple.h"
#include <filesystem>


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

	nbnInfo.calculateNBNhnsw(env, rnd);



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



void createCSWIDN_environments(std::shared_ptr<ofec::Environment>& env,
	const std::string& processName = "localProcess") {
	using namespace ofec;
	using namespace std;
	ofec::ParameterMap params;

	//std::string algName = "AMP-GL-SaDE-sampling";
	///params["maximum evaluations"] = int(200000);

	std::string proname = "CSIWDN";
	params["net-sensor-source case"] = std::string("Net2/sensor_case1/source/case11.txt");
	params["process name"] = processName;
	int numRun = 30;

	genenratePro(proname, params, env);
}


void testNBN_cwidn() {

	auto savedir = ofec::g_working_directory + "/nbn_cswidn/";


	std::shared_ptr<ofec::Environment> env;
	createCSWIDN_environments(env);
	std::shared_ptr<ofec::Random> rnd(new ofec::Random(0.5));

	std::vector<std::shared_ptr<ofec::SolutionBase>> sols;

	// random sampling with 1e6 solutions

	int numSamples = 1e4;
	sols.resize(numSamples);
	ofec::Real pos = (env->problem()->optimizeMode(0) == ofec::OptimizeMode::kMaximize) ? 1 : -1;

	for (auto& it : sols) {
		it.reset(env->problem()->createSolution());
		it->initialize(env.get(), rnd.get());
		//	it->evaluate(env.get(), false);
		//	it->setFitness(pos * it->objective(0));
		it->setFitness(rnd->uniform.next());
	}

	ofec::nbn::NBNinfo nbnInfo;
	for (auto& it : sols) {
		nbnInfo.m_solbases.push_back(it.get());
	}

	calculateNBN(nbnInfo, savedir, savedir, "nbn_cswidn", env.get(), rnd.get());

}


namespace ofec {
	void run(int argc, char* argv[]) {
		registerInstance();

		// the path to data
		ofec::g_working_directory = "";
		ofec::g_working_directory = "//172.24.242.8/share/Student/2018/YiyaDiao/code_total/data";
		testNBN_cwidn();

		//std::cout << "error" << std::endl;
	}
}

#endif // !OFEC_CUSTOM_METHOD_HPP
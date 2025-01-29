#include "../utility/catch.hpp"
#include "../run/custom_method.h"
#include "../instance/algorithm/combination/sequence/SP/SP_operator.h"
#include "../instance/algorithm/combination/gl/gl_adaptor_com.h"
#include "../instance/algorithm/combination/gl/gl_com.h"
#include "../instance/algorithm/combination/gl/gl_pop_seq.h"
#include "../instance/problem/combination/selection_problem/selection_problem.h"
#include "../instance/algorithm/template/combination/multi_population/distance_calculator.h"
#include "../instance/algorithm/combination/gl/mp/gl_mp_pop.h"


#include "../instance/algorithm/combination/uncertianty_algorithm/multi_pop/mp_uncertianty_seq.h"
#include "../instance/algorithm/combination/uncertianty_algorithm/multi_pop/pop_mp_main_uncertianty_seq.h"
#include "../instance/algorithm/combination/uncertianty_algorithm/multi_pop/pop_mp_sub_uncertianty_seq.h"

using namespace ofec;
using namespace std;




TEST_CASE("com SP2", "[MP_distance_calculator]") {


	DistanceCalculator<SP_Interpreter> a;
}

TEST_CASE("com SP", "[SP]") {

	Solution s1;
	ParameterMap params;
	params["problem name"] = std::string("ComOP_DSP");
	params["numDim"] = "5";

	Random *rnd = ADD_RND(0.5);
	int id_param = std::make_shared<const ParameterMap>(params);
	Problem *pro = Problem::generateByFactory(id_param, 0.1);
	pro->initialize();
	using sol_type = ofec::sp::SelectionProblem::solution_type;
	sol_type cur_sol(pro->numberObjectives(), pro->numberConstraints(), pro->numberVariables());
	int testnum(1);

	std::vector<Real> obj_val(0);
	Real mean_val(0), distri_val(0);
	int size(1000);
	while (testnum--) {
		cur_sol.initialize(pro, rnd);

		cout << "id\t" << testnum << "sol:\t";
		for (auto& it : cur_sol.variable().vect()) {
			cout << it << "\t";
		}

		for (int t(0); t < 2000; ++t)
		{

			cur_sol.evaluate(pro, -1, false);
			//cout << "obj:\t" << cur_sol.objective()[0] << endl;
			obj_val.clear();
			for (int idx(0); idx < size; ++idx) {
				cur_sol.evaluate(pro, -1, false);
				obj_val.push_back(cur_sol.objective()[0]);

			}
			mean_val = 0;
			distri_val = 0;
			for (auto& it : obj_val) mean_val += it;
			mean_val /= obj_val.size();
			for (auto& it : obj_val) distri_val += (it - mean_val) * (it - mean_val);
			distri_val = sqrt(distri_val / obj_val.size());
			cout << "mean obj:\t" << cur_sol.objective()[0] << "\t distri obj\t" << distri_val << endl;
			GET_DYN_SP(pro)->change();
		}
	}



	std::vector<sol_type> samples;
	GET_DYN_SP(pro)->generate_level_samples(samples, pro);
	for (auto& it : samples) {
		it.evaluate(pro, -1, false);
	}
	//GET_DYN_SP(pro)->(samples, caller::Problem);
	std::vector<Real> mean(samples.size(), 0);
	std::vector<Real> var(samples.size(), 0);

	const int sample_num(1000);

	std::vector<Real> sample_val(sample_num);

	for (int indi(0); indi < samples.size(); ++indi) {
		for (int sam_id(0); sam_id < sample_num; ++sam_id) {
			samples[indi].evaluate(pro, -1, false);
			sample_val[sam_id] = samples[indi].objective()[0];
		}
		for (const auto& sample_iter : sample_val) {
			mean[indi] += sample_iter;
		}
		mean[indi] /= sample_val.size();
		for (int sam_id(0); sam_id < sample_num; ++sam_id) {
			var[indi] += (sample_val[sam_id] - mean[indi]) * ((sample_val[sam_id] - mean[indi]));
		}
		var[indi] /= sample_val.size();
		var[indi] = sqrt(var[indi]);
	}

	std::vector<Real> dis(samples.size(), 0);
	for (int indi(0); indi < samples.size(); ++indi) {
		for (auto& others : samples) {
			dis[indi] += samples[indi].variableDistance(others, pro);
		}
	}
	//s1.evaluate()
}


using MP_GLU_SP = MultiPopUncertiantySeq<
	EvaluationStrategyBase,
	DistanceCalculatorBase,
	SelectionProblemOperator>;

using OpType = typename SelectionProblemOperator;
using SolutionType = typename OpType::SolutionType;
using InterpreterType = typename OpType::InterpreterType;
using MainPopType = typename PopulationMPMainUncertiantySeqEnsemble
<DistanceCalculatorBase, SelectionProblemOperator>;
using SubPopType = typename PopulationMPSubUncertiantySeqEnsemble
<DistanceCalculatorBase, SelectionProblemOperator>;


void testCopy(SubPopType& pop) {
	
}


TEST_CASE("com gl pop seq", "[com][GL_dynamic_pop][uncertianty_alg]") {


//	MainPopType m_main_pop;
//	MultiPopulation<SubPopType> m_sub_pops;
//	std::shared_ptr<EvaluationStrategyBase<SolutionType>> m_eval_strategy;
//
//	MP_GLU_SP m_temp;
//	// for test 
//	
//	MultiPopulation<SubPopType> m_sub_pops2;
//	
//
//	std::unique_ptr<SubPopType> a;
//	SubPopType b;
//	SubPopType c;
////	c.testCopy(b);
	
	//b.judgeSearchInside(c);
	//b.mergePopByFitnessDis(c);
	//a.mergePopByFitnessDis(b);


	ParameterMap params;
	params["problem name"] = std::string("ComOP_DSP");
	params["numDim"] = "5";
	params["algorithm name"] = std::string("MP_GLU_SP");
	params["changeFre"] = int(20000);
	params["population size"] = int(40);
	params["flag of point correlation"] = true;
	params["flagChange"] = true;
	params["flagChangeExecution"] = false;
	params["flagNoise"] = false;
	params["flagNoiseObjectives"] = false;
	params["flagNoiseVariables"] = false;
	params["flagNoiseEnvironments"] = false;
	params["maximum evaluations"] = int(20000000);
	params["number of runs"] = 11;
	//v.count("sample frequency")
	params["sample frequency"] = int(1000);
	params["noisy ratio"] = ofec::Real(0);


	params["combinatorial fitness type"] = static_cast<int>
		(ofec::SelectionProblemOperator::EFitnessCalType::Objective);
	params["combinatorial memory type"] = static_cast<int>
		(ofec::SelectionProblemOperator::EMemoryType::Change);

	

	int id_param = std::make_shared<const ParameterMap>(params);
	Problem *pro = Problem::generateByFactory(id_param, 0.1);
	int id_rcr = ADD_RCR(id_param);

	pro->initialize();
	Algorithm *alg = Algorithm::generateByFactory(id_param, pro, 0.01, id_rcr);
	pro->setIdAlg(alg);
	//GET_DYN_SP(pro)->testProblemCharacer();
	alg.initialize();
	//	GET_DYN_SP(pro)->testProblemCharacer();
	alg.run();

	DEL_ALG(alg);
	DEL_PRO(pro);
	DEL_PARAM(id_param);
}




TEST_CASE("com gl pop", "[com][GL_dynamic_pop]") {


	ParameterMap params;
	params["problem name"] = std::string("ComOP_DSP");
	params["numDim"] = "5";
	params["algorithm name"] = std::string("GL_SP");
	params["changeFre"] = int(20000);
	params["population size"] = int(40);
	params["flag of point correlation"] = true;
	params["flagChange"] = false;
	params["flagChangeExecution"] = false;
	params["flagNoise"] = false;
	params["flagNoiseObjectives"] = false;
	params["flagNoiseVariables"] = false;
	params["flagNoiseEnvironments"] = false;
	params["maximum evaluations"] = int(20000000);
	params["number of runs"] = 11;
	//v.count("sample frequency")
	params["sample frequency"] = int(1000);
	params["noisy ratio"] = ofec::Real(0);


	params["combinatorial fitness type"] = static_cast<int>
		(ofec::SelectionProblemOperator::EFitnessCalType::Objective);
	params["combinatorial memory type"] = static_cast<int>
		(ofec::SelectionProblemOperator::EMemoryType::Change);



	int id_param = std::make_shared<const ParameterMap>(params);
	Problem *pro = Problem::generateByFactory(id_param, 0.1);
	int id_rcr = ADD_RCR(id_param);

	pro->initialize();
	Algorithm *alg = Algorithm::generateByFactory(id_param, pro, 0.01, id_rcr);
	pro->setIdAlg(alg);
	//GET_DYN_SP(pro)->testProblemCharacer();


	alg.initialize();
	//	GET_DYN_SP(pro)->testProblemCharacer();
	alg.run();

	DEL_ALG(alg);
	DEL_PRO(pro);
	DEL_PARAM(id_param);
}


TEST_CASE("com gl dynamic seq", "[com][GL_dynamic]") {



	ParameterMap params;
	params["problem name"] = std::string("ComOP_DSP");
	params["numDim"] = "5";
	params["algorithm name"] = std::string("GL_SP");

	params["population size"] = int(60);
	params["flag of point correlation"] = true;
	params["flagChange"] = true;
	params["flagChangeExecution"] = true;
	params["flagNoise"] = true;
	params["flagNoiseObjectives"] = true;
	params["flagNoiseVariables"] = false;
	params["flagNoiseEnvironments"] = false;

	params["changeFre"] = int(1000);
	params["maximum evaluations"] = int(40000 * 20);
	params["combinatorial fitness type"] = static_cast<int>
		(ofec::SelectionProblemOperator::EFitnessCalType::Objective);
	params["combinatorial memory type"] = static_cast<int>
		(ofec::SelectionProblemOperator::EMemoryType::Change);
	/*
	*
	* 	getTypeVal<int, EFitnessCalType>(par, "combinatorial fitness type", m_fitness_type,EFitnessCalType::Objective);
	getTypeVal<int, EMemoryType>(par, "combinatorial memory type", m_memory_type, EMemoryType::Change);

	*/
	//params["maximum evaluations"] = int(100000);
	//params["number of variables"] = int();
	int id_param = std::make_shared<const ParameterMap>(params);
	Problem *pro = Problem::generateByFactory(id_param, 0.1);
	int id_rcr = ADD_RCR(id_param);

	pro->initialize();
	Algorithm *alg = Algorithm::generateByFactory(id_param, pro, 0.01, id_rcr);
	pro->setIdAlg(alg);



	alg.initialize();
	//	GET_DYN_SP(pro)->testProblemCharacer();
	alg.run();

	DEL_ALG(alg);
	DEL_PRO(pro);
	DEL_PARAM(id_param);

}





TEST_CASE("com gl static", "[com][GL_static]") {



	ParameterMap params;
	params["problem name"] = std::string("ComOP_DSP");
	params["numDim"] = "5";
	params["algorithm name"] = std::string("GL_SP");
	params["changeFre"] = int(40000);
	params["population size"] = int(40);
	params["flag of point correlation"] = false;
	params["flagChange"] = false;
	params["flagChangeExecution"] = false;
	params["flagNoise"] = false;
	params["flagNoiseObjectives"] = false;
	params["flagNoiseVariables"] = false;
	params["flagNoiseEnvironments"] = false;

	params["combinatorial fitness type"] = static_cast<int>
		(ofec::SelectionProblemOperator::EFitnessCalType::Objective);
	params["combinatorial memory type"] = static_cast<int>
		(ofec::SelectionProblemOperator::EMemoryType::Change);


	/*
	*
	* 	getTypeVal<int, EFitnessCalType>(par, "combinatorial fitness type", m_fitness_type,EFitnessCalType::Objective);
	getTypeVal<int, EMemoryType>(par, "combinatorial memory type", m_memory_type, EMemoryType::Change);

	*/



	//params["maximum evaluations"] = int(100000);
	//params["number of variables"] = int();
	int id_param = std::make_shared<const ParameterMap>(params);
	Problem *pro = Problem::generateByFactory(id_param, 0.1);
	pro->initialize();
	Algorithm *alg = Algorithm::generateByFactory(id_param, pro, 0.01, -1);
	alg.initialize();


	//	GET_DYN_SP(pro)->testProblemCharacer();
	alg.run();

	DEL_ALG(alg);
	DEL_PRO(pro);
	DEL_PARAM(id_param);

}




TEST_CASE("com alg", "[com][adaptor]") {



	ParameterMap params;
	params["problem name"] = std::string("ComOP_DSP");
	params["numDim"] = "5";
	params["algorithm name"] = std::string("GL_SP");
	//params["changeFre"] = int(1e8);
	params["population size"] = int(40);




	// params["maximum evaluations"] = int(100000);
	 //	params["number of variables"] = int();
	int id_param = std::make_shared<const ParameterMap>(params);
	Problem *pro = Problem::generateByFactory(id_param, 0.1);
	pro->initialize();
	Algorithm *alg = Algorithm::generateByFactory(id_param, 0.1, 0.01, -1);
	alg.initialize();



	alg.run();



	DEL_ALG(alg);
	DEL_PRO(pro);
	DEL_PARAM(id_param);

}


//#include <torch/torch.h>
//
//TEST_CASE("prediction", "[lstm][calbest]")
//{
//
//	torch::Tensor m_input;
//
//
//}
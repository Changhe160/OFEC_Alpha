#include "../utility/catch.hpp"
#include "../core/algorithm/population.h"
#include "../core/algorithm/Solution.h"
#include "../run/user_initialization.h"
#include "../utility/functional.h"
#include "../utility/clustering/hslh.h"
#include "../utility/clustering/apc.h"
#include "../utility/clustering/nbc.h"
#include "../utility/clustering/apd.h"
#include "../instance/algorithm/HLC/HLC.h"
#include "../instance/algorithm/DE/Canonical_DE/Canonical_DE.h"
#include "../instance/algorithm/GA/SPAE_GA/SPAE_GA.h"
#include "../instance/algorithm/GA/p_fGA/p_fGA.h"
#include "../instance/algorithm/GA/EA_KDTree/EA_KDTree.h"
#include "../instance/problem/continuous/global/BBOB/BBOB.h"
#include "../instance/problem/continuous/global/CEC2005/F23_non_continuous_rotated_hybrid.h"
#include "../instance/problem/continuous/multi_modal/CEC2013/F1_F8.h"
#include "../instance/problem/continuous/multi_modal/CEC2013/F9_composition1.h"

using namespace ofec;

void plot_problem(const std::string& pro_name) {
	std::cout << "plot problem" << std::endl;
	size_t divNum = 100;
	auto& rangeX = CONTINUOUS_CAST->range();
	{
		std::ofstream outfile("E:/Document/MyPaper/DSAPE/python/data/" + pro_name + "_x1.csv");
		std::stringstream out;
		for (size_t i = 0; i < divNum; ++i) {
			out << rangeX[0].limit.first;
			for (size_t j = 1; j < divNum; ++j) {
				out << "," << rangeX[0].limit.first + j * (rangeX[0].limit.second - rangeX[0].limit.first) / divNum;
			}
			out << "\n";
		}
		outfile << out.str();
		outfile.close();
	}
	{
		std::ofstream outfile("E:/Document/MyPaper/DSAPE/python/data/" + pro_name + "_x2.csv");
		std::stringstream out;
		for (size_t i = 0; i < divNum; ++i) {
			out << rangeX[1].limit.first + i * (rangeX[1].limit.second - rangeX[1].limit.first) / divNum;
			for (size_t j = 1; j < divNum; ++j) {
				out << "," << rangeX[1].limit.first + i * (rangeX[1].limit.second - rangeX[1].limit.first) / divNum;
			}
			out << "\n";
		}
		outfile << out.str();
		outfile.close();
	}
	{
		solution<> temp_sol(1, 0, 2);
		real positive = CONTINUOUS_CAST->opt_mode(0) == optimization_mode::Minimization ? -1 : 1;
		std::ofstream outfile("E:/Document/MyPaper/DSAPE/python/data/" + pro_name + "_f.csv");
		std::stringstream out;
		for (size_t i = 0; i < divNum; ++i) {
			temp_sol.variable()[1] = rangeX[1].limit.first + i * (rangeX[1].limit.second - rangeX[1].limit.first) / divNum;
			temp_sol.variable()[0] = rangeX[0].limit.first;
			temp_sol.evaluate(false);
			out << positive * temp_sol.objective(0);
			for (size_t j = 1; j < divNum; ++j) {
				temp_sol.variable()[0] = rangeX[0].limit.first + j * (rangeX[0].limit.second - rangeX[0].limit.first) / divNum;
				temp_sol.evaluate(false);
				out << "," << positive * temp_sol.objective(0);
			}
			out << "\n";
		}
		outfile << out.str();
		outfile.close();
	}
}

void plot_SPAE(const std::string& pro_name, std::vector<Solution<>> &pop, HLC& hlc) {
	std::cout << "plot SPAE" << std::endl;
	for (auto& ind : pop)
		hlc.input_sample(ind);
	hlc.clustering();
	std::ofstream outfile("E:/Document/MyPaper/DSAPE/python/data/" + pro_name + "_SPAE.csv");
	std::stringstream out;
	out << "popID,x1,x2,f\n";
	size_t idx_clu;
	std::map<size_t, size_t> idx_clu_to_k;
	size_t k = 0;
	for (auto& ind : pop) {
		idx_clu = hlc.affiliated_cluster(hlc.affiliated_subspace(ind.variable().vect()));
		if (idx_clu_to_k.count(idx_clu) == 0)
			idx_clu_to_k.emplace(idx_clu, k++);
		out << idx_clu_to_k[idx_clu] << "," << ind.variable()[0] << "," << ind.variable()[1] << "," << ind.objective(0) << "\n";
	}
	outfile << out.str();
	outfile.close();
}

void plot_HSLH(const std::string& pro_name, std::vector<Solution<>>& pop) {
	std::cout << "plot HSLH" << std::endl;
	HSLH<solution<>> hslh;
	std::vector<solution<>*> ptrs_pop;
	for (auto& ind : pop)
		ptrs_pop.push_back(&ind);
	hslh.update_data(ptrs_pop);
	hslh.clustering(5);
	std::ofstream outfile("E:/Document/MyPaper/DSAPE/python/data/" + pro_name + "_HSLH.csv");
	std::stringstream out;
	out << "popID,x1,x2,f\n";
	for (size_t k = 0; k < hslh.size(); ++k) {
		for (auto iter = hslh[k].begin(); iter != hslh[k].end(); ++iter) {
			out << k << "," << iter->second->variable()[0] << "," << iter->second->variable()[1] << "," << iter->second->objective(0) << "\n";
		}
	}
	outfile << out.str();
	outfile.close();
}

void plot_APC(const std::string& pro_name, std::vector<Solution<>>& pop) {
	std::cout << "plot APC" << std::endl;
	APC<Solution<>> apc(0.9, 100, 30);
	apc.update_data(pop);
	apc.clustering();
	auto& clusters = apc.clusters();
	std::ofstream outfile("E:/Document/MyPaper/DSAPE/python/data/" + pro_name + "_APC.csv");
	std::stringstream out;
	out << "popID,x1,x2,f\n";
	for (size_t k = 0; k < clusters.size(); ++k) {
		for (size_t idx_ind : clusters[k]) {
			out << k << "," << pop[idx_ind].variable()[0] << "," << pop[idx_ind].variable()[1] << "," << pop[idx_ind].objective(0) << "\n";
		}
	}
	outfile << out.str();
	outfile.close();
}

void plot_NBC(const std::string& pro_name, std::vector<Solution<>>& pop) {
	std::cout << "plot NBC" << std::endl;
	NBC<Solution<>> nbc;
	nbc.update_data(pop);
	nbc.clustering();
	auto& clusters = nbc.get_clusters();
	std::ofstream outfile("E:/Document/MyPaper/DSAPE/python/data/" + pro_name + "_NBC.csv");
	std::stringstream out;
	out << "popID,x1,x2,f\n";
	for (size_t k = 0; k < clusters.size(); ++k) {
		for (size_t idx_ind : clusters[k]) {
			out << k << "," << pop[idx_ind].variable()[0] << "," << pop[idx_ind].variable()[1] << "," << pop[idx_ind].objective(0) << "\n";
		}
	}
	outfile << out.str();
	outfile.close();
}

void plot_APD(const std::string& pro_name, std::vector<Solution<>>& pop) {
	std::cout << "plot APD" << std::endl;

	APD<Solution<>> apd;
	apd.update_data(pop);
	apd.clustering();
	auto& centers = apd.get_peaks();

	std::ofstream outfile("E:/Document/MyPaper/DSAPE/python/data/" + pro_name + "_APD.csv");
	std::stringstream out;
	out << "popID,x1,x2,f\n";
	size_t k = 0;
	for (auto ptr_sol : centers) {
		out << k++ << "," << ptr_sol->variable()[0] << "," << ptr_sol->variable()[1] << "," << ptr_sol->objective(0) << "\n";
	}
	outfile << out.str();
	outfile.close();
}

void plot_SPAE_GA(const std::string& pro_name) {
	std::cout << "plot SPAE-GA" << std::endl;
	SPAE_GA alg("HLC-GA", 20, 250);
	alg.initialize();
	alg.run();
	auto& hlc = alg.get_hlc();
	std::ofstream outfile("E:/Document/MyPaper/DSAPE/python/data/" + pro_name + "_SPAE_GA.csv");
	std::stringstream out;
	out << "x,y,w,h,c\n";
	size_t idx_clu = 0;
	for (size_t i = 0; i < hlc.num_basins(); ++i) {
		if (hlc.subspaces_basin(i).empty()) continue;
		for (auto idx_reg : hlc.subspaces_basin(i)) {
			auto& box = hlc.subspace(idx_reg);
			out << box[0].first << "," << box[1].first << "," << box[0].second - box[0].first << "," << box[1].second - box[1].first << "," << idx_clu << "\n";
		}
		idx_clu++;
	}
	outfile << out.str();
	outfile.close();
}

void plot_p_fGA(const std::string& pro_name) {
	std::cout << "plot p-fGA" << std::endl;
	p_fGA alg("p-fGA", 50);
	alg.initialize();
	alg.run();
	auto& S = alg.get_S();
	auto& attractors = alg.get_attractors();
	std::ofstream outfile("E:/Document/MyPaper/DSAPE/python/data/" + pro_name + "_p_fGA.csv");
	std::stringstream out;
	out << "x,y,w,h,c\n";
	size_t idx_clu = 0;
	for (auto& sol : attractors) {
		out << sol.variable()[0] - S[0]/2 << "," << sol.variable()[1] - S[1] / 2 << "," << S[0] << "," << S[1] << "," << idx_clu << "\n";
		idx_clu++;
	}
	outfile << out.str();
	outfile.close();
}

void plot_EA_KDTree(const std::string& pro_name) {
	std::cout << "plot EA-KDTree" << std::endl;
	EA_KDTree alg("EA-KDTree", 20);
	alg.initialize();
	alg.run();
	auto& attractors = alg.get_attractors();
	std::ofstream outfile("E:/Document/MyPaper/DSAPE/python/data/" + pro_name + "_EA_KDTree.csv");
	std::stringstream out;
	out << "x,y,w,h,c\n";
	size_t idx_clu = 0;
	for (auto& pair : attractors) {
		if (!pair.second) continue;
		out << pair.second->range[0].first << "," 
			<< pair.second->range[1].first << "," 
			<< pair.second->range[0].second - pair.second->range[0].first << "," 
			<< pair.second->range[1].second - pair.second->range[1].first << "," 
			<< idx_clu << "\n";
		idx_clu++;
	}
	outfile << out.str();
	outfile.close();
}

void initialize_pop(std::vector<Solution<>>& pop, HLC& hlc) {
	std::cout << "initialize Population" << std::endl;
	solution<> temp(1, 0, 2);
	std::vector<std::pair<real, real>> boundary(2);
	for (size_t j = 0; j < 2; ++j)
		boundary[j] = CONTINUOUS_CAST->range(j);
	hlc.initialize(boundary);
	for (size_t i = 0; i < hlc.num_subspaces(); ++i) {
		const auto& box = hlc.subspace(i);
		for (size_t k = 0; k < CONTINUOUS_CAST->variable_size(); ++k) {
			for (size_t j = 0; j < CONTINUOUS_CAST->variable_size(); ++j)
				temp.variable()[j] = global::ms_global->m_uniform[caller::Algorithm]->next_non_standard<real>(box[j].first, box[j].second);
			temp.evaluate();
			pop.push_back(temp);
		}
	}
}

void evolve_pop(std::vector<Solution<>>& pop, HLC& hlc) {
	std::cout << "evolve Population" << std::endl;
	DE::population<DE::Solution> DE_pop(pop.size());
	for (size_t i = 0; i < pop.size(); ++i)
		DE_pop[i] = DE::Solution(pop[i]);
	DE_pop.CR() = 0.9;
	DE_pop.F() = 0.5;
	DE_pop.mutation_strategy() = DE::mutation_strategy::nrand_1;
	for (size_t iter = 0; iter < 50; iter++) {
		DE_pop.evolve();
		for (size_t i = 0; i < DE_pop.size(); ++i) {
			if (DE_pop[i].is_improved())
				hlc.input_sample(DE_pop[i]);
			else
				hlc.input_sample(DE_pop[i].trial());
		}
	}
	for (size_t i = 0; i < pop.size(); ++i)
		pop[i] = DE_pop[i];
}

TEST_CASE("merge_sort", "[sort]") {
	std::vector<real> data = { 5,3,4,2,9 };
	std::vector<int> index;
	merge_sort(data, data.size(), index);
	for (size_t i = 0; i < index.size(); i++)
	{
		std::cout << index[i] << " ";
	}
	std::cout << std::endl;

}

TEST_CASE("MMOP_CEC2013_F07", "[MMOP][CEC2013][F07][initial]")
{
	std::cout << "MMOP_CEC2013_F07" << std::endl;
	global::ms_arg["maximum evaluations"] = 200000;
	global::ms_global.reset(new global(1, 1./7, 0.5));
	global::ms_global->m_problem.reset(new MMOP_CEC2013_F07("MMOP_CEC2013_F07", 2));
	global::ms_global->m_problem->initialize();
	//plot_EA_KDTree("MMOP_CEC2013_F07");
	//plot_p_fGA("MMOP_CEC2013_F07");
	plot_SPAE_GA("MMOP_CEC2013_F07");
	//plot_problem("MMOP_CEC2013_F07");
	//std::vector<Solution<>> pop;
	//HLC hlc(2);
	//initialize_pop(pop, hlc);
	//plot_SPAE("MMOP_CEC2013_F07", pop, hlc);
	//plot_HSLH("MMOP_CEC2013_F07", pop);
	//plot_APC("MMOP_CEC2013_F07", pop);
	//plot_NBC("MMOP_CEC2013_F07", pop);
}

TEST_CASE("MMOP_CEC2013_F09", "[MMOP][CEC2013][F09][initial]")
{
	std::cout << "MMOP_CEC2013_F09" << std::endl;
	global::ms_arg["maximum evaluations"] = 200000;
	global::ms_global.reset(new global(1, 1./7, 0.5));
	global::ms_global->m_problem.reset(new MMOP_CEC2013_F09("MMOP_CEC2013_F09", 2));
	global::ms_global->m_problem->initialize();
	//plot_EA_KDTree("MMOP_CEC2013_F09");
	//plot_p_fGA("MMOP_CEC2013_F09");
	plot_SPAE_GA("MMOP_CEC2013_F09");
	//plot_problem("MMOP_CEC2013_F09");
	//std::vector<Solution<>> pop;
	//HLC hlc(2);
	//initialize_pop(pop, hlc);
	//plot_SPAE("MMOP_CEC2013_F09", pop, hlc);
	//plot_HSLH("MMOP_CEC2013_F09", pop);
	//plot_APC("MMOP_CEC2013_F09", pop);
	//plot_NBC("MMOP_CEC2013_F09", pop);
}

TEST_CASE("BBOB_F09", "[GOP][BBOB][F09][initial]")
{
	std::cout << "BBOB_F09" << std::endl;
	global::ms_arg["maximum evaluations"] = 200000;
	global::ms_global.reset(new global(1, 1./7, 0.5));
	global::ms_global->m_problem.reset(new BBOB_F09("BBOB_F09", 2, 1));
	global::ms_global->m_problem->initialize();
	//plot_EA_KDTree("BBOB_F09");
	//plot_p_fGA("BBOB_F09");
	plot_SPAE_GA("BBOB_F09");
	//plot_problem("BBOB_F09");
	//std::vector<Solution<>> pop;
	//HLC hlc(2);
	//initialize_pop(pop, hlc);
	//plot_SPAE("BBOB_F09", pop, hlc);
	//plot_HSLH("BBOB_F09", pop);
	//plot_APC("BBOB_F09", pop);
	//plot_NBC("BBOB_F09", pop);
}

TEST_CASE("BBOB_F16", "[GOP][BBOB][F16][initial]")
{
	std::cout << "BBOB_F16" << std::endl;
	global::ms_arg["maximum evaluations"] = 200000;
	global::ms_global.reset(new global(1, 1./7, 0.5));
	global::ms_global->m_problem.reset(new BBOB_F16("BBOB_F16", 2, 1));
	global::ms_global->m_problem->initialize();
	//plot_EA_KDTree("BBOB_F16");
	//plot_p_fGA("BBOB_F16");
	plot_SPAE_GA("BBOB_F16");
	//plot_problem("BBOB_F16");
	//std::vector<Solution<>> pop;
	//HLC hlc(2);
	//initialize_pop(pop, hlc);
	//plot_SPAE("BBOB_F16", pop, hlc);
	//plot_HSLH("BBOB_F16", pop);
	//plot_APC("BBOB_F16", pop);
	//plot_NBC("BBOB_F16", pop);
}

TEST_CASE("BBOB_F24", "[GOP][BBOB][F24][initial]")
{
	std::cout << "BBOB_F24" << std::endl;
	global::ms_arg["maximum evaluations"] = 200000;
	global::ms_global.reset(new global(1, 1./7, 0.5));
	global::ms_global->m_problem.reset(new BBOB_F24("BBOB_F24", 2, 1));
	global::ms_global->m_problem->initialize();
	//plot_EA_KDTree("BBOB_F24");
	//plot_p_fGA("BBOB_F24");
	plot_SPAE_GA("BBOB_F24");
	//plot_problem("BBOB_F24");
	//std::vector<Solution<>> pop;
	//HLC hlc(2);
	//initialize_pop(pop, hlc);
	//plot_SPAE("BBOB_F24", pop, hlc);
	//plot_HSLH("BBOB_F24", pop);
	//plot_APC("BBOB_F24", pop);
	//plot_NBC("BBOB_F24", pop);
}

TEST_CASE("GOP_CEC2005_F23", "[GOP][CEC2005][F23][initial]")
{
	std::cout << "GOP_CEC2005_F23" << std::endl;
	global::ms_arg["maximum evaluations"] = 20000;
	global::ms_global.reset(new global(1, 1./7, 0.5));
	global::ms_global->m_problem.reset(new GOP_CEC2005_F23("GOP_CEC2005_F23", 2, 1));
	global::ms_global->m_problem->initialize();
	//plot_EA_KDTree("GOP_CEC2005_F23");
	//plot_p_fGA("GOP_CEC2005_F23");
	plot_SPAE_GA("GOP_CEC2005_F23");
	//plot_problem("GOP_CEC2005_F23");
	//std::vector<Solution<>> pop;
	//HLC hlc(2);
	//initialize_pop(pop, hlc);
	//plot_SPAE("GOP_CEC2005_F23", pop, hlc);
	//plot_HSLH("GOP_CEC2005_F23", pop);
	//plot_APC("GOP_CEC2005_F23", pop);
	//plot_NBC("GOP_CEC2005_F23", pop);
}

TEST_CASE("MMOP_CEC2013_F07_evolved", "[MMOP][CEC2013][F07][evolved]")
{
	std::cout << "MMOP_CEC2013_F07_evolved" << std::endl;
	global::ms_global.reset(new global(1, 1./7, 0.5));
	global::ms_global->m_problem.reset(new MMOP_CEC2013_F07("MMOP_CEC2013_F07", 2));
	global::ms_global->m_problem->initialize();
	std::vector<Solution<>> pop;
	HLC hlc(2, 50);
	initialize_pop(pop, hlc);
	evolve_pop(pop, hlc);
	plot_SPAE("MMOP_CEC2013_F07_evolved", pop, hlc);
	plot_HSLH("MMOP_CEC2013_F07_evolved", pop);
	plot_APC("MMOP_CEC2013_F07_evolved", pop);
	plot_NBC("MMOP_CEC2013_F07_evolved", pop);
}

TEST_CASE("MMOP_CEC2013_F09_evolved", "[MMOP][CEC2013][F09][evolved]")
{
	std::cout << "MMOP_CEC2013_F09_evolved" << std::endl;
	global::ms_global.reset(new global(1, 1./7, 0.5));
	global::ms_global->m_problem.reset(new MMOP_CEC2013_F09("MMOP_CEC2013_F09", 2));
	global::ms_global->m_problem->initialize();
	std::vector<Solution<>> pop;
	HLC hlc(2, 50);
	initialize_pop(pop, hlc);
	evolve_pop(pop, hlc);
	plot_SPAE("MMOP_CEC2013_F09_evolved", pop, hlc);
	plot_HSLH("MMOP_CEC2013_F09_evolved", pop);
	plot_APC("MMOP_CEC2013_F09_evolved", pop);
	plot_NBC("MMOP_CEC2013_F09_evolved", pop);
}

TEST_CASE("BBOB_F09_evolved", "[GOP][BBOB][F09][evolved]")
{
	std::cout << "BBOB_F09_evolved" << std::endl;
	global::ms_global.reset(new global(1, 1./7, 0.5));
	global::ms_global->m_problem.reset(new BBOB_F09("BBOB_F09", 2, 1));
	global::ms_global->m_problem->initialize();
	std::vector<Solution<>> pop;
	HLC hlc(2, 50);
	initialize_pop(pop, hlc);
	evolve_pop(pop, hlc);
	plot_SPAE("BBOB_F09_evolved", pop, hlc);
	plot_HSLH("BBOB_F09_evolved", pop);
	plot_APC("BBOB_F09_evolved", pop);
	plot_NBC("BBOB_F09_evolved", pop);
}

TEST_CASE("BBOB_F16_evolved", "[GOP][BBOB][F16][evolved]")
{
	std::cout << "BBOB_F16_evolved" << std::endl;
	global::ms_global.reset(new global(1, 1./7, 0.5));
	global::ms_global->m_problem.reset(new BBOB_F16("BBOB_F16", 2, 1));
	global::ms_global->m_problem->initialize();
	std::vector<Solution<>> pop;
	HLC hlc(2, 50);
	initialize_pop(pop, hlc);
	evolve_pop(pop, hlc);
	plot_SPAE("BBOB_F16_evolved", pop, hlc);
	plot_HSLH("BBOB_F16_evolved", pop);
	plot_APC("BBOB_F16_evolved", pop);
	plot_NBC("BBOB_F16_evolved", pop);
}

TEST_CASE("BBOB_F24_evolved", "[GOP][BBOB][F24][evolved]")
{
	std::cout << "BBOB_F24_evolved" << std::endl;
	global::ms_global.reset(new global(1, 1./7, 0.5));
	global::ms_global->m_problem.reset(new BBOB_F24("BBOB_F24", 2, 1));
	global::ms_global->m_problem->initialize();
	std::vector<Solution<>> pop;
	HLC hlc(2, 50);
	initialize_pop(pop, hlc);
	evolve_pop(pop, hlc);
	plot_SPAE("BBOB_F24_evolved", pop, hlc);
	plot_HSLH("BBOB_F24_evolved", pop);
	plot_APC("BBOB_F24_evolved", pop);
	plot_NBC("BBOB_F24_evolved", pop);
}

TEST_CASE("GOP_CEC2005_F23_evolved", "[GOP][CEC2005][F23][evolved]")
{
	std::cout << "GOP_CEC2005_F23_evolved" << std::endl;
	global::ms_global.reset(new global(1, 1./7, 0.5));
	global::ms_global->m_problem.reset(new GOP_CEC2005_F23("GOP_CEC2005_F23", 2, 1));
	global::ms_global->m_problem->initialize();
	std::vector<Solution<>> pop;
	HLC hlc(2, 50);
	initialize_pop(pop, hlc);
	evolve_pop(pop, hlc);
	plot_SPAE("GOP_CEC2005_F23_evolved", pop, hlc);
	plot_HSLH("GOP_CEC2005_F23_evolved", pop);
	plot_APC("GOP_CEC2005_F23_evolved", pop);
	plot_NBC("GOP_CEC2005_F23_evolved", pop);
}

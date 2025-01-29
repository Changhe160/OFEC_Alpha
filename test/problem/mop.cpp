#include <iomanip>
#include "../../utility/catch.hpp"
#include "../../run/custom_method.h"
#include "../../core/algorithm/population.h"
#include "../../core/algorithm/Solution.h"
#include"../../core/problem/continuous/continuous.h"

#include "../../instance/algorithm/continuous/multi_objective/nsgaii_sbx/nsgaii_sbx.h"
#include"../../instance/algorithm/template/classic/ga/sbx_pop.h"

#include "../../utility/kd-tree/kdtree_space.h"
#include "../../utility/kd-tree/nanoflann.hpp"
#include "../../utility/kd-tree/data_adaptor.h"
#include "../../utility/random/newran.h"
#include <map>
#include <list>
#include "../../utility/nondominated_sorting/fast_sort.h"
#include "../../utility/clustering/dbscan.h"
#include <fstream>
#include <string>

using namespace ofec;

//TEST_CASE("MOP", "[GLT1]")
//{
//	InstanceManager::ms_instance_manager.reset(new InstanceManager);
//
//
//	std::vector<std::string> alg_names = { "MOEA/D-DE","MOEA/D-SBX","CDGMOEA","GrEA","NSGAII-DE","NSGAII-SBX","NSGAIII-SBX" };
//	InstanceManager::ms_instance_manager.reset(new InstanceManager);
//
//	std::vector<int> DTLZ_objs = { 3,5,8,10,15 };
//
//	std::vector<int> objs_idxs = { 2,3,8};
//	
//
//	for (const auto& alg : alg_names)
//	{
//		ParameterMap params;
//		params["algorithm name"] = alg;
//		params["population size"] = int(200);
//		params["maximum evaluations"] = int(5250);
//		params["number of variables"] = 7;
//		params["kFactor"] = 2;
//		params["lFactor"] = 2;
//		std::string pro_name = "DTLZ";
//		std::vector<int> dtlz_id = { 1,2,3,4,7 };
//
//		for (auto& pro_id : dtlz_id) {
//			std::string cur_pro_name = pro_name + std::to_string(pro_id);
//			params["problem name"] = cur_pro_name;
//			for (auto& obj_idx : DTLZ_objs) {
//				params["number of objectives"] = obj_idx;
//				if (alg.substr(0, 4) == "MOEA") {
//					if (obj_idx >= 5) continue;
//					params["population size"] = int(105);
//				}
//				else if (alg == "CDGMOEA") {
//					if (obj_idx > 3)continue;
//				}
//				else {
//					params["population size"] = int(200);
//				}
//
//
//
//				std::cout << "alg name\t" << alg << "\t" << "problem_names\t" << cur_pro_name << "\t" << "obj number\t" << obj_idx << std::endl;
//
//
//				int id_param = std::make_shared<const ParameterMap>(params);
//				Problem *pro = ADD_PRO(id_param, 0.1);
//				CAST_CONOP(pro)->initialize();
//				Algorithm *alg = ADD_ALG(id_param, 0.1, 0.01, -1);
//				alg.initialize();
//				alg.run();
//
//				DEL_ALG(alg);
//				DEL_PRO(pro);
//				DEL_PARAM(id_param);
//			}
//		}
//
//		{
//			ParameterMap params;
//			params["algorithm name"] = alg;
//			params["population size"] = int(200);
//			params["maximum evaluations"] = int(5250);
//			params["number of variables"] = 7;
//			params["kFactor"] = 2;
//			params["lFactor"] = 2;
//			std::string pro_name = "MOP_GLT";
//
//			for (int pro_id = 1; pro_id <= 6; ++pro_id) {
//				std::string cur_pro_name = pro_name + std::to_string(pro_id);
//				params["problem name"] = cur_pro_name;
//				for (auto& obj_idx : objs_idxs) {
//					if ((pro_id == 5 || pro_id == 6) && obj_idx <= 2)continue;
//					params["number of objectives"] = obj_idx;
//					if (alg.substr(0, 4) == "MOEA") {
//						if (obj_idx != 3) continue;
//						if (obj_idx == 3) params["population size"] = int(105);
//						//else if(obj_idx==2) params["population size"] = int(20);
//					}
//					else if (alg == "CDGMOEA") {
//						if (obj_idx > 3) continue;
//					}
//					else {
//						params["population size"] = int(200);
//					}
//
//					std::cout << "alg name\t" << alg << "\t" << "problem_names\t" << cur_pro_name << "\t" << "obj number\t" << obj_idx << std::endl;
//
//					int id_param = std::make_shared<const ParameterMap>(params);
//					Problem *pro = ADD_PRO(id_param, 0.1);
//					CAST_CONOP(pro)->initialize();
//					Algorithm *alg = ADD_ALG(id_param, 0.1, 0.01, -1);
//					alg.initialize();
//					alg.run();
//
//					DEL_ALG(alg);
//					DEL_PRO(pro);
//					DEL_PARAM(id_param);
//				}
//			}
//		}
//
//	}
//	/*ParameterMap params;
//	params["problem name"] = std::string("MOP_GLT1");
//	params["number of variables"] = 6;
//	params["number of objectives"] = 2;
//	params["algorithm name"] = std::string("CDGMOEA");
//	params["population size"] = int(20);
//	params["maximum evaluations"] = int(2000);
//	int id_param = std::make_shared<const ParameterMap>(params);
//	Problem *pro = ADD_PRO(id_param, 0.1);
//	CAST_CONOP(pro)->initialize();
//	Algorithm *alg = ADD_ALG(id_param, 0.1, 0.01, -1);
//	alg.initialize();
//	alg.run();
//
//	DEL_ALG(alg);
//	DEL_PRO(pro);
//	DEL_PARAM(id_param);*/
//
//	for (size_t i = 5; i <6; i++) {
//		ParameterMap params;
//		if (i == 5) {
//			std::string inx = std::to_string(i+2);
//			params["problem name"] = std::string("DTLZ") + inx;
//		}
//		else {
//			std::string inx = std::to_string(i);
//			params["problem name"] = std::string("DTLZ") + inx;
//		}
//		/*std::string inx = std::to_string(i);
//		params["problem name"] = std::string("WFG") + inx;*/
//		if (i == 5) {
//			params["number of variables"] = 7;
//			params["number of objectives"] = 3;
//			/*params["kFactor"] = 2;
//			params["lFactor"] = 2;*/
//			params["population size"] = int(200);
//			params["maximum evaluations"] = int(5250);
//		}
//		else {
//			params["number of variables"] = 6;
//			params["number of objectives"] = 2;
//			params["population size"] = int(200);
//			params["maximum evaluations"] = int(2000);
//		}
//		params["algorithm name"] = std::string("NSGAIII-SBX");
//		int id_param = std::make_shared<const ParameterMap>(params);
//		Problem *pro = ADD_PRO(id_param, 0.1);
//		CAST_CONOP(pro)->initialize();
//		Algorithm *alg = ADD_ALG(id_param, 0.1, 0.01, -1);
//		alg.initialize();
//		alg.run();
//
//		DEL_ALG(alg);
//		DEL_PRO(pro);
//		DEL_PARAM(id_param);
//	}
//}


TEST_CASE("PF_PS_map", "[PF_PS_map]")
{
	//InstanceManager::ms_instance_manager.reset(new InstanceManager);
	
	//增加问题，变量数，目标数
	ParameterMap params;
	std::vector<int> num_var = { 2,3,4,5,6,7,8,9,10,15,20 };
	std::vector<int> num_obj = { 2 };//暂时只考虑2目标
	std::vector < std::string> pro_name = { "MOP_GLT2" };
	int total_eval = 10000;
	for (size_t i = 0; i < num_obj.size(); ++i) {
		for (size_t j = 0; j < num_var.size(); ++j) {
			if (num_var[j] >= num_obj[i]) {
				for (size_t k = 0; k < pro_name.size(); ++k) {
					params["problem name"] = pro_name[k];
					params["number of variables"] = num_var[j];
					params["number of objectives"] = num_obj[i];
					params["algorithm name"] = std::string("SPMOEA");
					params["population size"] = int(100);
					if (num_var[j] < 5) {
						total_eval = 300000;
					}
					else if (num_var[j] < 10) {
						total_eval = 500000;
					}
					else {
						total_eval = 700000;
					}
					params["maximum evaluations"] = total_eval;
					params["number of obj subspaces"] = int(40);
					params["number of subspaces"] = int(200);
					int id_param = std::make_shared<const ParameterMap>(params);
					Problem *pro = ADD_PRO(id_param, 0.1);
					CAST_CONOP(pro)->initialize();
					Algorithm *alg = ADD_ALG(id_param, 0.1, 0.01, -1);
					alg->initialize();
					alg->run();

					DEL_ALG(alg);
					DEL_PRO(pro);
					DEL_PARAM(id_param);
				}
			}
		}
	}
}

TEST_CASE("MOP", "[ZDT1]")
{
	//InstanceManager::ms_instance_manager.reset(new InstanceManager);

	ParameterMap params;
	params["problem name"] = std::string("MOP_GLT1");
	params["number of variables"] = 4;
	params["number of objectives"] = 2;
	params["algorithm name"] = std::string("NSGAII-SBX");
	params["population size"] = int(40);
	params["maximum evaluations"] = int(2000);
	int id_param = std::make_shared<const ParameterMap>(params);
	Problem *pro = ADD_PRO(id_param, 0.1);
	CAST_CONOP(pro)->initialize();
	Algorithm *alg = ADD_ALG(id_param, 0.1, 0.01, -1);
	alg->initialize();
	alg->run();

	DEL_ALG(alg);
	DEL_PRO(pro);
	DEL_PARAM(id_param);
}

bool edgeSamplingJudge(std::vector<std::shared_ptr<Solution<>>> &his_data, std::vector<std::shared_ptr<Solution<>>>& front_data, Problem *pro, Random *rnd, size_t& count) {
	bool islocal = false;
	size_t num_var = CAST_CONOP(pro)->numberVariables();
	if (front_data.size() < num_var) {
		islocal = false;
		return islocal;
	}
	std::vector<Real> dim_span;
	std::map<size_t, std::pair<Real, Real>> dim_slice;
	std::vector<size_t> lower_inx;
	std::vector<size_t> upper_inx;
	//寻找每一维的切面
	for (size_t i = 0; i < num_var; ++i) {
		Real min_v = (Real)INT16_MAX;
		Real max_v = (Real)INT16_MAX * -1;
		for (size_t j = 0; j < front_data.size(); ++j) {
			if (min_v > front_data[j]->variable().vect()[i]) {
				min_v = front_data[j]->variable().vect()[i];
			}
			if (max_v < front_data[j]->variable().vect()[i]) {
				max_v = front_data[j]->variable().vect()[i];
			}
		}
		dim_span.push_back(max_v - min_v);
		dim_slice.insert(std::make_pair(i, std::make_pair(min_v, max_v)));
	}
	//找出最小的跨度的维
	auto min_value = std::min_element(dim_span.begin(), dim_span.end());
	auto min_dim_inx = std::distance(dim_span.begin(), min_value);
	//封闭超矩形的中心坐标
	std::vector<Real> center;
	for (size_t i = 0; i < num_var; ++i) {
		center.push_back((dim_slice[i].first + dim_slice[i].second) / 2);
	}
	//前沿点之间的

	//将中心点沿着最小跨度维平移得到2个测试点
	Real shift_v = 0.;
	Real d1 = dim_slice[min_dim_inx].second - center[min_dim_inx];
	//Real d2 = center[min_dim_inx]- dim_slice[min_dim_inx].first;
	Real d3 = CAST_CONOP(pro)->domain().range(min_dim_inx).limit.second - center[min_dim_inx];
	Real d4 = center[min_dim_inx] - CAST_CONOP(pro)->domain().range(min_dim_inx).limit.first;
	Real max_b = d3 < d4 ? d3 : d4;
	shift_v = d1 + rnd->uniform.next() * (max_b - d1);
	std::vector<Real> ind1, ind2;
	ind1 = ind2 = center;
	ind1[min_dim_inx] += shift_v;
	ind2[min_dim_inx] -= shift_v;
	PopSBX<> temp_pop(2, pro);
	temp_pop[0].variable().vect() = ind1;
	temp_pop[1].variable().vect() = ind2;
	temp_pop.evaluate(pro, -1);
	his_data.emplace_back(new Solution<>(temp_pop[0]));
	his_data.emplace_back(new Solution<>(temp_pop[1]));
	count += 2;
	//判断两个解是否被当前子空间前沿支配
	for (size_t i = 0; i < temp_pop.size(); ++i) {
		bool flag = false;
		for (size_t j = 0; j < front_data.size(); ++j) {
			auto relation = temp_pop[i].compare(*front_data[j], CAST_CONOP(pro)->optimizeMode());
			if (relation == Dominance::kDominated) {
				flag = true;
			}
		}
		if (!flag) {
			break;
		}
		if (i == temp_pop.size() - 1) {
			islocal = true;
		}
	}
	//更新前沿
	for (size_t i = 0; i < temp_pop.size(); ++i) {
		bool if_preserve = true;
		for (size_t j = 0; j < front_data.size(); ++j) {
			if (front_data[j].dominate(temp_pop[i], pro)) {
				if_preserve = false;
				break;
			}
		}
		if (if_preserve) {
			auto temp_front_sols = front_data;
			front_data.clear();
			front_data.emplace_back(new Solution<>(temp_pop[i]));
			for (size_t j = 0; j < temp_front_sols.size(); ++j) {
				if (!temp_pop[i].dominate(temp_front_sols[j], pro)) {
					front_data.emplace_back(temp_front_sols[j]);
				}
			}
		}
	}
	
	return islocal;
}

bool frontClusterJudge(std::vector<std::shared_ptr<Solution<>>>& front_data, Problem *pro, Random *rnd) {
	bool islocal;
	size_t num_var = CAST_CONOP(pro)->numberVariables();
	size_t minimum_num_cluster = 2;//每个类中最小的个体数
	if (front_data.size() < num_var) {
		islocal = false;
	}
	else {
		Real epsilon = 0.5 * 0.5;
		std::vector<std::vector<Real>> cluster_sol;
		for (size_t i = 0; i < front_data.size(); ++i) {
			cluster_sol.emplace_back(front_data[i]->variable().vect());
		}
		DBSCAN dscluster(minimum_num_cluster, epsilon, cluster_sol);
		dscluster.run();
		std::vector<int> cluster_id;//每个解所属的类
		for (size_t i = 0; i < dscluster.m_points.size(); ++i) {
			cluster_id.push_back(dscluster.m_points[i]->clusterID);
		}
		std::vector<int> cluster_num;//所有的类
		cluster_num.push_back(cluster_id[0]);
		for (size_t i = 1; i < cluster_id.size(); ++i) {
			if (std::find(cluster_num.begin(), cluster_num.end(), cluster_id[i]) == cluster_num.end()) {
				cluster_num.push_back(cluster_id[i]);
			}
		}
		//将相同的类的点放在一起
		std::vector<std::vector<size_t>> ind2cluster;
		for (size_t i = 0; i < cluster_num.size(); ++i) {
			std::vector<size_t> temp_cluster;
			for (size_t j = 0; j < cluster_id.size(); ++j) {
				if (cluster_id[j] == cluster_num[i]) {
					temp_cluster.push_back(j);
				}
			}
			ind2cluster.emplace_back(temp_cluster);
		}

		//根据每个类前沿的位置判断
		//先得到边界
		std::vector<std::pair<Real, Real>> bound;
		for (size_t i = 0; i < num_var; ++i) {
			bound.emplace_back(CAST_CONOP(pro)->domain().range(i).limit);
		}
		bool flag = false;
		for (size_t i = 0; i < ind2cluster.size(); ++i) {
			auto inds_inx = ind2cluster[i];
			std::vector<Real> center_pos;
			for (size_t i = 0; i < num_var; ++i) {
				Real temp_sum = 0.;
				for (size_t j = 0; j < inds_inx.size(); ++j) {
					temp_sum += (front_data[inds_inx[j]]->variable()[i]);
				}
				center_pos.push_back(temp_sum / inds_inx.size());
			}
			//中心点到bound的最短距离及维度
			flag = false;
			std::vector<Real> min_dim_dist(num_var, 0.);
			for (size_t j = 0; j < num_var; ++j) {
				min_dim_dist[j] = std::fabs(center_pos[j] - bound[j].first) < std::fabs(center_pos[j] - bound[j].second) ? std::fabs(center_pos[j] - bound[j].first) : std::fabs(center_pos[j] - bound[j].second);
				Real delta = min_dim_dist[j] / (bound[j].second - bound[j].first);
				if (delta < 0.1) {
					flag = true;
					break;
				}
			}
		}
		if (!flag) {
			islocal = true;
		}
		else {
			islocal = false;
		}
	}
	
	return islocal;
}

bool evovleDirectionJudge(const std::vector<std::vector<std::shared_ptr<Solution<>>>> &gen_front_data, Problem *pro, Random *rnd, size_t& count) {
	//对历史前沿数据按照时间
	return false;
}

bool decisionTreeJudge() {
	return false;
}

Real propobalityJudge() {

	return 0.5;
}

TEST_CASE("identify local Pareto", "[identify local Pareto]")
{
	ParameterMap params;
	int num_var = 2;
	int num_obj = 2;//暂时只考虑2目标
	//int var_partition_num = 20;
	std::vector<std::string> all_name = {"MOP_feature2" ,"MOP_feature3" ,"MOP_feature4" ,"MOP_feature5" ,"MOP_feature6", "MOP_feature7", "MOP_feature8" };
	//std::vector<std::string> all_name = {"MOP_feature6"};
	int total_eval = 300;
	int size_pop = 10;
	size_t run_times = 10;

	for (size_t p = 0; p < all_name.size(); ++p) {
		for (size_t i = 0; i < run_times; ++i) {
			//问题参数
			params["problem name"] = all_name[p];
			params["number of variables"] = num_var;
			params["number of objectives"] = num_obj;

			//算法参数
			params["algorithm name"] = std::string("NSGAII-SBX");
			params["population size"] = size_pop;
			//params["number of subspaces"] = var_partition_num;
			params["maximum evaluations"] = total_eval;

			int id_param = std::make_shared<const ParameterMap>(params);
			Problem *pro = ADD_PRO(id_param, 0.1 * (i + 1) / 5);
			CAST_CONOP(pro)->initialize();
			Random *rnd = ADD_RND(0.5);//每次不同

			//根据问题范围划分搜索子空间
			std::vector<std::pair<Real, Real>> bound;
			for (size_t j = 0; j < num_var; ++j) {
				bound.emplace_back(CAST_CONOP(pro)->domain().range(j).limit);
			}

			//写入文件
			// "_edgeSampling","frontCluster","_evolveDirection","_decisionTree","_probabilityModel"
			auto file_name = g_working_dir + "/result/judgePS/edgeSampling/" + all_name[p]
				+ "_eval_" + std::to_string(total_eval)
				+ "_pop_" + std::to_string(size_pop) + "_edgeSampling" + std::to_string(i + 1) + ".txt";
			std::ofstream out_file;
			out_file.open(file_name);

			//初始化种群1
			PopSBX<> pop(size_pop, pro);
			pop.initialize(pro, rnd);
			pop.setRate(0.9, 1. / num_var);
			pop.setEta(20, 20);
			size_t eval_count1 = 0;
			pop.evaluate(pro, -1);
			eval_count1 += size_pop;
			std::vector<std::shared_ptr<Solution<>>> his_data1;//存储历史解
			std::vector<std::shared_ptr<Solution<>>> front_data1;//存储前沿解
			std::vector<std::vector<std::shared_ptr<Solution<>>>> gen_front_data1;//存储前沿解
			for (size_t j = 0; j < size_pop; ++j) {
				his_data1.emplace_back(new Solution<>(pop[j]));
			}
			std::vector<std::vector<Real>*> objs0;
			for (size_t j = 0; j < pop.size(); ++j) {
				objs0.emplace_back(&pop[j].objective());
			}
			std::vector<int> rank0;
			ofec::nd_sort::fastSort<Real>(objs0, rank0, CAST_CONOP(pro)->optimizeMode());
			for (size_t j = 0; j < pop.size(); ++j) {
				pop[j].setFitness(rank0[j]);
				if (rank0[j] == 0) {
					pop[j].setCounter(0);
					front_data1.emplace_back(new Solution<>(pop[j]));
				}
			}
			gen_front_data1.emplace_back(front_data1);

			//初始化种群2
			//Algorithm *alg = ADD_ALG(id_param, 0.1, 0.01, -1);
			PopNSGAII_SBX pop2(size_pop, pro);
			pop2.initialize(pro, rnd);
			pop2.setRate(0.9, 1. / num_var);
			pop2.setEta(20, 20);
			size_t eval_count2 = 0;
			pop2.evaluate(pro, -1);
			eval_count2 += size_pop;
			std::vector<std::shared_ptr<Solution<>>> his_data2;//存储历史解
			std::vector<std::shared_ptr<Solution<>>> front_data2;//存储前沿解
			std::vector<std::vector<std::shared_ptr<Solution<>>>> gen_front_data2;//存储前沿解
			for (size_t j = 0; j < size_pop; ++j) {
				his_data2.emplace_back(new Solution<>(pop2[j]));
			}
			std::vector<std::vector<Real>*> objs1;
			for (size_t j = 0; j < pop2.size(); ++j) {
				objs1.emplace_back(&pop2[j].objective());
			}
			std::vector<int> rank1;
			ofec::nd_sort::fastSort<Real>(objs1, rank1, CAST_CONOP(pro)->optimizeMode());
			for (size_t j = 0; j < pop2.size(); ++j) {
				pop2[j].setFitness(rank1[j]);
				if (rank1[j] == 0) {
					pop2[j].setCounter(0);
					front_data2.emplace_back(new Solution<>(pop2[j]));
				}
			}
			gen_front_data2.emplace_back(front_data2);

			//算法循环
			size_t iter_num = 1;
			while (eval_count1 < total_eval) {
				//提取搜索数据，采用不同方法检测是否具有局部前沿
				bool islocal1 = edgeSamplingJudge(his_data1, front_data1, pro, rnd, eval_count1);
				bool islocal2 = edgeSamplingJudge(his_data2, front_data2, pro, rnd, eval_count2);

				/*bool islocal1 = frontClusterJudge(front_data1, pro, rnd);
				bool islocal2 = frontClusterJudge(front_data2, pro, rnd);*/

				/*bool islocal1 = evovleDirectionJudge(gen_front_data1, pro, rnd, eval_count1);
				bool islocal2 = evovleDirectionJudge(gen_front_data2, pro, rnd, eval_count2);*/

				out_file << eval_count1 << " " << islocal1 << " " << eval_count2 << " " << islocal2 << std::endl;

				//种群1产生新解
				for (size_t j = 0; j < pop.size(); ++j) {
					//随机生成一个位置
					std::vector<Real> temp_pos;
					for (size_t k = 0; k < num_var; ++k) {
						temp_pos.push_back(bound[k].first + rnd->uniform.next() * (bound[k].second - bound[k].first));
					}
					pop[j].variable().vect() = temp_pos;
				}
				pop.evaluate(pro, -1);

				eval_count1 += size_pop;
				for (size_t j = 0; j < size_pop; ++j) {
					his_data1.emplace_back(new Solution<>(pop[j]));
				}
				//更新前沿解
				//先对新个体排序
				std::vector<std::vector<Real>*> objs;
				for (size_t j = 0; j < pop.size(); ++j) {
					objs.emplace_back(&pop[j].objective());
				}
				std::vector<int> rank;
				ofec::nd_sort::fastSort<Real>(objs, rank, CAST_CONOP(pro)->optimizeMode());
				std::vector<size_t> front_inx;
				for (size_t j = 0; j < pop.size(); ++j) {
					pop[j].setFitness(rank[j]);
					if (rank[j] == 0) {
						front_inx.push_back(j);
					}
				}
				for (size_t j = 0; j < front_inx.size(); ++j) {
					bool if_preserve = true;
					for (size_t k = 0; k < front_data1.size(); ++k) {
						if (front_data1[k].dominate(pop[front_inx[j]], pro)) {
							if_preserve = false;
							break;
						}
					}
					if (if_preserve) {
						auto temp_front_sols = front_data1;
						front_data1.clear();
						pop[front_inx[j]].setCounter(iter_num);
						front_data1.emplace_back(new Solution<>(pop[front_inx[j]]));
						for (size_t k = 0; k < temp_front_sols.size(); ++k) {
							if (!pop[front_inx[j]].dominate(temp_front_sols[k], pro)) {
								front_data1.emplace_back(temp_front_sols[k]);
							}
						}
					}
				}


				//种群2产生新解，环境选择
				int tag = pop2.evolve(pro, -1, rnd);
				eval_count2 += size_pop;
				if (tag != kNormalEval) {
					break;
				}
				auto compound_pop = pop2.getCombinPop();
				for (size_t j = 0; j < compound_pop.size() / 2; ++j) {
					his_data2.emplace_back(new Solution<>(compound_pop[j]));
				}
				//更新前沿解
				//先对新个体排序
				std::vector<std::vector<Real>*> objs2;
				for (size_t j = 0; j < pop2.size(); ++j) {
					objs2.emplace_back(&compound_pop[j].objective());
				}
				std::vector<int> rank2;
				ofec::nd_sort::fastSort<Real>(objs2, rank2, CAST_CONOP(pro)->optimizeMode());
				std::vector<size_t> front_inx2;
				for (size_t j = 0; j < pop2.size(); ++j) {
					if (rank2[j] == 0) {
						front_inx2.push_back(j);
					}
				}
				for (size_t j = 0; j < front_inx2.size(); ++j) {
					bool if_preserve = true;
					for (size_t k = 0; k < front_data2.size(); ++k) {
						if (front_data2[k].dominate(compound_pop[front_inx2[j]], pro)) {
							if_preserve = false;
							break;
						}
					}
					if (if_preserve) {
						auto temp_front_sols = front_data2;
						front_data2.clear();
						compound_pop[front_inx2[j]].setCounter(iter_num);
						front_data2.emplace_back(new Solution<>(compound_pop[front_inx2[j]]));
						for (size_t k = 0; k < temp_front_sols.size(); ++k) {
							if (!compound_pop[front_inx2[j]].dominate(temp_front_sols[k], pro)) {
								front_data2.emplace_back(temp_front_sols[k]);
							}
						}
					}
				}
				iter_num++;
			}
			out_file.close();

			DEL_PRO(pro);
			DEL_RND(rnd);
			DEL_PARAM(id_param);
		}
	}
	
}

Real predict(const std::vector<Real>& weigh, const std::vector<Real>& data, int feature_num) {
	Real sum = 0;
	for (int i = 0; i < feature_num; i++) {
		sum += weigh[i] * data[i];
	}
	return sum;
}

// 损失函数
Real Theta(const std::vector<std::vector<Real>>& training_set, int featue_num, const std::vector<Real>& w) {
	Real sum = 0;
	for (int i = 0; i < training_set.size(); i++) {
		sum += (training_set[i][featue_num] - predict(w, training_set[i], featue_num)) * (training_set[i][featue_num] - predict(w, training_set[i], featue_num));
	}
	return sum / (2 * training_set.size());
}

// 梯度下降
void gradient_descent(const std::vector<std::vector<Real>>& training_set, int feature_num, std::vector<Real>& w, Real learn_ratio, int iterator_time) {
	int sample_num = training_set.size();
	size_t count = 0;
	//Real loss = 1.;
	while (count < iterator_time) {
		//迭代前J的值
		Real temp = Theta(training_set, feature_num, w);

		std::vector<Real> del_theta(feature_num, 0.);
		for (int i = 0; i < feature_num; i++) {
			for (int j = 0; j < sample_num; j++) {
				del_theta[i] += ((predict(w, training_set[j], feature_num) - training_set[j][feature_num]) * training_set[j][i]);
			}
		}
		//w[i]的更新必须等所有的del_theta测算出来了才可以！不然更新的会影响没更新的
		//上述问题在代码内表示即是下面的for循环不能和上面的合并！
		for (int i = 0; i < feature_num; i++)
		{
			w[i] -= learn_ratio * del_theta[i] / (Real)sample_num;
		}
		//迭代后J的值
		Real temp1 = Theta(training_set, feature_num, w);
		//loss = temp1;
		//两次迭代J的值变化小于0.001，循环终止
		if (std::fabs(temp1) < 0.00001) {
			break;
		}
		/*if (std::fabs(temp1 - temp) < 0.0001) {
			break;
		}*/
		count++;
		//std::cout <<"iterate_time: "<< count<<"         " << "J_Theta = " << Theta(training_set, feature_num, w) << std::endl;
	}
}


std::vector<Real> linearRegression(const std::vector<std::vector<Real>>& input_data, Real learn_ratio, int iterate_times) {
	std::vector<std::vector<Real>> training_set = input_data;
	std::vector<Real> weigh(input_data[0].size() - 1, 1.0);
	//梯度下降
	gradient_descent(training_set, input_data[0].size() - 1, weigh, learn_ratio, iterate_times);//第二个参数为特征数
	return weigh;
}

TEST_CASE("linear regression", "[linear regression]")
{
	Real learn_ratio = 0.01;
	size_t iterate_times = 10000;
	std::vector<std::vector<Real>> train_data;
	std::vector<Real> x1 = {-1,-0.9,-0.8,-0.7,-0.6,-0.5,-0.4,-0.3,-0.2,-0.1};
	for (size_t i = 0; i < x1.size(); ++i) {
		std::vector<Real> temp;
		temp.push_back(x1[i]);
		temp.push_back(1.);
		temp.push_back(-10000*x1[i]-10);
		train_data.emplace_back(temp);
	}
	auto weigh = linearRegression(train_data, learn_ratio, iterate_times);
	size_t a = 1;
}

TEST_CASE("DBSCAN", "[DBSCAN]")
{
	//还有问题，分的类不对
	//产生数据点，环状数据
	Random *rnd = ADD_RND(0.5);//每次不同
	std::vector<std::vector<Real>> data;
	Real angle = 2 * OFEC_PI;
	size_t point_num = 500;
	Real interval = 0.1;
	size_t clu_num = 3;
	for (size_t i = 0; i < clu_num; i++) {
		for (size_t j = 0; j < point_num; ++j) {
			Real r = 0.4*(i+1)+interval*(2*rnd->uniform.next()-1);
			Real theta = angle/point_num*j;
			std::vector<Real> p(2);
			p[0] = r*std::sin(theta);
			p[1] = r*std::cos(theta);
			data.emplace_back(p);
		}
	}
	//采用DBSCAN聚类
	size_t minPts = 4;
	Real epsilon = 0.2;
	
	DBSCAN dscluster(minPts, epsilon, data);
	dscluster.run();
	std::vector<int> cluster_id;//每个解所属的类
	for (size_t i = 0; i < dscluster.m_points.size(); ++i) {
		cluster_id.push_back(dscluster.m_points[i]->clusterID);
	}
	std::vector<int> cluster_num;//所有的类
	cluster_num.push_back(cluster_id[0]);
	for (size_t i = 1; i < cluster_id.size(); ++i) {
		if (std::find(cluster_num.begin(), cluster_num.end(), cluster_id[i]) == cluster_num.end()) {
			cluster_num.push_back(cluster_id[i]);
		}
	}
	//将相同的类的点放在一起
	std::vector<std::vector<size_t>> ind2cluster;
	for (size_t i = 0; i < cluster_num.size(); ++i) {
		std::vector<size_t> temp_cluster;
		for (size_t j = 0; j < cluster_id.size(); ++j) {
			if (cluster_id[j] == cluster_num[i]) {
				temp_cluster.push_back(j);
			}
		}
		ind2cluster.emplace_back(temp_cluster);
	}


	DEL_RND(rnd);
}



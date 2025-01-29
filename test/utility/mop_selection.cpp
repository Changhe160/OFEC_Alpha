#include <iomanip>
#include "../utility/catch.hpp"
#include "../run/custom_method.h"
#include "../core/algorithm/population.h"
#include "../core/algorithm/Solution.h"
#include <fstream>
#include "../instance/algorithm/template/multi_objective/nsgaii/nsgaii.h"
#include "../instance/algorithm/template/multi_objective/moead/moead.h"
#include "../utility/kd-tree/kdtree_space.h"
#include "../utility/functional.h"
#include "../instance/problem/continuous/multi_objective/indicator.h"
#include<string>

using namespace ofec;

std::vector<size_t> selectIndSubspace(const std::vector<Real>& ref_point,const std::vector<std::vector<Real>>& points,int num) {
	std::vector<size_t> select_ind;
	size_t count = 0;
	//先加入边界点
	size_t num_obj = ref_point.size();
	for (size_t i = 0; i < num_obj; ++i) {
		if (count < num) {
			size_t inx = 0;
			Real max_v = -1. * 10e14;
			for (size_t j = 0; j < points.size(); ++j) {
				if (points[j][i] > max_v) {
					max_v = points[j][i];
					inx = j;
				}
			}
			if (select_ind.empty()||std::find(select_ind.begin(),select_ind.end(),inx)==select_ind.end()) {
				select_ind.push_back(inx);
				count++;
			}
		}
	}
	//再基于边界点选择中间点
	while (points.size()>num && count < num) {
		size_t inx = 0;
		Real max_ind_angle = -1. * 10e14;
		std::vector<Real> ind_min_angle;//选用角度还是弧长还是跨度计算
		for (size_t i = 0; i < points.size(); ++i) {
			Real min_angle = 1.*10e14;
			for (size_t j = 0; j < select_ind.size(); ++j) {
				auto angle_v = vectorAngle(ref_point, points[i], points[select_ind[j]]);
				if (angle_v < min_angle) {
					min_angle = angle_v;
				}
			}
			ind_min_angle.push_back(min_angle);
		}
		for (size_t i = 0; i < ind_min_angle.size(); ++i) {
			if (ind_min_angle[i] > max_ind_angle) {
				max_ind_angle = ind_min_angle[i];
				inx = i;
			}
		}
		select_ind.push_back(inx);
		count++;
	}
	return select_ind;
}

Real calSubspaceSpan(const std::vector<Real>& ideal_point, const std::vector<Real>& nadir_point, const std::vector<std::vector<Real>>& points) {
	//先判断子空间流形的凹凸，决定选择哪一个为参考点
	//先找边界点
	if (points.size() == 1) {
		return 0.00001;
	}
	else {
		std::vector<std::vector<Real>> boundary_points;
		size_t num_obj = ideal_point.size();
		for (size_t i = 0; i < num_obj; ++i) {
			if (i + 1 <= points.size()) {
				size_t inx = 0;
				Real max_v = -1. * 10e14;
				for (size_t j = 0; j < points.size(); ++j) {
					if (points[j][i] > max_v) {
						max_v = points[j][i];
						inx = j;
					}
				}
				boundary_points.push_back(points[inx]);
			}
		}

		bool if_convex = false;
		//记录边界点到理想点的最小值
		Real min_v = 1. * 10e14;
		for (const auto& p : boundary_points) {
			auto dist = euclideanDistance(ideal_point.begin(), ideal_point.end(), p.begin());
			if (dist < min_v) {
				min_v = dist;
			}
		}
		//再看其他点的距离有没有小于最小值的
		size_t count = 0;//记数满足条件数
		for (const auto& p : points) {
			auto dist = euclideanDistance(ideal_point.begin(), ideal_point.end(), p.begin());
			if (dist < min_v) {
				count++;
			}
		}
		if (count > 1) {
			if_convex = true;
		}
		std::vector<Real> ref_point;
		if (if_convex) {
			ref_point = nadir_point;
		}
		else {
			ref_point = ideal_point;
		}
		auto span = manifoldSpan(ref_point, points);
		return span.second;
	}
}

std::vector<std::vector<Real>> crowdSelect(const std::vector<std::vector<Real>>& points, int num) {
	std::vector<std::vector<Real>> select_pop=points;
	while (select_pop.size() > num)
	{
		//先找出子目标极值个体
		std::vector<size_t> sub_optima_inx;
		for (size_t p = 0; p < select_pop[0].size(); ++p) {
			size_t inx = 0;
			Real max_v = -1. * 10e14;
			for (size_t q = 0; q < select_pop.size(); ++q) {
				if (select_pop[q][p] > max_v) {
					max_v = select_pop[q][p];
					inx = q;
				}
			}
			sub_optima_inx.push_back(inx);
		}
		
		//先计算个体的拥挤度
		std::vector<Real> density;
		for (size_t p = 0; p < select_pop.size(); ++p) {
			Real min_dist = 1.e14;
			if (std::find(sub_optima_inx.begin(), sub_optima_inx.end(), p) == sub_optima_inx.end()) {
				for (size_t k = 0; k < select_pop.size(); ++k) {
					if (p != k) {
						auto dist = euclideanDistance(select_pop[p].begin(), select_pop[p].end(), select_pop[k].begin());
						if (dist < min_dist) {
							min_dist = dist;
						}
					}
				}
			}
			density.push_back(min_dist);
		}
		//再去掉最拥挤的
		size_t inx = 0;
		Real dist = 1.e14;
		for (size_t p = 0; p < density.size(); ++p) {
			if (density[p] < dist) {
				dist = density[p];
				inx = p;
			}
		}
		select_pop.erase(select_pop.begin() + inx);
	}
	return select_pop;
}

std::vector<std::vector<Real>> randomSelect(const std::vector<std::vector<Real>>& points, int num) {
	std::vector<std::vector<Real>> select_pop;
	std::vector<size_t> ind_index;
	size_t total_num = points.size();
	for (size_t i = 0; i < total_num; ++i) {
		ind_index.push_back(i);
	}
	GET_RND(0).uniform.shuffle(ind_index.begin(),ind_index.end());
	for (size_t i = 0; i < num; ++i) {
		select_pop.push_back(points[ind_index[i]]);
	}
	return select_pop;
}

TEST_CASE("random_selection", "[random_selection]")
{


	std::vector<int> num_samples = { 100,200,250,300,400,500,600,750,800,1000,1600,2000,3000,3200,4000,5000 };//pop_size
	//std::vector<int> num_samples = { 100 };//pop_size
	std::vector<int> num_output1 = { 10,20,40,50,80,100,105,150,200,300,400,500 };
	std::vector<int> num_output2 = { 40,50,80,100,105,150,200,300,400,500 };
	//std::vector<int> num_output = { 10 };
	std::vector<int> num_obj = { 2,3 };
	//std::vector<int> num_obj = { 4,6,8,10,12,15};

	std::vector<std::string> PF_files = { "linear", "convex", "concave", "mix", "discontinue" };
	//std::vector<std::string> PF_files ={"discontinue"};

	for (const auto& tmp_f : PF_files) {
		for (size_t j = 0; j < num_obj.size(); ++j) {
			Real hv = -1.;
			std::vector<int> num_output;
			if (num_obj[j] > 3) {
				num_output = num_output2;
			}
			else {
				num_output = num_output1;
			}
			for (int i = num_samples.size() - 1; i >= 0; --i) {
				for (size_t ii = 0; ii < num_output.size(); ++ii) {
					if (num_output[ii] < num_samples[i]) {
						//先读数据
						std::vector<std::vector<Real>> obj_sols;
						std::ifstream infile;
						std::stringstream os;
						os << "E:/PlatEMO/PF/data/" << num_samples[i] << "_points/" << num_obj[j] << "_obj/" << tmp_f << ".txt";
						infile.open(os.str());
						if (!infile) {
							throw MyExcept("open PF file is fail");
						}
						std::string str;
						size_t line = 0;
						while (getline(infile, str))
							++line;
						obj_sols.resize(line);
						infile.close();
						infile.clear();
						infile.open(os.str());
						size_t numobj = num_obj[j];
						for (size_t k = 0; k < line; k++) {
							std::vector<Real> temp_obj(numobj);
							for (size_t p = 0; p < numobj; p++) {
								infile >> temp_obj[p];
							}
							obj_sols[k] = temp_obj;
						}
						infile.close();

						//数据归一化
						auto temp_obj_sols = obj_sols;
						dataNormalize(obj_sols);

						ParameterMap params;
						params["algorithm name"] = "NSGAII-SBX";
						params["population size"] = int(obj_sols.size());
						params["maximum evaluations"] = int(100);
						params["number of variables"] = 8;
						params["kFactor"] = 2;
						params["lFactor"] = 2;
						params["problem name"] = (std::string)"MOP_DTLZ1";
						params["number of objectives"] = (int)numobj;

						int id_param = std::make_shared<const ParameterMap>(params);
						Problem *pro = Problem::generateByFactory(id_param, 0.1);
						pro->initialize();

						std::vector<OptimizeMode> opt_mode;
						for (size_t j = 0; j < numobj; ++j) {
							opt_mode.push_back(OptimizeMode::kMinimize);
						}

						if (i == num_samples.size() - 1 && hv < 0.) {
							std::vector<Real> hv_ref_points(numobj, 1.1);
							std::vector<Real> metrics;
							hv = hyperVolume(obj_sols, hv_ref_points, pro, pro);
						}
						/*****************************************************************/
						/*                           随机选择
						/*****************************************************************/
						std::vector<std::vector<Real>> pop_select1;
						if (obj_sols.size() > num_output[ii]) {
							pop_select1.resize(num_output[ii]);
							pop_select1 = randomSelect(obj_sols, num_output[ii]);
							/*****************************************************************/
							/*                           性能指标评价
							/*****************************************************************/
							std::vector<Real> hv_ref_points(numobj, 1.1);
							std::vector<Real> metrics;
							auto s1 = hyperVolume(pop_select1, hv_ref_points, pro, pro) / hv;
							auto s2 = Spacing(pop_select1);
							metrics.push_back(s1);
							metrics.push_back(s2);

							//将选择的个体保存
							std::vector<std::pair<Real, Real>> boundary;
							size_t num_p = temp_obj_sols.size();
							size_t num_d = temp_obj_sols[0].size();
							for (size_t i = 0; i < num_d; ++i) {
								Real min_v = 1 * 10e14;
								Real max_v = -1 * 10e14;
								for (size_t j = 0; j < num_p; ++j) {
									if (temp_obj_sols[j][i] < min_v)
										min_v = temp_obj_sols[j][i];
									if (temp_obj_sols[j][i] > max_v)
										max_v = temp_obj_sols[j][i];
								}
								boundary.push_back(std::make_pair(min_v, max_v));
							}
							dataRestore(pop_select1, boundary);
							std::string file_name = "E:/Gitlab/OFEC/result/PF/random_pops/" + std::to_string(num_samples[i]) + "_points/" \
								+ std::to_string(num_output[ii]) + "_output/" + std::to_string(num_obj[j]) + "_obj/" + tmp_f + ".txt";
							std::ofstream out(file_name);
							if (out) {
								for (size_t i = 0; i < pop_select1.size(); ++i) {
									for (size_t j = 0; j < pop_select1[i].size(); ++j) {
										out << pop_select1[i][j] << " ";
									}
									out << std::endl;
								}
								out.close();
							}

							//性能指标写入文件
							std::string file_name2 = "E:/Gitlab/OFEC/result/PF/random_metrics/" + tmp_f + \
								"/" + std::to_string(num_samples[i]) + "_points/" + std::to_string(num_obj[j]) + ".txt";
							std::ofstream out2(file_name2, std::ios::app);
							if (out2) {
								for (auto& me : metrics) {
									out2 << me << " ";
								}
								out2 << std::endl;
								out2.close();
							}
						}
						DEL_PRO(pro);
						DEL_PARAM(id_param);
					}
				}
			}
		}
		std::cout << "finish one type" << std::endl;
	}
}

TEST_CASE("crowded_selection", "[crowded_selection]")
{


	std::vector<int> num_samples = {100,200,250,300,400,500,600,750,800,1000,1600,2000,3000,3200,4000,5000};//pop_size
	//std::vector<int> num_samples = { 100 };//pop_size
	std::vector<int> num_output1 = {10,20,40,50,80,100,105,150,200,300,400,500};
	std::vector<int> num_output2 = {40,50,80,100,105,150,200,300,400,500 };
	//std::vector<int> num_output = { 10 };
	std::vector<int> num_obj = { 2,3 };
	//std::vector<int> num_obj = { 4,6,8,10,12,15};

	std::vector<std::string> PF_files ={ "linear", "convex", "concave", "mix", "discontinue" };
	//std::vector<std::string> PF_files ={"discontinue"};

	for (const auto& tmp_f : PF_files) {
		for (size_t j = 0; j < num_obj.size(); ++j) {
			Real hv=-1.;
			std::vector<int> num_output;
			if (num_obj[j] > 3) {
				num_output = num_output2;
			}
			else {
				num_output = num_output1;
			}
			for (int i = num_samples.size() - 1; i >= 0; --i) {
				for (size_t ii = 0; ii < num_output.size(); ++ii) {
					if (num_output[ii] < num_samples[i]) {
						//先读数据
						std::vector<std::vector<Real>> obj_sols;
						std::ifstream infile;
						std::stringstream os;
						os << "E:/PlatEMO/PF/data/" << num_samples[i] << "_points/" << num_obj[j] << "_obj/" << tmp_f << ".txt";
						infile.open(os.str());
						if (!infile) {
							throw MyExcept("open PF file is fail");
						}
						std::string str;
						size_t line = 0;
						while (getline(infile, str))
							++line;
						obj_sols.resize(line);
						infile.close();
						infile.clear();
						infile.open(os.str());
						size_t numobj = num_obj[j];
						for (size_t k = 0; k < line; k++) {
							std::vector<Real> temp_obj(numobj);
							for (size_t p = 0; p < numobj; p++) {
								infile >> temp_obj[p];
							}
							obj_sols[k] = temp_obj;
						}
						infile.close();

						//数据归一化
						auto temp_obj_sols = obj_sols;
						dataNormalize(obj_sols);

						ParameterMap params;
						params["algorithm name"] = "NSGAII-SBX";
						params["population size"] = int(obj_sols.size());
						params["maximum evaluations"] = int(100);
						params["number of variables"] = 8;
						params["kFactor"] = 2;
						params["lFactor"] = 2;
						params["problem name"] = (std::string)"MOP_DTLZ1";
						params["number of objectives"] = (int)numobj;

						int id_param = std::make_shared<const ParameterMap>(params);
						Problem *pro = Problem::generateByFactory(id_param, 0.1);
						pro->initialize();

						std::vector<OptimizeMode> opt_mode;
						for (size_t j = 0; j < numobj; ++j) {
							opt_mode.push_back(OptimizeMode::kMinimize);
						}

						if (i == num_samples.size() - 1 && hv < 0.) {
							std::vector<Real> hv_ref_points(numobj, 1.1);
							std::vector<Real> metrics;
							hv = hyperVolume(obj_sols, hv_ref_points, pro, pro);
						}
						/*****************************************************************/
						/*                           基于拥挤度选择
						/*****************************************************************/
						std::vector<std::vector<Real>> pop_select1;
						if (obj_sols.size() > num_output[ii]) {
							pop_select1.resize(num_output[ii]);
							NSGAII nsga(numobj, opt_mode);
							nsga.crowSelect(pop_select1, obj_sols);
							//pop_select1 = crowdSelect(obj_sols, num_output[ii]);
							/*****************************************************************/
							/*                           性能指标评价
							/*****************************************************************/
							std::vector<Real> hv_ref_points(numobj, 1.1);
							std::vector<Real> metrics;
							auto s1 = hyperVolume(pop_select1, hv_ref_points, pro, pro) / hv;
							auto s2 = Spacing(pop_select1);
							metrics.push_back(s1);
							metrics.push_back(s2);

							//将选择的个体保存
							std::vector<std::pair<Real, Real>> boundary;
							size_t num_p = temp_obj_sols.size();
							size_t num_d = temp_obj_sols[0].size();
							for (size_t i = 0; i < num_d; ++i) {
								Real min_v = 1 * 10e14;
								Real max_v = -1 * 10e14;
								for (size_t j = 0; j < num_p; ++j) {
									if (temp_obj_sols[j][i] < min_v)
										min_v = temp_obj_sols[j][i];
									if (temp_obj_sols[j][i] > max_v)
										max_v = temp_obj_sols[j][i];
								}
								boundary.push_back(std::make_pair(min_v, max_v));
							}
							dataRestore(pop_select1, boundary);
							std::string file_name = "E:/Gitlab/OFEC/result/PF/crowded_pops/" + std::to_string(num_samples[i]) + "_points/" \
								+ std::to_string(num_output[ii]) + "_output/" + std::to_string(num_obj[j]) + "_obj/" + tmp_f + ".txt";
							std::ofstream out(file_name);
							if (out) {
								for (size_t i = 0; i < pop_select1.size(); ++i) {
									for (size_t j = 0; j < pop_select1[i].size(); ++j) {
										out << pop_select1[i][j] << " ";
									}
									out << std::endl;
								}
								out.close();
							}

							//性能指标写入文件
							std::string file_name2 = "E:/Gitlab/OFEC/result/PF/crowded_metrics/" + tmp_f + \
								"/" + std::to_string(num_samples[i]) + "_points/" + std::to_string(num_obj[j]) + ".txt";
							std::ofstream out2(file_name2, std::ios::app);
							if (out2) {
								for (auto& me : metrics) {
									out2 << me << " ";
								}
								out2 << std::endl;
								out2.close();
							}
						}
						DEL_PRO(pro);
						DEL_PARAM(id_param);
					}
				}
			}
		}
		std::cout << "finish one type" << std::endl;
	}
}

TEST_CASE("refvector_selection", "[refvector_selection]")
{


	std::vector<int> num_samples = {100,200,250,300,400,500,600,750,800,1000,1600,2000,3000,3200,4000,5000};//pop_size
	//std::vector<int> num_samples = { 100 };//pop_size
	std::vector<int> num_output1 = {10,20,40,50,80,100,105,150,200,300,400,500};
	std::vector<int> num_output2 = {40,50,80,100,150,200,300,400,500 };
	//std::vector<int> num_output = { 10 };
	std::vector<int> num_obj = { 2,3 };
	//std::vector<int> num_obj = {4,6,8,10,12,15 };

	std::vector<std::string> PF_files ={ "linear", "convex", "concave", "mix", "discontinue" };
	//std::vector<std::string> PF_files = { "convex" };

	for (const auto& tmp_f : PF_files) {
		for (size_t j = 0; j < num_obj.size(); ++j) {
			Real hv=-1.;
			std::vector<int> num_output;
			if (num_obj[j] > 3) {
				num_output = num_output2;
			}
			else {
				num_output = num_output1;
			}
			for (int i = num_samples.size() - 1; i >= 0; --i) {
				for (size_t ii = 0; ii < num_output.size(); ++ii) {
					if (num_output[ii] < num_samples[i]) {
						//先读数据
						//std::string pro_name = (std::string)"MOP_GLT1";
						std::vector<std::vector<Real>> obj_sols;
						std::ifstream infile;
						std::stringstream os;
						//os << g_working_dir << "/instance/problem/continuous/multi_objective/glt/data/" << pro_name << ".dat";
						os << "E:/PlatEMO/PF/data/" << num_samples[i] << "_points/" << num_obj[j] << "_obj/" << tmp_f << ".txt";
						infile.open(os.str());
						if (!infile) {
							throw MyExcept("open PF file is fail");
						}
						std::string str;
						size_t line = 0;
						while (getline(infile, str))
							++line;
						obj_sols.resize(line);
						infile.close();
						infile.clear();
						infile.open(os.str());
						size_t numobj = num_obj[j];
						for (size_t k = 0; k < line; k++) {
							std::vector<Real> temp_obj(numobj);
							for (size_t p = 0; p < numobj; p++) {
								infile >> temp_obj[p];
							}
							obj_sols[k] = temp_obj;
						}
						infile.close();

						//数据归一化
						auto temp_obj_sols = obj_sols;
						dataNormalize(obj_sols);

						ParameterMap params;
						params["algorithm name"] = "NSGAII-SBX";
						params["population size"] = int(obj_sols.size());
						params["maximum evaluations"] = int(100);
						params["number of variables"] = 8;
						params["kFactor"] = 2;
						params["lFactor"] = 2;
						params["problem name"] = (std::string)"MOP_DTLZ1";
						params["number of objectives"] = (int)numobj;

						int id_param = std::make_shared<const ParameterMap>(params);
						Problem *pro = Problem::generateByFactory(id_param, 0.1);
						pro->initialize();


						std::vector<OptimizeMode> opt_mode;
						for (size_t j = 0; j < numobj; ++j) {
							opt_mode.push_back(OptimizeMode::kMinimize);
						}

						if (i == num_samples.size() - 1 && hv < 0.) {
							std::vector<Real> hv_ref_points(numobj, 1.1);
							std::vector<Real> metrics;
							hv = hyperVolume(obj_sols, hv_ref_points, pro, pro);
						}

						/*****************************************************************/
						/*                           基于参考向量选择
						/*****************************************************************/
						std::vector<std::vector<Real>> pop_select2;
						if (obj_sols.size() > num_output[ii]) {
							//读取参考向量
							std::vector<std::vector<Real>> m_ref_vector;
							std::ifstream infile;
							std::stringstream os;
							os << "E:/PlatEMO/PF/refvector/" << num_obj[j] << "_obj/" << num_output[ii] << "_points.txt";
							infile.open(os.str());
							if (!infile) {
								throw MyExcept("open PF file is fail");
							}
							std::string str;
							size_t line = 0;
							while (getline(infile, str))
								++line;
							m_ref_vector.resize(line);
							infile.close();
							infile.clear();
							infile.open(os.str());
							for (size_t k = 0; k < line; k++) {
								std::vector<Real> temp_obj(numobj);
								for (size_t p = 0; p < numobj; p++) {
									infile >> temp_obj[p];
								}
								m_ref_vector[k] = temp_obj;
							}
							infile.close();

							//根据参考向量选择
							pop_select2.resize(m_ref_vector.size());
							MOEAD<Solution<>> moead(numobj);
							//moead.initUniformWeight(obj_sols.size(),pro);
							pop_select2 = moead.selectByVector(m_ref_vector, obj_sols, pop_select2.size(), pro);

							/*****************************************************************/
							/*                           性能指标评价
							/*****************************************************************/
							std::vector<Real> hv_ref_points(numobj, 1.1);
							std::vector<Real> metrics;
							auto s1 = hyperVolume(pop_select2, hv_ref_points, pro, pro) / hv;
							auto s2 = Spacing(pop_select2);
							metrics.push_back(s1);
							metrics.push_back(s2);

							//将选择的个体保存
							std::vector<std::pair<Real, Real>> boundary;
							size_t num_p = temp_obj_sols.size();
							size_t num_d = temp_obj_sols[0].size();
							for (size_t i = 0; i < num_d; ++i) {
								Real min_v = 1 * 10e14;
								Real max_v = -1 * 10e14;
								for (size_t j = 0; j < num_p; ++j) {
									if (temp_obj_sols[j][i] < min_v)
										min_v = temp_obj_sols[j][i];
									if (temp_obj_sols[j][i] > max_v)
										max_v = temp_obj_sols[j][i];
								}
								boundary.push_back(std::make_pair(min_v, max_v));
							}
							dataRestore(pop_select2, boundary);
							std::string file_name = "E:/Gitlab/OFEC/result/PF/refvector_pops/" + std::to_string(num_samples[i]) + "_points/" \
								+ std::to_string(num_output[ii]) + "_output/" + std::to_string(num_obj[j]) + "_obj/" + tmp_f + ".txt";
							std::ofstream out(file_name);
							if (out) {
								for (size_t i = 0; i < pop_select2.size(); ++i) {
									for (size_t j = 0; j < pop_select2[i].size(); ++j) {
										out << pop_select2[i][j] << " ";
									}
									out << std::endl;
								}
								out.close();
							}


							//性能指标写入文件
							std::string file_name2 = "E:/Gitlab/OFEC/result/PF/refvector_metrics/" + tmp_f + \
								"/" + std::to_string(num_samples[i]) + "_points/" + std::to_string(num_obj[j]) + ".txt";
							std::ofstream out2(file_name2, std::ios::app);
							if (out2) {
								for (auto& me : metrics) {
									out2 << me << " ";
								}
								out2 << std::endl;
								out2.close();
							}
						}
						DEL_PRO(pro);
						DEL_PARAM(id_param);
					}
				}
			}
		}
		std::cout << "finish one type" << std::endl;
	}
}

TEST_CASE("subspace_selection", "[subspace_selection]")
{


	std::vector<int> num_samples = {100,200,250,300,400,500,600,750,800,1000,1600,2000,3000,3200,4000,5000};//pop_size
	//std::vector<int> num_samples = { 100 };//pop_size
	std::vector<int> num_output1 = {10,20,40,50,80,100,105,150,200,300,400,500};
	std::vector<int> num_output2 = { 40,50,80,100,105,150,200,300,400,500 };
	//std::vector<int> num_output = { 10};
	std::vector<int> num_obj = { 2,3};
	//std::vector<int> num_obj = { 2,3,4,6,8,10,12,15};

	std::vector<int> num_subspaces = { 10,20,40,80,160,320,640 };

	std::vector<std::string> PF_files ={ "linear", "convex", "concave", "mix", "discontinue" };
	//std::vector<std::string> PF_files = { "convex" };

	for (const auto& tmp_f : PF_files) {
		for (size_t j = 0; j < num_obj.size(); ++j) {
			Real hv=-1.;
			std::vector<int> num_output;
			if (num_obj[j] > 3) {
				num_output = num_output2;
			}
			else {
				num_output = num_output1;
			}
			for (int i = num_samples.size() - 1; i >= 0; --i) {
				for (size_t ii = 0; ii < num_output.size(); ++ii) {
					if (num_output[ii] < num_samples[i]) {
						for (size_t jj = 0; jj < num_subspaces.size(); ++jj) {
							//先读数据
							std::vector<std::vector<Real>> obj_sols;
							std::ifstream infile;
							std::stringstream os;
							os << "E:/PlatEMO/PF/data/" << num_samples[i] << "_points/" << num_obj[j] << "_obj/" << tmp_f << ".txt";
							infile.open(os.str());
							if (!infile) {
								throw MyExcept("open PF file is fail");
							}
							std::string str;
							size_t line = 0;
							while (getline(infile, str))
								++line;
							obj_sols.resize(line);
							infile.close();
							infile.clear();
							infile.open(os.str());
							size_t numobj = num_obj[j];
							for (size_t k = 0; k < line; k++) {
								std::vector<Real> temp_obj(numobj);
								for (size_t p = 0; p < numobj; p++) {
									infile >> temp_obj[p];
								}
								obj_sols[k] = temp_obj;
							}
							infile.close();

							//数据归一化
							auto temp_obj_sols = obj_sols;
							dataNormalize(obj_sols);

							ParameterMap params;
							params["algorithm name"] = "NSGAII-SBX";
							params["population size"] = int(obj_sols.size());
							params["maximum evaluations"] = int(100);
							params["number of variables"] = 8;
							params["kFactor"] = 2;
							params["lFactor"] = 2;
							params["problem name"] = (std::string)"MOP_DTLZ1";
							params["number of objectives"] = (int)numobj;

							int id_param = std::make_shared<const ParameterMap>(params);
							Problem *pro = Problem::generateByFactory(id_param, 0.1);
							pro->initialize();


							std::vector<OptimizeMode> opt_mode;
							for (size_t j = 0; j < numobj; ++j) {
								opt_mode.push_back(OptimizeMode::kMinimize);
							}

							if (i == num_samples.size() - 1 && hv < 0.) {
								std::vector<Real> hv_ref_points(numobj, 1.1);
								std::vector<Real> metrics;
								hv = hyperVolume(obj_sols, hv_ref_points, pro, pro);
							}

							/*****************************************************************/
							/*                   基于子空间跨度选择的多参考点选择
							/*****************************************************************/
							//先根据目标值范围构造kd-tree
							std::vector<std::vector<Real>> pop_select3;
							size_t select_pop;
							if (obj_sols.size() > num_output[ii]) {
								select_pop=num_output[ii];
								std::vector<std::pair<Real, Real>> boundary(numobj);
								for (size_t p = 0; p < numobj; ++p) {
									Real min_v = 1.0 * 10e14;
									Real max_v = -1.0 * 10e14;
									for (size_t q = 0; q < obj_sols.size(); ++q) {
										if (min_v > obj_sols[q][p]) {
											min_v = obj_sols[q][p];
										}
										if (max_v < obj_sols[q][p]) {
											max_v = obj_sols[q][p];
										}
									}
									boundary[p].first = min_v;
									boundary[p].second = max_v;
								}
								size_t num_genes = num_subspaces[jj];//20,40,80,160,320,640
								size_t size_var = 10;
								//uniform rand(0.5);
								nanoflann::PartitioningKDTree<Real> obj_tree(numobj, 2);
								obj_tree.setInitBox(boundary);
								obj_tree.inputRatioData(std::vector<Real>(num_genes, 1.0 / num_genes));
								obj_tree.buildIndex();

								//将个体存入子空间，形成映射
								std::map<size_t, std::vector<size_t>> ind_subspace;
								for (size_t p = 0; p < obj_sols.size(); ++p) {
									size_t ind = obj_tree.getRegionIdx(obj_sols[p]);
									if (!ind_subspace.empty() && ind_subspace.find(ind) != ind_subspace.end()) {
										ind_subspace[ind].push_back(p);
									}
									else {
										std::vector<size_t> inx;
										inx.push_back(p);
										ind_subspace.emplace(std::make_pair(ind, inx));
									}
								}

								//计算每个子空间的跨度
								std::vector<Real> subspace_span;
								for (const auto& subspace : ind_subspace) {
									std::vector<Real> ideal_point;
									std::vector<Real> nadir_point;
									auto sub_boundary = obj_tree.getBox(subspace.first);
									for (size_t j = 0; j < sub_boundary.size(); ++j) {
										ideal_point.push_back(sub_boundary[j].first);
										nadir_point.push_back(sub_boundary[j].second);
									}
									std::vector<size_t> points_idx = subspace.second;
									std::vector<std::vector<Real>> sub_points;
									for (size_t q = 0; q < points_idx.size(); ++q) {
										sub_points.push_back(obj_sols[points_idx[q]]);
									}
									subspace_span.push_back(calSubspaceSpan(ideal_point, nadir_point, sub_points));
								}

								//根据种群数为子空间分配存活个体数
								std::vector<size_t> subspace_ind_num;
								Real sum_span = 0.;
								for (size_t p = 0; p < subspace_span.size(); ++p) {
									sum_span += subspace_span[p];
								}
								for (size_t p = 0; p < subspace_span.size(); ++p) {
									subspace_ind_num.push_back(1 + std::ceil(select_pop * subspace_span[p] / sum_span));
								}

								//在子空间进行个体存活选择
								size_t index = 0;
								std::vector<size_t> select_index;
								for (const auto& subspace : ind_subspace) {
									std::vector<Real> anchor_point;
									auto sub_boundary = obj_tree.getBox(subspace.first);
									for (size_t p = 0; p < sub_boundary.size(); ++p) {
										anchor_point.push_back(sub_boundary[p].first);
									}
									std::vector<size_t> points_idx = subspace.second;
									std::vector<std::vector<Real>> sub_points;
									for (size_t p = 0; p < points_idx.size(); ++p) {
										sub_points.push_back(obj_sols[points_idx[p]]);
									}
									select_index = selectIndSubspace(anchor_point, sub_points, subspace_ind_num[index]);
									for (size_t p = 0; p < select_index.size(); ++p) {
										pop_select3.push_back(sub_points[select_index[p]]);
									}
									index++;
								}

								//最后根据拥挤度去掉多余的最拥挤的个体,边界个体设置最大的拥挤距离
								while (pop_select3.size() > select_pop)
								{
									//先找出子目标极值个体
									std::vector<size_t> sub_optima_inx;
									for (size_t p = 0; p < numobj; ++p) {
										size_t inx = 0;
										Real max_v = -1. * 10e14;
										for (size_t q = 0; q < pop_select3.size(); ++q) {
											if (pop_select3[q][p] > max_v) {
												max_v = pop_select3[q][p];
												inx = q;
											}
										}
										sub_optima_inx.push_back(inx);
									}
									//先计算个体的拥挤度
									std::vector<Real> density;
									for (size_t p = 0; p < pop_select3.size(); ++p) {
										Real min_dist = 1.e14;
										if (std::find(sub_optima_inx.begin(), sub_optima_inx.end(), p) == sub_optima_inx.end()) {
											for (size_t k = 0; k < pop_select3.size(); ++k) {
												if (p != k) {
													auto dist = euclideanDistance(pop_select3[p].begin(), pop_select3[p].end(), pop_select3[k].begin());
													if (dist < min_dist) {
														min_dist = dist;
													}
												}
											}
										}
										density.push_back(min_dist);
									}
									//再去掉最拥挤的
									size_t inx = 0;
									Real dist = 1.e14;
									for (size_t p = 0; p < density.size(); ++p) {
										if (density[p] < dist) {
											dist = density[p];
											inx = p;
										}
									}
									pop_select3.erase(pop_select3.begin() + inx);
								}

								/*************************************************************************/
								/*                              性能指标评价
								/*************************************************************************/
								std::vector<Real> hv_ref_points(numobj, 1.1);
								std::vector<Real> metrics;
								auto s1 = hyperVolume(pop_select3, hv_ref_points, pro, pro) / hv;
								auto s2 = Spacing(pop_select3);
								metrics.push_back(s1);
								metrics.push_back(s2);

								//将选择的个体保存
								std::vector<std::pair<Real, Real>> boundary2;
								size_t num_p = temp_obj_sols.size();
								size_t num_d = temp_obj_sols[0].size();
								for (size_t i = 0; i < num_d; ++i) {
									Real min_v = 1 * 10e14;
									Real max_v = -1 * 10e14;
									for (size_t j = 0; j < num_p; ++j) {
										if (temp_obj_sols[j][i] < min_v)
											min_v = temp_obj_sols[j][i];
										if (temp_obj_sols[j][i] > max_v)
											max_v = temp_obj_sols[j][i];
									}
									boundary2.push_back(std::make_pair(min_v, max_v));
								}

								dataRestore(pop_select3, boundary2);
								std::string file_name2 = "E:/Gitlab/OFEC/result/PF/subspace_pops/" + std::to_string(num_samples[i]) + "_points/" \
									+ std::to_string(num_output[ii]) + "_output/" + std::to_string(num_obj[j]) + "_obj/" + std::to_string(num_subspaces[jj]) + "_subspace/" + tmp_f + ".txt";
								std::ofstream out2(file_name2);
								if (out2) {
									for (size_t i = 0; i < pop_select3.size(); ++i) {
										for (size_t j = 0; j < pop_select3[i].size(); ++j) {
											out2 << pop_select3[i][j] << " ";
										}
										out2 << std::endl;
									}
									out2.close();
								}

								//性能指标写入文件
								std::string file_name = "E:/Gitlab/OFEC/result/PF/subspace_metrics/" + tmp_f + \
									"/" + std::to_string(num_samples[i]) + "_points/" + std::to_string(num_subspaces[jj]) + "_subspace/" + std::to_string(num_obj[j]) + ".txt";
								std::ofstream out(file_name, std::ios::app);
								if (out) {
									for (auto& me : metrics) {
										out << me << " ";
									}
									out << std::endl;
									out.close();
								}
							}
							DEL_PRO(pro);
							DEL_PARAM(id_param);
						}
					}
				}
			}
		}
		std::cout << "finish one type" << std::endl;
	}
}

TEST_CASE("MOP_selection", "[MOP_selection]")
{
	std::vector<std::string> alg_names = { "NSGAII-SBX","MOEAD-SBX" };


	std::vector<int> num_samples = { 250,500,750,1000,2000,3000,4000,5000 };//pop_size
	std::vector<int> num_output = { 100,105,150,200,300,500,1000 };
	std::vector<int> num_subspaces = { 20,40,80,160,320,640};
	std::vector<int> num_obj = {2,3};

	std::vector<std::string> PF_files =
	{ "linear", "convex", "concave", "mix", "discontinue"};

	for (const auto& tmp_f : PF_files) {
		for (size_t j = 0; j < num_obj.size(); ++j) {
			Real hv;
			for (int i = num_samples.size()-1; i >=0; --i) {
				//先读数据
				//std::string pro_name = (std::string)"MOP_GLT1";
				std::vector<std::vector<Real>> obj_sols;
				std::ifstream infile;
				std::stringstream os;
				//os << g_working_dir << "/instance/problem/continuous/multi_objective/glt/data/" << pro_name << ".dat";
				os << "E:/PlatEMO/PF/data/" << num_samples[i]<<"_points/"<<num_obj[j]<<"_obj/"<<tmp_f<<".txt";
				infile.open(os.str());
				if (!infile) {
					throw MyExcept("open PF file is fail");
				}
				std::string str;
				size_t line = 0;
				while (getline(infile, str))
					++line;
				obj_sols.resize(line);
				infile.close();
				infile.clear();
				infile.open(os.str());
				size_t numobj = num_obj[j];
				for (size_t k = 0; k < line; k++) {
					std::vector<Real> temp_obj(numobj);
					for (size_t p = 0; p < numobj; p++) {
						infile >> temp_obj[p];
					}
					obj_sols[k] = temp_obj;
				}
				infile.close();

				//数据归一化
				auto temp_obj_sols = obj_sols;
				dataNormalize(obj_sols);

				ParameterMap params;
				params["algorithm name"] = "NSGAII-SBX";
				params["population size"] = int(obj_sols.size());
				params["maximum evaluations"] = int(100);
				params["number of variables"] = 8;
				params["kFactor"] = 2;
				params["lFactor"] = 2;
				params["problem name"] = (std::string)"MOP_DTLZ1";
				params["number of objectives"] = (int)numobj;

				int id_param = std::make_shared<const ParameterMap>(params);
				Problem *pro = Problem::generateByFactory(id_param, 0.1);
				pro->initialize();

				
				std::vector<OptimizeMode> opt_mode;
				for (size_t j = 0; j < numobj; ++j) {
					opt_mode.push_back(OptimizeMode::kMinimize);
				}

				if (i == num_samples.size() - 1) {
					std::vector<Real> hv_ref_points(numobj, 1.1);
					std::vector<Real> metrics;
					hv = hyperVolume(obj_sols, hv_ref_points, pro, pro);
				}


				/*****************************************************************/
				/*                           基于拥挤度选择
				/*****************************************************************/
				std::vector<std::vector<Real>> pop_select1;
				if (numobj == 2) {
					pop_select1.resize(num_output[0]);
				}
				else {
					pop_select1.resize(num_output[1]);
				}
				NSGAII nsga(numobj, opt_mode);
				nsga.crowSelect(pop_select1, obj_sols);

				/*****************************************************************/
				/*                           基于参考向量选择
				/*****************************************************************/
				std::vector<std::vector<Real>> pop_select2;
				std::vector<std::vector<Real>> m_ref_vector;
				if (numobj == 2) {
					pop_select2.resize(num_output[0]);
				}
				else {
					pop_select2.resize(num_output[1]);
				}
				MOEAD<Solution<>> moead(numobj);
				//moead.initUniformWeight(obj_sols.size(),pro);
				pop_select2 = moead.selectByVector(m_ref_vector, obj_sols, pop_select2.size(), pro);

				/*****************************************************************/
				/*                   基于子空间跨度选择的多参考点选择
				/*****************************************************************/
				//先根据目标值范围构造kd-tree
				std::vector<std::vector<Real>> pop_select3;
				size_t select_pop;
				if (numobj == 2) {
					select_pop=num_output[0];
				}
				else {
					select_pop=num_output[1];
				}
				std::vector<std::pair<Real, Real>> boundary(numobj);
				for (size_t p = 0; p < numobj; ++p) {
					Real min_v = 1.0 * 10e14;
					Real max_v = -1.0 * 10e14;
					for (size_t q = 0; q < obj_sols.size(); ++q) {
						if (min_v > obj_sols[q][p]) {
							min_v = obj_sols[q][p];
						}
						if (max_v < obj_sols[q][p]) {
							max_v = obj_sols[q][p];
						}
					}
					boundary[p].first = min_v;
					boundary[p].second = max_v;
				}
				size_t num_genes = num_subspaces[2];//20,40,80,160,320,640
				size_t size_var = 10;
				//uniform rand(0.5);
				nanoflann::PartitioningKDTree<Real> obj_tree(numobj, 2);
				obj_tree.setInitBox(boundary);
				obj_tree.inputRatioData(std::vector<Real>(num_genes, 1.0 / num_genes));
				obj_tree.buildIndex();

				//将个体存入子空间，形成映射
				std::map<size_t, std::vector<size_t>> ind_subspace;
				for (size_t p = 0; p < obj_sols.size(); ++p) {
					size_t ind = obj_tree.getRegionIdx(obj_sols[p]);
					if (!ind_subspace.empty() && ind_subspace.find(ind) != ind_subspace.end()) {
						ind_subspace[ind].push_back(p);
					}
					else {
						std::vector<size_t> inx;
						inx.push_back(p);
						ind_subspace.emplace(std::make_pair(ind, inx));
					}
				}

				//计算每个子空间的跨度
				std::vector<Real> subspace_span;
				for (const auto& subspace : ind_subspace) {
					std::vector<Real> ideal_point;
					std::vector<Real> nadir_point;
					auto sub_boundary = obj_tree.getBox(subspace.first);
					for (size_t j = 0; j < sub_boundary.size(); ++j) {
						ideal_point.push_back(sub_boundary[j].first);
						nadir_point.push_back(sub_boundary[j].second);
					}
					std::vector<size_t> points_idx = subspace.second;
					std::vector<std::vector<Real>> sub_points;
					for (size_t q = 0; q < points_idx.size(); ++q) {
						sub_points.push_back(obj_sols[points_idx[q]]);
					}
					subspace_span.push_back(calSubspaceSpan(ideal_point, nadir_point, sub_points));
				}

				//根据种群数为子空间分配存活个体数
				std::vector<size_t> subspace_ind_num;
				Real sum_span = 0.;
				for (size_t p = 0; p < subspace_span.size(); ++p) {
					sum_span += subspace_span[p];
				}
				for (size_t p = 0; p < subspace_span.size(); ++p) {
					subspace_ind_num.push_back(std::ceil(num_output[2] * subspace_span[p] / sum_span));
				}

				//在子空间进行个体存活选择
				size_t index = 0;
				std::vector<size_t> select_index;
				for (const auto& subspace : ind_subspace) {
					std::vector<Real> anchor_point;
					auto sub_boundary = obj_tree.getBox(subspace.first);
					for (size_t p = 0; p < sub_boundary.size(); ++p) {
						anchor_point.push_back(sub_boundary[p].first);
					}
					std::vector<size_t> points_idx = subspace.second;
					std::vector<std::vector<Real>> sub_points;
					for (size_t p = 0; p < points_idx.size(); ++p) {
						sub_points.push_back(obj_sols[points_idx[p]]);
					}
					select_index = selectIndSubspace(anchor_point, sub_points, subspace_ind_num[index]);
					for (size_t p = 0; p < select_index.size(); ++p) {
						pop_select3.push_back(sub_points[select_index[p]]);
					}
					index++;
				}
				//最后根据拥挤度去掉多余的最拥挤的个体
				while (pop_select3.size() > select_pop)
				{
					//先计算个体的拥挤度
					std::vector<Real> density;
					for (size_t p = 0; p < pop_select3.size(); ++p) {
						Real min_dist = 1.e14;
						for (size_t k = 0; k < pop_select3.size(); ++k) {
							if (p != k) {
								auto dist = euclideanDistance(pop_select3[p].begin(), pop_select3[p].end(), pop_select3[k].begin());
								if (dist < min_dist) {
									min_dist = dist;
								}
							}
						}
						density.push_back(min_dist);
					}
					//再去掉最拥挤的
					size_t inx = 0;
					Real dist = 1.e14;
					for (size_t p = 0; p < density.size(); ++p) {
						if (density[p] < dist) {
							dist = density[p];
							inx =p;
						}
					}
					pop_select3.erase(pop_select3.begin() + inx);
				}

				//性能指标评价
				std::vector<Real> hv_ref_points(numobj, 1.1);
				std::vector<Real> metrics;
				//Real hv= hyperVolume(obj_sols, hv_ref_points, pro, pro);
				auto s1 = hyperVolume(pop_select1, hv_ref_points, pro, pro)/hv;
				auto s2 = hyperVolume(pop_select2, hv_ref_points, pro, pro)/hv;
				auto s3 = hyperVolume(pop_select3, hv_ref_points, pro, pro)/hv;
				auto s4 = Spacing(pop_select1);
				auto s5 = Spacing(pop_select2);
				auto s6 = Spacing(pop_select3);
				metrics.push_back(s1);
				metrics.push_back(s2);
				metrics.push_back(s3);
				metrics.push_back(s4);
				metrics.push_back(s5);
				metrics.push_back(s6);

				////将选择的个体保存
				//std::vector<std::pair<Real, Real>> boundary;
				//size_t num_p = temp_obj_sols.size();
				//size_t num_d = temp_obj_sols[0].size();
				//for (size_t i = 0; i < num_d; ++i) {
				//	Real min_v = 1 * 10e14;
				//	Real max_v = -1 * 10e14;
				//	for (size_t j = 0; j < num_p; ++j) {
				//		if (temp_obj_sols[j][i] < min_v)
				//			min_v = temp_obj_sols[j][i];
				//		if (temp_obj_sols[j][i] > max_v)
				//			max_v = temp_obj_sols[j][i];
				//	}
				//	boundary.push_back(std::make_pair(min_v,max_v));
				//}
				//dataRestore(pop_select1, boundary);
				//dataRestore(pop_select2, boundary);
				//dataRestore(pop_select3, boundary);
				//std::string file_name = "E:/Gitlab/OFEC/result/PF/pops/" + tmp_f + "/" + std::to_string(numobj) + ".txt";
				//std::ofstream out(file_name, std::ios::app);
				//if (out) {
				//	for (auto& me : metrics) {
				//		out << me << " ";
				//	}
				//	out << std::endl;
				//	out.close();
				//}


				//性能指标写入文件
			    std::string file_name = "E:/Gitlab/OFEC/result/PF/metrics/" + tmp_f +"/" + std::to_string(numobj) + ".txt";
		        std::ofstream out(file_name,std::ios::app);
				if (out) {
					for (auto &me:metrics) {
						out << me << " ";
					}
					out << std::endl;
					out.close();
				}

				DEL_PRO(pro);
				DEL_PARAM(id_param);
			}
		}
		std::cout << "finish one type" << std::endl;
	}
}
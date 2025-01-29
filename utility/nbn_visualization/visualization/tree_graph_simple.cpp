#include "tree_graph_simple.h"
#include "../../../core/problem/solution.h"

namespace ofec {
	void TreeGraphSimple::ConProMeshSample::initialize(const std::shared_ptr<Environment>& env, int numDiv) {
		m_environnment = env;
		auto pro = m_environnment->problem();
		m_numDiv = numDiv;
		m_maxSample = m_numDiv * m_numDiv;
		ofec::Solution<> curSol(pro->numberObjectives(), pro->numberConstraints(), pro->numberVariables());
		m_samples.resize(m_maxSample,curSol);
		m_attributes.resize(m_maxSample);
		for (auto& it : m_attributes) it.resize(m_maxAtri);
		m_pos_idx.resize(m_maxSample);

		int curId = 0;
		for (int idx(0); idx < numDiv; ++idx) {
			for (int idy(0); idy < numDiv; ++idy) {
				m_pos_idx[curId++] = { idx,idy };
			}
		}
	}
	void TreeGraphSimple::ConProMeshSample::sampleMultiThread() {
		std::vector<std::thread> thrds;
		int num_task = std::thread::hardware_concurrency();
		int num_samples = m_maxSample;
		std::vector<int> tasks;
		UTILITY::assignThreads(num_samples, num_task, tasks);
		std::pair<int, int> from_to;
		for (size_t i = 0; i < num_task; ++i) {
			from_to.first = tasks[i];
			from_to.second = tasks[i + 1];
			thrds.push_back(std::thread(
				&TreeGraphSimple::ConProMeshSample::sampleThreadTask, this,
				from_to.first, from_to.second,0));
		}
		for (auto& thrd : thrds)
			thrd.join();
	}

	void TreeGraphSimple::ConProMeshSample::sampleThreadTask(int from, int to, int updateTag) {
		auto pro = m_environnment->problem();
		ofec::Real minFlag = (pro->optimizeMode(0) == ofec::OptimizeMode::kMaximize) ? 1 : -1;
		const auto& boundary = CAST_CONOP(pro)->boundary();
		std::array<double, 2> pro_range;
		for (int idDim(0); idDim < 2; ++idDim) {
			pro_range[idDim] = boundary[idDim].second - boundary[idDim].first;
		}
		int Dim = pro->numberVariables();
	//	ofec::Solution<> curSol(pro->numObjectives(), pro->numConstraints(), pro->numVariables());
		for (int id(from); id < to; ++id) {
			auto& curSol(m_samples[id]);
			if (updateTag == 0) {
				auto& curpos = m_pos_idx[id];
				curSol.variable().resize(Dim);
				curSol.resizeObjective(pro->numberObjectives());
				curSol.resizeConstraint(pro->numberConstraints());
				//auto& meshPos = mesh[id];
				for (int idDim(0); idDim < Dim; ++idDim) {
					curSol.variable()[idDim] = double(curpos[idDim]) / double(m_numDiv) * pro_range[idDim] + boundary[idDim].first;
				}
			}
			
			curSol.evaluate(m_environnment.get());

			if (updateTag == 0) {
				m_attributes[id][static_cast<int>(TreeGraphSimple::ConProMeshSample::EAtri::kFit)] = curSol.objective()[0] * minFlag;
				double con = 1.0;
				if (!curSol.isFeasible()) con = -1;
				m_attributes[id][static_cast<int>(TreeGraphSimple::ConProMeshSample::EAtri::kCon)] = con;
			}
			else if (updateTag == 1) {
				m_attributes[id][static_cast<int>(TreeGraphSimple::ConProMeshSample::EAtri::kDynamic)] = curSol.objective()[0] * minFlag;

			}

		}
	}
	void TreeGraphSimple::ConProMeshSample::evaluateMutliThread() {
		std::vector<std::thread> thrds;
		int num_task = std::thread::hardware_concurrency();
		int num_samples = m_maxSample;
		std::vector<int> tasks;
		UTILITY::assignThreads(num_samples, num_task, tasks);
		std::pair<int, int> from_to;
		for (size_t i = 0; i < num_task; ++i) {
			from_to.first = tasks[i];
			from_to.second = tasks[i + 1];
			thrds.push_back(std::thread(
				&TreeGraphSimple::ConProMeshSample::sampleThreadTask, this,
				from_to.first, from_to.second, 1));
		}
		for (auto& thrd : thrds)
			thrd.join();
	}

	void mesh2DproblemMutlithread(Environment *env, int numDiv,
		const std::vector<std::array<int, 2>>& pos,
		std::vector<TreeGraphSimple::Pos>& mesh, int from, int to) {
		auto pro = env->problem();
		ofec::Real minFlag = (pro->optimizeMode(0) == ofec::OptimizeMode::kMaximize) ? 1 : -1;
		const auto& boundary = CAST_CONOP(pro)->boundary();
		std::array<double, 2> pro_range;
		for (int idDim(0); idDim < 2; ++idDim) {
			pro_range[idDim] = boundary[idDim].second - boundary[idDim].first;
		}
		ofec::Solution<> curSol(pro->numberObjectives(), pro->numberConstraints(), pro->numberVariables());
		for (int id(from); id < to; ++id) {
			auto& curpos = pos[id];
			auto& meshPos = mesh[id];
			for (int idDim(0); idDim < 2; ++idDim) {
				curSol.variable()[idDim] = double(curpos[idDim]) / double(numDiv) * pro_range[idDim] + boundary[idDim].first;
			}
			curSol.evaluate(env);
			for (int idDim(0); idDim < 2; ++idDim) {
				meshPos[idDim] = curSol.variable()[idDim];
			}

			meshPos[2] = curSol.objective()[0] * minFlag;

		}
	}

	void mesh2Dproblem(
		Environment *env, int numDiv,
		std::vector<std::array<int, 2>>& pos,
		std::vector<TreeGraphSimple::Pos>& mesh) {
		pos.clear();
		mesh.clear();
		if (env->problem()->numberVariables() != 2)return;
		int totalsample = numDiv * numDiv;
		pos.resize(totalsample);
		mesh.resize(totalsample);
		int curId(0);
		const auto& boundary = CAST_CONOP(env->problem())->boundary();

		for (int idx(0); idx < numDiv; ++idx) {
			for (int idy(0); idy < numDiv; ++idy) {
				pos[curId++] = { idx,idy };
			}
		}

		std::vector<std::thread> thrds;
		int num_task = std::thread::hardware_concurrency();
		int num_samples = totalsample;
		std::vector<int> tasks;
		UTILITY::assignThreads(num_samples, num_task, tasks);
		std::pair<int, int> from_to;
		for (size_t i = 0; i < num_task; ++i) {
			from_to.first = tasks[i];
			from_to.second = tasks[i + 1];
			thrds.push_back(std::thread(
				mesh2DproblemMutlithread, env,numDiv,
				std::ref(pos),std::ref(mesh),
				from_to.first, from_to.second));
		}
		for (auto& thrd : thrds)
			thrd.join();
	}



	void ouputMesh(std::ostream& out,
		const std::vector<std::array<int, 2>>& pos,
		const std::vector<TreeGraphSimple::Pos>& mesh
	) {
		std::stringstream buf;
		int dim(3);
		for (int idx(0); idx < pos.size(); ++idx) {
			buf << pos[idx].front() << "\t" << pos[idx].back();
			for (int idDim(0); idDim < dim; ++idDim) {
				buf << "\t" << mesh[idx][idDim];
			}
			buf << std::endl;
		}


		std::string buf_str = buf.str();
		out.write(buf_str.c_str(), buf_str.size());
	}


	void outputVectorIdx(std::ostream& out, const std::vector<std::vector<double>>& data) {
		std::stringstream buf;
		for (int idx(0); idx < data.size(); ++idx) {
			buf << idx;
			for (auto& it : data[idx]) {
				buf << "\t" << it;
			}
			buf << std::endl;
		}

		std::string buf_str = buf.str();
		out.write(buf_str.c_str(), buf_str.size());

	}


	void inputVectorIdx(std::istream& in,
		std::vector<std::vector<double>>& data) {
		std::stringstream buf;
		buf << in.rdbuf();

		std::vector<double> cur_data;

		{
			double curVal(0);
			std::string curLine;
			std::getline(buf, curLine);
			std::stringstream tmpbuf;
			tmpbuf << curLine;
			while (tmpbuf >> curVal) {
				cur_data.push_back(curVal);
			}
		}
		int dim = 0;
		data.push_back(cur_data);
		while (buf >> cur_data[dim]) {
			++dim;
			if (dim == cur_data.size()) {
				data.push_back(cur_data);
				dim = 0;
			}
		}


	}

	void ouputMeshPoints(std::ostream& out,
		Environment *env,
		const std::vector<std::shared_ptr<ofec::SolutionBase>>& sols
	) {

		std::stringstream buf;
		ofec::Real minFlag = (env->problem()->optimizeMode(0) == ofec::OptimizeMode::kMaximize) ? 1 : -1;
		for (int idx(0); idx < sols.size(); ++idx) {
			auto& conSol = dynamic_cast<Solution<>&>(*sols[idx]);
			buf << idx<<"\t" << conSol.variable()[0] << "\t" << conSol.variable()[1] << "\t" << conSol.objective()[0] * minFlag << std::endl;
		}

		std::string buf_str = buf.str();
		out.write(buf_str.c_str(), buf_str.size());

		
	}

	void readFileBuffer(const std::string& filepath, std::stringstream& buf) {
		//clock_t start = clock();
		std::ifstream fin(filepath);
		//stringstream buf;
		buf << fin.rdbuf();
		fin.close();

	}

	void inputMeshPoints(const std::string& filepath,
		std::vector<std::array<double, 3>>& pos) {
		std::stringstream buf;
		readFileBuffer(filepath, buf);
		std::array<double, 3> curpos;
		int id(0);
	    while(buf >>id>> curpos[0] >> curpos[1] >> curpos[2]){
			pos.push_back(curpos);
		}

	}


	//void setNBNdata(const std::vector<NBNdata>& nbn_data) {
	//	m_nbn_data = nbn_data;
	//}


	void TreeGraphSimple::inputNBNnetwork(const std::string& filepath,
		std::vector<Pos>& nbn_pos,
		std::vector<int>& belongs
	) {

		std::stringstream buf;
		readFileBuffer(filepath, buf);
		std::string line;
		std::getline(buf, line);
		TreeGraphSimple::NBNdata cur_data;
		Pos curPos;
		int belong, id(-1);
		while (buf >> id >> curPos[0] >> curPos[1] >> curPos[2] >> belong) {
			nbn_pos.push_back(curPos);
			belongs.push_back(belong);
			// for test
			//break;
		}
	}


	void TreeGraphSimple::inputNBNnetworkColor(const std::string& filepath,
		std::vector<Pos>& nbn_pos,
		std::vector<int>& belongs,
		std::vector<double>& fitness) {
		std::stringstream buf;
		readFileBuffer(filepath, buf);
		std::string line;
		std::getline(buf, line);
		TreeGraphSimple::NBNdata cur_data;
		Pos curPos;
		int belong, id(-1);
		double color;
		while (buf >> id >> curPos[0] >> curPos[1] >> curPos[2] >> belong>>color) {
			nbn_pos.push_back(curPos);
			belongs.push_back(belong);
			fitness.push_back(color);
			// for test
			//break;
		}
	}


	void inputNBNdata(const std::string& filepath,
		std::vector<TreeGraphSimple::NBNdata>& nbn_data) {
		std::stringstream buf;
		readFileBuffer(filepath, buf);
		std::string line;
		std::getline(buf, line);
		TreeGraphSimple::NBNdata cur_data;
		while (cur_data.input(buf)) {
			nbn_data.push_back(cur_data);
		}
	}

	void ouputNBNdata(const std::string& filepath,
		const std::vector<TreeGraphSimple::NBNdata>& nbn_data) {
		std::stringstream buf;
		TreeGraphSimple::NBNdata::outputHead(buf);
		for (auto& it : nbn_data) {
			it.output(buf);
		}
		std::ofstream out(filepath);
		std::string buf_str = buf.str();
		out.write(buf_str.c_str(), buf_str.size());
		out.close();
	}


	void outputNBNdata(
		std::ostream& out,
		const std::vector<int>& solId,
		const std::vector<int>& belong,
		const std::vector<double>& dis2parent,
		const std::vector<double>& fitness
	) {

		std::stringstream buf;
		buf << "CurId\tOrigin\tBelong\tDis2parent\tFitness" << std::endl;
		for (int idx(0); idx < solId.size(); ++idx) {
			buf << idx << "\t" << solId[idx] << "\t" << belong[idx] << "\t" << dis2parent[idx] <<
				"\t" << fitness[idx] << std::endl;
		}
		std::string buf_str = buf.str();
		out.write(buf_str.c_str(), buf_str.size());
	}



	void transferNBNData(
		std::vector<TreeGraphSimple::NBNdata>& nbn_data
		, const std::vector<int>& solId,
		const std::vector<int>& belong,
		const std::vector<double>& dis2parent,
		const std::vector<double>& fitness) {
		nbn_data.resize(solId.size());
		for (int idx(0); idx < solId.size(); ++idx) {
			nbn_data[idx].m_id = idx;
			nbn_data[idx].m_originId = solId[idx];
			nbn_data[idx].m_belong = belong[idx];
			nbn_data[idx].m_dis2parent = dis2parent[idx];
			nbn_data[idx].m_fitness = fitness[idx];
		}
	}


	void cutNBNData(
		const std::vector<ofec::TreeGraphSimple::NBNdata>& lon_data,
		const std::array<std::array<double, 2>, 2>& boundary,
		const std::vector<int>& idxs,
		std::vector<double>& radius,
		double cut_radius,
		int min_div
	) {

		if (cut_radius < 1e-3) return;

		std::array<double, 2> boundary_div;
		for (int idx(0); idx < 2; ++idx) {
			boundary_div[idx] = (boundary[idx][1] - boundary[idx][0]) / 2.0;
		}

		using Boundary_Type = std::array<std::array<double, 2>, 2>;

		std::array< Boundary_Type, 4> sub_boundary;
		std::array<std::vector<int>, 4> sub_idxs;
		//std::array<int, 2> cur_id;

		for (int idx(0); idx < 2; ++idx) {
			for (int idy(0); idy < 2; ++idy) {
				int id = idx * 2 + idy;
				auto& subB = sub_boundary[id];
				subB[0][0] = boundary[0][0] + idx * boundary_div[0];
				subB[0][1] = boundary[0][1] - (1 - idx) * boundary_div[0];

				subB[1][0] = boundary[1][0] + idy * boundary_div[1];
				subB[1][1] = boundary[1][1] - (1 - idy) * boundary_div[1];
			}
		}

		{
			int idx(0), idy(0);
			for (auto& id : idxs) {
				auto& curlon = lon_data[id];
				idx = (lon_data[id].m_fitness - boundary[0][0]) / boundary_div[0];
				idy = (lon_data[id].m_dis2parent - boundary[1][0]) / boundary_div[1];
				if (idx == 2)idx--;
				if (idy == 2)idy--;
				int sub_id = idx * 2 + idy;

				sub_idxs[sub_id].push_back(id);
			}
		}

		cut_radius = cut_radius / 2.0;
		double cur_radius = 0;
		bool flag_cut = false;
		for (int idx(0); idx < 4; ++idx) {
			flag_cut = false;
			if (sub_idxs[idx].size() <= min_div && !sub_idxs[idx].empty()) {
				cur_radius = cut_radius / double(sub_idxs[idx].size());
				for (auto& it : sub_idxs[idx]) {
					radius[it] = std::max(radius[it], cur_radius);
					if (radius[it] < cut_radius) {
						flag_cut = true;
					}
				}
			}
			else if (!sub_idxs.empty()) flag_cut = true;
			if (flag_cut) {
				cutNBNData(
					lon_data, sub_boundary[idx], sub_idxs[idx],
					radius, cut_radius,
					min_div);
			}
		}

	}

	void calNBNDensity(const std::vector<ofec::TreeGraphSimple::NBNdata> & lon_data2,
		std::vector<double>& density) {

		std::array<std::array<double, 2>, 2> boundary_div;
		for (int idx(0); idx < 2; ++idx) {
			boundary_div[idx][0] = 1e9;
			boundary_div[idx][1] = -1e9;
		}
		for (auto& it : lon_data2) {
			boundary_div[0][0] = std::min(boundary_div[0][0], it.m_fitness);
			boundary_div[0][1] = std::max(boundary_div[0][1], it.m_fitness);
			boundary_div[1][0] = std::min(boundary_div[1][0], it.m_dis2parent);
			boundary_div[1][1] = std::max(boundary_div[1][1], it.m_dis2parent);

		}
		std::vector<int> idxs;
		for (int idx(0); idx < lon_data2.size(); ++idx) {
			idxs.push_back(idx);
		}

		int cut_threadhold = 5;
		cutNBNData(
			lon_data2, boundary_div, idxs,
			density, 1.0, cut_threadhold);

	}

	void nbnDataToGraphInfo(const TreeGraphSimple& nbn_graph,
		std::vector<std::array<ofec::Real, 3>>& points, 
		std::list<std::pair<int, int>>& lines)
	{
		using namespace ofec;
	//	TreeGraphSimple nbn_graph;
	//	nbn_graph.setNBNdata(data);
	//	nbn_graph.calNetwork();
		auto& nbn_data = nbn_graph.get_nbn_data();
		std::vector<std::array<int, 2>> id_belong;
		id_belong.resize(nbn_data.size());
		std::vector<std::array<double, 2>> pos;
		pos.resize(nbn_data.size());
		for (int idx(0); idx < nbn_data.size(); ++idx) {
			id_belong[idx][0] = nbn_graph.get_nbn_data()[idx].m_id;
			id_belong[idx][1] = nbn_graph.get_nbn_data()[idx].m_belong;
			pos[idx][0] = nbn_graph.get_nbn_pos()[idx][0];
			pos[idx][1] = nbn_graph.get_nbn_pos()[idx][1];
		}
		auto min_fit = std::numeric_limits<Real>::max();
		auto max_fit = -min_fit;
		for (int idx(0); idx < nbn_data.size(); ++idx) {
			if (min_fit > nbn_data[idx].m_fitness)
				min_fit = nbn_data[idx].m_fitness;
			if (max_fit < nbn_data[idx].m_fitness)
				max_fit = nbn_data[idx].m_fitness;
		}
		//std::vector<std::array<Real, 3>> points(pos.size());
		points.resize(nbn_data.size());
		std::vector<Real> nodefit(pos.size());
		for (int idx(0); idx < pos.size(); ++idx) {
			int id_assign = idx;
			points[idx][0] = pos[id_assign][0];
			points[idx][1] = pos[id_assign][1];
			nodefit[idx] = points[idx][2] = (nbn_data[id_assign].m_fitness - min_fit) / (max_fit - min_fit);
		}


		//std::list<std::pair<int, int>> lines;
		lines.clear();
		size_t num_lines = id_belong.size();
		for (int idx(0); idx < num_lines; ++idx) {
			if (id_belong[idx][1] < pos.size()) {
				lines.emplace_back(id_belong[idx][0], id_belong[idx][1]);
			}
		}
	}

	void filterNBNDataByDensity(const std::string& savepath,
		const std::string& filename, const std::vector<ofec::TreeGraphSimple::NBNdata>& lon_data) {
		;
		//inputNBNdata(filepath + filename + nbn_subfix, lon_data);

		//std::vector<std::array<double, 3>> pos_fit;
		//ofec::inputMeshPoints(filepath + filename + "_nbnFit.txt", pos_fit);

		if (lon_data.empty())return;
//		lon_data.pop_back();
		///pos_fit.pop_back();

		std::vector<ofec::TreeGraphSimple::NBNdata> lon_data2;

		for (auto& it : lon_data) {
			if (it.m_id!=it.m_belong) {
				lon_data2.push_back(it);
			}
		}
		//	std::vector<int> div_times(lon_data2.size(), 0);san 
		//	std::vector<double> dis2(lon_data2.size());
		std::vector<double> radius(lon_data2.size(), 0);
		calNBNDensity(lon_data2,radius);

		std::vector<std::vector<double>> datas(lon_data2.size(), std::vector<double>(3, 0));
		for (int idx(0); idx < lon_data2.size(); ++idx) {
			datas[idx][0] = lon_data2[idx].m_fitness;
			datas[idx][1] = lon_data2[idx].m_dis2parent;
			datas[idx][2] = radius[idx];
		}

		{
			std::ofstream out(savepath + filename + "_FitDisRadius.txt");
			ofec::outputVectorIdx(out, datas);
			out.close();
		}

		//std::vector<std::vector<double>> filter_pos_fit;
		//for (auto& it : lon_data2) {
		//	std::vector<double> curPos(3, 0);
		//	for (int idx(0); idx < 3; ++idx) curPos[idx] = pos_fit[it.m_id][idx];
		//	filter_pos_fit.push_back(curPos);
		//}
		//{
		//	std::ofstream out(savepath + filename + "_FitDisRadiusFitPos.txt");
		//	ofec::outputVectorIdx(out, filter_pos_fit);
		//	out.close();
		//}


	}



	void filterNBNDataByDensity(const std::vector<ofec::TreeGraphSimple::NBNdata>& lon_data, std::vector<std::vector<double>>& fitDisInfo) {
		//inputNBNdata(filepath + filename + nbn_subfix, lon_data);
//std::vector<std::array<double, 3>> pos_fit;
//ofec::inputMeshPoints(filepath + filename + "_nbnFit.txt", pos_fit);

		if (lon_data.empty())return;
		//		lon_data.pop_back();
				///pos_fit.pop_back();

		std::vector<ofec::TreeGraphSimple::NBNdata> lon_data2;

		for (auto& it : lon_data) {

			lon_data2.push_back(it);
			if (it.m_id == it.m_belong) {
				lon_data2.back().m_dis2parent = 1.0;
			}


			//if (it.m_dis2parent > 1e9) {
			//	int stop = -1;
			//}


		}
		//	std::vector<int> div_times(lon_data2.size(), 0);san 
		//	std::vector<double> dis2(lon_data2.size());
		std::vector<double> radius(lon_data2.size(), 0);
		calNBNDensity(lon_data2, radius);





		std::vector<std::vector<double>> datas(lon_data2.size(), std::vector<double>(3, 0));
		for (int idx(0); idx < lon_data2.size(); ++idx) {
			datas[idx][0] = lon_data2[idx].m_fitness;
			datas[idx][1] = lon_data2[idx].m_dis2parent;
			datas[idx][2] = radius[idx];


			if (lon_data2[idx].m_dis2parent > 1e9) {
				int stop = -1;
			}
		}

		fitDisInfo = datas;

	}


	void outputProXY(
		std::ostream& out,
		Environment *env,
		const std::vector<std::shared_ptr<ofec::SolutionBase>>& sols) {
		//pro->outputSolsHeader(out);
		//pro->outputSols(out, sols);

		//out << "Dim\t" << pro->numVariables() << "\t" << "Cons\t" << pro->numConstraints() << "\t" <<"Objs\t" << pro->numObjectives() << std::endl;
		//if (pro->hasTag(ofec::ProTag::kConOP)) {
		//	using solType = ofec::
		//}
		//else if (pro->hasTag(ofec::ProTag::kOneMax)) {

		//}
		//else if (pro->hasTag(ofec::ProTag::kTSP)) {

		//}
		
		
	}



	void calNearestSolId(
		const std::vector<std::shared_ptr<ofec::SolutionBase>>& sampleSols,
		std::vector<std::shared_ptr<ofec::SolutionBase>>& newsols,
		std::vector<nearSolInfo>& affairSolInfo, ofec::Environment *env,
		const std::function<void(SolutionBase& sol, Environment *env)>& eval_fun
	) {
		affairSolInfo.resize(newsols.size());
		for (int idx(0); idx < newsols.size(); ++idx) {
			auto& curinfo = affairSolInfo[idx];
			auto& cursol = newsols[idx];
			eval_fun(*cursol, env);
			curinfo.init();
			curinfo.m_curFitness = cursol->fitness();
			for (int idy(0); idy < sampleSols.size(); ++idy) {
				auto& otherSol = sampleSols[idy];
				double curdis = cursol->variableDistance(*otherSol, env);
				if (curdis < curinfo.m_dis2NearSol) {
					curinfo.m_nearSolId = idy;
					curinfo.m_dis2NearSol = curdis;
				}
				else if (otherSol->fitness() > sampleSols[curinfo.m_nearSolId]->fitness()) {
					curinfo.m_nearSolId = idy;
				}
				
			}
		}
	}


	void outputNearSols(std::ostream& out, const std::vector<nearSolInfo>& info) {
		int cursolId = 0;
		for (auto& it : info) {
			out << cursolId++ << "\t" << it.m_curFitness << "\t" << it.m_nearSolId << "\t" << it.m_dis2NearSol << std::endl;
		}
	}


	void TreeGraphSimple::RandomSample::sampleMultiThread()
	{
		m_samples.resize(m_numSample);

		std::vector<std::thread> thrds;
		std::vector<int> tasks;
		UTILITY::assignThreads(m_numSample, m_numThread, tasks);
		std::pair<int, int> from_to;
		for (size_t i = 0; i < m_numThread; ++i) {
			from_to.first = tasks[i];
			from_to.second = tasks[i + 1];
			thrds.push_back(std::thread(
				&TreeGraphSimple::RandomSample::sampleThreadTask, this,
				from_to.first, from_to.second));
		}
		for (auto& thrd : thrds)
			thrd.join();
	}

	void TreeGraphSimple::RandomSample::sampleThreadTask(int from, int to)
	{
		auto pro = m_environnment->problem();
		for (int id(from); id < to; ++id) {
			auto& curSol(m_samples[id]);
			curSol.reset(pro->createSolution());
			curSol->initialize(m_environnment.get(), m_random.get());
			curSol->evaluate(m_environnment.get());
			m_eval_fun(*curSol, m_environnment.get());
		}
	}





}
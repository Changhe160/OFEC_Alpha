#ifndef TREE_GRAPH_SIMPLE_H
#define TREE_GRAPH_SIMPLE_H

#include <vector>
#include <memory>
#include <map>
#include "../../../core/environment/environment.h"
#include "../../../core/problem/solution.h"
#include "../../../core/problem/continuous/continuous.h"
#include "../../../core/random/newran.h"

#include "../../function/custom_function.h"
#include "../../functional.h"

#include <algorithm>
#include <ostream>
#include <fstream>
#include <queue>
#include <memory>

namespace ofec {
	class TreeGraphSimple {
	public:
		struct ConProMeshSample {
			enum class EAtri { kFit = 0, kCon, kDynamic };
			int m_maxAtri = 3;
			std::shared_ptr<Environment> m_environnment;
			int m_numDiv = 0;
			int m_maxSample = 0;
			int m_numThread = 0;
			std::vector<std::array<int, 2>> m_pos_idx;

			std::vector<ofec::Solution<>> m_samples;
			std::vector<std::vector<double>> m_attributes;

			void setNumberThread(int numThread) {
				m_numThread = numThread;
			}

			void initialize(const std::shared_ptr<Environment>& env, int numDiv);
			void sampleMultiThread();
			void sampleThreadTask(int from, int to, int updateTag = 0);
			void evaluateMutliThread();

		};

		struct RandomSample {

			std::shared_ptr<Environment> m_environnment;
			std::shared_ptr<Random> m_random;
			int m_numSample = 0;
			int m_numThread = 0;
			std::vector<std::unique_ptr<ofec::SolutionBase>> m_samples;

			std::function<void(SolutionBase& sol, Environment* pro)> m_eval_fun;

			void setNumberThread(int numThread) {
				m_numThread = numThread;
			}

			void initialize(
				const std::shared_ptr<Environment>& env,
				const std::shared_ptr<Random>& rnd, int numSample,
				const std::function<void(SolutionBase& sol, Environment* env)>& eval_fun
			) {
				m_environnment = env;;
				m_random = rnd;
				m_numSample = numSample;
				m_numThread = 1;
				m_eval_fun = eval_fun;
			}
			void sampleMultiThread();
			void sampleThreadTask(int from, int to);

		};




		struct SolInfo {
			int m_solId = 0;
			double m_fitness = 0;
			double m_objectives = 0;
			double m_constraints = 0;
			double m_dynamic_val = 0;
			bool m_is_opt = false;


			static void outputSolInfo(std::ostream& out, const std::vector<SolInfo>& info) {
				std::stringstream buf;
				buf << "SolId\tFitness\tObj\tCon\tDynamic\tOpt" << std::endl;
				for (auto& it : info) {
					buf << it.m_solId << "\t" << it.m_fitness << "\t" << it.m_objectives << "\t" << it.m_constraints << "\t" << it.m_dynamic_val << "\t" << it.m_is_opt << std::endl;
				}


				std::string buf_str = buf.str();
				out.write(buf_str.c_str(), buf_str.size());
			}

			static void transfer(
				std::vector<SolInfo>& info,
				const std::vector<std::shared_ptr<ofec::SolutionBase>>& sols,
				const std::vector<bool>& optFlag
			) {
				info.clear();
				info.resize(sols.size());
				for (int idx(0); idx < info.size(); ++idx) {
					info[idx].m_solId = idx + 1;
					info[idx].m_fitness = sols[idx]->fitness();
					info[idx].m_objectives = sols[idx]->objective()[0];
					if (sols[idx]->isFeasible()) info[idx].m_constraints = 1;
					else info[idx].m_constraints = -1;
					if (optFlag[idx]) info[idx].m_is_opt = 1;

				}
			}

			static void setOpt(std::vector<SolInfo>& info, int from, int to) {
				for (int idx(from); idx < to; ++idx) {
					info[idx].m_is_opt = true;
				}
			}
		};


		struct NBNdata {
			int m_id = 0;
			int m_originId = 0;
			int m_belong = 0;
			double m_dis2parent = 0;
			double m_fitness = 0;



			NBNdata() = default;
			NBNdata(int id, int originId, int belongId, double dis2parent, double fit) :
				m_id(id), m_originId(originId), m_belong(belongId), m_dis2parent(dis2parent), m_fitness(fit) {}


			bool input(std::istream& in) {
				if (in >> m_id >> m_originId >> m_belong >> m_dis2parent >> m_fitness) return true;
				else return false;
			}

			bool input(std::stringstream& buf) {
				if (buf >> m_id >> m_originId >> m_belong >> m_dis2parent >> m_fitness) return true;
				else return false;
			}


			void single_output(std::ostream& out) const {
				out << "curIt\t" << m_id << "\tbelongId\t" << m_belong <<
					"\tdis2parent\t" << m_dis2parent << "\tfitness\t" << m_fitness;
			}

			void single_output(std::stringstream& out)const {
				out << "curIt\t" << m_id << "\tbelongId\t" << m_belong <<
					"\tdis2parent\t" << m_dis2parent << "\tfitness\t" << m_fitness;
			}

			static void outputHead(std::stringstream& out) {
				out << "CurId\tOrigin\tBelong\tDis2parent\tFitness" << std::endl;
			}

			void output(std::stringstream& out) const {
				out << m_id << "\t" << m_originId << "\t" << m_belong <<
					"\t" << m_dis2parent << "\t" << m_fitness << std::endl;
			}


			void output(std::ostream& out) const {
				out << m_id << "\t" << m_originId << "\t" << m_belong <<
					"\t" << m_dis2parent << "\t" << m_fitness << std::endl;
			}

		};
		using Pos = std::array<double, 3>;

		//struct Pos {
		//	double m_x = 0;
		//	double m_y = 0;
		//	double m_z = 0;

		//	void set(double x, double y, double z) {
		//		m_x = x;
		//		m_y = y;
		//		m_z = z;
		//	}
		//};

	protected:


		std::vector<NBNdata> m_nbn_data;
		std::vector<Pos> m_nbn_pos;
		std::vector<int> m_belong;
		std::vector<double> m_nodeColor;
		std::vector<double> m_nodeBasin;



	public:
		//std::string m_task_name;


		void setNodeBasin(const std::vector<double>& nodeBasin) {
			m_nodeBasin = nodeBasin;
		}

		const std::vector<NBNdata>& get_nbn_data() const{
			return m_nbn_data;
		}

		const std::vector<Pos>& get_nbn_pos()const {
			return m_nbn_pos;
		}

		void setNodeColor(const std::vector<double>& color) {
			m_nodeColor = color;
		}

		void calNetwork(double maxDis=-1) {
			if (m_nbn_data.empty()) {
				m_nbn_pos.clear();
			}
			else {

				m_nbn_pos.clear();
				m_nbn_pos.resize(m_nbn_data.size());
				generateTreeRandomLeaveOrder(maxDis);

			}
		}

		void modifyBestSols(ofec::Random* rnd, ofec::Problem* pro, 
			const std::vector<ofec::SolutionBase*>& sols) {

			double maxFit = -std::numeric_limits<double>::max();
			double maxDis = 0;


			for (auto& it : m_nbn_data) {
				maxFit = std::max(maxFit, it.m_fitness);

				if (it.m_dis2parent < 1e9)
					maxDis = std::max(maxDis, it.m_dis2parent);
			}
			maxDis = maxDis * 1.5;



			for (int idx(0); idx < m_nbn_data.size(); ++idx) {
				if (m_nbn_data[idx].m_fitness == maxFit) m_nbn_data[idx].m_belong = idx;
			}
			std::vector<int> belong(m_nbn_data.size());
			for (int idx(0); idx < m_nbn_data.size(); ++idx) {
				belong[idx] = m_nbn_data[idx].m_belong;
			}

			std::vector<double> rootFit;
			//int rootId = 0;
			std::vector<int> rootIds;
			for (int idx(0); idx < m_nbn_data.size(); ++idx) {

				if (m_nbn_data[idx].m_belong != idx) {
				}
				else {
					rootIds.push_back(idx);
					rootFit.push_back(m_nbn_data[idx].m_fitness);
				}
			}

			if (rootIds.size() > 1&&rootIds.size()<=1000) {

				int compareNeighborSize = std::min<int>(1000, rootIds.size());
				auto optIds = rootIds;
				for (auto& it : rootIds) {
					rnd->uniform.shuffle(optIds.begin(), optIds.end());
					auto& curdata = m_nbn_data[it];
					for (int idx(0); idx < compareNeighborSize; ++idx) {
						auto curId = optIds[idx];
						if (it != curId) {
							double d = pro->variableDistance(sols[it]->variableBase(), sols[curId]->variableBase());
							curdata.m_dis2parent = std::min(curdata.m_dis2parent, d);
						}
					}
				}
			}
		}


		void generateTreeRandomLeaveOrder(double maxDis=-1) {


			if (m_nbn_data.empty())return;

			//for (int idx(0); idx < m_nbn_data.size(); ++idx) {
			//	if (m_nbn_data[m_nbn_data[idx].m_belong].m_fitness < m_nbn_data[idx].m_fitness) {
			//		int stop = -1;
			//		std::cout << "erro" << std::endl;
			//	}
			//}

			if (m_nodeBasin.size() != m_nbn_data.size()) {
				m_nodeBasin.resize(m_nbn_data.size());
				for (auto& it : m_nodeBasin) it = 1.0;
			}

			for (auto& it : m_nbn_data) {
				if (it.m_belong == -1) it.m_belong = it.m_id;
			}

			std::vector<std::vector<int>> sons(m_nbn_data.size());

			double maxFit = -std::numeric_limits<double>::max();

			for (auto& it : m_nbn_data) {
				maxFit = std::max(maxFit, it.m_fitness);
			}
			//double maxDis = 0;
			if (maxDis < 0) {
				maxDis = 0;
				for (auto& it : m_nbn_data) {
					if (it.m_dis2parent < 1e9)
						maxDis = std::max(maxDis, it.m_dis2parent);
				}
				maxDis = maxDis * 1.5;

			}

			//for (int idx(0); idx < m_nbn_data.size(); ++idx) {
			//	if (m_nbn_data[idx].m_fitness == maxFit) m_nbn_data[idx].m_belong = idx;
			//}
			std::vector<int> belong(m_nbn_data.size());
			for (int idx(0); idx < m_nbn_data.size(); ++idx) {
				belong[idx] = m_nbn_data[idx].m_belong;
			}

			std::vector<double> rootFit;
			//int rootId = 0;
			std::vector<int> rootIds;
			for (int idx(0); idx < m_nbn_data.size(); ++idx) {

				if (m_nbn_data[idx].m_belong != idx) {
					sons[m_nbn_data[idx].m_belong].push_back(idx);
				}
				else {
					rootIds.push_back(idx);
					rootFit.push_back(m_nbn_data[idx].m_fitness);
				}
			}
			//double inf_dis = 1e9;
			if (rootIds.size() > 1) {

				for (auto& bestId1 : rootIds) {
					auto& curdata = m_nbn_data[bestId1];
					curdata.m_dis2parent = std::min(curdata.m_dis2parent, maxDis);
				}
			}

			for (auto& it : m_nbn_data) {
				if (it.m_dis2parent > 1e9) {
					it.m_dis2parent = maxDis;
				}
			}

			for (auto& it : m_nbn_data) {
				it.m_dis2parent /= maxDis;
			}



			int rootId = rootIds.front();
			if (rootIds.size() > 1) {
				rootId = sons.size();
				sons.push_back(rootIds);
				m_nbn_pos.push_back({ 0,0,1 });
				belong.push_back(rootId);
				for (auto& sonId : rootIds) {
					belong[sonId] = rootId;
				}
				for (auto& sonId : rootIds) {
					m_nbn_data[sonId].m_belong = rootId;
				}
				m_nbn_data.push_back(NBNdata(rootId, -1, rootId, 0, maxFit));
				m_nodeBasin.push_back(1.0);
			}

			m_belong = belong;
			std::vector<int> sortedId;
			std::queue<int> sortQue;
			sortQue.push(rootId);
			int curId(0);
			std::vector<bool> visited(m_nbn_pos.size(), false);
			bool flag = false;
			visited[rootId] = true;
			while (!sortQue.empty()) {
				int cur = sortQue.front();
				sortQue.pop();
				sortedId.push_back(cur);
				for (auto& sonId : sons[cur]) {
					sortQue.push(sonId);
					if (!visited[sonId]) {
						visited[sonId] = true;
					}
					else flag = false;
				}

			}
			if (flag) std::cout << "error" << std::endl;


			//for (int idx(0); idx < m_nbn_pos.size(); ++idx) {
			//	sortedId[idx] = idx;
			//}
			//std::sort(sortedId.begin(), sortedId.end(), [&](int a, int b) {
			//	if (rootId == a) return true;
			//	else if (rootId == b) return false;
			//	else return m_nbn_data[a].m_fitness > m_nbn_data[b].m_fitness;
			//});

			std::vector<double> leaf_length(m_nbn_pos.size(), 0);
			for (auto it(sortedId.rbegin()); it != sortedId.rend(); ++it) {
				auto& cur(*it);
				for (auto& son_iter : sons[cur]) {
					leaf_length[cur] += leaf_length[son_iter];
				}

				leaf_length[cur] += m_nodeBasin[cur];

				//leaf_length[cur] = std::max<int>(1, leaf_length[cur]);
			}

			//for (auto& it : sortedId) {
			//	if (it < 0 || it >= sortedId.size()) {
			//		std::cout << "error at" << m_task_name << std::endl;
			////		return;
			//	}
			//}

			//std::vector<double> radius(m_nbn_data.size(), 0);
			std::vector<double> from_angle(m_nbn_pos.size(), 0);
			std::vector<double> to_angle(m_nbn_pos.size(), 0);

			to_angle[rootId] = 1;
			//if (sons[rootId].size() == 1) {
			//	to_angle[rootId] = 0.5;
			//}
			//else {
			//	to_angle[rootId] = 1;
			//}
			for (auto& parantId : sortedId) {
				double theta_len = to_angle[parantId] - from_angle[parantId];
				theta_len /= double(leaf_length[parantId]);
				double from_theta(from_angle[parantId]);
				for (auto& son : sons[parantId]) {
					//radius[son] = radius[parantId] + m_nbn_data[son].m_dis2parent;
					from_angle[son] = from_theta;
					from_theta += theta_len * leaf_length[son];
					to_angle[son] = from_theta;
				}
			}


			std::pair<double, double> fit_range = { m_nbn_data.front().m_fitness,m_nbn_data.front().m_fitness };
			double fit_len = 0;
			for (auto& it : m_nbn_data) {
				fit_range.first = std::min(it.m_fitness, fit_range.first);
				fit_range.second = std::max(it.m_fitness, fit_range.second);
			}
			fit_len = fit_range.second - fit_range.first;


			if (fit_len == 0) {
				m_nbn_pos[rootId] = { 0, 0,0.5 };
			}
			else {
				m_nbn_pos[rootId] = { 0, 0,1 };
			}

			for (int idx(1); idx < sortedId.size(); ++idx) {

				auto& cur = sortedId[idx];
				double angle = (from_angle[cur] + to_angle[cur]) / 2.0;
				double norFit = ofec::mapReal<double>(m_nbn_data[cur].m_fitness, fit_range.first, fit_range.second, 0, 1);
				setPosition(m_nbn_pos[cur],
					m_nbn_data[cur].m_dis2parent, angle, norFit);
				setOffsetPos(m_nbn_pos[cur], m_nbn_pos[belong[cur]]);

				//if (isinf(m_nbn_pos[cur].front())) {
				//	int stop = -1;
				//}
			}

			double maxRadius = 0;
			for (auto& pos : m_nbn_pos) {
				maxRadius = std::max<double>(maxRadius, get2Dradius(pos));
			}

			for (auto& pos : m_nbn_pos) {
				for (int idx(0); idx < 2; ++idx) {
					pos[idx] /= maxRadius;
				}
			}
		}
		double get2Dradius(const Pos & pos) {
			return std::sqrt(pos[0] * pos[0] + pos[1] * pos[1]);

		}

		void setPosition(Pos& pos,
			double radius, double angle, double zvalue) const {
			const static double PI = acos(-1.0);
			pos[0] = (radius * std::sin(angle * PI * 2));
			pos[1] = (radius * std::cos(angle * PI * 2));
			pos[2] = zvalue;
		}
		void setOffsetPos(Pos& pos, Pos& curPos) {
			for (int idx(0); idx < 2; ++idx) {
				pos[idx] += curPos[idx];
			}
		}
		void setNBNdata(const std::vector<NBNdata>& nbn_data) {
			m_nbn_data = nbn_data;
		}


		static void inputNBNnetwork(const std::string& filepath,
			std::vector<Pos>& nbn_pos,
			std::vector<int>& belongs);
		static void inputNBNnetworkColor(const std::string& filepath,
			std::vector<Pos>& nbn_pos,
			std::vector<int>& belongs,
			std::vector<double>& fitness);

		//) {

		//	std::stringstream buf;
		//	readFileBuffer(filepath, buf);
		//	std::string line;
		//	std::getline(buf, line);
		//	TreeGraphSimple::NBNdata cur_data;
		//	Pos curPos;
		//	int belong, id(-1);
		//	while (buf >> id >> curPos[0] >> curPos[1] >> curPos[2] >> belong) {
		//		nbn_pos.push_back(curPos);
		//		belongs.push_back(belong);
		//		// for test
		//		//break;
		//	}
		//}

		void outputNBNnetwork(std::ostream & out) {

			std::stringstream buf;
			buf << "Id\tX\tY\tZ\tBelong\t" << std::endl;
			int id(-1);
			for (int idx(0); idx < m_nbn_pos.size(); ++idx) {
				id = -1;
				if (idx < m_nbn_data.size()) id = m_nbn_data[idx].m_id;

				buf << id;
				
				for (auto& it : m_nbn_pos[idx]) {
					buf << "\t" << it;
				}
				buf << "\t" << m_belong[idx];
				buf << std::endl;
			}
			std::string buf_str = buf.str();
			out.write(buf_str.c_str(), buf_str.size());

			
		}


		

		void outputNBNnetworkWithColor(std::ostream& out) {

			std::vector<double> color(m_nodeColor);
			color.resize(m_nbn_pos.size(), -1e9);


			std::stringstream buf;
			buf << "Id\tX\tY\tZ\tBelong\tColor\t" << std::endl;
			int id(-1);
			for (int idx(0); idx < m_nbn_pos.size(); ++idx) {
				id = -1;
				if (idx < m_nbn_data.size()) id = m_nbn_data[idx].m_id;

				buf << id;

				for (auto& it : m_nbn_pos[idx]) {
					buf << "\t" << it;
				}
				buf << "\t" << m_belong[idx];
				buf << "\t" << color[idx];
				buf << std::endl;
			}
			std::string buf_str = buf.str();
			out.write(buf_str.c_str(), buf_str.size());


		}
	};

	struct nearSolInfo {
		int m_nearSolId = -1;
		double m_dis2NearSol = 0;
		double m_curFitness = 0;

		void init() {
			m_nearSolId = -1;
			m_dis2NearSol = std::numeric_limits<double>::max();
			m_curFitness = 0;
		}

		
	};




	extern void mesh2DproblemMutlithread(Environment *env, int numDiv,
		const std::vector<std::array<int, 2>>& pos,
		std::vector<TreeGraphSimple::Pos>& mesh, int from, int to);

	extern void mesh2Dproblem(
		Environment *env, int numDiv,
		std::vector<std::array<int, 2>>& pos,
		std::vector<TreeGraphSimple::Pos>& mesh);

	extern void ouputMesh(std::ostream& out,
		const std::vector<std::array<int, 2>>& pos,
		const std::vector<TreeGraphSimple::Pos>& mesh
	);

	extern void ouputMeshPoints(std::ostream& out,
		Environment *env,
		const std::vector<std::shared_ptr<ofec::SolutionBase>> & sols
	);

	extern void outputVectorIdx(std::ostream& out,
		const std::vector<std::vector<double>>& data);

	extern void inputVectorIdx(std::istream& out,
		std::vector<std::vector<double>>& data);

	extern void readFileBuffer(const std::string& filepath, std::stringstream& buf);

	extern void inputMeshPoints(const std::string& filepath,
		std::vector<std::array<double, 3>>& pos);

	extern void inputNBNdata(const std::string& filepath,
		std::vector<TreeGraphSimple::NBNdata>& nbn_data);

	extern void ouputNBNdata(const std::string& filepath,
		const std::vector<TreeGraphSimple::NBNdata>& nbn_data);


	extern void outputNBNdata(
		std::ostream& out,
		const std::vector<int>& solId,
		const std::vector<int>& belong,
		const std::vector<double>& dis2parent,
		const std::vector<double>& fitness
	);

	extern void transferNBNData(
		std::vector<TreeGraphSimple::NBNdata>& nbn_data
		, const std::vector<int>& solId,
		const std::vector<int>& belong,
		const std::vector<double>& dis2parent,
		const std::vector<double>& fitness);
		
	extern void cutNBNData(
		const std::vector<ofec::TreeGraphSimple::NBNdata>& lon_data,
		const std::array<std::array<double, 2>, 2>& boundary,
		const std::vector<int>& idxs,
		std::vector<double>& radius,
		double cut_radius,
		int min_div = 5
	);

	
	extern 	void nbnDataToGraphInfo(
	//	const std::vector<ofec::TreeGraphSimple::NBNdata>& data,
		const TreeGraphSimple& graph,
		std::vector<std::array<ofec::Real, 3>>& points,
		std::list<std::pair<int, int>>& lines);

	extern void filterNBNDataByDensity(const std::string& savepath,
		const std::string& filename,const std::vector<ofec::TreeGraphSimple::NBNdata>& lon_data);
	

	extern void filterNBNDataByDensity(const std::vector<ofec::TreeGraphSimple::NBNdata>& lon_data, std::vector<std::vector<double>>& fitDisInfo);


	extern void outputProXY(
		std::ostream& out,
		Environment *env,
		const std::vector<std::shared_ptr<ofec::SolutionBase>>& sols);

	

	extern void calNearestSolId(
		const std::vector<std::shared_ptr<ofec::SolutionBase>>& sampleSols,
		std::vector<std::shared_ptr<ofec::SolutionBase>>& cursol,
		std::vector< nearSolInfo>& affairSolInfo, ofec::Problem* pro,
		const std::function<void(SolutionBase& sol, Problem* pro)>& eval_fun
	);

	extern void outputNearSols(std::ostream& out, const std::vector<nearSolInfo>& info);

}

#endif
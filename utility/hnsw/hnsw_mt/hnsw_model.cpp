#include "hnsw_model.h"
#include <unordered_set>
#include "../../../core/exception.h"
#include "../../../utility/nbn_visualization/nbn_calculator/distance_calculator/distance_calculator.h"
namespace ofec {
	namespace hnsw {
		void HnswModel::initialize(ofec::Environment* env, ofec::Random* rnd, int numThread)
		{
			m_env = env->getSharedPtr();
			m_rnd = rnd->getSharedPtr();
			setInfo();
			clear();
			m_distanceFun = [&](int a, int b) {
				return m_solutions[a]->variableDistance(*m_solutions[b], m_env.get());
				};
			setNumberThreads(numThread);
		}

		void HnswModel::copy(const HnswModel& model, double seed)
		{
			m_ = model.m_;
			max_m_ = model.max_m_;
			max_m0_ = model.max_m0_;
			ef_construction_ = model.ef_construction_;
			level_mult_ = model.level_mult_;

			neighbor_selecting_ = model.neighbor_selecting_;
			post_neighbor_selecting_ = model.post_neighbor_selecting_;
			post_graph_process_ = model.post_graph_process_;

			setInfo();

			m_distanceFun = [&](int a, int b) {
				return m_solutions[a]->variableDistance(*m_solutions[b], m_env.get());
				};


			setNumberThreads(model.m_numberThreads);


			m_env = model.m_env->getSharedPtr();
			m_rnd.reset(new Random(seed));

			max_level_ = model.max_level_;
			m_enterpoint_id = model.m_enterpoint_id;
			nodes_ = model.nodes_;
			m_solutions = model.m_solutions;

		}

		void HnswModel::setNumberThreads(int numThread) {
			m_numberThreads = numThread;
			m_threadInfos.resize(m_numberThreads);
			for (auto& it : m_threadInfos) {
				it.intialize();
			}
		}

		void HnswModel::setInfo() {


			if (neighbor_selecting_ == NeighborSelectingPolicy::HEURISTIC) {
				selecting_policy_ = std::make_unique<HeuristicNeighborSelectingPoliciesSol>(false);
				is_naive_ = false;
			}
			else if (neighbor_selecting_ == NeighborSelectingPolicy::HEURISTIC_SAVE_REMAINS) {
				selecting_policy_ = std::make_unique<HeuristicNeighborSelectingPoliciesSol>(true);
				is_naive_ = false;
			}
			else if (neighbor_selecting_ == NeighborSelectingPolicy::NAIVE) {
				selecting_policy_ = std::make_unique<NaiveNeighborSelectingPoliciesSol>();
				is_naive_ = true;
			}
			if (post_neighbor_selecting_ == NeighborSelectingPolicy::HEURISTIC) {
				post_selecting_policy_ = std::make_unique<HeuristicNeighborSelectingPoliciesSol>(false);
			}
			else if (post_neighbor_selecting_ == NeighborSelectingPolicy::HEURISTIC_SAVE_REMAINS) {
				post_selecting_policy_ = std::make_unique<HeuristicNeighborSelectingPoliciesSol>(true);
			}
			else if (post_neighbor_selecting_ == NeighborSelectingPolicy::NAIVE) {
				post_selecting_policy_ = std::make_unique<NaiveNeighborSelectingPoliciesSol>();
			}
		}

		void HnswModel::configsToParameters(ofec::ParameterVariantStream& params) {

			params << m_ << max_m_ << max_m0_ << ef_construction_ << level_mult_;
			//	params << m_ensure_k;
			params << static_cast<int>(neighbor_selecting_)
				<< static_cast<int>(post_neighbor_selecting_)
				<< static_cast<int>(post_graph_process_);
			params << m_numberThreads;
		}


		void HnswModel::configsfromParameters(ofec::ParameterVariantStream& params) {
			params >> m_ >> max_m_ >> max_m0_ >> ef_construction_ >> level_mult_;
			//	params >> m_ensure_k;
			int policyType1(0), policyType2, policyType3;

			params >> policyType1 >> policyType2 >> policyType3;
			neighbor_selecting_ = static_cast<NeighborSelectingPolicy>(policyType1);
			post_neighbor_selecting_ = static_cast<NeighborSelectingPolicy>(policyType2);
			post_graph_process_ = static_cast<GraphPostProcessing>(policyType3);

			//size_t node_size(0);
			params >> m_numberThreads;
			//nodes_.resize(node_size);

			setNumberThreads(m_numberThreads);
			setInfo();

		}



		void HnswModel::datasToParameters(ofec::ParameterVariantStream& params) {
			params << max_level_ << m_enterpoint_id << nodes_.size();
			std::vector<int> levels(nodes_.size());
			for (int idx(0); idx < levels.size(); ++idx) {
				levels[idx] = nodes_[idx].GetLevel();
			}
			params << levels;

			std::vector<std::vector<int>> neighbors;
			for (int idx(0); idx < nodes_.size(); ++idx) {
				nodes_[idx].getNeighborIds(neighbors);
				params << neighbors.size();
				for (auto& it : neighbors) {
					params << it;
				}
			}
		}
		void HnswModel::datasFromParameters(ofec::ParameterVariantStream& params) {
			size_t nodeSize;
			params >> max_level_ >> m_enterpoint_id >> nodeSize;
			nodes_.resize(nodeSize);
			std::vector<int> levels(nodes_.size());
			params >> levels;
			for (int idx(0); idx < levels.size(); ++idx) {
				nodes_[idx].set(idx, levels[idx], max_m_, max_m0_);
			}
			//std::vector<std::vector<int>> neighbors;

			std::vector<std::vector<std::vector<int>>> total_neighborsIds;
			total_neighborsIds.resize(nodeSize);

			size_t vsize;
			for (int idx(0); idx < nodes_.size(); ++idx) {
				auto& neighbors = total_neighborsIds[idx];
				params >> vsize;
				neighbors.resize(vsize);
				for (auto& it : neighbors) {
					params >> it;
				}
			}

			std::vector<ofec::SolutionBase*> solutions;
			for (auto& it : m_solutions) {
				solutions.push_back(it.get());
			}

			std::vector<double> dis;
			std::map<std::pair<int, int>, int> mPairId2id;
			std::pair<int, int> curPair;

			std::vector<std::pair<int, int>> tasks;
			int id = 0;
			int curId = 0;
			std::vector<std::vector<std::vector<int>>> distanceIdx(total_neighborsIds);
			for (int idx(0); idx < distanceIdx.size(); ++idx) {
				for (int idy(0); idy < distanceIdx[idx].size(); ++idy) {
					for (int idz(0); idz < distanceIdx[idx][idy].size(); ++idz) {
						curPair.first = idx;
						curPair.second = total_neighborsIds[idx][idy][idz];
						if (mPairId2id.find(curPair) == mPairId2id.end()) {
							curId = mPairId2id[curPair] = id++;
							tasks.push_back(curPair);
						}
						else {
							curId = mPairId2id[curPair];
						}
						distanceIdx[idx][idy][idz] = curId;
					}
				}
			}

			dis.resize(id);
			ofec::NBN_DistanceCalculator calculator;
			calculator.calculateDistance(solutions, tasks, dis, m_env.get());

			Info curInfo;

			for (int idx(0); idx < distanceIdx.size(); ++idx) {
				auto& curNode = nodes_[idx];
				for (int idy(0); idy < distanceIdx[idx].size(); ++idy) {
					for (int idz(0); idz < distanceIdx[idx][idy].size(); ++idz) {
						curInfo.set(total_neighborsIds[idx][idy][idz], dis[distanceIdx[idx][idy][idz]]);
						curNode.getFriends(idy).push(curInfo);
					}
				}
			}
		}



		void HnswModel::searchBySolution(ThreadLocalInfo& curInfo, size_t k,
			int ef_search, bool ensure_k, std::vector<Info>& result)const
		{
			using namespace std;
			using namespace ofec;

			curInfo.swapVisited();
			curInfo.m_distanceVisited.Reset();


			if (ef_search < 0)
				ef_search = 50 * k;

			std::vector<Info> ensure_k_path_;

			Info curNode(m_enterpoint_id, getSaveDistanceSolbase(m_enterpoint_id, m_env.get(), curInfo));

			if (ensure_k) {
				ensure_k_path_.clear();
				ensure_k_path_.push_back(curNode);
			}

			bool changed;
			for (auto i = max_level_; i > 0; --i) {

				curInfo.m_visisted.Reset();
				curInfo.m_visisted.MarkAsVisited(curNode.nodeId());


				changed = true;
				while (changed) {
					changed = false;

					for (auto& it : nodes_[curNode.nodeId()].getFriends(0).neighbors()) {
						int fid = it.nodeId();
						if (!curInfo.m_visisted.NotVisited(fid)) {
							auto d = getSaveDistanceSolbase(fid, m_env.get(), curInfo);
							if (d < curNode.value()) {
								curNode.set(fid, d);
								changed = true;
								if (ensure_k) ensure_k_path_.push_back(curNode);
							}
						}
					}
				}
			}

			CallSearchById_(curInfo, curNode, k, ef_search, ensure_k, result, ensure_k_path_);

		}

		void HnswModel::SearchByIdV1_(ThreadLocalInfo& curInfo, const Info& curNode, size_t k,
			size_t ef_search,
			bool ensure_k, std::vector<Info>& result)const {

			IdValueMinHeap candidates;
			IdValueMinHeap visited_nodes;


			candidates.emplace(curNode);
			curInfo.m_visisted.Reset();
			curInfo.m_visisted.MarkAsVisited(curNode.nodeId());

			//unsigned int visited_mark = visited_list_->GetVisitMark();
			//unsigned int* visited = visited_list_->GetVisited();
			//visited[cur_node_id] = visited_mark;

			if (ensure_k && !result.empty()) {
				if (!PrepareEnsureKSearch(curInfo, curNode, result, visited_nodes)) {
					return;
				}
			}

			ofec::Real farthest_distance = curNode.value();
			size_t candidate_found_cnt = 1;
			size_t visited_cnt = 0;

			int cur_node_id = -1;

			while (!candidates.empty() && visited_cnt < ef_search) {
				const Info& c = candidates.top();
				cur_node_id = c.nodeId();
				visited_nodes.emplace(c);
				++visited_cnt;
				candidates.pop();

				ofec::Real minimum_distance = farthest_distance;

				for (auto& neighbor : nodes_[cur_node_id].getFriends(0).neighbors()) {
					if (curInfo.m_visisted.NotVisited(neighbor.nodeId())) {
						curInfo.m_visisted.MarkAsVisited(neighbor.nodeId());
						auto d = getSaveDistanceSolbase(neighbor.nodeId(), m_env.get(), curInfo);

						if (d < minimum_distance || candidate_found_cnt < ef_search) {
							candidates.emplace(neighbor.nodeId(), d);
							if (d > farthest_distance) {
								farthest_distance = d;
							}
							++candidate_found_cnt;
						}
					}
				}

			}

			MakeSearchResult(k, candidates, visited_nodes, result);
		}

		void HnswModel::SearchByIdV2_(ThreadLocalInfo& curInfo, const Info& curNode, size_t k, size_t ef_search,
			bool ensure_k, std::vector<Info>& result)const {
			IdValueMinHeap candidates;
			IdValueMinHeap visited_nodes;
			IdValueMaxHeap found_distances;


			candidates.push(curNode);
			found_distances.push(curNode);

			curInfo.m_visisted.Reset();
			if (ensure_k && !result.empty()) {
				if (!PrepareEnsureKSearch(curInfo, curNode, result, visited_nodes)) {
					return;
				}
			}

			while (!candidates.empty()) {
				const auto& c = candidates.top();
				if (c.value() > found_distances.top().value()) {
					break;
				}

				int cur_node_id = c.nodeId();
				visited_nodes.push(c);
				candidates.pop();

				for (auto& neighbor : nodes_[cur_node_id].getFriends(0).neighbors()) {
					int node_id = neighbor.nodeId();
					if (curInfo.m_visisted.NotVisited(node_id)) {
						curInfo.m_visisted.MarkAsVisited(node_id);
						auto d = getSaveDistanceSolbase(node_id, m_env.get(), curInfo);
						if (d < found_distances.top().value() || found_distances.size() < ef_search) {
							candidates.emplace(node_id, d);
							found_distances.emplace(node_id, d);
							if (found_distances.size() > ef_search) {
								found_distances.pop();
							}
						}
					}
				}
			}

			MakeSearchResult(k, candidates, visited_nodes, result);

		}


		bool HnswModel::PrepareEnsureKSearch(ThreadLocalInfo& curInfo, const Info& curNode,
			std::vector<Info>& result,
			IdValueMinHeap& visited_nodes) const {


			for (size_t i = 0; i < result.size(); ++i) {
				if (result[i].nodeId() == curNode.nodeId()) {
					return false;
				}
				curInfo.m_visisted.MarkAsVisited(result[i].nodeId());
				visited_nodes.emplace(std::move(result[i]));
			}
			result.clear();

			return true;
		}


		void HnswModel::MakeSearchResult(size_t k, IdValueMinHeap& candidates, IdValueMinHeap& visited_nodes,
			std::vector<Info>& result) const {
			while (result.size() < k) {
				if (!candidates.empty() && !visited_nodes.empty()) {
					const auto& c = candidates.top();
					const auto& v = visited_nodes.top();
					if (c.value() < v.value()) {
						result.emplace_back(c);
						candidates.pop();
					}
					else {
						result.emplace_back(v);
						visited_nodes.pop();
					}
				}
				else if (!candidates.empty()) {
					const auto& c = candidates.top();
					result.emplace_back(c);
					candidates.pop();
				}
				else if (!visited_nodes.empty()) {
					const auto& v = visited_nodes.top();
					result.emplace_back(v);
					visited_nodes.pop();
				}
				else {
					break;
				}
			}
		}

		void HnswModel::assignMemory(int size) {
			clear();

			nodes_.resize(size);
			m_solutions.resize(size);

			int level = 0;
			for (int idx(0); idx < size; ++idx) {
				nodes_[idx].set(idx, level, max_m_, max_m0_);
			}
		}

		void HnswModel::getSolutions(std::vector<const ofec::SolutionBase*>& sols)const {
			sols.resize(m_solutions.size());
			for (int idx(0); idx < sols.size(); ++idx) {
				sols[idx] = m_solutions[idx].get();
			}
		}
		void HnswModel::setSolutions(const std::vector<ofec::SolutionBase*>& sols) {
			m_solutions.resize(sols.size());
			for (int idx(0); idx < sols.size(); ++idx) {
				m_solutions[idx] = sols[idx]->getSharedPtr();
			}
		}


		int HnswModel::addDataSingleThread(ofec::SolutionBase* sol)
		{

			int id = nodes_.size();
			int level = GetRandomNodeLevel(m_rnd.get());
			nodes_.resize(nodes_.size() + 1);
			nodes_.back().set(id, level, max_m_, max_m0_);
			m_solutions.emplace_back(sol->getSharedPtr());
			if (id == 0) {
				{
					//	std::unique_lock lock(max_level_guard_);
					max_level_ = level;
					m_enterpoint_id = id;
				}
			}
			else {
				auto& curInfo = getThreadInfo();
				curInfo.m_curId = id;
				curInfo.resize(nodes_.size());

				curInfo.swapVisited();
				curInfo.m_distanceVisited.Reset();
				InsertNodeSingleThread(curInfo);
			}
			return id;
		}

		void HnswModel::SetConfigs(const std::vector<std::pair<std::string, std::string>>& configs)
		{
			int m = -1, max_m0 = -1, ef_construction = -1, n_threads = -1;
			float mult = -1;
			NeighborSelectingPolicy neighbor_selecting = NeighborSelectingPolicy::HEURISTIC;
			GraphPostProcessing graph_merging = GraphPostProcessing::SKIP;

			for (const auto& c : configs) {
				if (c.first == "M") {
					m = stoi(c.second);
				}
				else if (c.first == "MaxM0") {
					max_m0 = stoi(c.second);
				}
				else if (c.first == "efConstruction") {
					ef_construction = stoi(c.second);
				}
				//else if (c.first == "NumThread") {
				//	n_threads = stoi(c.second);
				//}
				else if (c.first == "Mult") {
					mult = stof(c.second);
				}
				else if (c.first == "NeighborSelecting") {
					if (c.second == "heuristic") {
						neighbor_selecting = NeighborSelectingPolicy::HEURISTIC;
					}
					else if (c.second == "heuristic_save_remains") {
						neighbor_selecting = NeighborSelectingPolicy::HEURISTIC_SAVE_REMAINS;
					}
					else if (c.second == "naive") {
						neighbor_selecting = NeighborSelectingPolicy::NAIVE;
					}
					else {
						throw ofec::Exception("[Error] at HnswModel:: Invalid configuration value for NeighborSelecting: " + c.second);
					}
				}
				else if (c.first == "GraphMerging") {
					if (c.second == "skip") {
						graph_merging = GraphPostProcessing::SKIP;
					}
					else if (c.second == "merge_level0") {
						graph_merging = GraphPostProcessing::MERGE_LEVEL0;
					}
					else {
						throw ofec::Exception("[Error]@ HnswModel:: Invalid configuration value for GraphMerging: " + c.second);
					}
				}
				else if (c.first == "EnsureK") {
				}
				else {
					throw ofec::Exception("[Error]@HnswModel:: Invalid configuration key: " + c.first);
				}
			}

			SetConfigs(m, max_m0, ef_construction, mult, neighbor_selecting, graph_merging);

		}

		void HnswModel::SetConfigs(int m, int max_m0, int ef_construction, float mult, NeighborSelectingPolicy neighbor_selecting, GraphPostProcessing graph_merging)
		{
			if (m > 0) max_m_ = m_ = m;
			if (max_m0 > 0) max_m0_ = max_m0;
			if (ef_construction > 0) ef_construction_ = ef_construction;
			//if (n_threads > 0) n_threads_ = n_threads;
			level_mult_ = mult > 0 ? mult : 1 / log(1.0 * m_);
			neighbor_selecting_ = neighbor_selecting;
			post_graph_process_ = graph_merging;
		}
		int HnswModel::GetRandomNodeLevel(ofec::Random* rnd)
		{
			double r = rnd->uniform.next();
			if (r < std::numeric_limits<double>::epsilon())
				r = 1.0;
			return (int)(-log(r) * level_mult_);
		}

		void HnswModel::InsertNodeSingleThread(ThreadLocalInfo& cur)
		{
			using namespace std;
			int qNodeId = cur.m_curId;
			int cur_level = nodes_[cur.m_curId].GetLevel();
			int max_level_copy = max_level_;
			{
				//	std::shared_lock lock(max_level_guard_);
				//	max_level_copy = max_level_;
			}
			vector<int> enterpoints;
			//std::vector<Info> neighbors;
			if (cur_level < max_level_copy) {
				int curNodeId = m_enterpoint_id;
				double d = getSaveDistanceId(curNodeId, m_env.get(), cur);
				//double d = qnode->getOriginData().variableDistance(cur_node->getOriginData(), m_env.get());
				double cur_dist = d;
				for (auto i = max_level_copy; i > cur_level; --i) {
					bool changed = true;
					while (changed) {
						changed = false;
						const auto& neighbors = nodes_[curNodeId].getFriends(i);
						for (auto& it : neighbors.neighbors()) {
							d = getSaveDistanceId(it.nodeId(), m_env.get(), cur);
							//d = qnode->getOriginData().variableDistance(iter->GetNode()->getOriginData(), m_env.get());
							if (d < cur_dist) {
								cur_dist = d;
								curNodeId = it.nodeId();
								changed = true;
							}
						}
					}
				}
				enterpoints.push_back(curNodeId);
			}
			else {
				enterpoints.push_back(m_enterpoint_id);
			}

			//  _mm_prefetch(&selecting_policy_, _MM_HINT_T0);
			for (auto i = min(max_level_copy, cur_level); i >= 0; --i) {
				cur.m_visisted.Reset();
				NeighborInfo result;
				SearchAtLayerSingleThread(cur, enterpoints, i, result);
				enterpoints.clear();
				for (auto& it : result.neighbors()) {
					enterpoints.push_back(it.nodeId());
				}
				selecting_policy_->Select(m_, i == 0, result, m_distanceFun);
				Info curEdge;
				for (auto& it : result.neighbors()) {
					curEdge.set(qNodeId, it.value());
					LinkSingleThread(it.nodeId(), curEdge, i);
				}
				nodes_[qNodeId].setFriendsSingleThreadMove(i, result);
			}

			if (cur_level > nodes_[m_enterpoint_id].GetLevel()) {
				m_enterpoint_id = qNodeId;
			}
		}
		void HnswModel::SearchAtLayerSingleThread(
			ThreadLocalInfo& cur,
			const std::vector<int>& enterpoints,
			int level,
			NeighborInfo& result)const
		{
			using namespace std;
			priority_queue<Info, std::vector<Info>, CloserSolFirst> candidates;


			Info curEdge;
			for (const auto& enterpoint : enterpoints) {
				cur.m_visisted.MarkAsVisited(enterpoint);
				//double d = qnode->getOriginData().variableDistance(enterpoint->getOriginData(), m_env.get());
				double d = getSaveDistanceId(enterpoint, m_env.get(), cur);
				curEdge.set(enterpoint, d);
				result.push(curEdge);
				candidates.emplace(enterpoint, d);
			}
			//std::vector<Info> neighbors;
			while (!candidates.empty()) {
				Info candidate = candidates.top();
				float lower_bound = result.top().value();
				if (candidate.value() > lower_bound)
					break;

				int candidate_node = candidate.nodeId();
				const auto& neighbors = nodes_[candidate_node].getFriends(level);
				candidates.pop();
				for (const auto& neighbor : neighbors.neighbors()) {
					int id = neighbor.nodeId();
					if (cur.m_visisted.NotVisited(id)) {
						cur.m_visisted.MarkAsVisited(id);

						double d = getSaveDistanceId(neighbor.nodeId(), m_env.get(), cur);
						//double d = qnode->getOriginData().variableDistance(neighbor.GetNode()->getOriginData(), m_env.get());
						//float d = dist_func_(qnode, neighbor, data_dim_);
						if (result.size() < ef_construction_ || result.top().value() > d) {
							curEdge.set(neighbor.nodeId(), d);
							result.push(curEdge);
							candidates.push(curEdge);
							if (result.size() > ef_construction_)
								result.pop();
						}
					}
				}
			}
		}

		void HnswModel::SearchAtLayerMultiThread(ThreadLocalInfo& cur,
			const std::vector<int>& enterpoints,
			int level,
			NeighborInfo& result)const
		{
			using namespace std;
			priority_queue<Info, std::vector<Info>, CloserSolFirst> candidates;


			Info curEdge;
			for (const auto& enterpoint : enterpoints) {
				cur.m_visisted.MarkAsVisited(enterpoint);
				double d = getSaveDistanceId(enterpoint, m_env.get(), cur);
				//	double d = qnode->getOriginData().variableDistance(enterpoint->getOriginData(), m_env.get());
				curEdge.set(enterpoint, d);
				result.push(curEdge);
				candidates.emplace(enterpoint, d);
			}
			std::set<Info> neighbors;
			while (!candidates.empty()) {
				auto candidate = candidates.top();
				float lower_bound = result.top().value();
				if (candidate.value() > lower_bound)
					break;

				int candidate_node = candidate.nodeId();
				nodes_[candidate_node].getFriends(level, neighbors);
				//const auto& neighbors = candidate_node->getFriends(level);
				//candidate_node->getFriends(neighbors, level);
				candidates.pop();
				for (const auto& neighbor : neighbors) {
					int id = neighbor.nodeId();
					if (cur.m_visisted.NotVisited(id)) {
						cur.m_visisted.MarkAsVisited(id);
						double d = getSaveDistanceId(id, m_env.get(), cur);
						//float d = dist_func_(qnode, neighbor, data_dim_);
						if (result.size() < ef_construction_ ||
							result.top().value() > d) {
							curEdge.set(neighbor.nodeId(), d);
							result.push(curEdge);
							candidates.emplace(curEdge);
							if (result.size() > ef_construction_)
								result.pop();
						}
					}
				}
			}
		}


		void HnswModel::LinkSingleThread(int source, const Info& info, int level)
		{
			nodes_[source].linkSingleThread(info, level, is_naive_, selecting_policy_.get(), m_distanceFun);
		}

		void HnswModel::LinkMultiThread(int source, const Info& info, int level)
		{
			nodes_[source].link(info, level, is_naive_, selecting_policy_.get(), m_distanceFun);
		}
		void HnswModel::MergeEdgesOfTwoGraphsSingleThread(
			int cur, NeighborInfo& result, int level)
		{
			auto& curNeighbor = nodes_[cur].getFriends(level);
			for (auto& it : result.neighbors()) {
				if (!curNeighbor.has(it)) {
					curNeighbor.push(it);
				}
			}
			size_t m = level <= 0 ? max_m0_ : max_m_;
			post_selecting_policy_->Select(m, level == 0, curNeighbor, m_distanceFun);

		}
	}
}
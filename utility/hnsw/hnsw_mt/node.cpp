#include "node.h"

namespace ofec {
	namespace hnsw {
		void Node::link(const Info& info,
			int level,
			bool is_naive_,
			BaseNeighborSelectingPoliciesSol* select_policy,
			std::function<double(int, int)>& distanceFun) {
			using namespace std;
			{
				std::unique_lock lock(mutex_);
				auto& neighbors = friends_at_layer_[level];
				neighbors.push(info);
				selectNeighbors(neighbors, level, max_m_, max_m0_, is_naive_,
					select_policy, distanceFun);
			}

		}

		void Node::linkSingleThread(const Info& info,
			int level,
			bool is_naive_,
			BaseNeighborSelectingPoliciesSol* select_policy,
			std::function<double(int, int)>& distanceFun)
		{
			using namespace std;
			auto& neighbors = friends_at_layer_[level];
			neighbors.push(info);

			selectNeighbors(neighbors, level, max_m_, max_m0_, is_naive_,
				select_policy, distanceFun);
		}


	}
}
#include "select_policy.h"
namespace ofec {
    namespace hnsw {
        void NaiveNeighborSelectingPoliciesSol::Select(
            size_t m, bool select_nn, NeighborInfo& neighbors, std::function<double(int, int)>& distanceFun)
        {
            while (neighbors.size() > m) {
                neighbors.pop();
            }
        }
        void HeuristicNeighborSelectingPoliciesSol::Select(size_t m, bool select_nn,
            NeighborInfo& result, std::function<double(int, int)>& distanceFun)
        {
            if (result.size() <= m) return;

            size_t nn_num = 0;  // # of nearest neighbors
            if (select_nn) {
                nn_num = (size_t)(m * 0.25);
                // m - nn_num neighbors will be chosen as usual with the heuristic algorithm
            }
            size_t nn_picked_num = 0;  // # of nearest neighbors also picked by the heuristic algorithm
            // nn_num - nn_picked_num = # of nearest neighbors selected unconditionally but not picked by the heuristic algorithm 

            std::vector<Info> neighbors, picked;
            // MinHeap<double, HnswSolutionNode*> skipped;
            std::priority_queue<Info, std::vector<Info>, CloserSolFirst> skipped;


            result.getNeighbors(neighbors);

            for (auto it = neighbors.rbegin(); it != neighbors.rend(); it++) {
                bool nn_selected = false;
                if (result.size() < nn_num) {
                    result.push(*it);
                    nn_selected = true;
                }


                bool skip = false;
                for (size_t j = 0; j < picked.size(); ++j) {
                    if (distanceFun(it->nodeId(), picked[j].nodeId()) < it->value()) {
                        skip = true;
                        break;
                    }
                }

                if (!skip) {
                    picked.push_back(*it);
                    if (nn_selected) {
                        // nearest neighbors included in result & picked by the heuristic algorithm
                        ++nn_picked_num;
                    }
                }
                else if (!nn_selected && save_remains_) {
                    skipped.push(*it);
                }

                if (picked.size() - nn_picked_num == m - nn_num)
                    // check if # of neighbors exclusively picked by the heuristic algorithm equals m - nn_num
                    break;
            }

            for (size_t i = nn_picked_num; i < picked.size(); ++i) {
                result.push(picked[i]);
            }

            if (save_remains_) {
                while (result.size() < m && skipped.size()) {
                    result.push(skipped.top());
                    skipped.pop();
                }
            }
        }



        void selectNeighbors(NeighborInfo& neighbors, int level, int maxM, int maxM0,
            bool is_naive_, BaseNeighborSelectingPoliciesSol* select_policy,
            std::function<double(int, int)>& distanceFun)
        {

            bool shrink = (level > 0 && neighbors.size() > maxM) ||
                (level <= 0 && neighbors.size() > maxM0);
            if (!shrink) return;
            if (is_naive_) {
                neighbors.pop();
            }
            else {
                select_policy->Select(neighbors.size() - 1, level == 0, neighbors, distanceFun);
            }
        }
    }
}
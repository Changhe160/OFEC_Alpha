/******************************************************************************
* Project:Open Frameworks for Evolutionary Computation (OFEC)
*******************************************************************************
* Author: YiyaDiao
* Email: diaoyiyacug@163.com
* Language: C++
*-------------------------------------------------------------------------------
*  This file is part of OFEC. This library is free software;
*  you can redistribute it and/or modify it under the terms of the
*  GNU General Public License as published by the Free Software
*  Foundation; either version 2, or (at your option) any later version.
*
*  see https://github.com/Changhe160/OFEC for more information
*
*-------------------------------------------------------------------------------
*
* hnsw model:  multithread
*
*********************************************************************************/

#ifndef NBN_HNSW_MODEL_H
#define NBN_HNSW_MODEL_H


#include <memory>
#include <list>

#include "node.h"
#include "select_policy.h"
#include "../../function/visit_lazy_list.h"
#include "../../../core/problem/solution.h"
#include "../../../core/environment/environment.h"
#include "../../../core/parameter/variants_to_stream.h"

namespace ofec {
    namespace hnsw {


        class HnswModel {

        protected:

            struct GlobalThreadInfo {
                std::mutex m_info_mtx;
                std::set<int> m_calculatingNodes;
                std::list<int> m_tasks;

                void clear() {
                    m_calculatingNodes.clear();
                    m_tasks.clear();
                }
            };


            struct ThreadLocalInfo {
                custom_fun::VisitedLazyList m_visisted;
                custom_fun::VisitedLazyList m_distanceVisited;
                std::vector<double> m_distanceMat;
                int m_curId = -1;

                const ofec::SolutionBase* m_curSol = nullptr;

                std::shared_ptr<ofec::Random> m_rnd = nullptr;


                void intialize(const ofec::SolutionBase* sol) {
                    m_curSol = sol;
                    m_visisted.initialize();
                    m_distanceVisited.initialize();
                    m_distanceMat.clear();
                }

                void intialize() {
                    m_curId = -1;
                    m_visisted.initialize();
                    m_distanceVisited.initialize();
                    m_distanceMat.clear();
                }

                void resize(int size) {
                    m_visisted.resize(size);
                    m_distanceMat.resize(size);
                    m_distanceVisited.resize(size);
                }

                void swapVisited() {
                    std::swap(m_visisted, m_distanceVisited);
                }

            };


            struct SearchGlobalInfo {
                std::vector<const ofec::SolutionBase*> m_tasks;
            };


        public:

            void clear() {
                max_level_ = 0;
                m_enterpoint_id = -1;
                m_threadInfos.clear();
                m_numberThreads = 0;
                nodes_.clear();
                m_solutions.clear();

            }
            void initialize(ofec::Environment* env, ofec::Random* rnd,
                int numThread = 1);

            void copy(const HnswModel& model, double seed=0.5);

            void setNumberThreads(int numThread);

            int addDataSingleThread(ofec::SolutionBase* sol);
       

            void SetConfigs(const std::vector<std::pair<std::string, std::string>>& configs);

            int numberNodes()const {
                return nodes_.size();
            }

            void configsToParameters(ofec::ParameterVariantStream& params);
            void configsfromParameters(ofec::ParameterVariantStream& params);


            void getSolutions(std::vector<const ofec::SolutionBase*>& sols)const;
            void setSolutions(const std::vector<ofec::SolutionBase*>& sols);

            void datasToParameters(ofec::ParameterVariantStream& params);
            void datasFromParameters(ofec::ParameterVariantStream& params);





            const NeighborInfo& getNeighbors(int nodeId, int level = 0) const {
                return nodes_[nodeId].getFriends(level);
            }

            void getTotalNeighbors(int nodeId,
                std::vector<std::vector<int>>& neighbors)const {
                nodes_[nodeId].getNeighborIds(neighbors);
            }
            const Node& node(int idx)const {
                return nodes_[idx];
            }
            const  std::vector<Node>& totalNodes() const {
                return nodes_;
            }

            const  std::vector<std::shared_ptr<ofec::SolutionBase>>& solutions()const {
                return m_solutions;
            }

            std::vector<std::shared_ptr<ofec::SolutionBase>>& solutions() {
                return m_solutions;
            }
            /**
     * @brief Search k nearest items (as vectors) to a query item.
     * @param solutionbase: A solution.
     * @param numberNeighbor: k value.
     * @param ef_search: (default: 50 * k). If you pass a negative value to ef_search,
     *        ef_search will be set as the default value.
     * @param[out] result: ``k`` nearest items.
     */
            void searchBySolutionSingleThread(const ofec::SolutionBase& sol,
                std::vector<Info>& result,
                size_t numberNeighbor, int ef_search = -1) {

                result.clear();
                auto& curInfo = getThreadInfo();
                curInfo.intialize(&sol);
                curInfo.resize(m_solutions.size());

                searchBySolution(curInfo, numberNeighbor, ef_search, m_ensure_k, result);
            }


            void searchBySolution(const ofec::SolutionBase& sol,
                std::vector<Info>& result,
                size_t numberNeighbor, int ef_search = -1) const {

                result.clear();

                ThreadLocalInfo curInfo;
                curInfo.intialize(&sol);
                curInfo.resize(m_solutions.size());

                searchBySolution(curInfo, numberNeighbor, ef_search, m_ensure_k, result);
            }


        protected:


            void searchBySolution(ThreadLocalInfo& curInfo, size_t numberNeighbor, int ef_search, bool ensure_k,
                std::vector<Info>& result)const;
            void CallSearchById_(ThreadLocalInfo& curInfo, const Info& curNode, size_t k, size_t ef_search,
                bool ensure_k, std::vector<Info>& result,
                std::vector<Info>& ensure_k_path_) const {
                if (ensure_k) {
                    while (result.size() < k && !ensure_k_path_.empty()) {
                        auto cur_node = std::move(ensure_k_path_.back());
                        ensure_k_path_.pop_back();
                        // ensure_k_path_.pop_back();
                        SearchById_(curInfo, cur_node, k, ef_search, ensure_k, result);
                    }
                }
                else {
                    SearchById_(curInfo, curNode, k, ef_search, ensure_k, result);
                }
            }

            inline void SearchById_(ThreadLocalInfo& curInfo, const Info& curNode, size_t k, size_t ef_search,
                bool ensure_k, std::vector<Info>& result) const {
                if (ef_search < k)
                    SearchByIdV1_(curInfo, curNode, k, ef_search, ensure_k, result);
                else
                    SearchByIdV2_(curInfo, curNode, k, ef_search, ensure_k, result);
            }

            void SearchByIdV1_(ThreadLocalInfo& curInfo, const Info& curNode, size_t k, size_t ef_search,
                bool ensure_k, std::vector<Info>& result)const;

            void SearchByIdV2_(ThreadLocalInfo& curInfo, const Info& curNode, size_t k, size_t ef_search,
                bool ensure_k, std::vector<Info>& result)const;


            bool PrepareEnsureKSearch(ThreadLocalInfo& curInfo, const Info& curNode, std::vector<Info>& result,
                IdValueMinHeap& visited_nodes)const;

            void MakeSearchResult(size_t k, IdValueMinHeap& candidates, IdValueMinHeap& visited_nodes,
                std::vector<Info>& result)const;

            void assignMemory(int size);

            void SetConfigs(int m, int max_m0, int ef_construction,
                float mult,
                NeighborSelectingPolicy neighbor_selecting, GraphPostProcessing graph_merging);

            void setInfo();

            int GetRandomNodeLevel(ofec::Random* rnd);


            void InsertNodeSingleThread(
                ThreadLocalInfo& cur);
         

            void SearchAtLayerSingleThread(
                ThreadLocalInfo& cur,
                const std::vector<int>& enterpoints,
                int level,
                NeighborInfo& result) const;

            void SearchAtLayerMultiThread(
                ThreadLocalInfo& cur,
                const std::vector<int>& enterpoints,
                int level,
                NeighborInfo& result) const;



            void LinkSingleThread(int source, const Info& info, int level);
            void LinkMultiThread(int source, const Info& info, int level);

            void MergeEdgesOfTwoGraphsSingleThread(int cur, NeighborInfo& result, int level = 0);


            double getSaveDistanceId(
                int otherId,
                ofec::Environment* env,
                ThreadLocalInfo& info
            ) const {
                if (info.m_distanceVisited.NotVisited(otherId)) {
                    info.m_distanceVisited.MarkAsVisited(otherId);
                    info.m_distanceMat[otherId] = m_solutions[info.m_curId]->variableDistance(*m_solutions[otherId], env);
                }
                return  info.m_distanceMat[otherId];
            }



            double getSaveDistanceSolbase(
                int otherId,
                ofec::Environment* env,
                ThreadLocalInfo& info
            ) const {
                if (info.m_distanceVisited.NotVisited(otherId)) {
                    info.m_distanceVisited.MarkAsVisited(otherId);
                    info.m_distanceMat[otherId] = info.m_curSol->variableDistance(*m_solutions[otherId], env);
                }
                return  info.m_distanceMat[otherId];
            }



            void updateSaveDistance(
                int otherId,
                double distance,
                ofec::Problem* pro,
                ThreadLocalInfo& info
            ) const {
                info.m_distanceVisited.MarkAsVisited(otherId);
                info.m_distanceMat[otherId] = distance;
            }

            ThreadLocalInfo& getThreadInfo() {
                static size_t threadId = 0;
                {
                    threadId = (threadId + 1) % m_threadInfos.size();
                }
                return m_threadInfos[threadId];
            }


        protected:

            const std::string n2_signature = "TOROS_N2@N9R4";
            size_t m_ = 12;
            size_t max_m_ = 12;
            size_t max_m0_ = 24;
            size_t ef_construction_ = 150;
            ofec::Real level_mult_ = 1 / log(1.0 * m_);

            bool m_ensure_k = false;

            // int n_threads_ = 1;
            NeighborSelectingPolicy neighbor_selecting_ = NeighborSelectingPolicy::HEURISTIC;
            NeighborSelectingPolicy post_neighbor_selecting_ = NeighborSelectingPolicy::HEURISTIC_SAVE_REMAINS;
            GraphPostProcessing post_graph_process_ = GraphPostProcessing::SKIP;

            bool is_naive_ = false;
            std::unique_ptr<BaseNeighborSelectingPoliciesSol> selecting_policy_;
            std::unique_ptr<BaseNeighborSelectingPoliciesSol> post_selecting_policy_;


            std::function<double(int, int)> m_distanceFun;

            int m_numberThreads = 1;
            std::vector<ThreadLocalInfo> m_threadInfos;
            GlobalThreadInfo  m_globalInfo;

            std::shared_ptr<ofec::Environment> m_env;
            std::shared_ptr<ofec::Random> m_rnd;


            int max_level_ = 0;
            int m_enterpoint_id = -1;
            mutable std::shared_mutex max_level_guard_;

            std::vector<Node> nodes_;
            std::vector<std::shared_ptr<ofec::SolutionBase>> m_solutions;

        };

    }
}
#endif

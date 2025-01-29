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
* four dimensional network for hnsw nbn
* 
* designed based on HNSW
* //https://github.com/kakao/n2 
*
*********************************************************************************/

#ifndef NBN_HNSW_SELECT_POLICY_H
#define NBN_HNSW_SELECT_POLICY_H


#include "neighbor_info.h"
#include <functional>


namespace ofec {
    namespace hnsw {




        /**
         * Graph merging heuristic.
         */
        enum class GraphPostProcessing {
            SKIP = 0, /**< Do not merge (recommended for large-scale data (over 10M)). */
            MERGE_LEVEL0 = 1 /**< Performs an additional graph build in reverse order,
            then merges edges at level 0. So, it takes twice the build time compared to
            ``"skip"`` but shows slightly higher accuracy. (recommended for data under 10M scale). */
        };

        /**
         * Neighbor selecting policy.
         */
        enum class NeighborSelectingPolicy {
            NAIVE = 0, /**< Select closest neighbors (not recommended). */
            HEURISTIC = 1, /**< Select neighbors using algorithm4 on HNSW paper (recommended). */
            HEURISTIC_SAVE_REMAINS = 2, /**< Experimental. */
        };



        class BaseNeighborSelectingPoliciesSol {
        public:
            BaseNeighborSelectingPoliciesSol() {}
            virtual ~BaseNeighborSelectingPoliciesSol() {}

            virtual void Select(size_t m,
                bool select_nn,
                NeighborInfo& neighbors, std::function<double(int, int)>& distanceFun) = 0;
        };

        class NaiveNeighborSelectingPoliciesSol : public BaseNeighborSelectingPoliciesSol {
        public:
            NaiveNeighborSelectingPoliciesSol() {}
            ~NaiveNeighborSelectingPoliciesSol() override {}
            void Select(size_t m, bool select_nn,
                NeighborInfo& neighbors, std::function<double(int, int)>& distanceFun) override;
        };



        class HeuristicNeighborSelectingPoliciesSol : public BaseNeighborSelectingPoliciesSol {
        public:
            HeuristicNeighborSelectingPoliciesSol() : save_remains_(false) {}
            HeuristicNeighborSelectingPoliciesSol(bool save_remain) : save_remains_(save_remain) {}
            ~HeuristicNeighborSelectingPoliciesSol() override {}
            /**
             * Returns selected neighbors to result
             * (analagous to SELECT-NEIGHBORS-HEURISTIC in Yu. A. Malkov's paper.)
             *
             * select_nn: if true, select 0.25 * m nearest neighbors to result without applying the heuristic algorithm
             */
            void Select(size_t m, bool select_nn,
                NeighborInfo& result, std::function<double(int, int)>& distanceFun) override;


        private:
            bool save_remains_;
        public:

        };


        extern void selectNeighbors(
            NeighborInfo& neighbors,
            int level, int maxM, int maxM0, bool is_naive_,
            BaseNeighborSelectingPoliciesSol* select_policy,
            std::function<double(int, int)>& distanceFun);



    }
}


#endif // !NBN_SELECT_STRATEGY_H


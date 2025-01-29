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
* node 
*
*********************************************************************************/

#ifndef NBN_HNSW_NODE_H
#define NBN_HNSW_NODE_H

#include <mutex>
#include <vector>
#include <shared_mutex>

#include "neighbor_info.h"
#include "select_policy.h"

namespace ofec {
    namespace hnsw {


        class Node {
        public:

            //   using Info = std::pair<HnswSolutionNode*, double> ;


            Node() = default;
            Node(int id, int level, size_t max_m, size_t max_m0)
                : id_(id), level_(level), max_m_(max_m), max_m0_(max_m0), friends_at_layer_(level + 1) {}

            Node(const Node& a) {
                id_ = a.id_;
                level_ = a.level_;
                max_m_ = a.max_m_;
                max_m0_ = a.max_m0_;
                friends_at_layer_ = a.friends_at_layer_;
            }

            Node& operator = (const Node& a) {
                id_ = a.id_;
                level_ = a.level_;
                max_m_ = a.max_m_;
                max_m0_ = a.max_m0_;
                friends_at_layer_ = a.friends_at_layer_;
                return *this;
            }

            void set(int id, int level,
                size_t max_m,
                size_t max_m0) {
                id_ = id;
                level_ = level;
                max_m_ = max_m;
                max_m0_ = max_m0;
                friends_at_layer_.clear();
                friends_at_layer_.resize(level + 1);
            }

            inline int GetId() const { return id_; }
            inline int GetLevel() const { return level_; }
            inline void setLevel(int level) {
                level_ = level;
            }
            //inline int level() { return level_; }
            inline size_t GetMaxM() const { return max_m_; }
            inline size_t GetMaxM0() const { return max_m0_; }



            inline void getFriends(int level, std::set<Info>& neighbors)const {
                std::shared_lock lock(mutex_);
                neighbors = friends_at_layer_[level].neighbors();
            }

            inline const std::set<Info>& getFriendsInfo(int level) const {
                //std::shared_lock lock(mutex_);
                return friends_at_layer_[level].neighbors();
            }

            inline NeighborInfo& getFriends(int level) {
                //std::shared_lock lock(mutex_);
                return friends_at_layer_[level];
            }

            inline const NeighborInfo& getFriends(int level)const {
                //std::shared_lock lock(mutex_);
                return friends_at_layer_[level];
            }


            void link(const Info& info, int level,
                bool is_naive_,
                BaseNeighborSelectingPoliciesSol* select_policy,
                std::function<double(int, int)>& distanceFun);
            void linkSingleThread(const Info& info, int level,
                bool is_naive_,
                BaseNeighborSelectingPoliciesSol* select_policy,
                std::function<double(int, int)>& distanceFun
            );

            const std::vector<NeighborInfo>& getTotalFriends()const {
                return friends_at_layer_;
            }
            std::vector<NeighborInfo>& getTotalFriends() {
                return friends_at_layer_;
            }
            //inline void SetFriendsMove(int level,NeighborInfo& new_friends) {
            //    std::unique_lock lock(mutex_);
            //    friends_at_layer_[level] = std::move(new_friends);
            //}
            inline void setFriendsSingleThreadMove(int level, NeighborInfo& new_friends) {
                // std::unique_lock lock(mutex_);
                friends_at_layer_[level] = std::move(new_friends);
            }


            inline void setFriendsSingleThread(int level, const NeighborInfo& new_friends) {
                // std::unique_lock lock(mutex_);
                friends_at_layer_[level] = std::move(new_friends);
            }

            //inline void setFriendsSingleThreadMove(int level,NeighborInfo& new_friends) {
            //    // std::unique_lock lock(mutex_);
            //    friends_at_layer_[level] = std::move(new_friends);
            //}
            void getNeighborIds(std::vector<std::vector<int>>& neighbors)const {
                neighbors.resize(friends_at_layer_.size());
                for (int idx(0); idx < neighbors.size(); ++idx) {
                    neighbors[idx].clear();
                    for (auto& it : friends_at_layer_[idx].neighbors()) {
                        neighbors[idx].push_back(it.nodeId());
                    }
                }
            }


            std::shared_mutex& mutex() {
                return mutex_;
            }

            bool operator==(const Node& n2) const {
                return id_ == n2.id_ && level_ == n2.level_
                    && max_m_ == n2.max_m_ && max_m0_ == n2.max_m0_
                    ;      //          && friends_at_layer_ == n2.friends_at_layer_;
            }
            bool operator!=(const Node& n2) const {
                return id_ != n2.id_ || level_ != n2.level_
                    || max_m_ != n2.max_m_ || max_m0_ != n2.max_m0_;
                //            || friends_at_layer_ != n2.friends_at_layer_;
            }
        private:

            int id_;
            int level_;
            size_t max_m_;
            size_t max_m0_;

            std::vector<NeighborInfo> friends_at_layer_;
            mutable std::shared_mutex mutex_;
        };



    } // namespace n2

}
#endif


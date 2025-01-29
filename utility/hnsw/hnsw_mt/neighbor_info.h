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
*********************************************************************************/

#ifndef NBN_HNSW_INFO_STRUCT_H
#define NBN_HNSW_INFO_STRUCT_H


#include <set>
#include <map>
#include <vector>
#include <queue>

#include "../../../core/definition.h"
namespace ofec {
    namespace hnsw {

        class Info {
        protected:

            int m_nodeId = -1;
            ofec::Real m_value = 0;
            ofec::Real m_randValue = 0;
        public:

            inline ofec::Real value() const { return m_value; }
            inline ofec::Real& value() { return m_value; }
            inline int nodeId()const { return m_nodeId; }
            inline int& nodeId() { return m_nodeId; }
            inline ofec::Real randValue()const { return m_randValue; }
            inline ofec::Real& randValue() { return m_randValue; }
            void set(int nodeId, ofec::Real value) {
                m_nodeId = nodeId;
                m_value = value;
            }

            Info() = default;
            Info(int nodeId, ofec::Real value) : m_nodeId(nodeId), m_value(value) {};
            Info(const Info& other) {
                m_nodeId = other.m_nodeId;
                m_value = other.m_value;
            };

            Info& operator=(const Info& other) {
                m_nodeId = other.m_nodeId;
                m_value = other.m_value;
                return *this;
            };

            bool operator==(const Info& other) const {
                if (m_nodeId == other.m_nodeId)
                    return true;
                return false;
            };
        };


        inline bool operator<(const Info& lhs, const Info& rhs)
        {
            return lhs.nodeId() < rhs.nodeId();
        }


        class FurtherSolFirst {
        public:
            bool operator() (const Info& a, const Info& b)
            {
                if (a.value() == b.value()) {
                    return  a.nodeId() < b.nodeId();
                }
                return a.value() < b.value();
            }
        };



        class CloserSolFirst {
        public:
            bool operator() (const Info& a, const Info& b)
            {
                if (a.value() == b.value()) {
                    return  a.nodeId() > b.nodeId();
                }
                return a.value() > b.value();
            }

        };
        class FurtherRandomSolFirst {
        public:
            bool operator() (const Info& a, const Info& b)
            {
                if (a.value() == b.value()) {
                    return  a.randValue() < b.randValue();
                }
                return a.value() < b.value();
            }
        };


        class CloserRandomSolFirst {
        public:
            bool operator() (const Info& a, const Info& b)
            {
                if (a.value() == b.value()) {
                    
                    return  a.randValue() > b.randValue();
                }
                return a.value() > b.value();
            }

        };


        using IdValueMinHeap = std::priority_queue<Info, std::vector<Info>, CloserSolFirst>;
        using IdValueMaxHeap = std::priority_queue<Info, std::vector<Info>, FurtherSolFirst>;



        struct NeighborInfo {
        protected:
            std::set<Info> ms_neighbors;
            std::priority_queue<Info, std::vector<Info>, FurtherSolFirst>
                m_results;

        public:

            NeighborInfo() = default;
            ~NeighborInfo() = default;


            bool operator==(const NeighborInfo& n2) const {
                return ms_neighbors == n2.ms_neighbors;
            }
            bool operator!=(const NeighborInfo& n2) const {
                return ms_neighbors != n2.ms_neighbors;
            }

            const std::set<Info>& neighbors()const {
                return ms_neighbors;
            }

            void getNeighbors(std::vector<Info>& neighbors) {
                while (!m_results.empty()) {
                    neighbors.push_back(m_results.top());
                    m_results.pop();
                }
                ms_neighbors.clear();
            }

            const Info& top() {
                return m_results.top();
            }

            void emplace(Info&& info) {
                ms_neighbors.insert(info);
                m_results.emplace(info);
            }

            void push(const Info& info) {
                ms_neighbors.insert(info);
                m_results.push(info);
            }
            void pop() {
                auto& topValue = m_results.top();
                ms_neighbors.erase(topValue);
                m_results.pop();;

            }

            void clear() {
                ms_neighbors.clear();
                while (!m_results.empty()) m_results.pop();
            }
            bool empty() {
                return ms_neighbors.empty();
            }
            inline size_t size()const {
                return ms_neighbors.size();
            }
            bool has(const Info& cur) {
                return ms_neighbors.find(cur) != ms_neighbors.end();
            }

        };

    }
}

#endif //  HNSW_INFO_STRUCT_H
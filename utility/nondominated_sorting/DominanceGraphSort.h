#ifndef D_GRAPH
#define D_GRAPH

#include <vector>
#include <memory>
#include <map>
#include <utility>
#include "../../utility/functional.h"

namespace NDS {
	template<typename T>
	struct dg_node {
		int m_index;
		int m_rank;
		std::list<int> m_children;
		std::list<int> m_parents;
		const std::vector<T> m_obj;
		bool m_compared;
		dg_node(int index, const std::vector<T>& obj) : m_index(index), m_obj(obj), m_rank(0), m_compared(false) {}
	};

	template<typename T>
	class dg_graph {
	private:
		std::vector<std::unique_ptr<dg_node<T>>> m_nodes;
		int m_nodeNum;
		std::vector<std::list<int>> m_ranks;
		int m_comparisons;
		int m_objNum;
	private:
		// when dominating a node, dominating its all offpsring
		void dominateChildren(std::unique_ptr<dg_node<T>>& toAdd, int node, std::vector<int>& D) {
			if (!m_nodes[node]->m_compared) {
				toAdd->m_children.push_back(node);
				m_nodes[node]->m_parents.push_back(toAdd->m_index);
				m_nodes[node]->m_compared = true;
				D.push_back(node);
				for (int child : m_nodes[node]->m_children)
					dominateChildren(toAdd, child, D);
			}
		}
		// when parent's rank increasd, its child's rank might increased
		void update_child1(const int child, const int parent) {
			if (m_nodes[child]->m_rank < m_nodes[parent]->m_rank + 1) {
				m_ranks[m_nodes[child]->m_rank].erase(std::find(m_ranks[m_nodes[child]->m_rank].begin(), m_ranks[m_nodes[child]->m_rank].end(), child));
				m_nodes[child]->m_rank = m_nodes[parent]->m_rank + 1;
				if (m_nodes[child]->m_rank > (int)m_ranks.size() - 1)
					m_ranks.push_back(std::list<int>(1, child));
				else
					m_ranks[m_nodes[child]->m_rank].push_back(child);
				for (int _child : m_nodes[child]->m_children)
					update_child1(_child, child);
			}
		}
		// when a node's rank decreasd, child's rank might decrease
		void update_child2(const int child) {
			int tempRank(0);
			for (int _parent : m_nodes[child]->m_parents)
				if (tempRank < m_nodes[_parent]->m_rank + 1)
					tempRank = m_nodes[_parent]->m_rank + 1;
			if (tempRank < m_nodes[child]->m_rank) {
				// if decreased
				m_ranks[m_nodes[child]->m_rank].erase(std::find(m_ranks[m_nodes[child]->m_rank].begin(), m_ranks[m_nodes[child]->m_rank].end(), child));
				m_nodes[child]->m_rank = tempRank;
				m_ranks[m_nodes[child]->m_rank].push_back(child);
				// all its children's rank might decrease
				for (int _child : m_nodes[child]->m_children)
					update_child2(_child);
			}
		}
	public:
		int numrank() { return m_ranks.size(); }
		dg_graph(std::vector<std::vector<T>>& data) : m_nodeNum(0), m_comparisons(0) {
			if (!data.empty())
				m_objNum = data[0].size();
			for (std::vector<T>& obj : data)
				insert(obj);
		}
		dg_graph() : m_nodeNum(0), m_comparisons(0) {}
		int insert(const std::vector<T>& obj) {
			m_nodes.push_back(std::unique_ptr<dg_node<T>>(new dg_node<T>(m_nodeNum, obj)));
			std::unique_ptr<dg_node<T>>& toAdd = m_nodes[m_nodeNum]; //
			m_nodeNum++;

			std::vector<int> P; // store indexs of nodes that dominating 'toAdd' node
			std::vector<int> D; // store indexs of nodes that dominated by 'toAdd' node

			for (std::list<int>& rank : m_ranks) {
				for (int node : rank) {
					if (!m_nodes[node]->m_compared) {
						std::pair<OFEC::dominationship, int> compare_result = OFEC::objective_compare(toAdd->m_obj, m_nodes[node]->m_obj, OFEC::optimization_mode::Minimization);
						m_comparisons += compare_result.second;
						if (compare_result.first == OFEC::dominationship::Dominating) {
							dominateChildren(toAdd, node, D);
						}
						else if (compare_result.first == OFEC::dominationship::Dominated) {
							m_nodes[node]->m_children.push_back(toAdd->m_index);
							toAdd->m_parents.push_back(node);
							m_nodes[node]->m_compared = true;
							P.push_back(node);
						}
						else
							m_nodes[node]->m_compared = true;
					}
				}
			}
			// reinitialize the compared state of all nodes
			for (std::unique_ptr<dg_node<T>>& node : m_nodes)
				node->m_compared = false;

			//from P, get toAdd node's rank
			for (int p : P)
				if (toAdd->m_rank < m_nodes[p]->m_rank + 1)
					toAdd->m_rank = m_nodes[p]->m_rank + 1;

			if (toAdd->m_rank > int(m_ranks.size()) - 1)
				m_ranks.push_back(std::list<int>(1, toAdd->m_index));
			else
				m_ranks[toAdd->m_rank].push_back(toAdd->m_index);

			//update all nodes' rank in D  
			for (int d : D)
				update_child1(d, toAdd->m_index);

			return m_nodeNum - 1;
		}

		void _delete(const int index) {
			std::unique_ptr<dg_node<T>>& toDelete = m_nodes[index];
			// delete 'toDelete' node from its parents' m_children
			for (int parent : toDelete->m_parents)
				m_nodes[parent]->m_children.erase(std::find(m_nodes[parent]->m_children.begin(), m_nodes[parent]->m_children.end(), index));
			//delte  'toDelete' node from m_ranks
			m_ranks[toDelete->m_rank].erase(std::find(m_ranks[toDelete->m_rank].begin(), m_ranks[toDelete->m_rank].end(), index));
			toDelete->m_rank = -1;
			// delete 'toDelete' node from its children's m_parents
			for (int child : toDelete->m_children) {
				// child's rank might decrease
				update_child2(child);
				m_nodes[child]->m_parents.erase(std::find(m_nodes[child]->m_parents.begin(), m_nodes[child]->m_parents.end(), index));
			}
			//delete 'toDelete' node
			toDelete.reset();
			//update indexs of nodes after 'toDelete' node in m_nodes
			for (int i = index; i < m_nodeNum - 1; ++i) {
				m_nodes[i] = std::move(m_nodes[i + 1]);
				m_nodes[i]->m_index = i;
			}
			//update indexs of nodes after 'toDelete' node in m_ranks
			for (std::list<int>& rank : m_ranks)
				for (int& node : rank)
					if (node > index)
						node--;
			//delete space
			m_nodes.pop_back();
			m_nodeNum--;
			//update indexs of nodes after 'toDelete' node in m_parents and m_children of each node
			for (auto& node : m_nodes) {
				for (int& child : node->m_children)
					if (child > index)
						child--;
				for (int& parent : node->m_parents)
					if (parent > index)
						parent--;
			}
		}

		const std::map<int, int> ranking() {
			std::map<int, int> ranking;
			for (std::unique_ptr<dg_node<T>>& node : m_nodes)
				ranking.emplace(node->m_index, node->m_rank);
			return ranking;
		}
		const int number() { return m_comparisons; }
	};
}

#endif // !D_GRAPH


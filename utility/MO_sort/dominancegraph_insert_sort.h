#ifndef DOMINANCEGRAPH_INSERT_SORT_H
#define DOMINANCEGRAPH_INSERT_SORT_H

#include <iostream>
#include <map>
#include <utility>
#include <limits>
#include "../../core/algorithm/encoding.h"
#include "../../core/definition.h"

using namespace std;
using namespace OFEC;

namespace dominance_graph {

	class node {
	private:
		const int m_index;
		const vector<double> m_obj;
		int m_ranking;
		vector<node*> m_parents;
		vector<node*> m_children;

	public:
		node(vector<double>& obj, int index) :m_obj(obj), m_index(index), m_ranking(0) {}
		void set_ranking(int ranking) { m_ranking = ranking; }
		void add_parent(node* parent) { m_parents.push_back(parent); }
		const vector<node*>& get_parents() const { return m_parents; }
		const vector<node*>& get_children() const { return m_children; }
		const vector<double>& get_obj() const { return m_obj; }
		const int get_index() const { return m_index; }
		const int get_ranking() const { return m_ranking; }
		void delete_parent(node* parent) { m_parents.erase(find(m_parents.begin(), m_parents.end(), parent)); }
		void add_child(node* child) { m_children.push_back(child); }
		void delete_child(node* child) { m_children.erase(find(m_children.begin(), m_children.end(), child)); }
	};

	class graph {
	private:
		node m_root;
		vector<node> m_nodes;
		map<int, int> m_ranking;
		size_t m_nodecount;
		objective_compare<> m_compare;
		size_t m_comparisons;
		dominationship m_compareResult;
		
		bool compare_node(vector<int>& compared, node* tobecompared, node* tocompare) { //return wheather tocompare is dominated by tobecompared or its children

			if (tobecompared->get_index() == -1) // if tobecompared is root node
				m_compareResult = dominationship::Dominated;
			else {
				if (compared[tobecompared->get_index()] != 0) {// if tobecompared node has been compared with
					if (compared[tobecompared->get_index()] == 1)
						return true;
					else
						return false;
				}

				m_compareResult = m_compare(tocompare->get_obj(), tobecompared->get_obj(), optimization_mode::Minimization);
				m_comparisons++;
				cout << "comparison: " << m_comparisons << endl;
				if (m_compareResult == dominationship::Dominated)
					compared[tobecompared->get_index()] = 1;
				else if (m_compareResult == dominationship::Non_dominated)
					compared[tobecompared->get_index()] = 3;
			}

			if (m_compareResult == dominationship::Dominated) {

				bool dominated_bychild(false);

				for (auto& child : tobecompared->get_children()) {
					if (compare_node(compared, child, tocompare))
						dominated_bychild = true;
				}
				if (!dominated_bychild) {
					tobecompared->add_child(tocompare);
					tocompare->add_parent(tobecompared);
				}

				return true;
			}
			else if (m_compareResult == dominationship::Dominating) {

				tocompare->add_child(tobecompared);
				tobecompared->add_parent(tocompare);
				comparedallchildren(compared, tobecompared);
				return false;
			}
			else if (m_compareResult == dominationship::Non_dominated) {

				for (auto& child : tobecompared->get_children())
					compare_node(compared, child, tocompare);
				return false;
			}
			else
				throw("comparing error!");

		}

		void comparedallchildren(vector<int>& compared, node* _node) {
			compared[_node->get_index()] = 2;
			for (auto& child : _node->get_children())
				comparedallchildren(compared, child);
		}

	public:
		graph(vector<vector<double>> & data) : m_root(vector<double>(2, numeric_limits<double>::min()), -1), m_nodecount(0), m_comparisons(0) {
			for (auto& obj : data) {
				m_nodes.push_back(node(obj, m_nodecount));
				m_nodecount++;
			}

			for (size_t i = 0; i < m_nodecount; i++) {
				vector<int> compared(i, 0);
				compare_node(compared, &m_root, &m_nodes[i]);
				cout << "node: " << i << endl;
			}
		}

		//void add_node(vector<double>& obj) {
		//	m_nodes.push_back(node(obj, m_nodecount));
		//	link_node(&m_root, &m_nodes[m_nodecount]);
		//	m_nodecount++;
		//}

		map<int, int>& update_ranking() {

			vector<size_t> _n(m_nodecount, 0);
			vector<vector<int>> _S(m_nodecount);

			for (size_t i = 0; i < m_nodecount; i++) {
				_n[i] = m_nodes[i].get_parents().size();
				for (auto& child : m_nodes[i].get_children())
					_S[i].push_back(child->get_index());
			}

			for (auto& child : m_root.get_children()) {
				_n[child->get_index()]--;
			}


			vector<vector<int>> F;
			F.push_back({});
			for (size_t i = 0; i < m_nodecount; i++)
				if (_n[i] == 0) {
					F[0].push_back(i);
					m_nodes[i].set_ranking(1);
					m_ranking.emplace(m_nodes[i].get_index(), 0);
				}

			int i = 0;
			while (!F[i].empty()) {
				vector<int> Q;
				for (int p : F[i]) {
					for (int q : _S[p]) {
						_n[q]--;
						if (_n[q] == 0) {
							m_nodes[q].set_ranking(i + 2);
							m_ranking.emplace(m_nodes[q].get_index(), i+1);
							Q.push_back(q);
						}
					}
				}
				i++;
				F.push_back(Q);
			}

			return m_ranking;
		}

		int number() { return m_comparisons; }
	};

}


#endif // !DOMINANCEGRAPH_INSERT_SORT_H
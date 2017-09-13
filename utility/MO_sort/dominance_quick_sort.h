#ifndef DOMINANCE_GRAPH_H
#define DOMINANCE_GRAPH_H

#include "../../core/algorithm/encoding.h"
#include "../../core/definition.h"

using namespace std;
using namespace OFEC;

namespace dominance_graph {

	class node {
	private:
		int m_ranking;
		const int m_label;
		const vector<double> m_obj;
		vector<node*> m_parents;
		//vector<node*> m_children;

	public:
		node(vector<double>& obj, int label) :m_obj(obj), m_ranking(0), m_label(label) {}
		void set_ranking(int ranking) { m_ranking = ranking; }
		void add_parent(node* parent) { m_parents.push_back(parent); }
		const vector<node*>& get_parents() { return m_parents; }
		const vector<double>& get_obj() { return m_obj; }
		const int get_label() { return m_label; }
		const int get_ranking() { return m_ranking; }
		//void delete_parent(node* parent) { m_parents.erase(find(m_parents.begin(), m_parents.end(), parent)); }
		//void add_child(node* child) { m_children.push_back(child); }
		//void delete_child(node* child) { m_children.erase(find(m_children.begin(), m_children.end(), child)); 
	};

	class graph {
	private:
		vector<node> m_nodes;
		size_t m_nodecount;
		objective_compare<> m_compare;
		size_t m_comparisons;
		vector<int> m_index;
		bool m_sorted;
		map<int, int> m_label;
		
		void quick_sort(vector<int>& all) {
			if (all.size() <= 1)
				return;
			vector<int> state(all.size(), 0);
			size_t i = 0;
			while (i < all.size()) {
				if (state[i] != 0) {
					i++;
					continue;
				}
				else {
					int key = all[i];
					for (size_t j = 0; j < all.size(); j++) {
						int index = all[j];
						if (key != index) {
							dominationship compare_rusult = m_compare(m_nodes[key].get_obj(), m_nodes[index].get_obj(), optimization_mode::Minimization);
							m_comparisons++;
							if (compare_rusult == dominationship::Dominating) {
								m_nodes[index].add_parent(&m_nodes[key]);
								if (state[j] == 0)
									state[j] = -1;
								else if (state[j] == 1)
									throw("something went wrong during comparing!");
							}
							else if (compare_rusult == dominationship::Dominated) {
								m_nodes[key].add_parent(&m_nodes[index]);
								if (state[j] == 0) 
									state[j] = 1;
								else if (state[j] == -1)
									throw("something went wrong during comparing!");
							}
						}
					}
					i++;
				}
			}
			vector<int> left, right;
			for (size_t i = 0; i < all.size(); i++) {
				if (state[i] == -1)
					left.push_back(all[i]);
				//else if (state[index] == 0) {
				//	left.push_back(index);
				//	right.push_back(index);
				//}
				else if (state[i] == 1)
					right.push_back(all[i]);
			}
			quick_sort(left);
			quick_sort(right);
		}
	public:
		graph(vector<vector<double>>& objs) : m_nodecount(0), m_comparisons(0) ,m_sorted(false) {
			for (size_t i = 0; i < objs.size(); i++) {
				m_nodes.push_back(node(objs[i], i));
				m_nodecount++;
			}
		}
		~graph() {}
		
		size_t number() { return m_comparisons; }

		void quick_sort() {
			m_index.clear();
			for (size_t i = 0; i < m_nodecount; i++)
				m_index.push_back(i);
			m_comparisons = 0;
			quick_sort(m_index);
			m_sorted = true;
		}

		void set_ranking() {

			if (!m_sorted)
				quick_sort();

			vector<size_t> _n(m_nodecount, 0);
			vector<vector<int>> _S(m_nodecount);
			for (size_t i = 0; i < m_nodecount; i++) {
				auto parents = m_nodes[i].get_parents();
				_n[i] = parents.size();
				for (auto parent : parents)
					_S[parent->get_label()].push_back(i);
			}

			vector<vector<int>> F;
			F.push_back({});
			for (size_t i = 0; i < m_nodecount; i++)
				if (_n[i] == 0) {
					F[0].push_back(i);
					m_nodes[i].set_ranking(0);
				}

			int i = 0;
			while (!F[i].empty()) {
				vector<int> Q;
				for (int p : F[i]) {
					for (int q : _S[p]) {
						_n[q]--;
						if (_n[q] == 0) {
							m_nodes[q].set_ranking(i + 1);
							Q.push_back(q);
						}
					}
				}
				i++;
				F.push_back(Q);
			}

			//for (auto _rank : F) {
			//	for (int _node : _rank) {
			//		cout << _node << ":(";
			//		for (auto element : m_nodes[_node].get_obj())
			//			cout << element << " ";
			//		cout << ")" << "\t";
			//	}
			//	cout << endl;
			//}
		}

		map<int, int>& ranking() {
			m_label.clear();
			for (int i = 0; i < m_nodecount; ++i) {
				m_label.emplace(m_nodes[i].get_label(), m_nodes[i].get_ranking());
			}
			return m_label;
		}

	};
}
#endif // !DOMINANCE_TREE_H

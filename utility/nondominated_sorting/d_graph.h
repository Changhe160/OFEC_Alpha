#ifndef D_GRAPH2
#define D_GRAPH2

#include "../../core/algorithm/encoding.h"
#include "../../core/definition.h"

template<typename T>
class dg_node {
public:
	const int m_index;
	int m_rank;
	vector<int> m_children;
	vector<T> m_obj;
	bool m_compared;
	dg_node(int index, vector<T>& obj) : m_index(index), m_obj(obj), m_rank(0), m_compared(false){}
};

template<typename T>
class dg_graph {
private:
	vector<dg_node<T>> m_nodes;
	int m_nodeNum;
	vector<vector<int>> m_rank;
	OFEC::objective_compare<> m_compare;
	int m_comparisons;
	int m_objNum;

	void ignoreChildren(dg_node<T>& toadd, int node, vector<int>& D) {
		if (!m_nodes[node].m_compared) {
			toadd.m_children.push_back(node);
			m_nodes[node].m_compared = true;
			D.push_back(node);
			for (int& child : m_nodes[node].m_children)
				ignoreChildren(toadd, child, D);
		}
	}

	void deepenChildren(int child, int parent) {
		if (m_nodes[child].m_rank < m_nodes[parent].m_rank + 1) {
			m_rank[m_nodes[child].m_rank].erase(find(m_rank[m_nodes[child].m_rank].begin(), m_rank[m_nodes[child].m_rank].end(), m_nodes[child].m_index));
			m_nodes[child].m_rank = m_nodes[parent].m_rank + 1;
			if (m_nodes[child].m_rank > m_rank.size() - 1)
				m_rank.push_back(vector<int>(1, m_nodes[child].m_index));
			else
				m_rank[m_nodes[child].m_rank].push_back(m_nodes[child].m_index);
			for (int& _child : m_nodes[child].m_children)
				deepenChildren(_child, child);
		}
	}

public:
	dg_graph(vector<vector<T>>& data) : m_nodeNum(0), m_comparisons(0){
		if (!data.empty())
			m_objNum = data[0].size();
		for (vector<T>& obj : data)
			addNode(obj);
	}
	void addNode(vector<T>& obj) {
		m_nodes.push_back(dg_node<T>(m_nodeNum, obj));
		dg_node<T>& toadd = m_nodes[m_nodeNum];
		m_nodeNum++;

		vector<int> P;
		vector<int> D;

		for (vector<int>& rank : m_rank) {
			for (int& node : rank) {
				if (!m_nodes[node].m_compared) {
					OFEC::dominationship compare_result = m_compare(toadd.m_obj, m_nodes[node].m_obj, OFEC::optimization_mode::Minimization);
					m_comparisons += m_objNum;
					if (compare_result == OFEC::dominationship::Dominating) {
						ignoreChildren(toadd, node, D);
					}
					else if (compare_result == OFEC::dominationship::Dominated) {
						m_nodes[node].m_children.push_back(toadd.m_index);
						m_nodes[node].m_compared = true;
						P.push_back(node);
					}
					else
						m_nodes[node].m_compared = true;
				}
			}
		}
		for (dg_node<T>& node : m_nodes)
			node.m_compared = false;

		for (int& p : P)
			if (toadd.m_rank < m_nodes[p].m_rank + 1) 
				toadd.m_rank = m_nodes[p].m_rank + 1;

		if (toadd.m_rank > int(m_rank.size()) - 1)
			m_rank.push_back(vector<int>(1, toadd.m_index));
		else
			m_rank[toadd.m_rank].push_back(toadd.m_index);

		for (int& d : D)
			deepenChildren(d, toadd.m_index);
			//if (m_nodes[d].m_rank < toadd.m_rank + 1) {
			//	m_rank[m_nodes[d].m_rank].erase(find(m_rank[m_nodes[d].m_rank].begin(), m_rank[m_nodes[d].m_rank].end(), m_nodes[d].m_index));
			//	m_nodes[d].m_rank = toadd.m_rank + 1;
			//	if (m_nodes[d].m_rank > m_rank.size() - 1)
			//		m_rank.push_back(vector<int>(1, m_nodes[d].m_index));
			//	else
			//		m_rank[m_nodes[d].m_rank].push_back(m_nodes[d].m_index);
			//	for (int& child : m_nodes[d].m_children)
			//		deepenChildren(child, d);
			//}
	}

	const map<int, int> ranking() { 
		map<int, int> ranking;
		for (dg_node<T>& node : m_nodes)
			ranking.emplace(node.m_index, node.m_rank);
		return ranking; 
	}

	const int number() { return m_comparisons; }
};

#endif // !D_GRAPH2


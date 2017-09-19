#ifndef DOMINANCE_GRAPH_H
#define DOMINANCE_GRAPH_H

#include <iostream>
#include <list>
#include <map>
#include <utility>
#include <limits>
#include <memory>
#include "../../core/algorithm/encoding.h"
#include "../../core/definition.h"

using namespace std;

template<typename T>
struct d_node {
	const int m_index;
	const vector<T> m_obj;
	bool m_compared;
	int m_ranking;
	vector<d_node*> m_children;
	d_node(int index, vector<T>&& obj) : m_index(index), m_obj(obj) {}
};

template<typename T = double>
class d_graph {
private:
	d_node<T>* m_root;
	OFEC::objective_compare<> m_compare;
	int m_comparisons(0);
	vector<unique_ptr<d_node<T>*>> m_nodes;
	int m_nodesnum(0);

private:
	template<typename visit, typename ...Args>
	void travel(d_node<T>* p, visit f, Args&&... args) {
		list<d_node<T>*> l(1, p);
		while (!l.empty()) {
			d_node<T>* q = *l.begin();
			l.pop_front();

			for (auto &i : q->m_children)
				if (i != nullptr)
					l.push_back(i);

			f(p,std::forward<Args>(args)...);

		}
	}
	void compare(d_node<T>* p, d_node<T>* node) {

	}
	void mark(d_node<T>* p, bool compared) { 
	
		travel(m_root, [=](d_node<T>* p) {p->m_compared = compared; })
	}
	void setrank(d_node<T>* p, int ranking) {
		//travel(m_root, [](d_node<T>* p, int ranking) {p->m_ranking = ranking; })
	}
public:
	d_graph(vector<pair<int, vector<T>>>& data) : m_root(new d_node<T>(-1, std::move(vector<T>(2, numeric_limits<T>::min())))) {
		for (pair<int, vector<T>>& member : data) {
			unique_ptr<d_node<T>*> new_node(member.first, member.second);
			m_nodes.push_back(std::move(new_node));
			travel(m_root, compare(, m_nodes[m_nodesnum]));

		}
	}
	void add_node(){}

};
#endif // !DOMINANCE_GRAPH_H


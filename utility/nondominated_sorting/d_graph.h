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
	d_node(int index, vector<T>& obj) : m_index(index), m_obj(obj), m_ranking(0), m_compared(false) {}
};

template<typename T = double>
class d_graph {
private:
	d_node<T>* m_root;
	OFEC::objective_compare<> m_compare;
	int m_comparisons;
	vector<d_node<T>*> m_nodes;
	int m_nodesnum;
	map<int, int> m_ranking;
private:
	template<typename visit, typename ...Args>
	void travel(d_node<T>* p, visit f, Args&&... args) {
		list<d_node<T>*> l(1, p);
		while (!l.empty()) {
			d_node<T>* q = *l.begin();
			l.pop_front();
			for (auto &i : q->m_children)
				l.push_back(i);
			f(q, std::forward<Args>(args)...);
		}
	}

	template<typename visit, typename ...Args>
	void preorder_travel(d_node<T>* p, visit f, Args&&... args) {
		f(p, std::forward<Args>(args)...);
		for (auto& child : p->m_children)
			preorder_travel(child, f, std::forward<Args>(args)...);
	}

	static void compare(d_node<T> *p, d_node<T> *node, d_graph<T> *_this) {
		//travel(m_root, [this](d_node<T> *p, d_node<T> *node) {
		if (p->m_compared)
			return;
		OFEC::dominationship compare_result;
		if (p->m_index == -1)
			compare_result = OFEC::dominationship::Dominated;
		else {
			compare_result = _this->m_compare(node->m_obj, p->m_obj, OFEC::optimization_mode::Minimization);
			_this->m_comparisons += 2;
		}
		if (compare_result == OFEC::dominationship::Dominating) {
			//_this->travel(p, [node](d_node<T>* p) {
			_this->preorder_travel(p, [node](d_node<T>* p) {
				if (!p->m_compared) {
					p->m_compared = true;
					node->m_children.push_back(p);
				}
			});
		}
		else if (compare_result == OFEC::dominationship::Dominated)
			p->m_children.push_back(node);
		p->m_compared = true;
	}

public:
	d_graph(vector<pair<int, vector<T>>>& data) : m_root(new d_node<T>(-1, std::move(vector<T>(2, numeric_limits<T>::min())))), m_comparisons(0) , m_nodesnum(0) {
		for (pair<int, vector<T>> &member : data)
			add_node(member);
	}
	~d_graph() {
		m_nodes.clear();
		delete m_root;
	}
	void add_node(pair<int, vector<T>> &member) {
		d_node<T>* new_node = new d_node<T>(member.first, member.second);
		m_nodes.push_back(std::move(new_node));
		preorder_travel(m_root, compare, new_node, this);
		m_root->m_compared = false;
		for (d_node<T>* node : m_nodes)
			node->m_compared = false;
	}

	void update_ranking() {
		int rank(0);
		vector<int> size(1, 1);
		size.push_back(0);
		list<d_node<T>*> l(1, m_root);
		while (!l.empty()) {
			d_node<T>* q = *l.begin();
			l.pop_front();
			q->m_ranking = rank;
			size[rank]--;
			for (auto &i : q->m_children) {
				l.push_back(i);
				size[rank+1]++;
			}
			if (size[rank] == 0) {
				rank++;
				size.push_back(0);
			}
		}
		m_ranking.clear();
		for (auto& node : m_nodes)
			m_ranking.emplace(node->m_index, node->m_ranking);
	}

	const map<int, int>& ranking() { return m_ranking; }

	const int number() { return m_comparisons; }
};

#endif // 


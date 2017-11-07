#pragma once

#include "../../core/algorithm/encoding.h"
#include "../../core/definition.h"
#include <map>

using namespace OFEC;

namespace NDS {

	struct dt_node
	{
		pair<int, vector<double>> obj;
		vector<dt_node*> children;
		dt_node* parent = nullptr;
		int rank = -1;
		int ith; // the ith child of parents node
		dt_node(int lable, int n, double value) :obj(make_pair(lable, vector<double>(n, value))), children(1, nullptr) {}
		dt_node(pair<int, vector<double>>&& o) :obj(std::move(o)), children(1, nullptr) {}
		dt_node(const pair<int, vector<double>>& o) :obj(o), children(1, nullptr) {}
	};

	class d_quicktree {
	private:
		template<typename visit>
		void travel_(dt_node * p, visit f) {
			list<dt_node*> l(1, p);
			while (!l.empty()) {
				dt_node * q = *l.begin();
				l.pop_front();

				for (auto &i : q->children)
					if (i != nullptr)
						l.push_back(i);

				f(q);
			}
		}

		//this version is used when deleting node whose parent is m_root
		void insert2(dt_node* p, dt_node* root, dt_node* x) {
			auto result = objective_compare(x->obj.second, p->obj.second, optimization_mode::Minimization);
			m_num += result.second;
			if (result.first == dominationship::Non_dominated || result.first == dominationship::Equal) {
				if (p == root->children.back()) {
					x->ith = root->children.size();
					x->parent = root;
					x->rank = 0;
					root->children.push_back(x);
					return;
				}
				else
					insert2(root->children[p->ith + 1], root, x);
			}
			else if (result.first == dominationship::Dominated) {
				x->ith = p->children.size();
				x->parent = p;
				p->children.push_back(x);
				return;
			}
		}

		//this version is used when adding node whose parent is m_root
		void insert(dt_node* p, dt_node* q, dt_node* x) {
			auto result = objective_compare(x->obj.second, p->obj.second, optimization_mode::Minimization);
			m_num+=result.second;
			if (result.first == dominationship::Non_dominated || result.first == dominationship::Equal) {
				if (p == q->children.back()) 
					return;	
				else
					insert(q->children[p->ith + 1], q, x);
			}
			else if (result.first == dominationship::Dominating) {
				int p_ith = p->ith;
				for (size_t c = p->ith + 1; c < q->children.size(); ++c) {
					q->children[c]->ith--;
					q->children[c - 1] = q->children[c];
				}
				q->children.pop_back();
				p->ith = x->children.size();
				p->parent = x;
				x->children.push_back(p);
				travel_(p, [](dt_node *n) {++(n->rank); });
				if (p_ith == q->children.size()) {
					return;
				}
				else
					insert(q->children[p_ith], q, x);
			}
		}
	public:
		template<typename visit>
		void travel(visit f) {
			travel_(m_root, f);
		}
		~d_quicktree() {
			for (auto& node : m_nodes)
				node.reset();
		}
		d_quicktree(int n) :m_root(new dt_node(-1, n, numeric_limits<double>::min())) {	}

		void insert(dt_node* &p, dt_node* q, pair<int, vector<double>> &obj) {

			if (p == nullptr) {
				dt_node* x = new dt_node(obj);
				m_nodes.push_back(unique_ptr<dt_node>(x));
				x->ith = 0;
				x->rank = q->rank + 1;
				x->parent = q;
				q->children[0] = x;
			}
			else {
				auto result = make_pair(dominationship::Dominated, 0);
				if (p != m_root) {
					result = objective_compare(obj.second, p->obj.second, optimization_mode::Minimization);
					m_num+=result.second;
				}
				if (result.first == dominationship::Dominating) {
					dt_node *x = new dt_node(obj);
					m_nodes.push_back(unique_ptr<dt_node>(x));
					x->ith = p->ith;

					p->parent = x;
					x->children[0] = p;
					x->rank = p->rank;
					q->children[x->ith] = x;
					x->parent = q;
					x->children[0]->ith = 0;
					travel_(x->children[0], [](dt_node *n) {++(n->rank); });
					if (q == m_root)
						if (x != q->children.back()) 
							insert(q->children[x->ith + 1], q, x);
					return;
				}
				else if (result.first == dominationship::Non_dominated || result.first == dominationship::Equal) {
					if (p == q->children.back()) { // a new sibling of p

						dt_node *x = new dt_node(obj);
						m_nodes.push_back(unique_ptr<dt_node>(x));
						x->ith = p->ith + 1;
						x->rank = p->rank;
						q->children.push_back(x);
						x->parent = q;
						return;
					}
					else {
						insert(q->children[p->ith + 1], q, obj);                                                                                
					}
				}
				else {
					insert(p->children[0], p, obj);
				}

			}
		}

		void _delete(const int index) {
			unique_ptr<dt_node>& todelete = m_nodes[index];
			for (size_t i = todelete->ith + 1; i < todelete->parent->children.size(); ++i)
				todelete->parent->children[i]->ith--;
			todelete->parent->children.erase(todelete->parent->children.begin() + todelete->ith);
			for (auto child : todelete->children) {
				if (child == nullptr)
					break;
				if (todelete->parent == m_root) {
					insert2(m_root->children[0], m_root, child);
				}
				else {
					child->ith = todelete->parent->children.size();
					child->parent = todelete->parent;
					child->rank = todelete->rank;
					child->parent->children.push_back(child);
				}
			}
			m_nodes.erase(m_nodes.begin() + index);
			for (size_t i = index; i < m_nodes.size(); ++i)
				m_nodes[i]->obj.first--;
		}

		void build(vector< pair<int, vector<double>>>& data) {
			for (auto &i : data) {
				insert(m_root, nullptr, i);
				//vector<pair<node*, int>> all;
				//find_all()
			}
		}

		void add(vector<double>& obj) {
			insert(m_root, nullptr, make_pair(int(m_nodes.size()), obj));
		}

		const map<int, int>& ranking() {

			travel_(m_root, [this](dt_node *n) {
				if (n->rank != -1)
					m_label.emplace(n->obj.first, n->rank);
			});

			return m_label;
		}
		int number() {
			return m_num;
		}
	private:
		vector<unique_ptr<dt_node>> m_nodes;
		dt_node *m_root;
		map<int, int> m_label;
		int m_num = 0;
	};
}
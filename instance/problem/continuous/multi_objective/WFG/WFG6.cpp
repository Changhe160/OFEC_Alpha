#include "WFG6.h"

namespace OFEC {
	WFG6::WFG6(param_map & v) : WFG6(v.at("proName"), v.at("numDim"), v.at("numObj")) {
		m_k = v.at("kFactor")*(v.at("numObj") - 1);
	}
	WFG6::WFG6(const std::string & name, size_t size_var, size_t size_obj) : problem(name, size_var, size_obj), \
		WFG(name, size_var, size_obj) {

	}

	void WFG6::initialize() {
		WFG::initialize();
	}

	std::vector<real> WFG6::t1(const std::vector<real> &y) {	//as t1 from WFG1
		int n = y.size();
		std::vector<real> t(y.begin(), y.begin() + m_k);
		for (int i = m_k; i < n; i++) {
			t.push_back(s_linear(y[i], 0.35));
		}
		return std::move(t);
	}

	std::vector<real> WFG6::t2(const std::vector<real> &y) {
		int n = y.size();
		std::vector<real> t;
		for (int i = 1; i <= m_objective_size - 1; i++) {
			const int head = (i - 1)*m_k / (m_objective_size - 1);
			const int tail = i * m_k / (m_objective_size - 1);
			const std::vector<real>& y_sub = subvector(y, head, tail);
			t.push_back(r_nonsep(y_sub, m_k / (m_objective_size - 1)));
		}
		const std::vector<real>& y_sub = subvector(y, m_k, n);
		t.push_back(r_nonsep(y_sub, n - m_k));
		return std::move(t);
	}

	std::vector<real> WFG6::shape(const std::vector<real> &t_p) {
		int M = t_p.size();
		std::vector<real> x = calculate_x(t_p);
		for (int m = 1; m <= M; m++) {
			m_h[m - 1] = concave(x, m);
		}
		return std::move(x);
	}


}
#include "WFG2.h"

namespace OFEC {
	WFG2::WFG2(param_map & v) : WFG2(v.at("proName"), (v.at("kFactor")*(v.at("numObj") - 1) + 2 * v.at("lFactor")), v.at("numObj")) {	//D = k_Factor * (M - 1) + 2 * l_factor
		m_k = v.at("kFactor")*(v.at("numObj") - 1);
	}
	WFG2::WFG2(const std::string & name, size_t size_var, size_t size_obj) : problem(name, size_var, size_obj), \
		WFG(name, size_var, size_obj) {

	}

	void WFG2::initialize() {
		WFG::initialize();
	}

	std::vector<real> WFG2::t1(const std::vector<real> &y) {	//as t1 from WFG1
		int n = y.size();
		std::vector<real> t(y.begin(), y.begin() + m_k);
		for (int i = m_k; i < n; i++) {
			t.push_back(s_linear(y[i], 0.35));
		}
		return std::move(t);
	}

	std::vector<real> WFG2::t2(const std::vector<real> &y) {
		int n = y.size();
		int l = n - m_k;
		std::vector<real> t(y.begin(), y.begin() + m_k);
		for (int i = m_k + 1; i <= m_k + l / 2; i++) {
			const int head = m_k + 2 * (i - m_k) - 2;
			const int tail = m_k + 2 * (i - m_k);
			t.push_back(r_nonsep(subvector(y, head, tail), 2));
		}
		return std::move(t);
	}

	std::vector<real> WFG2::t3(const std::vector<real> &y) {
		int n = y.size();
		std::vector<real> w(n, 1.0);
		std::vector<real> t;
		for (int i = 1; i <= m_objective_size - 1; i++) {
			const int head = (i - 1)*m_k / (m_objective_size - 1);
			const int tail = i * m_k / (m_objective_size - 1);
			const std::vector<real>& y_sub = subvector(y, head, tail);
			const std::vector<real>& w_sub = subvector(w, head, tail);
			t.push_back(r_sum(y_sub, w_sub));
		}
		const std::vector<real>& y_sub = subvector(y, m_k, n);
		const std::vector<real>& w_sub = subvector(w, m_k, n);
		t.push_back(r_sum(y_sub, w_sub));
		return std::move(t);
	}	

	std::vector<real> WFG2::shape(const std::vector<real> &t_p) {
		int M = t_p.size();		
		std::vector<real> x = calculate_x(t_p);
		for (int m = 1; m <= M - 1; m++){
			m_h[m - 1] = convex(x, m);
		}
		m_h[M - 1] = disc(x, 5, 1.0, 1.0);		
		return std::move(x);
	}
	
}
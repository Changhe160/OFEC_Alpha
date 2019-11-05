#include "WFG3.h"

namespace OFEC {
	WFG3::WFG3(param_map & v) : WFG3(v.at("proName"), (v.at("kFactor")*(v.at("numObj") - 1) + 2 * v.at("lFactor")), v.at("numObj")) {	//D = k_Factor * (M - 1) + 2 * l_factor
		m_k = v.at("kFactor")*(v.at("numObj") - 1);
	}
	WFG3::WFG3(const std::string & name, size_t size_var, size_t size_obj) : problem(name, size_var, size_obj), \
		WFG(name, size_var, size_obj) {

	}

	void WFG3::initialize() {
		WFG::initialize();
		for (int i = 1; i < m_objective_size - 1; ++i) {
			m_A[i] = 0;
		}
	}

	std::vector<real> WFG3::t1(const std::vector<real> &y) {	//as t1 from WFG2
		int n = y.size();
		std::vector<real> t(y.begin(), y.begin() + m_k);
		for (int i = m_k; i < n; i++) {
			t.push_back(s_linear(y[i], 0.35));
		}
		return std::move(t);
	}

	std::vector<real> WFG3::t2(const std::vector<real> &y) {	//as t2 from WFG2
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

	std::vector<real> WFG3::t3(const std::vector<real> &y) {	//as t3 from WFG2
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

	std::vector<real> WFG3::shape(const std::vector<real> &t_p) {
		int M = t_p.size();
		std::vector<real> x = calculate_x(t_p);
		for (int m = 1; m <= M; m++) {
			m_h[m - 1] = linear(x, m);
		}		
		return std::move(x);
	}


}
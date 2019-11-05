#include "WFG8.h"

namespace OFEC {
	WFG8::WFG8(param_map & v) : WFG8(v.at("proName"), v.at("numDim"), v.at("numObj")) {
		m_k = v.at("kFactor")*(v.at("numObj") - 1);
	}
	WFG8::WFG8(const std::string & name, size_t size_var, size_t size_obj) : problem(name, size_var, size_obj), \
		WFG(name, size_var, size_obj) {

	}

	void WFG8::initialize() {
		WFG::initialize();
	}

	std::vector<real> WFG8::t1(const std::vector<real> &y) {
		int n = y.size();
		const std::vector<real> w(n, 1.0);
		std::vector<real> t(y.begin(), y.begin() + m_k);
		for (int i = m_k; i < n; i++)	{
			const std::vector<real>& y_sub = subvector(y, 0, i);
			const std::vector<real>& w_sub = subvector(w, 0, i);
			const double u = r_sum(y_sub, w_sub);
			t.push_back(b_param(y[i], u, 0.98 / 49.98, 0.02, 50));
		}
		return std::move(t);
	}

	std::vector<real> WFG8::t2(const std::vector<real> &y) {	//as t1 from WFG1
		int n = y.size();
		std::vector<real> t(y.begin(), y.begin() + m_k);
		for (int i = m_k; i < n; i++) {
			t.push_back(s_linear(y[i], 0.35));
		}
		return std::move(t);
	}

	std::vector<real> WFG8::t3(const std::vector<real> &y) {	//as t2 from WFG4
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
	
	std::vector<real> WFG8::shape(const std::vector<real> &t_p) {
		int M = t_p.size();
		std::vector<real> x = calculate_x(t_p);
		for (int m = 1; m <= M; m++) {
			m_h[m - 1] = concave(x, m);
		}
		return std::move(x);
	}


}
#include "WFG9.h"

namespace OFEC {
	WFG9::WFG9(param_map & v) : WFG9(v.at("proName"), v.at("numDim"), v.at("numObj")) {
		m_k = v.at("kFactor")*(v.at("numObj") - 1);
	}
	WFG9::WFG9(const std::string & name, size_t size_var, size_t size_obj) : problem(name, size_var, size_obj), \
		WFG(name, size_var, size_obj) {

	}

	void WFG9::initialize() {
		WFG::initialize();
	}

	std::vector<real> WFG9::t1(const std::vector<real> &y) {
		int n = y.size();		
		const std::vector<real> w(n, 1.0);
		std::vector<real> t;
		for (int i = 0; i < n - 1; i++)	{
			const std::vector<real>& y_sub = subvector(y, i + 1, n);
			const std::vector<real>& w_sub = subvector(w, i + 1, n);
			const real u = r_sum(y_sub, w_sub);
			t.push_back(b_param(y[i], u, 0.98 / 49.98, 0.02, 50));
		}
		t.push_back(y.back());
		return std::move(t);
	}

	std::vector<real> WFG9::t2(const std::vector<real> &y) {
		int n = y.size();
		std::vector<real> t(y.begin(), y.begin() + m_k);
		for (int i = m_k; i < n; i++)	{
			t.push_back(s_multi(y[i], 30, 95, 0.35));
		}
		return std::move(t);
	}

	std::vector<real> WFG9::t3(const std::vector<real> &y) {	//as t2 from WFG6
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
	
	std::vector<real> WFG9::shape(const std::vector<real> &t_p) {
		int M = t_p.size();
		std::vector<real> x = calculate_x(t_p);
		for (int m = 1; m <= M; m++) {
			m_h[m - 1] = concave(x, m);
		}
		return std::move(x);
	}

}
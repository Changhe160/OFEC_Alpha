#include "WFG1.h"

namespace OFEC {
	WFG1::WFG1(param_map & v) : WFG1(v.at("proName"), v.at("numDim"), v.at("numObj")) {
		m_k = v.at("kFactor")*(v.at("numObj") - 1);
	}
	WFG1::WFG1(const std::string & name, size_t size_var, size_t size_obj) : problem(name, size_var, size_obj), \
		WFG(name, size_var, size_obj) {

	}

	void WFG1::initialize() {
		WFG::initialize();
	}

	std::vector<real> WFG1::t1(const std::vector<real> &y) {
		int n = y.size();
		std::vector<real> t(y.begin(), y.begin() + m_k);		
		for (int i = m_k; i < n; i++) {
			t.push_back(s_linear(y[i], 0.35));
		}
		return std::move(t);
	}

	std::vector<real> WFG1::t2(const std::vector<real> &y) {
		int n = y.size();
		std::vector<real> t(y.begin(), y.begin() + m_k);	
		for (int i = m_k; i < n; i++)	{
			t.push_back(b_flat(y[i], 0.8, 0.75, 0.85));
		}
		return std::move(t);
	}

	std::vector<real> WFG1::t3(const std::vector<real> &y) {
		int n = y.size();
		std::vector<real> t;
		for (int i = 0; i < n; i++) {
			t.push_back(b_poly(y[i], 0.02));
		}
		return std::move(t);
	}

	std::vector<real> WFG1::t4(const std::vector<real> &y) {
			int n = y.size();
			std::vector<real> w;
			for (int i = 1; i <= n; i++) {
				w.push_back(2.0*i);
			}
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

	std::vector<real> WFG1::shape(const std::vector<real> &t_p) {
		int M = t_p.size();		
		std::vector<real> x = calculate_x(t_p);
		for (int m = 1; m <= M - 1; m++){
			m_h[m - 1] = convex(x, m);
		}
		m_h[M - 1] = mixed(x, 5, 1.0);
		return std::move(x);
	}


}
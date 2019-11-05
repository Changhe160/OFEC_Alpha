#include "WFG5.h"

namespace OFEC {
	WFG5::WFG5(param_map & v) : WFG5(v.at("proName"), v.at("numDim"), v.at("numObj")){
		m_k = v.at("kFactor")*(v.at("numObj") - 1);
	}
	WFG5::WFG5(const std::string & name, size_t size_var, size_t size_obj) : problem(name, size_var, size_obj), \
		WFG(name, size_var, size_obj) {

	}

	void WFG5::initialize() {
		WFG::initialize();
	}

	std::vector<real> WFG5::t1(const std::vector<real> &y) {
		int n = y.size();
		std::vector<real> t;
		for (int i = 0; i < n; i++)	{
			t.push_back(s_decept(y[i], 0.35, 0.001, 0.05));
		}
		return std::move(t);
	}

	std::vector<real> WFG5::t2(const std::vector<real> &y) {	//as t2 from WFG4
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

	

	std::vector<real> WFG5::shape(const std::vector<real> &t_p) {
		int M = t_p.size();
		std::vector<real> x = calculate_x(t_p);
		for (int m = 1; m <= M; m++) {
			m_h[m - 1] = concave(x, m);
		}	
		return std::move(x);
	}


}
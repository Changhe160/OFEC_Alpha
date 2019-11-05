#include "WFG.h"
#include <fstream>

namespace OFEC {
	WFG::WFG(const std::string & name, size_t size_var, size_t size_obj) : problem(name, size_var, size_obj), continuous(name, size_var, size_obj) {

	}
	void WFG::initialize() {
	//	m_k = m_objective_size - 1;	//temporarily set to M-1
		m_D = 1;
		m_A.resize(m_objective_size - 1);
		for (int i = 0; i < (m_objective_size - 1); ++i) {
			m_A[i] = 1;
		}
		m_S.resize(m_objective_size);
		for (int i = 0; i < m_objective_size; ++i) {
			m_S[i] = 2 * (i + 1);
		}
		m_h.resize(m_objective_size);

		std::vector<std::pair<real, real>> r;		
		for (int i = 0; i < m_variable_size; ++i) {
			r.push_back(std::make_pair(0., 2.*(i + 1)));
		}
		set_init_range(r);
		set_range(r);
		load_PF();
	}

	void WFG::load_PF() {
		std::ifstream infile;
		std::stringstream os;
		//	os << global::ms_arg.at("workingDir") << "./instance/problem/continuous/multi_objective/WFG/data/" << m_name << "." << m_objective_size << "D.pf";
		os << "./instance/problem/continuous/multi_objective/WFG/data/" << m_name << "." << m_objective_size << "D.pf";
		infile.open(os.str());
		if (!infile) {
			//std::cout << "open file is failed" << std::endl;
			throw myexcept("open PF file of WFG problem is fail");
			return;
		}
		std::string str;
		size_t line = 0;
		while (getline(infile, str))
			++line;
		m_optima.resize_objective_set(line);
		infile.close();
		infile.clear();
		infile.open(os.str());
		for (size_t i = 0; i < line; i++) {
			std::vector<real> temp_obj(m_objective_size);
			for (size_t j = 0; j < m_objective_size; j++)
				infile >> temp_obj[j];
			m_optima.set_objective(temp_obj, i);
		}
		infile.close();
	}

	void WFG::evaluate_objective(real *x, std::vector<real> &obj) {
		std::vector<real> y = normalise(x);	//normalization
		y = t1(y);	
		y = t2(y);	
		y = t3(y);	
		y = t4(y);
		y = shape(y); //calculate x, and set m_h
		for (int i = 0; i < m_h.size(); i++) {	//calculate objective value
			obj[i] = m_D * y[m_objective_size - 1] + m_S[i] * m_h[i];
		}
	}

	std::vector<real> WFG::calculate_x(const std::vector<real> &t_p) {
		std::vector<real> x(t_p.size());
		int i;
		for (i = 0; i < (t_p.size() - 1); i++) {
			real tmp1 = std::max<real>(t_p.back(), m_A[i]);
			x[i] = tmp1 * (t_p[i] - 0.5) + 0.5;
		}
		x[i] = t_p.back();
		return std::move(x);
	}

	std::vector<real> WFG::normalise(real *z) {
		std::vector<real> y(m_variable_size);
		for (int i = 0; i < m_variable_size; i++) {
			real z_max = 2 * (i + 1);
			y[i] = z[i] / z_max;
		}
		return std::move(y);
	}
	
	std::vector<real> WFG::subvector(const std::vector<real> &v, int head, int tail) {
		std::vector<real> result;
		for (int i = head; i < tail; i++){
			result.push_back(v[i]);
		}
		return result;
	}

	/*Shape Function*/
	real WFG::linear(const std::vector<real> &x, int m) {		
		int M = static_cast<int>(x.size());
		real result = 1.0;
		for (int i = 1; i <= M - m; i++){
			result *= x[i - 1];
		}
		if (m != 1)	{
			result *= 1 - x[M - m];
		}
		return correct_to_01(result);
	}
	real WFG::convex(const std::vector<real> &x, int m) {		
		int M = x.size();
		real result = 1.0;
		for (int i = 1; i <= M - m; i++){
			result *= 1.0 - cos(x[i - 1] * OFEC_PI / 2.0);
		}
		if (m != 1)	{
			result *= 1.0 - sin(x[M - m] * OFEC_PI / 2.0);
		}
		return correct_to_01(result);
	}
	real WFG::concave(const std::vector<real> &x, int m) {		
		int M = static_cast<int>(x.size());
		real result = 1.0;
		for (int i = 1; i <= M - m; i++){
			result *= sin(x[i - 1] * OFEC_PI / 2.0);
		}
		if (m != 1)	{
			result *= cos(x[M - m] * OFEC_PI / 2.0);
		}
		return correct_to_01(result);
	}
	real WFG::mixed(const std::vector<real> &x, int A, real alpha) {
		real tmp = 2.0*A*OFEC_PI;
		return correct_to_01(pow(1.0 - x[0] - cos(tmp*x[0] + OFEC_PI / 2.0) / tmp, alpha));	
	}
	real WFG::disc(const std::vector<real> &x, int A, real alpha, real beta) {
		real tmp1 = A * pow(x[0], beta)*OFEC_PI;
		return correct_to_01(1.0 - pow(x[0], alpha)*pow(cos(tmp1), 2.0));
	}
	real WFG::correct_to_01(const real &a, real epsilon) {
		real min = 0.0;
		real max = 1.0;
		real min_epsilon = min - epsilon;
		real max_epsilon = max + epsilon;
		if (a <= min && a >= min_epsilon) {
			return min;
		}
		else if (a >= max && a <= max_epsilon){
			return max;
		}
		else{
			return a;
		}
	}
	/*TransFunctions*/
	real WFG::b_poly(real y, real alpha){		
		return correct_to_01(pow(y, alpha));
	}
	real WFG::b_flat (real y, real A, real B, real C){	
		real tmp1 = std::min<real>(0.0, floor(y - B)) *A*(B - y) / B;
		real tmp2 = std::min<real>(0.0, floor(C - y)) * (1.0 - A)*(y - C) / (1.0 - C);
		return correct_to_01(A + tmp1 - tmp2);
	}
	real WFG::b_param(real y, real u, real A, real B, real C) {
		real v = A - (1.0 - 2.0*u)*fabs(floor(0.5 - u) + A);
		return correct_to_01(pow(y, B + (C - B)*v));
	}

	real WFG::s_linear(real y, real A) {
		return correct_to_01(fabs(y - A) / fabs(floor(A - y) + A));
	}
	real WFG::s_decept(real y, real A, real B, real C)	{	
		real tmp1 = floor(y - A + B)*(1.0 - C + (A - B) / B) / (A - B);
		real tmp2 = floor(A + B - y)*(1.0 - C + (1.0 - A - B) / B) / (1.0 - A - B);
		return correct_to_01(1.0 + (fabs(y - A) - B)*(tmp1 + tmp2 + 1.0 / B));
	}
	real WFG::s_multi(real y, int A, real B, real C){
		real tmp1 = fabs(y - C) / (2.0*(floor(C - y) + C));
		real tmp2 = (4.0*A + 2.0)*OFEC_PI*(0.5 - tmp1);
		return correct_to_01((1.0 + cos(tmp2) + 4.0*B*pow(tmp1, 2.0)) / (B + 2.0));
	}
	real WFG::r_sum(const std::vector<real> &y, const std::vector<real> &w){		
		real numerator = 0.0;
		real denominator = 0.0;
		for (int i = 0; i < static_cast<int>(y.size()); i++){
			numerator += w[i] * y[i];
			denominator += w[i];
		}
		return correct_to_01(numerator / denominator);
	}
	real WFG::r_nonsep(const std::vector<real> &y, int A){
		int y_len = static_cast<int>(y.size());
		real numerator = 0.0;
		for (int j = 0; j < y_len; j++)	{
			numerator += y[j];
			for (int k = 0; k <= A - 2; k++){
				numerator += fabs(y[j] - y[(j + k + 1) % y_len]);
			}
		}
		real tmp = ceil(A / 2.0);
		real denominator = y_len * tmp*(1.0 + 2.0*A - 2.0*tmp) / A;
		return correct_to_01(numerator / denominator);
	}

}
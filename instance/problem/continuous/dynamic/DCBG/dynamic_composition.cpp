#include "dynamic_composition.h"
#include "../../../../../core/global.h"
#include <algorithm>
namespace OFEC {
	thread_local composition_DBG::ComDBGFuncID composition_DBG::ms_fun_idx;

	composition_DBG::composition_DBG(param_map &v) :composition_DBG(v.at("proName"), v.at("numDim"), v.at("numPeak"), v.at("changeFre"), static_cast<ChangeType>((int)(v.at("changeType"))), \
		static_cast<ComDBGFuncID>((int)(v.at("comDBGFunID"))), v.at("changeRatio"), v.at("flagNumDimChange"), v.at("flagNumPeakChange"), v.at("peakNumChangeMode"), v.at("flagNoise"), v.at("flagTimeLinkage")) {
	} 

	composition_DBG::composition_DBG(const std::string &name, int rDimNumber, int rNumPeaks, int frequency, ChangeType rT, ComDBGFuncID rF, \
		real rChangingRatio, bool rFlagDimChange, bool rFlagNumPeakChange, \
		int peakNumChangeMode, bool flagNoise, bool flagTimelinkage) :\
		problem(name, rDimNumber, 1), real_DBG(name, rDimNumber, rNumPeaks, 1, rT, rFlagDimChange, rFlagNumPeakChange, peakNumChangeMode, flagNoise, flagTimelinkage), 
		m_com_domain(ms_num_basic_funs), m_converge_severity(rNumPeaks), m_stretch_severity(rNumPeaks), m_basic_function(rNumPeaks), m_fmax(rNumPeaks){

		for (int i = 0; i < ms_num_basic_funs; i++) {
			m_com_domain[i].resize(rDimNumber);
		}

		set_basic_fun_domain();
		m_height_normalize_severity = 2000.;
		set_frequency(frequency);		
		set_num_change(rChangingRatio);
		ms_fun_idx = rF;
		BasicProblem *basic_fun=new BasicProblem[rNumPeaks];
		switch (ms_fun_idx) {
		case COMDBG_SPHERE:
			for (int i = 0; i < m_peak.size(); i++) basic_fun[i] = Sphere;
			break;
		case COMDBG_RASTRIGIN:
			for (int i = 0; i < m_peak.size(); i++) basic_fun[i] = Rastrigin;
			break;
		case COMDBG_GRIEWANK:
			for (int i = 0; i < m_peak.size(); i++) basic_fun[i] = Griewank;
			break;
		case COMDBG_ACKLEY:
			for (int i = 0; i < m_peak.size(); i++) basic_fun[i] = Ackley;
			break;
		case COMDBG_HYBRID:
			basic_fun[0] = Sphere;		basic_fun[5] = Sphere;
			basic_fun[1] = Rastrigin;		basic_fun[6] = Rastrigin;
			basic_fun[2] = Weierstrass;	basic_fun[7] = Weierstrass;
			basic_fun[3] = Griewank;		basic_fun[8] = Griewank;
			basic_fun[4] = Ackley;		basic_fun[9] = Ackley;
			for (int i = 10; i < m_peak.size(); i++) basic_fun[i] = Sphere;
			break;
		}
		
		std::copy(basic_fun, basic_fun + m_peak.size(), m_basic_function.begin());
		delete[] basic_fun;

		
		for (int i = 0; i < m_peak.size(); i++)m_converge_severity[i] = 1.;
	
		set_stretch_severity();
		set_rotation_matrix();

		matrix m( 1, m_variable_size);
		real *gene = new real[m_variable_size];
		for (int i = 0; i < m_peak.size(); i++) {
			for (int j = 0; j < m_variable_size; j++) { // calculate the estimate max value of funciton i
				gene[j] = m_domain[j].limit.second;
				gene[j] /= m_stretch_severity[i];
			}
			m.set_row(gene, m_variable_size);
			m *= m_rotM[i];
			std::copy(m[0].begin(), m[0].end(), gene);
			repair(m_basic_function[i], gene);
			m_fmax[i] = selectFun(m_basic_function[i], gene);
			if (m_fmax[i] == 0)   throw myexcept("the estimation max value must be greater not equal to 0@composition_DBG::initialize");

		}

		calculate_global_optima();
		update_time_linkage();
		delete[] gene;
		gene = 0;
	}

	void composition_DBG::set_basic_fun_domain() {
		//Sphere=0,Rastrigin,Weierstrass,Griewank,Ackley

		for (int j = 0; j < m_variable_size; j++) {
			m_com_domain[0][j].limit.second = 100.;
			m_com_domain[0][j].limit.first = -100.;
			m_com_domain[1][j].limit.second = 5.;
			m_com_domain[1][j].limit.first = -5.;
			m_com_domain[2][j].limit.second = 0.5;
			m_com_domain[2][j].limit.first = -0.5;
			m_com_domain[3][j].limit.second = 100.;
			m_com_domain[3][j].limit.first = -100.;
			m_com_domain[4][j].limit.second = 32.;
			m_com_domain[4][j].limit.first = -32.;
		}
	}
	void composition_DBG::get_basic_fun_domain(const BasicProblem &f, real &l, real &u, const int rDimIdx)const {
		switch (f) {
		case Sphere:
			l = m_com_domain[0][rDimIdx].limit.first;
			u = m_com_domain[0][rDimIdx].limit.second;
			break;
		case Rastrigin:
			l = m_com_domain[1][rDimIdx].limit.first;
			u = m_com_domain[1][rDimIdx].limit.second;
			break;
		case Weierstrass:
			l = m_com_domain[2][rDimIdx].limit.first;
			u = m_com_domain[2][rDimIdx].limit.second;
			break;
		case Griewank:
			l = m_com_domain[3][rDimIdx].limit.first;
			u = m_com_domain[3][rDimIdx].limit.second;
			break;
		case Ackley:
			l = m_com_domain[4][rDimIdx].limit.first;
			u = m_com_domain[4][rDimIdx].limit.second;
			break;
		default:
			throw myexcept("No the function in the basic component fs@composition_DBG::get_basic_fun_domain");
			break;
		}
	}
	
	void composition_DBG::set_rotation_matrix() {
		// for each basic function of dimension n(even number), R=R(l1,l2)*R(l3,l4)*....*R(ln-1,ln), 0<=li<=n
		matrix I(m_variable_size, m_variable_size);

		std::vector<int>  d(m_variable_size);
		global::ms_global->m_uniform[caller::Problem]->shuffle(d.begin(), d.end());
		for (int i = 0; i < m_peak.size(); i++) {
			for (int j = 0; j + 1 < m_variable_size; j += 2) {
				real angle = 2 * OFEC_PI*global::ms_global->m_uniform[caller::Problem]->next();				// random angle for rotation plane of d[j]-d[j+1] from d[j]th axis to d[j+1]th axis
				I.set_rotation_axes(d[j], d[j + 1], angle);
				if (j == 0)  m_rotM[i] = I;
				else	m_rotM[i] *= I;
				I.identify();
			}
		}
	}

	void composition_DBG::set_stretch_severity() {

		for (int i = 0; i < m_peak.size(); i++) {
			real l, u;
			get_basic_fun_domain(m_basic_function[i], l, u);
			m_stretch_severity[i] = m_converge_severity[i] * (m_domain[0].limit.second - m_domain[0].limit.first) / (u - l);
		}
	}

	void composition_DBG::evaluate_objective(real* x_, std::vector<real>& obj_) {
		//auto &s = dynamic_cast<solution<> &>(ss);
		real *x = new real[m_variable_size];
		for (size_t i = 0; i < m_variable_size; i++)
			x[i] = x_[i];
		//std::copy(s.variable().begin(), s.variable().end(), x);

		if (this->m_flag_noise)	add_noise(x);

		std::vector<real> width(m_peak.size(), 0), fit(m_peak.size());
		for (int i = 0; i < m_peak.size(); i++) { // calculate weight for each function		
			for (int j = 0; j < m_variable_size; j++)
				width[i] += (x[j] - m_peak[i][j])*(x[j] - m_peak[i][j]);
			if (width[i] != 0)	width[i] = exp(-std::sqrt(width[i] / (2 * m_variable_size*m_converge_severity[i] * m_converge_severity[i])));
		}

		for (int i = 0; i < m_peak.size(); i++) { // calculate objective value for each function
			for (int j = 0; j < m_variable_size; j++)	// calculate the objective value of tranformation function i
				x[j] = (x[j] - m_peak[i][j]) / m_stretch_severity[i];//((1+fabs(m_peak[i][j]/mp_searchRange[j].limit.second))*
			matrix m(1, m_variable_size);
			m.set_row(x, m_variable_size);
			m *= m_rotM[i];
			std::copy(m[0].begin(), m[0].end(), x);
			repair(m_basic_function[i], x);
			fit[i] = selectFun(m_basic_function[i], x);
			fit[i] = m_height_normalize_severity*fit[i] / fabs(m_fmax[i]);
			for (size_t i = 0; i < m_variable_size; i++)
				x[i] = x_[i];
		}
		real sumw = 0, wmax;
		wmax = *std::max_element(width.begin(), width.end());
		for (int i = 0; i < m_peak.size(); i++)
			if (width[i] != wmax)
				width[i] = width[i] * (1 - pow(wmax, 10));
		for (int i = 0; i < m_peak.size(); i++)
			sumw += width[i];
		for (int i = 0; i < m_peak.size(); i++)
			width[i] /= sumw;
		real obj = 0;
		for (int i = 0; i < m_peak.size(); i++)
			obj += width[i] * (fit[i] + m_height[i]);
		obj_[0] = obj;

		if (m_eval_monitor && m_effective_eval%m_frequency == 0) {
			ms_minmax_objective.clear();
		}

		is_tracked(x, obj);
		bool flag_stop;

		if (global::ms_global->m_algorithm != nullptr)	flag_stop = global::ms_global->m_algorithm->terminating();
		else flag_stop = false;

		if (m_effective_eval%m_frequency == 0 && !flag_stop) {
			dynamic::change();
			if (m_flag_time_linkage) update_time_linkage();
		}
		delete[] x;
		x = 0;
		//evaluation_tag rf = evaluation_tag::Normal;
		//if (global::ms_global->m_algorithm != nullptr && flag_stop) {
		//	rf = evaluation_tag::Terminate;
		//}

		//if ((m_effective_eval + 1) % (m_frequency) == 0) {
		//	rf = evaluation_tag::Change_next_eval;
		//}
		//else if (m_effective_eval % m_frequency == 0) {
		//	if (CAST_DYN->get_flag_dimension_change()) {
		//		rf = evaluation_tag::Change_dimension;
		//	}
		//	else if (CAST_DYN->get_flag_time_linkage() && CAST_DYN->get_trigger_flag_time_linkage()) {
		//		rf = evaluation_tag::Change_timelinkage;
		//	}
		//	else	rf = evaluation_tag::Change;
		//}
		//return rf;

	}

	real composition_DBG::selectFun(const BasicProblem &f, real *x) {
		real value;
		switch (f) {
		case Sphere:
			value = fSphere(x);
			break;
		case Rastrigin:
			value = fRastrigin(x);
			break;
		case Weierstrass:
			value = fWeierstrass(x);
			break;
		case Griewank:
			value = fGriewank(x);
			break;
		case Ackley:
			value = fAckley(x);
			break;
		default:
			break;
		}
		return value;
	}
	real composition_DBG::fAckley(real *x) {
		real fitness = 0;
		real s1 = 0, s2 = 0;
		for (int i = 0; i < m_variable_size; i++) {
			s1 += x[i] * x[i];
			s2 += cos(2 * OFEC_PI*x[i]);
		}
		fitness = -20 * exp(-0.2*sqrt(s1 / m_variable_size)) - exp(s2 / m_variable_size) + 20 + OFEC_E;
		return fitness;
	}
	real composition_DBG::fGriewank(real *x) {
		real s1 = 0, s2 = 1;
		for (int i = 0; i < m_variable_size; i++) {
			s1 += x[i] * x[i] / 4000.;
			s2 *= cos(x[i] / sqrt((real)(i + 1)));
		}
		return s1 - s2 + 1.;
	}
	real composition_DBG::fRastrigin(real *x) {
		real fit = 0;
		for (int i = 0; i < m_variable_size; i++)
			fit = fit + x[i] * x[i] - 10.*cos(2 * OFEC_PI*x[i]) + 10.;
		return fit;
	}
	real composition_DBG::fSphere(real *x) {
		real fit = 0;
		for (int i = 0; i < m_variable_size; i++)
			fit += x[i] * x[i];
		return fit;
	}
	real composition_DBG::fWeierstrass(real *x) {
		real a = 0.5, b = 3;
		int kmax = 20;
		real fit = 0, s = 0;
		for (int i = 0; i < m_variable_size; i++)
			for (int k = 0; k <= kmax; k++)
				fit += pow(a, k)*cos(2 * OFEC_PI*pow(b, k)*(x[i] + 0.5));
		for (int k = 0; k <= kmax; k++)
			s += pow(a, k)*cos(2 * OFEC_PI*pow(b, k)*0.5);
		s = s*m_variable_size;
		return fit - s;
	}
	void composition_DBG::repair(const BasicProblem &f, real *x) {

		real l, u;
		get_basic_fun_domain(f, l, u);
		for (int j = 0; j < m_variable_size; j++) {
			if (x[j] > u)  x[j] = u;
			else if (x[j] < l)  x[j] = l;
		}
	}

	void composition_DBG::change_random() {
		update_pre_peak();

		//change the global minimum value of each function
		change_height();
		//change the position of global optimum of each function randomly
		change_location(0);

		restore();
		calculate_global_optima();
		update_num_change();
		m_change.counter++;
	}
	void composition_DBG::change_recurrent_noisy() {
		update_pre_peak();
		real initial_angle;
		real height_range = m_max_height - m_min_height;

		real noisy;
		for (int i = 0; i < m_peak.size(); i++) {
			if (m_flag_change[i] == false) continue;
			initial_angle = (real)m_period*i / m_peak.size();

			m_height[i] = get_recurrent_noise(m_change.counter, m_min_height, m_max_height, height_range, initial_angle, m_recurrent_noisy_severity);
		}

		initial_angle = OFEC_PI*(sin(2 * OFEC_PI*(m_change.counter) / m_period) + 1) / 12.;
		noisy = m_recurrent_noisy_severity*global::ms_global->m_normal[caller::Problem]->next();
		change_location(initial_angle + noisy);

		restore();
		calculate_global_optima();
		update_num_change();
		m_change.counter++;
	}
	void composition_DBG::change_recurrent() {

		update_pre_peak();
		real initial_angle;
		real height_range = m_max_height - m_min_height;

		for (int i = 0; i < m_peak.size(); i++) {
			if (m_flag_change[i] == false) continue;
			initial_angle = (real)m_period*i / m_peak.size();
			m_height[i] = m_min_height + height_range*(sin(2 * OFEC_PI*(m_change.counter + initial_angle) / m_period) + 1) / 2.;
		}
		initial_angle = OFEC_PI*(sin(2 * OFEC_PI*m_change.counter / m_period) + 1) / 12.;
		change_location(initial_angle);

		restore();
		calculate_global_optima();
		update_num_change();
		m_change.counter++;
	}
	void composition_DBG::change_small_step() {
		update_pre_peak();

		change_height();
		change_location(0);

		restore();
		calculate_global_optima();
		update_num_change();
		m_change.counter++;
	}
	void composition_DBG::change_large_step() {
		update_pre_peak();

		change_height();
		change_location(0);

		restore();
		calculate_global_optima();
		update_num_change();
		m_change.counter++;
	}
	void composition_DBG::change_chaotic() {

		update_pre_peak();

		for (int i = 0; i < m_peak.size(); i++) {
			if (m_flag_change[i] == false) continue;
			m_height[i] = chaotic_value(m_height[i], m_min_height, m_max_height);
		}

		change_location(0);

		restore();
		calculate_global_optima();
		update_num_change();
		m_change.counter++;
	}

	void composition_DBG::change_dimension() {
		/// no need to preserve  previous information, e.g., positions, heights, width....

		composition_DBG r_dbg("compositionDBG", m_temp_dimension, m_peak.size(), m_frequency,m_change.type, ms_fun_idx, m_ratio_changing_peak, m_flag_dimension_change,
			m_flag_num_peak_change, m_mode, m_flag_noise, m_flag_time_linkage);

		r_dbg.copy(*this);
		r_dbg.calculate_global_optima();

		*this = r_dbg;

	}

	void composition_DBG::copy(const problem &rDP) {
		real_DBG::copy(rDP);
		
		auto& r_dbg = dynamic_cast<const composition_DBG&>(rDP);

		int peaks = m_peak.size() < r_dbg.m_peak.size() ? m_peak.size() : r_dbg.m_peak.size();
		std::copy(r_dbg.m_basic_function.begin(), r_dbg.m_basic_function.begin()+ peaks, m_basic_function.begin());
		std::copy(r_dbg.m_converge_severity.begin(), r_dbg.m_converge_severity.begin() + peaks, m_converge_severity.begin());
		std::copy(r_dbg.m_stretch_severity.begin(), r_dbg.m_stretch_severity.begin() + peaks, m_stretch_severity.begin());
	}



	void  composition_DBG::change_num_peak() {
		composition_DBG r_dbg("compositionDBG", m_variable_size, m_temp_num_peak, m_frequency,m_change.type, ms_fun_idx,
			m_ratio_changing_peak, m_flag_dimension_change, m_flag_num_peak_change,	m_mode, m_flag_noise, m_flag_time_linkage);
	
		r_dbg.copy(*this);
		r_dbg.calculate_global_optima();

		*this = r_dbg;
	}

	void composition_DBG::initialize() {

	m_objective_accuracy = 1.0;
	m_variable_accuracy = 0.1;
	m_optima.set_flag_variable(true);
	set_opt_mode(optimization_mode::Minimization);
	m_initialized = true;
	}

	void composition_DBG::set_fun_idx(ComDBGFuncID idx) {
		ms_fun_idx = idx;
		m_parameters["function index"] = ms_fun_idx;
	}

	void composition_DBG::update_parameters() {
		real_DBG::update_parameters();
		m_parameters["function index"] = ms_fun_idx;
	}
}

#include "rotation_peak.h"
#include "../../../../../core/global.h"
#include <algorithm>

namespace OFEC {
	rotation_DBG::rotation_DBG(param_map &v) :rotation_DBG(v.at("proName"), v.at("numDim"),v.at("numPeak"), v.at("changeFre"),static_cast<ChangeType>((int)(v.at("changeType"))), \
		v.at("changeRatio"), v.at("flagNumDimChange"), v.at("flagNumPeakChange"), v.at("peakNumChangeMode"), v.at("flagNoise"), v.at("flagTimeLinkage")){
	}

	rotation_DBG::rotation_DBG(const std::string &name, int rDimNumber, int rNumPeaks, int fre,\
		dynamic::ChangeType rT, real  rChangingRatio, bool rFlagDimChange, bool rFlagNumPeakChange, \
		int peakNumChangeMode, bool flagNoise, bool flagTimelinkage) :problem(name, rDimNumber, 1), real_DBG(name,rDimNumber, rNumPeaks,1, 
			 rT, rFlagDimChange, rFlagNumPeakChange, peakNumChangeMode, flagNoise, flagTimelinkage) {
		//ctor	
		add_tag(problem_tag::MMOP);
		set_frequency(fre);
		set_num_change(rChangingRatio);
	}

	void  rotation_DBG::set_width(real w) {

		for (int i = 0; i < m_peak.size(); i++)
			if (m_change.type == CT_Chaotic)
				m_width[i] = m_min_width + (m_max_width - m_min_width)*global::ms_global->m_uniform[caller::Problem]->next();
			else
				m_width[i] = w;
	};

	void rotation_DBG::change_random() {
		update_pre_peak();

		change_height();
		change_width();
		change_location(0);

		restore();
		calculate_global_optima();
		update_num_change();
		m_change.counter++;
	}
	void rotation_DBG::change_recurrent() {
		update_pre_peak();

		real initial_angle;

		real height_range = m_max_height - m_min_height;
		real width_range = m_max_width - m_min_width;
		for (int i = 0; i < m_peak.size(); i++) {
			if (m_flag_change[i] == false) continue;
			initial_angle = (real)m_period*i / m_peak.size();
			m_height[i] = m_min_height + height_range*(sin(2 * OFEC_PI*(m_change.counter + initial_angle) / m_period) + 1) / 2.;
			m_width[i] = m_min_width + width_range*(sin(2 * OFEC_PI*(m_change.counter + initial_angle) / m_period) + 1) / 2.;
		}
		initial_angle = OFEC_PI*(sin(2 * OFEC_PI*(m_change.counter) / m_period) + 1) / 12.;
		change_location(initial_angle);

		restore();
		calculate_global_optima();
		update_num_change();
		m_change.counter++;
	}
	void rotation_DBG::change_chaotic() {

		update_pre_peak();

		for (int i = 0; i < m_peak.size(); i++) {
			if (m_flag_change[i] == false) continue;
			m_height[i] = chaotic_value(m_height[i], m_min_height, m_max_height);
			m_width[i] = chaotic_value(m_width[i], m_min_width, m_max_width);
		}
		change_location(0);

		restore();
		calculate_global_optima();
		update_num_change();
		m_change.counter++;
	}
	void rotation_DBG::change_small_step() {

		update_pre_peak();

		change_height();
		change_width();
		change_location(0);

		restore();
		calculate_global_optima();
		update_num_change();
		m_change.counter++;
	}
	void rotation_DBG::change_large_step() {

		update_pre_peak();

		change_height();
		change_width();
		change_location(0);

		restore();
		calculate_global_optima();
		update_num_change();
		m_change.counter++;
	}
	void rotation_DBG::change_recurrent_noisy() {

		update_pre_peak();

		real initial_angle;
		real height_range = m_max_height - m_min_height;
		real width_range = m_max_width - m_min_width;
		real noisy;
		for (int i = 0; i < m_peak.size(); i++) {
			if (m_flag_change[i] == false) continue;
			initial_angle = (real)m_period*i / m_peak.size();
			m_height[i] = get_recurrent_noise(m_change.counter, m_min_height, m_max_height, height_range, initial_angle, m_recurrent_noisy_severity);
			m_width[i] = get_recurrent_noise(m_change.counter, m_min_width, m_max_width, width_range, initial_angle, m_recurrent_noisy_severity);
		}
		initial_angle = OFEC_PI*(sin(2 * OFEC_PI*(m_change.counter) / m_period) + 1) / 12.;

		noisy = m_recurrent_noisy_severity*global::ms_global->m_normal[caller::Problem]->next();
		change_location(initial_angle + noisy);

		restore();
		calculate_global_optima();
		update_num_change();
		m_change.counter++;
	}
	void rotation_DBG::evaluate_objective(real* x_, std::vector<real>& obj_) {
		real *x = new real[m_variable_size];
		for (size_t i = 0; i < m_variable_size; ++i)
			x[i] = x_[i];

		if (this->m_flag_noise)	add_noise(x);
		std::vector<real> fit(m_peak.size(), 0);
		for (int i = 0; i < m_peak.size(); i++) {
			for (int j = 0; j < m_variable_size; j++)
				fit[i] += (x[j] - m_peak[i][j])*(x[j] - m_peak[i][j]);
			if (fit[i] != 0) fit[i] = sqrt(fit[i] / m_variable_size);
			fit[i] = m_height[i] / (1 + m_width[i] * fit[i]);
		}
		obj_[0] = *std::max_element(fit.begin(), fit.end());


		if (m_eval_monitor && m_effective_eval%m_frequency == 0) {
			ms_minmax_objective.clear();
		}

		is_tracked(x, obj_[0]);
		bool flag_stop;

		if (global::ms_global->m_algorithm != nullptr)	flag_stop = global::ms_global->m_algorithm->terminating();
		else flag_stop = false;

		if (m_effective_eval%m_frequency == 0 && !flag_stop) {
			dynamic::change();
		}
		delete[] x;
		x = 0;
		//evaluation_tag rf = evaluation_tag::Normal;
		//if (effective) {
		//	if (global::ms_global->m_algorithm != nullptr&&flag_stop) {
		//		rf = evaluation_tag::Terminate;
		//	}

		//	if ((m_effective_eval + 1) % (m_frequency) == 0) {
		//		rf = evaluation_tag::Change_next_eval;
		//	}
		//	else if (m_effective_eval % m_frequency == 0) {
		//		if (CAST_DYN->get_flag_dimension_change()) {
		//			rf = evaluation_tag::Change_dimension;
		//		}
		//		else if (CAST_DYN->get_flag_time_linkage() && CAST_DYN->get_trigger_flag_time_linkage()) {
		//			rf = evaluation_tag::Change_timelinkage;
		//		}
		//		else	rf = evaluation_tag::Change;
		//	}
		//}
		//return rf;
	
	}


	void rotation_DBG::change_width() {
		real step;
		for (int i = 0; i < m_peak.size(); i++) {
			if (m_flag_change[i] == false) continue;
			step = m_width_severity[i] * basic_change(m_change.type, m_min_width, m_max_width);
			m_width[i] = m_width[i] + step;

			if (m_width[i] > m_max_width || m_width[i] < m_min_width) m_width[i] = m_width[i] - step;

		}
	}

	void rotation_DBG::change_dimension() {
		/// no need to preserve  previous information, e.g., positions, heights, width....

		rotation_DBG r_dbg("rotationDBG", m_temp_dimension, m_peak.size(), m_frequency,m_change.type,
			m_ratio_changing_peak, m_flag_dimension_change, m_flag_num_peak_change,	m_mode, m_flag_noise, m_flag_time_linkage);
		
		r_dbg.copy(*this);
		r_dbg.calculate_global_optima();

		*this = r_dbg;

	}

	void rotation_DBG::initialize() {
		
		m_optima.set_flag_variable(true);
		set_opt_mode(optimization_mode::Maximization);
		calculate_global_optima();	
		m_variable_accuracy = 0.1;
		m_objective_accuracy = 0.1;
		m_initialized = true;
	}


	void  rotation_DBG::change_num_peak() {

		rotation_DBG r_dbg ("rotationDBG", m_variable_size, m_temp_num_peak, m_frequency,m_change.type, \
			m_ratio_changing_peak, m_flag_dimension_change, m_flag_num_peak_change, \
			m_mode, m_flag_noise, m_flag_time_linkage);

		r_dbg.copy(*this);
		r_dbg.calculate_global_optima();
		*this = r_dbg;
	}

}


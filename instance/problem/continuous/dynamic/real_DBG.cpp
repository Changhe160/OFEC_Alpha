/*************************************************************************
* Project:Open Frameworks for Evolutionary Computation (OFEC)
*************************************************************************
* Author: Changhe Li
* Email: changhe.lw@gmail.com 
* Language: C++
*************************************************************************
*  This file is part of OFEC. This library is free software;
*  you can redistribute it and/or modify it under the terms of the
*  GNU General Public License as published by the Free Software
*  Foundation; either version 2, or (at your option) any later version.
*************************************************************************/
// Created: 11 May 2011
// Last modified:

#include "real_DBG.h"
#include "../../../../utility/functional.h"

namespace OFEC {

	real_DBG::real_DBG(const std::string &name, int rDimNumber,  int rNumPeaks,  int numObj,  ChangeType rT,  bool rFlagDimChange,  bool rFlagNumPeakChange
		,  int peakNumChangeMode,  bool flagNoise,  bool flagTimelinkage) :dynamic_continuous(name, rDimNumber, rNumPeaks, numObj)
	{
		//ctor
		
		m_rotM.resize(m_peak.size());
		for (auto i = 0; i < m_peak.size(); i++) {
			m_rotM[i].resize(m_variable_size, m_variable_size);
		}
		set_range(-5, 5);
		m_max_height = 100;
		m_min_height = 10;
		m_max_width = 10;
		m_min_width = 1;
		set_recurrent_noisy_severity(0.8);
		set_height_severity(5.0);
		set_width_severity(0.5);
		set_width(5); /// value in 1-10

		set_dimension_change(rFlagDimChange);
		set_num_peak_change_mode(peakNumChangeMode);
		set_flag_num_peaks_change(rFlagNumPeakChange);
		set_flag_noise(flagNoise);
		set_flag_time_linkage(flagTimelinkage);

		if (m_flag_dimension_change) {
			m_change.type = CT_Random;
			m_direction_dimension_change = true;
		}
		else if (m_flag_num_peak_change) {
			m_change.type = CT_Random;
			m_dir_num_peak_change = true;
		}
		else if (m_flag_noise || m_flag_time_linkage) {
			m_change.type = CT_Random;

		}
		else {
			m_change.type = rT;
		}
		m_change.counter = 0;

		if (m_change.type == CT_Recurrent || m_change.type == CT_RecurrentNoisy)      set_period(12);
		else      set_period(0);

		real *t = new real[m_peak.size()];

		set_choatic_constant(3.67);

		for (int i = 0; i < m_peak.size(); i++) {
			if (m_change.type == CT_Chaotic)
				t[i] = m_min_height + (m_max_height - m_min_height)*global::ms_global->m_uniform[caller::Problem]->next();
			else
				t[i] = 50;
		}
		set_height(t);
		delete[] t;
		
		std::vector<std::vector<real>> position(m_peak.size(),std::vector<real>(m_variable_size));
		
		for (int i = 0; i < m_peak.size(); i++) {
			for (int j = 0; j < m_variable_size; j++) {
				position[i][j] = m_domain[j].limit.first + (m_domain[j].limit.second - m_domain[j].limit.first)*global::ms_global->m_uniform[caller::Problem]->next();
			}
		}
		set_location(position);

		for (int i = 0; i < m_peak.size(); i++) m_pre_peak[i] = m_peak[i];
		m_pre_height = m_height;
		m_pre_width = m_width;
	}

	void real_DBG::set_period( int p){
		dynamic::set_period(p);
		m_rotP.resize(m_period);	
		for (int i = 0; i < m_period; i++) {
			m_rotP[i].resize(m_peak.size());
			for (int j = 0; j < m_peak.size(); j++)
				m_rotP[i][j].resize(m_variable_size);
		}	
	}

	void real_DBG::repair(real *x) {
		for (int j = 0; j < m_variable_size; j++) {
			if (m_domain[j].limit.second < x[j])
				x[j] = m_domain[j].limit.second;
			else if (m_domain[j].limit.first > x[j])
				x[j] = m_domain[j].limit.first;
		}
	}
	void real_DBG::change_height() {

		real step;
		for (int i = 0; i < m_peak.size(); i++) {
			if (m_flag_change[i] == false) continue;
			step = m_height_severity[i] * basic_change(m_change.type, m_min_height, m_max_height);
			m_height[i] = m_height[i] + step;
			if (m_height[i] > m_max_height || m_height[i] < m_min_height) m_height[i] = m_height[i] - step;

		}
	}
	void real_DBG::change_location(real angle) {

		if (m_change.type == CT_Chaotic) {
			for (int i = 0; i < m_peak.size(); i++) {
				if (m_flag_change[i] == false) continue;
				for (int j = 0; j < m_variable_size; j++)
					m_peak[i][j] = chaotic_value(m_peak[i][j], m_domain[j].limit.first, m_domain[j].limit.second, m_chaotic_constant);
			}
			return;
		}

		// for each basic function of dimension n(even number) , R=R(l1,l2)*R(l3,l4)*....*R(li-1,li), 0<=li<=n

		std::vector<int> d (m_variable_size);
		matrix I(m_variable_size, m_variable_size);
		std::vector<real> gene (m_variable_size);
		for (int i = 0; i < m_peak.size(); i++) {
			if ((m_change.type == CT_Recurrent || m_change.type == CT_RecurrentNoisy) && m_change.counter >= m_period) {
				d = m_rotP[m_change.counter%m_period][i];				
			}
			else {
				global::ms_global->m_uniform[caller::Problem]->shuffle(d.begin(), d.end());
				if (m_change.type == CT_Recurrent || m_change.type == CT_RecurrentNoisy)
					m_rotP[m_change.counter][i] = d;
			}

			if ((m_change.type == CT_Recurrent || m_change.type == CT_RecurrentNoisy) && m_change.counter%m_period == 0)
				m_peak[i] = m_ini_peak[i];
				
			if (m_flag_change[i] == false) continue;

			I.identify();
			for (int j = 0; j + 1 < m_variable_size; j += 2) {
				if (m_change.type == CT_SmallStep || m_change.type == CT_LargeStep || m_change.type == CT_Random)
					angle = basic_change(m_change.type, -OFEC_PI, OFEC_PI);
				I.set_rotation_axes(d[j], d[j + 1], angle);
				if (j == 0) m_rotM[i] = I;
				else
					m_rotM[i] *= I;
			}
			matrix m(1, m_variable_size);
			m.set_row(m_peak[i].data(), m_variable_size);
			m *= m_rotM[i];
			gene = m[0].vect();
			repair(gene.data());
			m_peak[i].vect() = gene;
		}
	}
	void real_DBG::copy(const problem &rP) {

		dynamic_continuous::copy(rP);

		auto& r_dbg = dynamic_cast<const real_DBG &>(rP);
		int dim = m_temp_dimension < rP.variable_size() ? m_temp_dimension : rP.variable_size();
		int peaks = m_peak.size() < r_dbg.get_num_peak() ? m_peak.size() : r_dbg.get_num_peak();

		m_prediction = r_dbg.m_prediction;
		if (m_change.type == CT_Recurrent || m_change.type == CT_RecurrentNoisy) {
			for (int i = 0; i < r_dbg.m_period; i++) {
				if (m_change.counter <= i) break;
				for (int j = 0; j < peaks; j++) {
					if (dim == m_temp_dimension) {// the number of dimensions decreases
						for (int m = 0, k = 0; k < dim; k++, m++)
							if (r_dbg.m_rotP[i][j][m] == dim) { k--; continue; }
							else
								m_rotP[i][j][k] = r_dbg.m_rotP[i][j][m];

					}
					else
						m_rotP[i][j] = r_dbg.m_rotP[i][j];						

				}

			}
		}
	}

	real  real_DBG::basic_change( ChangeType T,  real min,  real max) {
		real step, sign;
		switch (T) {
		case CT_SmallStep:
			step = -1 + 2 * global::ms_global->m_uniform[caller::Problem]->next();
			step = m_alpha*step*(max - min);
			break;
		case CT_Random:
			step = global::ms_global->m_normal[caller::Problem]->next();
			break;
		case CT_LargeStep:
			step = -1 + 2 * global::ms_global->m_uniform[caller::Problem]->next();
			if (step > 0)sign = 1;
			else if (step < 0) sign = -1;
			else sign = 0;
			step = (m_alpha*sign + (m_max_alpha - m_alpha)*step)*(max - min);
			break;
		case CT_Recurrent:
		case CT_Chaotic:
		case CT_RecurrentNoisy:
			break;
		}
		return step;
	}	

	void real_DBG::restore() {
		for (int i = 0; i < m_peak.size(); i++) {
			if (!m_flag_change[i]) {
				m_peak[i] = m_pre_peak[i];
				m_height[i] = m_pre_height[i];
				m_width[i] = m_pre_width[i];
			}
		}

	}
}
/******************************************************************************
* Project:Open Frameworks for Evolutionary Computation (OFEC)
*******************************************************************************
* Author: Changhe Li
* Email: changhe.lw@gmail.com
* Language: C++
*-------------------------------------------------------------------------------
*  This file is part of OFEC. This library is free software;
*  you can redistribute it and/or modify it under the terms of the
*  GNU General Public License as published by the Free Software
*  Foundation; either version 2, or (at your option) any later version.
*
*  see https://github.com/Changhe160/OFEC for more information
*
*-------------------------------------------------------------------------------
* 1. Changhe Li and Shengxiang Yang. A Generalized Approach to Construct Benchmark Problems for Dynamic Optimization.
* 2008. Berlin, Heidelberg: Springer Berlin Heidelberg.
* 2. Changhe Li, Shengxiang Yang, D. Pelta. Benchmark Generator for CEC'2012 Competition on Evolutionary Computation for
* Dynamic Optimization Problems,Tech. Rep., the School of Computer Science, China University of Geosciences, Wuhan, China, 2012.
*
*********************************************************************************/
// Created: 11 May 2011
// Last modified: 30 July 2018

#ifndef DYNAMICCONTINUOUS_H
#define DYNAMICCONTINUOUS_H

#include "../../../../core/problem/continuous/continuous.h"
#include "dynamic.h"

namespace OFEC {
#define CAST_DYN_CONT dynamic_cast<dynamic_continuous*>(global::ms_global->m_problem.get())

	class dynamic_continuous : public dynamic, public continuous{
	protected:
		
		std::vector<variable_vector<real>> m_peak;					    	// positions of local or global optima(local optima in Rotation_DBG,
		std::vector<variable_vector<real>> m_pre_peak;							// global optima of basic function in Composition_DBG)
		std::vector<variable_vector<real>> m_ini_peak;				        // save the initial positions
		std::vector<real> m_height;							// peak height in Rotation_DBG, height of global optima in Composition_DBG
		std::vector<real> m_width;                           // weight value of each basic function in Composition_DBG,  peak width in Rotation_DBG

		///TODO preHeight and preWidth not considered in current version
		std::vector<real> m_pre_height;
		std::vector<real>m_pre_width;

		real m_min_height, m_max_height;		// minimum\maximum height of all peaks(local optima) in Rotation_DBG(Composition_DBG)
		std::vector<real> m_height_severity;
		real m_min_width, m_max_width;
		std::vector<real> m_width_severity;

		std::vector<real> m_fitness;						    	// objective value of each basic funciton in Composition_DBG, peak height in Rotation_DBG
		std::vector<bool> m_flag_global_optima;                      // the index of the global optimal peak

		int m_current_peak;                         // the peak where the best individual is located
		std::vector<bool> m_flag_change;                      // whether peaks change or not
		int m_num_changing_peak;                       // the number of peaks that change
		real m_ratio_changing_peak = 1.0;                    // the ratio of changing peaks

		int m_num_visable_peak;                      // number of visable peaks, a peak is visable only if no peak is on top of it
		std::vector<int> m_num_tracking;							// accumulated number of peak[i] being tracked
		std::vector<int> m_height_order;
		int m_num_peak_tracked;
		std::vector<bool> m_tracked;
		std::vector<real> m_time_linkage;						//a random vector to change peaks position which are being tracked
	public:
		dynamic_continuous(const std::string &name, size_t rDimNumber, size_t rNumPeaks, size_t numObj = 1);
		void set_num_change(real rRatio);
		void set_height_severity(const real rS);
		void set_width_severity(const real rS);
		void set_height(const real *h);
		void set_location(const std::vector<std::vector<real>>&);
		void set_initial_location(const std::vector<std::vector<real>>&);
		virtual void set_width(const real w);
		int get_num_visible_peak();
		bool is_visible(int rIdx);
		bool is_tracked(real *gen, real obj);// is any peak tracked for the first time
		int get_num_peak_found();

		//15-07-2013
		bool is_GO_tracked();
		const std::vector<real>& get_nearest_peak(const std::vector<real>&);
	protected:
		void calculate_global_optima();
		void update_num_change();
		void update_num_visable_peak();
		void add_noise(real *x);
		void update_time_linkage();
		void move_peak( int idx);
		void copy(const problem&);
		void update_parameters();
		void update_pre_peak();
	};
}
#endif // DYNAMICCONTINUOUS_H

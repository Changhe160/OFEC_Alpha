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
* Interface of the GDBG
*
*
*********************************************************************************/

/*
1. Changhe Li and Shengxiang Yang. A Generalized Approach to Construct Benchmark Problems for Dynamic Optimization.
2008. Berlin, Heidelberg: Springer Berlin Heidelberg.
*/

// Created: 19 June 2018
// Last modified:
#ifndef DYNAMIC_H
#define DYNAMIC_H
#include "../../../../core/problem/problem.h"

namespace OFEC {
#define CAST_DYN dynamic_cast<dynamic*>(global::ms_global->m_problem.get())

	class dynamic : virtual public problem
	{
	public:
		enum ChangeType { CT_SmallStep = 0, CT_LargeStep, CT_Random, CT_Recurrent, CT_Chaotic, CT_RecurrentNoisy };
		struct Change {
			ChangeType type;
			int counter;
		};
	protected:
		int m_frequency;
		Change m_change;

		int m_period;					// definite period for values repeating
		int m_counter;			// counter of number of changes
		real m_recurrent_noisy_severity;			// deviation servity from the trajactory of recurrent change
		bool m_flag_dimension_change;			// flag=true, the number of dimensions change, otherwise no change,  default value is false
		bool m_direction_dimension_change;			// direction of change, dir=true means increasing the dimension, otherwise decrease it
		bool m_synchronize;                 // default=true all dimensions change at the same time

		int m_temp_dimension;                //a temporal variable for dimensional change only

		int m_num_peaks;
		bool m_flag_num_peak_change;                  // flag of the change of the number of peaks
		bool m_dir_num_peak_change;                   // true for increasing the number of peaks, otherwise decreasing the number of peaks
		int m_temp_num_peak;                         // temporal varibal for number of peaks change only


		static const unsigned ms_max_dimension = 15;
		static const unsigned ms_min_dimension = 2;     //should be greater than 1

		static const int ms_max_peaks = 100;
		static const int ms_min_peaks = 10;

		static thread_local std::unique_ptr<int> ms_init_peaks, ms_init_dimensions, ms_instances;
		real m_alpha, m_max_alpha;              // to control step severity
		real m_chaotic_constant;

		// features below added on NOV 22 2012
		int m_mode;		// for the number of peaks change; 1: periodic with fixed step, 2: periodic with random step, 3: chaotic change
		bool m_flag_noise;				// objective evaluation with noise in descision space
		bool m_flag_time_linkage;			// optima move to a random posotion if they are beging tracked 

		// severity of noise and time-linkage added in noisy and time-linkage enviroment, June 05 2014
		real m_noise_severity, m_time_linkage_severity;

		void set_dimension_change( bool rFlag);
		void set_change_dirction( bool rFlag);

		bool m_flag_trigger_time_linkage = false;
		static std::vector<std::string> ms_type;
	public:
		
		dynamic(const std::string &name,  size_t rDimNumber, size_t rNumPeaks, size_t numObj = 1);
		virtual ~dynamic() = 0;

		void set_frequency( int rChangeFre);
		virtual void set_period( int rPeriod);
		void set_type( ChangeType rT);
		void set_flag_num_peaks_change( bool rPC);
		bool get_flag_num_peaks_change() {
			return m_flag_num_peak_change;
		}
		void set_flag_synchronize( bool rFlag);
		void set_recurrent_noisy_severity( real rSeverity);

		void set_alpha(real rAlpha) {
			m_alpha = rAlpha;
		};
		void set_max_alpha( real rMaxAlpha) {
			m_max_alpha = rMaxAlpha;
		};
		void set_choatic_constant(real rValue) {
			m_chaotic_constant = rValue;
		}

		int get_frequency()const {
			return m_frequency;
		};
		int get_change_counter()const {
			return m_counter;
		};
		int get_period()const {
			return m_period;
		}
		ChangeType get_type() const {
			return m_change.type;
		};
		bool get_flag_dimension_change() const {
			return m_flag_dimension_change;
		};
		bool get_dir_dimension_change() const {
			return m_direction_dimension_change;
		};
		bool get_flag_synchronize_change()const {
			return m_synchronize;
		};

		void set_num_peak_change_mode(int mode);
		int get_num_peak_change_mode();
		void set_flag_noise( bool flag);
		int get_num_peak()const;
		void set_flag_time_linkage( bool flag);
		bool get_flag_time_linkage() const{
			return m_flag_time_linkage;
		}
		void change();
		real get_recurrent_noise(int x, real min, real max, real amplitude, real angle, real noisy_severity = 1.);
		real chaotic_step(real x, real min, real max, real scale = 1.0);
		bool predict_change( int evalsMore);
		void set_noise_severity(real value);
		void set_time_linkage_severity(real value);
		bool &get_trigger_flag_time_linkage() {
			return m_flag_trigger_time_linkage;
		}
	protected:
		virtual void change_random() {};
		virtual void change_small_step() {};
		virtual void change_large_step() {};
		virtual void change_recurrent() {};
		virtual void change_chaotic() {};
		virtual void change_recurrent_noisy() {};

		virtual void change_dimension() {};
		virtual void change_num_peak() {};


		void copy(const problem & rP);
		void update_parameters();
	
	};
}

#endif
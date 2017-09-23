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
* class dynamic_problem defines dynamic optimization problems
*
*********************************************************************************/
#ifndef OFEC_DYNAMIC_PROBLEM_H
#define OFEC_DYNAMIC_PROBLEM_H

#include"../problem.h"
#include"../../global.h"

enum class change_type { CT_small_step = 0, CT_large_step, CT_random, CT_recurrent, CT_chaotic, CT_recurrent_noisy };

namespace OFEC {
	class dynamic_problem : virtual public problem {

		struct s_change_type {
			change_type type;
			int counter;
			s_change_type & operator=(const s_change_type & change_type) {
				if (this == &change_type)  return *this;
				type = change_type.type;
				counter = change_type.counter;
				return *this;
			};
		};

	protected:
		int m_change_interval;
		s_change_type m_change_type;

		int m_period;				           // definite period for values repeating
		int m_change_counter;			       // counter of number of changes
		double m_noisy_severity;		       // deviation serverity from the trajactory of recurrent change

		bool m_flag_dimension_change;	       // flag=true, the number of dimensions change, otherwise no change,  default value is false
		bool m_dir_dimension_change;	       // direction of change, dir=true means increasing the dimension, otherwise decrease it
		bool m_synchronize;                    // default=true all dimensions change at the same time
		int m_dim_number_temp;                 //a temporal variable for dimensional change only
		int m_num_peaks;
		bool m_flag_num_peaks_change;          // flag of the change of the number of peaks
		bool m_dir_num_peaks_change;           // true for increasing the number of peaks, otherwise decreasing the number of peaks
		int m_num_peaks_temp;                  // temporal varibal for number of peaks change only

		static const unsigned msc_max_dimension_number = 15;
		static const unsigned msc_min_dimension_number = 2;     //should be greater than 1
		static const int msc_max_num_peaks = 100;
		static const int msc_min_num_peaks = 10;

		static thread_local unique_ptr<int> ms_init_num_peaks, ms_init_num_dim, ms_num_instance;
		double m_alpha, m_max_alpha;              // to control step severity
		double m_chaotic_constant;

		int m_num_peaks_change_mode;	// for the number of peaks change; 1: periodic with fixed step, 2: periodic with random step, 3: chaotic change
		bool m_noise_flag;		        // objective evaluation with noise in descision space
		bool m_time_linkage_flag;	    // optima move to a random posotion if they are beging tracked 

		double m_noise_severity_, m_time_linkage_severity; // severity of noise and time-linkage added in noisy and time-linkage enviroment

		void set_dimension_change(const bool flag);
		void set_change_dirction(const bool flag);

		bool m_flag_trigger_time_linkage;
	public:
		static const int msc_num_change_types = 12;

		dynamic_problem(const int dim_number, const int num_peaks, const unsigned num_obj = 1);
		virtual ~dynamic_problem() = 0;

		dynamic_problem & operator=(const dynamic_problem & dynamic_problem);

		void set_change_interval(const int change_interval);
		virtual bool set_period(const int period);
		void set_change_type(const s_change_type &change_type);
		void set_change_type(const change_type type);
		void set_num_peaks_change(const bool peaks_change);
		bool get_flag_num_peaks_change() {
			return m_flag_num_peaks_change;
		}
		void set_synchronize(const bool flag);
		void set_noisy_severity(const double severity);

		void set_alpha(const double alpha) {
			m_alpha = alpha;
		};
		void set_max_alpha(const double max_alpha) {
			m_max_alpha = max_alpha;
		};
		void set_choatic_constant(const double value) {
			m_chaotic_constant = value;
		}

		int get_change_fre()const {
			return m_change_interval;
		};
		int get_change_counter()const {
			return m_change_counter;
		};
		int get_period()const {
			return m_period;
		}
		change_type get_change_type() const {
			return m_change_type.type;
		};
		bool get_flag_dimension_change() const {
			return m_flag_dimension_change;
		};
		bool get_dir_dimension_change() const {
			return m_dir_dimension_change;
		};
		bool get_flag_synchronize_change()const {
			return m_synchronize;
		};

		void set_num_peak_change_mode(const int mode);
		int get_num_peak_change_mode();
		void set_noise_flag(const bool flag);
		int get_number_of_peak()const;
		void set_time_linkage_flag(const bool flag);
		bool get_flag_time_linkage() {
			return m_time_linkage_flag;
		}
		void change();
		double sin_value_noisy(const int x, const double min, const double max, const double amplitude, const double angle, const double noisy_severity = 1.);
		double chaotic_step(const double x, const double min, const double max, const double scale = 1.0);
		bool predict_change(const int evals_more);
		void set_noise_severity_(double value);
		void set_timelinkage_severity(double value);
		bool &get_trigger_timelinkage() {
			return m_flag_trigger_time_linkage;
		}
		static int get_initial_num_peaks();
	protected:
		virtual void random_change() {};
		virtual void small_step_change() {};
		virtual void large_step_change() {};
		virtual void recurrent_change() {};
		virtual void chaotic_change() {};
		virtual void recurrent_noisy_change() {};

		virtual void change_dimension() {};
		virtual void change_num_peaks() {};


		void parameter_setting(problem * rp);
		virtual void  free_memory() {};
	};
}

#endif // !OFEC_DYNAMIC_PROBLEM_H

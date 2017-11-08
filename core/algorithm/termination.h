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
* This file defines several commonly used termination criteria.
*
*********************************************************************************/

#ifndef OFEC_TERMINATION_H
#define OFEC_TERMINATION_H
#include<chrono>

#include "../../utility/typevar/typevar.h"


namespace OFEC {
	//terminate when the global opt. is found
	class termination
	{
	public:
		termination(param_map &v);
		virtual ~termination() {}
		termination() = default;
		virtual bool terminating();
		bool terminated() {
			return m_terminated;
		}
		void set_terminated_true() {
			m_terminated = true;
			m_end_time = std::chrono::system_clock::now();
		}
		void set_terminating_true() { 
			m_isTerminating = true; 
		}
		double duration() {
			return std::chrono::duration<double>(m_end_time - m_start_time).count();
		}
		void disable() {
			m_enable = false;
		}
		void enable() {
			m_enable = true;
		}
	protected:
		bool m_terminated = false, m_isTerminating = false, m_enable=true;
		std::chrono::time_point<std::chrono::system_clock> m_start_time = std::chrono::system_clock::now(), m_end_time;
		unsigned long m_maxTime = 12 * 3600; // 12 hours time by default
	};

	//terminate when the maximum number of iterations is reached
	class term_max_iteration :public termination {
	protected:
		int m_max_iter;
	public:
		using termination::terminating;
		term_max_iteration(param_map &v) :termination(v) {
			if (v.find(param_maxIter) != v.end()) m_max_iter = v[param_maxIter];
			else	THROW("para_maxIter is not given");
		}
		term_max_iteration(int iters) :m_max_iter(iters) {	}
		bool terminating(int value);
	};

	//terminate when the maximum number of evaluations is reached
	class term_max_evals :public termination {
	protected:
		int m_max_evals;
	public:
		term_max_evals(param_map &v) :termination(v) {
			if (v.find(param_maxEvals) != v.end()) m_max_evals = v[param_maxEvals];
			else	THROW("m_max_evals is not given");
		}
		term_max_evals(int evals) :m_max_evals(evals) { }
		bool terminating();
	};

	//terminate when the best solution remains over a number of successive iterations
	class term_best_remain :public termination {
	protected:
		int m_max_iter;
		int m_suc_iter = 0;
		double m_previous=0, m_current=0;
	public:
		using termination::terminating;
		term_best_remain(param_map &v) :termination(v) {
			if (v.find(param_maxSucIter) != v.end()) m_max_iter = v[param_maxSucIter];
			else	THROW("param_maxSucIter is not given");
		}
		term_best_remain(int iters) :m_max_iter(iters) {	}

		bool terminating(double value);
	};

	//terminate when the average objectives changes less than a threshold value over a number of successive iterations
	class term_mean_remain :public termination {
	protected:
		int m_max_iter;
		int m_suc_iter = 0;
		double m_previous = 0;
		double m_current = 0;
		double m_epsilon = 1.E-2;
	public:
		using termination::terminating;
		term_mean_remain(param_map &v) :termination(v) {
			if (v.find(param_epsilon) != v.end()) m_epsilon = v[param_epsilon];
			else	THROW("param_epsilon is not given");
			if (v.find(param_maxSucIter) != v.end()) m_max_iter = v[param_maxSucIter];
			else	THROW("param_maxSucIter is not given");
		}
		term_mean_remain(int iters, double epsilon, double value) :m_max_iter(iters), m_epsilon(epsilon), m_previous(value), m_current(value) { }
	
		bool terminating(double value);

		void reset(int maxIter, double value) {
			m_previous = m_current = value;
			m_suc_iter = 0;
		}
		void set_epsilon(double epsilon) {
			m_epsilon = epsilon;
		}
	};

	//terminate when the variance of objective ls less than a small value
	class term_variance :public termination {
	protected:
		double m_epsilon;
	public:
		using termination::terminating;
		term_variance(param_map &v) :termination(v) {
			if (v.find(param_epsilon) != v.end()) m_epsilon = v[param_epsilon];
			else	THROW("param_epsilon is not given");
		}
		term_variance(double epsilon) :m_epsilon(epsilon) {}
		bool terminating(double var);
		void set_epsilon(double epsilon) {
			m_epsilon = epsilon;
		}

	};

	//terminate if the population shows no improvement over a certain sucessive iterations
	class term_stagnation :public termination {
	protected:
		int m_suc_iter = 1;
		int m_cnt = 0; //connter
	public:
		using termination::terminating;
		term_stagnation(param_map &v) :termination(v) {
			if (v.find(param_maxSucIter) != v.end()) m_suc_iter = v[param_maxSucIter];
			else	THROW("param_maxSucIter is not given");
		}
		term_stagnation(int iter) :m_suc_iter(iter) {}
		bool terminating(int value);
		void set_max_iter(int iter) {
			m_suc_iter = iter;
		}

	};


}

#endif // !OFEC_TERMINATION_H


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
		bool terminating();
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
		real duration() {
			return std::chrono::duration<real>(m_end_time - m_start_time).count();
		}
		void disable() {
			m_enable = false;
		}
		void enable() {
			m_enable = true;
		}
		const std::string & criterion()const { return m_name; }
	protected:
		bool m_terminated = false, m_isTerminating = false, m_enable = true;
		std::chrono::time_point<std::chrono::system_clock> m_start_time = std::chrono::system_clock::now(), m_end_time;
		unsigned long m_maxTime = 12 * 3600; // 12 hours time by default
		std::string  m_name;
	};

	//terminate when the maximum number of iterations is reached
	class term_max_iteration :public termination {
	protected:
		int m_max_iter;
	public:
		using termination::terminating;
		term_max_iteration(param_map &v) :termination(v) {
			if (v.find("maxIter") != v.end()) m_max_iter = v.at("maxIter");
			else	THROW("para_maxIter is not given");
			m_name = "max iterations";
		}
		term_max_iteration(int iters) :m_max_iter(iters) { m_name = "max iterations"; }
		bool terminating(int value);
	};

	//terminate when the maximum number of evaluations is reached
	class term_max_evals :public termination {
	protected:
		int m_max_evals;
	public:
		term_max_evals(param_map &v) :termination(v) {
			if (v.find("maxEvals") != v.end()) m_max_evals = v.at("maxEvals");
			else	THROW("m_max_evals is not given");
			m_name = "max evalutations";
		}
		term_max_evals(int evals) :m_max_evals(evals) { 
			m_name = "max evalutations";
		}
		bool terminating(int evals);
	};

	//terminate when the best solution remains over a number of successive iterations
	class term_best_remain :public termination {
	protected:
		int m_max_iter;
		int m_suc_iter = 0;
		real m_previous = 0, m_current = 0;
	public:
		using termination::terminating;
		term_best_remain(param_map &v) :termination(v) {
			if (v.find("maxSucIter") != v.end()) m_max_iter = v.at("maxSucIter");
			else	THROW("param_maxSucIter is not given");
			m_name = "convergence best";
		}
		term_best_remain(int iters) :m_max_iter(iters) { m_name = "convergence best"; }

		bool terminating(real value);
	};

	//terminate when the average objectives changes less than a threshold value over a number of successive iterations
	class term_mean_remain :public termination {
	protected:
		int m_max_iter;
		int m_suc_iter = 0;
		real m_previous = 0;
		real m_current = 0;
		real m_epsilon = 1.E-2;
	public:
		using termination::terminating;
		term_mean_remain(param_map &v) :termination(v) {
			if (v.find("epsilon") != v.end()) m_epsilon = v.at("epsilon");
			else	THROW("param_epsilon is not given");
			if (v.find("maxSucIter") != v.end()) m_max_iter = v.at("maxSucIter");
			else	THROW("param_maxSucIter is not given");
			m_name = "convergence mean";
		}
		term_mean_remain(int iters, real epsilon, real value) :m_max_iter(iters), m_epsilon(epsilon), m_previous(value), m_current(value) {
			m_name = "convergence mean";
		}

		bool terminating(real value);

		void reset(int maxIter, real value) {
			m_previous = m_current = value;
			m_suc_iter = 0;
		}
		void set_epsilon(real epsilon) {
			m_epsilon = epsilon;
		}
	};

	//terminate when the variance of objective ls less than a small value
	class term_variance :public termination {
	protected:
		real m_epsilon;
	public:
		using termination::terminating;
		term_variance(param_map &v) :termination(v) {
			if (v.find("epsilon") != v.end()) m_epsilon = v.at("epsilon");
			else	THROW("param_epsilon is not given");
			m_name = "convergence variance";
		}
		term_variance(real epsilon) :m_epsilon(epsilon) {
			m_name = "convergence variance";
		}
		bool terminating(real var);
		void set_epsilon(real epsilon) {
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
			if (v.find("maxSucIter") != v.end()) m_suc_iter = v.at("maxSucIter");
			else	THROW("param_maxSucIter is not given");
			m_name = "stagnation";
		}
		term_stagnation(int iter) :m_suc_iter(iter) {
			m_name = "stagnation";
		}
		bool terminating(int value);
		void set_max_iter(int iter) {
			m_suc_iter = iter;
		}

	};


}

#endif // !OFEC_TERMINATION_H
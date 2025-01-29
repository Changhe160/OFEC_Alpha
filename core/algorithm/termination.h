/******************************************************************************
* Project:Open Frameworks for Evolutionary Computation (OFEC)
*******************************************************************************
* Author: Yiya Diao & Junchen Wang & Changhe Li
* Email: diaoyiyacug@gmail.com & wangjunchen.chris@gmail.com & changhe.lw@gmail.com
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

#include <chrono>
#include <string>
#include "../definition.h"

namespace ofec {
	class Termination {
	public:
		Termination();
		virtual ~Termination() = default;
		bool terminating();
		bool terminated() {
			return m_terminated;
		}
		void setTerminatedTrue() {
			m_terminated = true;
			m_end_time = std::chrono::system_clock::now();
		}
		void setTerminatingTrue() {
			m_isTerminating = true;
		}
		Real duration() {
			return std::chrono::duration<Real>(m_end_time - m_start_time).count();
		}
		void disable() {
			m_enable = false;
		}
		void enable() {
			m_enable = true;
		}
		const std::string& criterion() const { return m_name; }

		void setMaxTime(unsigned long time) {
			m_max_time = time;
		}
	protected:
		bool m_terminated = false, m_isTerminating = false, m_enable = true;
		std::chrono::time_point<std::chrono::system_clock> m_start_time = std::chrono::system_clock::now(), m_end_time;
		unsigned long m_max_time;
		std::string  m_name;
	};

	//terminate when the maximum number of iterations is reached
	class TerminationMaximumIterations : public Termination {
	protected:
		int m_max_iters;
	public:
		using Termination::terminating;
		TerminationMaximumIterations(int iters) : Termination(), m_max_iters(iters) { m_name = "max iterations"; }
		bool terminating(int value);
	};

	//terminate when the maximum number of evaluations is reached
	class TerminationMaximumEvaluations : public Termination {
	protected:
		int m_maximum_evaluations;
	public:
		TerminationMaximumEvaluations(int evals) : m_maximum_evaluations(evals) { m_name = "max evalutations"; }
		int maximumEvaluations() const { return m_maximum_evaluations; }
		virtual bool terminating(int evals);
	};

	//terminate when the best solution remains over a number of successive iterations
	class TerminationBestRemain : public Termination {
	protected:
		int m_max_iters;
		int m_suc_iters = 0;
		Real m_previous = 0, m_current = 0;
	public:
		using Termination::terminating;
		TerminationBestRemain(int iters) :m_max_iters(iters) { m_name = "convergence best"; }

		bool terminating(Real value);
	};

	//terminate when the average objectives changes less than a threshold value over a number of successive iterations
	class TerminationMeanRemain : public Termination {
	protected:
		int m_max_iters;
		int m_suc_iters = 0;
		Real m_previous = 0;
		Real m_current = 0;
		Real m_epsilon = 1.E-2;
	public:
		using Termination::terminating;
		TerminationMeanRemain(int iters, Real epsilon, Real value) :m_max_iters(iters), m_epsilon(epsilon), m_previous(value), m_current(value) {
			m_name = "convergence mean";
		}

		bool terminating(Real value);

		void reset(int maxIter, Real value) {
			m_previous = m_current = value;
			m_suc_iters = 0;
		}
		void setEpsilon(Real epsilon) {
			m_epsilon = epsilon;
		}
	};

	//terminate when the variance of objective ls less than a small value
	class TerminationVariance : public Termination {
	protected:
		Real m_epsilon;
	public:
		using Termination::terminating;
		TerminationVariance(Real epsilon) :m_epsilon(epsilon) {
			m_name = "convergence variance";
		}
		bool terminating(Real var);
		void setEpsilon(Real epsilon) {
			m_epsilon = epsilon;
		}

	};

	//terminate if the population shows no improvement over a certain sucessive iterations
	class TerminationStagnation :public Termination {
	protected:
		int m_suc_iters = 1;
		int m_cnt = 0; //connter
	public:
		using Termination::terminating;
		TerminationStagnation(int iter) :m_suc_iters(iter) {
			m_name = "stagnation";
		}
		bool terminating(int value);
		void setMaxIters(int iter) {
			m_suc_iters = iter;
		}

	};


}

#endif // !OFEC_TERMINATION_H
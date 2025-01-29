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
*
* class Algorithm is an abstract for all algorithms.
*
*********************************************************************************/

#ifndef OFEC_ALGORITHM_H
#define OFEC_ALGORITHM_H

#include <array>
#include <memory>
#include <list>
#include <atomic>
#include "termination.h"
#include "../instance.h"

namespace ofec {
	class Environment;
	class DatumBase;
	class SolutionBase;

	class Algorithm : virtual public Instance {
		OFEC_ABSTRACT_INSTANCE(Algorithm)
	public:
		virtual void reset();
		void initialize(Environment *env);
		void run(Environment *env);
		int maximumEvaluations() const { return m_maximum_evaluations; }
		const size_t& evaluations() const { return m_evaluations; }
		void increaseEvaluations() { m_evaluations++; }
		void terminate();
		bool terminated();
		virtual bool terminating();
		Real duration();
		Termination* termination() { return m_termination.get(); }
		void setTermination(Termination *term);
		bool initialized() const { return m_initialized; }
		virtual void datumUpdated(Environment *env, DatumBase& datum);
		virtual void handleEvaluation(const SolutionBase &s, Environment *env, int rf) {}

	protected:
		void addInputParameters();
		virtual void initialize_(Environment *env);
		virtual void run_(Environment *env) = 0;
		virtual void handleDatumUpdated(Environment* env) {}

		std::unique_ptr<Termination> m_termination;
		int m_maximum_evaluations;
		size_t m_evaluations;

	private:
		Algorithm(const Algorithm&) = delete;
		Algorithm& operator=(const Algorithm&) = delete;

		bool m_initialized = false;
		std::atomic<bool> m_terminate_flag = false;
	};

}
#endif // !OFEC_ALGORITHM_H

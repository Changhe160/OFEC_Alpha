#include "environment.h"

#ifdef OFEC_STATISTICS
#include <statistics/global.h>
#endif

#ifdef OFEC_PLAYBACK
#include <playback/global.h>
#endif

namespace ofec {
	int Environment::evaluate_(SolutionBase &sol) {
		m_algorithm->increaseEvaluations();
		sol.timeEvaluate() = m_algorithm->evaluations();
		m_problem->evaluate(sol.variableBase(), sol.objective(), sol.constraint());
		return kNormalEval;
	}

	int Environment::evaluate(SolutionBase &sol, bool effective) {
		if (!m_problem || !m_problem->initialized()) {
			throw Exception("Problem not initialized.");
		}
		int rf = kNormalEval;
		if (effective && m_algorithm && m_algorithm->initialized()) {
			rf = evaluate_(sol);
			m_algorithm->handleEvaluation(sol, this, rf);
#ifdef OFEC_STATISTICS
			ofec_statistics::g_record_task->handleEvaluation(sol, this, rf);
#endif
#ifdef OFEC_PLAYBACK
			ofec_playback::g_buffer_manager->handleEvaluation(sol, this, rf);
#endif
			if (m_algorithm->termination() && m_algorithm->terminating()) {
				rf |= kTerminate;
			}
		}
		else {
			m_problem->evaluate(sol.variableBase(), sol.objective(), sol.constraint());
		}
		return rf;
	}

	void Environment::initialize(Real seed) {
		if (seed > 0) {
			initialize(std::make_shared<Random>(seed));
		}
		else {
			reset();
			restoreInputParameters();
			initialize_();
		}
	}

	void Environment::initialize(const std::shared_ptr<Random> &random) {
		reset();
		restoreInputParameters();
		setRandom(random);
		initialize_();
	}

	void Environment::initializeProblem(Real seed) {
		if (seed > 0) {
			initializeProblem(std::make_shared<Random>(seed));
		}
		else {
			m_problem->reset();
			m_problem->restoreInputParameters();
			m_problem->initialize(this);
		}
	}

	void Environment::initializeProblem(const std::shared_ptr<Random> &random) {
		m_problem->reset();
		m_problem->restoreInputParameters();
		m_problem->setRandom(random);
		m_problem->initialize(this);
	}

	void Environment::initializeAlgorithm(Real seed) {
		if (seed > 0) {
			initializeAlgorithm(std::make_shared<Random>(seed));
		}
		else {
			m_algorithm->reset();
			m_algorithm->restoreInputParameters();
			m_algorithm->initialize(this);
		}
	}

	void Environment::initializeAlgorithm(const std::shared_ptr<Random> &random) {
		m_algorithm->reset();
		m_algorithm->restoreInputParameters();
		m_algorithm->setRandom(random);
		m_algorithm->initialize(this);
	}

	void Environment::runAlgorithm() {
		m_algorithm->run(this);
	}

	void Environment::clear() {
		m_algorithm.reset();
		m_problem.reset();
		m_random.reset();
	}
}

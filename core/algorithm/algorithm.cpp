#include "algorithm.h"
#include "../environment/environment.h"
#include "../datum_base.h"

#ifdef OFEC_PLAYBACK
#include <playback/global.h>
#endif
#ifdef OFEC_STATISTICS
#include <statistics/global.h>
#endif

namespace ofec {
	void Algorithm::addInputParameters()  {
		m_input_parameters.add("maximum evaluations", new RangedInt(m_maximum_evaluations, -1, 10000000, -1));
	}

	void Algorithm::reset() {
		m_evaluations = 0;
	}

	void Algorithm::initialize(Environment *env) {
		m_initialized = false;
		initialize_(env);
		m_initialized = true;
		m_terminate_flag = false;
	}

	void Algorithm::initialize_(Environment *env) {
		m_termination.reset(m_maximum_evaluations > 0 ? new TerminationMaximumEvaluations(m_maximum_evaluations) : new Termination);
	}

	void Algorithm::run(Environment* env) {
		if (!m_initialized) {
			throw Exception("Please initialize the algorithm.");
		}
		run_(env);
		m_termination->setTerminatedTrue();
	}

	void Algorithm::terminate() {
		m_terminate_flag = true;
	}

	bool Algorithm::terminated() {
		if (m_initialized) {
			return m_termination->terminated();
		}
		else {
			return true;
		}
	}

	bool Algorithm::terminating() {
		if (m_terminate_flag) {
			return true;
		}
		if (auto t = dynamic_cast<TerminationMaximumEvaluations*>(m_termination.get())) { //by default 
			return t->terminating(m_evaluations);
		}
		return m_termination->terminating();
	}

	Real Algorithm::duration() {
		if (m_initialized) {
			return m_termination->duration();
		}
		return 0;
	}

	void Algorithm::setTermination(Termination *term) {
		m_termination.reset(term);
	}

	void Algorithm::datumUpdated(Environment *env, DatumBase &datum) {
		datum.m_update_flag = true;
		handleDatumUpdated(env);
#ifdef OFEC_PLAYBACK
		ofec_playback::g_buffer_manager->handleAlgorithmDatumUpdated(env);
#endif // OFEC_PLAYBACK
#ifdef OFEC_STATISTICS
		ofec_statistics::g_record_task->handleAlgorithmDatumUpdated(env);
#endif // OFEC_STATISTICS
		datum.m_update_flag = false;
	}
}

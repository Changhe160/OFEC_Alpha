#include "../global.h"
#include <cmath>

namespace OFEC {

	termination::termination(param_map &v) :m_maxTime(24 * 3600 * 30) {
		if (v.find("maxRunTime") != v.end())
			m_maxTime = (int)v.at("maxRunTime");

	}

	bool termination::terminating() {
		if (!m_enable) return false;

		if (m_isTerminating) return true;
		if (global::ms_global->m_problem != nullptr && global::ms_global->m_problem->solved())
			return true;

		auto durat = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - m_start_time).count();
		if (durat >= m_maxTime)
			return true;

		return false;
	};

	bool term_max_evals::terminating(int evals) {

		if (termination::terminating()) return true;

		if (evals >= m_max_evals) return true;

		return false;
	}


	bool term_max_iteration::terminating(int value) {
		// Assume that vp[0] is a main population that records the number of iterations since the run starts

		if (termination::terminating()) return true;

		return value >= m_max_iter;
	}


	bool term_best_remain::terminating(real value) {

		if (termination::terminating()) return true;

		m_current = value;

		if (m_current != m_previous) {
			m_previous = m_current;
			m_suc_iter = 0;
		}
		else {
			m_suc_iter++;
		}

		return m_suc_iter > m_max_iter;
	}


	bool term_mean_remain::terminating(real value) {

		if (termination::terminating()) return true;

		m_current = value;

		if (fabs(m_previous - m_current)>m_epsilon) {
			m_suc_iter = 0;
			m_previous = m_current;
		}
		else	m_suc_iter++;

		return m_suc_iter >= m_max_iter;
	}

	bool term_variance::terminating(real var) {

		if (termination::terminating()) return true;

		return var < m_epsilon;

	}

	bool term_stagnation::terminating(int value) {

		if (termination::terminating()) return true;

		if (value == 0) m_cnt++;
		else m_cnt = 0;

		return m_cnt >= m_suc_iter;
	}
}

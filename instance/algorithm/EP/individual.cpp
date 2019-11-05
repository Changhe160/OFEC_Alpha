#include "individual.h"
#include "../../../core/problem/continuous/continuous.h"

namespace OFEC {
	namespace EP {
		individual::individual(size_t num_obj, size_t num_cont, size_t size_var) : OFEC::individual<>(num_obj, num_cont, size_var), m_eta(size_var){
			initialize_eta();
		}

		void individual::resize_variable(size_t n) {
			m_var.resize(n);
			m_eta.resize(n);
			initialize_eta();
		}

		void individual::initialize_eta() {
			for (size_t i = 0; i < m_eta.size(); ++i)
				m_eta[i] = (CONTINUOUS_CAST->range(i).second - CONTINUOUS_CAST->range(i).first) / 10;
		}
	}
}
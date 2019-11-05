#include "particle.h"
#include "../../../core/problem/continuous/continuous.h"

namespace OFEC {
	particle::particle(size_t num_obj, size_t num_con, size_t size_var) :
	        individual(num_obj, num_con, size_var),
	        m_pbest(num_obj, num_con, size_var),
	        m_vel(size_var) {}

	real particle::velocity()	{
		real ve = 0;
		for (size_t i = 0; i<m_var.size(); i++)
			ve += m_vel[i] * m_vel[i];
		return sqrt(ve);
	}
	void particle::initialize_velocity() {
		for (size_t i = 0; i<m_var.size(); i++) {
			auto& range = CONTINUOUS_CAST->range(i);
			m_vel[i] = global::ms_global->m_uniform[caller::Algorithm]->next_non_standard(range.first, range.second);			
		}
	}

	void particle::initialize_velocity(const std::vector<std::pair<real, real>> &vrange) {
		for (size_t i = 0; i<m_var.size(); i++) {	
			m_vel[i] = global::ms_global->m_uniform[caller::Algorithm]->next_non_standard(vrange[i].first, vrange[i].second);
		}
	}

	void particle::initialize_velocity(real min, real max) {
		for (int i = 0; i < m_var.size(); i++) {
			m_vel[i] = global::ms_global->m_uniform[caller::Algorithm]->next_non_standard(min, max);
		}
	}

	void particle::next_velocity(const solution<> &lbest, real w, real c1, real c2) {		
		for (size_t j = 0; j<m_var.size(); j++) {
			m_vel[j] = w * m_vel[j] + c1 * global::ms_global->m_uniform[caller::Algorithm]->next()*(m_pbest.variable()[j] - m_var[j]) + c2 * global::ms_global->m_uniform[caller::Algorithm]->next()*(lbest.variable()[j] - m_var[j]);
		}
	}

	void particle::clamp_velocity() {
		for (size_t j = 0; j<m_var.size(); j++) {	
			auto& range = CONTINUOUS_CAST->range(j);
			if (m_var[j]>range.second) {
				m_var[j] = range.second;
				m_vel[j] = 0;
			}
			else if (m_var[j]<range.first) {
				m_var[j] = range.first;
				m_vel[j] = 0;
			}
		}
	}
	
	void particle::move() {
		for (size_t j = 0; j<m_var.size(); j++) {			
			m_var[j] += m_vel[j];
		}
	}
	
	void particle::resize_variable(size_t n) {
		m_var.resize(n);
		m_pbest.variable().resize(n);
		m_vel.resize(n);
	}
}

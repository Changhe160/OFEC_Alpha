#include "particle.h"
#include "../../../../../core/problem/continuous/continuous.h"
#include "../../../../../core/environment/environment.h"
#include "../../../../../utility/functional.h"

namespace ofec {
	Particle::Particle(Environment *env) : Particle(
		CAST_CONOP(env->problem())->numberObjectives(),
		CAST_CONOP(env->problem())->numberConstraints(),
		env->problem()->numberVariables()) {}

	Particle::Particle(size_t num_obj, size_t num_con, size_t size_var) :
		Solution(num_obj, num_con, size_var),
		m_pbest(num_obj, num_con, size_var),
		m_vel(size_var),
		m_vel_max(size_var) {}

	Particle::Particle(const Solution<> &sol) :
		Solution(sol),
		m_pbest(sol),
		m_vel(sol.variable().size()),
		m_vel_max(sol.variable().size()) {}

	Real Particle::speed() const {
		Real ve = 0;
		for (size_t j = 0; j < variable().size(); j++) {
			ve += m_vel[j] * m_vel[j];
		}
		return sqrt(ve);
	}

	void Particle::initVelocityMax(Environment *env, Random *rnd) {
		for (size_t j = 0; j < variable().size(); j++) {
			auto &range = CAST_CONOP(env->problem())->range(j);
			m_vel_max[j].max = (range.second - range.first) / 2;
			m_vel_max[j].min = -m_vel_max[j].max;
		}
	}

	void Particle::initVelocity(Environment *env, Random *rnd) {
		for (size_t j = 0; j < variable().size(); j++) {
			auto &range = CAST_CONOP(env->problem())->range(j);
			m_vel[j] = (range.second - range.first) * (-0.5 + rnd->uniform.next()) / 2;
		}
	}

	void Particle::initVelocity(const std::vector<std::pair<Real, Real>> &vrange, Random *rnd) {
		for (size_t j = 0; j < variable().size(); j++) {
			m_vel[j] = (vrange[j].second - vrange[j].first) * (-0.5 + rnd->uniform.next()) / 2;
		}
	}

	void Particle::initVelocity(Real min, Real max, Random *rnd) {
		for (int j = 0; j < variable().size(); j++) {
			m_vel[j] = rnd->uniform.nextNonStd(min, max);
		}
	}

	void Particle::nextVelocity(const Solution<> *lbest, Real w, Real c1, Real c2, Random *rnd) {
		for (size_t j = 0; j < variable().size(); j++) {
			m_vel[j] = w * m_vel[j]
				+ c1 * rnd->uniform.next() * (m_pbest.variable()[j] - variable()[j])
				+ c2 * rnd->uniform.next() * (lbest->variable()[j] - variable()[j]);
			if (m_vel[j] > m_vel_max[j].max)
				m_vel[j] = m_vel_max[j].max;
			else if (m_vel[j] < m_vel_max[j].min)
				m_vel[j] = m_vel_max[j].min;
		}
	}

	void Particle::clampVelocity(Environment *env, Random *rnd) {
		for (size_t j = 0; j < variable().size(); j++) {
			auto &range = CAST_CONOP(env->problem())->range(j);
			if (variable()[j] > range.second) {
				variable()[j] = range.second;
				m_vel[j] = 0;
			}
			else if (variable()[j] < range.first) {
				variable()[j] = range.first;
				m_vel[j] = 0;
			}
		}
	}

	void Particle::move() {
		for (size_t j = 0; j < variable().size(); j++) {
			variable()[j] += m_vel[j];
		}
	}

	void Particle::updatePBest(Environment *env) {
		if(this->dominate(m_pbest,env)) {
			m_pbest = *this;
			m_improved = true;
		}
		else {
			m_improved = false;
		}
	}

	void Particle::resizeVar(size_t n) {
		variable().resize(n);
		m_pbest.variable().resize(n);
		m_vel.resize(n);
		m_vel_max.resize(n);
	}
}

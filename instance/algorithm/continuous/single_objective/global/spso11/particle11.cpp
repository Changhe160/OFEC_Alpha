#include "particle11.h"
#include "../../../../../../core/problem/continuous/continuous.h"
#include "../../../../../../utility/linear_algebra/vector.h"
#include "../../../../../../core/environment/environment.h"

namespace ofec {
	void Particle11::initVelocity(Environment *env, Random *rnd) {
		for (size_t j = 0; j < variable().size(); j++) {
			auto &range = CAST_CONOP(env->problem())->range(j);
			m_vel[j] = rnd->uniform.nextNonStd(range.first - variable()[j], range.second - variable()[j]);
		}
	}

	void Particle11::clampVelocity(Environment *env, Random *rnd) {
		for (size_t j = 0; j < variable().size(); j++) {
			auto& range = CAST_CONOP(env->problem())->range(j);
			if (variable()[j] > range.second) {
				variable()[j] = range.second;
				m_vel[j] *= -0.5;
			}
			else if (variable()[j] < range.first) {
				variable()[j] = range.first;
				m_vel[j] *= -0.5;
			}
		}
	}

	void Particle11::nextVelocity(const Solution<>* lbest, Real w, Real c1, Real c2, Random *rnd) {
		std::vector<Real> G(variable().size());

		if (lbest != &m_pbest) {
			for (size_t j = 0; j < variable().size(); j++)
				G[j] = variable()[j] + c1 * (m_pbest.variable()[j] + lbest->variable()[j] - 2 * variable()[j]) / 3;
		}
		else {
			for (size_t j = 0; j < variable().size(); j++)
				G[j] = variable()[j] + c1 * (m_pbest.variable()[j] - variable()[j]) / 2;
		}

		Real dis = 0;
		for (size_t j = 0; j < variable().size(); j++)
			dis += (G[j] - variable()[j]) * (G[j] - variable()[j]);
		dis = sqrt(dis);

		Vector v(variable().size(), 0);
		v.randomizeInSphere(dis, &rnd->uniform);
		v += G;

		for (size_t j = 0; j < variable().size(); j++)
			m_vel[j] = w * m_vel[j] + v[j] - variable()[j];
	}
}
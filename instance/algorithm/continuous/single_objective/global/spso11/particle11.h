#ifndef OFEC_PARTICLE11_H
#define OFEC_PARTICLE11_H

#include "../../../../template/classic/particle_swarm_optimization/particle.h"

namespace ofec {
	class Particle11 final : public Particle {
	public:
		Particle11(size_t num_obj, size_t num_con, size_t size_var) : Particle(num_obj, num_con, size_var) {}
		//Particle11(const Solution<>& rhs) : Particle(rhs) {}
		void initVelocity(Environment *env, Random *rnd) override;
		void nextVelocity(const Solution<>* lbest, Real w, Real c1, Real c2, Random *rnd) override;
		void clampVelocity(Environment *env, Random *rnd) override;
	};
}

#endif // !OFEC_PARTICLE11_H

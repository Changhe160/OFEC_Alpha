/********* Begin Register Information **********
{
	"name": "GTOP-Cassini2",
	"identifier": "GTOP_Cassini2",
	"tags": [ "continuous", "single-objective" ]
}
*********** End Register Information **********/

#ifndef OFEC_REALWORLD_TRAJECTORY_CASSINI2_H
#define OFEC_REALWORLD_TRAJECTORY_CASSINI2_H

#include "../../../../core/problem/continuous/continuous.h"

namespace ofec {
	namespace trajectory {
		class Cassini2 : virtual public Continuous {
			OFEC_CONCRETE_INSTANCE(Cassini2)
		protected:
			void addInputParameters() {}
			virtual void initialize_(Environment* env) override;
			void evaluateObjective(Real *x, std::vector<Real> &obj)const override;
			void updateOptima(Environment* env) override;
		};
	}
	using GTOP_Cassini2 = trajectory::Cassini2;
}

#endif // !OFEC_REALWORLD_TRAJECTORY_CASSINI2_H

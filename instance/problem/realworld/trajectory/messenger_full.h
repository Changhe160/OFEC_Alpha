/********* Begin Register Information **********
{
	"name": "GTOP-MessengerFull",
	"identifier": "GTOP_MessengerFull",
	"tags": [ "continuous", "single-objective" ]
}
*********** End Register Information **********/

#ifndef OFEC_REALWORLD_TRAJECTORY_MESSENGER_FULL_H
#define OFEC_REALWORLD_TRAJECTORY_MESSENGER_FULL_H

#include "../../../../core/problem/continuous/continuous.h"

namespace ofec {
	namespace trajectory {
		class MessengerFull : virtual public Continuous {

			OFEC_CONCRETE_INSTANCE(MessengerFull)
		protected:
			void initialize_(Environment* env) override;
			void evaluateObjective(Real *x, std::vector<Real> &obj)const override;
			void addInputParameters() {}
		};
	}
	using GTOP_MessengerFull = trajectory::MessengerFull;
}

#endif // !OFEC_REALWORLD_TRAJECTORY_MESSENGER_FULL_H

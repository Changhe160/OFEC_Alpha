#ifndef OFEC_DATUM_SL_SWARM_H
#define OFEC_DATUM_SL_SWARM_H

#include "../../../instance/algorithm/continuous/single_objective/global/slpso/sl_swarm.h"
#include "../../../core/datum_base.h"

namespace ofec {
	struct DatumSLSwarm : public DatumBase {
		const SwarmSL *value;
	};
	extern thread_local DatumSLSwarm g_sl_swarm;
}

#endif // !OFEC_DATUM_SL_SWARM_H

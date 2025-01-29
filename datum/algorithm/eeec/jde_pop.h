#ifndef OFEC_DATUM_JDE_POP_H
#define OFEC_DATUM_JDE_POP_H

#include "../../../instance/algorithm/continuous/single_objective/global/jde/jde_pop.h"
#include "../../../core/datum_base.h"

namespace ofec {
	struct DatumPopJDE : public DatumBase {
		const PopJDE *value;
	};
	extern thread_local DatumPopJDE g_jde_pop;
}

#endif // !OFEC_DATUM_JDE_POP_H

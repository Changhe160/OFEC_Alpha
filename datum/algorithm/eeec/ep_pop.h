#ifndef OFEC_DATUM_EP_POP_H
#define OFEC_DATUM_EP_POP_H

#include "../../../instance/algorithm/template/classic/evolutionary_programming/population.h"
#include "../../../core/datum_base.h"

namespace ofec {
	struct DatumPopEP : public DatumBase {
		const PopEP<> *value;
	};
	extern thread_local DatumPopEP g_ep_pop;
}

#endif // !OFEC_DATUM_EP_POP_H

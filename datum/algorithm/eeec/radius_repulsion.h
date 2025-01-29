#ifndef OFEC_DATUM_RADIUS_REPULSION_H
#define OFEC_DATUM_RADIUS_REPULSION_H

#include "../../../core/definition.h"
#include "../../../core/datum_base.h"

namespace ofec {
	struct DatumRadiusRepulsion : public DatumBase {
		Real value;
	};
	extern thread_local DatumRadiusRepulsion g_radius_repulsion;
}

#endif // !OFEC_DATUM_RADIUS_REPULSION_H

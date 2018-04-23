#ifndef USER_PREF_H
#define USER_PREF_H

#include "include_algorithm.h"
#include "include_problem.h"
#include "../core/measure/measure.h"

namespace OFEC {

	void register_parameter();
	void register_problem();
	void register_algorithm();
	int before_run();
	void after_run();
}

#endif // !USER_PREF_H



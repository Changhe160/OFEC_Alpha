#ifndef INCLUDE_H
#define INCLUDE_H

#include "include_algorithm.h"
#include "include_problem.h"
#include "../core/measure/measure.h"

namespace OFEC {

	void register_paramter();
	void register_class();
	void set_global_parameters(int argn, char *argv[]);
	void run();
	int go(std::vector<int> runIds);
}

#endif // !INCLUDE_H

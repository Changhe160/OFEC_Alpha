#ifndef INCLUDE_H
#define INCLUDE_H

#include "../instance/problem/continuous/multi_objective/ZDT/ZDT1.h"
#include "../instance/problem/continuous/multi_objective/ZDT/ZDT2.h"
#include "../instance/problem/continuous/multi_objective/ZDT/ZDT3.h"
#include "../instance/problem/continuous/multi_objective/ZDT/ZDT4.h"
#include "../instance/problem/continuous/multi_objective/ZDT/ZDT6.h"
#include "../instance/problem/continuous/multi_objective/DTLZ/DTLZ1.h"
#include "../instance/problem/continuous/multi_objective/DTLZ/DTLZ2.h"
#include "../instance/problem/continuous/multi_objective/DTLZ/DTLZ3.h"
#include "../instance/problem/continuous/multi_objective/DTLZ/DTLZ4.h"
#include "../instance/problem/continuous/multi_objective/MOEA_F/MOEA_F1.h"
#include "../instance/problem/continuous/multi_objective/MOEA_F/MOEA_F2.h"
#include "../instance/problem/continuous/multi_objective/MOEA_F/MOEA_F3.h"
#include "../instance/problem/continuous/multi_objective/MOEA_F/MOEA_F4.h"
#include "../instance/problem/continuous/multi_objective/MOEA_F/MOEA_F5.h"
#include "../instance/problem/continuous/multi_objective/MOEA_F/MOEA_F6.h"
#include "../instance/problem/continuous/multi_objective/MOEA_F/MOEA_F7.h"
#include "../instance/problem/continuous/multi_objective/MOEA_F/MOEA_F8.h"
#include "../instance/problem/continuous/multi_objective/MOEA_F/MOEA_F9.h"

#include "../instance/algorithm/MOEA/NSGAII/CS_NSGAII.h"
#include "../instance/algorithm/MOEA/NSGAII/DG_NSGAII.h"
#include "../instance/algorithm/MOEA/NSGAII/T_ENS_NSGAII.h"


#include "../core/measure/measure.h"

namespace OFEC {

	void register_paramter();
	void register_class();
	void set_global_parameters(int argn, char *argv[]);
	void run();
	int go(std::vector<int> runIds);
}

#endif // !INCLUDE_H


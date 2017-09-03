#include "problem.h"

namespace OFEC {
	
#ifdef OFEC_CONSOLE
	thread_local std::map<int, pair<unique_ptr<base>, unique_ptr<base>>> problem::ms_minmax_objective; // the best and worst so far solutions of each objective 
#endif
#ifdef OFEC_DEMON
	std::map<int, pair<unique_ptr<base>, unique_ptr<base>>> problem::ms_minmax_objective; // the best and worst so far solutions of each objective 
#endif	




}
#ifndef NBN_MODIFY_EQUAL_H
#define NBN_MODIFY_EQUAL_H


#include "../../../core/problem/solution.h"
#include "../../../core/problem/problem.h"
#include "../../../core/environment/environment.h"
#include "../../../core/random/newran.h"






namespace ofec {
	namespace nbn {
		
		struct NBNinfo;

		void modifyEqualSolutions(
			NBNinfo& info,
			ofec::Environment* env,
			ofec::Random* rnd);

		// yes
	}
}




#endif
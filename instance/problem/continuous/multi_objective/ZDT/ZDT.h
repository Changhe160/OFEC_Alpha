#ifndef ZDT_H
#define ZDT_H

#include "../../../../../core/problem/continuous/continuous.h"

namespace OFEC {
	class ZDT :public continuous {
	protected:
		ZDT(const std::string &name, size_t size_var);
		void generateAdLoadPF();
	};
}



#endif //ZDT_H
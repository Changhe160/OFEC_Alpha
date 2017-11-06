#ifndef ZDT_H
#define ZDT_H

#include "../../../../../core/problem/continuous/continuous.h"

namespace OFEC {
	class ZDT :public continuous
	{
	public:
		ZDT(param_map& v);
		~ZDT() {};
	protected:
		virtual void evaluate__(double *x, std::vector<double>& obj) {}
		void generateAdLoadPF();
	};
}



#endif //ZDT_H
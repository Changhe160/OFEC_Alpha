#ifndef ZDT2_H
#define ZDT2_H


#include "ZDT.h"

namespace OFEC {
	class ZDT2 final : public ZDT
	{
	public:
		ZDT2(param_map &v);
		~ZDT2() {};
	protected:
		void evaluate__(double *x, std::vector<double>& obj);
	};
}

#endif //ZDT2_H
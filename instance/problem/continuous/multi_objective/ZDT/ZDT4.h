#ifndef ZDT4_H
#define ZDT4_H


#include "ZDT.h"

namespace OFEC {
	class ZDT4 final : public ZDT
	{
	public:
		ZDT4(param_map &v);
		~ZDT4() {};
	protected:
		void evaluate__(double *x, std::vector<double>& obj);
	};
}

#endif //ZDT4_H
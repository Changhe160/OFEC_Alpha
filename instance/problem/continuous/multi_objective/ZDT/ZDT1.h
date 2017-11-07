#ifndef ZDT1_H
#define ZDT1_H


#include "ZDT.h"

namespace OFEC {
	class ZDT1 final : public ZDT {
	public:
		ZDT1(param_map &v);
		ZDT1(const std::string &name, size_t size_var);
		~ZDT1() {};
	private:
		void evaluate__(double *x, std::vector<double>& obj);
	};
}

#endif //ZDT1_H
#ifndef ZDT6_H
#define ZDT6_H


#include "ZDT.h"

namespace OFEC {
	class ZDT6 final : public ZDT {
	public:
		ZDT6(param_map &v);
		ZDT6(const std::string &name, size_t size_var);
		~ZDT6() {};
	private:
		void evaluate__(double *x, std::vector<double>& obj);
	};
}

#endif //ZDT6_H
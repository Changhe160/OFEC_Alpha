#ifndef ZDT3_H
#define ZDT3_H


#include "ZDT.h"

namespace OFEC {
	class ZDT3 final : public ZDT {
	public:
		ZDT3(param_map &v);
		ZDT3(const std::string &name, size_t size_var);
		~ZDT3() {};
	private:
		void evaluate__(double *x, std::vector<double>& obj);
	};
}

#endif //ZDT3_H
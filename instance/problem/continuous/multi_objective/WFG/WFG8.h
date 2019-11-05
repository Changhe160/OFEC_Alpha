#ifndef WFG8_H
#define WFG8_H

#include "WFG.h"

namespace OFEC {
	class WFG8 final : public WFG {
	public:
		WFG8(param_map &v);
		WFG8(const std::string &name, size_t size_var, size_t size_obj);
		~WFG8() {}
		void initialize();
		std::vector<real> t1(const std::vector<real> &y);
		std::vector<real> t2(const std::vector<real> &y);
		std::vector<real> t3(const std::vector<real> &y);
		std::vector<real> shape(const std::vector<real> &y);
	};
}


#endif

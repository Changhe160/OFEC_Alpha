#ifndef WFG6_H
#define WFG6_H

#include "WFG.h"

namespace OFEC {
	class WFG6 final : public WFG {
	public:
		WFG6(param_map &v);
		WFG6(const std::string &name, size_t size_var, size_t size_obj);
		~WFG6() {}
		void initialize();
		std::vector<real> t1(const std::vector<real> &y);
		std::vector<real> t2(const std::vector<real> &y);
		std::vector<real> shape(const std::vector<real> &y);
	};
}


#endif

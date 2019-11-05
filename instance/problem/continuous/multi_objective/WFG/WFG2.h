#ifndef WFG2_H
#define WFG2_H

#include "WFG.h"

namespace OFEC {
	class WFG2 final : public WFG {
	public:
		WFG2(param_map &v);
		WFG2(const std::string &name, size_t size_var, size_t size_obj);
		~WFG2() {}
		void initialize();
		std::vector<real> t1(const std::vector<real> &y);
		std::vector<real> t2(const std::vector<real> &y);
		std::vector<real> t3(const std::vector<real> &y);
		std::vector<real> shape(const std::vector<real> &y);
	};
}


#endif

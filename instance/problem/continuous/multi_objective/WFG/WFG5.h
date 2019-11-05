#ifndef WFG5_H
#define WFG5_H

#include "WFG.h"

namespace OFEC {
	class WFG5 final : public WFG {
	public:
		WFG5(param_map &v);
		WFG5(const std::string &name, size_t size_var, size_t size_obj);
		~WFG5() {}
		void initialize();
		std::vector<real> t1(const std::vector<real> &y);
		std::vector<real> t2(const std::vector<real> &y);
		std::vector<real> shape(const std::vector<real> &y);
	};
}


#endif

#ifndef WFG4_H
#define WFG4_H

#include "WFG.h"

namespace OFEC {
	class WFG4 final : public WFG {
	public:
		WFG4(param_map &v);
		WFG4(const std::string &name, size_t size_var, size_t size_obj);
		~WFG4() {}
		void initialize();
		std::vector<real> t1(const std::vector<real> &y);
		std::vector<real> t2(const std::vector<real> &y);
		std::vector<real> shape(const std::vector<real> &y);
	};
}


#endif

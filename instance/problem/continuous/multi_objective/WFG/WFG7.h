#ifndef WFG7_H
#define WFG7_H

#include "WFG.h"

namespace OFEC {
	class WFG7 final : public WFG {
	public:
		WFG7(param_map &v);
		WFG7(const std::string &name, size_t size_var, size_t size_obj);
		~WFG7() {}
		void initialize();
		std::vector<real> t1(const std::vector<real> &y);
		std::vector<real> t2(const std::vector<real> &y);
		std::vector<real> t3(const std::vector<real> &y);
		std::vector<real> shape(const std::vector<real> &y);
	};
}


#endif

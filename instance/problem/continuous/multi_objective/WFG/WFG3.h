#ifndef WFG3_H
#define WFG3_H

#include "WFG.h"

namespace OFEC {
	class WFG3 final : public WFG {
	public:
		WFG3(param_map &v);
		WFG3(const std::string &name, size_t size_var, size_t size_obj);
		~WFG3() {}
		void initialize();
		std::vector<real> t1(const std::vector<real> &y);
		std::vector<real> t2(const std::vector<real> &y);
		std::vector<real> t3(const std::vector<real> &y);
		std::vector<real> shape(const std::vector<real> &y);
	};
}


#endif

#ifndef WFG9_H
#define WFG9_H

#include "WFG.h"

namespace OFEC {
	class WFG9 final : public WFG {
	public:
		WFG9(param_map &v);
		WFG9(const std::string &name, size_t size_var, size_t size_obj);
		~WFG9() {}
		void initialize();
		std::vector<real> t1(const std::vector<real> &y);
		std::vector<real> t2(const std::vector<real> &y);
		std::vector<real> t3(const std::vector<real> &y);
		std::vector<real> shape(const std::vector<real> &y);
	};
}


#endif

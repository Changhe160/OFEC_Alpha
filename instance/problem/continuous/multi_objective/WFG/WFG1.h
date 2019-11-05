#ifndef WFG1_H
#define WFG1_H

#include "WFG.h"

namespace OFEC {
	class WFG1 final : public WFG {
	public:
		WFG1(param_map &v);
		WFG1(const std::string &name, size_t size_var, size_t size_obj);
		~WFG1() {}
		void initialize();
		std::vector<real> t1(const std::vector<real> &y);
		std::vector<real> t2(const std::vector<real> &y);
		std::vector<real> t3(const std::vector<real> &y);
		std::vector<real> t4(const std::vector<real> &y);
		std::vector<real> shape(const std::vector<real> &y);
	};
}


#endif
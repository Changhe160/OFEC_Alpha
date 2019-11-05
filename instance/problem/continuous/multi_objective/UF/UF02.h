#ifndef UF2_H
#define UF2_H


#include "UF.h"

namespace OFEC {
	class UF02 final : public UF {
	public:
		UF02(param_map &v);
		UF02(const std::string &name, size_t size_var);
		~UF02() {}
		void initialize();
	private:
		void evaluate_objective(real *x, std::vector<real> &obj);
	};
}

#endif

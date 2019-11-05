#ifndef UF3_H
#define UF3_H


#include "UF.h"

namespace OFEC {
	class UF03 final : public UF {
	public:
		UF03(param_map &v);
		UF03(const std::string &name, size_t size_var);
		~UF03() {}
	private:
		void evaluate_objective(real *x, std::vector<real> &obj);
	};
}

#endif


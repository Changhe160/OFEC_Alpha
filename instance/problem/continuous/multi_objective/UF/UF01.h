#ifndef UF1_H
#define UF1_H


#include "UF.h"

namespace OFEC {
	class UF01 final : public UF {
	public:
		UF01(param_map &v);
		UF01(const std::string &name, size_t size_var);
		~UF01() {}
		void initialize();
	private:
		void evaluate_objective(real *x, std::vector<real> &obj);
	};
}

#endif

#ifndef UF7_H
#define UF7_H


#include "UF.h"

namespace OFEC {
	class UF07 final : public UF {
	public:
		UF07(param_map &v);
		UF07(const std::string &name, size_t size_var);
		~UF07() {}
		void initialize();
	private:
		void evaluate_objective(real *x, std::vector<real> &obj);
	};
}

#endif

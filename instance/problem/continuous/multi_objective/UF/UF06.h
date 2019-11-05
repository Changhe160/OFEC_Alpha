#ifndef UF6_H
#define UF6_H


#include "UF.h"

namespace OFEC {
	class UF06 final : public UF {
	public:
		UF06(param_map &v);
		UF06(const std::string &name, size_t size_var);
		~UF06() {}
		void initialize();
	private:
		void evaluate_objective(real *x, std::vector<real> &obj);
	};
}

#endif


#ifndef UF10_H
#define UF10_H


#include "UF.h"

namespace OFEC {
	class UF10 final : public UF {
	public:
		UF10(param_map &v);
		UF10(const std::string &name, size_t size_var);
		~UF10() {}
		void initialize();
	private:
		void evaluate_objective(real *x, std::vector<real> &obj);
	};
}

#endif


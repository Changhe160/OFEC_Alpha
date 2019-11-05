#ifndef UF5_H
#define UF5_H


#include "UF.h"

namespace OFEC {
	class UF05 final : public UF {
	public:
		UF05(param_map &v);
		UF05(const std::string &name, size_t size_var);
		~UF05() {}
		void initialize();
	private:
		void evaluate_objective(real *x, std::vector<real> &obj);
	};
}

#endif


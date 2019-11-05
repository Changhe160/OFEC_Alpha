#ifndef UF9_H
#define UF9_H


#include "UF.h"

namespace OFEC {
	class UF09 final : public UF {
	public:
		UF09(param_map &v);
		UF09(const std::string &name, size_t size_var);
		~UF09() {}
		void initialize();
	private:
		void evaluate_objective(real *x, std::vector<real> &obj);
	};
}

#endif

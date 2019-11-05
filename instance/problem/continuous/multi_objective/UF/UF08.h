#ifndef UF8_H
#define UF8_H


#include "UF.h"

namespace OFEC {
	class UF08 final : public UF {
	public:
		UF08(param_map &v);
		UF08(const std::string &name, size_t size_var);
		~UF08() {}
		void initialize();
	private:
		void evaluate_objective(real *x, std::vector<real> &obj);
	};
}

#endif

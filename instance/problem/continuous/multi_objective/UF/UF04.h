#ifndef UF4_H
#define UF4_H


#include "UF.h"

namespace OFEC {
	class UF04 final : public UF {
	public:
		UF04(param_map &v);
		UF04(const std::string &name, size_t size_var);
		~UF04() {}
		void initialize();
	private:
		void evaluate_objective(real *x, std::vector<real> &obj);
	};
}

#endif


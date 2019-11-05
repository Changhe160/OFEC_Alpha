#ifndef GLT6_H
#define GLT6_H


#include "GLT.h"

namespace OFEC {
	class GLT6 final : public GLT {
	public:
		GLT6(param_map &v);
		GLT6(const std::string &name, size_t size_var);
		~GLT6() {}
	private:
		void evaluate_objective(real *x, std::vector<real> &obj);
	};
}

#endif

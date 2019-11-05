#ifndef GLT5_H
#define GLT5_H


#include "GLT.h"

namespace OFEC {
	class GLT5 final : public GLT {
	public:
		GLT5(param_map &v);
		GLT5(const std::string &name, size_t size_var);
		~GLT5() {}
		void initialize();
	private:
		void evaluate_objective(real *x, std::vector<real> &obj);
	};
}

#endif

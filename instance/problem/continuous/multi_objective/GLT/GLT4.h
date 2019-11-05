#ifndef GLT4_H
#define GLT4_H


#include "GLT.h"

namespace OFEC {
	class GLT4 final : public GLT {
	public:
		GLT4(param_map &v);
		GLT4(const std::string &name, size_t size_var);
		~GLT4() {}
		void initialize();
	private:
		void evaluate_objective(real *x, std::vector<real> &obj);
	};
}

#endif

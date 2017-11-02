
#ifndef OFEC_UNEVEN_DE_MAXIMA_H
#define OFEC_UNEVEN_DE_MAXIMA_H
#include "../../../../../core/problem/continuous/function.h"

namespace OFEC {
	
	class uneven_de_maxima : public function
	{ //Beasley's F4 function
	public:
		uneven_de_maxima(param_map &v);
		uneven_de_maxima(const std::string &name, size_t size_var, size_t size_obj);
	protected:
		void initialize();
		void evaluate__(real *x, std::vector<real>& obj);
	private:
	};
	
}
#endif // !OFEC_UNEVEN_DE_MAXIMA_H
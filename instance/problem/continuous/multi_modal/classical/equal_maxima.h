
#ifndef OFEC_EQUAL_MAXIMA_H
#define OFEC_EQUAL_MAXIMA_H
#include "../../../../../core/problem/continuous/function.h"
namespace OFEC {
	
	class equal_maxima : public function
	{
	public:
		equal_maxima(param_map &v);
		equal_maxima(const std::string &name, size_t size_var, size_t size_obj);
	protected:
		void initialize();
		void evaluate__(real *x, std::vector<real>& obj);
	private:
	};
	
}

#endif // !OFEC_EQUAL_MAXIMA_H
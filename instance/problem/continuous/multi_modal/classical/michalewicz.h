#ifndef OFEC_MICHALEWICZ_H
#define OFEC_MICHALEWICZ_H

#include "../../../../../core/problem/continuous/function.h"

namespace OFEC {
	
	class michalewicz : public function
	{
	public:
		michalewicz(param_map &v);
		michalewicz(const std::string &name, size_t size_var, size_t size_obj);
	protected:
		void initialize();
		void evaluate__(real *x, std::vector<real>& obj);
		int m_m;
	private:
	};
	
}
#endif // !OFEC_MICHALEWICZ_H
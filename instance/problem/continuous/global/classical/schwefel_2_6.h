#ifndef OFEC_SCHWEFEL_2_6_H
#define OFEC_SCHWEFEL_2_6_H

#include "../../../../../core/problem/continuous/function.h"
namespace OFEC {
	
	class schwefel_2_6 : public function
	{
	public:
		schwefel_2_6(param_map &v);
		schwefel_2_6(const std::string &name, size_t size_var, size_t size_obj);
		void initialize();
	protected:
		void load_data();
		void evaluate_objective(real *x, std::vector<real>& obj) override;

		std::vector<real> m_b;
		std::vector<std::vector<int>> m_a;
	};
	
}
#endif // !OFEC_SCHWEFEL_2_6_H

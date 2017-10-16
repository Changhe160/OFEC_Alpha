#ifndef OFEC_F12_SCHWEFEL_2_13_H
#define OFEC_F12_SCHWEFEL_2_13_H

#include "../../../../../../core/problem/continuous/function.h"

namespace OFEC {

	class F12_schwefel_2_13 : public function
	{
	public:
		F12_schwefel_2_13(param_map &v);
		F12_schwefel_2_13(const std::string &name, size_t size_var, size_t size_obj);
		virtual ~F12_schwefel_2_13();
	protected:
		void initialize();
		void load_data(const string & path);
		void evaluate__(real *x, std::vector<real>& obj);
	private:
		std::vector<std::vector<int>> m_a;
		std::vector<std::vector<int>> m_b;
		std::vector<real> m_alpha;
	};
}
#endif // !OFEC_F12_SCHWEFEL_2_13_H

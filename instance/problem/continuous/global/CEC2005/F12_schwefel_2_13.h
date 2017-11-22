#ifndef OFEC_F12_SCHWEFEL_2_13_H
#define OFEC_F12_SCHWEFEL_2_13_H

#include "../../../../../core/problem/continuous/function.h"
#include <cstring>

namespace OFEC {
	namespace CEC2005 {
		class F12_schwefel_2_13 final: public function
		{
		public:
			F12_schwefel_2_13(param_map &v);
			F12_schwefel_2_13(const std::string &name, size_t size_var, size_t size_obj);
		protected:
			void initialize();
			void load_data(const std::string & path);
			void evaluate__(real *x, std::vector<real>& obj);
		private:
			std::vector<std::vector<int>> m_a;
			std::vector<std::vector<int>> m_b;
			std::vector<real> m_alpha;
		};
	}
	using CEC2005_GOP_F12 = CEC2005::F12_schwefel_2_13;
}
#endif // !OFEC_F12_SCHWEFEL_2_13_H

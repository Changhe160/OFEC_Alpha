
#include "ShiftedRosenbrock_F12.h"
namespace OFEC {
	namespace CEC2013 {
		ShiftedRosenbrock_F12::ShiftedRosenbrock_F12(param_map &v) : problem((v[param_proName]), (v[param_numDim]), 1), \
			function_CEC2013((v[param_proName]), (v[param_numDim]), 1) \
		{
			set_range(-100, 100);
			set_init_range(-100, 100);
			initialize();
		}

		ShiftedRosenbrock_F12::ShiftedRosenbrock_F12(const std::string &name, size_t size_var, size_t size_obj) : problem(name, size_var, size_obj), \
			function_CEC2013(name, size_var, size_obj) \
		{
			set_range(-100, 100);
			set_init_range(-100, 100);
			initialize();
		}

		ShiftedRosenbrock_F12::~ShiftedRosenbrock_F12() {
			delete[] mp_Ovector;

			delete[] mp_anotherz;

		}

		void ShiftedRosenbrock_F12::initialize() {
			ID = 12;
			mp_anotherz = new real[m_variable_size];

			// Ovector = createShiftVector(dimension,minX,maxX);
			mp_Ovector = readOvector();

			//set_original_global_opt();
			std::vector<real> data(m_variable_size,1);
			for (size_t i = 0; i < m_variable_size; ++i)
				data[i] += mp_Ovector[i];
			set_global_opt(data.data());
		}

		void ShiftedRosenbrock_F12::evaluate__(real *x, std::vector<real>& obj) {
			int i;
			double result = 0.0;

			for (i = 0; i < m_variable_size; i++)
			{
				mp_anotherz[i] = x[i] - mp_Ovector[i];
			}

			result = rosenbrock(mp_anotherz, m_variable_size);

			obj[0] = result;
		}
	}
}
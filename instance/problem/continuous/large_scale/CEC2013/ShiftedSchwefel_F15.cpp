
#include "ShiftedSchwefel_F15.h"
namespace OFEC {
	namespace CEC2013 {
		ShiftedSchwefel_F15::ShiftedSchwefel_F15(param_map &v) : problem((v[param_proName]), (v[param_numDim]), 1), \
			function_CEC2013((v[param_proName]), (v[param_numDim]), 1) \
		{
			set_range(-100, 100);
			set_init_range(-100, 100);
			initialize();
		}

		ShiftedSchwefel_F15::ShiftedSchwefel_F15(const std::string &name, size_t size_var, size_t size_obj) : problem(name, size_var, size_obj), \
			function_CEC2013(name, size_var, size_obj) \
		{
			set_range(-100, 100);
			set_init_range(-100, 100);
			initialize();
		}

		ShiftedSchwefel_F15::~ShiftedSchwefel_F15() {
			delete[] mp_Ovector;

			delete[] mp_anotherz;

		}

		void ShiftedSchwefel_F15::initialize() {
			ID = 15;
			mp_anotherz = new real[m_variable_size];

			// Ovector = createShiftVector(dimension,minX,maxX);
			mp_Ovector = readOvector();

			//set_original_global_opt();	
			set_global_opt(mp_Ovector);

		}

		void ShiftedSchwefel_F15::evaluate__(real *x, std::vector<real>& obj) {
			int i;
			double result = 0.0;



			for (i = 0; i < m_variable_size; i++)
			{
				mp_anotherz[i] = x[i] - mp_Ovector[i];
			}

			result = schwefel(mp_anotherz, m_variable_size);

			obj[0] = result;
		}
	}
}
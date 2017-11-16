
#include "ConflictingOS_ShiftedSchwefel_F14.h"
namespace OFEC {
	namespace CEC2013 {
		ConflictingOS_ShiftedSchwefel_F14::ConflictingOS_ShiftedSchwefel_F14(param_map &v) : problem((v[param_proName]), (v[param_numDim]), 1), \
			function_CEC2013((v[param_proName]), (v[param_numDim]), 1) \
		{
			set_range(-100, 100);
			set_init_range(-100, 100);
			initialize();
		}

		ConflictingOS_ShiftedSchwefel_F14::ConflictingOS_ShiftedSchwefel_F14(const std::string &name, size_t size_var, size_t size_obj) : problem(name, size_var, size_obj), \
			function_CEC2013(name, size_var, size_obj) \
		{
			set_range(-100, 100);
			set_init_range(-100, 100);
			initialize();
		}

		ConflictingOS_ShiftedSchwefel_F14::~ConflictingOS_ShiftedSchwefel_F14() {
			for (int i = 0; i < m_nonSeparableGroupNumber; i++)
			{
				free(mpp_OvectorVec[i]);
			}
			free(mpp_OvectorVec);

			delete[] mp_Pvector;

			for (int i = 0; i < 25; ++i)
				delete[] mpp_r25[i];
			
			for (int i = 0; i < 50; ++i)
				delete[] mpp_r50[i];

			for (int i = 0; i < 100; ++i)
				delete[] mpp_r100[i];

			delete[] mpp_r25;
			delete[] mpp_r50;
			delete[] mpp_r100;
			delete[] mp_s;
			delete[] mp_w;

		}

		void ConflictingOS_ShiftedSchwefel_F14::initialize() {
			ID = 14;
			m_nonSeparableGroupNumber = 20;
			m_overlap = 5;
			m_variable_size = 905;

			//Ovector = createShiftVector(dimension,minX,maxX);
			mp_s = readS(m_nonSeparableGroupNumber);
			mpp_OvectorVec = readOvectorVec();
			mp_Pvector = readPermVector();

			mpp_r25 = readR(25);
			mpp_r50 = readR(50);
			mpp_r100 = readR(100);
			mp_w = readW(m_nonSeparableGroupNumber);

			//set_original_global_opt();
			std::vector<real> temp_obj(m_objective_size, 0);
			m_optima.append(temp_obj);
		}

		void ConflictingOS_ShiftedSchwefel_F14::evaluate__(real *x, std::vector<real>& obj) {
			size_t i;
			double result = 0.0;


			// s_size non-separable part with rotation
			size_t c = 0;
			for (i = 0; i < m_nonSeparableGroupNumber; i++)
			{
				mp_anotherz1 = rotate_vector_conflict(i, c, x);
				result += mp_w[i] * schwefel(mp_anotherz1, mp_s[i]);
				delete[] mp_anotherz1;
			}

			obj[0] = result;
		}
	}
}

#include "ConformingOS_ShiftedSchwefel_F13.h"
namespace OFEC {
	namespace CEC2013 {
		ConformingOS_ShiftedSchwefel_F13::ConformingOS_ShiftedSchwefel_F13(param_map &v) :
			ConformingOS_ShiftedSchwefel_F13((v.at("proName")), (v.at("numDim")), 1) \
		{
			
		}

		ConformingOS_ShiftedSchwefel_F13::ConformingOS_ShiftedSchwefel_F13(const std::string &name, size_t size_var, size_t size_obj) : problem(name, size_var, size_obj), \
			function_CEC2013(name, size_var, size_obj) \
		{
			
		}

		ConformingOS_ShiftedSchwefel_F13::~ConformingOS_ShiftedSchwefel_F13() {
			delete[] mp_Ovector;
			delete[] mp_Pvector;
			delete[] mp_anotherz;

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

		void ConformingOS_ShiftedSchwefel_F13::initialize() {
			m_variable_monitor = true;
			set_range(-100, 100);
			set_init_range(-100, 100);
			ID = 13;
			m_nonSeparableGroupNumber = 20;
			m_overlap = 5;
			m_variable_size = 905;
			mp_anotherz = new real[m_variable_size];

			// Ovector = createShiftVector(dimension,minX,maxX);
			mp_Ovector = readOvector();
			mp_Pvector = readPermVector();
			mpp_r25 = readR(25);
			mpp_r50 = readR(50);
			mpp_r100 = readR(100);
			mp_s = readS(m_nonSeparableGroupNumber);
			mp_w = readW(m_nonSeparableGroupNumber);

			//set_original_global_opt();	
			set_global_opt(mp_Ovector);
			m_initialized = true;
		}

		void ConformingOS_ShiftedSchwefel_F13::evaluate_objective(real *x, std::vector<real> &obj) {
			size_t i;
			real result = 0.0;

			for (i = 0; i < m_variable_size; i++)
			{
				mp_anotherz[i] = x[i] - mp_Ovector[i];
			}

			// s_size non-separable part with rotation
			size_t c = 0;
			for (i = 0; i < m_nonSeparableGroupNumber; i++)
			{
				mp_anotherz1 = rotate_vector_conform(i, c);
				result += mp_w[i] * schwefel(mp_anotherz1, mp_s[i]);
				delete[] mp_anotherz1;
			}

			obj[0] = result;
		}
	}
}
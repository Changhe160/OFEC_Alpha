
#include "N7S1_SR_Ackley_F6.h"
namespace OFEC {
	namespace CEC2013 {
		N7S1_SR_Ackley_F6::N7S1_SR_Ackley_F6(param_map &v) : problem((v[param_proName]), (v[param_numDim]), 1), \
			function_CEC2013((v[param_proName]), (v[param_numDim]), 1) \
		{
			set_range(-32, 32);
			set_init_range(-32, 32);
			initialize();
		}

		N7S1_SR_Ackley_F6::N7S1_SR_Ackley_F6(const std::string &name, size_t size_var, size_t size_obj) : problem(name, size_var, size_obj), \
			function_CEC2013(name, size_var, size_obj) \
		{
			set_range(-32, 32);
			set_init_range(-32, 32);
			initialize();
		}

		N7S1_SR_Ackley_F6::~N7S1_SR_Ackley_F6() {
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

		void N7S1_SR_Ackley_F6::initialize() {
			ID = 6;
			m_nonSeparableGroupNumber = 7;

			mp_anotherz = new real[m_variable_size];
			// Ovector = createShiftVector(dimension,minX,maxX);
			mp_Ovector = readOvector();
			mp_Pvector = readPermVector();
			mpp_r25 = readR(25);
			mpp_r50 = readR(50);
			mpp_r100 = readR(100);
			mp_s = readS(m_nonSeparableGroupNumber);
			mp_w = readW(m_nonSeparableGroupNumber);

			set_original_global_opt();
			set_global_opt(mp_Ovector);

		}

		void N7S1_SR_Ackley_F6::evaluate__(real *x, std::vector<real>& obj) {
			size_t    i;
			double result = 0.0;
			
			for (i = 0; i < m_variable_size; i++) {
				mp_anotherz[i] = x[i] - mp_Ovector[i];
			}

			// s_size non-separable part with rotation
			size_t c = 0;
			for (i = 0; i < m_nonSeparableGroupNumber; i++)
			{
				mp_anotherz1 = rotate_vector(i, c);
				result += mp_w[i] * ackley(mp_anotherz1, mp_s[i]);
				delete[] mp_anotherz1;
				mp_anotherz1 = nullptr;
			}

			// one separable part without rotation
			double* z = new double[m_variable_size - c];
			for (i = c; i < m_variable_size; i++)
			{
				z[i - c] = mp_anotherz[mp_Pvector[i]];
			}
			result += ackley(z, m_variable_size - c);
			delete[]z;

			obj[0] = result;
		}
	}
}
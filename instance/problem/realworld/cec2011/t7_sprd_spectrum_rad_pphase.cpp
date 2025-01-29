#include "t7_sprd_spectrum_rad_pphase.h"

namespace ofec::cec2011 {
	void SprdSpectrumRadPphase::addInputParameters() {
	}

	void SprdSpectrumRadPphase::initialize_(Environment* env) {
		Continuous::initialize_(env);
		m_number_variables = 20;
		resizeVariable(m_number_variables);
		for (size_t i = 0; i < m_number_variables; ++i) {
			m_domain.setRange(0, OFEC_PI*2, i);
		}
		resizeObjective(1);
		m_optimize_mode[0] = OptimizeMode::kMinimize;
	}

	void SprdSpectrumRadPphase::evaluateObjective(Real* x, std::vector<Real>& obj)const {
        int d = m_number_variables;

        std::vector<double> hsum;
        int var = 2 * d - 1;
        for (int kk = 1; kk <= 2 * var; ++kk) {
            if (kk % 2 != 0) {
                int i = (kk + 1) / 2;
                double sumForThisIteration = 0;
                for (int j = i; j <= d; ++j) {
                    double summ = 0;
                    for (int i1 = std::abs(2 * i - j - 1) + 1; i1 <= j; ++i1) {
                        summ += x[i1 - 1];
                    }
                    sumForThisIteration += std::cos(summ);
                }
                hsum.push_back(sumForThisIteration);
            }
            else {
                int i = kk / 2;
                double sumForThisIteration = 0;
                for (int j = i + 1; j <= d; ++j) {
                    double summ = 0;
                    for (int i1 = std::abs(2 * i - j) + 1; i1 <= j; ++i1) {
                        summ += x[i1 - 1];
                    }
                    sumForThisIteration += std::cos(summ);
                }
                hsum.push_back(sumForThisIteration + 0.5);
            }
        }

        double f = hsum[0];
        for (double value : hsum) {
            f = std::max(f, value);
        }

        obj[0] = f;
	}
}
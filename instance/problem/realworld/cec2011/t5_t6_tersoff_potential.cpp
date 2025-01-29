#include "t5_t6_tersoff_potential.h"

namespace ofec::cec2011 {
	void TersoffPotential::addInputParameters() {
		m_input_parameters.add("number of variables", new RangedSizeT(m_number_variables, 3, 444, 30));
	}


	void TersoffPotential::setParameterT5() {
		m_R1 = 3.0;
		m_R2 = 0.2;
		m_A = 3.2647e+3;
		m_B = 9.5373e+1;
		m_lamda1 = 3.2394;
		m_lamda2 = 1.3258;
		m_lamda3 = 1.3258;
		m_c = 4.8381;
		m_d = 2.0417;
		m_n1 = 22.956;
		m_gamma = 0.33675;
		m_h = 0;
	}
	void TersoffPotential::setParameterT6() {


		m_R1 = 2.85;
		m_R2 = 0.15;
		m_A = 1.8308e+3;
		m_B = 4.7118e+2;
		m_lamda1 = 2.4799;
		m_lamda2 = 1.7322;
		m_lamda3 = 1.7322;
		m_c = 1.0039e+05;
		m_d = 1.6218e+01;
		m_n1 = 7.8734e-01;
		m_gamma = 1.0999e-06;
		m_h = -5.9826e-01;
	}

	void TersoffPotential::initialize_(Environment* env) {
		Continuous::initialize_(env);
		if (m_number_variables < 3 || m_number_variables % 3 != 0) {
			throw Exception("The number of varialbes must be a multiple of 3.");
		}
		resizeVariable(m_number_variables);
		//m_domain.setRange(0, 4, 0);
		//m_domain.setRange(0, 4, 1);
		//m_domain.setRange(0, OFEC_PI, 2);
		//for (size_t i = 3; i < m_number_variables; ++i) {
		//	m_domain.setRange(-4- 0.25* floor((i-4)/3.0), 4+ 0.25 * floor((i-4)/3.0), i);
		//}


		m_domain.setRange(-4.25, 4.25, 0);
		m_domain.setRange(0, 4, 1);
		m_domain.setRange(0, OFEC_PI, 2);
		for (size_t i = 3; i < m_number_variables; ++i) {
			m_domain.setRange(-4.25, 4.25, i);
		}
		resizeObjective(1);
		m_optimize_mode[0] = OptimizeMode::kMinimize;

		if (m_fun_id == 1) {
			setParameterT5();
		}
		else {
			setParameterT6();
		}

	}

	void TersoffPotential::evaluateObjective(Real *x, std::vector<Real> &obj)const {
		size_t NP = m_number_variables / 3 ;
		//std::vector<std::vector<double>> x(NP, std::vector<double>(3));
		//for (int i = 0; i < NP; ++i) {
		//	for (int j = 0; j < 3; ++j) {
		//		x[i][j] = x_[i * 3 + j];
		//	}
		//}



		//E.resize(NP, 0.0);
		//std::fill(E.begin(), E.end(), 0);

		//std::vector<std::vector<double>> r(NP, std::vector<double>(NP, 0.0));
		//std::vector<std::vector<double>> fcr(NP, std::vector<double>(NP, 0.0));
		//std::vector<std::vector<double>> VRr(NP, std::vector<double>(NP, 0.0));
		//std::vector<std::vector<double>> VAr(NP, std::vector<double>(NP, 0.0));
		//std::vector<std::vector<double>> jeta(NP, std::vector<double>(NP, 0.0));
		//std::vector<std::vector<double>> Bij(NP, std::vector<double>(NP, 0.0));

		//for (int i = 0; i < NP; ++i) {
		//	for (int j = 0; j < NP; ++j) {
		//		double sumSquaredDiff = 0;
		//		for (int k = 0; k < x[i].size(); ++k) {
		//			double diff = x[i][k] - x[j][k];
		//			sumSquaredDiff += diff * diff;
		//		}
		//		r[i][j] = std::sqrt(sumSquaredDiff);

		//		if (r[i][j] < (m_R1 - m_R2)) {
		//			fcr[i][j] = 1;
		//		}
		//		else if (r[i][j] > (m_R1 + m_R2)) {
		//			fcr[i][j] = 0;
		//		}
		//		else {
		//			fcr[i][j] = 0.5 - 0.5 * std::sin(OFEC_PI / 2 * (r[i][j] - m_R1) / m_R2);
		//		}

		//		VRr[i][j] = m_A * std::exp(-m_lamda1 * r[i][j]);
		//		VAr[i][j] = m_B * std::exp(-m_lamda2 * r[i][j]);
		//	}
		//}

		//for (int i = 0; i < NP; ++i) {
		//	for (int j = 0; j < NP; ++j) {
		//		if (i == j) continue;

		//		for (int k = 0; k < NP; ++k) {
		//			if (i == k || j == k) continue;

		//			double rd1 = 0;
		//			for (int l = 0; l < x[i].size(); ++l) {
		//				double diff = x[i][l] - x[k][l];
		//				rd1 += diff * diff;
		//			}
		//			rd1 = std::sqrt(rd1);

		//			double rd3 = 0;
		//			for (int l = 0; l < x[k].size(); ++l) {
		//				double diff = x[k][l] - x[j][l];
		//				rd3 += diff * diff;
		//			}
		//			rd3 = std::sqrt(rd3);

		//			double rd2 = r[i][j];

		//			double ctheta_ijk = (rd1 * rd1 + rd2 * rd2 - rd3 * rd3) / (2 * rd1 * rd2);

		//			double G_th_ijk = 1 + (m_c * m_c) / (m_d * m_d) - (m_c * m_c) / (m_d * m_d + (m_h - ctheta_ijk) * (m_h - ctheta_ijk));

		//			jeta[i][j] += fcr[i][k] * G_th_ijk * std::exp(m_lamda3 * m_lamda3 * (r[i][j] - r[i][k]) * (r[i][j] - r[i][k]) * (r[i][j] - r[i][k]));
		//		}

		//		double Bij_temp = (1 + (m_gamma * jeta[i][j]) * (m_gamma * jeta[i][j])) * (-0.5 / m_n1);
		//		Bij[i][j] = std::pow(Bij_temp, 1.0);

		//		E[i] += fcr[i][j] * (VRr[i][j] - Bij[i][j] * VAr[i][j]) / 2;
		//	}
		//}

		//double f = 0.0;
		//for (double value : E) {
		//	f += value;
		//}

		//obj[0] = f;


		std::vector<std::array<Real, 3>> pos(NP);
		pos[0] = { 0,0,0 };
		pos[1] = { x[0], 0, 0 };
		pos[2] = { x[1], x[2], 0 };
		for (size_t i = 3; i < NP; ++i) {
			pos[i] = { x[(i - 2) * 3], x[(i - 2) * 3 + 1] , x[(i - 2) * 3 + 2] };
		}
		std::vector<Real> E(NP, 0);
		std::vector<std::vector<Real>> r(NP, std::vector<Real>(NP, 0));
		std::vector<std::vector<Real>> fcr(NP, std::vector<Real>(NP));
		std::vector<std::vector<Real>> VRr(NP, std::vector<Real>(NP));
		std::vector<std::vector<Real>> VAr(NP, std::vector<Real>(NP));
		for (size_t i = 0; i < NP; ++i) {
			for (size_t j = 0; j < NP; ++j) {
				for (size_t k = 0; k < 3; ++k)
					r[i][j] += pow(pos[i][k] - pos[j][k], 2);
				r[i][j] = sqrt(r[i][j]);
				if (r[i][j] < m_R1 - m_R2)
					fcr[i][j] = 1;
				else if(r[i][j] > m_R1 + m_R2)
					fcr[i][j] = 0;
				else
					fcr[i][j] = 0.5 - 0.5 * sin(OFEC_PI / 2 * (r[i][j] - m_R1) / m_R2);
				VRr[i][j] = m_A * exp(-m_lamda1 * r[i][j]);
				VAr[i][j] = m_B * exp(-m_lamda2 * r[i][j]);
			}
		}
		for (size_t i = 0; i < NP; ++i) {
			for (size_t j = 0; j < NP; ++j) {
				if (i == j)
					continue;
				Real rd2 = 0;
				for (size_t l = 0; l < 3; ++l)
					rd2 += pow(pos[i][l] - pos[j][l], 2);
				rd2 = sqrt(rd2);
				std::vector<std::vector<Real>> jeta(NP, std::vector<Real>(NP, 0));
				for (size_t k = 0; k < NP; ++k) {
					if (i == k || j == k)
						continue;
					Real rd1 = 0;
					for (size_t l = 0; l < 3; ++l)
						rd1 += pow(pos[i][l] - pos[k][l], 2);
					rd1 = sqrt(rd1);
					Real rd3 = 0; 
					for (size_t l = 0; l < 3; ++l)
						rd3 += pow(pos[k][l] - pos[j][l], 2);
					rd3 = sqrt(rd3);
					Real ctheta_ijk = (pow(rd1, 2) + pow(rd2, 2) - pow(rd3, 3)) / (2 * rd1 * rd2);
					Real G_th_ijk = 1 + pow(m_c, 2) / pow(m_d, 2) - pow(m_c, 2) / (pow(m_d, 2) + pow(m_h - ctheta_ijk, 2));
					jeta[i][j] += fcr[i][k] * G_th_ijk * exp(pow(m_lamda3, 3) * pow((r[i][j] - r[i][k]) / 10, 3));
				}
				Real Bij = pow(1 + pow(m_gamma * jeta[i][j], m_n1), -0.5 / m_n1);
				E[i] += fcr[i][j] * (VRr[i][j] - Bij * VAr[i][j]) / 2;
			}
		}
		obj[0] = 0;
		for (Real val : E)
			obj[0] += val;

		if (std::isnan(obj[0])) {
			obj[0] = std::numeric_limits<Real>::max();
		}
	}
}
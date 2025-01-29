#include "t10_circular_antenna_array_design.h"
#include <algorithm>

namespace ofec::cec2011 {
	void CircularAntennaArrayDesign::initialize_(Environment* env) {
		Continuous::initialize_(env);
		resizeVariable(12);
		for (size_t i = 0; i < 6; ++i)
			m_domain.setRange(0.2, 1, i);
		for (size_t i = 6; i < 12; ++i)
			m_domain.setRange(-180, 180, i);
		resizeObjective(1);
		m_optimize_mode[0] = OptimizeMode::kMinimize;
		m_null = { 50,120 };
		m_phi_desired = 180;
		m_distance = 0.5;
	}

	Real CircularAntennaArrayDesign::array_factorcir(const std::vector<double>& x1, double phi) const {
		double phi_desired = m_phi_desired;
		double distance = m_distance;
		int dim = x1.size();
		const double pi = OFEC_PI;
		double y = 0;
		double y1 = 0;
		for (int i1 = 1; i1 <= dim / 2; ++i1) {
			double delphi = 2 * pi * (i1 - 1) / dim;
			double shi = std::cos(phi - delphi) - std::cos(phi_desired * (pi / 180) - delphi);
			shi *= dim * distance;
			y += x1[i1 - 1] * std::cos(shi + x1[i1 - 1 + dim / 2] * (pi / 180));
		}
		for (int i1 = dim / 2 + 1; i1 <= dim; ++i1) {
			double delphi = 2 * pi * (i1 - 1) / dim;
			double shi = std::cos(phi - delphi) - std::cos(phi_desired * (pi / 180) - delphi);
			shi *= dim * distance;
			y += x1[i1 - 1 - dim / 2] * std::cos(shi - x1[i1 - 1] * (pi / 180));
		}
		for (int i1 = 1; i1 <= dim / 2; ++i1) {
			double delphi = 2 * pi * (i1 - 1) / dim;
			double shi = std::cos(phi - delphi) - std::cos(phi_desired * (pi / 180) - delphi);
			shi *= dim * distance;
			y1 += x1[i1 - 1] * std::sin(shi + x1[i1 - 1 + dim / 2] * (pi / 180));
		}
		for (int i1 = dim / 2 + 1; i1 <= dim; ++i1) {
			double delphi = 2 * pi * (i1 - 1) / dim;
			double shi = std::cos(phi - delphi) - std::cos(phi_desired * (pi / 180) - delphi);
			shi *= dim * distance;
			y1 += x1[i1 - 1 - dim / 2] * std::sin(shi - x1[i1 - 1] * (pi / 180));
		}
		y = y * y + y1 * y1;
		return std::sqrt(y);
	}

	Real  CircularAntennaArrayDesign::trapezoidalcir(const std::vector<double>& x2, double upper, double lower, int N1) const {
		double phi_desired = m_phi_desired;
		double distance = m_distance;
		int dim = x2.size();
		const double pi = OFEC_PI;
		double h = (upper - lower) / N1;
		double x1 = lower;
		double y = std::abs(std::pow(array_factorcir(x2, lower), 2) * std::sin(lower - pi / 2));
		std::vector<double> yValues(N1 + 1);
		yValues[0] = y;
		for (int i = 1; i <= N1; ++i) {
			x1 += h;
			yValues[i] = std::abs(std::pow(array_factorcir(x2, x1), 2) * std::sin(x1 - pi / 2));
		}
		double s = 0;
		for (int i = 0; i <= N1; ++i) {
			if (i == 0 || i == N1) {
				s += yValues[i];
			}
			else {
				s += 2 * yValues[i];
			}
		}
		return (h / 2) * s;
	}

	void CircularAntennaArrayDesign::evaluateObjective(Real *tmp_x, std::vector<Real> &obj) const {
		std::vector<double> x1(m_number_variables);
		for (int idx(0); idx < m_number_variables; ++idx) {
			x1[idx] = round(tmp_x[idx] * 10000) / 10000;// For fixing the 4 digit precision
		}

		double y = 0;
		auto& null = m_null;
		int temp, num_null = null.size();
		int num1 = 300;

		auto pi = OFEC_PI;
		std::vector<double> phi(num1);
		for (int i = 0; i < num1; ++i) {
			phi[i] = (double)i * 360.0 / (num1-1);
		}
		std::vector<double> yax(num1);
		yax[0] = array_factorcir(x1, (pi / 180) * phi[0]);
		double maxi = yax[0];
		int phizero = 0;
		int phi_ref = 1;
		for (int i = 1; i < num1; ++i) {
			yax[i] = array_factorcir(x1, (pi / 180) * phi[i]);
			if (maxi < yax[i]) {
				maxi = yax[i];
				phizero = phi[i];
				phi_ref = i;
			}
		}
		double maxtem = 0;
		int count = 0;

		std::vector<double> sidelobes;
		std::vector<double> sllphi;
		if (yax[0] > yax[num1 - 1] && yax[0] > yax[1]) {
			count++;
			sidelobes.push_back(yax[0]);
			sllphi.push_back(phi[0]);
		}
		if (yax[num1 - 1] > yax[0] && yax[num1 - 1] > yax[num1 - 2]) {
			count++;
			sidelobes.push_back(yax[num1 - 1]);
			sllphi.push_back(phi[num1 - 1]);
		}
		for (int i = 1; i < num1 - 1; ++i) {
			if (yax[i] > yax[i + 1] && yax[i] > yax[i - 1]) {
				count++;
				sidelobes.push_back(yax[i]);
				sllphi.push_back(phi[i]);
			}
		}

//		std::copy(sidelobes, sidelobes + count + 1, sidelobes.begin());
		std::sort(sidelobes.begin(), sidelobes.end(), std::greater<double>());
		double upper_bound = 180;
		double lower_bound = 180;

		if (sidelobes.size() <= 1) {
			obj = m_default_objective;
		}
		else {

			y = sidelobes[1] / maxi;
			double sllreturn = 20 * std::log10(y);
			for (int i = 0; i < num1 / 2; ++i) {
				if ((phi_ref + i) > num1 - 2) {
					upper_bound = 180;
					break;
				}
				double tem = yax[phi_ref + i];
				if (yax[phi_ref + i] < yax[phi_ref + i - 1] && yax[phi_ref + i] < yax[phi_ref + i + 1]) {
					upper_bound = phi[phi_ref + i] - phi[phi_ref];
					break;
				}
			}
			for (int i = 0; i < num1 / 2; ++i) {
				if ((phi_ref - i) < 1) {
					lower_bound = 180;
					break;
				}
				double tem = yax[phi_ref - i];
				if (yax[phi_ref - i] < yax[phi_ref - i - 1] && yax[phi_ref - i] < yax[phi_ref - i + 1]) {
					lower_bound = phi[phi_ref] - phi[phi_ref - i];
					break;
				}
			}
			double bwfn = upper_bound + lower_bound;
			double y1 = 0;
			for (int i = 0; i < num_null; ++i) {
				y1 += array_factorcir(x1, null[i]) / maxi;
			}
			double y2 = 0;
			double uavg = trapezoidalcir(x1, 0, 2 * pi, 50);
			y2 = std::abs(2 * pi * maxi * maxi / uavg);
			double directivity = 10 * std::log10(y2);
			double y3 = std::abs(phizero - m_phi_desired);
			if (y3 < 5) {
				y3 = 0;
			}
			y = 0;
			if (bwfn > 80) {
				y += std::abs(bwfn - 80);
			}
			y = sllreturn + y + y1 + y3;



			//Real y = 0;
			//int num1 = 300;
			//std::vector<Real> phi(num1);
			//for (int i = 0; i < num1; ++i)
			//	phi[i] = i * 360.0 / (num1 - 1);
			//Real phizero = 0;
			//std::vector<Real> yax(num1);
			//yax[0] = arrayFactorCir(x, OFEC_PI / 180 * phi[0]);
			//Real maxi = yax[0];
			//int phi_ref = 0;
			//for (int i = 1; i < num1; ++i) {
			//	yax[i] = arrayFactorCir(x, OFEC_PI / 180 * phi[i]);
			//	if (maxi < yax[i]) {
			//		maxi = yax[i];
			//		phizero = phi[i];
			//		phi_ref = i;
			//	}
			//}
			//std::vector<Real> sidelobes;
			//if (yax[0] > yax[num1 -  1] && yax[0] > yax[1]) {
			//	sidelobes.emplace_back(yax[0]);
			//}
			//if (yax[num1 -  1] > yax[0] && yax[num1 -  1] > yax[num1 - 2]) {
			//	sidelobes.emplace_back(yax[num1 -  1]);
			//}
			//for (size_t i = 1; i < num1 - 1; ++i) {
			//	if (yax[i] > yax[i + 1] && yax[i] > yax[i - 1]) {
			//		sidelobes.emplace_back(yax[i]);
			//	}
			//}
			//std::sort(sidelobes.begin(), sidelobes.end(), std::greater<>());
			//Real upper_bound = 180;
			//Real lower_bound = 180;
			//y = sidelobes[1] / maxi;
			//Real sllreturn = 20 * log10(y);
			//for (int i = 1; i <= num1 / 2; ++i) {
			//	if (phi_ref + i > num1 - 2) {
			//		upper_bound = 180;
			//		break;
			//	}
			//	if (yax[phi_ref + i] < yax[phi_ref + i - 1] && yax[phi_ref + i] < yax[phi_ref + i + 1]) {
			//		upper_bound = phi[phi_ref + i] - phi[phi_ref];
			//		break;
			//	}
			//}
			//for (int i = 1; i <= num1 / 2; ++i) {
			//	if (phi_ref - i < 1) {
			//		lower_bound = 180;
			//		break;
			//	}
			//	if (yax[phi_ref - i] < yax[phi_ref - i - 1] && yax[phi_ref - i] < yax[phi_ref - i + 1]) {
			//		lower_bound = phi[phi_ref] - phi[phi_ref - i];
			//		break;
			//	}
			//}
			//Real bwfn = upper_bound + lower_bound;
			//Real y1 = 0;
			//for (Real null : m_null) {
			//	y1 = y1 + arrayFactorCir(x, null) / maxi;
			//}
			//Real y2 = 0;
			//Real uavg = trapezoidalCir(x, 0, 2 * OFEC_PI, 50);
			//y2 = abs(2 * OFEC_PI * maxi * maxi / uavg);
			//Real y3 = abs(phizero - m_phi_desired);
			//if (y3 < 5) {
			//	y3 = 0;
			//}
			//y = 0;
			//if (bwfn > 80) {
			//	y = y + abs(bwfn - 80);
			//}
			//y = sllreturn + y + y1 + y3;
			obj[0] = y;
		}
	}

	//Real CircularAntennaArrayDesign::arrayFactorCir(const Real *x1, Real phi) const {
	//	Real phi_desired = m_phi_desired;
	//	Real distance = m_distance;
	//	int dim = m_number_variables;
	//	Real y = 0;
	//	Real y1 = 0;
	//	for (int i1 = 0; i1 < dim / 2; ++i1) {
	//		Real delphi = 2 * OFEC_PI * (i1 - 1) / dim;
	//		Real shi = cos(phi - delphi) - cos(phi_desired * (OFEC_PI / 180) - delphi);
	//		shi = shi * dim * distance;
	//		y = y + x1[i1] * cos(shi + x1[dim / 2 + i1] * (OFEC_PI / 180));
	//	}
	//	for (int i1 = dim / 2; i1 < dim; ++i1) {
	//		Real delphi = 2 * OFEC_PI * (i1 - 1) / dim;
	//		Real shi = cos(phi - delphi) - cos(phi_desired * (OFEC_PI / 180) - delphi);
	//		shi = shi * dim * distance;
	//		y = y + x1[i1 - dim / 2] * cos(shi - x1[i1] * (OFEC_PI / 180));
	//	}
	//	for (int i1 = 0; i1 < dim / 2; ++i1) {
	//		Real delphi = 2 * OFEC_PI * (i1 - 1) / dim;
	//		Real shi = cos(phi - delphi) - cos(phi_desired * (OFEC_PI / 180) - delphi);
	//		shi = shi * dim * distance;
	//		y1 = y1 + x1[i1] * sin(shi + x1[dim / 2 + i1] * (OFEC_PI / 180));
	//	}
	//	for (int i1 = dim / 2; i1 < dim; ++i1) {
	//		Real delphi = 2 * OFEC_PI * (i1 - 1) / dim;
	//		Real shi = cos(phi - delphi) - cos(phi_desired * (OFEC_PI / 180) - delphi);
	//		shi = shi * dim * distance;
	//		y1 = y1 + x1[i1 - dim / 2] * sin(shi - x1[i1] * (OFEC_PI / 180));
	//	}
	//	y = y * y + y1 * y1;
	//	y = sqrt(y);
	//	return y;
	//}

	//Real CircularAntennaArrayDesign::trapezoidalCir(const Real *x2, Real upper, Real lower, size_t N1) const {
	//	Real phi_desired = m_phi_desired;
	//	Real distance = m_distance;
	//	int dim = m_number_variables;
	//	Real h = (upper - lower) / N1;
	//	Real x1 = lower;
	//	std::vector<Real> y(N1 + 1);
	//	y[0] = abs(pow(arrayFactorCir(x2, lower), 2) * sin(lower - OFEC_PI / 2));
	//	for (int i = 1; i < N1 + 1; ++i) {
	//		x1 = x1 + h;
	//		y[i] = abs(pow(arrayFactorCir(x2, x1), 2) * sin(x1 - OFEC_PI / 2));
	//	}
	//	Real s = 0;
	//	for (int i = 0; i < N1 + 1; ++i) {
	//		if (i == 0 || i == N1)
	//			s = s + y[i];
	//		else
	//			s = s + 2 * y[i];
	//	}
	//	Real q = (h / 2) * s;
	//	return q;
	//}
}
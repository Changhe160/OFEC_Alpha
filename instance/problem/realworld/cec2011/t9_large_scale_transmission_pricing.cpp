#include "t9_large_scale_transmission_pricing.h"

#include <fstream>
#include <algorithm>
#include <numeric>

#include <Eigen/Dense>

namespace ofec::cec2011 {

	// 函数模拟 MATLAB 的 formybus 函数
	void LargeScaleTransmissionPricing::formybus(const std::vector<LineData>& linedata, int dim, std::vector<std::vector<double>>& YIbus) {
		std::vector<int> busa;
		std::vector<int> busb;
		for (const auto& line : linedata) {
			busa.push_back(line.firstNumber);
			busb.push_back(line.secondNumber);
		}



		//m_error_out<< "busa" << std::endl;

		//for (auto& it : busa) {
		//	m_error_out<< it << "\t";
		//}
		//m_error_out<< std::endl;

		//m_error_out<< "busb" << std::endl;
		//for (auto& it : busb) {
		//	m_error_out<< it << "\t";
		//}
		//m_error_out<< std::endl;



		int L = busa.size();
		std::vector<double> ZI;
		for (const auto& line : linedata) {
			ZI.push_back(std::imag(line.complexValue));
		}

		std::vector<double> YI(L, 0.0);
		for (int i = 0; i < L; ++i) {
			YI[i] = 1.0 / ZI[i];
		}


		YIbus.assign(dim, std::vector<double>(dim, 0.0));
		for (int k = 0; k < L; ++k) {
			int n = busa[k]-1;
			int m = busb[k]-1;
			YIbus[n][n] += YI[k];
			YIbus[n][m] -= YI[k];
			YIbus[m][n] -= YI[k];
			YIbus[m][m] += YI[k];
		}
	}
	void LargeScaleTransmissionPricing::calculateGD(Real* x, std::vector<std::vector<double>>& GD)const {
		GD.resize(g.size(), std::vector<double>(d.size(), 0.0));

		for (size_t i = 0; i < g.size(); ++i) {
			for (size_t j = 0; j < d.size(); ++j) {
				GD[i][j] = x[(i * d.size() + j)];
			}
		}
	}


	double LargeScaleTransmissionPricing::calculateObjective(const std::vector<std::vector<double>>& GD) const {
		std::vector<double> Rg = { 32.7290, 32.1122, 30.3532, 33.6474, 64.1156, 66.8729 };
		std::vector<double> Rd = { 7.5449, 10.7964, 10.9944, 11.0402, 11.7990, 15.3803, 42.6800, 41.4551,
								 73.1939, 57.0430, 45.5920, 43.6553, 61.8002, 59.6409, 57.0279,
								 51.0749, 67.1070, 60.6623, 198.6744, 178.9956, 199.9483 };

		std::vector<double> FC;
		double xijSum = std::accumulate(xij.begin(), xij.end(), 0.0);
		for (double value : xij) {
			FC.push_back(100 * value / xijSum);
		}



		std::vector<double> flows(na.size(), 0.0);
		std::vector<double> cost_line(na.size(), 0.0);

		for (size_t k = 0; k < na.size(); ++k) {
			for (size_t i = 0; i < g.size(); ++i) {
				for (size_t j = 0; j < d.size(); ++j) {
					flows[k] += std::abs(PTDF[i][j][k] * GD[i][j]) + std::abs(PTDF[i][j][k] * BT[i][j]);
				}
			}
			cost_line[k] = FC[k] / flows[k];
		}

		//m_error_out << "cost_line" << std::endl;
		//for (auto& it : cost_line) {
		//	m_error_out << it << "\t";
		//}
		//m_error_out << std::endl;



		std::vector<double> pg;
		for (size_t i = 0; i < Pg.size(); ++i) {
			pg.push_back(Pg[i] - Pg2[i]);
		}

		std::vector<double> pd;
		for (size_t i = 0; i < Pd.size(); ++i) {
			pd.push_back(Pd[i] - Pd2[i]);
		}


		double rate_ebe1 = 0;
		std::vector<std::vector<double>> cost_l(GD.size(),std::vector<double>(GD.front().size(), 0));
		std::vector<double> cost_gen(GD.size(),0);


		for (size_t i = 0; i < GD.size(); ++i) {
			for (size_t j = 0; j < GD[i].size(); ++j) {
				for (size_t k = 0; k < na.size(); ++k) {
					cost_l[i][j] += std::abs(cost_line[k] * PTDF[i][j][k]);
				}
				cost_gen[i] += GD[i][j] * cost_l[i][j];
			}
			rate_ebe1 += std::pow((cost_gen[i] / pg[i] - Rg[i]), 2);
		}


		double rate_ebe2 = 0;
		std::vector<double> cost_load(GD[0].size(), 0.0);


		for (size_t j = 0; j < d.size(); ++j) {
			for (size_t i = 0; i < g.size(); ++i) {
				cost_load[j] += GD[i][j] * cost_l[i][j];
			}
			rate_ebe2 += std::pow((cost_load[j] / pd[j] - Rd[j]), 2);
		}


	    return rate_ebe1 + rate_ebe2;

	}
	double LargeScaleTransmissionPricing::calculateViolations(const std::vector<std::vector<double>>& GD)const {
		std::vector<double> Pg_x(g.size(), 0.0);
		for (size_t i = 0; i < g.size(); ++i) {
			for (size_t j = 0; j < d.size(); ++j) {
				Pg_x[i] += GD[i][j];
			}
			for (size_t j = 0; j < d.size(); ++j) {
				Pg_x[i] += BT[i][j];
			}
		}

		std::vector<double> Pd_x(d.size(), 0.0);
		for (size_t j = 0; j < d.size(); ++j) {
			for (size_t i = 0; i < g.size(); ++i) {
				Pd_x[j] += GD[i][j];
			}
			for (size_t i = 0; i < g.size(); ++i) {
				Pd_x[j] += BT[i][j];
			}
		}

		double Gpen = 0;
		for (size_t i = 0; i < g.size(); ++i) {
			double diff = std::abs(Pg_x[i] - Pg[i]);
			Gpen += 100 * diff;
		}

		double LDpen = 0;
		for (size_t i = 0; i < d.size(); ++i) {
			double diff = std::abs(Pd_x[i] - Pd[i]);
			LDpen += 100 * diff;
		}

		double PENALTY = Gpen + LDpen;

		return PENALTY;
	}

	void LargeScaleTransmissionPricing::addInputParameters() {
	}

	void LargeScaleTransmissionPricing::initialize_(Environment* env) {
		Continuous::initialize_(env);
		
		resizeVariable(m_number_variables);
		resizeObjective(1);
		m_optimize_mode[0] = OptimizeMode::kMinimize;

		readData();
		m_number_variables = g.size() * d.size();
		resizeVariable(m_number_variables);
		for (int i = 0; i < g.size(); ++i) {
			for (int j = 0; j < d.size(); ++j) {
				m_domain.setRange(0, m_GD_max[i][j], (i * d.size() + j));
			}
		}
		
	}
	void LargeScaleTransmissionPricing::readData() {


		double basemva = 100;
		double accuracy = 0.0001;
		double maxiter = 10;
		double  tc = 283.4 * 4.52 * 2;
		std::vector<std::vector<double>> ebeData;
		readEBEdata(ebeData);

		std::vector<LineData> lineData;
		readLineData(lineData);

		

		na.resize(lineData.size());
		nb.resize(lineData.size());
		for (int idx(0); idx < lineData.size(); ++idx) {
			na[idx] = lineData[idx].firstNumber-1;
			nb[idx] = lineData[idx].secondNumber-1;
		}
		
		Pg.clear();
		Pd.clear();

		for (const auto& bus : ebeData) {
			Pg.push_back(bus[6] / 100.0);
			Pd.push_back(bus[4] / 100.0);
		}
		g.clear();
		d.clear();

		for (int i = 0; i < Pg.size(); ++i) {
			if (Pg[i] > 0) {
				g.push_back(i);
			}
			if (Pd[i] > 0) {
				d.push_back(i);
			}
		}

		std::vector<double> newPg;
		for (int index : g) {
			newPg.push_back(Pg[index]);
		}
		Pg = newPg;

		std::vector<double> newPd;
		for (int index : d) {
			newPd.push_back(Pd[index]);
		}
		Pd = newPd;

		BT.resize(g.size(), std::vector<double>(d.size(), 0.0));
		for (auto& it : BT) {
			std::fill(it.begin(), it.end(), 0);
		}

		BT[0][3] = 5;
		BT[0][4] = 10;
		BT[0][5] = 5;
		BT[1][2] = 5;
		BT[2][20] = 2.5;
		BT[3][20] = 2.5;
		BT[3][15] = 15;
		BT[4][11] = 2.5;
		BT[5][7] = 2.5;

		for (auto& row : BT) {
			for (auto& value : row) {
				value /= 100.0;
			}
		}

		m_GD_max.resize(g.size(), std::vector<double>(d.size(), 0.0));

		for (int i = 0; i < g.size(); ++i) {
			for (int j = 0; j < d.size(); ++j) {
				double temp1 = Pg[i] - BT[i][j];
				double temp2 = Pd[j] - BT[i][j];
				m_GD_max[i][j] = std::min(temp1, temp2);
			}
		}


		Pg2.resize(g.size(), 0.0);
		for (size_t i = 0; i < g.size(); ++i) {
			for (size_t j = 0; j < d.size(); ++j) {
				Pg2[i] += BT[i][j];
			}
		}

		Pd2.resize(d.size(), 0.0);
		for (size_t j = 0; j < d.size(); ++j) {
			for (size_t i = 0; i < g.size(); ++i) {
				Pd2[j] += BT[i][j];
			}
		}





		int dim = ebeData.size();

		std::vector<std::vector<double>> YIbus;

		formybus(lineData, dim, YIbus);
		YIbus.erase(YIbus.begin());
		for (auto& row : YIbus) {
			row.erase(row.begin());
		}



	    
		//Matrix Xt(YIbus.size(), YIbus.front().size());
		//for (int idx(0); idx < YIbus.size(); ++idx) {
		//	for (int idy(0); idy < YIbus[idx].size(); ++idy) {
		//		Xt[idx][idy] = YIbus[idx][idy];
		//	}
		//}
		//Xt.inverse();


		std::vector<std::vector<double>> X(dim, std::vector<double>(dim, 0.0));
		{
			const auto& a = YIbus;
			Eigen::MatrixXd eigenMatrix(a.size(), a[0].size());
			for (int i = 0; i < a.size(); ++i) {
				for (int j = 0; j < a[i].size(); ++j) {
					eigenMatrix(i, j) = a[i][j];
				}
			}

			Eigen::MatrixXd Xt;
			if (eigenMatrix.determinant() != 0) {
				Xt = eigenMatrix.inverse();
			}
			else {
				throw Exception("Matrix is singular. Cannot find inverse.");
			}

			for (int i = 0; i < dim; ++i) {
				for (int j = 0; j < dim; ++j) {
					if (i != 0 && j != 0) {
						X[i][j] = Xt(i - 1,j - 1);
					}
					else {
						X[i][j] = 0;
					}
				}
			}

		}
		


		//m_error_out << "X" << std::endl;

		//for (auto& it : X) {
		//	for (auto& it2 : it) {
		//		m_error_out << it2 << "\t";
		//	}
		//	m_error_out << std::endl;
		//}

		xij.clear();
		for (const auto& line : lineData) {
			xij.push_back(std::imag(line.complexValue));
		}


		for (size_t i = 0; i < g.size(); ++i) {
			PTDF.resize(g.size());
			for (size_t j = 0; j < d.size(); ++j) {
				PTDF[i].resize(d.size());
				for (size_t k = 0; k < na.size(); ++k) {
					PTDF[i][j].resize(na.size());
					PTDF[i][j][k] = ((X[na[k]][g[i]] - X[nb[k]][g[i]] - X[na[k]][d[j]] + X[nb[k]][d[j]]) / xij[k]);
				}
			}
		}


		//for (auto& it : PTDF.front()) {
		//	for (auto& it2 : it) {
		//		m_error_out << it2 << "\t";
		//	}
		//	m_error_out << std::endl;
		//}

	}

	void LargeScaleTransmissionPricing::readEBEdata(std::vector<std::vector<double>>& ebeData) {
		std::ifstream inputFile(std::string(OFEC_DIR) + "/instance/problem/realworld/cec2011/data/EBEinputfile.txt");

		std::vector<double> curValue(11);
		std::string line;
		while (std::getline(inputFile, line)) {
			std::istringstream iss(line);
			for (auto& it : curValue) {
				iss >> it;
			}
			ebeData.emplace_back(curValue);
		}
		inputFile.close();
	}

	void LargeScaleTransmissionPricing::readLineData(std::vector<LineData>& dataList) {
		std::ifstream inputFile(std::string(OFEC_DIR) + "/instance/problem/realworld/cec2011/data/EBE_linedata.txt");
	//	std::vector<LineData> dataList;

		if (inputFile.is_open()) {
			std::string line;
			while (std::getline(inputFile, line)) {
				std::istringstream iss(line);
				LineData data;
				std::string complexPart;
				iss >> data.firstNumber >> data.secondNumber >> complexPart >> data.fourthValue >> data.fifthValue >> data.sixthValue;

				// 解析复数部分
				size_t plusPos = complexPart.find('+');
				double realPart = std::stod(complexPart.substr(0, plusPos));
				double imagPart = std::stod(complexPart.substr(plusPos + 1, complexPart.length() - plusPos - 2));
				data.complexValue = std::complex<double>(realPart, imagPart);

				dataList.push_back(data);
			}
			inputFile.close();
		}
	}

	void LargeScaleTransmissionPricing::evaluateObjective(Real* x, std::vector<Real>& obj) const {
		std::vector<std::vector<double>> GD;
		calculateGD(x, GD);
		

		obj[0] = calculateObjective(GD) + 50*Kp * calculateViolations(GD);
	}
}
#ifndef NODES_INITIALIZE_H
#define NODES_INITIALIZE_H

/*
This header file is used to generate test data for non-dominance sorting.
It has several initial objective-space distributions of solutions.
*/

#include "../random/newran.h"
#include <math.h>
#include <fstream>
#include <vector>

namespace NDS {
	class circle_distribution {
	private:
		const int m_obj_num;
		const int m_data_size;
		std::vector<std::vector<double>> m_data;
	public:
		circle_distribution(const int obj_num, const int data_size, OFEC::random& rand) :m_obj_num(obj_num), m_data_size(data_size) {
			for (int i = 0; i < m_data_size;) {
				std::vector<double> temp;
				double dis(0);
				for (int j = 0; j < m_obj_num; j++) {
					temp.push_back(rand.next());
					dis += temp[j] * temp[j];
				}
				dis = sqrt(dis);
				if (rand.next() < pow((1 - abs(0.8 - dis) / 0.8),10)) {
					m_data.push_back(temp);
					++i;
				}
			}
		}
		std::vector<double> new_one(OFEC::random& rand) {
			while (true) {
				std::vector<double> temp;
				double dis(0);
				for (int j = 0; j < m_obj_num; j++) {
					temp.push_back(rand.next());
					dis += temp[j] * temp[j];
				}
				dis = sqrt(dis);
				if (rand.next() < pow((1 - abs(0.8 - dis) / 0.8), 10))
					return temp;
			}
		}
		void output_file() {
			std::ofstream file("e:\\Circle_distribution.csv");
			for (auto& datum : m_data) {
				for (double obj : datum) {
					if (obj != datum.back())
						file << obj << ", ";
					else
						file << obj;
				}
				file << std::endl;
			}
			file.close();
		}
		std::vector<std::vector<double>>& get_data() { return m_data; }
	};
	class ellipse_distribution {
	private:
		const int m_obj_num;
		const int m_data_size;
		std::vector<std::vector<double>> m_data;
	public:
		ellipse_distribution(const int obj_num, const int data_size, OFEC::random& rand) :m_obj_num(obj_num), m_data_size(data_size){
			for (int i = 0; i < m_data_size;) {
				std::vector<double> temp;
				std::vector<double> p(2, 0);
				std::vector<double> ellipse_F1(m_obj_num, 0.5);
				std::vector<double> ellipse_F2(m_obj_num, 1.0);

				for (int j = 0; j < m_obj_num; ++j) {
					p[0] += ellipse_F1[j] * ellipse_F1[j];
					p[1] += ellipse_F2[j] * ellipse_F2[j];
				}
				double maxdis = sqrt(p[0]) + sqrt(p[1]);

				p[0] = 0; p[1] = 0;
				for (int j = 0; j < m_obj_num; ++j) {
					temp.push_back(rand.next());
					p[0] += (temp[j] - ellipse_F1[j]) * (temp[j] - ellipse_F1[j]);
					p[1] += (temp[j] - ellipse_F2[j]) * (temp[j] - ellipse_F2[j]);
				}
				double dis = sqrt(p[0]) + sqrt(p[1]);

				if (rand.next() < pow((1 - dis / maxdis), 10)) {
					m_data.push_back(temp);
					++i;
				}
			}
		}
		std::vector<double> new_one(OFEC::random& rand) {
			while (true) {
				std::vector<double> temp;
				std::vector<double> p(2, 0);
				std::vector<double> ellipse_F1(m_obj_num, 0.5);
				std::vector<double> ellipse_F2(m_obj_num, 1.0);

				for (int j = 0; j < m_obj_num; ++j) {
					p[0] += ellipse_F1[j] * ellipse_F1[j];
					p[1] += ellipse_F2[j] * ellipse_F2[j];
				}
				double maxdis = sqrt(p[0]) + sqrt(p[1]);

				p[0] = 0; p[1] = 0;
				for (int j = 0; j < m_obj_num; ++j) {
					temp.push_back(rand.next());
					p[0] += (temp[j] - ellipse_F1[j]) * (temp[j] - ellipse_F1[j]);
					p[1] += (temp[j] - ellipse_F2[j]) * (temp[j] - ellipse_F2[j]);
				}
				double dis = sqrt(p[0]) + sqrt(p[1]);

				if (rand.next() < pow((1 - dis / maxdis), 10))
					return temp;
			}
		}
		void output_file() {
			std::ofstream file("e:\\Ellipse_distribution.csv");
			for (auto& datum : m_data) {
				for (double obj : datum) {
					if (obj != datum.back())
						file << obj << ", ";
					else
						file << obj;
				}
				file << std::endl;
			}
			file.close();
		}
		std::vector<std::vector<double>> get_data() { return m_data; }
	};
	class hyperbola_distribution {
	private:
		const int m_obj_num;
		const int m_data_size;
		std::vector<std::vector<double>> m_data;
	public:
		hyperbola_distribution(const int obj_num, const int data_size, OFEC::random& rand) :m_obj_num(obj_num), m_data_size(data_size){
			for (int i = 0; i < m_data_size;) {
				std::vector<double> temp;
				std::vector<double> p(2, 0);
				std::vector<double> ellipse_F1(m_obj_num, 0.3);
				std::vector<double> ellipse_F2(m_obj_num, 1.0);

				for (int j = 0; j < m_obj_num; ++j) {
					p[0] += ellipse_F1[j] * ellipse_F1[j];
					p[1] += ellipse_F2[j] * ellipse_F2[j];
				}
				double maxdis = abs(sqrt(p[0]) - sqrt(p[1]));

				p[0] = 0; p[1] = 0;
				for (int j = 0; j < m_obj_num; ++j) {
					temp.push_back(rand.next());
					p[0] += (temp[j] - ellipse_F1[j]) * (temp[j] - ellipse_F1[j]);
					p[1] += (temp[j] - ellipse_F2[j]) * (temp[j] - ellipse_F2[j]);
				}
				double dis = abs(sqrt(p[0]) - sqrt(p[1]));

				if (rand.next() < pow((1 - dis / maxdis), 3)) {
					m_data.push_back(temp);
					++i;
				}
			}
		}
		void update(OFEC::random& rand) {
			for (int i = 0; i < m_data_size;) {
				std::vector<double> temp;
				std::vector<double> p(2, 0);
				std::vector<double> ellipse_F1(m_obj_num, 0.5);
				std::vector<double> ellipse_F2(m_obj_num, 1.0);

				for (int j = 0; j < m_obj_num; ++j) {
					p[0] += ellipse_F1[j] * ellipse_F1[j];
					p[1] += ellipse_F2[j] * ellipse_F2[j];
				}
				double maxdis = abs(sqrt(p[0]) - sqrt(p[1]));

				p[0] = 0; p[1] = 0;
				for (int j = 0; j < m_obj_num; ++j) {
					temp.push_back(rand.next());
					p[0] += (temp[j] - ellipse_F1[j]) * (temp[j] - ellipse_F1[j]);
					p[1] += (temp[j] - ellipse_F2[j]) * (temp[j] - ellipse_F2[j]);
				}
				double dis = abs(sqrt(p[0]) + sqrt(p[1]));

				if (rand.next() < pow((1 - dis / maxdis), 10)) {
					m_data[i] = temp;
					++i;
				}
			}
		}
		void output_file() {
			std::ofstream file("e:\\Hyperbola_distribution.csv");
			for (auto& datum : m_data) {
				for (double obj : datum) {
					if (obj != datum.back())
						file << obj << ", ";
					else
						file << obj;
				}
				file << std::endl;
			}
			file.close();
		}
		std::vector<std::vector<double>> get_data() { return m_data; }
	};
	class points_distribution {
	private:
		const int m_obj_num;
		const int m_data_size;
		std::vector<std::vector<double>> m_data;
	public:
		points_distribution(const int obj_num, const int data_size, OFEC::random& rand) :m_obj_num(obj_num), m_data_size(data_size){
			for (int i = 0; i < m_data_size;) {
				std::vector<double> temp;
				std::vector<std::vector<double>> points(m_obj_num, std::vector<double>(m_obj_num, 0.3));
				for (int j = 0; j < m_obj_num; ++j)
					points[j][j] = 0.7;
				std::vector<double> pdis(m_obj_num, 0);
				double dis(0);
				double maxdis(0);
				for (int j = 0; j < m_obj_num; ++j) {
					temp.push_back(rand.next());
					for (int k = 0; k < m_obj_num; ++k) {
						pdis[k] += (points[k][j] - temp[j])*(points[k][j] - temp[j]);
						maxdis += points[k][j] * points[k][j];
					}
				}
				double min_pdis(pdis[0]);
				for (int k = 1; k < m_obj_num; ++k)
					if (min_pdis > pdis[k])
						min_pdis = pdis[k];
				dis = sqrt(min_pdis);
				maxdis = sqrt(maxdis);
				if (rand.next() < pow((1 - dis/maxdis), 10)) {
					m_data.push_back(temp);
					++i;
				}
			}
		}
		void update(OFEC::random& rand) {
			for (int i = 0; i < m_data_size;) {
				std::vector<double> temp;
				std::vector<std::vector<double>> points(m_obj_num, std::vector<double>(m_obj_num, 0.2));
				for (int j = 0; j < m_obj_num; ++j)
					points[i][i] = 0.8;
				std::vector<double> pdis(m_obj_num, 0);
				double dis(0);
				double maxdis(0);
				for (int j = 0; j < m_obj_num; ++j) {
					temp.push_back(rand.next());
					for (int k = 0; k < m_obj_num; ++k) {
						pdis[k] += (points[k][j] - temp[j])*(points[k][j] - temp[j]);
						maxdis += points[k][j] * points[k][j];
					}
				}
				double min_pdis(pdis[0]);
				for (int k = 1; k < m_obj_num; ++k)
					if (min_pdis > pdis[k])
						min_pdis = pdis[k];
				dis = sqrt(min_pdis);
				maxdis = sqrt(maxdis);
				if (rand.next() < pow((1 - dis / maxdis), 10)) {
					m_data[i] = temp;
					++i;
				}
			}
		}
		void output_file() {
			std::ofstream file("e:\\Points_distribution.csv");
			for (auto& datum : m_data) {
				for (double obj : datum) {
					if (obj != datum.back())
						file << obj << ", ";
					else
						file << obj;
				}
				file << std::endl;
			}
			file.close();
		}
		std::vector<std::vector<double>> get_data() { return m_data; }
	};
	class line_distribution {
	private:
		const int m_obj_num;
		const int m_data_size;
		std::vector<std::vector<double>> m_data;
	public:
		line_distribution(const int obj_num, const int data_size, OFEC::random& rand) :m_obj_num(obj_num), m_data_size(data_size){
			for (int i = 0; i < m_data_size;) {
				std::vector<double> temp;
				double dis(0);
				for (int j = 0; j < m_obj_num; j++)
					temp.push_back(rand.next());
				dis = temp[0] - 0.7;
				double maxdis = 0 - 0.7;
				if (rand.next() < pow((1 - abs(dis/maxdis)), 10)) {
					m_data.push_back(temp);
					++i;
				}
			}
		}
		std::vector<double> new_one(OFEC::random& rand) {
			while (true) {
				std::vector<double> temp;
				double dis(0);
				for (int j = 0; j < m_obj_num; j++)
					temp.push_back(rand.next());
				dis = temp[0] - 0.7;
				double maxdis = 0 - 0.7;
				if (rand.next() < pow((1 - abs(dis / maxdis)), 10)) {
					return temp;
				}
			}
		}
		void output_file() {
			std::ofstream file("e:\\Line_distribution.csv");
			for (auto& datum : m_data) {
				for (double obj : datum) {
					if (obj != datum.back())
						file << obj << ", ";
					else
						file << obj;
				}
				file << std::endl;
			}
			file.close();
		}
		std::vector<std::vector<double>> get_data() { return m_data; }
	};
}

#endif // !NODES_INITIALIZE_H


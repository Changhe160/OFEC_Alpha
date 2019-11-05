#ifndef FCM_H
#define FCM_H

//Fuzzy c means clustering
//https://en.wikipedia.org/wiki/Fuzzy_clustering
//Code Author: gyaikhom
//https://github.com/gyaikhom/fcm

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <locale>

class fcm
{
public:
	struct Result {
		std::vector<std::vector<double>> membership;
		int numClst;
		std::vector<std::vector<size_t>> member;
	};
	fcm(const size_t Dim, const double Fuzziness, const double Epsilon, const size_t Num_clusters) :m_dim(Dim), m_fuzziness(Fuzziness), m_epsilon(Epsilon), m_num_clusters(Num_clusters) {
		if (m_fuzziness <= 1.0)
		{
			std::cout << "The fuzziness must be > 1.0 !" << std::endl;
			exit(1);
		}
		if (m_epsilon > 1.0 || m_epsilon <= 0.0)
		{
			std::cout << "The epsilon must be <= 1.0 !" << std::endl;
			exit(1);
		}
	}

	void clustering(std::vector<std::vector<double>> &Data_points, fcm::Result &);
private:
	size_t m_dim;
	double m_fuzziness;
	double m_epsilon;
	std::vector<std::pair<double, double>> m_min_max;
	std::vector<std::vector<double>> m_data_points;
	size_t m_num_data_points = 0;
	size_t m_num_clusters;
	std::vector<std::vector<double>> m_degree_of_memb;
	std::vector<std::vector<double>> m_cluster_centre;

	void init(std::vector<std::vector<double>> &Data_points);
	void calculate_centre_vectors();
	double get_norm(int i, int j);
	double get_new_value(int i, int j);
	double update_degree_of_membership();


};

#endif

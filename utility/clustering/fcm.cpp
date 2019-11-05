#include "fcm.h"
#include "../../core/global.h"

void fcm::init(std::vector<std::vector<double>> &Data_points)
{
	m_data_points = Data_points;
	m_num_data_points = m_data_points.size();

	m_min_max.resize(m_dim);
	for (int i = 0; i < m_num_data_points; i++) {
		for (int j = 0; j < m_dim; j++) {
			if (m_data_points[i][j] < m_min_max[j].first)
				m_min_max[j].first = m_data_points[i][j];
			if (m_data_points[i][j] > m_min_max[j].second)
				m_min_max[j].second = m_data_points[i][j];
		}
	}

	//initial membership
	m_degree_of_memb.resize(m_num_data_points);
	double s;
	int r, rval;
	for (int i = 0; i < m_num_data_points; ++i) {
		s = 0.0;
		r = 100;
		m_degree_of_memb[i].resize(m_num_clusters);
		for (int j = 1; j < m_num_clusters; ++j) {
			rval = OFEC::global::ms_global->m_uniform[OFEC::caller::Problem]->next_non_standard<size_t>(0, r + 1);
			//rval = rand() % (r + 1);
			r -= rval;
			m_degree_of_memb[i][j] = rval / 100.0;
			s += m_degree_of_memb[i][j];
		}
		m_degree_of_memb[i][0] = 1.0 - s;
	}
}

void fcm::calculate_centre_vectors()
{
	double numerator, denominator;
	std::vector<std::vector<double>> t;
	t.resize(m_num_data_points);
	for (int i = 0; i < m_num_data_points; ++i) {
		t[i].resize(m_num_clusters);
		for (int j = 0; j < m_num_clusters; ++j) {
			t[i][j] = pow(m_degree_of_memb[i][j], m_fuzziness);
		}
	}
	m_cluster_centre.resize(m_num_clusters);
	for (int j = 0; j < m_num_clusters; j++) {
		m_cluster_centre[j].resize(m_dim);
		for (int k = 0; k < m_dim; k++) {
			numerator = 0.0;
			denominator = 0.0;
			for (int i = 0; i < m_num_data_points; i++) {
				numerator += t[i][j] * m_data_points[i][k];
				denominator += t[i][j];
			}
			m_cluster_centre[j][k] = numerator / denominator;
		}
	}
}

void fcm::clustering(std::vector<std::vector<double>> &Data_points, fcm::Result &result)
{
	double max_diff;
	init(Data_points);
	do {
		calculate_centre_vectors();
		max_diff = update_degree_of_membership();
	} while (max_diff > m_epsilon);


	result.membership = m_degree_of_memb;
	result.numClst = m_num_clusters;
	int cluster;
	double highest;
	result.member.resize(m_num_clusters);
	for (int i = 0; i < m_num_data_points; i++) {
		cluster = 0;
		highest = 0.0;
		for (int j = 0; j < m_num_clusters; j++) {
			if (m_degree_of_memb[i][j] > highest) {
				highest = m_degree_of_memb[i][j];
				cluster = j;
			}
		}
		result.member[cluster].push_back(i);
	}
}

double fcm::get_norm(int i, int j)
{

	double sum = 0.0;
	int k;
	for (k = 0; k < m_dim; k++) {
		sum += pow(m_data_points[i][k] - m_cluster_centre[j][k], 2);
	}

	return sqrt(sum);
}

double fcm::get_new_value(int i, int j)
{
	int k;
	double t, p, sum;
	sum = 0.0;
	p = 2 / (m_fuzziness - 1);
	for (k = 0; k < m_num_clusters; k++) {
		t = get_norm(i, j) / get_norm(i, k);
		t = pow(t, p);
		sum += t;
	}
	return 1.0 / sum;
}

double fcm::update_degree_of_membership()
{
	int i, j;
	double new_uij;
	double max_diff = 0.0, diff;
	for (j = 0; j < m_num_clusters; j++) {
		for (i = 0; i < m_num_data_points; i++) {
			new_uij = get_new_value(i, j);
			diff = new_uij - m_degree_of_memb[i][j];
			if (diff > max_diff)
				max_diff = diff;
			m_degree_of_memb[i][j] = new_uij;
		}
	}
	return max_diff;
}

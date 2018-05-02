#include "benchmark.h"
#include "../../functional.h"

#ifdef _WIN32  
#include <direct.h>  
#elif __linux
#include <sys/stat.h>  
#endif  

#ifdef _WIN32   
#define MKDIR(a) _mkdir((a))  
#elif __linux   
#define MKDIR(a) mkdir((a),0755)  
#endif  

namespace NS {

	data_generator::data_generator(const std::string& name, int num_sol, int num_obj, double rand_seed) 
		: m_name(name), m_num_sol(num_sol), m_num_obj(num_obj), m_rand(rand_seed) {}

	void data_generator::output_data(const std::stringstream& path)
	{
		std::ofstream outputfile(path.str());
		outputfile.precision(18);
		for (int i = 0; i < m_num_obj - 1; ++i)
			outputfile << "obj" << i + 1 << ",";
		outputfile << "obj" << m_num_obj << std::endl;
		for (auto& row : m_data) {
			for (int i = 0; i < m_num_obj - 1; ++i)
				outputfile << row[i] << ",";
			outputfile << row[m_num_obj - 1] << std::endl;
		}
		outputfile.close();
	}

	void data_generator::shuffle_data()
	{
		//std::vector<int> shuffle_indexs(m_num_sol);
		//for (int i = 0; i < m_num_sol; ++i)
		//	shuffle_indexs[i] = i;
		m_rand.shuffle(m_data.begin(), m_data.end());
	}

	void data_generator::output_data_set(int num_data)
	{
		std::stringstream dir;
		dir << "result/" << m_name << "/sol" << m_num_sol << "_obj" << m_num_obj;
		MKDIR(dir.str().c_str());
		for (int i = 0; i < num_data; ++i) {
			std::stringstream path;
			path << dir.str() << "/data_" << i << ".csv";
			update_data();
			output_data(path);
		}
	}

	void data_generator::read_data(std::vector<std::vector<double>>& data, int number)
	{
		std::stringstream path;
		path << "result/" << m_name << "/sol" << m_num_sol << "_obj" << m_num_obj << "/data_" << number << ".csv";
		std::ifstream inputfile(path.str());
		std::string head;
		std::getline(inputfile, head);
		data.clear();
		data.resize(m_num_sol);
		for (auto& row : data)
			row.resize(m_num_obj);
		std::string temp;
		for (int i = 0; i < m_num_sol; ++i) {
			for (int j = 0; j < m_num_obj - 1; ++j) {
				std::getline(inputfile, temp, ',');
				data[i][j] = atof(temp.c_str());
			}
			std::getline(inputfile, temp);
			data[i][m_num_obj - 1] = atof(temp.c_str());
		}
		inputfile.close();
	}

	benchmark1::benchmark1(int num_sol, int num_obj, double rand_seed) : data_generator("benchmark1",num_sol,num_obj,rand_seed){}

	void benchmark1::update_data()
	{
		m_data.clear();
		m_data.reserve(m_num_sol);
		for (int i = 0; i < m_num_sol;) {
			std::vector<double> temp;
			double multiplicative(1.);
			for (int j = 0; j < m_num_obj; j++) {
				temp.push_back(m_rand.next_non_standard(0.,1.));
				multiplicative *= temp[j];
			}
			double difference = multiplicative - pow(0.2, m_num_obj);
			if (difference > 0){
				double biggest_dif = 1 - pow(0.2, m_num_obj);
				double prob = m_rand.next_non_standard(0., biggest_dif);
				if (pow(prob, 50) > difference) {
					m_data.push_back(temp);
					++i;
				}
			}
		}
	}

	fixed_num_front::fixed_num_front(const std::string & name, int num_sol, int num_obj, int num_fro, double rand_seed)
		: data_generator(name, num_sol, num_obj, rand_seed), m_num_fro(num_fro) {}

	void fixed_num_front::output_data_set(int num_data)
	{
		std::stringstream dir;
		dir << "result/" << m_name << "/sol" << m_num_sol << "_obj" << m_num_obj << "_fro" << m_num_fro;
		MKDIR(dir.str().c_str());
		for (int i = 0; i < num_data; ++i) {
			std::stringstream path;
			path << dir.str() << "/data_" << i << ".csv";
			update_data();
			output_data(path);
		}
	}

	void fixed_num_front::read_data(std::vector<std::vector<double>>& data, int number)
	{
		std::stringstream path;
		path << "result/" << m_name << "/sol" << m_num_sol << "_obj" << m_num_obj << "_fro" << m_num_fro << "/data_" << number << ".csv";
		std::ifstream inputfile(path.str());
		std::string head;
		std::getline(inputfile, head);
		data.resize(m_num_sol);
		for (auto& row : data)
			row.resize(m_num_obj);
		std::string temp;
		for (int i = 0; i < m_num_sol; ++i) {
			for (int j = 0; j < m_num_obj - 1; ++j) {
				std::getline(inputfile, temp, ',');
				data[i][j] = atof(temp.c_str());
			}
			std::getline(inputfile, temp);
			data[i][m_num_obj - 1] = atof(temp.c_str());
		}
		inputfile.close();
	}

	benchmark2::benchmark2(int num_sol, int num_obj, int num_fro, double rand_seed) : fixed_num_front("benchmark2",num_sol,num_obj,num_fro,rand_seed) {}

	//void benchmark2::update_data()
	//{
	//	m_data.clear();
	//	m_data.reserve(m_num_sol);
	//	const double Magnification(1);
	//	int cur_rank = 1;
	//	int num_node = 0;
	//	std::vector<std::vector<double>> last_bounds;
	//	std::vector<std::vector<double>> cur_bounds;
	//	last_bounds.push_back(std::vector<double>(m_num_obj, 0.));
	//	while (num_node < m_num_sol) {
	//		std::vector<double> new_node(m_num_obj);
	//		double radius = cur_rank * 1. / m_num_fro * Magnification;
	//		std::vector<double>& lower_bound = last_bounds[m_rand.next_non_standard(static_cast<std::std::size_t>(0), last_bounds.size())];
	//		std::vector<double> upper_bound(m_num_obj);
	//		for (int i = 0; i < m_num_obj; ++i) {
	//			double pow_sum(0);
	//			for (int j = 0; j < m_num_obj; ++j) {
	//				if (j != i)
	//					pow_sum += pow(lower_bound[j], 2);
	//			}
	//			upper_bound[i] = sqrt(pow(radius, 2) - pow_sum);
	//		}
	//		for (int i = 0; i < m_num_obj; ++i)
	//			new_node[i] = m_rand.next_non_standard(lower_bound[i], upper_bound[i]);
	//		double lenth(0);
	//		for (double x : new_node)
	//			lenth += pow(x, 2);
	//		lenth = sqrt(lenth);
	//		for (double& x : new_node)
	//			x = x / lenth * radius;
	//		// check node's validity
	//		bool flag_valid(true);
	//		for (int i = 0; i < m_num_obj; ++i)
	//			if (new_node[i] < lower_bound[i] || new_node[i] > upper_bound[i]) {
	//				flag_valid = false;
	//				break;
	//			}
	//		if (flag_valid) {
	//			m_data.push_back(new_node);
	//			cur_bounds.push_back(new_node);
	//			num_node++;
	//		}
	//		if (cur_rank < m_num_fro && num_node == cur_rank * m_num_sol / m_num_fro) {
	//			cur_rank++;
	//			last_bounds = cur_bounds;
	//			cur_bounds.clear();
	//		}
	//	}
	//}

	void benchmark2::update_data()
	{
		m_data.clear();
		m_data.reserve(m_num_sol);
		std::vector<int> num_each_fro(m_num_fro);
		double temp(0);
		for (int k = 1; k < m_num_fro + 1; ++k)
			temp += pow(k, (m_num_obj-1));
		int rest(m_num_sol);
		for (int k = 1; k < m_num_fro; ++k) {
			rest -= num_each_fro[k - 1] = static_cast<int>(pow(k, (m_num_obj - 1)) * m_num_sol / temp) + 1;
		}
		num_each_fro[m_num_fro - 1] = rest;
		int cur_rank = 0;
		int num_node = 0;
		int num_fro_node = 0;
		double radius = (cur_rank+1) * 1. / m_num_fro;
		std::vector<std::vector<double>> last_bounds;
		std::vector<std::vector<double>> cur_bounds;
		last_bounds.push_back(std::vector<double>(m_num_obj, 0.));
		while (num_node < m_num_sol) {
			// randomly generate a node
			std::vector<double> new_node(m_num_obj);
			for (int i = 0; i < m_num_obj; ++i)
				new_node[i] = m_rand.next_non_standard(0., 1.);
			// stretch it
			double lenth(0);
			for (double x : new_node)
				lenth += pow(x, 2);
			lenth = sqrt(lenth);
			for (double& x : new_node)
				x = x / lenth * radius;
			// check node's validity
			bool flag1(false);
			for (int i = 0; i < last_bounds.size(); ++i) {
				bool flag2(true);
				for (int j = 0; j < m_num_obj; ++j) {
					if (new_node[j] < last_bounds[i][j]) {
						flag2 = false;
						break;
					}
				}
				if (flag2) {
					flag1 = true;
					break;
				}
			}
			if (flag1) {
				m_data.push_back(new_node);
				cur_bounds.push_back(new_node);
				num_node++;
				num_fro_node++;
			}
			//***********
			if (num_fro_node == num_each_fro[cur_rank]) {
				cur_rank++;
				radius = (cur_rank + 1) * 1. / m_num_fro;
				last_bounds = cur_bounds;
				cur_bounds.clear();
				num_fro_node = 0;
			}
		}
		shuffle_data();
	}

	benchmark3::benchmark3(int num_sol, int num_obj, int num_fro, double rand_seed) : fixed_num_front("benchmark3", num_sol, num_obj, num_fro, rand_seed) {}

	void benchmark3::update_data()
	{
		m_data.clear();
		m_data.reserve(m_num_sol);
		std::vector<int> num_each_fro(m_num_fro);

		double temp(0);
		for (int k = 1; k < m_num_fro + 1; ++k)
			temp += pow(k, (m_num_obj - 1));
		int rest(m_num_sol);
		for (int k = 1; k < m_num_fro; ++k) {
			rest -= num_each_fro[m_num_fro - k] = static_cast<int>(pow(k, (m_num_obj - 1)) * m_num_sol / temp) + 1;
		}
		num_each_fro[0] = rest;

		int cur_rank = 0;
		int num_node = 0;
		int num_fro_node = 0;
		double radius = (m_num_fro + 1 - cur_rank) * 1. / (m_num_fro + 1);
		std::vector<std::vector<double>> last_bounds;
		std::vector<std::vector<double>> cur_bounds;
		last_bounds.push_back(std::vector<double>(m_num_obj, 0.));
		while (num_node < m_num_sol) {
			// randomly generate a node
			std::vector<double> new_node(m_num_obj);
			for (int i = 0; i < m_num_obj; ++i)
				new_node[i] = m_rand.next_non_standard(0., 1.);
			// stretch it
			double lenth(0);
			for (double x : new_node)
				lenth += pow((1. - x), 2);
			lenth = sqrt(lenth);
			for (double& x : new_node)
				x = 1. - (1. - x) / lenth * radius;
			// check node's validity
			bool flag1(false);
			for (int i = 0; i < last_bounds.size(); ++i) {
				bool flag2(true);
				for (int j = 0; j < m_num_obj; ++j) {
					if (new_node[j] < last_bounds[i][j]) {
						flag2 = false;
						break;
					}
				}
				if (flag2) {
					flag1 = true;
					break;
				}
			}
			if (flag1) {
				m_data.push_back(new_node);
				cur_bounds.push_back(new_node);
				num_node++;
				num_fro_node++;
			}
			//***********
			if (num_fro_node == num_each_fro[cur_rank]) {
				cur_rank++;
				radius = (m_num_fro + 1 - cur_rank) * 1. / (m_num_fro + 1);
				last_bounds = cur_bounds;
				cur_bounds.clear();
				num_fro_node = 0;
			}
		}
		shuffle_data();
	}

	benchmark4::benchmark4(int num_sol, int num_obj, int num_fro, double rank_seed) : fixed_num_front("benchmark4", num_sol, num_obj, num_fro, rank_seed) {}

	void benchmark4::update_data()
	{
		m_data.clear();
		m_data.reserve(m_num_sol);
		std::vector<int> num_each_fro(m_num_fro);
		double temp(0);
		for (int k = 1; k < m_num_fro + 1; ++k)
			temp += pow(k, (m_num_obj - 1));
		int rest(m_num_sol);
		for (int k = 1; k < m_num_fro; ++k) {
			rest -= num_each_fro[k - 1] = m_num_sol/m_num_fro;
		}
		num_each_fro[m_num_fro - 1] = rest;
		int cur_rank = 0;
		int num_node = 0;
		int num_fro_node = 0;
		double radius = (cur_rank + 1) * 1. / m_num_fro;
		std::vector<std::vector<double>> last_bounds;
		std::vector<std::vector<double>> cur_bounds;
		last_bounds.push_back(std::vector<double>(m_num_obj, 0.));
		while (num_node < m_num_sol) {
			// generate a new node
			std::vector<double> new_node(m_num_obj);
			std::vector<double>& lower_bound = last_bounds[m_rand.next_non_standard(static_cast<std::size_t>(0), last_bounds.size())];
			std::vector<double> upper_bound(m_num_obj);
			for (int i = 0; i < m_num_obj; ++i) {
				double pow_sum(0);
				for (int j = 0; j < m_num_obj; ++j) {
					if (j != i)
						pow_sum += pow(lower_bound[j], 2);
				}
				upper_bound[i] = sqrt(pow(radius, 2) - pow_sum);
			}
			for (int i = 0; i < m_num_obj; ++i)
				new_node[i] = m_rand.next_non_standard(lower_bound[i], upper_bound[i]);
			// stretch it
			double lenth(0);
			for (double x : new_node)
				lenth += pow(x, 2);
			lenth = sqrt(lenth);
			for (double& x : new_node)
				x = x / lenth * radius;
			// check node's validity
			bool flag_valid(true);
			for (int i = 0; i < m_num_obj; ++i) {
				if (new_node[i] < lower_bound[i] || new_node[i] > upper_bound[i]) {
					flag_valid = false;
					break;
				}
			}
			if (flag_valid) {
				m_data.push_back(new_node);
				cur_bounds.push_back(new_node);
				num_node++;
				num_fro_node++;
			}
			//***********
			if (num_fro_node == num_each_fro[cur_rank]) {
				cur_rank++;
				radius = (cur_rank + 1) * 1. / m_num_fro;
				last_bounds = cur_bounds;
				cur_bounds.clear();
				num_fro_node = 0;
			}
		}
		shuffle_data();
	}

	benchmark5::benchmark5(int num_sol, int num_obj, int num_fro, double rank_seed) : fixed_num_front("benchmark5", num_sol, num_obj, num_fro, rank_seed) {}

	void benchmark5::update_data()
	{
		m_data.clear();
		m_data.reserve(m_num_sol);
		std::vector<int> num_each_fro(m_num_fro);
		double temp(0);
		for (int k = 1; k < m_num_fro + 1; ++k)
			temp += pow(k, (m_num_obj - 1));
		int rest(m_num_sol);
		//for (int k = 1; k < m_num_fro; ++k) {
		//	rest -= num_each_fro[m_num_fro - k] = static_cast<int>(pow(k, (m_num_obj - 1)) * m_num_sol / temp) + 1;
		//}
		for (int k = 1; k < m_num_fro; ++k) {
			rest -= num_each_fro[m_num_fro - k] = m_num_sol/m_num_fro;
		}
		num_each_fro[0] = rest;
		int cur_rank = 0;
		int num_node = 0;
		int num_fro_node = 0;
		double radius = (m_num_fro + 1 - cur_rank) * 1. / (m_num_fro + 1);
		std::vector<std::vector<double>> last_bounds;
		std::vector<std::vector<double>> cur_bounds;
		last_bounds.push_back(std::vector<double>(m_num_obj, 0.));
		while (num_node < m_num_sol) {
			// generate a new node
			std::vector<double> new_node(m_num_obj);
			std::vector<double>& lower_bound = last_bounds[m_rand.next_non_standard(static_cast<std::size_t>(0), last_bounds.size())];
			std::vector<double> upper_bound(m_num_obj);
			for (int i = 0; i < m_num_obj; ++i) {
				double pow_sum(0);
				for (int j = 0; j < m_num_obj; ++j) {
					if (j != i)
						pow_sum += pow((1. - lower_bound[j]), 2);
				}
				double temp_upper_pow = pow(radius, 2) - pow_sum;
				if (temp_upper_pow > 0)
					upper_bound[i] = 1. - sqrt(temp_upper_pow);
				else
					upper_bound[i] = 1.;
			}
			for (int i = 0; i < m_num_obj; ++i)
				new_node[i] = m_rand.next_non_standard(lower_bound[i], upper_bound[i]);
			// stretch it
			double lenth(0);
			for (double x : new_node)
				lenth += pow((1. - x), 2);
			lenth = sqrt(lenth);
			for (double& x : new_node)
				x = 1. - (1. - x) / lenth * radius;
			// check node's validity
			bool flag_valid(true);
			for (int i = 0; i < m_num_obj; ++i) {
				if (new_node[i] < lower_bound[i] || new_node[i] > upper_bound[i]) {
					flag_valid = false;
					break;
				}
			}
			if (flag_valid) {
				m_data.push_back(new_node);
				cur_bounds.push_back(new_node);
				num_node++;
				num_fro_node++;
			}
			//***********
			if (num_fro_node == num_each_fro[cur_rank]) {
				cur_rank++;
				radius = (m_num_fro + 1 - cur_rank) * 1. / (m_num_fro + 1);
				last_bounds = cur_bounds;
				cur_bounds.clear();
				num_fro_node = 0;
			}
		}
		shuffle_data();
	}

}
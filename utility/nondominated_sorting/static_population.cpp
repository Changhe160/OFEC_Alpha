#include "static_population.h"
#include <list>
#include <fstream>

namespace NDS {
	std::vector<std::vector<double>> uniform_population::generate_new(const int rank_num)
	{
		std::vector<std::vector<double>> data;
		data.reserve(m_pop_size);
		int cur_rank = 1;
		int num_node = 0;
		std::list<std::vector<double>> last_bounds;
		std::list<std::vector<double>> cur_bounds;
		last_bounds.push_back(std::vector<double>(m_obj_num, 0.));
		while (num_node < m_pop_size) {
			OFEC::Vector new_node(m_obj_num);
			new_node.randomize_on_sphere(cur_rank * 1. / rank_num, &m_rand);
			bool flag_valid(false);
			for (const auto& bound : last_bounds) {
				bool flag_valid2(true);
				for (int i = 0; i < m_obj_num; ++i)
					if (new_node.data()[i] < bound[i]) {
						flag_valid2 = false;
						break;
					}
				if (flag_valid2) {
					flag_valid = true;
					break;
				}
			}
			if (flag_valid) {
				data.push_back(new_node.data());
				cur_bounds.push_back(new_node.data());
			}
			else
				continue;
			num_node++;
			if (cur_rank < rank_num && num_node == cur_rank * m_pop_size / rank_num) {
				cur_rank++;
				last_bounds = cur_bounds;
				cur_bounds.clear();
			}
		}
		return data;
	}
	void uniform_population::generate_output(const int rank_num) {
		std::vector<std::vector<double>> data = generate_new(rank_num);
		std::ofstream outputfile("E://uniform_population_2_30_3.csv");
		for (auto& row : data)
			outputfile << row[0] << ", " << row[1] << std::endl;
		outputfile.close();
	}
}
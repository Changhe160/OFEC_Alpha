#include <fstream>
#include "measure.h"

namespace OFEC {
	std::unique_ptr<measure> measure::ms_measure(nullptr);
	void measure::output() {
		std::stringstream os;
		os << global::ms_arg[param_workingDir] << "Test_result/";
		os << global::ms_arg[param_proName] << "(" << global::ms_arg[param_numDim] << "_" << global::ms_arg[param_numObj] << ")_";
		os << global::ms_arg[param_algName] << "(" << global::ms_arg[param_popSize] << ").txt";
		std::ofstream out(os.str());
		std::vector<double> sum;
		size_t max = mvv_data[0].size();
		size_t row_size = m_headers.size();
		for (size_t i = 1; i < mvv_data.size(); ++i)
			if (max < mvv_data[i].size())
				max = mvv_data[i].size();
		for (size_t i = 0; i < mvv_data.size(); ++i) {
			if (mvv_data[i].size() < max) {
				size_t size = mvv_data[i].size();
				std::vector<double> temp_row;
				for (size_t j = row_size; j > 0; --j)
					temp_row.push_back(mvv_data[i][size - j]);
				for (size_t j = size; j < max; j += row_size)
					for (auto value : temp_row)
						mvv_data[i].push_back(value);
			}
		}
		sum.resize(max, 0);
		for (size_t i = 0; i < max - 1; i += row_size) {
			for (size_t j = 0; j < mvv_data.size(); ++j)
				for (size_t k = i; k < i + row_size; ++k)
					sum[k] += mvv_data[j][k];
			for (size_t k = i; k < i + row_size; ++k)
				sum[k] /= mvv_data.size();
		}
		for (auto header : m_headers)
			if (header != m_headers.back())
				out << header << " ";
			else
				out << header << std::endl;
		for (size_t i = 0; i < max - row_size + 1; i += row_size) {
			for (size_t j = i; j < i + row_size - 1; ++j)
				out << sum[j] << " ";
			out << sum[i + row_size - 1] << std::endl;
		}
		out.close();
		out.clear();
	}
}
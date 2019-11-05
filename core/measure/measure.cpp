#include <fstream>
#include "measure.h"
#include <algorithm>
#include <cmath>

namespace OFEC {
	std::unique_ptr<measure> measure::ms_measure(nullptr);
	void measure::set_filename() {
		m_filename.str("");
		if (global::ms_arg.find("workingDir") == global::ms_arg.end())
			m_filename << "./result/";
		else
			m_filename << global::ms_arg.at("workingDir") << "result/";
		std::vector<std::string> abbr_args;
		for (auto& i : global::ms_arg) {
			if (global::ms_filename_info.at(i.first)) {
				for (auto& j : global::ms_param) {
					if (i.first == j.second) {
						abbr_args.push_back(j.first);
						break;
					}
				}
			}
		}
		std::sort(abbr_args.begin(), abbr_args.end());
		for (const auto& arg : abbr_args) {
			for (auto& i : global::ms_arg) {
				if (i.first == global::ms_param[arg]) {
					m_filename << arg << "(" << i.second << ")_";
					break;
				}
			}
		}
	}
	void measure::initialize(int numRum, const std::vector<std::string> &heading) {
		if (ms_measure) return;
		ms_measure.reset(new measure(numRum));
		ms_measure->set_heading(heading);
	}
	void measure::output_progr() {
		if (mvv_data.empty())
			throw myexcept("No data recorded");
		if (m_filename.str().empty())
			set_filename();
		std::stringstream os;
		os << m_filename.str();
		os << "progr.txt";
		std::ofstream out(os.str());
		std::vector<real> sum;
		size_t max = mvv_data[0].size();
		size_t size_row = m_heading.size();
		for (size_t i = 1; i < mvv_data.size(); ++i)
			if (max < mvv_data[i].size())
				max = mvv_data[i].size();
		for (auto & row : mvv_data) {
			if (row.size() < max) {
				size_t size = row.size();
				std::vector<real> temp_row;
				for (size_t j = size_row; j > 0; --j)
					temp_row.push_back(row[size - j]);
				for (size_t j = size; j < max; j += size_row)
					for (auto value : temp_row)
						row.push_back(value);
			}
		}
		sum.resize(max, 0);
		for (size_t i = 0; i < max - 1; i += size_row) {
			for (size_t j = 0; j < mvv_data.size(); ++j)
				for (size_t k = i; k < i + size_row; ++k)
					sum[k] += mvv_data[j][k];
			for (size_t k = i; k < i + size_row; ++k)
				sum[k] /= mvv_data.size();
		}
		// output heading
		for (size_t i = 0; i < size_row - 1; i++)
			out << m_heading[i] << " ";
		out << m_heading[size_row - 1] << std::endl;
		// output data
		for (size_t i = 0; i < max - size_row + 1; i += size_row) {
			for (size_t j = i; j < i + size_row - 1; ++j)
				out << sum[j] << " ";
			out << sum[i + size_row - 1] << std::endl;
		}
		out.close();
		out.clear();
	}
	void measure::output_final() {
		if (mvv_data.empty())
			throw myexcept("No data recorded");
		if (m_filename.str().empty())
			set_filename();
		std::stringstream os;
		os << m_filename.str();
		os << "final.txt";
		std::ofstream out(os.str());
		size_t size_row = m_heading.size();
		// intercept the last record
		std::vector<std::vector<real>> final_data(mvv_data.size());
		for (size_t i = 0; i < mvv_data.size(); ++i) {
			for (size_t j = size_row; j > 0; --j) {
				final_data[i].push_back(mvv_data[i][mvv_data[i].size() - j]);
			}
		}
		std::vector<std::pair<real, real>> mean_and_var(size_row, {0,0});
		// calculate mean
		for (size_t j = 0; j < size_row; ++j) {
			for (size_t i = 0; i < final_data.size(); i++) {
				mean_and_var[j].first += final_data[i][j];
			}
			mean_and_var[j].first /= final_data.size();
		}
		// calculate variance
		for (size_t j = 0; j < size_row; ++j) {
			for (size_t i = 0; i < final_data.size(); i++) {
				mean_and_var[j].second += pow((final_data[i][j] - mean_and_var[j].first), 2);
			}
			mean_and_var[j].second = sqrt(mean_and_var[j].second / static_cast<real>(final_data.size() - 1));
		}
		//output mean and variance
		for (size_t j = 0; j < size_row; ++j) {
			out << "Mean of " << m_heading[j] << ": " << mean_and_var[j].first << std::endl;
			out << "St.D. of " << m_heading[j] << ": " << mean_and_var[j].second << std::endl;
		}
		out.close();
		out.clear();
	}
}
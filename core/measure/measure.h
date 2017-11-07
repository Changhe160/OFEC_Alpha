#ifndef MEASURE_H
#define MEASURE_H

#include <vector>
#include <string>
#include <memory>

#include "../global.h"

namespace OFEC {
	struct global;
	class measure {
	public:
		static std::unique_ptr<measure> ms_measure;
	private:
		std::vector<std::vector<double>> mvv_data;
		std::vector<std::string> m_headers;
	private:
		void input_value(const int runID) {}
		template<typename T, typename... Args>
		void input_value(const int runID, T value, Args... args);
	public:
		measure(int numRum, std::vector<std::string>& headers):mvv_data(numRum),m_headers(headers){}
		template<typename... Args>
		void record(global *glob, Args... args);
		void output();
	};

	template<typename T, typename ...Args>
	void measure::input_value(const int runID, T value, Args ...args) {
		mvv_data[runID].push_back(value);
		input_value(runID, args...);
	}
	template<typename ...Args>
	void measure::record(global * glob, Args ...args) {
		if (sizeof...(args) != m_headers.size())
			throw myexcept("sample data size was wrong");
		int evals = glob->m_problem->total_evaluations();
		if (evals % (int)global::ms_arg[param_sampleFre] == 0) {
			int runID = glob->m_runID;
			input_value(runID, args...);
		}
	}
}

#endif // !MEASURE_H


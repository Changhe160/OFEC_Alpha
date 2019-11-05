#ifndef MEASURE_H
#define MEASURE_H

#include <vector>
#include <string>
#include <memory>

#include "../global.h"
#include "../../utility/types.h"
namespace OFEC {
	struct global;
	class measure:public singleton {
	protected:
		std::stringstream m_filename;
		std::vector<std::vector<real>> mvv_data;
		std::vector<std::string> m_heading;
		static std::unique_ptr<measure> ms_measure;
	protected:
		measure(int numRum) : mvv_data(numRum){}
		void input_value(const int runID) {}
		template<typename T, typename... Args>
		void input_value(const int runID, const T& value, const Args&... args);
		void set_filename();
	public:
		static measure* get_measure() { return ms_measure.get(); }
		static void initialize(int numRum, const std::vector<std::string> &heading);
		void set_heading(const std::vector<std::string> &heading) { 
			m_heading = heading; 
		}
		template<typename... Args>
		void record(global *glob, const Args&... args);
		void output_progr();
		virtual void output_final();
	};

	template<typename T, typename ...Args>
	void measure::input_value(const int runID, const T& value, const Args& ...args) {
		mvv_data[runID].push_back((real)value);
		input_value(runID, args...);
	}

	// The format is "Evaluations", "XX", "XX" 
	template<typename ...Args>
	void measure::record(global * glob, const Args& ...args) {
		if (sizeof...(args) != m_heading.size())
			throw myexcept("The size of Data being recorded is not equal to the size of Heading preset");
		input_value(glob->m_runID, args...);
	}
}

#endif // !MEASURE_H

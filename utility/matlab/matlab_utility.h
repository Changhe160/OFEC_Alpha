#ifndef OFEC_MATLAB_UTILITY_H
#define OFEC_MATLAB_UTILITY_H

#include <vector>
#include <iostream>

namespace ofec {
	namespace matlab {
		
		template<typename T>
		void outputMatrix(std::ofstream& out,const std::vector<std::vector<T>>& data) {
			if (data.empty()) {
				out << "0\t0" << std::endl;
			}
			else {
				out << data.front().size() << "\t" << data.size() << std::endl;
				for (auto& it : data) {
					for (auto& it2 : it) {
						out << it2 << "\t";
					}
					out << std::endl;
				}
				out << std::endl;
			}
		}


		template<typename T>
		void outputVector(std::ofstream& out,const std::vector<T>& data) {
			if (data.empty()) {
				out << "0\t0" << std::endl;
			}
			else {
				out << 1 << "\t" << data.size() << std::endl;
				for (auto& it : data) {
					out << it << std::endl;
				}
				out << std::endl;
			}
		}


	}
}



#endif
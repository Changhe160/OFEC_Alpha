
/******************************************************************************
* Project:Open Frameworks for Evolutionary Computation (OFEC)
*******************************************************************************
* Author: Yiya Diao
* Email: diaoyiyacug@gmail.com
* Language: C++
*-------------------------------------------------------------------------------
*  This file is part of OFEC. This library is free software;
*  you can redistribute it and/or modify it under the terms of the
*  GNU General Public License as published by the Free Software
*  Foundation; either version 2, or (at your option) any later version.
*
*  see https://github.com/Changhe160/OFEC for more information
*
*-------------------------------------------------------------------------------
* functions related to file output and input, and string
*
*********************************************************************************/

#ifndef OFEC_STREAM_FINCTIONAL_H
#define OFEC_STREAM_FINCTIONAL_H

#include<string>
#include<vector>
#include<functional>
#include<array>
#include<thread>
#include<sstream>
#include <map>
#include <fstream>
#include "../../core/random/newran.h"
#include "../../core/problem/solution.h"
#include "../../core/environment/environment.h"
#include <filesystem>

namespace ofec::utility {


	struct Str64ToDec {
		std::vector<char> m_base64_map = { 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
					 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
					 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
					 'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/' };

		std::map<char, int> m_char2int;

		void init() {
			for (int idx(0); idx < m_base64_map.size(); ++idx) {
				m_char2int[m_base64_map[idx]] = idx;
			}
		}
		int string64ToDec(const std::string& number) {
			int numDec(0);
			for (auto& it : number) {
				numDec = numDec * 64 + m_char2int[it];
			}
			return numDec;
		}

	};


	extern int  HexadecimalStrToInt(const std::string& name, int digitNums = 4);

	template<typename T>
	void assignVVector(std::vector<std::vector<T>>& mat, const std::vector<int>& matSize, const T& initValue) {
		mat.resize(matSize.size());
		for (int idx(0); idx < matSize.size(); ++idx) {
			mat[idx].resize(matSize[idx]);
			std::fill(mat[idx].begin(), mat[idx].end(), initValue);
		}
	}


	template<typename T>
	void assignVVector(std::vector<std::vector<T>>& mat, const T& initValue) {
		for (auto& it : mat) {
			for (auto& it2 : it) {
				it2 = initValue;
			}
		}
	}


	template<typename T>
	void resizeVVector(std::vector<std::vector<T>>& mat, const std::vector<int>& matSize, const T& initValue) {
		mat.resize(matSize.size());
		for (int idx(0); idx < matSize.size(); ++idx) {
			mat[idx].resize(matSize[idx], initValue);
		}
	}

	template<typename T>
	void vectorToStream(const std::vector<T>& data, std::ostream& out) {
		out << 1 << "\t" << data.size() << std::endl;
		for (auto& a : data) {
			out << a << "\t";
		}
		out << std::endl;
	}

	template<typename T>
	void vectorToStream(const std::vector<T>& data, std::stringstream& out) {
		out << 1 << "\t" << data.size() << std::endl;
		for (const auto& it : data) {
			out << it << "\t";
		}
		out << std::endl;
	}

	template<typename T>
	void streamToVector(std::vector<T>& data, std::stringstream& in) {
		int id(0), vsize(0);
		in >> id >> vsize;
		data.resize(vsize);
		for (auto& it : data) {
			in >> it;
		}
	}




	template<typename T>
	void vvectorToStream(const std::vector<std::vector<T>>& data, std::ostream& out) {
		if (data.empty())return;
		out << data.size() << "\t" << data.front().size() << std::endl;
		for (auto& it : data) {
			for (auto& it2 : it) {
				out << it2 << "\t";
			}
			out << std::endl;
		}
	}


	template<typename T>
	void vvectorToStream(const std::vector<std::vector<T>>& data, std::stringstream& out) {
		if (data.empty())return;
		out << data.size() << "\t" << data.front().size() << std::endl;
		for (auto& it : data) {
			for (auto& it2 : it) {
				out << it2 << "\t";
			}
			out << std::endl;
		}
	}

	template<typename T>
	void streamToVvector(std::vector<std::vector<T>>& data, std::stringstream& in) {
		int vsize(0), vvsize(0);
		in >> vsize >> vvsize;
		data.resize(vsize);
		for (auto& it : data) {
			it.resize(vvsize);
			for (auto& it2 : it) {
				in >> it2;
			}
		}
	}


	// function 
	extern std::vector<std::string> split(const std::string& input,
		const std::string& regex);
	// get current system time
	extern std::string getCurrentSystemTime();
	extern std::string toFixIntStr(int number, int fixNum = 5);
	extern std::string createRandomString(size_t length, ofec::Random* rnd);

	 

	extern bool endsWith(const std::string& str, const std::string& suffix);
	extern std::string removeSuffix(const std::string& str, const std::string& suffix);
	extern bool file_exists_with_prefix(std::vector<std::filesystem::path>& results, const std::string& savedir,
		const std::string& prefix);
	extern std::vector<std::string> getFilesNamesWithExtension(const std::string& directory, const std::string& extension);





	extern bool is_number(const std::string& s);
	extern void cutStringEndl(const std::stringstream& buffer, std::vector<size_t>& cut, int cutSize);
	extern void readFileBuffer(const std::string& filepath, std::stringstream& buf);
	extern void readCSVdata(const std::string& filepath, std::vector<std::vector<std::string>>& data);




	extern void outputSolutions(const std::string& saveDir, const std::string& filename, std::vector<ofec::SolutionBase*>& sols,
		ofec::Environment* env
	);
	extern void inputSolutions(const std::string& saveDir, const std::string& filename, std::vector<std::shared_ptr<ofec::SolutionBase>>& sols,
		ofec::Environment* env);

}


#endif
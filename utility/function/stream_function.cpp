#include "stream_function.h"
#include <regex>
#include <string>
#include "../../core/parameter/variants_to_stream.h"

namespace ofec::utility {


	int HexadecimalStrToInt(const std::string& name, int digits)
	{
		int x;
		std::string val("0x");
		val += name.substr(name.size() - digits, name.size());
		std::stringstream ss;
		ss << std::hex << val;
		ss >> x;
		return x;
	}
	
	std::vector<std::string> split(const std::string& input, const std::string& regex)
	{
		std::regex re(regex);
		std::sregex_token_iterator first{ input.begin(), input.end(), re, -1 }, last;
		return { first, last };
	}

	std::string getCurrentSystemTime()
	{
		auto tt = std::chrono::system_clock::to_time_t
		(std::chrono::system_clock::now());
		struct tm* ptm = localtime(&tt);
		char date[60] = { 0 };
		sprintf(date, "%d-%02d-%02d-%02d-%02d-%02d",
			(int)ptm->tm_year + 1900, (int)ptm->tm_mon + 1, (int)ptm->tm_mday,
			(int)ptm->tm_hour, (int)ptm->tm_min, (int)ptm->tm_sec);
		return std::string(date);
	}

	std::string toFixIntStr(int number, int fixNum) {
		std::stringstream ss;
		ss << std::setw(fixNum) << std::setfill('0') << number;
		return std::move(ss.str());
	}
	std::string createRandomString(size_t length, ofec::Random* rnd) {

		// 定义可能的字符集合
		const char charset[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
		const size_t charset_size = sizeof(charset) - 1; // 不包括结尾的空字符


		std::string random_string;

		for (size_t i = 0; i < length; ++i) {
			// 生成随机索引并选择字符
			random_string += charset[rnd->uniform.nextNonStd<int>(0, charset_size - 1)];
		}

		return random_string;
	}




	bool endsWith(const std::string& str, const std::string& suffix) {
		if (str.length() < suffix.length()) {
			return false;
		}
		return str.compare(str.length() - suffix.length(), suffix.length(), suffix) == 0;
	}


	bool file_exists_with_prefix(std::vector<std::filesystem::path>& results, const std::string& savedir,
		const std::string& prefix) {
		namespace fs = std::filesystem;
		std::error_code ec;
		results.clear();
		// 使用递归目录迭代器遍历当前目录及其子目录
		for (const auto& entry : fs::recursive_directory_iterator(savedir)) {
			if (entry.is_regular_file() && entry.path().filename().string().rfind(prefix, 0) == 0) {
				results.push_back(entry.path().filename());
			}
		}
		// 如果结果不为空，说明找到了至少一个符合条件的文件
		return !results.empty();
	}

	std::vector<std::string> getFilesNamesWithExtension(const std::string& directory, const std::string& extension) {
		namespace fs = std::filesystem;
		std::vector<std::string> files;
		for (const auto& entry : fs::directory_iterator(directory)) {
			if (fs::is_regular_file(entry) && endsWith(entry.path().string(), extension)) {
				files.push_back(entry.path().filename().string());
			}
		}
		return files;
	}

	std::string removeSuffix(const std::string& str, const std::string& suffix) {
		//std::string suffix = "_solutions.txt";
		size_t pos = str.find(suffix);
		if (pos != std::string::npos) {
			return str.substr(0, pos);
		}
		return str;
	}



	void readFileBuffer(const std::string& filepath, std::stringstream& buf) {
		std::ifstream fin(filepath);
		buf << fin.rdbuf();
		fin.close();
	}



	void readCSVdata(const std::string& filepath, std::vector<std::vector<std::string>>& content) {
		using namespace std;
		content.clear();
		vector<string> row;
		string line, word;

		fstream file(filepath, ios::in);
		if (file.is_open())
		{
			while (getline(file, line))
			{
				row.clear();

				stringstream str(line);

				while (getline(str, word, ','))
					row.push_back(word);
				content.push_back(row);
			}
		}
		else
			cout << "Could not open the file\n";

		file.close();
	}


	bool is_number(const std::string& s)
	{
		std::string::const_iterator it = s.begin();
		while (it != s.end() && std::isdigit(*it)) ++it;
		return !s.empty() && it == s.end();
	}

	void cutStringEndl(const std::stringstream& buffer, std::vector<size_t>& cut, int cutSize) {
		using namespace std;
		std::string info = buffer.str();
		//buffer.flush();
		string line;
		size_t from = 0;
		size_t to = info.find_first_of('\n', from);
		line = info.substr(from, to);
		from = to + 1;
		//total.push_back(header1.substr(from + 1, to));

		int total(0);
		int bufferSize = cutSize;
		cut.clear();
		//std::vector<size_t> cut;
		cut.push_back(to);
		int avg_cut = (info.size() - to) / cutSize;
		from = avg_cut + to;
		to = info.find_first_of('\n', from);

		while (to <= info.size()) {
			cut.push_back(to);
			from = to + avg_cut;
			if (from >= info.size()) {
				cut.push_back(info.size());
				break;
			}
			to = info.find_first_of('\n', from);
			if (to > info.size()) {
				cut.push_back(info.size());
				break;
			}
		}
	}





	void outputSolutions(const std::string& saveDir, const std::string& filename, std::vector<ofec::SolutionBase*>& sols,
		ofec::Environment* env
	) {
		{
			ofec::ParameterVariantStream paramsStream;

			paramsStream << sols.size();
			for (auto& it : sols) {
				env->problem()->solutionToParameterVariants(*it, paramsStream);
			}
			ofec::variants_stream::outputToFile(paramsStream, saveDir + filename + "_solutions.txt");
		}
	}

	void inputSolutions(const std::string& saveDir, const std::string& filename, std::vector<std::shared_ptr<ofec::SolutionBase>>& sols,
		ofec::Environment* env) {

		ofec::ParameterVariantStream paramsStream;
		ofec::variants_stream::inputFromFile(paramsStream, saveDir + filename + "_solutions.txt");
		size_t solSize;
		paramsStream >> solSize;
		sols.resize(solSize);
		for (auto& it : sols) {
			it.reset(env->problem()->createSolution());
			env->problem()->parameterVariantsToSolution(paramsStream, *it);
		}

	}


}
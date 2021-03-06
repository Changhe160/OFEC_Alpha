#include "system_initialization.h"
#include "user_initialization.h"
#include "../core/global.h"
#include <thread>

namespace OFEC {
	std::mutex g_mutexStream;

	void set_global_parameters(int argn, char * argv[]) {
		//format of argument list from main: param_proName=FUN_Sphere param_numDim=5  param_numPeak=10
		register_parameter(); //register all system parameters first
		std::string letter;
		for (auto i = 'a'; i <= 'z'; i++) {
			letter += i;
			letter += static_cast<char>(i - 32);
		}
		std::string remove = "\r\t\n\b\v";
		letter += "\\/:";
		for (int i = 1; i < argn; i++) {
			std::string par = argv[i];
			while (size_t pos = par.find_first_of(remove)) {
				if (std::string::npos == pos) break;
				par.erase(par.begin() + pos);
			}
			size_t pos = par.find('=');
			if (pos == std::string::npos)  throw myexcept("Invalid argument:gSetGlobalParameters()");
			std::string value = par.substr(pos + 1, par.size() - 1), name = par.substr(0, pos);
			if (global::ms_param.find(name) == global::ms_param.end()) 
				throw myexcept("Invalid argument @set_global_parameters()");
			if (value == "true") {
				global::ms_arg[global::ms_param[name]] = true;
			}
			else if (value == "false") {
				global::ms_arg[global::ms_param[name]] = false;
			}
			else if (value.find_first_of(letter) != std::string::npos) {
				if (value.size() == 1)
					global::ms_arg[global::ms_param[name]] = value[0];
				else
					global::ms_arg[global::ms_param[name]] = value;
			}
			else if (std::string::npos != par.find('.', pos + 1)) {
				real val = atof(value.c_str());
				global::ms_arg[global::ms_param[name]] = val;
			}
			else {
				int val = atoi(value.c_str());
				global::ms_arg[global::ms_param[name]] = val;
			}
		}
		if (global::ms_arg.find("sampleFre") != global::ms_arg.end()) global::ms_sample_fre = global::ms_arg.at("sampleFre");
		if (global::ms_arg.find("workingDir") == global::ms_arg.end())  global::ms_arg.insert({ "workingDir", "./" });
		if (global::ms_arg.find("numRun") == global::ms_arg.end())  global::ms_arg.insert({ "numRun", 1 });
		if (global::ms_arg.find("dataFile1") != global::ms_arg.end())
		{
			std::string str = global::ms_arg.at("dataFile1");
			size_t i;
			if ((i = str.find(".tsp")) == std::string::npos) return;
			str.erase(i, 4);
			for (i = str.size() - 1; i >= 0; i--)
			{
				if (str[i] >= '0'&&str[i] <= '9')
					continue;
				else break;
			}
			std::string s(str, i + 1);
			if (global::ms_arg.find("numDim") == global::ms_arg.end())
				global::ms_arg.insert({ "numDim", atoi(s.c_str()) });
			if (global::ms_arg.find("popSize") == global::ms_arg.end())
				global::ms_arg.insert({ "popSize", atoi(s.c_str()) });
		}
	}

	void run() {
		before_run();
		int numTask;
		if (global::ms_arg.find("numTask") != global::ms_arg.end())
			numTask = global::ms_arg.at("numTask");
		else
			numTask = std::thread::hardware_concurrency();
		if (numTask > static_cast<int>(global::ms_arg.at("numRun")))
			numTask = global::ms_arg.at("numRun");
		std::vector<std::thread> atrd;
		int rest = static_cast<int>(global::ms_arg.at("numRun")) % numTask;
		int id1 = 0, id2 = id1 + static_cast<int>(global::ms_arg.at("numRun")) / numTask - 1 + (rest-- > 0 ? 1 : 0);
		for (size_t i = 0; i < numTask; i++) {
			std::vector<int> runs;
			for (int r = id1; r <= id2; r++) runs.push_back(r);
			id1 = id2 + 1;
			id2 = id1 + static_cast<int>(global::ms_arg.at("numRun")) / numTask - 1 + (rest-- > 0 ? 1 : 0);
			atrd.push_back(std::thread(go, runs));
		}
		for (auto&t : atrd) t.join();
		set_default_filename_info();
		after_run();
	}

	int go(std::vector<int> runIds) {
		for (auto & runId : runIds) {
			try {
				g_mutexStream.lock();
				std::cout << "runId: " << runId << std::endl;
				g_mutexStream.unlock();
				global::ms_global.reset(new global(runId, 1. / 7, (runId + 1.) / ((int)(global::ms_arg.at("numRun")) + 1.)));
				global::ms_global->m_problem.reset(factory<problem>::produce(global::ms_arg.at("proName"), global::ms_arg));
				global::ms_global->m_problem->initialize();
				global::ms_global->m_algorithm.reset(factory<algorithm>::produce(global::ms_arg.at("algName"), global::ms_arg));
				global::ms_global->m_algorithm->initialize();
				global::ms_global->m_algorithm->run();
			}
			catch (std::exception &e) {
				g_mutexStream.lock();
				std::cout << "exception, runId " << runId << ": " << e.what() << std::endl;
				g_mutexStream.unlock();
			}
		}
		return 0;
	}

	void set_default_filename_info() {
		for (auto& i : global::ms_arg) {
			if (i.first == "gOptFlag" || i.first == "algId" || i.first == "proId" || i.first == "flagNoise" || \
				i.first == "flagNumPeakChange" || i.first == "flagTimeLinkage" || i.first == "numRun" || \
				i.first == "numTask" || i.first == "minNumPopSize" || i.first == "hibernatingRadius" || \
				i.first == "solutionValidationMode" || i.first == "evalCountFlag" || \
				i.first == "workingDir" || i.first == "sampleFre" || i.first == "maxEvals" || i.first == "flagNumPeakChange" || \
				i.first == "peakNumChangeMode" || i.first == "dataDirectory1")
				global::ms_filename_info[i.first] = false;
			else
				global::ms_filename_info[i.first] = true;
		}
	}

	void add_filename_info(const std::vector<std::string>& info) {
		for (const std::string& info_name : info) {
			if (global::ms_arg.find(info_name) == global::ms_arg.end())
				throw myexcept("info to add is not included in global::ms_arg");
			global::ms_filename_info[info_name] = true;
		}
	}

	void set_alg4pro() {
		//bind algorithms and problems here
		for (auto &alg : global::ms_reg_algorithm.get()) {
			for (auto &pro : global::ms_reg_problem.get()) {
				for (auto it = pro.second.second.begin();;) {
					if (!alg.second.second.count(*it))
						break;
					if (++it == pro.second.second.end())
					{
						global::ms_alg4pro[alg.first].insert(pro.first);
						break;
					}
				}
			}
		}
	}

	bool check_validation() {
#ifndef OFEC_UNIT_TEST
		if (global::ms_alg4pro.at(global::ms_arg.at("algName")).find(global::ms_arg.at("proName")) == global::ms_alg4pro.at(global::ms_arg.at("algName")).end()) {
			std::cout << "error: algorithm " << global::ms_arg.at("algName") << " is not for problem " << global::ms_arg.at("proName") << std::endl;
			return false;
		}
#endif
		return true;
	}
}
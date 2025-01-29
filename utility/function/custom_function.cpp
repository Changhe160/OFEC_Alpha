#include "custom_function.h"
#include<regex>
#include <chrono>
#include<sstream>
#include <iomanip>

#include "../../core/environment/environment.h"
#include "stream_function.h"





void UTILITY::idx2RealIdxs(int idx,
	std::array<int, 3>& idxs,
	const std::array<int, 3>& divs) {
	int div12 = (divs[1] * divs[2]);
	idxs[0] = idx / (div12);
	int rest = idx % div12;
	idxs[1] = rest / divs[2];
	idxs[2] = rest % divs[2];
}


void UTILITY::assignThreads(int num_samples, int num_task, std::vector<int>& ids)
{
	ids.clear();
	int rest = num_samples % num_task;
	int id1 = 0, id2 = 0;
	std::pair<int, int> from_to;
	ids.push_back(id2);
	for (size_t i = 0; i < num_task; ++i) {
		id1 = id2;
		id2 = id1 + num_samples / num_task + (rest-- > 0 ? 1 : 0);
		ids.push_back(id2);
	}
	
}

size_t UTILITY::getCurSize(double scale, size_t size) {
	return std::max<int>(size - 1, scale * size);
}


bool UTILITY::judgeEqualWithPrecision(double a, double b, double pre) {
	return abs(a - b) < pre;
}





void generateRandomSolutionsNoEvaluteThreadTask(
	std::vector<std::shared_ptr<ofec::SolutionBase>>& sols,
	const std::pair<int, int>& from_to,
	double randSeed, ofec::Environment* env
) {
	using namespace ofec;
	Random rand(randSeed);
	for (int idx(from_to.first); idx < from_to.second; ++idx) {
		sols[idx].reset(env->problem()->createSolution());
		env->problem()->initializeVariables(sols[idx]->variableBase(), &rand);
	}
}



void UTILITY::generateRandomSolutionsMultiThreads(
	std::vector<std::shared_ptr<ofec::SolutionBase>>& samples,
	ofec::Environment* env,
	ofec::Random* rnd
) {
	int num_task = std::thread::hardware_concurrency();
	int num_samples = samples.size();
	std::vector<std::thread> thrds;
	std::vector<int> tasks;
	UTILITY::assignThreads(num_samples, num_task, tasks);
	std::pair<int, int> from_to;
	for (size_t i = 0; i < num_task; ++i) {
		from_to.first = tasks[i];
		from_to.second = tasks[i + 1];
		double randomSeed(rnd->uniform.nextNonStd<double>(0.01, 1.0));
		thrds.push_back(std::thread(
			generateRandomSolutionsNoEvaluteThreadTask,
			std::ref(samples), from_to, randomSeed, env));
	}
	for (auto& thrd : thrds)
		thrd.join();
}


void generateRandomSolutionsThreadTask(
	std::vector<std::shared_ptr<ofec::SolutionBase>>& sols,
	const std::pair<int, int>& from_to,
	double randSeed, ofec::Environment *env,
	const std::function<void(ofec::SolutionBase& sol, ofec::Environment *env)>& eval_fun
) {
	using namespace ofec;
	Random rand(randSeed);
	for (int idx(from_to.first); idx < from_to.second; ++idx) {
		sols[idx].reset(env->problem()->createSolution());
		env->problem()->initializeVariables(sols[idx]->variableBase(), &rand);
		eval_fun(*sols[idx], env);
	}
}


 void UTILITY::generateRandomSolutionsMultiThreads(
	std::vector<std::shared_ptr<ofec::SolutionBase>>& samples,
	ofec::Random *rnd,
	 ofec::Environment *env,
	const std::function<void(ofec::SolutionBase& sol, ofec::Environment *env)>& eval_fun
) {
	 int num_task = std::thread::hardware_concurrency();
	 int num_samples = samples.size();
	 std::vector<std::thread> thrds;
	 std::vector<int> tasks;
	 UTILITY::assignThreads(num_samples, num_task, tasks);
	 std::pair<int, int> from_to;
	 for (size_t i = 0; i < num_task; ++i) {
		 from_to.first = tasks[i];
		 from_to.second = tasks[i + 1];
		 double randomSeed(rnd->uniform.nextNonStd<double>(0.01, 1.0));
		 thrds.push_back(std::thread(
			 generateRandomSolutionsThreadTask,
			 std::ref(samples), from_to, randomSeed, env, eval_fun));
	 }
	 for (auto& thrd : thrds)
		 thrd.join();
}



 void evaluateRandomSolutionsThreadTask(
	 std::vector<ofec::SolutionBase*>& sols,
	 const std::pair<int, int>& from_to,
	 ofec::Environment *env,
	 const std::function<void(ofec::SolutionBase& sol, ofec::Environment *env)>& eval_fun
 ) {
	 using namespace ofec;
	 for (int idx(from_to.first); idx < from_to.second; ++idx) {
		 eval_fun(*sols[idx], env);
	 }
 }

 void UTILITY::evaluateRandomSolutionsMultiThreads(
	 std::vector<ofec::SolutionBase*>& samples,
	 ofec::Environment *env,
	 const std::function<void(ofec::SolutionBase& sol, ofec::Environment *env)>& eval_fun
 ) {
	 int num_task = std::thread::hardware_concurrency();
	 int num_samples = samples.size();
	 std::vector<std::thread> thrds;
	 std::vector<int> tasks;
	 UTILITY::assignThreads(num_samples, num_task, tasks);
	 std::pair<int, int> from_to;
	 for (size_t i = 0; i < num_task; ++i) {
		 from_to.first = tasks[i];
		 from_to.second = tasks[i + 1];
		 thrds.push_back(std::thread(
			 evaluateRandomSolutionsThreadTask,
			 std::ref(samples), from_to, env, eval_fun));
	 }
	 for (auto& thrd : thrds)
		 thrd.join();
 }


 bool UTILITY::waitForFile(const std::string& saveDir, const std::string& filename, const std::string& process_name) {
	 std::vector<std::filesystem::path> results;
	 if (ofec::utility::file_exists_with_prefix(results, saveDir, filename)) {
		 return false;
	 }
	 auto testfilename = filename + process_name + ".dat";
	 {
		 std::ofstream out(saveDir + testfilename);
		 out << "yes" << std::endl;
		 out.close();
	 }
	 // 获取当前时间
	 auto now = std::chrono::steady_clock::now();
	 // 设置2分钟后的时间
	 auto wait_until = now + std::chrono::minutes(2);
	 // 让当前线程等待，直到 wait_until
	 std::this_thread::sleep_until(wait_until);
	 ofec::utility::file_exists_with_prefix(results, saveDir, filename);
	 std::sort(results.begin(), results.end());
	 if (results.front() == testfilename) {
		 return true;
	 }
	 return false;
 }

 void UTILITY::getPrimeNumber(int maxNumber, std::vector<int>& primeNumber) {


	 std::vector<bool> isPrimeNumber(maxNumber + 1, false);
	 primeNumber.clear();
	 primeNumber.push_back(2);


	 for (int curNum(3); curNum < maxNumber; ++curNum) {
		 isPrimeNumber[curNum] = true;
		 for (auto& iterPrime : primeNumber) {
			 if (curNum % iterPrime == 0) {
				 isPrimeNumber[curNum] = false;
				 break;
			 }
		 }

		 if (isPrimeNumber[curNum]) {
			 primeNumber.push_back(curNum);
		 }
	 }
 }


 //void setFileName(std::stringstream& filename,
	// int proFrameId, int algFrameId
 //) {

	// using namespace ofec;
	// filename.str("");
	// filename << std::string(OFEC_DIR) << "/result/";

	// std::string proName = "problem name";
	// std::string algName = "algorithm name";

	// //insert problem and alg para 
	// std::vector<std::string> names = { proName,algName };
	// std::vector<ParamType> types = { ParamType::pro,ParamType::alg };

	// std::vector<std::string> idArgName = { "proFrameId", "algFrameId" };
	// std::vector<int> frameIds = { proFrameId,algFrameId };
	// std::vector<std::string> infos;
	// ParamType cur_para;

	// std::string cur_name;
	// std::vector<std::pair<int, std::string>> output_infos =
	// { {0, "problem name"},{0,"dataFile1"}, {0, "number of variables"}, {1,"algorithm name"} };

	// std::vector<std::string> output_names =
	// { "problem_name", "dataFile1", "number_of_variables","algorithm_name" };

	// for (int idx(0); idx < output_infos.size(); ++idx) {
	//	 auto& it = output_infos[idx];

	//	 auto& cur_name = it.second;
	//	 auto& cur_para = types[it.first];

	//	 if (g_params[cur_para].find(cur_name) != g_params[cur_para].end()) {
	//		 filename << output_names[idx] << "-" << g_params[cur_para][cur_name] << "--";
	//	 }
	// }

	// //for (auto& it : output_infos) {
	// //	auto& cur_name = it.second;
	// //	auto& cur_para = types[it.first];
	// //	
	// //	if (g_params[cur_para].find(cur_name) != g_params[cur_para].end()) {
	// //		filename << cur_name << "_(" << g_params[cur_para][cur_name] << ")_";
	// //	}
	// //}







	// //for(int idx(0);idx<2;++idx){
	// //	infos.clear();
	// //	cur_para = types[idx];
	// //	std::string& cur_name = names[idx];
	// //	if (g_params[cur_para].find(cur_name) == g_params[cur_para].end())continue;
	// //	for (auto& it : g_params[cur_para]) {
	// //		if (it.first != cur_name) {
	// //			infos.push_back(it.first);
	// //		}
	// //	}
	// //	std::sort(infos.begin(), infos.end());
	// //	filename << cur_name << "_(" << g_params[cur_para][cur_name] << ")_";
	// //	for (const auto& arg : infos) {
	// //		filename << arg << "_(" << g_params[cur_para][arg] << ")_";
	// //	}
	// //	filename << idArgName[idx] << "_(" << frameIds[idx] << ")_";
	// //}
 //}


//
//template<typename TSol>
//void UTILITY::generateSamplesThreadTask(
//	std::vector<std::shared_ptr<TSol>>& samples,
//	const std::pair<int, int>& from_to,
//	double randSeed, Problem *pro) {
//
//}
//
//template<typename TSol>
//void UTILITY::generateSamples(
//	std::vector<std::shared_ptr<TSol>>& samples,
//	Random *rnd,
//	Problem *pro) {
//
//}




 std::vector<double> UTILITY::valuesToSortedValues(const std::vector<double>& values) {
	 if (values.empty())return std::vector<double>();
	 std::vector<int> sortedIds(values.size());
	 for (int idx(0); idx < sortedIds.size(); ++idx) {
		 sortedIds[idx] = idx;
	 }

	 std::sort(sortedIds.begin(), sortedIds.end(), [&](int a,int b) {
		 return values[a] < values[b];
		 });

	 auto sortedValue = values;
	 
	 int curSortedId = 0;
	 sortedValue[sortedIds.front()] = curSortedId;
	 for (int idx(1); idx < values.size(); ++idx) {
		 if (values[sortedIds[idx]] != values[sortedIds[idx - 1]]) {
			 ++curSortedId;
		 }
		 sortedValue[sortedIds[idx]] = curSortedId;
	 }
	 
	 return sortedValue;
 }


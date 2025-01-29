#include "variants_to_stream.h"

#include <iostream>
#include <thread>

namespace ofec {
	namespace variants_stream {



		using TupleString = std::tuple<int, int, std::string>;

		struct GlobalThreadInfo {
			int m_precision = 6;
			std::mutex m_info_mtx;
			std::string m_info;
			ParameterVariantStream* m_stream;
			std::list<ParameterVariantStream::Type> m_list;

		};
		struct ThreadLocalInfo {
			//			
		   // std::list<std::string> m_tmpStr;

			std::list<TupleString> m_outputStr;
			std::list<ParameterVariant> m_readVariants;

			int m_threadId = 0;
			int from = 0;
			int to = 0;
		};
		struct CompareTuple
		{
			bool operator() (const TupleString& a, const TupleString& b)
			{
				return std::get<0>(a) > std::get<0>(b);
			}
		};



		template <typename T>
		void outputSingle(std::ostream& os, const std::vector<T>& v)
		{
			os << "[";
			for (int i = 0; i < v.size(); ++i) {
				os << v[i];
				if (i != v.size() - 1)
					os << ", ";
			}
			os << "]\n";
			//		return os;
		}

		template <typename T>
		void outputSingle(std::ostream& os, const T& v)
		{
			os << "[";
			os << v;
			os << "]\n";
		}

		template<typename T, typename... Ts>
		void output(std::ostream& os, const std::variant<T, Ts...>& v)
		{
			std::visit([&os](auto&& arg) {
				outputSingle(os, arg);
				}, v);
		}

		template<typename T>
		void typeT_ToStream(const ofec::ParameterVariant& data,
			std::stringstream& info, int precision = 6) {
			info << "\t" << std::get<T>(data);
		}

		template<>
		void typeT_ToStream<Real>(const ofec::ParameterVariant& data,
			std::stringstream& info, int precision);


		template<typename T>
		void vectorTypeToStream(const ofec::ParameterVariant& data,
			std::stringstream& info, int precision = 6) {
			auto& curdata = std::get<std::vector<T>>(data);
			info << "\t" << curdata.size();
			for (const auto& it : curdata) {
				info << "\t" << it;
			}
		}

		template<>
		void vectorTypeToStream<Real>(const ofec::ParameterVariant& data,
			std::stringstream& info, int precision);


		template<typename T>
		void streamToTypeT(std::stringstream& info, ofec::ParameterVariant& data) {
			T value;
			info >> value;
			data = value;
		}


		template<typename T>
		void streamToVectorTypeT(std::stringstream& info, ofec::ParameterVariant& data) {
			size_t dataSize(0);
			info >> dataSize;
			std::vector<T> values(dataSize);
			for (auto& value : values) {
				info >> value;
			}
			data = values;
		}
		template<>
		void streamToVectorTypeT<bool>(std::stringstream& info, ofec::ParameterVariant& data);




		void parameterVariantToStringstream(const ofec::ParameterVariant& data,
			std::stringstream& info, int precision = 6);
		void stringstreamToParameterVariant(std::stringstream& info,
			ofec::ParameterVariant& data);



		template<>
		void typeT_ToStream<Real>(const ofec::ParameterVariant& data,
			std::stringstream& info, int precision) {
			info << "\t" << std::setprecision(precision) << std::get<Real>(data);
		}



		template<>
		void vectorTypeToStream<Real>(const ofec::ParameterVariant& data,
			std::stringstream& info, int precision) {
			auto& curdata = std::get<std::vector<Real>>(data);
			info << "\t" << curdata.size();
			for (const auto& it : curdata) {
				info << "\t" << std::setprecision(precision) <<  it;
			}
		}


		template<>
		void streamToVectorTypeT<bool>(std::stringstream& info, ofec::ParameterVariant& data) {
			size_t dataSize(0);
			info >> dataSize;
			bool tmp;
			std::vector<bool> values(dataSize);
			for (int idx(0); idx < values.size(); ++idx) {
				info >> tmp;
				values[idx] = tmp;
			}
			data = values;
		}

		void parameterVariantToStringstream(const ofec::ParameterVariant& data,
			std::stringstream& info, int precision ) {
			using namespace ofec;
			auto type = static_cast<ParameterType>(data.index());
			info << data.index();
			switch (type)
			{
			case ParameterType::kBool: {
				typeT_ToStream<bool>(data, info, precision);
				break;
			}
			case ParameterType::kInt: {
				typeT_ToStream<int>(data, info, precision);
				break;
			}
			case ParameterType::kSizeT: {
				typeT_ToStream<size_t>(data, info, precision);
				break;
			}
			case ParameterType::kChar: {
				typeT_ToStream<char>(data, info, precision);
				break;
			}
			case ParameterType::kReal: {
				typeT_ToStream<Real>(data, info, precision);
				break;
			}

			case ParameterType::kVectorBool: {
				vectorTypeToStream<bool>(data, info, precision);
				break;
			}

			case ParameterType::kVectorInt: {
				vectorTypeToStream<int>(data, info, precision);
				break;
			}

			case ParameterType::kVectorSizeT: {
				vectorTypeToStream<size_t>(data, info, precision);
				break;
			}

			case ParameterType::kString: {
				typeT_ToStream<std::string>(data, info, precision);
				break;
			}

			case ParameterType::kVectorReal: {
				vectorTypeToStream<Real>(data, info, precision);
				break;
			}
			default:
				break;
			}
			info << "\n";
		}


		void stringstreamToParameterVariant(std::stringstream& info,
			ofec::ParameterVariant& data) {
			using namespace ofec;
			size_t numType(0);
			info >> numType;
			auto type = static_cast<ofec::ParameterType>(numType);
			switch (type)
			{

			case ParameterType::kBool: {
				streamToTypeT<bool>(info, data);
				break;
			}
			case ParameterType::kInt: {
				streamToTypeT<int>(info, data);
				break;
			}
			case ParameterType::kSizeT: {
				streamToTypeT<size_t>(info, data);
				break;
			}
			case ParameterType::kChar: {

				streamToTypeT<char>(info, data);

				break;
			}
			case ParameterType::kReal: {
				streamToTypeT<Real>(info, data);
				break;
			}

			case ParameterType::kVectorBool: {
				streamToVectorTypeT<bool>(info, data);
				break;
			}

			case ParameterType::kVectorInt: {
				streamToVectorTypeT<int>(info, data);
				break;
			}

			case ParameterType::kVectorSizeT: {
				streamToVectorTypeT<size_t>(info, data);
				break;
			}

			case ParameterType::kString: {
				streamToTypeT<std::string>(info, data);
				break;
			}

			case ParameterType::kVectorReal: {
				streamToVectorTypeT<Real>(info, data);
				break;
			}
			default:
				break;
			}


		}


		void str2DataOneTask(
			GlobalThreadInfo& globalInfo, ThreadLocalInfo& threadInfo) {

			std::string substr;
			std::stringstream curstream;
			int paramId(0);
			size_t from = globalInfo.m_info.find_first_of("\n", threadInfo.from);
			size_t to = globalInfo.m_info.find_first_of("\n", threadInfo.to);

			//if (threadInfo.m_threadId == 0) {
			//	int stop = -1;
			//}
			// for test

			int id = 0;
			if (from < threadInfo.from || from >= threadInfo.to) {}
			else {
				++from;
				--to;
				if (from < to) {
					substr = globalInfo.m_info.substr(from, to - from);
					curstream << substr;
					while (curstream) {
						ParameterVariant value;
						stringstreamToParameterVariant(curstream, value);
						threadInfo.m_readVariants.emplace_back(value);

					}
					//	threadInfo.m_readVariants.pop_back();
				}

			}


		}


		void str2DataOneTaskLine(
			GlobalThreadInfo& globalInfo, ThreadLocalInfo& threadInfo) {

			std::string substr;

			int paramId(0);
			size_t from = globalInfo.m_info.find_first_of("\n", threadInfo.from);
			size_t to = globalInfo.m_info.find_first_of("\n", threadInfo.to);

			std::string lineInfo;
		
			int id = 0;
			if (from < threadInfo.from || from >= threadInfo.to) {}
			else {
				++from;
				if (from < to) {
					substr = globalInfo.m_info.substr(from, to - from);
					std::stringstream curstream(substr);
					//curstream << substr;
					while (std::getline(curstream, lineInfo)) {
						//if (id++ == 271) {
						//	int stop = -1;
						//}
						std::stringstream linestr(lineInfo);
						ParameterVariant value;
						stringstreamToParameterVariant(linestr, value);
						threadInfo.m_readVariants.emplace_back(value);
					//	threadInfo.m_tmpStr.emplace_back(lineInfo);
					}
					//	threadInfo.m_readVariants.pop_back();
				}

			}

		}

		void assignThreads(int num_samples, int num_task, std::vector<int>& ids) {
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




		void stream2ParametersMutithread(std::stringstream& info,
			ParameterVariantStream& params) {

			GlobalThreadInfo globalInfo;
			globalInfo.m_info = info.str();
			auto len = globalInfo.m_info.size();
			std::vector<int> tasks;
			auto numThread = std::thread::hardware_concurrency();
			//numThread = 1;
			assignThreads(len, numThread, tasks);
			std::vector<ThreadLocalInfo> threadInfos(numThread);
			for (int idx(0); idx < numThread; ++idx) {
				auto& threadinfo = threadInfos[idx];
				threadinfo.from = tasks[idx];
				threadinfo.to = tasks[idx + 1];
				threadinfo.m_threadId = idx;
			}


			std::vector<std::thread> thrds;
			for (size_t i = 0; i < numThread; ++i) {
				thrds.push_back(std::thread(
					&str2DataOneTask, std::ref(globalInfo), std::ref(threadInfos[i])));
			}

			for (auto& thrd : thrds)
				thrd.join();

			params.clear();
			int curIdx = 0;

			//int threadInfoIdx = 0;
			//ErrorInfo curInfo;
			for (auto& threadinfo : threadInfos) {
				//threadInfoIdx = 0;
				while (!threadinfo.m_readVariants.empty()) {
					auto info = std::move(threadinfo.m_readVariants.front());
					threadinfo.m_readVariants.pop_front();
					params.pushVariant(info);
				}
			}
		}



		void stream2ParametersMutithreadLine(std::stringstream& info,
			ParameterVariantStream& params) {

			GlobalThreadInfo globalInfo;
			globalInfo.m_info = info.str();
			auto len = globalInfo.m_info.size();
			std::vector<int> tasks;
			auto numThread = std::thread::hardware_concurrency();
			//numThread = 1;
			assignThreads(len, numThread, tasks);
			std::vector<ThreadLocalInfo> threadInfos(numThread);
			for (int idx(0); idx < numThread; ++idx) {
				auto& threadinfo = threadInfos[idx];
				threadinfo.from = tasks[idx];
				threadinfo.to = tasks[idx + 1];
				threadinfo.m_threadId = idx;
			}


			std::vector<std::thread> thrds;
			for (size_t i = 0; i < numThread; ++i) {
				thrds.push_back(std::thread(
					&str2DataOneTaskLine, std::ref(globalInfo), std::ref(threadInfos[i])));
			}

			for (auto& thrd : thrds)
				thrd.join();

			params.clear();
		//	int curIdx = 0;

		//	int threadInfoIdx = 0;
		//	ErrorInfo curInfo;
			for (auto& threadinfo : threadInfos) {
				//threadInfoIdx = 0;
				while (!threadinfo.m_readVariants.empty()) {
					auto info = std::move(threadinfo.m_readVariants.front());
					threadinfo.m_readVariants.pop_front();
					params.pushVariant(info);
				}
			}
		}




		void data2StrOneTask(GlobalThreadInfo& globalInfo, ThreadLocalInfo& threadInfo) {
			std::tuple<int, int, std::string> substr;
			while (true) {
				ParameterVariantStream::Type curVar;
				{
					std::unique_lock lock(globalInfo.m_info_mtx);
					if (globalInfo.m_list.empty()) break;
					curVar = std::move(globalInfo.m_list.front());
					globalInfo.m_list.pop_front();
				}
				std::get<0>(substr) = curVar.second;
				std::get<1>(substr) = threadInfo.m_threadId;
				std::stringstream curstream;

				parameterVariantToStringstream(curVar.first, curstream, globalInfo.m_precision);

				std::get<2>(substr) = curstream.str();
				threadInfo.m_outputStr.emplace_back(substr);
			}

		}



		void parameters2StreamMutithread(std::stringstream& info,
			ParameterVariantStream& params, int precision ) {

			if (params.empty())return;
			GlobalThreadInfo globalInfo;
			globalInfo.m_stream = &params;
			globalInfo.m_precision = precision;
			int numThread = std::thread::hardware_concurrency();

			std::vector<ThreadLocalInfo> threadinfos(numThread);

			for (int idx(0); idx < numThread; ++idx) {
				threadinfos[idx].m_threadId = idx;
			}

			while (!params.empty()) {
				globalInfo.m_list.emplace_back(params.popVariant());
			}


			std::vector<std::thread> thrds;
			for (size_t i = 0; i < numThread; ++i) {
				thrds.push_back(std::thread(
					&data2StrOneTask, std::ref(globalInfo), std::ref(threadinfos[i])));
			}

			for (auto& thrd : thrds)
				thrd.join();

			{
				

				info << "\n";
				using type = std::tuple<int, int, std::string>;
				std::priority_queue<type, std::vector<type>, CompareTuple> que;

				for (auto& it : threadinfos) {
					if (it.m_outputStr.empty())continue;
					que.push(it.m_outputStr.front());
					it.m_outputStr.pop_front();
				}
				type topValue;
				while (!que.empty()) {
					topValue = que.top();
					que.pop();
					info << std::get<2>(topValue);
					int threadId = std::get<1>(topValue);
					if (!threadinfos[threadId].m_outputStr.empty()) {
						que.push(threadinfos[threadId].m_outputStr.front());
						threadinfos[threadId].m_outputStr.pop_front();
					}
				}
			}
		}



		void stringstream2parameterStream(std::stringstream& info,
			ParameterVariantStream& params) {
			stream2ParametersMutithreadLine(info, params);
		}
		void parameterStream2stringstream(ParameterVariantStream& params, std::stringstream& info,
			 int precision) {
			parameters2StreamMutithread(info, params, precision);
		}



		void outputToFile(ofec::ParameterVariantStream& paramsStream, const std::string& filepath) {
			std::stringstream buf;
			ofec::variants_stream::parameterStream2stringstream(paramsStream, buf);
			std::ofstream out(filepath);
			out << buf.rdbuf();
			out.close();
		}


		void inputFromFile(ofec::ParameterVariantStream& paramsStream, const std::string& filepath) {
			std::stringstream buf;
			std::ifstream in(filepath);
			buf << in.rdbuf();
			in.close();
			ofec::variants_stream::stringstream2parameterStream(buf, paramsStream);

		}
	}
}
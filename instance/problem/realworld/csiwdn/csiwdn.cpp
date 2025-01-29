#include "csiwdn.h"
#include "../../../../core/global.h"
#include <cstring>
#include <numeric>
#include "../../../../utility/graph/graph.h"
#include <filesystem>

using namespace epanet;
namespace ofec {

	int CSIWDN::m_totalCswidnPro=0;
	std::mutex CSIWDN::m_totalPro_mtx;

	//std::mutex CSIWDN::ms_mutex_epanet;


	void CSIWDN::addInputParameters() {
		m_input_parameters.add("net-sensor-source case", new FileName(m_full_path,
			"instance/problem/realworld/csiwdn",
			"case (case*.txt)", "Net2/sensor_case1/source/case1.txt"));

		m_input_parameters.add("process name", new InputParameterValueTypeString(m_processName));
		
	}

	void CSIWDN::initialize_(Environment* env) {
		Problem::initialize_(env);

		m_map_inp.str("");
		m_map_rpt.str("");
		m_map_rpt_bn.str("");

		m_hyd.str("");

		size_t pos_1st_slash = m_full_path.find('/');
		m_map_name = m_full_path.substr(0, pos_1st_slash);
		size_t pos_1st_udrln = m_full_path.find('_');
		size_t pos_2nd_slash = m_full_path.find('/', pos_1st_slash + 1);
		m_sensor_case = m_full_path.substr(pos_1st_udrln + 1, pos_2nd_slash - pos_1st_udrln - 1);
		size_t pos_3rd_slash = m_full_path.find('/', pos_2nd_slash + 1);
		m_source_case = m_full_path.substr(pos_3rd_slash + 1);


#ifdef OFEC_LITTORCH
		m_use_lstm_model = false;
#endif // OFEC_DEMO_LITTORCH

		std::stringstream map_inp_origin;

		std::string saveFileRnd;
		{
			m_totalPro_mtx.lock();
			saveFileRnd = m_processName+"_" + std::to_string(m_totalCswidnPro++);
			m_totalPro_mtx.unlock();
		}
		//m_fileRnd = 

		m_map_inp << g_working_directory << "/instance/problem/realworld/csiwdn/" << m_map_name << "/map.inp";
		//m_map_inp << g_working_directory << "/instance/problem/realworld/csiwdn/" << m_map_name << "/map_"<< m_fileRnd<<".inp";

		//{
		//	std::filesystem::path p1 = map_inp_origin.str();
		//	std::filesystem::path p2 = m_map_inp.str();

		//	std::filesystem::copy(p1, p2);
		//}
		m_map_rpt << g_working_directory << "/instance/problem/realworld/csiwdn/" << m_map_name << "/map_" << saveFileRnd << ".rpt";
		m_map_rpt_bn << g_working_directory << "/instance/problem/realworld/csiwdn/" << m_map_name << "/map_" << saveFileRnd << ".rptBn";



		m_init_type = InitType::kRandom;
		resizeObjective(1);
		m_optimize_mode[0] = OptimizeMode::kMinimize;
		m_distanceType = DistanceType::Mix;

		readSensorParams();
		readSourceParams();
		readConnect();

		long num_WQS = m_total_duration / m_quality_time_step;  // times of water quality simulation (WQS)
		m_num_pattern = m_total_duration / m_pattern_step;
		m_Ciobs.resize(m_num_sensor, std::vector<float>(num_WQS));

		m_total_phase = m_total_duration / (m_num_quality_records_each_phase * m_quality_time_step);
		m_phase = m_total_phase;

		m_start_algorithm = false;

		//ms_mutex_epanet.lock();
		initializeEPANET(m_epanet); //的到水力信息
		readLocation();  //读取各节点坐标
		getCitByEPANET(m_epanet,m_real_PS_read, m_Ciobs);
		//ms_mutex_epanet.unlock();

		///*输出m_Ciobs数据*/
		int num_records = m_total_duration / m_quality_time_step;
		std::vector<int> time_records(num_records);
		std::vector<float> rates(num_records, 0.0);
		std::stringstream out_stream;
		out_stream << "rate,time,sensor....\n";
		for (int time = m_real_PS_read.startTime(0); time < m_real_PS_read.startTime(0) + m_real_PS_read.duration(0); time += m_quality_time_step)
			rates[time / m_quality_time_step] = m_real_PS_read.multiplier(0)[(time - m_real_PS_read.startTime(0)) / m_pattern_step];
		for (size_t i = 0; i < num_records; i++) {
			time_records[i] = i * m_quality_time_step;
			out_stream << rates[i] << "," << time_records[i];
			for (size_t j = 0; j < m_num_sensor; j++)
				out_stream << "," << m_Ciobs[j][i];
			out_stream << "\n";
		}
		std::stringstream path;
		path << g_working_directory << "/result/loc" << m_real_PS_read.location(0) << "_5.txt";    //m_file_name;
		std::ofstream fout(path.str());
		fout << out_stream.str();
		fout.close();

		/*LSTM data*/
		//auto &var = m_optima()->solution(0).variable();
		//m_random->uniform.next();
		//for (size_t id_node =79; id_node < m_num_node; id_node++)
		//{
		//	std::strcpy(var.location(0), m_node_name[id_node].c_str());
		//	std::stringstream out_stream;
		//	out_stream << "rate,time,sensor1,sensor2,sensor3,sensor4,sensor5,sensor6,sensor7,sensor8,sensor9,sensor10,sensor11,sensor12,sensor13,sensor14,sensor15,sensor16\n";
		//	int num_records = m_total_duration / m_quality_time_step;
		//	std::vector<std::array<float, 2>> input(num_records);
		//	for (size_t i = 0; i < num_records; i++)
		//		input[i][1] = i * m_quality_time_step;
		//	for (size_t id_input = 0; id_input < 20000; id_input++) {
		//		var.duration(0) = m_random->uniform.nextNonStd<int>(0, m_total_duration / 5) / 600 * 600;
		//		var.startTime(0) = m_random->uniform.nextNonStd<int>(0, m_total_duration / 2 - var.duration(0)) / 600 * 600;
		//		var.multiplier(0).resize(var.duration(0) / m_pattern_step);
		//		for (size_t k = 0; k < m_optima()->solution(0).variable().multiplier(0).size(); k++) {
		//			var.multiplier(0)[k] = m_random->uniform.nextNonStd<float>(m_min_multiplier, m_max_multiplier);
		//		}
		//		for (size_t z = 1; z < m_num_source; z++) {
		//			var.duration(z) = 0;
		//			var.startTime(z) = 0;
		//			var.multiplier(z).resize(var.duration(z) / m_pattern_step);
		//			for (size_t k = 0; k < m_optima()->solution(0).variable().multiplier(z).size(); k++) {
		//				var.multiplier(z)[k] = 0.0;
		//			}
		//		}
		//		initializeEPANET();
		//		getCitByEPANET(var, m_Ciobs);
		//		for (size_t i = 0; i < num_records; i++) {
		//			input[i][0] = 0;
		//		}
		//		for (int time = var.startTime(0); time < var.startTime(0) + var.duration(0); time += m_quality_time_step) {
		//			input[time / m_quality_time_step][0] = var.multiplier(0)[(time - var.startTime(0)) / m_pattern_step];
		//		}
		//		for (size_t i = 0; i < num_records; i++) {
		//			out_stream << input[i][0] << "," << input[i][1];
		//			for (size_t j = 0; j < m_num_sensor; ++j)
		//				out_stream << "," << m_Ciobs[j][i];
		//			out_stream << "\n";
		//		}
		//		if (id_input % 100 == 0)
		//			std::cout << "id_input: " << id_input << std::endl;
		//	}
		//	std::stringstream path2;
		//	path2 << g_working_dir << "/result/" << m_map_name << "/loc" << var.location(0) << ".txt";    //m_file_name;
		//	std::ofstream fout2(path2.str(), std::ios::app);
		//	fout2 << out_stream.str();
		//	fout2.close();
		//}
		//return;


		//std::stringstream out_stream;
		//auto &var = m_optima()->solution(0).variable();
		//out_stream << "rate,time,sensor1,sensor2,sensor3,sensor4\n";
		//int num_records = m_total_duration / m_quality_time_step;
		//std::vector<std::array<float, 2>> input(num_records);
		//for (size_t i = 0; i < num_records; i++) {
		//	input[i][0] = 0;
		//}
		//for (int time = var.startTime(0); time < var.startTime(0) + var.duration(0); time += m_quality_time_step) {
		//	input[time / m_quality_time_step][0] = var.multiplier(0)[(time - var.startTime(0)) / m_pattern_step];
		//}
		//for (size_t i = 0; i < num_records; i++)
		//	input[i][1] = i * m_quality_time_step;
		//for (size_t i = 0; i < num_records; i++) {
		//	out_stream << input[i][0] << "," << input[i][1];
		//	for (size_t j = 0; j < m_num_sensor; ++j)
		//		out_stream << "," << m_Ciobs[j][i];
		//	out_stream << "\n";
		//}
		//std::stringstream path2;
		//path2 << g_working_dir << "/result/" << m_map_name << "/source" << var.location(0) << "_optima.txt";    //m_file_name;
		//std::ofstream fout2(path2.str());
		//fout2 << out_stream.str();
		//fout2.close();

		///*test LSTM*/

		int first_detected_interval = m_real_PS_read.firstDetectedTime() / m_quality_time_step;
		m_first_phase = first_detected_interval / m_num_quality_records_each_phase + 1;
		m_phase = m_first_phase;
		if (m_phase < 0) throw Exception("No detection!");

		updateAdjmatrix();
		updateShortestPathes();



		m_source_idx = { 0, m_num_source-1 };

		m_q_ahc.clear();
		m_q_ahc.initialize(getAdjmatrix());
		m_num_be_visited.resize(m_num_source, std::vector<int>(m_num_node));
		m_pro_be_visited.resize(m_num_source);
		m_index_flag.resize(m_num_node, true);
		m_objective_accuracy = 1.e-2;


		m_max_location_dis = 0;
		for (size_t z = 1; z <= m_location.size(); z++) {
			for (size_t z2 = 1; z2 < z; ++z2) {
				m_max_location_dis = std::max<double>(m_max_location_dis, calculateDistance(z, z2));
			}
		}

		m_sensor_nodeId.resize(m_sensor_Loc.size());
		for (int idx(0); idx < m_sensor_nodeId.size(); ++idx) {
			m_sensor_nodeId[idx] = m_node_id[m_sensor_Loc[idx]]; // m_map_name
		}




	}

	void CSIWDN::updateOptima(Environment* env) {
		Solution<VarCSIWDN> temp_sol(m_number_objectives, m_number_constraints);
		temp_sol.variable() = m_real_PS_read;
		evaluate(temp_sol.variable(), temp_sol.objective(), temp_sol.constraint());
		auto new_optima = new Optima<VarCSIWDN>();
		new_optima->appendSolution(temp_sol);
		m_optima.reset(new_optima);
	}

	void CSIWDN::clustering(size_t num, Random *rnd) {
		m_clusters.clear();
		m_clusters.resize(num);
		if (m_cluster_type == ClusterType::kAHC) {
			m_q_ahc.clear();
			m_q_ahc.initialize(m_adjmatrix);
			m_q_ahc.clustering(num, m_adjmatrix);
			for (size_t i = 0; i < num; ++i)
				m_clusters[i] = m_q_ahc.clusters()[i].member;
		}
		else if (m_cluster_type == ClusterType::kKMeans) {
			std::vector<int> seq(m_num_node);
			std::iota(seq.begin(), seq.end(), 1);
			m_k_means.clustering(seq, num, this, rnd);
			for (size_t i = 0; i < num; ++i)
				m_clusters[i] = m_k_means.clusters()[i].samples;
		}
		else if (m_cluster_type == ClusterType::kNOCluster) {
			for (size_t i = 0; i < num; ++i)
				for (size_t j = 0; j < numberNode(); ++j)
					m_clusters[i].push_back(m_node_id[m_node_name[j]]);
		}
		else
			throw Exception("The initialization cluster method is incorrect");
	}

	float CSIWDN::calculateDistance(int index1, int index2) const {
		return sqrt(pow(m_location[index1 - 1][0] - m_location[index2 - 1][0], 2) + pow(m_location[index1 - 1][1] - m_location[index2 - 1][1], 2));
	}

	void CSIWDN::readSensorParams() {
		size_t i;
		std::string Line;
		char label[32];
		char *Keyword = 0;
		const char *Delimiters = " ():=\n\t\r\f\v\xef\xbb\xbf";  //分隔符
		std::ostringstream oss1;
		std::ifstream infile;
		oss1 << g_working_directory << "/instance/problem/realworld/csiwdn/" << m_map_name << "/sensor_" << m_sensor_case << "/sensor.txt";
		infile.open(oss1.str().c_str()); //c_str()将string类型转换为char*类型
		if (!infile) {
			throw Exception("The CSIWDN sensor file does not exist.");
		}
		char *savePtr;
		while (getline(infile, Line))
		{
			if (!(Keyword = gStrtok_r((char *)Line.c_str(), Delimiters, &savePtr)))//？？？
				continue;
			for (i = 0; i < strlen(Keyword); ++i)
				Keyword[i] = toupper(Keyword[i]); //小写字母转为大写

			if (!strcmp(Keyword, "NUMSENSOR")) {
				char *token = gStrtok_r(0, Delimiters, &savePtr);
				m_num_sensor = atoi(token);
				m_sensor_Loc.resize(m_num_sensor);
			}
			else if (!strcmp(Keyword, "SET_SENSORLOC"))
			{
				for (i = 0; i < m_num_sensor; i++)
					infile >> m_sensor_Loc[i];
			}
			else if (!strcmp(Keyword, "SET_DURATIONRANGE")) {
				infile >> m_min_duration;
				infile >> m_max_duration;
			}
			else if (!strcmp(Keyword, "SET_STARTTIMERANGE")) {
				infile >> m_min_start_time;
				infile >> m_max_start_time;
			}
			else if (!strcmp(Keyword, "SET_MULTIPLIERRANGE")) {
				infile >> m_min_multiplier;
				infile >> m_max_multiplier;
			}
			else if (!strcmp(Keyword, "SET_TOTALDURATION"))
			{
				infile >> m_total_duration;
			}
			else if (!strcmp(Keyword, "SET_QUALITYTIMESTEP"))
			{
				infile >> m_quality_time_step;
			}
		}
		infile.close();
		infile.clear();
	}

	void CSIWDN::readSourceParams() {
		size_t i;
		std::string Line;
		char label[32];
		char *Keyword = 0;
		const char *Delimiters = " ():=\n\t\r\f\v\xef\xbb\xbf";  //分隔符
		std::ostringstream oss1;
		std::ifstream infile;
		oss1 << g_working_directory << "/instance/problem/realworld/csiwdn/" << m_map_name << "/sensor_" << m_sensor_case << "/source/" << m_source_case;
		infile.open(oss1.str().c_str()); //c_str()将string类型转换为char*类型
		if (!infile) {
			throw Exception("The CSIWDN source file does not exist.");
		}
		char *savePtr;
		while (getline(infile, Line))
		{
			if (!(Keyword = gStrtok_r((char *)Line.c_str(), Delimiters, &savePtr)))//？？？
				continue;
			for (i = 0; i < strlen(Keyword); ++i)
				Keyword[i] = toupper(Keyword[i]); //小写字母转为大写

			if (!strcmp(Keyword, "NUMSOURCE")) {
				char *token = gStrtok_r(0, Delimiters, &savePtr);
				m_num_source = atoi(token);
				m_real_PS_read.resize(m_num_source);
			}
			else if (!strcmp(Keyword, "LOC"))
			{
				for (i = 0; i < m_num_source; ++i) {
					infile >> m_real_PS_read.slocation(i);
					strcpy(m_real_PS_read.location(i), m_real_PS_read.slocation(i).c_str());
				}
			}
			else if (!strcmp(Keyword, "SOURCE"))
			{
				for (i = 0; i < m_num_source; ++i) {
					infile >> m_real_PS_read.source(i);
				}
			}
			else if (!strcmp(Keyword, "STARTTIME"))
			{
				for (i = 0; i < m_num_source; ++i) {
					infile >> m_real_PS_read.startTime(i);
				}
			}
			else if (!strcmp(Keyword, "DURATION"))
			{
				m_inject_duration.resize(m_num_source);
				for (i = 0; i < m_num_source; ++i) {
					infile >> m_real_PS_read.duration(i);
					m_inject_duration[i] = m_real_PS_read.duration(i);
				}
			}
			else if (!strcmp(Keyword, "SET_PATTERNSTEP"))
			{
				infile >> m_pattern_step;
			}
			else if (!strcmp(Keyword, "SET_TIMEINTERVAL"))
			{
				infile >> m_num_quality_records_each_phase;
			}
			else if (!strcmp(Keyword, "MULTIPLIER"))
			{
				size_t mul_size;
				for (i = 0; i < m_num_source; ++i) {
					mul_size = (size_t)(m_real_PS_read.duration(i) / m_pattern_step);
					m_real_PS_read.multiplier(i).resize(mul_size, 0);
					for (size_t j = 0; j < mul_size; ++j) {
						infile >> m_real_PS_read.multiplier(i)[j];
					}
				}
			}
		}
		infile.close();
		infile.clear();
	}

	//void CSIWDN::evaluate_(SolutionBase &s) {
	//	VarCSIWDN &x = dynamic_cast<Solution<VarCSIWDN> &>(s).variable();
	//	auto &obj = dynamic_cast<Solution<VarCSIWDN> &>(s).objective();

	//	evaluateObjective(x, obj);
	//	int z = endSourceIdx();
	//	for (int j = 0; j <= z; ++j) {
	//		++m_num_be_visited[j][x.index(j) - 1];
	//	}
	//}


	void CSIWDN::evaluate(const VariableBase& vars, std::vector<Real>& objs, std::vector<Real>& cons)const {
		

		VarCSIWDN x = dynamic_cast<const VarCSIWDN &>(vars);
		evaluateObjective(x, objs);
		int z = endSourceIdx();
		//for (int j = 0; j <= z; ++j) {
		//	++m_num_be_visited[j][x.index(j) - 1];
		//}
	}

	void CSIWDN::evaluateObjective(VarCSIWDN &x, std::vector<Real> &obj) const{    //  single evaluation
		int num = m_phase * m_num_quality_records_each_phase;
		std::vector<std::vector<float>> Cit(m_num_sensor, std::vector<float>(num));
		float temp = 0;
#ifdef OFEC_LITTORCH
		if (m_use_lstm_model)
			getCitByLSTM(x, Cit);
		else {
			ms_mutex_epanet.lock();
			initializeEPANET();
			getCitByEPANET(x, Cit);
			ms_mutex_epanet.unlock();
		}
#else
		//ms_mutex_epanet.lock();
		epanet::EPANET epanet;
		initializeEPANET(epanet);
		getCitByEPANET(epanet,x, Cit);
		//ms_mutex_epanet.unlock();
#endif // OFEC_DEMO_LITTORCH

		if (x.isDetected()) {
			for (size_t i = 0; i < num; ++i) {
				for (size_t j = 0; j < m_num_sensor; ++j) {
					temp += pow(m_Ciobs[j][i] - Cit[j][i], 2);
				}
			}
			obj[0] = sqrt(temp / (m_num_sensor * num));
		}
		else
			obj[0] = 10;
		x.isDetected() = false;
	}

	void CSIWDN::getCitByEPANET(epanet::EPANET& epanet, VarCSIWDN &sol, std::vector<std::vector<float>> &data) const {
		long tstep;
		long t;
		int index;
		char label[32];
		float c;


		epanet.ENopenQ();
		std::vector<float> pattern_value;
		for (int i = 0; i < m_num_source; i++) {
			pattern_value.assign(m_num_pattern, 0.0);
			for (int j = 1; j <= sol.multiplier(i).size(); ++j) {
				int period = sol.startTime(i) / m_pattern_step + j; //开始后的第几个阶段，10分钟一个阶段
				float value = sol.multiplier(i)[j - 1];  //该阶段的乘子
				pattern_value[period - 1] = value;
			}
			epanet.ENsetpattern(m_pattern_index[i], pattern_value.data(), m_num_pattern);  //为指定的时间模式设置所有的乘数因子
			if (sol.flagLocation()) {  //是否是真实源头的标示
				epanet.ENgetnodeindex(sol.location(i), &sol.index(i));
			}
			else epanet.ENgetnodeid(sol.index(i), sol.location(i));
			epanet.ENsetnodevalue(sol.index(i), EN_SOURCEQUAL, sol.source(i));//设置源头水质
			epanet.ENsetnodevalue(sol.index(i), EN_SOURCEPAT, (float)m_pattern_index[i]);//时间模式的索引
			epanet.ENsetnodevalue(sol.index(i), EN_SOURCETYPE, EN_FLOWPACED);
		}

		epanet.ENinitQ(0);
		long count = 0;
		do {
			epanet.ENrunQ(&t);
			epanet.ENstepQ(&tstep);
			for (int i = 0; i < m_num_sensor; ++i)
			{
				size_t j;
				for (j = 0; j < m_sensor_Loc[i].size(); ++j) {
					label[j] = m_sensor_Loc[i][j];
				}
				label[j] = '\0';
				epanet.ENgetnodeindex(label, &index);
				epanet.ENgetnodevalue(index, EN_QUALITY, &c);
				data[i][t / m_quality_time_step] = c;
				if (c > 1e-6 && !(sol.isDetected())) {
					sol.firstDetectedTime() = t;
					sol.isDetected() = true;
				}
			}
#ifdef OFEC_DEMO
			m_node_quality.resize(m_num_node, std::vector<float>(m_total_duration / m_quality_time_step));
			for (int i = 0; i < m_num_node; ++i) {
				m_epanet.ENgetnodeindex((char *)m_node_name[i].c_str(), &index);
				m_epanet.ENgetnodevalue(index, EN_QUALITY, &c);
				m_node_quality[i][t / m_quality_time_step] = c;
			}
#endif
			++count;
			if (count == m_num_quality_records_each_phase * m_phase) break;  //分时段
		} while (tstep > 0); //tstep=0表示模拟结束


		epanet.ENcloseQ();
		epanet.ENclose();
	}

#ifdef  OFEC_LITTORCH


	//torch::Device g_device(torch::cuda::is_available() ? "cuda" : "cpu");

	void CSIWDN::initializeLSTM() {
		int hidden_size, num_layers;
		std::string param_file_name, model_file_name;

		m_input_size = 2;
		m_output_size = m_num_sensor;
		m_input_range.resize(m_input_size);
		m_input_range[0] = { 0, m_max_multiplier };
		m_input_range[1] = { m_min_duration, m_max_duration };
		m_output_range.resize(m_output_size);
		for (size_t j = 0; j < m_output_size; j++)
			m_output_range[j] = { m_min_multiplier, m_max_multiplier };

		m_lstm_model.resize(m_num_node);
		for (size_t i = 0; i < m_num_node; i++) {
			param_file_name = g_working_dir + "/instance/problem/realworld/csiwdn/" + m_map_name + "/sensor_case1/model/model_informa/loc" + m_node_name[i] + ".txt";
			std::ifstream in_file(param_file_name);
			std::string line;
			int pos;
			while (std::getline(in_file, line)) {
				if (pos = line.find("hidden size: ") != std::string::npos)
					hidden_size = std::stoi(line.substr(pos + 12, line.size() - pos + 12));
				if (pos = line.find("num layers: ") != std::string::npos)
					num_layers = std::stoi(line.substr(pos + 11, line.size() - pos + 11));
			}
			m_lstm_model[i] = std::make_unique<LstmLinear>(m_input_size, m_output_size, hidden_size, num_layers);
			model_file_name = g_working_dir + "/instance/problem/realworld/csiwdn/" + m_map_name + "/sensor_case1/model/loc" + m_node_name[i] + ".pt";
			torch::load(*m_lstm_model[i], model_file_name);
			(*m_lstm_model[i])->to(g_device);
		}

		m_lstm_model_initialized = true;
	}


	void CSIWDN::getCitByLSTM(VarCSIWDN &sol, std::vector<std::vector<float>> &data) {
		if (!m_lstm_model_initialized)
			initializeLSTM();
		int num_quality_records = m_num_quality_records_each_phase * m_phase;
		std::vector<std::vector<float>> inputs(m_num_source, std::vector<float>(num_quality_records * m_input_size));
		std::vector<torch::Tensor> t_inputs(m_num_source), t_outputs(m_num_source);
		torch::Tensor sum_output;

		for (size_t k = 0; k < m_num_source; k++) {
			std::strcpy(sol.location(k), m_node_name[sol.index(k) - 1].c_str());
			sol.slocation(k) = m_node_name[sol.index(k) - 1];
		}

		for (size_t k = 0; k < m_num_source; k++) {
			for (size_t i = 0; i < num_quality_records; i++) {
				inputs[k][i * m_input_size] = 0;
				inputs[k][i * m_input_size + 1] = (i * m_quality_time_step - m_input_range[1].first)
					/ (m_input_range[1].second - m_input_range[1].first);
			}
		}

		for (size_t k = 0; k < m_num_source; k++) {
			for (int time = sol.startTime(k); time < sol.startTime(k) + sol.duration(k); time += m_quality_time_step) {
				if (time / (m_num_quality_records_each_phase * m_quality_time_step) > m_phase - 1)
					break;
				float value = (sol.multiplier(k)[(time - sol.startTime(k)) / m_pattern_step] - m_input_range[0].first)
					/ (m_input_range[0].second - m_input_range[0].first);
				inputs[k][time / m_quality_time_step * m_input_size] = value;
			}
			t_inputs[k] = torch::from_blob(inputs[k].data(), { 1, num_quality_records, m_input_size }).to(g_device);
			t_outputs[k] = (*m_lstm_model[m_node_id[sol.slocation(k)]])(t_inputs[k]);
			if (k == 0)
				sum_output = t_outputs[k];
			else
				sum_output += t_outputs[k];
		}
		for (size_t i = 0; i < m_output_size; i++) {
			for (size_t j = 0; j < num_quality_records; j++) {
				data[i][j] = sum_output[0][j][i].item().toFloat()
					* (m_output_range[i].second - m_output_range[i].first)
					+ m_output_range[i].first;
				if (data[i][j] < 0)
					data[i][j] = 0;
				if (data[i][j] > 0.001 && !(sol.isDetected())) {
					sol.firstDetectedTime() = data[i][j];
					sol.isDetected() = true;
				}
			}
		}
	}
#endif //  OFEC_DEMO_LITTORCH


	void CSIWDN::readLocation() {
		bool flag = false;
		m_location.resize(m_num_node, std::vector<Real>(2));
		m_node_name.resize(m_num_node);
		std::string line;
		std::ifstream ifname;
		ifname.open(m_map_inp.str());
		size_t i = 0;
		while (getline(ifname, line)) {
			if (line.find("[COORDINATES]") != std::string::npos) {
				flag = true;
				continue;
			}
			if (i == m_num_node)
				break;
			if (flag) {
				ifname >> m_node_name[i];
				m_node_id[m_node_name[i]] = i;
				ifname >> m_location[i][0];
				ifname >> m_location[i][1];
				++i;
			}
		}
		ifname.close();
	}

	void CSIWDN::getEPANETinfo() {
		m_epanet.ENgetcount(EN_NODECOUNT, &m_num_node);      // get number of node
		m_pattern_index.resize(m_num_source);
		std::vector<std::string> pattern_index(m_num_source);
		for (size_t i = 0; i < m_num_source; i++) {
			pattern_index[i] = "Contaminant " + std::to_string(i);
			m_epanet.ENaddpattern(pattern_index[i].data());
			m_epanet.ENgetpatternindex(pattern_index[i].data(), &m_pattern_index[i]);
		}
	}
	void CSIWDN::initializeEPANET(epanet::EPANET& epanet)const {
		epanet.ENopen(const_cast<char *>(m_map_inp.str().data()), const_cast<char *>(m_map_rpt.str().data()), const_cast<char*>(m_map_rpt_bn.str().data()));   //  open input and report file 
		epanet.ENsettimeparam(EN_DURATION, m_total_duration);     //  set Total Duration
		epanet.ENsettimeparam(EN_HYDSTEP, 60 * 10);         //  set Hydraulic Time Step
		epanet.ENsettimeparam(EN_QUALSTEP, m_quality_time_step);        //  set Quality Time Step 10mins
		epanet.ENsettimeparam(EN_PATTERNSTEP, m_pattern_step);     //  set Pattern Time Step
		epanet.ENsettimeparam(EN_PATTERNSTART, 0);        //  set Pattern Start Time
		epanet.ENsettimeparam(EN_REPORTSTEP, 60 * 10);      //  set Report Time Step
		epanet.ENsettimeparam(EN_REPORTSTART, 0);         //  set Report Start Time
		epanet.ENsetqualtype(EN_CHEM, "Chlorine", "mg/L", ""); // set type of source





		long tstep, t;
		int index;
		epanet.ENopenH();
		epanet.ENinitH(EN_SAVE);
		do {
			epanet.ENrunH(&t);
#ifdef OFEC_DEMO
			//long num_WHS = m_total_duration / m_quality_time_step;
			//float c;
			////m_flow_rate.resize(m_pipe.size(), std::vector<float>(num_WHS));
			//for (int i = 0; i < m_pipe.size(); i++)
			//	m_pipe[i].flow_rate.resize(num_WHS);
			//for (int i = 0; i < m_pipe.size(); i++) {
			//	m_epanet.ENgetlinkindex((char *)m_pipe[i].pipe_id.c_str(), &index);
			//	m_epanet.ENgetlinkvalue(index, EN_FLOW, &c);
			//	m_pipe[i].flow_rate[t / m_quality_time_step] = c;
			//	//m_flow_rate[i][t / m_quality_time_step] = c;
			//}
#endif
			epanet.ENnextH(&tstep);
		} while (tstep > 0);
		epanet.ENcloseH();
	}


	void CSIWDN::updateAdjmatrix() {
		m_adjmatrix.resize(m_num_node);
		for (size_t i = 0; i < m_num_node; i++) {
			m_adjmatrix[i].resize(m_num_node);
			for (size_t j = 0; j < m_adjmatrix[i].size(); j++)
				m_adjmatrix[i][j] = false;
		}
		m_directed_adjmatrix = m_adjmatrix;
		for (size_t i = 0; i < m_pipe.size(); i++) {
			int m = m_node_id[m_pipe[i].sNode];
			int n = m_node_id[m_pipe[i].eNode];
			m_adjmatrix[m][n] = true;
			m_adjmatrix[n][m] = true;
		}
		for (size_t i = 0; i < m_pipe.size(); i++) {
			int m = m_node_id[m_pipe[i].sNode];
			int n = m_node_id[m_pipe[i].eNode];

			m_directed_adjmatrix[m][n] = true;
		}




	}

	void CSIWDN::readConnect() {
		bool flag = false;
		bool pump = false;
		bool valve = false;

		std::string line;
		std::ifstream ifname;
		//std::pair<std::string, double> pipe;
		ifname.open(m_map_inp.str());
		//size_t i = 0;
		std::string pipe_id;
		//std::string pipe_length;
		//std::vector<std::string> connect(2);
		Pipe p;
		//double diameter;
		while (getline(ifname, line)) {
			if (line.find("[PIPES]") != std::string::npos) {
				flag = true;
				continue;
			}
			if (flag) {
				ifname >> pipe_id;
				if (pipe_id == "[PUMPS]") {
					getline(ifname, line);
					pump = true;
					continue;
				}
				else if (pipe_id == "[VALVES]") {
					getline(ifname, line);
					valve = true;
					continue;
				}
				else if (pipe_id == "[TAGS]")
					break;
				ifname >> p.sNode;
				ifname >> p.eNode;
				//m_connect.push_back(connect);
				if (!pump && !valve) {
					ifname >> p.length;
					ifname >> p.diameter;
					m_pipe.push_back(p);
					//m_pipe.push_back(std::make_pair(pipe_id, diameter));
				}
				else if (pump) {
					p.length = 10;
					p.diameter = 0;
					m_pipe.push_back(p);
					//m_pipe.push_back(std::make_pair(pipe_id, 0));
				}
				else if (valve) {
					ifname >> p.diameter;
					p.length = 10;
					m_pipe.push_back(p);
					//m_pipe.push_back(std::make_pair(pipe_id, diameter));
				}
			}
		}
		ifname.close();
	}

	void CSIWDN::updateShortestPathes() {
		Graph graph;
		graph.setNumVertex(m_num_node);
		for (size_t i = 0; i < m_pipe.size(); i++) {
			int m = m_node_id[m_pipe[i].sNode];
			int n = m_node_id[m_pipe[i].eNode];
			graph.addEdge(m, n);
		}
		m_shortest_pathes.clear();
		m_shortest_pathes.resize(m_num_node, std::vector<std::vector<int>>(m_num_node));
		//for (size_t i = 0; i < m_num_node; ++i) {
		//	//m_shortest_pathes[i][i] = 1.0;
		//}
		for (size_t i = 0; i < m_num_node; ++i) {
			for (size_t j = i + 1; j < m_num_node; ++j) {
				m_shortest_pathes[i][j] = m_shortest_pathes[j][i] = graph.getShortestPath(i, j);
			}
		}
	}

//	void CSIWDN::updateCandidates(const SolutionBase &sol, std::list<std::unique_ptr<SolutionBase>> &candidates) const {
//#ifdef OFEC_LITTORCH
//		if (m_use_lstm_model)
//			return;
//#endif // OFEC_DEMO_LITTORCH
//
//		if (candidates.empty())
//			candidates.emplace_back(new Solution<VarCSIWDN>(sol));
//		else if (sol.objectiveDistance(m_optima->objective(0)) < m_objective_accuracy) {
//			for (auto &c : candidates) {
//				bool flag = false;
//				for (size_t i = 0; i <= endSourceIdx(); ++i) {
//					if (dynamic_cast<const VarCSIWDN&>(sol.variableBase()).slocation(i) 
//						!= dynamic_cast<const VarCSIWDN&>(c->variableBase()).slocation(i))
//					{
//						flag = true;
//						break;
//					}
//				}
//				if (!flag)
//					return;
//			}
//			candidates.emplace_back(new Solution<VarCSIWDN>(sol));
//		}
//	}

	void CSIWDN::updatePhase(int phase) {
		m_phase = phase;
	}

	void CSIWDN::calProByBeVisited() {
		int z = endSourceIdx();
		for (int j = 0; j <= z; ++j) {
			m_pro_be_visited[j].clear();
			int max = 0;
			for (auto &i : m_num_be_visited[j])
				if (max < i) max = i;
			std::vector<int> inverse;
			for (auto &i : m_num_be_visited[j])
				inverse.push_back(max - i);
			int sum = 0;
			for (auto &i : inverse)
				sum += i;
			for (auto &i : inverse)
				m_pro_be_visited[j].push_back((double)i / (double)sum);
		}
	}

	bool CSIWDN::isTimeOut() const {
		return (m_total_duration / (m_num_quality_records_each_phase * m_quality_time_step) < m_phase);
	}

	int CSIWDN::endSourceIdx() const {
		int z = 0;
		while ((z + 1) < numberSource() && phase() >= ((m_real_PS_read.startTime(z + 1) / intervalTimeStep()))) {
			z++;
		}
		return z;
	}

	void CSIWDN::setDeaultVaribleValue(VariableBase& x)const {
		VarCSIWDN& var = dynamic_cast<VarCSIWDN&>(x);
		var.flagLocation() = false;
		for (size_t z = 0; z < m_num_source; z++) {
			var.source(z) = 1.0;
			var.startTime(z) = optima()->solution(0).variable().startTime(z);
			var.duration(z) = optima()->solution(0).variable().duration(z);
			size_t size = var.duration(z) / m_pattern_step;
			if (var.duration(z) % m_pattern_step != 0)
				++size;
			var.multiplier(z).resize(size);
		}
	}

	void CSIWDN::solutionToParameterVariants(const SolutionBase& sol, ParameterVariantStream& out)const  {
		Problem::solutionToParameterVariants(sol, out);
		auto& var = dynamic_cast<const VarCSIWDN&>(sol.variableBase());
		
		std::vector<int> pos(m_num_source);
		std::vector<std::vector<ofec::Real>> multis(m_num_source);
		for (int idx(0); idx < m_num_source;++idx) {
			pos[idx] = var.index(idx);
			multis[idx] = var.multiplier(idx);
		}
		
		out << pos;
		out << multis.size();
		for (int idx(0); idx < m_num_source; ++idx) {
			out << multis[idx];
		}
		
	}
	void CSIWDN::parameterVariantsToSolution(ParameterVariantStream& in, SolutionBase& sol) const {

		Problem::parameterVariantsToSolution(in,sol);
		setDeaultVaribleValue(sol.variableBase());
		auto& var = dynamic_cast<VarCSIWDN&>(sol.variableBase());

		std::vector<int> pos(m_num_source);
		std::vector<std::vector<ofec::Real>> multis(m_num_source);
		in>>pos;
		size_t size(0);
		in >> size;

		for (int idx(0); idx < m_num_source; ++idx) {
			in>> multis[idx];
		}

		for (int idx(0); idx < m_num_source; ++idx) {
			var.index(idx) = pos[idx];
			var.multiplier(idx)= multis[idx];
		}

	}

	void CSIWDN::initializeVariables(VariableBase&x, Random *rnd) const {
		VarCSIWDN& var = dynamic_cast<VarCSIWDN&>(x);
		setDeaultVaribleValue(x);
		
		for (size_t z = 0; z < m_num_source; z++) {
			if (m_init_type == InitType::kCluster) {
				if (z == 0) {
					int size = m_clusters[m_pop_identifier].size();
					int random = rnd->uniform.nextNonStd<int>(0, size);
					var.index(z) = m_clusters[m_pop_identifier][random] + 1;
				}
				else
					var.index(z) = rnd->uniform.nextNonStd<int>(1, m_num_node + 1);
			}
			else if (m_init_type == InitType::kRandom) {
				var.index(z) = rnd->uniform.nextNonStd<int>(1, m_num_node + 1);
			}
			else if (m_init_type == InitType::kDistance) {
				std::vector<Real> roulette(m_num_node + 1, 0);
				for (size_t i = 0; i < roulette.size() - 1; ++i) {
					roulette[i + 1] = roulette[i] + m_distance_node[i];
				}
				Real random_value = rnd->uniform.nextNonStd<Real>(roulette[0], roulette[roulette.size() - 1]);
				for (size_t i = 0; i < roulette.size() - 1; ++i) {
					if (random_value >= roulette[i] && random_value < roulette[i + 1]) {
						var.index(z) = i + 1;
						break;
					}
				}
			}
			else if (m_init_type == CSIWDN::InitType::kBeVisited) {
				if (z <= endSourceIdx()) {
					int identifier = popIdentifier();
					int size = clusters()[identifier].size();
					std::vector<Real> pro(size, 0.0);
					for (size_t i = 0; i < size; ++i) {
						pro[i] = getProBeVisited()[z][clusters()[identifier][i]];
					}
					var.index(z) = (rnd->uniform.spinWheel(pro.begin(), pro.end()) - pro.begin()) + 1;
				}
				else {
					var.index(z) = rnd->uniform.nextNonStd<int>(1, numberNode() + 1);
				}
			}
			else throw Exception("No this initialization type");
			for (auto &j : var.multiplier(z)) {
				j = rnd->uniform.nextNonStd<float>(m_min_multiplier, m_max_multiplier);
			}

		}
	}

	void CSIWDN::initSolutionMultiplier(SolutionBase &s, Random *rnd) {
		VarCSIWDN &var = dynamic_cast<Solution<VarCSIWDN> &>(s).variable();

		for (size_t z = 0; z < m_num_source; z++) {
			for (auto &j : var.multiplier(z)) {
				j = rnd->uniform.nextNonStd<float>(m_min_multiplier, m_max_multiplier);
			}
		}
	}


	void CSIWDN::initSolutionMultiplier(SolutionBase& s,
		const std::vector<std::vector<std::pair<double, double>>>& range, Random* rnd) {
		VarCSIWDN& var = dynamic_cast<Solution<VarCSIWDN> &>(s).variable();

		for (size_t z = 0; z < m_num_source; z++) {

			for (int j(0); j < var.multiplier(z).size(); ++j) {
				var.multiplier(z)[j] = rnd->uniform.nextNonStd<double>(range[z][j].first, range[z][j].second);
			}
			//for (auto& j : var.multiplier(z)) {
			//	j = rnd->uniform.nextNonStd<float>(m_min_multiplier, m_max_multiplier);
			//}
		}
	}


	void CSIWDN::setSolutionMultiplier(SolutionBase &s, const SolutionBase &copy) {
		VarCSIWDN &var = dynamic_cast<Solution<VarCSIWDN> &>(s).variable();
		const VarCSIWDN &copyVar = dynamic_cast<const Solution<VarCSIWDN> &>(copy).variable();
		for (size_t z = 0; z < m_num_source; z++) {
			var.multiplier(z) = copyVar.multiplier(z);
		}
	}


	void CSIWDN::initSolutionPosition(SolutionBase &s, Random *rnd, int num) {
		std::vector<int> totalSource(m_num_source);
		for (int idx(0); idx < m_num_source; ++idx) {
			totalSource[idx] = idx;
		}
		rnd->uniform.shuffle(totalSource.begin(), totalSource.end());
		totalSource.resize(num);
		VarCSIWDN &var = dynamic_cast<Solution<VarCSIWDN> &>(s).variable();
		//var.flagLocation() = false;
		for (auto z : totalSource) {
			var.index(z) = rnd->uniform.nextNonStd<int>(1, m_num_node + 1);
		}

	}

	void CSIWDN::setSolutionPosition(SolutionBase &s, const SolutionBase &copy) {
		VarCSIWDN &var = dynamic_cast<Solution<VarCSIWDN> &>(s).variable();
		const VarCSIWDN &copyVar = dynamic_cast<const Solution<VarCSIWDN> &>(copy).variable();
		for (size_t z = 0; z < m_num_source; z++) {
			var.index(z) = copyVar.index(z);
		}
	}

	void CSIWDN::fitlerSameSolutions(std::vector<ofec::SolutionBase*>& sols,
		std::vector<ofec::SolutionBase*>& filterSols, std::vector<int>& mapSolId) {
		
		std::vector<int> sortedIds(sols.size());
		for (int idx(0); idx < sortedIds.size(); ++idx) {
			sortedIds[idx] = idx;
		}


		std::vector<ofec::CSIWDN::solutionType*> cursols(sols.size());
		for (int idx(0); idx < cursols.size(); ++idx) {
			cursols[idx] = dynamic_cast<ofec::CSIWDN::solutionType*>(sols[idx]);
		}
		
		std::sort(sortedIds.begin(), sortedIds.end(), [&](int a, int b) {
			if (sols[a]->fitness() == sols[b]->fitness()) {

				for (size_t z = sourceIdx().first; z <= sourceIdx().second; z++) {
					if (cursols[a]->variable().index(z) < cursols[b]->variable().index(z)) {
						return false;
					}
					else if (cursols[a]->variable().index(z) > cursols[b]->variable().index(z)) return true;
					else if (cursols[a]->variable().multiplier(z) < cursols[b]->variable().multiplier(z)) {
						return false;
					}
					else if (cursols[a]->variable().multiplier(z) > cursols[b]->variable().multiplier(z)) return true;
				}
				return a<b;

				//return cswidnPro->compareSols(sols[a]->variableBase(), sols[b]->variableBase()) <= 0;
			}
			else return sols[a]->fitness() < sols[b]->fitness();

			});

		filterSols.push_back(sols[sortedIds.front()]);
		mapSolId.resize(sols.size());
		//originIds.push_back(sortedIds.front());
		int mapsolid = 0;
		mapSolId[sortedIds.front()] = mapsolid;

		for (int idx(1); idx < sortedIds.size(); ++idx) {
			bool isSame = same(sols[sortedIds[idx - 1]]->variableBase(), sols[sortedIds[idx]]->variableBase());
			if (!isSame) {
				filterSols.push_back(sols[sortedIds[idx]]);
			//	originIds.push_back(sortedIds[idx]);
				mapsolid++;
			}
			mapSolId[sortedIds[idx]] = mapsolid;
		}

	}

	


	//void CSIWDN::initializeSolution(SolutionBase &s, Random *rnd) const {
	//	VarCSIWDN &var = dynamic_cast<Solution<VarCSIWDN> &>(s).variable();
	//	var.flagLocation() = false;

	//	for (size_t z = 0; z < m_num_source; z++) {
	//		if (m_init_type == InitType::kCluster) {
	//			if (z == 0) {
	//				int size = m_clusters[m_pop_identifier].size();
	//				int random = rnd->uniform.nextNonStd<int>(0, size);
	//				var.index(z) = m_clusters[m_pop_identifier][random] + 1;
	//			}
	//			else
	//				var.index(z) = rnd->uniform.nextNonStd<int>(1, m_num_node + 1);
	//		}
	//		else if (m_init_type == InitType::kRandom) {
	//			var.index(z) = rnd->uniform.nextNonStd<int>(1, m_num_node + 1);
	//		}
	//		else if (m_init_type == InitType::kDistance) {
	//			std::vector<Real> roulette(m_num_node + 1, 0);
	//			for (size_t i = 0; i < roulette.size() - 1; ++i) {
	//				roulette[i + 1] = roulette[i] + m_distance_node[i];
	//			}
	//			Real random_value = rnd->uniform.nextNonStd<Real>(roulette[0], roulette[roulette.size() - 1]);
	//			for (size_t i = 0; i < roulette.size() - 1; ++i) {
	//				if (random_value >= roulette[i] && random_value < roulette[i + 1]) {
	//					var.index(z) = i + 1;
	//					break;
	//				}
	//			}
	//		}
	//		else if (m_init_type == CSIWDN::InitType::kBeVisited) {
	//			if (z <= endSourceIdx()) {
	//				int identifier = popIdentifier();
	//				int size = clusters()[identifier].size();
	//				std::vector<Real> pro(size, 0.0);
	//				for (size_t i = 0; i < size; ++i) {
	//					pro[i] = getProBeVisited()[z][clusters()[identifier][i]];
	//				}
	//				var.index(z) = (rnd->uniform.spinWheel(pro.begin(), pro.end()) - pro.begin()) + 1;
	//			}
	//			else {
	//				var.index(z) = rnd->uniform.nextNonStd<int>(1, numberNode() + 1);
	//			}
	//		}
	//		else throw Exception("No this initialization type");

	//		var.source(z) = 1.0;
	//		var.startTime(z) = optima()->variable(0).startTime(z);
	//		var.duration(z) = optima()->variable(0).duration(z);
	//		size_t size = var.duration(z) / m_pattern_step;
	//		if (var.duration(z) % m_pattern_step != 0) 
	//			++size;
	//		var.multiplier(z).resize(size);
	//		for (auto &j : var.multiplier(z)) {
	//			j = rnd->uniform.nextNonStd<float>(m_min_multiplier, m_max_multiplier);
	//		}

	//	}
	//}


	int CSIWDN::compareSols(const VariableBase& x1, const VariableBase& x2) const {
		const VarCSIWDN& var1 = dynamic_cast<const VarCSIWDN&>(x1);
		const VarCSIWDN& var2 = dynamic_cast<const VarCSIWDN&>(x2);

		for (size_t z = m_source_idx.first; z <= m_source_idx.second; z++) {
			if (var1.index(z) < var2.index(z)) {
				return -1;
			}
			else if (var1.index(z) > var2.index(z)) return 1;
			//else if (var1.multiplier(z) < var2.multiplier(z)) {
			//	return -1;
			//}
			//else if (var1.multiplier(z) > var2.multiplier(z)) return 1;
		}

		return 0;
	}

	bool CSIWDN::same(const VariableBase &x1, const VariableBase &x2) const {
		const VarCSIWDN &var1 = dynamic_cast<const VarCSIWDN&>(x1);
		const VarCSIWDN &var2 = dynamic_cast<const VarCSIWDN&>(x2);

		for (size_t z = m_source_idx.first; z <= m_source_idx.second; z++) {
			if (var1.index(z) != var2.index(z)) {
				return false;
			}
			if (var1.multiplier(z) != var2.multiplier(z)) {
				return false;
			}
		}

		return true;
	}

	//Real CSIWDN::variableDistance(const SolutionBase &s1, const SolutionBase &s2) const
	//{
	//	const VarCSIWDN &var1 = dynamic_cast<const Solution<VarCSIWDN> &>(s1).variable();
	//	const VarCSIWDN &var2 = dynamic_cast<const Solution<VarCSIWDN> &>(s2).variable();

	//	//Real total_multiplier(0);
	//	//Real total_pos(0);
	//	//std::vector<Real> norX1(m_num_source);
	//	//std::vector<Real> norX2(m_num_source);
	//	//for (size_t z = 0; z < m_num_source; z++) {
	//	//	if (var1.index(z) != var2.index(2)) total_pos += 1;
	//	//	norX1.resize(var1.multiplier(z).size());
	//	//	norX2 = norX1;
	//	//	for (size_t idDim(0); idDim < var1.multiplier(z).size(); ++idDim) {
	//	//		norX1[idDim] = mapReal<double>(norX1[idDim],
	//	//			m_min_multiplier, m_max_multiplier, 0, 1);

	//	//		norX2[idDim] = mapReal<double>(norX2[idDim],
	//	//			m_min_multiplier, m_max_multiplier, 0, 1);
	//	//	}

	//	//	total_multiplier += euclideanDistance(norX1.begin(), norX1.end(), norX2.begin());

	//	//}

	//	//return total_pos + total_pos / double(m_num_source);

	//	double locationDis(0);
	//	double multiplierDis(0);

	//	double len = m_max_multiplier - m_min_multiplier;
	//	double curMDis(0);
	//	for (size_t z = 0; z < m_num_source; z++) {
	//		locationDis += calculateDistance(var1.index(z), var2.index(z)) / m_max_location_dis;
	//		curMDis = ofec::euclideanDistance(var1.multiplier(z).begin(), var1.multiplier(z).end(), var2.multiplier(z).begin());
	//		curMDis = curMDis / (len * sqrt(var1.multiplier(z).size()));
	//		multiplierDis += curMDis;
	//	}
	//	return locationDis / m_num_source * m_dis_ratio + multiplierDis / m_num_source * (1.0 - m_dis_ratio);
	//}

	Real CSIWDN::variableDistance(const VariableBase &s1, const VariableBase &s2) const
	{

		return variableDistance(s1, s2, m_source_idx);
		//const VarCSIWDN &var1 = dynamic_cast<const VarCSIWDN &>(s1);
		//const VarCSIWDN &var2 = dynamic_cast<const VarCSIWDN &>(s2);


		////Real total_multiplier(0);
		////Real total_pos(0);
		////std::vector<Real> norX1(m_num_source);
		////std::vector<Real> norX2(m_num_source);
		////for (size_t z = 0; z < m_num_source; z++) {
		////	if (var1.index(z) != var2.index(2)) total_pos += 1;
		////	norX1.resize(var1.multiplier(z).size());
		////	norX2 = norX1;
		////	for (size_t idDim(0); idDim < var1.multiplier(z).size(); ++idDim) {
		////		norX1[idDim] = mapReal<double>(norX1[idDim],
		////			m_min_multiplier, m_max_multiplier, 0, 1);

		////		norX2[idDim] = mapReal<double>(norX2[idDim],
		////			m_min_multiplier, m_max_multiplier, 0, 1);
		////	}

		////	total_multiplier += euclideanDistance(norX1.begin(), norX1.end(), norX2.begin());

		////}
		////m_dis_ratio;
		////return total_pos + total_pos / double(m_num_source);



		//double locationDis(0);
		//double multiplierDis(0);

		//double len = m_max_multiplier - m_min_multiplier;
		//double curMDis(0);

		//std::vector<double> multi1;
		//std::vector<double> multi2;
		//


		//for (size_t z = 0; z < m_num_source; z++) {
		//	//locationDis += calculateDistance(var1.index(z), var2.index(z)) / m_max_location_dis;
		//	if (var1.index(z) != var2.index(z)) locationDis += 1;

		//	curMDis = ofec::euclideanDistance(var1.multiplier(z).begin(), var1.multiplier(z).end(), var2.multiplier(z).begin());
		//	for (auto& it : var1.multiplier(z)) {
		//		multi1.push_back(it);
		//	}
		//	for (auto& it:var2.multiplier(z)) {
		//		multi2.push_back(it);
		//	}

		//	curMDis = curMDis / (len * sqrt(var1.multiplier(z).size()));
		//	multiplierDis += curMDis;
		//}

		//multiplierDis = ofec::euclideanDistance(multi1.begin(), multi1.end(), multi2.begin())/ (len* sqrt(multi1.size()));
		//
		//if (m_distanceType == DistanceType::Mix) {
		//	return locationDis / m_num_source * m_dis_ratio + multiplierDis / m_num_source * (1.0 - m_dis_ratio);
		//}
		//else if (m_distanceType == DistanceType::Continous) {
		//	return multiplierDis;
		//}
		//else {
		//	return  locationDis / m_num_source;
		//}
	}

	Real CSIWDN::variableDistance(const VariableBase& s1, const VariableBase& s2, const std::pair<int, int>& source_idx) const {

		const VarCSIWDN& var1 = dynamic_cast<const VarCSIWDN&>(s1);
		const VarCSIWDN& var2 = dynamic_cast<const VarCSIWDN&>(s2);

		double len = m_max_multiplier - m_min_multiplier;
		double posDis(1.0);
		double multiplierDis(0);
		int q = source_idx.first, z = source_idx.second;

		std::vector<double> multi1;
		std::vector<double> multi2;

		for (size_t k = q; k <= z; k++) {

			for (auto& it : var1.multiplier(k)) {
				multi1.push_back(it);
			}
			for (auto& it : var2.multiplier(k)) {
				multi2.push_back(it);
			}
			if (var1.index(k) != var2.index(k)) {
				posDis *= shortestPath(var1.index(k) - 1, var2.index(k) - 1).size();
			}
		}

		multiplierDis = ofec::euclideanDistance(multi1.begin(), multi1.end(), multi2.begin()) / (len * sqrt(multi1.size()));

		if (m_distanceType == DistanceType::Mix) {
			return posDis + multiplierDis;
		}
		else if (m_distanceType == DistanceType::Continous) {
			return multiplierDis;
		}
		else {
			return posDis;
		}
	}



	void CSIWDN::outputSol(std::ostream &out, ofec::SolutionBase &sol) const
	{
		auto &var = dynamic_cast<const Solution<VarCSIWDN> &>(sol).variable();
		out << sol.fitness() << "\t";
		out << std::endl;
		for (size_t z = 0; z < numberSource(); z++) {
			out << var.index(z) << "\t" << var.multiplier(z).size() << "\t";
			for (auto &j : var.multiplier(z)) {
				out << j << "\t";
			}
			out << std::endl;
		}
	}

	void CSIWDN::inputSol(std::istream &in, ofec::SolutionBase &sol) const
	{

		VarCSIWDN &var = dynamic_cast<Solution<VarCSIWDN> &>(sol).variable();
		var.flagLocation() = false;

		for (size_t z = 0; z < m_num_source; z++) {
			var.source(z) = 1.0;
			var.startTime(z) = optima()->solution(0).variable().startTime(z);
			var.duration(z) = optima()->solution(0).variable().duration(z);
			size_t size = var.duration(z) / m_pattern_step;
			if (var.duration(z) % m_pattern_step != 0)
				++size;
			var.multiplier(z).resize(size);

		}

		double fitness;
		in >> fitness;
		sol.setFitness(fitness);

		int idxZ(0), mSize(0);
		for (size_t z = 0; z < numberSource(); z++) {
			in >> idxZ >> mSize;
			var.index(z) = idxZ;
			double j = 0;
			for (int idj(0); idj < mSize; ++idj) {
				in >> j;
				var.multiplier(z)[idj] = j;
			}
		}




	}


	bool CSIWDN::inputSol(std::stringstream &in, ofec::SolutionBase &sol) const
	{

		VarCSIWDN &var = dynamic_cast<Solution<VarCSIWDN> &>(sol).variable();
		var.flagLocation() = false;

		for (size_t z = 0; z < m_num_source; z++) {
			var.source(z) = 1.0;
			var.startTime(z) = optima()->solution(0).variable().startTime(z);
			var.duration(z) = optima()->solution(0).variable().duration(z);
			size_t size = var.duration(z) / m_pattern_step;
			if (var.duration(z) % m_pattern_step != 0)
				++size;
			var.multiplier(z).resize(size);

		}

		double fitness;
		in >> fitness;
		sol.setFitness(fitness);

		int idxZ(0), mSize(0);
		for (size_t z = 0; z < numberSource(); z++) {
			in >> idxZ >> mSize;
			var.index(z) = idxZ;
			double j = 0;
			for (int idj(0); idj < mSize; ++idj) {
				in >> j;
				var.multiplier(z)[idj] = j;
			}
		}


		return true;



	}

	bool CSIWDN::inputSol(FILE *fp, ofec::SolutionBase &sol)const {


		static char buf[1024];
		static std::stringstream in;


		VarCSIWDN &var = dynamic_cast<Solution<VarCSIWDN> &>(sol).variable();
		var.flagLocation() = false;

		for (size_t z = 0; z < m_num_source; z++) {
			var.source(z) = 1.0;
			var.startTime(z) = optima()->solution(0).variable().startTime(z);
			var.duration(z) = optima()->solution(0).variable().duration(z);
			size_t size = var.duration(z) / m_pattern_step;
			if (var.duration(z) % m_pattern_step != 0)
				++size;
			var.multiplier(z).resize(size);

		}
		if (fgets(buf, 1024, fp) != NULL)
		{
			in << buf;
		}
		else return false;
		int id;
		double fitness;
		in >> id >> fitness;
		sol.setFitness(fitness);

		int idxZ(0), mSize(0);
		for (size_t z = 0; z < numberSource(); z++) {
			if (fgets(buf, 1024, fp) != NULL)
			{
				in << buf;
			}
			else return false;

			in >> idxZ >> mSize;
			var.index(z) = idxZ;
			double j = 0;
			for (int idj(0); idj < mSize; ++idj) {
				in >> j;
				var.multiplier(z)[idj] = j;
			}
		}

		return true;
	}
}




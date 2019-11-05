#include "ESs.h"

namespace OFEC {
	std::mutex mutex_out;

	ESs::ESs(param_map & v) : algorithm(v.at("algName")), m_pop(v.at("popSize"), 1, v.at("subPopSize"), v.at("beta"), v.at("alpha")), m_path_result(v.at("dataFileResult"))
	{

	}

	void ESs::initialize() {
		CAST_RP_EPANET->set_phase(CAST_RP_EPANET->m_total_phase); // offline
		CAST_RP_EPANET->set_init_type(init_type::random);
		m_pop.initialize();
		m_pop.evaluate();
	}

	void ESs::run_() {
		evaluation_tag tag = evaluation_tag::Normal;

		while (tag != evaluation_tag::Terminate) {

			//size_t total_evaluations = CAST_RP_EPANET->total_evaluations();
			//update_flag_best() = false;
			//real curr_best_obj = best()[0]->objective()[0];

			record_intermediate_result();

			//std::cout << "runID: " << global::ms_global.get()->m_runID << " iteraton: " << m_iter << "  total_eval: " << total_evaluations << " coverage: " << m_coverage_result[m_iter] << " best: " << curr_best_obj <<std::endl;

			tag = m_pop.evolve();

		}
		//output();
	}

	void ESs::record() {
		//measure::get_measure()->record(global::ms_global.get(), 0, 0);
	}

	real ESs::cal_coverage_rate() {
		std::vector<bool> coverage(CAST_RP_EPANET->number_node(), false);
		int count = 0;
		
		for (size_t i = 0; i < m_pop.size(); ++i) {
			int index = m_pop[i].variable().index() - 1;
			if (!coverage[index]) { coverage[index] = true; ++count; }
		}
		return (real)count / (real)CAST_RP_EPANET->number_node();
	}

	void ESs::record_intermediate_result() {
		m_coverage_result.push_back(cal_coverage_rate());
		m_curr_best_obj.push_back(m_pop.best()[0]->objective()[0]);
	}

	//void ESs::output() {
	//	mutex_out.lock();
	//	std::stringstream path;
	//	
	//	path << global::ms_arg.at("workingDir") << "result/new/Alg4Epa/ESs/" << m_path_result;

	//	std::ofstream fout(path.str(), std::ios::app);
	//	fout << std::endl;

	//	fout << "runID " << global::ms_global.get()->m_runID << ": " << std::endl;
	//	update_flag_best() = false;
	//	fout << "best solution : " << std::endl;
	//	fout << "location: " << best()[0]->variable().location() << std::endl;
	//	fout << "start time: " << best()[0]->variable().start_time() << std::endl;
	//	fout << "duration: " << best()[0]->variable().duration() << std::endl;
	//	fout << "multiplier: " << std::endl;
	//	for (auto &mul_value : best()[0]->variable().multiplier())
	//		fout << mul_value << "  ";
	//	fout << std::endl;
	//	fout << "obj: " << best()[0]->objective()[0] << std::endl;
	//	fout << "standard deviation: " << cal_standard_deviation(best()[0]->variable().multiplier(), CAST_RP_EPANET->get_optima().variable(0).multiplier()) << std::endl;
	//	fout << std::endl;
	//	std::vector<std::vector<float>> concen_data(4, std::vector<float>(144));
	//	CAST_RP_EPANET->get_data(best()[0]->variable(), concen_data);
	//	for (size_t j = 0; j < 144; ++j) {
	//		fout << concen_data[0][j] << " " << concen_data[1][j] << " " << concen_data[2][j] << " " << concen_data[3][j] << " " << (concen_data[0][j] + concen_data[1][j] + concen_data[2][j] + concen_data[3][j]) / 4.0 << std::endl;
	//	}

	//	for (size_t i = 0; i<m_coverage_result.size(); ++i)
	//		fout << i << "   " << m_coverage_result[i] << std::endl;
	//	int total_evaluations = CAST_RP_EPANET->total_evaluations();
	//	fout << "total_evaluations: " << total_evaluations << std::endl;
	//	fout << "----------------------------------------------------" << std::endl;
	//	fout.close();

	//	std::stringstream path2;
	//	path2 << global::ms_arg.at("workingDir") << "result/new/Alg4Epa/ESs/NET2_97/20w/curr_best_obj.txt";
	//	std::ofstream fout2(path2.str(), std::ios::app);
	//	for (size_t i = 0; i<m_curr_best_obj.size(); ++i)
	//		fout2 << i << "   " << m_curr_best_obj[i] << std::endl;
	//	fout2.close();

	//	mutex_out.unlock();
	//}
}
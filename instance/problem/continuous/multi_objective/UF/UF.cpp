#include "UF.h"
#include <fstream>

namespace OFEC {
	UF::UF(const std::string &name, size_t size_var, size_t size_obj) : problem(name, size_var, size_obj), continuous(name, size_var, 2) {

	}
	void UF::initialize() {
		set_init_range(0., 1.);
		set_range(0., 1.);
		load_PF();
	}

	void UF::load_PF() {
		std::stringstream os;
		//os << global::ms_arg.at("workingDir") << "./instance/problem/continuous/multi_objective/UF/data/" << m_name << ".dat";
		os << "./instance/problem/continuous/multi_objective/UF/data/" << m_name << ".dat";
		std::ifstream infile(os.str());
		if (!infile) {
			//std::cout << "open file is failed" << std::endl;
			throw myexcept("open PF file of UF problem is fail");
			return;
		}
		std::string str;
		size_t line = 0;
		while (getline(infile, str))
			++line;
		m_optima.resize_objective_set(line);
		infile.close();
		infile.clear();
		infile.open(os.str());
		for (size_t i = 0; i < line; i++) {
			std::vector<real> temp_obj(m_objective_size);
			for (size_t j = 0; j < m_objective_size; j++)
				infile >> temp_obj[j];
			m_optima.set_objective(temp_obj, i);
		}
		infile.close();
	}
}
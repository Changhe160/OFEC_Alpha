#include "GLT.h"
#include <fstream>

namespace OFEC {
	GLT::GLT(const std::string &name, size_t size_var, size_t size_obj) : problem(name, size_var, size_obj), continuous(name, size_var, size_obj) {

	}
	void GLT::initialize() {		
		std::vector<std::pair<real, real>> r;
		r.push_back(std::make_pair(0., 1.));
		for (int i = 1; i < m_variable_size; ++i) {
			r.push_back(std::make_pair(-1., 1.));
		}
		set_init_range(r);
		set_range(r);		
		load_PF();
	}

	void GLT::load_PF(){
		std::ifstream infile;
		std::stringstream os;
		os << global::ms_arg.at("workingDir") << "./instance/problem/continuous/multi_objective/GLT/data/" << m_name << ".dat";
		infile.open(os.str());
		if (!infile) {
			//std::cout << "open file is failed" << std::endl;
			throw myexcept("open PF file of GLT problem is fail");
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
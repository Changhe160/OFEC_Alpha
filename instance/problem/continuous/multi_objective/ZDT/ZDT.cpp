#include "ZDT.h"
#include <fstream>

namespace OFEC {
	ZDT::ZDT(const std::string &name, size_t size_var) : problem(name, size_var, 2), continuous(name, size_var, 2) {

	}
	void ZDT::initialize() {
		set_init_range(0., 1.);
		set_range(0., 1.);
		generateAdLoadPF();
		m_initialized = true;
	}

	void ZDT::generateAdLoadPF() {
		size_t num = 1000;
		m_optima.resize_objective_set(num);
		m_optima.resize_variable_set(num);

		std::ofstream out;
		std::ifstream infile;
		std::stringstream os;
		os << "./instance/problem/continuous/multi_objective/ZDT/data/PF_" << m_name << "_Opt_" << num << ".txt";
		infile.open(os.str());

		if (!infile)
		{
			out.open(os.str());
			real temp;
			for (size_t i = 0; i<num; i++)
			{
				variable_vector<real> temp_var(m_variable_size);
				std::vector<real> temp_obj(m_objective_size);
				temp = static_cast<real>(i) / num;
				temp_var[0] = temp;
				for (size_t j = 1; j<m_variable_size; j++)
					temp_var[j] = 0.;
				evaluate_objective(temp_var.data(), temp_obj);
				m_optima.set_variable(temp_var, i);
				m_optima.set_objective(temp_obj, i);
				out << temp_obj[0] << " " << temp_obj[1] << std::endl;
			}
			out.close();
		}
		else
		{
			real temp;
			for (size_t i = 0; i<num; i++)
			{
				variable_vector<real> temp_var(m_variable_size);
				std::vector<real> temp_obj(m_objective_size);
				temp = static_cast<real>(i) / num;
				temp_var[0] = temp;
				for (size_t j = 1; j<m_variable_size; j++)
					temp_var[j] = 0.;
				infile >> temp_obj[0];
				infile >> temp_obj[1];
				m_optima.set_variable(temp_var, i);
				m_optima.set_objective(temp_obj, i);
			}
			infile.close();
		}
	}
}
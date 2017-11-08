#include "ZDT.h"
#include <fstream>

namespace OFEC {
	ZDT::ZDT(const std::string &name, size_t size_var) : problem(name, size_var, 2), continuous(name, size_var, 2) {
		set_range(0., 1.);
		set_tag(std::set<problem_tag>({ problem_tag::MOP, problem_tag::CONT }));
		for (size_t idx = 0; idx < opt_mode().size(); ++idx)
			set_opt_mode(optimization_mode::Minimization, idx);
	}
	void ZDT::generateAdLoadPF() {
		size_t num = 1000;
		m_optima.resize_objective_set(num);
		m_optima.resize_variable_set(num);
		m_optima.resize_variable(m_variable_size);
		m_optima.set_number_variable(num);

		std::ofstream out;
		std::ifstream infile;
		std::stringstream os;
		os << global::ms_arg[param_workingDir] << "FunctionOpt/PF_" << global::ms_arg[param_proName] << "_Opt_" << num << ".txt";
		infile.open(os.str());

		if (!infile)
		{
			out.open(os.str());
			double temp;
			for (size_t i = 0; i<num; i++)
			{
				std::vector<real> temp_var(m_variable_size);
				std::vector<real> temp_obj(m_objective_size);
				temp = static_cast<double>(i) / num;
				temp_var[0] = temp;
				for (size_t j = 1; j<m_variable_size; j++)
					temp_var[j] = 0.;
				evaluate__(temp_var.data(), temp_obj);
				m_optima.set_variable(temp_var, i);
				m_optima.set_objective(temp_obj, i);
				out << temp_obj[0] << " " << temp_obj[1] << std::endl;
			}
			out.close();
		}
		else
		{
			double temp;
			for (size_t i = 0; i<num; i++)
			{
				std::vector<real> temp_var(m_variable_size);
				std::vector<real> temp_obj(m_objective_size);
				temp = static_cast<double>(i) / num;
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
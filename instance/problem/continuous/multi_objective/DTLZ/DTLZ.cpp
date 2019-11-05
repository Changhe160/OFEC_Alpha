#include "DTLZ.h"
#include <fstream>

namespace OFEC {
	DTLZ::DTLZ(const std::string & name, size_t size_var, size_t size_obj) : problem(name, size_var, size_obj), continuous(name, size_var, size_obj){
		if (m_objective_size > m_variable_size) throw myexcept("the number of dim must be greater or eaqual to the number of obj for DTLZ pros");
	
	}
	void DTLZ::initialize() {
		set_init_range(0., 1.);
		set_range(0., 1.);

		load_PF();
		m_initialized = true;
	}

	void DTLZ::generate_PF()
	{
		const std::string problem_name[] = { "MOP_DTLZ1", "MOP_DTLZ2", "MOP_DTLZ3", "MOP_DTLZ4" };
		const int M[5] = { 3, 5, 8, 10, 15 };
		bool flag1(false);
		for (int i = 0; i < 4; ++i) {
			if (m_name == problem_name[i]) {
				flag1 = true;
				break;
			}
		}
		if (!flag1)
			throw myexcept("The problem name should be included in DTLZ1-DTLZ4");
		else {
			bool flag2(false);
			for (int i = 0; i < 5; ++i) {
				if (m_objective_size == M[i]) {
					flag2 = true;
					break;
				}
			}
			if (!flag2)
				throw myexcept("The number of objectives should be included in 3,5,8,10,15");
			else { // generate PF
				std::stringstream os;
				os << "./instance/problem/continuous/objective/DTLZ/data/PF_" << m_name << "_" << m_objective_size << "objs.dtlz";
				std::ofstream ofile(os.str());

				if (m_objective_size == 3)
					generate_PF_onelayer(ofile, m_name, m_objective_size, 12);
				else if (m_objective_size == 5)
					generate_PF_onelayer(ofile, m_name, m_objective_size, 6);
				else if(m_objective_size == 8 || m_objective_size == 10)
					generate_PF_twolayers(ofile, m_name, m_objective_size, 3, 2);
				else
					generate_PF_twolayers(ofile, m_name, m_objective_size, 2, 1);
				ofile.close();
			}
		}

	}
	void DTLZ::load_PF()
	{
		std::stringstream os;
		os << "./instance/problem/continuous/multi_objective/DTLZ/data/PF_" << m_name << "_" << m_objective_size << "objs.dtlz";
		std::ifstream infile(os.str());
		if (!infile)
			generate_PF();
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
	void generate_recursive(TFront * pf, TObjVec * pt, size_t num_objs, size_t left, size_t total, size_t element) {
		if (element == num_objs - 1)
		{
			(*pt)[element] = (real)left;
			pf->push_back(*pt);
		}
		else
		{
			for (size_t i = 0; i <= left; i += 1)
			{
				(*pt)[element] = (real)i;
				generate_recursive(pf, pt, num_objs, left - i, total, element + 1);
			}
		}
	}
	void generate_weight(TFront * pf, size_t M, size_t p)
	{
		TObjVec pt(M);

		generate_recursive(pf, &pt, M, p, p, 0);
	}
	void generate_PF_onelayer(std::ostream & os, const std::string & problem_name, int M, int p) {
		TFront PF;

		int num_objectives = M, num_divisions = p;
		generate_weight(&PF, num_objectives, num_divisions);

		if (problem_name == "MOP_DTLZ1")
		{
			for (size_t i = 0; i<PF.size(); i += 1)
			{
				for (size_t j = 0; j<PF[i].size(); j += 1)
				{
					os << (0.5*PF[i][j]) / num_divisions << ' ';
				}
				os << std::endl;
			}
		}
		else // DTLZ2-4
		{
			for (size_t i = 0; i<PF.size(); i += 1)
			{
				real sum = 0;

				for (size_t j = 0; j<PF[i].size(); j += 1)
				{
					sum += PF[i][j] * PF[i][j];
				}

				real k = sqrt(1.0 / sum);

				for (size_t j = 0; j<PF[i].size(); j += 1)
				{
					os << k*PF[i][j] << ' ';
				}
				os << std::endl;
			}
		}
	}
	void generate_PF_twolayers(std::ostream & os, const std::string & problem_name, int M, int outside_p, int inside_p) {

		generate_PF_onelayer(os, problem_name, M, outside_p);

		TFront PF;

		int num_objectives = M, num_divisions = inside_p;
		generate_weight(&PF, num_objectives, num_divisions);

		for (size_t i = 0; i<PF.size(); i += 1)
		{
			for (size_t j = 0; j<PF[i].size(); j += 1)
			{
				PF[i][j] = (static_cast<real>(num_divisions) / M + PF[i][j]) / 2; // (k=num_divisions/M, k, k, ..., k) is the center point
			}
		}

		if (problem_name == "MOP_DTLZ1")
		{
			for (size_t i = 0; i<PF.size(); i += 1)
			{
				for (size_t j = 0; j<PF[i].size(); j += 1)
				{
					os << (0.5*PF[i][j]) / num_divisions << ' ';
				}
				os << std::endl;
			}
		}
		else // DTLZ2-4
		{
			for (size_t i = 0; i < PF.size(); i += 1)
			{
				real sum = 0;

				for (size_t j = 0; j < PF[i].size(); j += 1)
				{
					sum += PF[i][j] * PF[i][j];
				}

				real k = sqrt(1.0 / sum);

				for (size_t j = 0; j < PF[i].size(); j += 1)
				{
					os << k*PF[i][j] << ' ';
				}
				os << std::endl;
			}
		}
	}
}

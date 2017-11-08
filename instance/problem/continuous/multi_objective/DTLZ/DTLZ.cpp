#include "DTLZ.h"
#include <fstream>

namespace OFEC {
	DTLZ::DTLZ(const std::string & name, size_t size_var, size_t size_obj) : problem(name, size_var, size_obj), continuous(name, size_var, size_obj){
		if (m_objective_size > m_variable_size) throw myexcept("the number of dim must be greater or eaqual to the number of obj for DTLZ pros");
		set_range(0., 1.);
		set_tag(std::set<problem_tag>({ problem_tag::MOP, problem_tag::CONT }));
		for (size_t idx = 0; idx < opt_mode().size(); ++idx)
			set_opt_mode(optimization_mode::Minimization, idx);
		generateAdLoadPF();
	}
	void DTLZ::generateAdLoadPF() {
		const std::string problem_name[] = { "DTLZ1", "DTLZ2", "DTLZ3", "DTLZ4" };
		std::stringstream os;
		os << global::ms_arg[param_workingDir] << "FunctionOpt/PF_" << global::ms_arg[param_proName] << "(" << global::ms_arg[param_numObj] << ")" << "_Opt.txt";

		for (size_t i = 0; i<4; i += 1) // problem
		{
			if (global::ms_arg[param_proName] != problem_name[i])
				continue;
			const int M[5] = { 3, 5, 8, 10, 15 };
			for (size_t j = 0; j<5; j += 1) // objectives
			{
				if (global::ms_arg[param_numObj] != M[j])
					continue;
				std::ifstream infile(os.str());
				if (infile)
				{
					infile.close(); break;
				}
				infile.close();
				std::ofstream ofile(os.str());

				if (M[j] <= 5) // #objectives <= 5
				{
					int p[2] = { 12, 6 }; // Check Section V, Table I in the original paper
					GeneratePF_OneLayer(ofile, problem_name[i], M[j], p[j]);
				}
				else
				{
					int p[3][2] = { { 3, 2 },{ 3, 2 },{ 2, 1 } }; // Check Section V, Table I in the original paper
					GeneratePF_TwoLayers(ofile, problem_name[i], M[j], p[j - 2][0], p[j - 2][1]);
				}
				ofile.close();
			}
		}
		size_t numObj = global::ms_arg[param_numObj];
		std::ifstream infile(os.str());
		if (!infile)
			throw myexcept("please std::set your own pareto front @DTLZ::generatePF()");
		std::string str;
		size_t line = 0;
		while (getline(infile, str))
			++line;
		m_optima.resize_objective_set(line);
		infile.close();
		infile.clear();
		infile.open(os.str());
		for (size_t i = 0; i < line; i++) {
			std::vector<real> temp_obj(numObj);
			for (size_t j = 0; j < numObj; j++)
				infile >> temp_obj[j];
			m_optima.set_objective(temp_obj, i);
		}
		infile.close();
	}

	void generate_recursive(TFront * pf, TObjVec * pt, size_t num_objs, size_t left, size_t total, size_t element) {
		if (element == num_objs - 1)
		{
			(*pt)[element] = left;
			pf->push_back(*pt);
		}
		else
		{
			for (size_t i = 0; i <= left; i += 1)
			{
				(*pt)[element] = i;
				generate_recursive(pf, pt, num_objs, left - i, total, element + 1);
			}
		}
	}
	void GenerateWeight(TFront * pf, size_t M, size_t p)
	{
		TObjVec pt(M);

		generate_recursive(pf, &pt, M, p, p, 0);
	}
	void GeneratePF_OneLayer(std::ostream & os, const std::string & problem_name, int M, int p) {
		TFront PF;

		int num_objectives = M, num_divisions = p;
		GenerateWeight(&PF, num_objectives, num_divisions);

		if (problem_name == "FUN_MOP_DTLZ1")
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
				double sum = 0;

				for (size_t j = 0; j<PF[i].size(); j += 1)
				{
					sum += PF[i][j] * PF[i][j];
				}

				double k = sqrt(1.0 / sum);

				for (size_t j = 0; j<PF[i].size(); j += 1)
				{
					os << k*PF[i][j] << ' ';
				}
				os << std::endl;
			}
		}
	}
	void GeneratePF_TwoLayers(std::ostream & os, const std::string & problem_name, int M, int outside_p, int inside_p) {

		GeneratePF_OneLayer(os, problem_name, M, outside_p);

		TFront PF;

		int num_objectives = M, num_divisions = inside_p;
		GenerateWeight(&PF, num_objectives, num_divisions);

		for (size_t i = 0; i<PF.size(); i += 1)
		{
			for (size_t j = 0; j<PF[i].size(); j += 1)
			{
				PF[i][j] = (static_cast<double>(num_divisions) / M + PF[i][j]) / 2; // (k=num_divisions/M, k, k, ..., k) is the center point
			}
		}

		if (problem_name == "FUN_MOP_DTLZ1")
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
				double sum = 0;

				for (size_t j = 0; j < PF[i].size(); j += 1)
				{
					sum += PF[i][j] * PF[i][j];
				}

				double k = sqrt(1.0 / sum);

				for (size_t j = 0; j < PF[i].size(); j += 1)
				{
					os << k*PF[i][j] << ' ';
				}
				os << std::endl;
			}
		}
	}
}

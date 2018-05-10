#include "multi_dimensional_knapsack.h"
#include <fstream>
#include <numeric>

namespace OFEC {
	multi_dimensional_knapsack::multi_dimensional_knapsack(param_map &v) : multi_dimensional_knapsack(v.at("proName"), v.at("numDim"), v.at("dataFile1"))
	{
		
	}

	multi_dimensional_knapsack::multi_dimensional_knapsack(const std::string & pro_name, size_t size_var, const std::string & file_name)
		: problem(pro_name, size_var, 1)
	{
		m_file_name = file_name;
	}
	void multi_dimensional_knapsack::initialize() {
		read_problem();

		global::ms_sample_fre  = m_variable_size * 2;
		for (size_t idx = 0; idx < m_opt_mode.size(); ++idx)
			m_opt_mode[idx] = optimization_mode::Maximization;

		allocate_memory<solution<variable_vector<int>, real>>(m_variable_size, m_objective_size);
	}
	evaluation_tag multi_dimensional_knapsack::evaluate_(solution_base &s, caller call, bool effective_fes, bool constructed)
	{
		variable_vector<int> &x = dynamic_cast< solution<variable_vector<int>, real> &>(s).variable();
		std::vector<double> &obj = dynamic_cast< solution<variable_vector<int>, real> &>(s).objective();
		int m = invalid_constrain_num(s);
		for (size_t i = 0; i < m_objective_size; ++i)
			obj[i] = 0;
		for (size_t n = 0; n < m_objective_size; ++n) {
			for (size_t i = 0; i < m_variable_size; ++i)
				obj[n] += mv_p[i] * x[i];
			obj[n] -= m * m_maxP;
		}
		if (constructed) {
			if (effective_fes)		m_effective_eval++;
			m_optima.is_optimal_objective(obj, m_objective_accuracy);
			if (m_optima.is_objective_found())
				m_solved = true;
			if (call == caller::Algorithm&& global::ms_global->m_algorithm&&global::ms_global->m_algorithm->terminating())
				return evaluation_tag::Terminate;
		}
		return evaluation_tag::Normal;
	}

	bool multi_dimensional_knapsack::is_valid(const solution_base & s) const
	{
		if (!m_if_valid_check) return true;
		const variable_vector<int> &x = dynamic_cast<const solution<variable_vector<int>, real> &>(s).variable();
		for (int i = 0; i < m_variable_size; i++) {
			if (x[i] != 0 && x[i] != 1)
				return false;
		}
		return true;
	}

	void multi_dimensional_knapsack::read_problem()
	{
		size_t i;
		std::string Line;
		std::ostringstream oss;
		std::ifstream infile;
		oss << "./instance/problem/combination/MKP/data/" << m_file_name << ".mkp";
		infile.open(oss.str().c_str());
		if (!infile) {
			throw myexcept("read Multidimensional Knapsack data error");
		}
		infile >> Line;
		m_variable_size = atoi(Line.c_str());
		infile >> Line;
		m_m = atoi(Line.c_str());
		infile >> Line;
		double temp = atof(Line.c_str());
		if (temp != 0)
			m_optima.append(temp);
		mv_p.resize(m_variable_size);
		mv_b.resize(m_m);
		mvv_r.resize(m_m);
		for (i = 0; i < m_m; i++) {
			mvv_r[i].resize(m_variable_size);
		}
		for (i = 0; i < m_variable_size; i++)
		{
			infile >> mv_p[i];
			if (i == 0) {
				m_maxP = mv_p[i];
			}
			else if (m_maxP < mv_p[i]) {
				m_maxP = mv_p[i];
			}
		}
		for (i = 0; i < m_m; i++) {
			for (int j = 0; j < m_variable_size; j++) {
				infile >> mvv_r[i][j];
			}
		}
		for (i = 0; i < m_m; i++) {
			infile >> mv_b[i];
		}
		infile.close();
		infile.clear();
	}

	int multi_dimensional_knapsack::invalid_constrain_num(solution_base & s) const
	{
		const variable_vector<int> &x = dynamic_cast<const solution<variable_vector<int>, real> &>(s).variable();
		int n = 0;
		double sum;
		for (int i = 0; i < m_m; i++) {
			sum = 0;
			for (int j = 0; j < m_variable_size; j++) {
				sum += mvv_r[i][j] * x[j];
			}
			if (sum > mv_b[i]) n++;
		}
		return n;
	}

	bool multi_dimensional_knapsack::get_optimal_obj(std::vector<double>& opt) const
	{
		if (m_optima.objective_given()) {
			opt = m_optima.multi_objective();
			return true;
		}
		return false;
	}

	bool multi_dimensional_knapsack::get_optimal_obj(std::vector<std::vector<double>>& opt) const
	{
		if (m_optima.objective_given()) {
			opt.clear();
			for (unsigned i = 0; i < m_optima.number_objective() ; ++i)
				opt.push_back(m_optima.multi_objective(i));
			return true;
		}
		return false;
	}
	const optima<variable_vector<int>, real>& multi_dimensional_knapsack::get_optima() const
	{
		return m_optima;
	}
	optima<variable_vector<int>, real>& multi_dimensional_knapsack::get_optima()
	{
		return m_optima;
	}
	bool multi_dimensional_knapsack::is_optima_given() const
	{
		return m_optima.objective_given();
	}
	multi_dimensional_knapsack * multi_dimensional_knapsack::get_type_ptr()
	{
		return this;
	}
	multi_dimensional_knapsack & multi_dimensional_knapsack::get_type_ref()
	{
		return *this;
	}
	bool multi_dimensional_knapsack::if_optima_found()
	{
		if (is_optima_given()) {
			if (m_optima.is_objective_found())
				return true;
		}
		return false;
	}
	double multi_dimensional_knapsack::get_constraint_value(const solution_base & s, std::vector<double>& val) const
	{
		const variable_vector<int> &x = dynamic_cast<const solution<variable_vector<int>, real> &>(s).variable();
		val.resize(m_m);
		double sum = 0;
		for (int i = 0; i < m_m; ++i) {
			val[i] = 0;
			for (int j = 0; j < m_variable_size; ++j)
				val[i] += mvv_r[i][j] * x[j];
			val[i] = sum - mv_b[i] < 0 ? 0 : sum - mv_b[i];
		}
		return std::accumulate(val.begin(), val.end(), 0.0);
	}

	void multi_dimensional_knapsack::initialize_solution(solution_base & s) const
	{
		variable_vector<int> &x = dynamic_cast<solution<variable_vector<int>, real> &>(s).variable();
		for (int i = 0; i < m_variable_size; ++i)
			x[i] = global::ms_global->m_uniform[caller::Problem]->next_non_standard(0, 2);
		if (!is_valid(s))
			throw myexcept("error in @multi_dimensional_knapsack::initialize_solution() in multi_dimensional_knapsack.cpp");
	}

	bool multi_dimensional_knapsack::same(const solution_base & s1, const solution_base & s2) const
	{
		const variable_vector<int> &x1 = dynamic_cast<const solution<variable_vector<int>, real> &>(s1).variable();
		const variable_vector<int> &x2 = dynamic_cast<const solution<variable_vector<int>, real> &>(s2).variable();
		for (int i = 0; i < m_variable_size; i++)
			if (x1[i] != x2[i])
				return false;
		return true;
	}

	double multi_dimensional_knapsack::variable_distance(const solution_base & s1, const solution_base & s2) const
	{
		const variable_vector<int> &x1 = dynamic_cast<const solution<variable_vector<int>, real> &>(s1).variable();
		const variable_vector<int> &x2 = dynamic_cast<const solution<variable_vector<int>, real> &>(s2).variable();
		double dis = 0;
		for (int i = 0; i < m_variable_size; i++)
			if (x1[i] != x2[i])
				dis++;
		return dis;
	}

	double multi_dimensional_knapsack::variable_distance(const variable_base & s1, const variable_base & s2) const
	{
		const variable_vector<int> &x1 = dynamic_cast<const variable_vector<int>&>(s1);
		const variable_vector<int> &x2 = dynamic_cast<const variable_vector<int>&>(s2);
		double dis = 0;
		for (int i = 0; i < m_variable_size; i++)
			if (x1[i] != x2[i])
				dis++;
		return dis;
	}
}
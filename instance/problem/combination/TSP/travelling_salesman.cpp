#include "travelling_salesman.h"
#include <fstream>
#include <cstring>


namespace OFEC {

	travelling_salesman::travelling_salesman(param_map & v)	: travelling_salesman(v.at("proName"), v.at("numDim"), v.at("numObj"), v.at("dataFile1")) {
		

	}

	travelling_salesman::travelling_salesman(const std::string& pro_name, int size_var, int size_obj, const std::string& file_name)
		: problem(pro_name, size_var, size_obj), m_domain(m_variable_size) {	
		m_file_name = file_name;
	}
	void travelling_salesman::initialize() {
		if (m_variable_size <= 20) global::ms_sample_fre  = 10;
		else if (m_variable_size <= 50) global::ms_sample_fre  = 100;
		else if (m_variable_size <= 100) global::ms_sample_fre  = 500;
		else if (m_variable_size <= 200) global::ms_sample_fre  = 10000;
		else if (m_variable_size <= 300) global::ms_sample_fre  = 20000;
		else if (m_variable_size <= 400) global::ms_sample_fre  = 50000;
		else if (m_variable_size <= 500) global::ms_sample_fre  = 100000;
		else if (m_variable_size <= 600) global::ms_sample_fre  = 200000;
		else if (m_variable_size <= 700) global::ms_sample_fre  = 500000;
		else if (m_variable_size <= 800) global::ms_sample_fre  = 1000000;
		else if (m_variable_size <= 900) global::ms_sample_fre  = 2000000;
		else if (m_variable_size <= 1000) global::ms_sample_fre  = 3000000;
		else if (m_variable_size <= 2000) global::ms_sample_fre  = 5000000;

		mvvv_cost.resize(m_objective_size);
		for (size_t i = 0; i<m_objective_size; i++)
			mvvv_cost[i].resize(m_variable_size);
		for (size_t i = 0; i<m_objective_size; i++)
			for (size_t j = 0; j<m_variable_size; j++)
				mvvv_cost[i][j].resize(m_variable_size);
	
		read_problem();

		for (size_t i = 0; i < m_variable_size; ++i)
			m_domain.set_range(0, m_variable_size - 1, i);
		m_initialized = true;
	}
	evaluation_tag travelling_salesman::evaluate_(solution_base & s, caller call, bool effective, bool constructed)
	{
		variable_vector<int> &x = dynamic_cast< solution<variable_vector<int>, real> &>(s).variable();
		std::vector<real> &obj = dynamic_cast< solution<variable_vector<int>, real> &>(s).objective();

		for (size_t i = 0; i<m_objective_size; i++)
			obj[i] = 0;
		int row, col;
		for (size_t n = 0; n<m_objective_size; n++)
		{
			for (size_t i = 0; i<m_variable_size; i++)
			{
				row = x[i];
				col = x[(i + 1) % m_variable_size];
				obj[n] += mvvv_cost[n][row][col];
			}
		}

		if (constructed) {
			if (effective)		m_effective_eval++;
			m_optima.is_optimal_objective(obj, m_objective_accuracy);
			if (m_optima.is_objective_found())
				m_solved = true;
			if (call == caller::Algorithm&& global::ms_global->m_algorithm&&global::ms_global->m_algorithm->terminating())
				return evaluation_tag::Terminate;
		}
		return evaluation_tag::Normal;
	}

	bool travelling_salesman::is_valid(solution_base & s) const
	{
		if (!m_if_valid_check)
			return true;

		const variable_vector<int> &x = dynamic_cast<const solution<variable_vector<int>, real> &>(s).variable();

		for (size_t i = 0; i < m_variable_size; i++)  //judge the range		
			if ((x[i]) < m_domain.range(i).limit.first || (x[i]) > m_domain.range(i).limit.second)
				return false;

		std::vector<int> flag(m_variable_size, 0);  //judge whether has the same gene
		int temp;
		for (size_t i = 0; i<m_variable_size; i++)
		{
			temp = x[i];
			flag[temp] = 1;
		}
		for (size_t i = 0; i<m_variable_size; i++)
			if (flag[i] == 0)
				return false;
		return true;
	}

	void travelling_salesman::initialize_solution(solution_base & s) const
	{
		variable_vector<int>& x = dynamic_cast< solution<variable_vector<int>, real>&>(s).variable();
		global::ms_global->m_uniform[caller::Problem]->shuffle(x.begin(), x.end());
		if (!is_valid(s))
			throw myexcept("error in @travelling_salesman::initialize_solution() in travelling_salesman.cpp");
	}

	bool travelling_salesman::same(const solution_base & s1, const solution_base & s2) const
	{
		const variable_vector<int> &x1 = dynamic_cast<const solution<variable_vector<int>, real> &>(s1).variable();
		const variable_vector<int> &x2 = dynamic_cast<const solution<variable_vector<int>, real> &>(s2).variable();
		int i, j, pos = 0;
		for (i = 0; i<m_variable_size; i++)
		{
			if (x1[0] == x2[i])
			{
				pos = i;
				break;
			}
		}
		j = pos;
		for (i = 0; i<m_variable_size; i++)
		{
			if (x1[i] != x2[j])  break;
			else j = (j + 1) % m_variable_size;
		}
		if (i == m_variable_size) return true;
		j = pos;
		for (i = 0; i<m_variable_size; i++)
		{
			if (x1[i] != x2[j])  break;
			else j = (j - 1 + m_variable_size) % m_variable_size;
		}
		if (i == m_variable_size) return true;
		return false;
	}

	real travelling_salesman::variable_distance(const solution_base & s1, const solution_base & s2) const
	{
		static std::vector<std::vector<bool>> mvv_s1_edge(m_variable_size); // a temporary matrix to store the edges of s1, thus helping calculate the distance between two solutions
		const variable_vector<int> &x1 = dynamic_cast<const solution<variable_vector<int>, real> &>(s1).variable();
		const variable_vector<int> &x2 = dynamic_cast<const solution<variable_vector<int>, real> &>(s2).variable();

		for (auto& row : mvv_s1_edge)
			row.assign(m_variable_size, false);
		for (size_t i = 0; i<m_variable_size; i++) {
			mvv_s1_edge[x1[i]][x1[(i + 1) % m_variable_size]] = true;
		}
		real dis = 0;
		for (size_t i = 0; i<m_variable_size; i++) {
			if (!mvv_s1_edge[x2[i]][x2[(i + 1) % m_variable_size]])dis += 1;
		}
		return dis;
	}

	real travelling_salesman::variable_distance(const variable_base & s1, const variable_base & s2) const
	{
		static std::vector<std::vector<bool>> mvv_s1_edge(m_variable_size); // a temporary matrix to store the edges of s1, thus helping calculate the distance between two solutions
		const variable_vector<int> &x1 = dynamic_cast<const variable_vector<int>&>(s1);
		const variable_vector<int> &x2 = dynamic_cast<const variable_vector<int>&>(s2);

		for (auto& row : mvv_s1_edge)
			row.assign(m_variable_size, false);
		for (size_t i = 0; i<m_variable_size; i++) {
			mvv_s1_edge[x1[i]][x1[(i + 1) % m_variable_size]] = true;
		}
		real dis = 0;
		for (size_t i = 0; i<m_variable_size; i++) {
			if (!mvv_s1_edge[x2[i]][x2[(i + 1) % m_variable_size]])dis += 1;
		}
		return dis;
	}

	std::pair<int, int> travelling_salesman::get_next_city(const solution_base & s, int city) const
	{
		const variable_vector<int> &x = dynamic_cast<const solution<variable_vector<int>, real> &>(s).variable();
		for (size_t i = 0; i<m_variable_size; i++) {
			if (x[i] == city)
				return std::pair<int, int>(x[(i - 1 + m_variable_size) % m_variable_size], x[(i + 1) % m_variable_size]);
		}
		throw myexcept("error@TravellingSalesman::get_next_city()");
	}

	void travelling_salesman::find_nearby_city(std::vector<std::vector<int>>& nearby, int num_near, int obj)
	{
		int i, j, z, pos;
		real min;
		std::vector<int> visited(m_variable_size);
		int num = num_near == 0 ? nearby[0].size() : num_near;
		for (i = 0; i < m_variable_size; i++)
		{
			for (j = 0; j < m_variable_size; j++)
				visited[j] = 0;
			for (j = 0; j < num - 1; j++)
			{
				min = 0xfffffff;
				for (z = 0; z < m_variable_size; z++)
				{
					if (visited[z] == 0 && i != z && min > mvvv_cost[obj][i][z])
					{
						min = mvvv_cost[obj][i][z];
						pos = z;
					}
				}
				visited[pos] = 1;
				nearby[i][j] = pos;
			}
		}
	}

	void travelling_salesman::prim(std::vector<std::vector<int>>& mst_edge, int n)
	{
		std::vector<int> near(m_variable_size);
		int i, j, k, l;
		int Min = mvvv_cost[n][0][1];
		k = 0; l = 1;
		for (i = 0; i<m_variable_size; i++)  //find the nearest edge in the graph
		{
			for (j = 0; j<m_variable_size; j++)
			{
				if (Min>mvvv_cost[n][i][j] && i != j)
				{
					Min = mvvv_cost[n][i][j];
					k = i; l = j;
				}
			}
		}
		mst_edge[0][0] = k; mst_edge[0][1] = l;
		for (i = 0; i<m_variable_size; i++)  //initial near
		{
			if (mvvv_cost[n][i][l]<mvvv_cost[n][i][k])
				near[i] = l;
			else near[i] = k;
		}
		near[l] = -1; near[k] = -1;
		for (i = 2; i<m_variable_size; i++)//find the remain n-2 edges
		{
			Min = 0xfffffff;
			for (int z = 0; z<m_variable_size; z++)
			{
				if (near[z] != -1 && Min>mvvv_cost[n][z][near[z]])
				{
					Min = mvvv_cost[n][z][near[z]];
					j = z;
				}
			}
			mst_edge[i - 1][0] = j; mst_edge[i - 1][1] = near[j];
			near[j] = -1;
			for (k = 0; k<m_variable_size; k++)
				if (near[k] != -1 && mvvv_cost[n][k][near[k]]>mvvv_cost[n][k][j])
					near[k] = j;
		}
	}

	void travelling_salesman::calculate_edge_weight(char * edge_type, std::vector<std::vector<real>>& coordinate)
	{
		if (!strcmp(edge_type, "EUC_2D"))
		{
			for (size_t i = 0; i<m_variable_size; i++)
				for (size_t j = 0; j<m_variable_size; j++)
					mvvv_cost[0][i][j] = static_cast<int>(sqrt(pow(coordinate[i][0] - coordinate[j][0], 2) + pow(coordinate[i][1] - coordinate[j][1], 2)) + 0.5);
		}
		else if (!strcmp(edge_type, "ATT"))
		{
			real r;
			int t;
			for (size_t i = 0; i<m_variable_size; i++)
			{
				for (size_t j = 0; j<m_variable_size; j++)
				{
					r = sqrt((pow(coordinate[i][0] - coordinate[j][0], 2) + pow(coordinate[i][1] - coordinate[j][1], 2)) / 10.0);
					t = static_cast<int>(r + 0.5);
					if (t<r) mvvv_cost[0][i][j] = t + 1;
					else mvvv_cost[0][i][j] = t;
				}
			}
		}
		else if (!strcmp(edge_type, "GEO"))
		{
			real pi = 3.141592, RRR = 6378.388, min;
			int deg;
			for (size_t i = 0; i<m_variable_size; i++)
			{
				for (size_t j = 0; j<2; j++)
				{
					deg = static_cast<int>(coordinate[i][j]);
					min = coordinate[i][j] - deg;
					coordinate[i][j] = pi * (deg + 5.0*min / 3.0) / 180.0;
				}
			}
			real q1, q2, q3;
			for (size_t i = 0; i<m_variable_size; i++)
			{
				for (size_t j = 0; j<m_variable_size; j++)
				{
					q1 = cos(coordinate[i][1] - coordinate[j][1]);
					q2 = cos(coordinate[i][0] - coordinate[j][0]);
					q3 = cos(coordinate[i][0] + coordinate[j][0]);
					mvvv_cost[0][i][j] = static_cast<int>(RRR*acos(0.5*((1.0 + q1)*q2 - (1.0 - q1)*q3)) + 1.0);
				}
			}
		}
		else
			throw myexcept("no exiting this edgeType in function calculate_edge_weight in travelling_salesman.cpp, please add it here!");
	}

	bool travelling_salesman::is_optima_given()
	{
		return m_optima.objective_given() && m_optima.variable_given();
	}

	bool travelling_salesman::is_optima_found()
	{
		if (is_optima_given())
			if (m_optima.is_objective_found() && m_optima.is_variable_found())
				return true;
		return false;
	}

	void travelling_salesman::read_problem()
	{

#if defined(linux) || defined(__linux) || defined(__linux__) || defined(__APPLE__)
#define	strtok_s strtok_r
#endif

		size_t i;
		std::string Line;
		char *edgeType = 0, *edgeFormat = 0;
		char *Keyword = 0;
		char *Type = 0, *Format = 0;
		const char *Delimiters = " ():=\n\t\r\f\v\xef\xbb\xbf";
		std::vector<std::vector<real> > coordinate;
		std::ostringstream oss1, oss2;
		std::ifstream infile;
		oss1 << "./instance/problem/combination/TSP/data/" << m_file_name << ".tsp";
		infile.open(oss1.str().c_str());
		if (!infile) {
			throw myexcept("read travelling salesman data error");
		}
		char *savePtr;
		while (std::getline(infile, Line))
		{
			if (!(Keyword = strtok_s((char*)Line.c_str(), Delimiters, &savePtr)))
				continue;
			for (i = 0; i<strlen(Keyword); i++)
				Keyword[i] = toupper(Keyword[i]);
			if (!strcmp(Keyword, "NAME"))
				continue;
			else if (!strcmp(Keyword, "COMMENT"))
				continue;
			else if (!strcmp(Keyword, "TYPE"))
				continue;
			else if (!strcmp(Keyword, "DIMENSION"))
			{
				char *token = strtok_s(0, Delimiters, &savePtr);
				m_variable_size = atoi(token);
			}
			else if (!strcmp(Keyword, "EDGE_WEIGHT_TYPE"))
			{
				edgeType = strtok_s(0, Delimiters, &savePtr);
				Type = new char[strlen(edgeType)+1];
				strncpy(Type, edgeType, strlen(edgeType) + 1);
			}
			else if (!strcmp(Keyword, "EDGE_WEIGHT_FORMAT"))
			{
				edgeFormat = strtok_s(0, Delimiters, &savePtr);
				Format = new char[strlen(edgeFormat)+1];
				strncpy(Format, edgeFormat, strlen(edgeFormat) + 1);
			}
			else if (!strcmp(Keyword, "NODE_COORD_SECTION"))
			{
				i = 0;
				std::vector<real> temp(2);
				while (infile >> Line)
				{
					infile >> temp[0];
					infile >> temp[1];
					coordinate.push_back(temp);
					++i;
					if (i == (size_t)m_variable_size) break;
				}
				break;
			}
			else if (!strcmp(Keyword, "EDGE_WEIGHT_SECTION"))
			{
				if (!strcmp(Format, "LOWER_DIAG_ROW"))
				{
					for (size_t i = 0; i<m_variable_size; i++)
						for (size_t j = 0; j <= i; j++)
						{
							infile >> mvvv_cost[0][i][j];
							mvvv_cost[0][j][i] = mvvv_cost[0][i][j];
						}
				}
				else if (!strcmp(Format, "UPPER_DIAG_ROW"))
				{
					for (size_t i = 0; i<m_variable_size; i++)
						for (int j = i; j<m_variable_size; j++)
						{
							infile >> mvvv_cost[0][i][j];
							mvvv_cost[0][j][i] = mvvv_cost[0][i][j];
						}
				}
				else if (!strcmp(Format, "FULL_MATRIX"))
				{
					for (size_t i = 0; i<m_variable_size; i++)
						for (size_t j = 0; j<m_variable_size; j++)
							infile >> mvvv_cost[0][i][j];
				}
				else
					throw myexcept("no exiting this edgeFormat in function readProblem in travelling_salesman.cpp, please add it here!");
			}
		}
		infile.close();
		infile.clear();
		if (!Type)
			throw myexcept("file format error in function readProblem in TravellingSalesman.cpp");
		if (strcmp(Type, "EXPLICIT"))
			calculate_edge_weight(Type, coordinate);
		if (Type) 
			delete []Type;
		if (Format) 
			delete []Format;
		oss2 << "./instance/problem/combination/TSP/data/" << m_file_name << ".opt.tour";
		infile.open(oss2.str().c_str());
		if (!infile)
			throw myexcept("read travelling salesman optimal tour file error");
		std::string oldLine;
		while (getline(infile, Line))
		{
			oldLine = Line;
			if (!(Keyword = strtok_s((char*)Line.c_str(), Delimiters, &savePtr)))			
				continue;
						
			for (i = 0; i<strlen(Keyword); i++)
				Keyword[i] = toupper(Keyword[i]);
			if (!strcmp(Keyword, "TOUR_SECTION"))
			{
				variable_vector<int> temp_opt_var(m_variable_size);
				int temp;
				for (i = 0; i<(size_t)m_variable_size; i++)
				{
					infile >> temp;
					--temp;
					temp_opt_var[i] = typevar(temp);
				}
				m_optima.append(temp_opt_var);
				//evaluate_(m_globalOpt[0].data(), false, Program_Problem, false);
				break;
			}
			else if (!strcmp(Keyword, "COMMENT"))  //get optimal cost
			{
				size_t j = 0;
				for (int z = oldLine.size() - 1; z >= 0; z--)
					Line[j++] = oldLine[z];
				Keyword = strtok_s((char*)Line.c_str(), Delimiters, &savePtr);				
				size_t len = strlen(Keyword) - 1;
				for (i = 0; i <= len; i++)
					oldLine[i] = Keyword[len - i];
				oldLine[len + 1] = '\0';
				m_optima.append(atof(oldLine.c_str()));
			}
			else
				continue;
		}
		infile.close();
		infile.clear();
	}

	int select_city_random(std::vector<std::vector<int>>& matrix, std::vector<int>& visited, int num, int row)
	{
		int i, pos, flag = -1;
		std::vector<int> arr(num);
		for (i = 0; i<num; i++)
			arr[i] = matrix[row][i];
		i = num - 1;
		while (i >= 0)
		{
			pos = int(i*global::ms_global->m_uniform[caller::Algorithm]->next());
			if (visited[arr[pos]] == 1)
			{
				arr[pos] = arr[i];
				i--;
			}
			else
			{
				visited[arr[pos]] = 1;
				flag = arr[pos];
				break;
			}
		}
		return flag;
	}

	int select_city_random(std::vector<int> visited, int dim)
	{
		int i, pos, flag = -1;
		std::vector<int> arr(dim);
		for (i = 0; i<dim; i++)
			arr[i] = i;
		i = dim - 1;
		while (i >= 0)
		{
			pos = int(i*global::ms_global->m_uniform[caller::Algorithm]->next());
			if (visited[arr[pos]] == 1)
			{
				arr[pos] = arr[i];
				i--;
			}
			else
			{
				visited[arr[pos]] = 1;
				flag = arr[pos];
				break;
			}
		}
		return flag;
	}

	int select_city_greedy(std::vector<std::vector<int>>& matrix, std::vector<int>& visited, int num, int row)
	{
		int i, flag = -1;
		for (i = 0; i < num; i++)
		{
			if (visited[matrix[row][i]] == 1)
				continue;
			else
			{
				visited[matrix[row][i]] = 1;
				flag = matrix[row][i];
				break;
			}
		}
		return flag;
	}
}
#include "travelling_salesman.h"
#include <fstream>
#include <cstring>

#include "../../../../core/global.h"
#include "../../../../core/problem/solution.h"
#include "../../../../core/problem/optima.h"

#include "../../../../core/environment/environment.h"

namespace ofec {

	void TravellingSalesman::addInputParameters() {


	
			std::ostringstream oss1;
			oss1 << static_cast<std::string>(g_working_directory);
			oss1 << "/instance/problem/combination/travelling_salesman/";
		//	m_file_path = oss1.str();
		
		m_input_parameters.add("dataFile1", new FileName(m_file_name,
			"instance/problem/combination/travelling_salesman",
			"map (*.tsp)", "A280.tsp"));

		m_input_parameters.add("dataDirectory", new DirectoryName(m_file_path, oss1.str()));
	}

	void TravellingSalesman::initialize_(Environment* env) {
		Problem::initialize_(env);




		//auto &v = *m_param;
		//m_file_name = v.get<std::string>("dataFile1");
		resizeObjective(1);
		readProblem();

		m_hash.initialize(m_random.get(), m_number_variables + 10);
		m_number_variables = m_number_variables;
	}

	void TravellingSalesman::updateOptima(Environment* env) {
		ProblemVariableVector::updateOptima(env);
		readOptima();
	}

	void TravellingSalesman::evaluate(const VariableBase& vars, std::vector<Real>& objs, std::vector<Real>& cons)const {
		const VariableType& x = dynamic_cast<const VariableType&>(vars);
		std::vector<int> x_(x.begin(), x.end()); //for parallel running
		objs.resize(m_number_objectives);
		evaluateObjective(x_.data(), objs);
	}


	void TravellingSalesman::evaluateObjective(int* x, std::vector<Real>& obj)const {
		for (size_t i = 0; i < m_number_objectives; i++)
			obj[i] = 0;
		int row, col;
		for (size_t n = 0; n < m_number_objectives; n++) {
			for (size_t i = 0; i < m_number_variables; i++) {
				row = x[i];
				col = x[(i + 1) % m_number_variables];
				obj[n] += mvvv_cost[n][row][col];
			}
		}
	}

	bool TravellingSalesman::isValid(const VariableBase &x_) const {
		if (!m_if_valid_check)
			return true;

		auto &x = dynamic_cast<const VariableType&>(x_);

		for (size_t i = 0; i < m_number_variables; i++)  //judge the range		
			if ((x[i]) < 0 || (x[i]) >= m_number_variables)
				return false;

		std::vector<int> flag(m_number_variables, 0);  //judge whether has the same gene
		int temp;
		for (size_t i = 0; i < m_number_variables; i++)
		{
			temp = x[i];
			flag[temp] = 1;
		}
		for (size_t i = 0; i < m_number_variables; i++)
			if (flag[i] == 0)
				return false;
		return true;
	}


	void TravellingSalesman::initializeVariables(VariableBase &x_, Random *rnd) const {
		auto &x = dynamic_cast<VariableType&>(x_);
		for (int idx(0); idx < x.size(); ++idx) {
			x[idx] = idx;
		}
		rnd->uniform.shuffle(x.begin(), x.end());
		if (!isValid(x))
			throw Exception("error in @TravellingSalesman::initSolution() in travelling_salesman.cpp");

	}


	void TravellingSalesman::initSolutionNN(SolutionBase& s, Random *rnd) const {
		std::vector<int> candidate(m_number_variables);
		int n = 0;
		for (int i(0); i < m_number_variables; ++i) {
			candidate[i] = i;
		}
		//	m_heuristic_sol.variable().resize(m_var_size);
		auto& x = dynamic_cast<SolutionType &>(s).variable();
		x[n] = m_number_variables * rnd->uniform.next();
		candidate[x[n]] = candidate[m_number_variables - 1];

		std::cout << x[n] << "\t" << candidate[x[n]] << std::endl;
		++n;
		//candidate[x[n++]] = candidate[m_variable_size - n];
		while (n < m_number_variables) {
			int loc = 0;
			double min = mvvv_cost[0][x[n - 1]][candidate[loc]];
			for (int m = 0; m < m_number_variables - n; m++) {
				if (mvvv_cost[0][x[n - 1]][candidate[m]] < min) {
					min = mvvv_cost[0][x[n - 1]][candidate[m]];
					loc = m;
				}
			}
			x[n++] = candidate[loc];
			candidate[loc] = candidate[m_number_variables - n];
		}

		if (!isValid(x))
			throw Exception("error in @travelling_salesman::initialize_solutionNN() in travelling_salesman.cpp");

	}

	bool TravellingSalesman::same(const VariableBase &x1_, const VariableBase &x2_) const {
		const VariableType& x1 = dynamic_cast<const VariableType&>(x1_);
		const VariableType& x2 = dynamic_cast<const VariableType&>(x2_);
		int i, j, pos = 0;
		for (i = 0; i < m_number_variables; i++)
		{
			if (x1[0] == x2[i])
			{
				pos = i;
				break;
			}
		}
		j = pos;
		for (i = 0; i < m_number_variables; i++)
		{
			if (x1[i] != x2[j])  break;
			else j = (j + 1) % m_number_variables;
		}
		if (i == m_number_variables) return true;
		j = pos;
		for (i = 0; i < m_number_variables; i++)
		{
			if (x1[i] != x2[j])  break;
			else j = (j - 1 + m_number_variables) % m_number_variables;
		}
		if (i == m_number_variables) return true;
		return false;
	}

	Real TravellingSalesman::variableDistance(const VariableBase& x1, const VariableBase& x2) const {
		
		std::vector<std::array<int, 2>> edge_x;
		std::vector<std::array<int, 2>> edge_y;

		transferEdgeSol(x1, edge_x);
		transferEdgeSol(x2, edge_y);
		return tspDis(edge_x, edge_y);
		
		//std::vector<std::vector<bool>> mvv_s1_edge(m_number_variables); // a temporary matrix to store the edges of s1, thus helping calculate the distance between two solutions
		//mvv_s1_edge.resize(m_number_variables);
		//const VariableType& x1_ = dynamic_cast<const VariableType&>(x1);
		//const VariableType& x2_ = dynamic_cast<const VariableType&>(x2);

		//for (auto& row : mvv_s1_edge)
		//	row.assign(m_number_variables, false);
		//for (size_t i = 0; i < m_number_variables; i++) {
		//	mvv_s1_edge[x1_[i]][x1_[(i + 1) % m_number_variables]] = true;
		//}
		//Real dis = 0;
		//for (size_t i = 0; i < m_number_variables; i++) {
		//	if (!mvv_s1_edge[x2_[i]][x2_[(i + 1) % m_number_variables]])dis += 1;
		//}
		//return dis;
	}

	std::pair<int, int> TravellingSalesman::adjacentCities(const SolutionBase& s, int city) const {
		const VariableType& x = dynamic_cast<const Solution<VariableType> &>(s).variable();
		for (size_t i = 0; i < m_number_variables; i++) {
			if (x[i] == city)
				return std::pair<int, int>(x[(i - 1 + m_number_variables) % m_number_variables], x[(i + 1) % m_number_variables]);
		}
		throw Exception("error@TravellingSalesman::adjacentCities()");
	}

	void TravellingSalesman::nearbyCities(std::vector<std::vector<int>>& nearby, int num_near, int obj) {
		int i, j, z, pos;
		Real min;
		std::vector<int> visited(m_number_variables);
		int num = num_near == 0 ? nearby[0].size() : num_near;
		for (i = 0; i < m_number_variables; i++)
		{
			for (j = 0; j < m_number_variables; j++)
				visited[j] = 0;
			for (j = 0; j < num - 1; j++)
			{
				min = 0xfffffff;
				for (z = 0; z < m_number_variables; z++)
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

	void TravellingSalesman::prim(std::vector<std::vector<int>>& mst_edge, int n) {
		std::vector<int> nears(m_number_variables);
		int i, j, k, l;
		int Min = mvvv_cost[n][0][1];
		k = 0; l = 1;
		for (i = 0; i < m_number_variables; i++)  //find the nearest edge in the graph
		{
			for (j = 0; j < m_number_variables; j++)
			{
				if (Min > mvvv_cost[n][i][j] && i != j)
				{
					Min = mvvv_cost[n][i][j];
					k = i; l = j;
				}
			}
		}
		mst_edge[0][0] = k; mst_edge[0][1] = l;
		for (i = 0; i < m_number_variables; i++)  //initial nears
		{
			if (mvvv_cost[n][i][l] < mvvv_cost[n][i][k])
				nears[i] = l;
			else nears[i] = k;
		}
		nears[l] = -1; nears[k] = -1;
		for (i = 2; i < m_number_variables; i++)//find the remain n-2 edges
		{
			Min = 0xfffffff;
			for (int z = 0; z < m_number_variables; z++)
			{
				if (nears[z] != -1 && Min > mvvv_cost[n][z][nears[z]])
				{
					Min = mvvv_cost[n][z][nears[z]];
					j = z;
				}
			}
			mst_edge[i - 1][0] = j; mst_edge[i - 1][1] = nears[j];
			nears[j] = -1;
			for (k = 0; k < m_number_variables; k++)
				if (nears[k] != -1 && mvvv_cost[n][k][nears[k]] > mvvv_cost[n][k][j])
					nears[k] = j;
		}
	}

	void TravellingSalesman::calculateEdgeWeight(char* edge_type) {
		mvvv_cost.resize(m_number_objectives);
		for (size_t i = 0; i < m_number_objectives; i++)
			mvvv_cost[i].resize(m_number_variables);
		for (size_t i = 0; i < m_number_objectives; i++)
			for (size_t j = 0; j < m_number_variables; j++)
				mvvv_cost[i][j].resize(m_number_variables);

		if (!strcmp(edge_type, "EUC_2D")) {
			for (size_t i = 0; i < m_number_variables; i++)
				for (size_t j = 0; j < m_number_variables; j++)
					mvvv_cost[0][i][j] = static_cast<int>(sqrt(pow(m_coordinate[i][0] - m_coordinate[j][0], 2) + pow(m_coordinate[i][1] - m_coordinate[j][1], 2)) + 0.5);
		}
		else if (!strcmp(edge_type, "ATT")) {
			Real r;
			int t;
			for (size_t i = 0; i < m_number_variables; i++)
			{
				for (size_t j = 0; j < m_number_variables; j++)
				{
					r = sqrt((pow(m_coordinate[i][0] - m_coordinate[j][0], 2) + pow(m_coordinate[i][1] - m_coordinate[j][1], 2)) / 10.0);
					t = static_cast<int>(r + 0.5);
					if (t < r) mvvv_cost[0][i][j] = t + 1;
					else mvvv_cost[0][i][j] = t;
				}
			}
		}
		else if (!strcmp(edge_type, "GEO")) {
			Real pi = 3.141592, RRR = 6378.388, min;
			int deg;
			for (size_t i = 0; i < m_number_variables; i++)
			{
				for (size_t j = 0; j < 2; j++)
				{
					deg = static_cast<int>(m_coordinate[i][j]);
					min = m_coordinate[i][j] - deg;
					m_coordinate[i][j] = pi * (deg + 5.0 * min / 3.0) / 180.0;
				}
			}
			Real q1, q2, q3;
			for (size_t i = 0; i < m_number_variables; i++) {
				for (size_t j = 0; j < m_number_variables; j++) {
					q1 = cos(m_coordinate[i][1] - m_coordinate[j][1]);
					q2 = cos(m_coordinate[i][0] - m_coordinate[j][0]);
					q3 = cos(m_coordinate[i][0] + m_coordinate[j][0]);
					mvvv_cost[0][i][j] = static_cast<int>(RRR * acos(0.5 * ((1.0 + q1) * q2 - (1.0 - q1) * q3)) + 1.0);
				}
			}
		}
		else
			throw Exception("no exiting this edgeType in function calculateEdgeWeight in travelling_salesman.cpp, please add it here!");
	}


	void TravellingSalesman::readProblem() {

#if defined(linux) || defined(__linux) || defined(__linux__) || defined(__APPLE__)
#define	strtok_s strtok_r
#endif

		size_t i;
		std::string Line;
		char *edgeType = 0, *edgeFormat = 0;
		char *Keyword = 0;
		char *Type = 0, *Format = 0;
		const char *Delimiters = " ():=\n\t\r\f\v\xef\xbb\xbf";
		std::ostringstream oss1;
		std::ifstream infile;

	//	std::cout << "filename\t" << m_file_name << std::endl;
		oss1 << m_file_path << m_file_name;
		infile.open(oss1.str().c_str());
		if (!infile) {
			throw Exception("read travelling salesman data error");
		}
		char *savePtr;
		while (std::getline(infile, Line)) {
			if (!(Keyword = strtok_s((char *)Line.c_str(), Delimiters, &savePtr)))
				continue;
			for (i = 0; i < strlen(Keyword); i++)
				Keyword[i] = toupper(Keyword[i]);
			if (!strcmp(Keyword, "NAME"))
				continue;
			else if (!strcmp(Keyword, "COMMENT"))
				continue;
			else if (!strcmp(Keyword, "TYPE"))
				continue;
			else if (!strcmp(Keyword, "DIMENSION")) {
				char *token = strtok_s(0, Delimiters, &savePtr);
				m_number_variables = atoi(token);
			}
			else if (!strcmp(Keyword, "EDGE_WEIGHT_TYPE")) {
				edgeType = strtok_s(0, Delimiters, &savePtr);
				Type = new char[strlen(edgeType) + 1];
				strncpy(Type, edgeType, strlen(edgeType) + 1);
			}
			else if (!strcmp(Keyword, "EDGE_WEIGHT_FORMAT")) {
				edgeFormat = strtok_s(0, Delimiters, &savePtr);
				Format = new char[strlen(edgeFormat) + 1];
				strncpy(Format, edgeFormat, strlen(edgeFormat) + 1);
			}
			else if (!strcmp(Keyword, "NODE_COORD_SECTION")) {
				m_coordinate.clear();
				i = 0;
				std::vector<Real> temp(2);
				while (infile >> Line)
				{
					infile >> temp[0];
					infile >> temp[1];
					m_coordinate.push_back(temp);
					++i;
					if (i == (size_t)m_number_variables) {
						break;
					}
				}
				break;
			}
			else if (!strcmp(Keyword, "EDGE_WEIGHT_SECTION")) {
				mvvv_cost.resize(1);
				mvvv_cost[0].resize(m_number_variables);
				for (size_t j = 0; j < m_number_variables; j++)
					mvvv_cost[0][j].resize(m_number_variables);
				if (!strcmp(Format, "LOWER_DIAG_ROW")) {
					for (size_t i = 0; i < m_number_variables; i++) {
						for (size_t j = 0; j <= i; j++) {
							infile >> mvvv_cost[0][i][j];
							mvvv_cost[0][j][i] = mvvv_cost[0][i][j];
						}
					}
				}
				else if (!strcmp(Format, "UPPER_DIAG_ROW")) {
					for (size_t i = 0; i < m_number_variables; i++) {
						for (int j = i; j < m_number_variables; j++) {
							infile >> mvvv_cost[0][i][j];
							mvvv_cost[0][j][i] = mvvv_cost[0][i][j];
						}
					}
				}
				else if (!strcmp(Format, "FULL_MATRIX")) {
					for (size_t i = 0; i < m_number_variables; i++)
						for (size_t j = 0; j < m_number_variables; j++)
							infile >> mvvv_cost[0][i][j];
				}
				else
					throw Exception("no exiting this edgeFormat in function readProblem in travelling_salesman.cpp, please add it here!");
			}
		}
		infile.close();
		infile.clear();
		if (!Type)
			throw Exception("file format error in function readProblem in TravellingSalesman.cpp");
		if (strcmp(Type, "EXPLICIT"))
			calculateEdgeWeight(Type);
		if (Type)
			delete[]Type;
		if (Format)
			delete[]Format;
	}

	void TravellingSalesman::readOptima() {
		size_t i;
		std::string Line;
		char *Keyword = 0;
		const char *Delimiters = " ():=\n\t\r\f\v\xef\xbb\xbf";
		std::ostringstream oss2;
		std::ifstream infile;
		char *savePtr;
	//	oss2 << static_cast<std::string>(g_working_directory);
		auto filename = m_file_name.substr(0, m_file_name.find_first_of("."));
		oss2 << m_file_path << filename << ".opt.tour";
		infile.open(oss2.str().c_str());
		if (!infile) return;
			//throw Exception("read travelling salesman optimal tour file error");
		std::string oldLine;
		while (getline(infile, Line)) {
			oldLine = Line;
			if (!(Keyword = strtok_s((char*)Line.c_str(), Delimiters, &savePtr)))
				continue;

			for (i = 0; i < strlen(Keyword); i++)
				Keyword[i] = toupper(Keyword[i]);
			if (!strcmp(Keyword, "TOUR_SECTION")) {
				SolutionType temp_opt(m_number_objectives, m_number_constraints, m_number_variables);
				int temp;
				for (i = 0; i < (size_t)m_number_variables; i++) {
					infile >> temp;
					--temp;
					temp_opt.variable()[i] = temp;
				}
				evaluateObjective(temp_opt.variable().data(), temp_opt.objective());
				dynamic_cast<Optima<VariableType>&>(*m_optima).appendSolution(temp_opt);
				break;
			}
			else if (!strcmp(Keyword, "COMMENT")) { //get optimal cost
				size_t j = 0;
				for (int z = oldLine.size() - 1; z >= 0; z--)
					Line[j++] = oldLine[z];
				Keyword = strtok_s((char*)Line.c_str(), Delimiters, &savePtr);
				size_t len = strlen(Keyword) - 1;
				for (i = 0; i <= len; i++)
					oldLine[i] = Keyword[len - i];
				oldLine[len + 1] = '\0';
				m_optima->appendObjective(atof(oldLine.c_str()));
			}
			else
				continue;
		}
		infile.close();
		infile.clear();
	}

	int selectCityRandom(std::vector<std::vector<int>>& matrix, std::vector<int>& visited, int num, int row, Random *rnd) {
		int i, pos, flag = -1;
		std::vector<int> arr(num);
		for (i = 0; i < num; i++)
			arr[i] = matrix[row][i];
		i = num - 1;
		while (i >= 0) {
			pos = int(i * rnd->uniform.next());
			if (visited[arr[pos]] == 1) {
				arr[pos] = arr[i];
				i--;
			}
			else {
				visited[arr[pos]] = 1;
				flag = arr[pos];
				break;
			}
		}
		return flag;
	}

	int selectCityRandom(std::vector<int> visited, int dim, Random *rnd) {
		int i, pos, flag = -1;
		std::vector<int> arr(dim);
		for (i = 0; i < dim; i++)
			arr[i] = i;
		i = dim - 1;
		while (i >= 0) {
			pos = int(i * rnd->uniform.next());
			if (visited[arr[pos]] == 1)	{
				arr[pos] = arr[i];
				i--;
			}
			else {
				visited[arr[pos]] = 1;
				flag = arr[pos];
				break;
			}
		}
		return flag;
	}

	int selectCityGreedy(std::vector<std::vector<int>>& matrix, std::vector<int>& visited, int num, int row) {
		int i, flag = -1;
		for (i = 0; i < num; i++) {
			if (visited[matrix[row][i]] == 1)
				continue;
			else {
				visited[matrix[row][i]] = 1;
				flag = matrix[row][i];
				break;
			}
		}
		return flag;
	}
}



bool ofec::TravellingSalesman::judgeCurSolLoop(const std::vector<std::array<int, 2>>& edge) {
	std::vector<bool> visited(edge.size(), false);
	int total(0);
	int next = 0;
	int from(0);
	while (next != -1) {
		from = next;
		++total;
		visited[next] = true;
		next = -1;
		for (int idx(0); idx < 2; ++idx) {
			if (!visited[edge[from][idx]]) {
				next = edge[from][idx];
				break;
			}
		}
	}
	return total == edge.size();
}

 void ofec::TravellingSalesman::transferEdgeSol(const SolutionBase& sol, std::vector<std::array<int, 2>>& edge) {
	auto& curSol(dynamic_cast<const SolutionType&>(sol));
	int dim(curSol.variable().size());
	edge.resize(dim);
	for (int idx(0); idx < dim; ++idx) {
		edge[curSol.variable()[idx]][0] = curSol.variable()[(idx - 1 + dim) % dim];
		edge[curSol.variable()[idx]][1] = curSol.variable()[(idx + 1) % dim];
	}
	for (auto& it : edge) {
		if (it.front() > it.back()) {
			std::swap(it.front(), it.back());
		}
	}
}

 void ofec::TravellingSalesman::transferEdgeSol(const std::vector<std::array<int, 2>>& edge, std::vector<int>& x) {
	if (edge.size() <= 2) {
		x.clear();
		for (int idx(0); idx < edge.size(); ++idx) {
			x.push_back(idx);
		}
	}
	else {
		int dim = edge.size();
		std::vector<bool> visited(dim, false);
		x.clear();
		int total(1);
		int from(0);
		x.push_back(from);
		visited[from] = true;
		int next = 1;
		if (edge[from][0] > edge[from][1]) {
			next = edge[from][1];
		}
		else next = edge[from][0];
		while (next != -1) {
			from = next;
			++total;
			visited[next] = true;
			x.push_back(from);
			next = -1;
			for (int idx(0); idx < 2; ++idx) {
				if (!visited[edge[from][idx]]) {
					next = edge[from][idx];
					break;
				}
			}
		}
	}
}


void ofec::TravellingSalesman::transferEdgeSol(const std::vector<int>& x, std::vector<std::array<int, 2>>& edge) {
	int dim(x.size());
	edge.resize(dim);
	for (int idx(0); idx < dim; ++idx) {
		edge[x[idx]][0] = x[(idx - 1 + dim) % dim];
		edge[x[idx]][1] = x[(idx + 1) % dim];

		//if(edge[x[idx]])
	}
	for (auto& it : edge) {
		if (it.front() > it.back()) {
			std::swap(it.front(), it.back());
		}
	}
}

void ofec::TravellingSalesman::transferEdgeSol(const VariableBase& sol, std::vector<std::array<int, 2>>& edge) {
	auto& curSol(dynamic_cast<const VariableType&>(sol));
	int dim(curSol.size());
	edge.resize(dim);
	for (int idx(0); idx < dim; ++idx) {
		edge[curSol[idx]][0] = curSol[(idx - 1 + dim) % dim];
		edge[curSol[idx]][1] = curSol[(idx + 1) % dim];
	}

	for (auto& it : edge) {
		if (it.front() > it.back()) {
			std::swap(it.front(), it.back());
		}
	}
}

double ofec::TravellingSalesman::tspNorDis(
	const std::vector<std::array<int, 2>>& edgex,
	const std::vector<std::array<int, 2>>& edgey) {

	return tspDis(edgex, edgey) / double(edgex.size());
	//double dis(0);
	//int dim(edgex.size());
	//for (int idv(0); idv < dim; ++idv) {
	//	for (auto& to1 : edgex[idv]) {
	//		for (auto& to2 : edgey[idv]) {
	//			if (to1 == to2) {
	//				dis += 1;
	//				break;
	//			}
	//		}
	//	}
	//}
	//return 1.0-dis / double(dim * edgex.front().size());
}

double ofec::TravellingSalesman::tspDis(
	const std::vector<std::array<int, 2>>& edgex,
	const std::vector<std::array<int, 2>>& edgey) {
	double dis(0);
	int dim(edgex.size());
	for (int idv(0); idv < dim; ++idv) {
		for (auto& to1 : edgex[idv]) {
			for (auto& to2 : edgey[idv]) {
				if (to1 == to2) {
					dis += 1;
					break;
				}
			}
		}
	}
	return edgex.size() - dis / edgex.front().size();
}

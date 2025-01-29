/********* Begin Register Information **********
{
	"name": "TSP",
	"identifier": "TravellingSalesman",
	"tags": [ "travelling salesman problem" ]
}
*********** End Register Information **********/

/*************************************************************************
* Project: Library of Open Frameworks for Evolutionary Computation (OFEC)
*************************************************************************
* Author: Changhe Li & Yong Xia
* Email: changhe.lw@google.com  Or cugxiayong@gmail.com
* Language: C++
*************************************************************************
*  This file is part of OFEC. This library is free software;
*  you can redistribute it and/or modify it under the terms of the
*  GNU General Public License as published by the Free Software
*  Foundation; either version 2, or (at your option) any later version.
*************************************************************************/
// Created: 7 Oct 2014
// Modified: 14 Mar 2018 By Junchen Wang (wangjunchen@cug.edu.cn)

#ifndef OFEC_TSP_H
#define OFEC_TSP_H

#include "../../../../core/problem/problem.h"
#include "../../../../core/problem/domain.h"
#include "../../../../core/problem/solution.h"
#include "../../../../core/problem/optima.h"

namespace ofec {
#define CAST_TSP(pro) dynamic_cast<TravellingSalesman*>(pro)

	class Environment;

	class TravellingSalesman : public ProblemVariableVector<int> {
		OFEC_CONCRETE_INSTANCE(TravellingSalesman)

	public:
		struct Hash {
			std::vector<unsigned long long> m_hash_val;
			bool m_init_flag = false;
		
			void initialize(ofec::Random* rnd, int numVariable) {

				m_init_flag = true;
				m_hash_val.resize(numVariable);
				unsigned long long maxVal = std::numeric_limits<unsigned long long>::max();
				maxVal = sqrt(maxVal);
				for (auto& it : m_hash_val) {
					it = rnd->uniform.nextNonStd<unsigned long long>(0, maxVal);
				}

			}
			unsigned long long getHash(int idx) {
				return m_hash_val[idx];
			}
			unsigned long long calHash(const std::vector<int>& sol)const {
				unsigned long long Hash(0);
				for (int idx(1); idx < sol.size(); ++idx) {
					Hash ^= m_hash_val[sol[idx - 1]] * m_hash_val[sol[idx]];
				}
				Hash ^= m_hash_val[sol.back()] * m_hash_val[sol.front()];
				return Hash;
			}

			unsigned long long calHash(const SolutionBase& s) {
				auto& cursolx(dynamic_cast<const SolutionType&>(s).variable().vector());
				return calHash(cursolx);;
			}
		};

		struct HashSolutionSubTree {

			std::map<unsigned long long, HashSolutionSubTree> m_map;
			Hash m_hash;
			std::vector<std::array<int, 2>> m_solEdge;


			void findSolution(std::vector<unsigned long long>& seqHash,
				const std::vector<int>& curSol,
				const std::vector<std::array<int, 2>>& curSolEdge,
				Random* rnd, int numVariable) {

				if (m_solEdge.empty()) {
					m_solEdge = curSolEdge;
				//	m_currentSolution = cursol;
					return;
				}
				else {
					auto dis = tspDis(m_solEdge, curSolEdge);
					if (dis == 0) return;
				}
				if (!m_hash.m_init_flag) {
					m_hash.initialize(rnd, numVariable);
				}
				auto curHash = m_hash.calHash(curSol);
				seqHash.push_back(curHash);
				
				m_map[curHash].findSolution(seqHash, curSol,curSolEdge, rnd, numVariable);
			}


		};

		
		struct HashSolutionMap {

			std::shared_ptr<Random> m_rnd;
			int m_numVariable = 0;
			std::map<unsigned long long, HashSolutionSubTree> m_map;
			Hash m_hash;
			int m_maxId = -1;
			std::map<std::vector<unsigned long long>, int> m_solId;

			void initialize(ofec::Random* rnd, int numVariable) {
				m_maxId = -1;
				m_rnd = rnd->getSharedPtr();
				m_numVariable = numVariable;
				m_hash.initialize(m_rnd.get(), m_numVariable);
			}

			int getSolId(const std::vector<int>& sol) {
				std::vector<unsigned long long> seq;
				auto curHash= m_hash.calHash(sol);
				seq.push_back(curHash);
				std::vector<std::array<int, 2>> curSolEdge;
				transferEdgeSol(sol, curSolEdge);
				m_map[curHash].findSolution(seq,sol, curSolEdge, m_rnd.get(), m_numVariable);
				if (m_solId.find(seq) == m_solId.end()) {
					return m_solId[seq] = ++m_maxId;
				}
				else return m_solId[seq];

			}

			int getSolId(const ofec::SolutionBase& sol) {
				auto& tsp_sol = dynamic_cast<const SolutionType&>(sol);
				return getSolId(tsp_sol.variable().vector());
			}
						
			
		};


	protected:

		std::string m_file_name;
		std::string m_file_path;
		
		std::vector<std::vector<std::vector<Real>>> mvvv_cost;  // the cost between cities
		//size_t m_number_variables;
		std::vector<std::vector<int>> mvv_nearby; // nearby cities of each city
		bool m_if_valid_check = true;
		std::vector<std::vector<Real>> m_coordinate;

		Hash m_hash;

	public:



		~TravellingSalesman() = default;

	public:

		virtual size_t numberVariables() const override { return m_number_variables; }
		bool judgeAccessibility(const SolutionBase& s1, const SolutionBase& s2)  {
			return int(variableDistance(s1.variableBase(), s2.variableBase())) != m_number_variables;
		}

		void initializeVariables(VariableBase &x, Random *rnd) const override;
		bool same(const VariableBase&x1, const VariableBase&x2) const override;
		Real variableDistance(const VariableBase &x1, const VariableBase &x2) const override;

		//inline virtual Real maxVarDis()const override {
		//	return m_number_variables;
		//}
		virtual Real normalizedVariableDistance(const VariableBase& s1, const VariableBase& s2) const override{
			return variableDistance(s1, s2)/ m_number_variables;
		}

		static bool judgeCurSolLoop(const std::vector<std::array<int, 2>>& edge);

		static void transferEdgeSol(const SolutionBase& sol, std::vector<std::array<int, 2>>& edge);

		static void transferEdgeSol(const std::vector<std::array<int, 2>>& edge, std::vector<int>& x);

		static void transferEdgeSol(const std::vector<int>& x, std::vector<std::array<int, 2>>& edge);

		static void transferEdgeSol(const VariableBase& sol, std::vector<std::array<int, 2>>& edge);
		void inputSol(std::istream& in, std::vector<int>& sol) {
			sol.resize(m_number_variables);
			for (int idx(0); idx < sol.size(); ++idx) {
				in >> sol[idx];
			}
		}

		static double tspNorDis(
			const std::vector<std::array<int, 2>>& edgex,
			const std::vector<std::array<int, 2>>& edgey);
		static double tspDis(
			const std::vector<std::array<int, 2>>& edgex,
			const std::vector<std::array<int, 2>>& edgey);


		static void mutation(SolutionBase& sol, int step, ofec::Random* rnd) {
			auto& cursolx  = dynamic_cast<SolutionType&>(sol).variable();
			auto dim = cursolx.size();
			while (step--) {
				int a = rnd->uniform.nextNonStd<int>(0, dim);
				int b = rnd->uniform.nextNonStd<int>(0, dim);
				std::swap(cursolx[a], cursolx[b]);
			}
		}


		static void getSortedSolutionX(const SolutionBase& sol, std::vector<int>& solX) {
			auto& cursolx = dynamic_cast<const SolutionType&>(sol).variable();
			int dim = cursolx.size();
			solX.clear();
			int i, n, Forward;
			n = dim;
			for (i = 0; i < n && cursolx[i] != 0; i++);

			Forward = 1;
			if (cursolx[(i - 1 + n) % n] < cursolx[(i + 1) % n]) Forward = -1;
			solX.resize(dim);
			for (int j(0); j < dim; ++j) {
				solX[j] = cursolx[i];
				i = (i + Forward + n) % n;
			}
		}



		static void sortedSolutionX(SolutionBase& sol) {
			auto& cursolx = dynamic_cast<SolutionType&>(sol).variable();
			std::vector<int> solX;
			getSortedSolutionX(sol, solX);
			cursolx.vector() = solX;
		}
	
	

		bool isValid(const VariableBase &x) const;
		void initSolutionNN(SolutionBase &s, Random *rnd) const; // generate a solution with the nearest neighbor
		std::pair<int, int> adjacentCities(const SolutionBase &s, int city) const; //return the next and previous cities of city in solution s
		const std::vector<std::vector<Real>>& coordinate() const { return m_coordinate; }
		void nearbyCities(std::vector<std::vector<int>> &nearby,int num_near=0, int obj=0);     //find some percent of nearby city
		void prim(std::vector<std::vector<int>>& mst_edge, int n = 0); // find MST edges
		void calculateEdgeWeight(char* edge_type);
		const std::vector<std::vector<Real>>& cost(int i = 0) const { return mvvv_cost[i]; }
		const std::vector<std::vector<int>>& nearby() const { return mvv_nearby; }
		const std::string& fileName() const { return m_file_name; }
		const std::string& filePath()const { return m_file_path; }
		std::string& filePath() { return m_file_path; }

		virtual void evaluate(const VariableBase& vars, std::vector<Real>& objs, std::vector<Real> &cons)const override;


		unsigned long long calHash(const SolutionBase& s) {
			return m_hash.calHash(s);
		}
	protected:
		void initialize_(Environment* env) override;
		void updateOptima(Environment* env) override;

		virtual void evaluateObjective(int* x, std::vector<Real>& obj)const;
		virtual void readProblem();    //read source data from file
		virtual void readOptima();	   //read optima data from file

		void addInputParameters();

	}; 

	int selectCityRandom(std::vector<std::vector<int>>& matrix, std::vector<int>& visited, int num, int row, Random *rnd);
	int selectCityRandom(std::vector<int> visited, int dim, Random *rnd);
	int selectCityGreedy(std::vector<std::vector<int>>& matrix, std::vector<int>& visited, int num, int row);
}

#endif // !OFEC_TSP_H


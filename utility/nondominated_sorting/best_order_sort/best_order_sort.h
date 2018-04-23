/*************************************************************************
* Project: Library of Open Frameworks for Evolutionary Computation (OFEC)
*************************************************************************
* Author: Changhe Li & Junchen Wang
* Email: changhe.lw@gmail.com, wangjunchen@cug.edu.cn
* Language: C++
*************************************************************************
*  This file is part of OFEC. This library is free software;
*  you can redistribute it and/or modify it under the terms of the
*  GNU General Public License as published by the Free Software
*  Foundation; either version 2, or (at your option) any later version.

*************************************************************************/
// Created: 29 Mar 2018 by Junchen Wang
// Last modified:

/*
Roy, P. C., Islam, M. M., & Deb, K. (2016, July). 
Best order sort: a new algorithm to non-dominated sorting for evolutionary multi-objective optimization. 
In Proceedings of the 2016 on Genetic and Evolutionary Computation Conference Companion (pp. 1113-1120). ACM.

Based on code from https://github.com/Proteek/Best-Order-Sort/blob/master/BestOrderSort.java 
*/

#ifndef BOS_H
#define BOS_H

#include <vector>
#include <utility>

namespace NS {
	// interface
	int best_order_sort(const std::vector<std::vector<double>>& data, std::vector<int>& rank, std::pair<int, int>& meas);
	// tansfered source code begin from here
	class BestOrderSort {
	public:
		class Node {
		public:
			int data;
			Node* link;
			Node(int d, Node* n) : data(d), link(n){}
		};
		class LinkedList {
		public:
			Node * start;
			LinkedList() : start(nullptr) {}
			void addStart(int val) {
				start = new Node(val, start);
			}
			~LinkedList() {
				while (start != nullptr) {
					Node* last = start;
					start = start->link;
					delete last;
				}
			}
		};
		class MergeSort {
		private:
			std::vector<int> helper;
			const std::vector<std::vector<double>>& population;
			int n;
			int left;
			int right;
			int largest;
			int obj;
			bool check;
			std::vector<std::vector<int>> rank;
			std::vector<int> lex_order;
		public:
			MergeSort(const std::vector<std::vector<double>>& pop) : population(pop){
				this->n = population.size();
				helper.resize(n);
			}
			void setrank(const std::vector<std::vector<int>>& rank) { this->rank = rank; }
			std::vector<std::vector<int>> getrank() { return rank; }
			//void setPopulation(const std::vector<std::vector<double>>& pop);
			void setLexOrder(std::vector<int>& order) { this->lex_order = order; }
			void sort(int obj); //merge sort main
			void mergesort(int low, int high); //merge sort algorithm O(nlogn) sorting time
			void merge(int low, int middle, int high);
			void sort_specific(int obj); //uses lexicographical order of 1st dimension
			void mergesort_specific(int low, int high); //Merge sort with the help of lexicographic order got by sorting first objectives
			void merge_specific(int low, int middle, int high);
			bool lexicopgraphic_dominate(int p1, int p2);
		};
	public:
		BestOrderSort(const std::vector<std::vector<double>>& data);
		void best_sort(int n, int m);
		bool dominates(int p1, int p2); //Non - domination check, Although previously sorted by lexicographic order, it may be true that those solutions are identical
		void sortingValues();
		const std::vector<int>& getRank() const { return rank; }
		const int getTotalFront() const { return totalfront; }
	private:
		//void extended_kung_sort_two_dimension();
	private:
		int m1 = -1;
		const std::vector<std::vector<double>>& population;
		std::vector<std::vector<int>> allrank;
		MergeSort mergesort;
		std::vector<int> rank;
		std::vector<bool> set;
		std::vector<std::vector<LinkedList>> list;
		//std::vector<LinkedList> L;//two dimension
		//std::vector<double> b;//two dimension
		int totalfront = 0;
		//std::vector<LinkedList> Front;
		int s;
		int n;
		int m, m2;
		int obj, obj_rank;
		std::vector<int> size;
		std::vector<int> lex_order;
	};
}


#endif // !BOS_H


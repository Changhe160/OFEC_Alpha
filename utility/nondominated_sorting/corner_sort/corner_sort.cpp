#include "corner_sort.h"

namespace NS {
	unsigned int min_obj(const double ** POP, unsigned int obj_index, cs_node ** head, cs_node ** mh, int * comp, const unsigned int m, int& NumComp) {
		unsigned int i = 0, cur;
		cs_node *p = *mh, *q = NULL;
		cur = p->index;
		q = p;
		while (p->marknext != NULL)
		{
			if (comp[obj_index] == 0) // minimization 
			{
				NumComp++;
				if (POP[p->marknext->index][obj_index] < POP[cur][obj_index])
				{
					cur = p->marknext->index;
					q = p->marknext;
				}
				else if (POP[p->marknext->index][obj_index] == POP[cur][obj_index])
				{
					unsigned int i = 0;
					while (i < m)
					{
						unsigned int temp_obj_index = (obj_index + i) % m;
						NumComp++;
						if (POP[p->marknext->index][temp_obj_index] < POP[cur][temp_obj_index]) {
							cur = p->marknext->index;
							q = p->marknext;
							break;
						}
						else if (POP[p->marknext->index][temp_obj_index] > POP[cur][temp_obj_index]) {
							break;
						}
						else
							i++;
					}
				}
			}
			else 
			{
				NumComp++;
				if (POP[p->marknext->index][obj_index] > POP[cur][obj_index])
				{
					cur = p->marknext->index;
					q = p->marknext;
				}
				else if (POP[p->marknext->index][obj_index] == POP[cur][obj_index])
				{
					unsigned int i = 0;
					while (i < m)
					{
						unsigned int temp_obj_index = (obj_index + i) % m;
						NumComp++;
						if (POP[p->marknext->index][temp_obj_index] > POP[cur][temp_obj_index]) {
							cur = p->marknext->index;
							q = p->marknext;
							break;
						}
						else if (POP[p->marknext->index][temp_obj_index] < POP[cur][temp_obj_index]) {
							break;
						}
						else
							i++;
					}
				}
			}

			p = p->marknext;
		}
		if (q->markpre == NULL)
		{
			*mh = (*mh)->marknext;
			if ((*mh) != NULL)
			{
				(*mh)->markpre = NULL;
			}
		}
		else if (q->marknext == NULL)
		{
			q->markpre->marknext = NULL;
		}
		else
		{
			q->markpre->marknext = q->marknext;
			q->marknext->markpre = q->markpre;
		}
		if (q->rankpre == NULL)
		{
			*head = (*head)->ranknext;
			if ((*head) != NULL)
			{
				(*head)->rankpre = NULL;
			}
			free(q);
		}
		else if (q->ranknext == NULL)
		{
			p = q;
			q->rankpre->ranknext = NULL;
			free(p);
		}
		else
		{
			p = q;
			q->rankpre->ranknext = q->ranknext;
			q->ranknext->rankpre = q->rankpre;
			free(p);
		}
		return(cur);
	}
	void cornerSort(const double ** POP, unsigned int m, unsigned int n, int * rank, int * comp, int& NumComp) {
		unsigned int i, j, cout = 0, markcout = 0, cur, obj_index, flag;
		int r = -1;
		cs_node *head = NULL, *mh = NULL, *p = NULL, *newone = NULL;
		//initialize linked list(rank and mark)
		for (i = 0; i < n; i++)
		{
			newone = (cs_node*)malloc(sizeof(cs_node));
			newone->index = i;
			newone->ranknext = NULL;
			if (i == 0)
			{
				head = newone;
				p = head;
				p->rankpre = NULL;
			}
			else
			{
				p->ranknext = newone;
				newone->rankpre = p;
				p = newone;
			}
		}

		while (head != NULL)
		{
			r = r + 1;//arrange the r-th ranking
					  //unmark all the unranked solutions
			p = head;
			while (p != NULL)
			{
				p->marknext = p->ranknext;
				p->markpre = p->rankpre;
				p = p->ranknext;
			}
			mh = head;
			while (mh != NULL)//until all the solutions are marked
			{
				for (obj_index = 0; mh != NULL&&obj_index < m; ++obj_index)
				{
					// find solution of the best objective obj_index among unmarked ones
					cur = min_obj(POP, obj_index, &head, &mh, comp, m, NumComp);// delete it in both mark and rank linked lists
					rank[cur] = r;
					cout++;
					p = mh;
					//mark the dominated solutions by cur
					while (p != NULL)
					{
						flag = 0;
						for (j = 0; j < m; ++j)
						{
							NumComp++;
							if (comp[j] == 0) {
								if (j != obj_index&&POP[cur][j] > POP[p->index][j])
								{
									flag = 1;
									break;
								}
							}
							else {
								if (j != obj_index&&POP[cur][j] < POP[p->index][j])
								{

									flag = 1;
									break;
								}
							}

						}
						if (flag == 0)
						{
							if (p->markpre == NULL)
							{
								mh = mh->marknext;
								if (mh != NULL)
								{
									mh->markpre = NULL;
								}
							}
							else if (p->marknext == NULL)
							{
								p->markpre->marknext = NULL;
							}
							else
							{
								p->markpre->marknext = p->marknext;
								p->marknext->markpre = p->markpre;
							}
						}
						p = p->marknext;
					}
				}
			}
		}
	}
	void corner_sort(const std::vector<std::vector<double>>& data, std::vector<int>& rank, std::pair<int, int>& measurement) {

		std::chrono::time_point<std::chrono::system_clock> Total_start_time;
		int NumComp(0);
		Total_start_time = std::chrono::system_clock::now();

		const int data_size = data.size();
		if (data_size == 0) return;
		const int obj_num = data.front().size();
		const double** POP = new const double*[data_size];
		for (int i = 0; i < data_size; ++i)
			POP[i] = data[i].data();
		std::vector<int> cs_comp(data_size, 0);
		NS::cornerSort(POP, obj_num, data_size, rank.data(), cs_comp.data(), NumComp);
		delete[] POP;

		int time = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now() - Total_start_time).count();
		measurement.first += time;
		measurement.second += NumComp;
	}
}
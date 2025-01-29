#include "k_means.h"
#include "csiwdn.h"

namespace ofec::csiwdn {
	double KMeans::cal_dis(const std::vector<Real>& point1, const std::vector<Real>& point2) {
		size_t da = point1.size();
		size_t db = point2.size();
		if (da != db) throw Exception("Alg4Epa::cal_dis(): Dimensions of two vectors must be same!!");
		double val = 0.0;
		for (size_t i = 0; i < da; i++)
		{
			val += pow((point1[i] - point2[i]), 2);
		}
		return pow(val, 0.5);
	}

	double KMeans::cal_var(Problem *pro) {
		double var = 0;
		for (int i = 0; i < m_clusters.size(); i++)
		{
			Cluster t = m_clusters[i];
			for (int j = 0; j < t.samples.size(); j++)
			{
				var += cal_dis(CAST_CSIWDN(pro)->getCoordinates(t.samples[j] + 1), t.centroid);
			}
		}
		return var;
	}

	int KMeans::findCluster(const int id_trainX, Problem *pro) {
		std::vector<Real> sample = CAST_CSIWDN(pro)->getCoordinates(id_trainX);
		double dist = cal_dis(m_clusters[0].centroid, sample);
		double tmp;
		int label = 0;//��ʾ������һ����
		for (int i = 1; i < m_clusters.size(); i++) {
			tmp = cal_dis(m_clusters[i].centroid, sample);
			if (tmp < dist) { dist = tmp; label = i; }
		}
		return label;
	}

	void KMeans::getMeans(const std::vector<int>& trainX, Problem *pro) {
		for (size_t i = 0; i < m_clusters.size(); i++)
		{
			size_t dim = 2;
			std::vector<double> val(dim, 0.0);
			for (size_t j = 0; j < m_clusters[i].samples.size(); j++)
			{
				size_t sample = m_clusters[i].samples[j];
				for (size_t d = 0; d < dim; d++)
				{
					val[d] += CAST_CSIWDN(pro)->getCoordinates(trainX[sample])[d];
					if (j == m_clusters[i].samples.size() - 1)
						m_clusters[i].centroid[d] = val[d] / m_clusters[i].samples.size();
				}
			}
		}
	}

	void KMeans::clustering(const std::vector<int>& trainX, size_t k, Problem *pro, Random *rnd) {
		const size_t size = trainX.size();

		/*��ʼ����������*/
		m_clusters.clear();
		m_clusters.resize(k);
		for (size_t i = 0; i < k; i++)
		{
			int c = rnd->uniform.nextNonStd<size_t>(0, size);
			m_clusters[i].centroid = CAST_CSIWDN(pro)->getCoordinates(trainX[c]);
		}

		//����Ĭ�ϵ����ĸ��ظ�ֵ
		int lable = 0;
		for (size_t i = 0; i != trainX.size(); ++i) {
			lable = findCluster(trainX[i], pro);
			m_clusters[lable].samples.push_back(i);
		}
		double oldVar = -1;
		double newVar = cal_var(pro);
		/*��ε���ֱ������*/
		int count = 0;
		while (fabs(newVar - oldVar) >= 1)
		{
			++count;
			//����ÿ���ص����ĵ�
			getMeans(trainX, pro);

			oldVar = newVar;
			newVar = cal_var(pro); //�����µ�׼����ֵ
			for (size_t i = 0; i < k; i++) //���ÿ����
			{
				m_clusters[i].samples.clear();
			}
			//�����µ����Ļ���µĴ�
			for (size_t i = 0; i != trainX.size(); ++i) {
				lable = findCluster(trainX[i], pro);
				m_clusters[lable].samples.push_back(i);
			}
		}
	}
}
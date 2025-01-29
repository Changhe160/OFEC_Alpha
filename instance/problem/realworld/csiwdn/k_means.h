#ifndef OFEC_CSIWDN_K_MEANS_H
#define OFEC_CSIWDN_K_MEANS_H

#include "../../../../core/definition.h"
#include "../../../../core/random/newran.h"
#include <vector>

namespace ofec {
	class Problem;
	namespace csiwdn {
		class KMeans {
		public:
			struct Cluster
			{
				std::vector<Real> centroid;
				std::vector<size_t> samples;
			};

			void clustering(const std::vector<int>& trainX, size_t k, Problem *pro, Random *rnd);
			const std::vector<Cluster>& clusters() const { return m_clusters; }

		private:
			std::vector<Cluster> m_clusters;

			void getMeans(const std::vector<int>& trainX, Problem *pro);
			double cal_dis(const std::vector<Real>& point1, const std::vector<Real>& point2);
			double cal_var(Problem *pro);
			int findCluster(const int id_trainX, Problem *pro);
			void getMeans(std::vector<int>& trainX, Problem *pro);
		};
	}
}

#endif // !OFEC_CSIWDN_K_MEANS_H

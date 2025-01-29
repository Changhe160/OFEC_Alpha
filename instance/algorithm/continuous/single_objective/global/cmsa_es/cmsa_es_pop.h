#ifndef OFEC_CMSA_ES_POP_H
#define OFEC_CMSA_ES_POP_H

#include "../../../../../../core/algorithm/population.h"
#include "../../../../../../utility/linear_algebra/matrix.h"

namespace ofec {
	class PopCMSA_ES : public Population<Solution<>> {
	protected:
		size_t m_lamda, m_mu;
		Real m_sigma, m_tau, m_TCovCoeff, m_tau_c;
		Vector m_weights, m_mean;
		Matrix m_covariance, m_cholesky;
		std::vector<Real> m_multiplier;
		std::vector<Vector> m_var_transformed;
		std::vector<size_t> m_order;

		void weightedMean();
		void estimateCovariance();

	public:
		PopCMSA_ES(size_t pop_size, Environment *env);
		void initialize(Environment *env, Random *rnd) override;
		void initializeBySamples(
			const std::vector<std::shared_ptr<Solution<>>> &sols, Environment *env);
		int evolve(Environment *env, Random *rnd) override;
		void estimateSampleParameters();
		int sampleNewPopulation(Environment *env, Random *rnd);
		void sort(Environment *env);

		static void calculateMean(const std::vector<std::shared_ptr<Solution<>>> &sols, Vector &mean);
		static void calculateCovariance(const std::vector<std::shared_ptr<Solution<>>> &sols,
			const Vector &mean, Matrix &covariance);
		static void calculateCovarianceUnivariate(const std::vector<std::shared_ptr<Solution<>>> &sols,
			const Vector &mean, Matrix &covariance);
	};
}

#endif // !OFEC_CMSA_ES_POP_H

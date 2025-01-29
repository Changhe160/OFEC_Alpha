#include "cmsa_es_pop.h"
#include "../../../../../../core/problem/continuous/continuous.h"
#include <numeric>

namespace ofec {
	PopCMSA_ES::PopCMSA_ES(size_t pop_size, Environment *env) :
		Population(pop_size, env, env->problem()->numberVariables()),
		m_lamda(pop_size),
		m_mu(0.5 * pop_size),
		m_sigma(1.0),
		m_tau(1.0 / sqrt(2.0 * env->problem()->numberVariables())),
		m_TCovCoeff(1.0),
		m_tau_c(1 + env->problem()->numberVariables() * (env->problem()->numberVariables() + 1.0) / m_mu * m_TCovCoeff),
		m_weights(m_mu),
		m_mean(env->problem()->numberVariables()),
		m_covariance(env->problem()->numberVariables(), env->problem()->numberVariables()),
		m_multiplier(m_lamda, 1),
		m_var_transformed(m_lamda, Vector(env->problem()->numberVariables(), 0)),
		m_order(pop_size)
	{
		Real sum_weights = 0.0;
		for (size_t i = 0; i < m_weights.size(); ++i) {
			m_weights[i] = log(m_mu + 1.0) - log(i + 1.0);
			sum_weights += m_weights[i];
		}
		for (size_t i = 0; i < m_weights.size(); ++i) {
			m_weights[i] /= sum_weights;
		}
		std::iota(m_order.begin(), m_order.end(), 0);
	}

	void PopCMSA_ES::initialize(Environment *env, Random *rnd) {
		m_sigma = 1;
		m_covariance.zeroize();
		for (size_t i = 0; i < env->problem()->numberVariables(); ++i) {
			auto &range = CAST_CONOP(env->problem())->range(i);
			m_mean[i] = (range.first + range.second) / 2;
			m_covariance[i][i] = (range.second - range.first) / 3;
		}
		m_covariance.choleskyDecomposeUnivariate(m_cholesky);
		m_multiplier.assign(m_lamda, 1);
		for (auto &vt : m_var_transformed) {
			vt.zeroize();
		}
	}

	void PopCMSA_ES::initializeBySamples(
		const std::vector<std::shared_ptr<Solution<>>> &sols, Environment *env
	) {
		m_sigma = 1;
		calculateMean(sols, m_mean);
		if (sols.size() == 1) {
			Real EEL = pow(CAST_CONOP(env->problem())->domainVolume() / sols.size(), 1.0 / env->problem()->numberVariables());
			m_covariance = EEL * 0.01 * Matrix::eye(env->problem()->numberVariables());
			m_cholesky = sqrt(EEL * 0.01) * Matrix::eye(env->problem()->numberVariables());
		}
		else if (sols.size() < env->problem()->numberVariables()) {
			calculateCovarianceUnivariate(sols, m_mean, m_covariance);
			m_covariance.choleskyDecomposeUnivariate(m_cholesky);
		}
		else {
			calculateCovariance(sols, m_mean, m_covariance);
			m_covariance.choleskyDecompose(m_cholesky);
		}
		m_multiplier.assign(m_lamda, 1);
		for (auto &vt : m_var_transformed) {
			vt.zeroize();
		}
	}

	int PopCMSA_ES::evolve(Environment *env,  Random *rnd) {
		estimateSampleParameters();
		return sampleNewPopulation(env, rnd);
	}

	void PopCMSA_ES::estimateSampleParameters() {
		weightedMean();
		Real sum_logsigma = 0.0;
		Real weighted_sum_logsigma = 0.0;
		for (size_t i = 0; i < m_mu; ++i) {
			weighted_sum_logsigma += m_weights[i] * log(m_multiplier[m_order[i]]);
			sum_logsigma += log(m_multiplier[m_order[i]]); // actually, this should be of the whole pop before selection TODO
		}
		m_sigma *= exp(weighted_sum_logsigma) / exp(sum_logsigma / m_mu);
		estimateCovariance();
	}

	void PopCMSA_ES::weightedMean() {
		m_mean.zeroize();
		for (size_t i = 0; i < m_mu; ++i) {
			m_mean += m_weights[i] * Vector(m_individuals[m_order[i]]->variable().vector());
		}
	}

	void PopCMSA_ES::estimateCovariance() {
		auto wgMatrix = Matrix::zeros(m_mean.size(), m_mean.size());
		for (size_t i = 0; i < m_mean.size(); i++) {
			for (size_t j = i; j < m_mean.size(); j++) {
				for (size_t k = 0; k < m_mu; ++k) {
					wgMatrix[i][j] += m_weights[k] *
						(m_var_transformed[m_order[k]][i]) *
						(m_var_transformed[m_order[k]][j]);
				}
				wgMatrix[j][i] = wgMatrix[i][j];
			}
		}
		for (size_t i = 0; i < m_mean.size(); i++) {
			for (size_t j = i; j < m_mean.size(); j++) {
				m_covariance[i][j] = (1.0 - 1.0 / m_tau_c) * m_covariance[i][j] + (1.0 / m_tau_c) * wgMatrix[i][j];
				m_covariance[j][i] = m_covariance[i][j];
			}
		}
		m_covariance.choleskyDecompose(m_cholesky);
	}

	int PopCMSA_ES::sampleNewPopulation(Environment *env, Random *rnd) {
		for (size_t i = 0; i < m_lamda; ++i) {
			do {
				auto z = Vector::randn(m_mean.size(), &rnd->normal);
				m_multiplier[i] = m_sigma * exp(m_tau * rnd->normal.next());
				m_var_transformed[i] = m_cholesky * z;
				m_individuals[i]->variable().vector() = (m_mean + m_multiplier[i] * m_var_transformed[i]).vect();
			} while (CAST_CONOP(env->problem())->boundaryViolated(*m_individuals[i]));
		}
		int rf = evaluate(env);
		sort(env);
		return rf;
	}

	void PopCMSA_ES::sort(Environment *env) {
		if (env->problem()->optimizeMode(0) == OptimizeMode::kMinimize) {
			std::sort(m_order.begin(), m_order.end(),
				[this](size_t i, size_t j) {
					return m_individuals[i]->objective(0) < m_individuals[j]->objective(0);
				}
			);
		}
		else {
			std::sort(m_order.begin(), m_order.end(),
				[this](size_t i, size_t j) {
					return m_individuals[i]->objective(0) > m_individuals[j]->objective(0);
				}
			);
		}
	}

	void PopCMSA_ES::calculateMean(const std::vector<std::shared_ptr<Solution<>>> &sols, 
		Vector &mean
	) {
		mean.resize(sols.front()->variable().size());
		mean.zeroize();
		for (auto &s : sols) {
			mean += s->variable().vector();
		}
		mean /= sols.size();
	}

	void PopCMSA_ES::calculateCovariance(const std::vector<std::shared_ptr<Solution<>>> &sols, 
		const Vector &mean, Matrix &covariance
	) {
		covariance.resize(mean.size(), mean.size());
		covariance.zeroize();
		for (size_t i = 0; i < mean.size(); i++) {
			for (size_t j = i; j < mean.size(); j++) {
				for (size_t k = 0; k < sols.size(); k++) {
					covariance[i][j] += (sols[k]->variable()[i] - mean[i])
						* (sols[k]->variable()[j] - mean[j]);
				}
				covariance[i][j] /= sols.size();
			}
		}
		for (size_t i = 0; i < mean.size(); i++) {
			for (size_t j = 0; j < i; j++) {
				covariance[i][j] = covariance[j][i];
			}
		}
	}
	
	void PopCMSA_ES::calculateCovarianceUnivariate(const std::vector<std::shared_ptr<Solution<>>> &sols,
		const Vector &mean, Matrix &covariance
	) {
		covariance.resize(mean.size(), mean.size());
		covariance.zeroize();
		for (size_t i = 0; i < mean.size(); i++) {
			for (size_t k = 0; k < sols.size(); k++) {
				covariance[i][i] += (sols[k]->variable()[i] - mean[i])
					* (sols[k]->variable()[i] - mean[i]);
			}
			covariance[i][i] /= sols.size();
		}
	}
}
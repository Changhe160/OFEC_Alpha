#include "function.h"
#include <sstream>
#include "../../../../core/global.h"
#include "../../../../utility/functional.h"

namespace ofec {
	void Function::reset() {
		Continuous::reset();
		m_scaled = false;
		m_rotated = false;
		m_translated = false;
		m_noisy = false;
		m_scale = 0;
		m_bias = 0;
		m_condition_number = 1;
		m_rotation.clear();
		m_translation.clear();
		m_original_optima.clear();
	}

	void Function::initialize_(Environment *env) {
		Continuous::initialize_(env);
		resizeObjective(1);
	}

	void Function::updateOptima(Environment *env) {
		Continuous::updateOptima(env);
	}

	void Function::setBias(Real val) {
		m_bias = val;
	}

	void Function::setScale(Real val) {
		m_scale = val;
		m_scaled = true;
	}

	void Function::setRotated(bool flag) {
		m_rotated = flag;
	}

	void Function::setTranlated(bool flag) {
		m_translated = flag;
	}

	void Function::setScaled(bool flag) {
		m_scaled = flag;
	}

	void Function::setConditionNumber(Real c) {
		m_condition_number = c;
	}

	void Function::clear() {
		m_translation.clear();
	}

	void Function::translateZero() {
		for (auto &i : m_translation)
			i = 0;
	}

	bool Function::loadTranslation(const std::string &path) {
		std::string s;
		std::stringstream ss;
		ss << m_number_variables << "Dim.txt";
		s = ss.str();
		s.insert(0, "_Shift_");
		s.insert(0, path);    // data path
		s.insert(0, g_working_directory);
		loadTranslation_(s);
		return true;
	}

	void Function::loadTranslation_(const std::string &path) {
		m_translation.resize(m_number_variables);
		std::ifstream in(path);
		if (in.fail()) {
			if (m_original_optima.isSolutionGiven())
				setTranslation(m_original_optima.solution(0).variable().data(),m_random.get());
			else {
				std::vector<Real> zero(m_number_variables, 0);
				setTranslation(zero.data(),m_random.get());
			}
			std::ofstream out(path);
			for (auto &i : m_translation)
				out << i << " ";
			out.close();
		}
		else {
			for (auto &i : m_translation)
				in >> i;
		}
		in.close();
		m_translated = true;
	}

	void Function::setTranslation(const Real *opt_var, Random* rnd) {
		// Initial the location of shifted global optimum
		m_translation.resize(m_number_variables);
		for (size_t j = 0; j < m_number_variables; ++j) {
			Real x, rl, ru, range;
			x = opt_var[j];
			ru = m_domain[j].limit.second - x;
			rl = x - m_domain[j].limit.first;
			range = rl < ru ? rl : ru;
			m_translation[j] = rnd->uniform.nextNonStd(-range, range);
		}
		m_translated = true;
	}

	bool Function::loadRotation(const std::string &path) {
		std::string s;
		std::stringstream ss;
		ss << m_number_variables << "Dim.txt";
		s = ss.str();
		s.insert(0, "_RotM_");

		s.insert(0, path);// data path
		s.insert(0, g_working_directory);

		loadRotation_(s);

		return true;
	}

	void Function::loadRotation_(const std::string &path) {
		m_rotation.resize(m_number_variables, m_number_variables);
		std::ifstream in;
		in.open(path);
		if (in.fail()) {
			setRotation();
			std::ofstream out(path);
			m_rotation.print(out);
			out.close();
		}
		else {
			m_rotation.load(in);
		}
		in.close();
		m_rotated = true;
	}

	void Function::setRotation() {
		
		m_rotation.generateRotationClassical(&m_random->normal, m_condition_number);
	}

	void Function::translate(Real *x) const {
		for (size_t i = 0; i < m_number_variables; ++i) {
			x[i] -= m_original_optima.solution(0).variable()[i];
		}
		if (m_translated) {
			for (size_t i = 0; i < m_number_variables; ++i) {
				x[i] -= m_translation[i];
			}
		}
	}

	void Function::translateOrigin(Real *x) const {
		for (size_t i = 0; i < m_number_variables; ++i) {
			x[i] += m_original_optima.solution(0).variable()[i];
		}
	}

	void Function::rotate(Real *x) const {
		if (m_rotated) {
			Real *x_ = new Real[m_number_variables];
			std::copy(x, x + m_number_variables, x_);
			for (size_t i = 0; i < m_number_variables; ++i) {
				x[i] = 0;
				for (size_t j = 0; j < m_number_variables; ++j) {
					x[i] += m_rotation[j][i] * x_[j];
				}
			}
			delete[] x_;
			x_ = 0;
		}
	}

	void Function::scale(Real *x) const {
		if (m_scaled) {
			for (size_t i = 0; i < m_number_variables; ++i)
				x[i] /= m_scale;
		}
	}

	void Function::irregularize(Real *x) {
		// this method from BBOB
		Real c1, c2, x_;
		for (size_t i = 0; i < m_number_variables; ++i) {
			if (x[i] > 0) {
				c1 = 10;	c2 = 7.9;
			}
			else {
				c1 = 5.5;	c2 = 3.1;
			}
			if (x[i] != 0) {
				x_ = log(fabs(x[i]));
			}
			else x_ = 0;
			x[i] = sign(x[i]) * exp(x_ + 0.049 * (sin(c1 * x_) + sin(c2 * x_)));
		}
	}

	void Function::asyemmetricalize(Real *x, Real belta) {
		// this method from BBOB
		if (m_number_variables == 1) return;
		for (size_t i = 0; i < m_number_variables; ++i) {
			if (x[i] > 0) {
				x[i] = pow(x[i], 1 + belta * i * sqrt(x[i]) / (m_number_variables - 1));
			}
		}
	}

	void Function::resizeTranslation(size_t n) {
		m_translation.resize(n);
	}

	void Function::resizeRotation(size_t n) {
		m_rotation.resize(n, n);
	}

	void Function::setOriginalGlobalOpt(Real *opt) {
		if (m_number_objectives > 1)
			throw Exception("Function::setOriginalGlobalOpt only for problems with a single obj");
		m_original_optima.clear();
		Solution<> temp_sol(m_number_objectives, m_number_constraints, m_number_variables);
		if (opt) {
			for (int i = 0; i < m_number_variables; i++) {
				temp_sol.variable()[i] = opt[i];
			}
		} 
		else {
			for (auto &i : temp_sol.variable()) {
				i = 0.;
			}
		}
		auto x_ = temp_sol.variable();
		evaluateOriginalObj(x_.data(), temp_sol.objective());
		m_original_optima.appendSolution(temp_sol);
	}

	void Function::setGlobalOpt(Real *tran) {
		if (m_number_objectives > 1)
			throw Exception("Function::setGlobalOpt only for problems with a single obj");
		Solution<> temp_sol(m_number_objectives, m_number_constraints, m_number_variables);
		for (int i = 0; i < m_number_variables; ++i) {
			if (tran != 0) temp_sol.variable()[i] = tran[i] + m_original_optima.solution(0).variable()[i];
			else temp_sol.variable()[i] = m_original_optima.solution(0).variable()[i];
		}
		evaluate(temp_sol.variable(), temp_sol.objective(), temp_sol.constraint());
		auto new_optima = new Optima<>();
		new_optima->appendSolution(temp_sol);
		m_optima.reset(new_optima);
	}

	void Function::evaluateObjective(Real *x, std::vector<Real> &obj) const {
		translate(x);
		scale(x);
		rotate(x);
		translateOrigin(x);
		evaluateOriginalObj(x, obj);
	}
}
/******************************************************************************
* Project:Open Frameworks for Evolutionary Computation (OFEC)
*******************************************************************************
* Author: Changhe Li and Li Zhou
* Email: changhe.lw@gmail.com, 441837060@qq.com
* Language: C++
*-------------------------------------------------------------------------------
*  This file is part of OFEC. This library is free software;
*  you can redistribute it and/or modify it under the terms of the
*  GNU General Public License as published by the Free Software
*  Foundation; either version 2, or (at your option) any later version.
*
*  see https://github.com/Changhe160/OFEC for more information
*
*-------------------------------------------------------------------------------
* class Function defines basic operations for function/numerical optimization problems
*
*********************************************************************************/

#ifndef OFEC_FUNCTION_H
#define OFEC_FUNCTION_H

#include "../../../../core/problem/continuous/continuous.h"
#include "../../../../utility/linear_algebra/matrix.h"

namespace ofec {
	class Function : virtual public Continuous {
	public:
		void reset() override;
		void setBias(Real val);
		void setScale(Real val);
		void setGlobalOpt(Real* tran = 0);
		void setOriginalGlobalOpt(Real* opt = 0);
		void setConditionNumber(Real c);
		void setRotated(bool flag);
		void setTranlated(bool flag);
		void setScaled(bool flag);

		Real bias() const { return m_bias; }
		Real scale() const { return m_scale; }
		Real conditionNumber() const { return m_condition_number; }
		Real translation(size_t i) const { return m_translation[i]; }
		const Matrix& rotation() const { return m_rotation; }
		const std::vector<Real>& translation() const { return m_translation; }
		const Optima<>& originalOptima() const { return m_original_optima; }

		virtual bool loadRotation(const std::string &path);
		virtual bool loadTranslation(const std::string &path);

		Matrix& rotation() { return m_rotation; }
		std::vector<Real>& translation() { return m_translation; }

	protected:
		void initialize_(Environment *env) override;
		void updateOptima(Environment *env) override;
		void evaluateObjective(Real *x, std::vector<Real> &obj) const final override;
		virtual void evaluateOriginalObj(Real *x, std::vector<Real> &obj) const {}
		virtual void clear();
		virtual void loadTranslation_(const std::string& path);
		virtual void setTranslation(const Real* opt_var, Random* rnd);
		virtual void loadRotation_(const std::string& path);
		virtual void setRotation();

		void translateZero();
		void resizeTranslation(size_t n);
		void resizeRotation(size_t n);
		void irregularize(Real* x);
		void asyemmetricalize(Real* x, Real belta);
		void translate(Real* x) const;
		void translateOrigin(Real* x) const;
		void rotate(Real* x) const;
		void scale(Real* x) const;

	protected:
		// translation in variable space
		std::vector<Real> m_translation;
		bool m_scaled, m_rotated, m_translated, m_noisy;
		Real m_scale, m_bias;
		Real m_condition_number;
		// dim: num_var * num_var
		Matrix m_rotation;
		Optima<> m_original_optima;
	};
	template<typename T> Function* createFunction() {
		return T::create();
	}
	typedef Function* (*pFun)();
	typedef std::vector<pFun> BasicFunctions;
}
#endif // !OFEC_FUNCTION_H
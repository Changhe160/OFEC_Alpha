/*************************************************************************
* Project:Open Frameworks for Evolutionary Computation (OFEC)
*************************************************************************
* Author: Changhe Li
* Email: changhe.lw@gmail.com
* Language: C++
*************************************************************************
*  This file is part of OFEC. This library is free software;
*  you can redistribute it and/or modify it under the terms of the
*  GNU General Public License as published by the Free Software
*  Foundation; either version 2, or (at your option) any later version.
*************************************************************************/

#ifndef OFEC_BBOB_H
#define OFEC_BBOB_H
#include "../../../../../core/problem/continuous/continuous.h"
#include "../../../../../utility/matrix.h"

namespace OFEC {

	class BBOB final : public continuous {
	public:
		BBOB(const std::string &name, size_t size_var, size_t size_obj);
		BBOB(param_map &v);
	protected:
		void initialize();
		void computeXopt();
		real computeFopt();
		real penalize(real *x);
		bool loadRotation(double base);
		void computeRotation(matrix& rot, size_t Dim);
		void reshape(matrix &B, std::vector<real>& vector, size_t m, size_t n);
		void irregularize(real *x);
		void asyemmetricalize(real *x, double belta);
		evaluation_tag evaluate_(base &s, caller call, bool effective_fes, bool constructed);
	private:
		void Sphere_F01(real *x, std::vector<real>& obj);
		void Ellipsoidal_F02(real *x, std::vector<real>& obj);
		void Rastrigin_F03(real *x, std::vector<real>& obj);
		void BucheRastrigin_F04(real *x, std::vector<real>& obj);
		void Slope_F05(real *x, std::vector<real>& obj);
		void Sector_F06(real *x, std::vector<real>& obj);
		void StepEllipsoid_F07(real *x, std::vector<real>& obj);
		void OriginalRosenbrock_F08(real *x, std::vector<real>& obj);
		void RotatedRosenbrock_F09(real *x, std::vector<real>& obj);
		void NonseparableEllipsoid_F10(real *x, std::vector<real>& obj);
		void Discus_F11(real *x, std::vector<real>& obj);
		void BentCigar_F12(real *x, std::vector<real>& obj);
		void SharpRidge_F13(real *x, std::vector<real>& obj);
		void DifferentPowers_F14(real *x, std::vector<real>& obj);
		void NonseparableRastrigin_F15(real *x, std::vector<real>& obj);
		void Weierstrass_F16(real *x, std::vector<real>& obj);
		void SchaffersF7_F17(real *x, std::vector<real>& obj);
		void IllconditionedSchaffersF7_F18(real *x, std::vector<real>& obj);
		void CompositeGriewankRosenbrock_F19(real *x, std::vector<real>& obj);
		void Schwefel_F20(real *x, std::vector<real>& obj);
		void GallagherGaussian101mePeaks_F21(real *x, std::vector<real>& obj);
		void GallagherGaussian21hiPeaks_F22(real *x, std::vector<real>& obj);
		void Katsuura_F23(real *x, std::vector<real>& obj);
		void LunacekBiRastrigin_F24(real *x, std::vector<real>& obj);
	private:
		typedef void (BBOB::*function_ptr)(real *x, std::vector<real>& obj);
		double m_beta, m_alpha;
		function_ptr m_fun = nullptr;
		matrix m_rot, m_rot2, m_linearTF;
		std::vector<real> m_norRand;
		real m_mu, m_scales, m_bias;
		real m_condition_number;
	
		//for F21 F22
		std::vector<double> m_peakvalues;
		std::vector < std::vector < double >> m_arrScales;
		std::vector < std::vector < real >> m_Xlocal;
		//for F16
		std::vector<double> m_aK, m_bK;
		double m_F0;

	};
	int compare_doubles(const void *a, const void *b);
}
#endif // !OFEC_BBOB_H

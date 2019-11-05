/*************************************************************************
* Project:Open Frameworks for Evolutionary Computation (OFEC)
*************************************************************************
* Author: Changhe Li and Li Zhou
* Email: changhe.lw@gmail.com, 441837060@qq.com
* Language: C++
*************************************************************************
*  This file is part of OFEC. This library is free software;
*  you can redistribute it and/or modify it under the terms of the
*  GNU General Public License as published by the Free Software
*  Foundation; either version 2, or (at your option) any later version.
*************************************************************************/
// source code refered to bbob.v15.02, http://coco.gforge.inria.fr/doku.php?id=bbob-2015-downloads
#ifndef OFEC_BBOB_H
#define OFEC_BBOB_H
#include "../../../../../core/problem/continuous/continuous.h"
#include "../../../../../utility/matrix.h"
#include <string>
#include <algorithm>

namespace OFEC {
	class BBOB final : public continuous {
	public:
		BBOB(const std::string &name, size_t size_var, size_t size_obj);
		BBOB(param_map &v);
		void initialize();
	protected:
		void compute_Xopt();
		real compute_Fopt();
		real penalize(real *x);
		bool load_rotation(real base_);
		void compute_rotation(matrix& rot, size_t Dim);
		void reshape(matrix &B, std::vector<real>& vector, size_t m, size_t n);
		void irregularize(real *x);
		void asyemmetricalize(real *x, real belta);
		void evaluate_objective(real *x, std::vector<real>& obj) override;
	private:
		void f1_sphere(real *x, std::vector<real>& obj);
		void f2_ellipsoidale(real *x, std::vector<real>& obj);
		void f3_rastrigin(real *x, std::vector<real>& obj);
		void f4_buche_rastrigin(real *x, std::vector<real>& obj);
		void f5_slope(real *x, std::vector<real>& obj);
		void f6_sector(real *x, std::vector<real>& obj);
		void f7_step_ellipsoid(real *x, std::vector<real>& obj);
		void f8_original_rosenbrock(real *x, std::vector<real>& obj);
		void f9_rotated_rosenbrock(real *x, std::vector<real>& obj);
		void f10_nonseparable_ellipsoid(real *x, std::vector<real>& obj);
		void f11_discus(real *x, std::vector<real>& obj);
		void f12_bent_cigar(real *x, std::vector<real>& obj);
		void f13_sharp_ridge(real *x, std::vector<real>& obj);
		void f14_different_powers(real *x, std::vector<real>& obj);
		void f15_nonseparable_rastrigin(real *x, std::vector<real>& obj);
		void f16_weierstrass(real *x, std::vector<real>& obj);
		void f17_schaffers_F7(real *x, std::vector<real>& obj);
		void f18_illconditioned_schaffers_F7(real *x, std::vector<real>& obj);
		void f19_composite_griewank_rosenbrock(real *x, std::vector<real>& obj);
		void f20_schwefel(real *x, std::vector<real>& obj);
		void f21_gallagher_gaussian101me_peaks(real *x, std::vector<real>& obj);
		void f22_gallagher_gaussian21hi_peaks(real *x, std::vector<real>& obj);
		void f23_katsuura(real *x, std::vector<real>& obj);
		void f24_lunacekbi_rastrigin(real *x, std::vector<real>& obj);
	private:
		typedef void (BBOB::*function_ptr)(real *x, std::vector<real>& obj);
		real m_beta, m_alpha;
		function_ptr m_fun = nullptr;
		matrix m_rot, m_rot2, m_linearTF;
		std::vector<real> m_norRand;
		real m_mu, m_scales, m_bias;
		real m_condition_number;
	
		//for f21 and f22
		std::vector<real> m_peak_values;
		std::vector < std::vector < real >> m_arrScales;
		std::vector < std::vector < real >> m_Xlocal;
		//for F16
		std::vector<real> m_aK, m_bK;
		real m_F0;

	};
	int compare_doubles(const void *a, const void *b);
	using BBOB_F01 = BBOB;
	using BBOB_F02 = BBOB;
	using BBOB_F03 = BBOB;
	using BBOB_F04 = BBOB;
	using BBOB_F05 = BBOB;
	using BBOB_F06 = BBOB;
	using BBOB_F07 = BBOB;
	using BBOB_F08 = BBOB;
	using BBOB_F09 = BBOB;
	using BBOB_F10 = BBOB;
	using BBOB_F11 = BBOB;
	using BBOB_F12 = BBOB;
	using BBOB_F13 = BBOB;
	using BBOB_F14 = BBOB;
	using BBOB_F15 = BBOB;
	using BBOB_F16 = BBOB;
	using BBOB_F17 = BBOB;
	using BBOB_F18 = BBOB;
	using BBOB_F19 = BBOB;
	using BBOB_F20 = BBOB;
	using BBOB_F21 = BBOB;
	using BBOB_F22 = BBOB;
	using BBOB_F23 = BBOB;
	using BBOB_F24 = BBOB;
}
#endif // !OFEC_BBOB_H

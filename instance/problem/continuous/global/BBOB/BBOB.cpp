#include "BBOB.h"

namespace OFEC {

	constexpr size_t NHIGHPEAKS21 = 101;
	constexpr size_t NHIGHPEAKS22 = 21;
	int compare_doubles(const void *a, const void *b)
	{
		real temp = (*(real*)a) - (*(real*)b);
		if (temp > 0)
			return 1;
		else if (temp < 0)
			return -1;
		else
			return 0;
	}
	BBOB::BBOB(const std::string &name, size_t size_var, size_t size_obj) :problem(name, size_var, size_obj), continuous(name, size_var, size_obj), \
		m_rot(size_var), m_rot2(size_var), m_norRand(size_var*size_var), m_linearTF(size_var) {
		
	}
	BBOB::BBOB(param_map &v) :BBOB((v.at("proName")), (v.at("numDim")), 1) {
		
	}

	void BBOB::initialize() {
		m_variable_monitor = true;
		set_range(-5, 5);
		//set_init_range(-5, 5);
		m_bias = compute_Fopt();
		m_optima.append(m_bias);
        m_objective_monitor = false;
		compute_Xopt();
		m_optima.set_flag_variable(true);

		if (m_name == "GOP_BBOB_F01_Sphere") {
			m_fun = &BBOB::f1_sphere;
		}
		else if (m_name == "GOP_BBOB_F02_Ellipsoidal") {
			m_condition_number = 1e6;
			m_fun = &BBOB::f2_ellipsoidale;
		}
		else if (m_name == "GOP_BBOB_F03_Rastrigin") {
			m_condition_number = 10;
			m_beta = 0.2;
			m_fun = &BBOB::f3_rastrigin;
		}
		else if (m_name == "GOP_BBOB_F04_BucheRastrigin") {
			m_condition_number = 10.;
			m_alpha = 100.;
			for (size_t i = 0; i < m_variable_size; i += 2) {
				m_optima.variable(0)[i] = fabs(m_optima.variable(0)[i]); /*Skew*/
			}
			m_fun = &BBOB::f4_buche_rastrigin;
		}
		else if (m_name == "GOP_BBOB_F05_Slope") {
			m_alpha = 100.;
			m_fun = &BBOB::f5_slope;
			for (size_t i = 0; i < m_variable_size; ++i)
			{
				real tmp = pow(sqrt(m_alpha), ((real)i) / ((real)(m_variable_size - 1)));
				if (m_optima.variable(0)[i] > 0)
				{
					m_optima.variable(0)[i] = 5.;
				}
				else if (m_optima.variable(0)[i] < 0)
				{
					m_optima.variable(0)[i] = -5.;
				}
				m_bias += 5. * tmp;
			}
		}
		else if (m_name == "GOP_BBOB_F06_Sector") {
			m_fun = &BBOB::f6_sector;
			m_alpha = 100.;
			m_condition_number = 10.;
			load_rotation(sqrt(m_condition_number));
		}
		else if (m_name == "GOP_BBOB_F07_StepEllipsoid") {
			m_fun = &BBOB::f7_step_ellipsoid;
			m_condition_number = 100.;
			m_alpha = 10.;
			load_rotation(sqrt(m_condition_number));
		}
		else if (m_name == "GOP_BBOB_F08_OriginalRosenbrock") {
			m_fun = &BBOB::f8_original_rosenbrock;
			m_scales = fmax(1., sqrt((real)m_variable_size) / 8.);
			for (size_t i = 0; i < m_variable_size; ++i)
				m_optima.variable(0)[i] *= 0.75;
		}
		else if (m_name == "GOP_BBOB_F09_RotatedRosenbrock") {
			m_fun = &BBOB::f9_rotated_rosenbrock;
			m_scales = fmax(1., sqrt((real)m_variable_size) / 8.);
			compute_rotation(m_rot, m_variable_size);
			for (size_t i = 0; i < m_variable_size; ++i)
			{
				for (size_t j = 0; j < m_variable_size; ++j)
					m_linearTF[i][j] = m_scales * m_rot[i][j];
			}
			for (size_t i = 0; i < m_variable_size; ++i)
			{
				m_optima.variable(0)[i] = 0.;
				for (size_t j = 0; j < m_variable_size; ++j)
				{
					m_optima.variable(0)[i] += m_linearTF[j][i] * 0.5 / m_scales / m_scales;
				}
			}
		}
		else if (m_name == "GOP_BBOB_F10_NonseparableEllipsoid") {
			m_fun = &BBOB::f10_nonseparable_ellipsoid;
			compute_rotation(m_rot, m_variable_size);
			m_condition_number = 1e6;
		}
		else if (m_name == "GOP_BBOB_F11_Discus") {
			m_fun = &BBOB::f11_discus;
			compute_rotation(m_rot, m_variable_size);
			m_condition_number = 1e6;
		}
		else if (m_name == "GOP_BBOB_F12_BentCigar") {
			m_fun = &BBOB::f12_bent_cigar;
			compute_rotation(m_rot, m_variable_size);
			m_condition_number = 1e6;
			m_beta = 0.5;
		}
		else if (m_name == "GOP_BBOB_F13_SharpRidge") {
			m_fun = &BBOB::f13_sharp_ridge;
			m_condition_number = 10.;
			m_alpha = 100.;
			load_rotation(sqrt(m_condition_number));
		}
		else if (m_name == "GOP_BBOB_F14_DifferentPowers") {
			m_fun = &BBOB::f14_different_powers;
			m_alpha = 4.;
			compute_rotation(m_rot, m_variable_size);
		}
		else if (m_name == "GOP_BBOB_F15_NonseparableRastrigin") {
			m_fun = &BBOB::f15_nonseparable_rastrigin;
			m_condition_number = 10.;
			m_beta = 0.2;
			load_rotation(sqrt(m_condition_number));
		}
		else if (m_name == "GOP_BBOB_F16_Weierstrass") {
			m_fun = &BBOB::f16_weierstrass;
			m_condition_number = 100.;
			m_F0 = 0.;
			m_aK.resize(12);
			m_bK.resize(12);
			load_rotation(1. / sqrt(m_condition_number));
			for (size_t i = 0; i < 12; ++i) /// number of summands, 20 in CEC2005, 10/12 saves 30% of time
			{
				m_aK[i] = pow(0.5, (real)i);
				m_bK[i] = pow(3., (real)i);
				m_F0 += m_aK[i] * cos(2 * OFEC_PI * m_bK[i] * 0.5);
			}
		}
		else if (m_name == "GOP_BBOB_F17_SchaffersF7") {
			m_fun = &BBOB::f17_schaffers_F7;
			compute_rotation(m_rot, m_variable_size);
			compute_rotation(m_rot2, m_variable_size);
			m_condition_number = 10.;
			m_beta = 0.5;
		}
		else if (m_name == "GOP_BBOB_F18_IllconditionedSchaffersF7") {
			m_fun = &BBOB::f18_illconditioned_schaffers_F7;
			compute_rotation(m_rot, m_variable_size);
			compute_rotation(m_rot2, m_variable_size);
			m_condition_number = 1e3;
			m_beta = 0.5;
		}
		else if (m_name == "GOP_BBOB_F19_CompositeGriewankRosenbrock") {
			m_fun = &BBOB::f19_composite_griewank_rosenbrock;
			m_scales = fmax(1., sqrt((real)m_variable_size) / 8.);
			compute_rotation(m_rot, m_variable_size);
			for (size_t i = 0; i < m_variable_size; ++i)
			{
				for (size_t j = 0; j < m_variable_size; ++j)
				{
					m_linearTF[i][j] = m_scales * m_rot[i][j];
				}
			}
			for (size_t i = 0; i < m_variable_size; ++i)
			{
				m_optima.variable(0)[i] = 0.;
				for (size_t j = 0; j < m_variable_size; ++j)
				{
					m_optima.variable(0)[i] += m_linearTF[j][i] * 0.5 / m_scales / m_scales;
				}
			}
		}
		else if (m_name == "GOP_BBOB_F20_Schwefel") {
			m_fun = &BBOB::f20_schwefel;
			m_condition_number = 10.;
			std::vector<real> tmpvect(m_variable_size);
			for (auto&i : tmpvect) i = global::ms_global->m_uniform[caller::Problem]->next();
			for (size_t i = 0; i < m_variable_size; ++i)
			{
				m_optima.variable(0)[i] = 0.5 * 4.2096874633;
				if (tmpvect[i] - 0.5 < 0)
					m_optima.variable(0)[i] *= -1.;
			}
		}
		else if (m_name == "GOP_BBOB_F21_GallagherGaussian101mePeaks") {
			m_peak_values.resize(NHIGHPEAKS21);
			m_Xlocal.resize(m_variable_size, std::vector<real>(NHIGHPEAKS21));

			for (size_t i = 0; i < NHIGHPEAKS21; ++i) {
				m_arrScales.push_back(std::vector<real>(m_variable_size));
			}

			m_fun = &BBOB::f21_gallagher_gaussian101me_peaks;

			std::vector<real> fitvalues = { (real)1.1, (real)9.1 };
			m_condition_number = 1000.;
			std::vector<size_t> rperm(std::max(m_variable_size, NHIGHPEAKS21 - 1));
			std::vector<real> arrCondition(NHIGHPEAKS21);
			std::vector<real> peaks(NHIGHPEAKS21 - 1);
			compute_rotation(m_rot, m_variable_size);

			for (size_t i = 0; i < NHIGHPEAKS21 - 1; ++i) {
				peaks[i] = global::ms_global->m_uniform[caller::Problem]->next();
			}

			for (size_t i = 0; i < NHIGHPEAKS21 - 1; ++i)
				rperm[i] = i;

			qsort(rperm.data(), NHIGHPEAKS21 - 1, sizeof(int), compare_doubles);
			arrCondition[0] = sqrt(m_condition_number);
			m_peak_values[0] = 10;
			for (size_t i = 1; i < NHIGHPEAKS21; ++i)
			{
				arrCondition[i] = pow(m_condition_number, (real)(rperm[i - 1]) / ((real)(NHIGHPEAKS21 - 2)));
				m_peak_values[i] = (real)(i - 1) / (real)(NHIGHPEAKS21 - 2) * (fitvalues[1] - fitvalues[0]) + fitvalues[0];
			}

			//peaks.resize(m_variable_size);
			for (size_t i = 0; i < NHIGHPEAKS21; ++i)
			{
				//for(auto&j: peaks) j = global::ms_global->m_uniform[caller::Problem]->next();

				for (size_t j = 0; j < m_variable_size; ++j)
					rperm[j] = j;

				qsort(rperm.data(), m_variable_size, sizeof(int), compare_doubles);

				for (size_t j = 0; j < m_variable_size; ++j)
				{
					m_arrScales[i][j] = pow(arrCondition[i], ((real)rperm[j]) / ((real)(m_variable_size - 1)) - 0.5);
				}
			}

			peaks.resize(m_variable_size * NHIGHPEAKS21);
			for (auto&j : peaks) j = global::ms_global->m_uniform[caller::Problem]->next();
			for (size_t i = 0; i < m_variable_size; ++i)
			{
				m_optima.variable(0)[i] = 0.8 * (10. * peaks[i] - 5.);
				for (size_t j = 0; j < NHIGHPEAKS21; ++j)
				{
					m_Xlocal[i][j] = 0.;
					for (size_t k = 0; k < m_variable_size; ++k)
					{
						m_Xlocal[i][j] += m_rot[i][k] * (10. * peaks[j * m_variable_size + k] - 5.);
					}
					if (j == 0)
						m_Xlocal[i][j] *= 0.8;
				}
			}
		}
		else if (m_name == "GOP_BBOB_F22_GallagherGaussian21hiPeaks") {
			m_fun = &BBOB::f22_gallagher_gaussian21hi_peaks;

			m_peak_values.resize(NHIGHPEAKS22);
			m_Xlocal.resize(m_variable_size, std::vector<real>(NHIGHPEAKS22));
			for (size_t i = 0; i < NHIGHPEAKS22; ++i) {
				m_arrScales.push_back(std::vector<real>(m_variable_size));
			}

			std::vector<real> fitvalues = { (real)1.1, (real)9.1 };
			m_condition_number = 1000.;
			std::vector<size_t> rperm(std::max(m_variable_size, NHIGHPEAKS22 - 1));
			std::vector<real> arrCondition(NHIGHPEAKS22);
			std::vector<real> peaks(NHIGHPEAKS22 - 1);

			compute_rotation(m_rot, m_variable_size);

			for (size_t i = 0; i < NHIGHPEAKS22 - 1; ++i) {
				peaks[i] = global::ms_global->m_uniform[caller::Problem]->next();
			}

			for (size_t i = 0; i < NHIGHPEAKS22 - 1; ++i)
				rperm[i] = i;

			qsort(rperm.data(), NHIGHPEAKS22 - 1, sizeof(int), compare_doubles);

			arrCondition[0] = sqrt(m_condition_number);
			m_peak_values[0] = 10;
			for (size_t i = 1; i < NHIGHPEAKS22; ++i)
			{
				arrCondition[i] = pow(m_condition_number, (real)(rperm[i - 1]) / ((real)(NHIGHPEAKS22 - 2)));
				m_peak_values[i] = (real)(i - 1) / (real)(NHIGHPEAKS22 - 2) * (fitvalues[1] - fitvalues[0]) + fitvalues[0];
			}

			//peaks.resize(m_variable_size);
			for (size_t i = 0; i < NHIGHPEAKS22; ++i)
			{
				//for (auto&j : peaks) j = global::ms_global->m_uniform[caller::Problem]->next();
				for (size_t j = 0; j < m_variable_size; ++j)
					rperm[j] = j;
				qsort(rperm.data(), m_variable_size, sizeof(int), compare_doubles);
				for (size_t j = 0; j < m_variable_size; ++j)
				{
					m_arrScales[i][j] = pow(arrCondition[i], ((real)rperm[j]) / ((real)(m_variable_size - 1)) - 0.5);
				}
			}

			peaks.resize(m_variable_size * NHIGHPEAKS22);
			for (auto&j : peaks) j = global::ms_global->m_uniform[caller::Problem]->next();
			for (size_t i = 0; i < m_variable_size; ++i)
			{
				m_optima.variable(0)[i] = 0.8 * (10. * peaks[i] - 5.);
				for (size_t j = 0; j < NHIGHPEAKS22; ++j)
				{
					m_Xlocal[i][j] = 0.;
					for (size_t k = 0; k < m_variable_size; ++k)
					{
						m_Xlocal[i][j] += m_rot[i][k] * (10. * peaks[j * m_variable_size + k] - 5.);
					}
					if (j == 0)
						m_Xlocal[i][j] *= 0.8;
				}
			}
		}
		else if (m_name == "GOP_BBOB_F23_Katsuura") {
			m_fun = &BBOB::f23_katsuura;
			m_condition_number = 100.;
			load_rotation(sqrt(m_condition_number));
		}
		else if (m_name == "GOP_BBOB_F24_LunacekBiRastrigin") {

			m_fun = &BBOB::f24_lunacekbi_rastrigin;
			m_condition_number = 100.;
			load_rotation(sqrt(m_condition_number));
			m_mu = 2.5;

			std::vector<real> tmpvect(m_variable_size);
			for (size_t i = 0; i < m_variable_size; ++i)
			{
				tmpvect[i] = global::ms_global->m_uniform[caller::Problem]->next();
			}

			for (size_t i = 0; i < m_variable_size; ++i)
			{
				m_optima.variable(0)[i] = 0.5 * m_mu;
				if (tmpvect[i] < 0.)
					m_optima.variable(0)[i] *= -1.;
			}
		}
		m_initialized = true;
	}


	void BBOB::compute_Xopt() {
		variable_vector<real> temp(m_variable_size);
		for (size_t i = 0; i < m_variable_size; ++i)
		{
			temp[i] = 8 * floor(1e4 * global::ms_global->m_uniform[caller::Problem]->next()) / 1e4 - 4;
			if (temp[i] == 0.0) {
				temp[i] = -1e-5;
			}
		}
		m_optima.append(temp);
	}

	real BBOB::compute_Fopt() {
		real gval, gval2;
		gval = global::ms_global->m_normal[caller::Problem]->next();
		gval2 = global::ms_global->m_normal[caller::Problem]->next();
		return fmin(1000., fmax(-1000., (round(100.*100.*gval / gval2) / 100.)));
	}

	real BBOB::penalize(real *x) {
		real tmp, fPen = 0;
		for (size_t i = 0; i < m_variable_size; ++i) {
			tmp = fabs(x[i]) - 5.;
			if (tmp > 0.)
				fPen += tmp * tmp;
		}
		return fPen;
	}

	void BBOB::reshape(matrix &B, std::vector<real>& vector, size_t m, size_t n) {
		size_t i, j;
		for (i = 0; i < m; ++i)
		{
			for (j = 0; j < n; ++j)
			{
				B[i][j] = vector[j * m + i];
			}
		}

	}

	void BBOB::compute_rotation(matrix& rot, size_t Dim) {

		for (auto &i : m_norRand) i = global::ms_global->m_normal[caller::Problem]->next();
		reshape(rot, m_norRand, Dim, Dim);
		/*1st coordinate is row, 2nd is column.*/
		size_t i, j, k;
		real prod;
		for (i = 0; i < Dim; ++i)
		{
			for (j = 0; j < i; ++j)
			{
				prod = 0;
				for (k = 0; k < Dim; ++k)
				{
					prod += rot[k][i] * rot[k][j];
				}
				for (k = 0; k < Dim; ++k)
				{
					rot[k][i] = rot[k][i] - prod * rot[k][j];
				}
			}
			prod = 0;
			for (k = 0; k < Dim; ++k)
			{
				prod += rot[k][i] * rot[k][i];
			}
			for (k = 0; k < Dim; ++k)
			{
				//rot[k][i] /= sqrt(prod);
				rot[k][i] = rot[k][i] / sqrt(prod);
			}
		}
	}
	bool BBOB::load_rotation(real base_) {
		compute_rotation(m_rot, m_variable_size);
		compute_rotation(m_rot2, m_variable_size);
		/* decouple scaling from function definition*/
		size_t i, j, k;
		for (i = 0; i < m_variable_size; ++i) {
			for (j = 0; j < m_variable_size; ++j) {
				m_linearTF[i][j] = 0.;
				for (k = 0; k < m_variable_size; ++k) {
					m_linearTF[i][j] += m_rot[i][k] * pow(base_, ((real)k) / ((real)(m_variable_size - 1))) * m_rot2[k][j];
				}
			}
		}
		return true;
	}

	void BBOB::evaluate_objective(real *x, std::vector<real>& obj){

		(this->*m_fun)(x, obj);
	}

	void BBOB::irregularize(real *x) {
		// this method from BBOB
		real c1, c2, x_;
		for (size_t i = 0; i < m_variable_size; ++i) {
			if (x[i]>0) {
				c1 = 10;	c2 = 7.9;
			}
			else {
				c1 = 5.5;	c2 = 3.1;
			}
			if (x[i] != 0) {
				x_ = log(fabs(x[i]));
			}
			else x_ = 0;
			x[i] = sign(x[i])*exp(x_ + 0.049*(sin(c1*x_) + sin(c2*x_)));
		}
	}

	void BBOB::asyemmetricalize(real *x, real belta) {
		// this method from BBOB
		if (m_variable_size == 1) return;
		for (size_t i = 0; i < m_variable_size; ++i) {
			if (x[i]>0) {
				x[i] = pow(x[i], 1 + belta*i*sqrt(x[i]) / (m_variable_size - 1));
			}
		}
	}

	void BBOB::f1_sphere(real *x, std::vector<real>& obj) {

		obj[0] = 0;
		for (size_t i = 0; i < m_variable_size; ++i) {
			x[i] -= m_optima.variable(0)[i];
		}
		for (size_t i = 0; i < m_variable_size; ++i) {
			obj[0] += x[i] * x[i];
		}
		obj[0] += m_bias;
	}

	void BBOB::f2_ellipsoidale(real *x, std::vector<real>& obj) {

		obj[0] = 0;
		for (size_t i = 0; i < m_variable_size; ++i) {
			x[i] -= m_optima.variable(0)[i];
		}
		irregularize(x);

		for (size_t i = 0; i < m_variable_size; ++i) {
			obj[0] += pow(m_condition_number, ((real)i) / ((real)(m_variable_size - 1))) * x[i] * x[i];
		}
		obj[0] += m_bias;
	}

	void BBOB::f3_rastrigin(real *x, std::vector<real>& obj) {

		for (size_t i = 0; i < m_variable_size; ++i) {
			x[i] -= m_optima.variable(0)[i];
		}
		irregularize(x);
		asyemmetricalize(x, m_beta);
		real tmp, tmp2;
		for (size_t i = 0; i < m_variable_size; ++i) {
			tmp = ((real)i) / ((real)(m_variable_size - 1));
			x[i] = pow(sqrt(m_condition_number), tmp) * x[i];
		}
		/* COMPUTATION core*/
		tmp = 0.;
		tmp2 = 0.;
		for (size_t i = 0; i < m_variable_size; ++i) {
			tmp += cos(2 * OFEC_PI*x[i]);
			tmp2 += x[i] * x[i];
		}
		obj[0] = 10 * (m_variable_size - tmp) + tmp2;
		obj[0] += m_bias;
	}

	void BBOB::f4_buche_rastrigin(real *x, std::vector<real>& obj) {

		real fPen = penalize(x);
		fPen *= 1e2;

		real fAdd = m_optima.objective()[0] + fPen;

		for (size_t i = 0; i < m_variable_size; ++i) {
			x[i] -= m_optima.variable(0)[i];
		}

		irregularize(x);
		for (size_t i = 0; i < m_variable_size; ++i)
		{
			if (i % 2 == 0 && x[i] > 0)
				x[i] = sqrt(m_alpha) * x[i];
			x[i] = pow(sqrt(m_condition_number), ((real)i) / ((real)(m_variable_size - 1))) * x[i];
		}
		/* COMPUTATION core*/
		real tmp = 0.;
		real tmp2 = 0.;
		for (size_t i = 0; i < m_variable_size; ++i)
		{
			tmp += cos(2 * OFEC_PI*x[i]);
			tmp2 += x[i] * x[i];
		}
		obj[0] = 10 * (m_variable_size - tmp) + tmp2;
		obj[0] += fAdd;
	}

	void BBOB::f5_slope(real *x, std::vector<real>& obj) {

		/* BOUNDARY HANDLING*/
		/* move "too" good coordinates back into domain*/
		for (size_t i = 0; i < m_variable_size; ++i) {
			if ((m_optima.variable(0)[i] == 5.) && (x[i] > 5))
				x[i] = 5.;
			else if ((m_optima.variable(0)[i] == -5.) && (x[i] < -5))
				x[i] = -5.;
		}

		obj[0] = 0;
		/* COMPUTATION core*/
		for (size_t i = 0; i < m_variable_size; ++i)
		{
			if (m_optima.variable(0)[i] > 0) {
				obj[0] -= pow(sqrt(m_alpha), ((real)i) / ((real)(m_variable_size - 1))) * x[i];
			}
			else {
				obj[0] += pow(sqrt(m_alpha), ((real)i) / ((real)(m_variable_size - 1))) * x[i];
			}
		}
		obj[0] += m_bias;

	}

	void BBOB::f6_sector(real *x, std::vector<real>& obj) {
		/* attractive sector function*/
		size_t i, j; /*Loop over dim*/

					 /* BOUNDARY HANDLING*/
					 /* TRANSFORMATION IN SEARCH SPACE*/
		real tmp;
		std::vector<real> trasX(x, x + m_variable_size);
		for (i = 0; i < m_variable_size; ++i) {
			tmp = 0.;
			for (j = 0; j < m_variable_size; ++j) {
				tmp += m_linearTF[i][j] * (x[j] - m_optima.variable(0)[j]);
			}
			trasX[i] = tmp;
		}

		obj[0] = 0.;
		/* COMPUTATION core*/
		for (i = 0; i < m_variable_size; ++i)
		{
			if (trasX[i] * m_optima.variable(0)[i] > 0)
				trasX[i] *= m_alpha;
			obj[0] += trasX[i] * trasX[i];
		}

		/*MonotoneTFosc...*/
		if (obj[0] > 0)
		{
			obj[0] = pow(exp(log(obj[0]) / 0.1 + 0.49*(sin(log(obj[0]) / 0.1) + sin(0.79*log(obj[0]) / 0.1))), 0.1);
		}
		else if (obj[0] < 0)
		{
			obj[0] = -pow(exp(log(-obj[0]) / 0.1 + 0.49*(sin(0.55 * log(-obj[0]) / 0.1) + sin(0.31*log(-obj[0]) / 0.1))), 0.1);
		}
		obj[0] = pow(obj[0], 0.9);
		obj[0] += m_bias;

	}

	void BBOB::f7_step_ellipsoid(real *x, std::vector<real>& obj) {

		size_t i, j;
		real x1, tmp;

		/* BOUNDARY HANDLING*/
		real fPen = penalize(x);

		real fAdd = m_optima.objective()[0] + fPen;

		std::vector<real> tmpVect(m_variable_size), trasX(m_variable_size);
		/* TRANSFORMATION IN SEARCH SPACE*/
		for (i = 0; i < m_variable_size; ++i) {
			tmpVect[i] = 0.;
			tmp = sqrt(pow(m_condition_number / 10., ((real)i) / ((real)(m_variable_size - 1))));
			for (j = 0; j < m_variable_size; ++j) {
				tmpVect[i] += tmp * m_rot2[i][j] * (x[j] - m_optima.variable(0)[j]);
			}

		}
		x1 = tmpVect[0];

		for (i = 0; i < m_variable_size; ++i) {
			if (fabs(tmpVect[i]) > 0.5)
				tmpVect[i] = round(tmpVect[i]);
			else
				tmpVect[i] = round(m_alpha * tmpVect[i]) / m_alpha;
		}

		for (i = 0; i < m_variable_size; ++i) {
			trasX[i] = 0.;
			for (j = 0; j < m_variable_size; ++j) {
				trasX[i] += m_rot[i][j] * tmpVect[j];
			}
		}

		/* COMPUTATION core*/
		obj[0] = 0.;
		for (i = 0; i < m_variable_size; ++i)
		{
			obj[0] += pow(m_condition_number, ((real)i) / ((real)(m_variable_size - 1))) * trasX[i] * trasX[i];
		}
		obj[0] = 0.1 * fmax(1e-4 * fabs(x1), obj[0]);
		obj[0] += fAdd;
	}

	void BBOB::f8_original_rosenbrock(real *x, std::vector<real>& obj) {

		size_t i;
		real tmp;

		/* TRANSFORMATION IN SEARCH SPACE*/
		std::vector<real> trasX(m_variable_size);
		for (i = 0; i < m_variable_size; ++i) {
			trasX[i] = m_scales * (x[i] - m_optima.variable(0)[i]) + 1;
		}

		/* COMPUTATION core*/
		obj[0] = 0.;
		for (i = 0; i < m_variable_size - 1; ++i)
		{
			tmp = (trasX[i] * trasX[i] - trasX[i + 1]);
			obj[0] += tmp * tmp;
		}
		obj[0] *= 1e2;
		for (i = 0; i < m_variable_size - 1; ++i)
		{
			tmp = (trasX[i] - 1.);
			obj[0] += tmp * tmp;
		}
		obj[0] += m_optima.objective()[0];
	}

	void BBOB::f9_rotated_rosenbrock(real *x, std::vector<real>& obj) {

		size_t i, j;
		real  tmp;

		std::vector<real> trasX(m_variable_size);
		/* TRANSFORMATION IN SEARCH SPACE*/
		for (i = 0; i <m_variable_size; ++i) {
			trasX[i] = 0.5;
			for (j = 0; j < m_variable_size; ++j) {
				trasX[i] += m_linearTF[i][j] * x[j];
			}
		}

		/* COMPUTATION core*/
		obj[0] = 0.;
		for (i = 0; i < m_variable_size - 1; ++i)
		{
			tmp = (trasX[i] * trasX[i] - trasX[i + 1]);
			obj[0] += tmp * tmp;
		}
		obj[0] *= 1e2;
		for (i = 0; i < m_variable_size - 1; ++i)
		{
			tmp = (trasX[i] - 1.);
			obj[0] += tmp * tmp;
		}
		obj[0] += m_bias;
	}

	void BBOB::f10_nonseparable_ellipsoid(real *x, std::vector<real>& obj) {

		size_t i, j;
		std::vector<real> trasX(m_variable_size);
		/* TRANSFORMATION IN SEARCH SPACE*/
		for (i = 0; i < m_variable_size; ++i)
		{
			trasX[i] = 0.;
			for (j = 0; j < m_variable_size; ++j) {
				trasX[i] += m_rot[i][j] * (x[j] - m_optima.variable(0)[j]);
			}
		}

		irregularize(trasX.data());
		/* COMPUTATION core*/
		obj[0] = 0.;
		for (i = 0; i < m_variable_size; ++i)
		{
			obj[0] += pow(m_condition_number, ((real)i) / ((real)(m_variable_size - 1))) * trasX[i] * trasX[i];
		}
		obj[0] += m_bias;
	}

	void BBOB::f11_discus(real *x, std::vector<real>& obj) {

		size_t i, j;
		std::vector<real> trasX(m_variable_size);
		/* TRANSFORMATION IN SEARCH SPACE*/
		for (i = 0; i < m_variable_size; ++i)
		{
			trasX[i] = 0.;
			for (j = 0; j < m_variable_size; ++j) {
				trasX[i] += m_rot[i][j] * (x[j] - m_optima.variable(0)[j]);
			}
		}

		irregularize(trasX.data());

		/* COMPUTATION core*/
		obj[0] = m_condition_number * trasX[0] * trasX[0];
		for (i = 1; i < m_variable_size; ++i)
		{
			obj[0] += trasX[i] * trasX[i];
		}
		obj[0] += m_bias;
	}

	void BBOB::f12_bent_cigar(real *x, std::vector<real>& obj) {

		size_t i, j;

		std::vector<real> trasX(m_variable_size), tmpvect(m_variable_size);
		/* TRANSFORMATION IN SEARCH SPACE*/
		for (i = 0; i < m_variable_size; ++i)
		{
			tmpvect[i] = 0.;
			for (j = 0; j < m_variable_size; ++j) {
				tmpvect[i] += m_rot[i][j] * (x[j] - m_optima.variable(0)[j]);
			}
			if (tmpvect[i] > 0)
			{
				tmpvect[i] = pow(tmpvect[i], 1 + m_beta * ((real)i) / ((real)(m_variable_size - 1)) * sqrt(tmpvect[i]));
			}
		}

		for (i = 0; i < m_variable_size; ++i)
		{
			trasX[i] = 0.;
			for (j = 0; j < m_variable_size; ++j) {
				trasX[i] += m_rot[i][j] * tmpvect[j];
			}
		}

		/* COMPUTATION core*/
		obj[0] = trasX[0] * trasX[0];
		for (i = 1; i < m_variable_size; ++i)
		{
			obj[0] += m_condition_number * trasX[i] * trasX[i];
		}
		obj[0] += m_bias;
	}


	void BBOB::f13_sharp_ridge(real *x, std::vector<real>& obj) {

		size_t i, j/*, k*/; // keep an eye on warnings 

							/* BOUNDARY HANDLING*/

		std::vector<real> trasX(m_variable_size);
		/* TRANSFORMATION IN SEARCH SPACE*/
		for (i = 0; i < m_variable_size; ++i)
		{
			trasX[i] = 0.;
			for (j = 0; j < m_variable_size; ++j) {
				trasX[i] += m_linearTF[i][j] * (x[j] - m_optima.variable(0)[j]);
			}
		}

		/* COMPUTATION core*/
		obj[0] = 0.;
		for (i = 1; i < m_variable_size; ++i)
		{
			obj[0] += trasX[i] * trasX[i];
		}
		obj[0] = m_alpha * sqrt(obj[0]);
		obj[0] += trasX[0] * trasX[0];
		obj[0] += m_bias;
	}

	void BBOB::f14_different_powers(real *x, std::vector<real>& obj) {

		size_t i, j;

		std::vector<real> trasX(m_variable_size);
		/* TRANSFORMATION IN SEARCH SPACE*/
		for (i = 0; i < m_variable_size; ++i)
		{
			trasX[i] = 0.;
			for (j = 0; j < m_variable_size; ++j) {
				trasX[i] += m_rot[i][j] * (x[j] - m_optima.variable(0)[j]);
			}
		}

		/* COMPUTATION core*/
		obj[0] = 0.;
		for (i = 0; i < m_variable_size; ++i)
		{
			obj[0] += pow(fabs(trasX[i]), 2. + m_alpha * ((real)i) / ((real)(m_variable_size - 1)));
		}
		obj[0] = sqrt(obj[0]);
		obj[0] += m_bias;
	}

	void BBOB::f15_nonseparable_rastrigin(real *x, std::vector<real>& obj) {

		size_t i, j;

		real tmp = 0., tmp2 = 0.;
		std::vector<real> trasX(m_variable_size), tmpvect(m_variable_size);
		/* TRANSFORMATION IN SEARCH SPACE*/
		for (i = 0; i < m_variable_size; ++i)
		{
			tmpvect[i] = 0.;
			for (j = 0; j < m_variable_size; ++j)
			{
				tmpvect[i] += m_rot[i][j] * (x[j] - m_optima.variable(0)[j]);
			}
		}
		irregularize(tmpvect.data());
		asyemmetricalize(tmpvect.data(), m_beta);
		for (i = 0; i < m_variable_size; ++i)
		{
			trasX[i] = 0.;
			for (j = 0; j < m_variable_size; ++j)
			{
				trasX[i] += m_linearTF[i][j] * tmpvect[j];
			}
		}
		/* COMPUTATION core*/
		for (i = 0; i < m_variable_size; ++i)
		{
			tmp += cos(2. * OFEC_PI * trasX[i]);
			tmp2 += trasX[i] * trasX[i];
		}
		obj[0] = 10. * ((real)m_variable_size - tmp) + tmp2;
		obj[0] += m_bias;
	}

	void BBOB::f16_weierstrass(real *x, std::vector<real>& obj) {
		size_t i, j;
		real tmp;

		std::vector<real> trasX(m_variable_size), tmpvect(m_variable_size);
		/* BOUNDARY HANDLING*/
		real fPen = penalize(x), Fopt = m_bias;
		Fopt += 10. / (real)m_variable_size * fPen;

		/* TRANSFORMATION IN SEARCH SPACE*/
		for (i = 0; i < m_variable_size; ++i)
		{
			tmpvect[i] = 0.;
			for (j = 0; j < m_variable_size; ++j)
			{
				tmpvect[i] += m_rot[i][j] * (x[j] - m_optima.variable(0)[j]);
			}
		}

		irregularize(tmpvect.data());
		for (i = 0; i < m_variable_size; ++i)
		{
			trasX[i] = 0.;
			for (j = 0; j < m_variable_size; ++j)
			{
				trasX[i] += m_linearTF[i][j] * tmpvect[j];
			}
		}
		/* COMPUTATION core*/
		obj[0] = 0.;
		for (i = 0; i < m_variable_size; ++i)
		{
			tmp = 0.;
			for (j = 0; j < 12; ++j)
			{
				tmp += cos(2 * OFEC_PI * (trasX[i] + 0.5) * m_bK[j]) * m_aK[j];
			}
			obj[0] += tmp;
		}
		obj[0] = 10. * pow(obj[0] / (real)m_variable_size - m_F0, 3.);
		obj[0] += Fopt;
	}

	void BBOB::f17_schaffers_F7(real *x, std::vector<real>& obj) {

		size_t i, j;

		real tmp;

		/* BOUNDARY HANDLING*/
		real fPen = penalize(x), fOpt = m_bias;
		fOpt += 10. * fPen;

		std::vector<real> trasX(m_variable_size), tmpvect(m_variable_size);
		/* TRANSFORMATION IN SEARCH SPACE*/
		for (i = 0; i < m_variable_size; ++i)
		{
			tmpvect[i] = 0.;
			for (j = 0; j < m_variable_size; ++j)
				tmpvect[i] += m_rot[i][j] * (x[j] - m_optima.variable(0)[j]);
		}
		asyemmetricalize(tmpvect.data(), m_beta);

		for (i = 0; i < m_variable_size; ++i)
		{
			trasX[i] = 0.;
			tmp = pow(sqrt(m_condition_number), ((real)i) / ((real)(m_variable_size - 1)));
			for (j = 0; j < m_variable_size; ++j)
			{
				trasX[i] += tmp * m_rot2[i][j] * tmpvect[j];
			}
		}

		/* COMPUTATION core*/
		obj[0] = 0.;
		for (i = 0; i < m_variable_size - 1; ++i)
		{
			tmp = trasX[i] * trasX[i] + trasX[i + 1] * trasX[i + 1];
			obj[0] += pow(tmp, 0.25) * (pow(sin(50 * pow(tmp, 0.1)), 2.) + 1.);
		}
		obj[0] = pow(obj[0] / (real)(m_variable_size - 1), 2.);
		obj[0] += fOpt;
	}

	void BBOB::f18_illconditioned_schaffers_F7(real *x, std::vector<real>& obj) {

		size_t i, j;

		real tmp;

		/* BOUNDARY HANDLING*/
		real fPen = penalize(x), fOpt = m_bias;
		fOpt += 10. * fPen;

		std::vector<real> trasX(m_variable_size), tmpvect(m_variable_size);
		/* TRANSFORMATION IN SEARCH SPACE*/
		for (i = 0; i < m_variable_size; ++i)
		{
			tmpvect[i] = 0.;
			for (j = 0; j < m_variable_size; ++j)
				tmpvect[i] += m_rot[i][j] * (x[j] - m_optima.variable(0)[j]);
		}
		asyemmetricalize(tmpvect.data(), m_beta);

		for (i = 0; i < m_variable_size; ++i)
		{
			trasX[i] = 0.;
			tmp = pow(sqrt(m_condition_number), ((real)i) / ((real)(m_variable_size - 1)));
			for (j = 0; j < m_variable_size; ++j)
			{
				trasX[i] += tmp * m_rot2[i][j] * tmpvect[j];
			}
		}

		/* COMPUTATION core*/
		obj[0] = 0.;
		for (i = 0; i < m_variable_size - 1; ++i)
		{
			tmp = trasX[i] * trasX[i] + trasX[i + 1] * trasX[i + 1];
			obj[0] += pow(tmp, 0.25) * (pow(sin(50. * pow(tmp, 0.1)), 2.) + 1.);
		}
		obj[0] = pow(obj[0] / (real)(m_variable_size - 1), 2.);
		obj[0] += fOpt;
	}


	void BBOB::f19_composite_griewank_rosenbrock(real *x, std::vector<real>& obj) {

		size_t i, j;

		real F2, tmp = 0., tmp2;

		std::vector<real> trasX(m_variable_size);
		/* TRANSFORMATION IN SEARCH SPACE*/
		for (i = 0; i < m_variable_size; ++i) {
			trasX[i] = 0.5;
			for (j = 0; j < m_variable_size; ++j) {
				trasX[i] += m_linearTF[i][j] * x[j];
			}
		}
		/* COMPUTATION core*/
		for (i = 0; i < m_variable_size - 1; ++i)
		{
			tmp2 = trasX[i] * trasX[i] - trasX[i + 1];
			F2 = 100. * tmp2 * tmp2;
			tmp2 = 1 - trasX[i];
			F2 += tmp2 * tmp2;
			tmp += F2 / 4000. - cos(F2);
		}
		obj[0] = 10. + 10. * tmp / (real)(m_variable_size - 1);
		obj[0] += m_bias;
	}


	void BBOB::f20_schwefel(real *x, std::vector<real>& obj) {

		size_t i;
		real tmp;
		std::vector<real> trasX(m_variable_size), tmpvect(m_variable_size);

		/* TRANSFORMATION IN SEARCH SPACE*/
		for (i = 0; i < m_variable_size; ++i)
		{
			tmpvect[i] = 2. * x[i];
			if (m_optima.variable(0)[i] < 0.)
				tmpvect[i] *= -1.;
		}

		trasX[0] = tmpvect[0];
		for (i = 1; i < m_variable_size; ++i)
		{
			trasX[i] = tmpvect[i] + 0.25 * (tmpvect[i - 1] - 2. * fabs(m_optima.variable(0)[i - 1]));
		}

		for (i = 0; i < m_variable_size; ++i)
		{
			trasX[i] -= 2 * fabs(m_optima.variable(0)[i]);
			trasX[i] *= pow(sqrt(m_condition_number), ((real)i) / ((real)(m_variable_size - 1)));
			trasX[i] = 100. * (trasX[i] + 2 * fabs(m_optima.variable(0)[i]));
		}

		real fPen = 0, fOpt = m_bias;
		/* BOUNDARY HANDLING*/
		for (i = 0; i < m_variable_size; ++i)
		{
			tmp = fabs(trasX[i]) - 500.;
			if (tmp > 0.)
			{
				fPen += tmp * tmp;
			}
		}

		fOpt += 0.01 * fPen;
		/* COMPUTATION core*/
		obj[0] = 0.;
		for (i = 0; i < m_variable_size; ++i)
		{
			obj[0] += trasX[i] * sin(sqrt(fabs(trasX[i])));
		}
		obj[0] = 0.01 * ((418.9828872724339) - obj[0] / (real)m_variable_size);
		obj[0] += fOpt;
	}

	void BBOB::f21_gallagher_gaussian101me_peaks(real *x, std::vector<real>& obj) {
		size_t i, j;
		real a = 0.1;
		real tmp2, f = 0., tmp;
		real fac = -0.5 / (real)m_variable_size;

		real fAdd = penalize(x);

		/* BOUNDARY HANDLING*/

		m_bias = 1e4 * fAdd + m_optima.objective()[0];

		std::vector<real> trasX(m_variable_size);
		/* TRANSFORMATION IN SEARCH SPACE*/
		for (i = 0; i < m_variable_size; ++i)
		{
			trasX[i] = 0.;
			for (j = 0; j < m_variable_size; ++j)
			{
				trasX[i] += m_rot[i][j] * x[j];
			}
		}

		/* COMPUTATION core*/
		for (i = 0; i <NHIGHPEAKS21; ++i)
		{
			tmp2 = 0.;
			for (j = 0; j < m_variable_size; ++j)
			{
				tmp = (trasX[j] - m_Xlocal[j][i]);
				tmp2 += m_arrScales[i][j] * tmp * tmp;
			}
			tmp2 = m_peak_values[i] * exp(fac * tmp2);
			f = fmax(f, tmp2);
		}

		f = 10. - f;
		if (f > 0)
		{
			obj[0] = log(f) / a;
			obj[0] = pow(exp(obj[0] + 0.49*(sin(obj[0]) + sin(0.79*obj[0]))), a);
		}
		else if (f < 0)
		{
			obj[0] = log(-f) / a;
			obj[0] = -pow(exp(obj[0] + 0.49*(sin(0.55 * obj[0]) + sin(0.31*obj[0]))), a);
		}
		else
			obj[0] = f;

		obj[0] *= obj[0];
		obj[0] += m_bias;
	}


	void BBOB::f22_gallagher_gaussian21hi_peaks(real *x, std::vector<real>& obj) {

		size_t i, j;
		real a = 0.1;
		real tmp2, f = 0., tmp;
		real fac = -0.5 / (real)m_variable_size;

		/* BOUNDARY HANDLING*/
		real fPen = penalize(x), fOpt = m_bias;
		fOpt += fPen;

		std::vector<real> trasX(m_variable_size);
		/* TRANSFORMATION IN SEARCH SPACE*/
		for (i = 0; i < m_variable_size; ++i)
		{
			trasX[i] = 0.;
			for (j = 0; j < m_variable_size; ++j)
			{
				trasX[i] += m_rot[i][j] * x[j];
			}
		}

		/* COMPUTATION core*/
		for (i = 0; i < NHIGHPEAKS22; ++i)
		{
			tmp2 = 0.;
			for (j = 0; j < m_variable_size; ++j)
			{
				tmp = (trasX[j] - m_Xlocal[j][i]);
				tmp2 += m_arrScales[i][j] * tmp * tmp;
			}
			tmp2 = m_peak_values[i] * exp(fac * tmp2);
			f = fmax(f, tmp2);
		}

		f = 10. - f;
		if (f > 0)
		{
			obj[0] = log(f) / a;
			obj[0] = pow(exp(obj[0] + 0.49*(sin(obj[0]) + sin(0.79*obj[0]))), a);
		}
		else if (f < 0)
		{
			obj[0] = log(-f) / a;
			obj[0] = -pow(exp(obj[0] + 0.49*(sin(0.55 * obj[0]) + sin(0.31*obj[0]))), a);
		}
		else
			obj[0] = f;

		obj[0] *= obj[0];
		obj[0] += fOpt;
	}


	void BBOB::f23_katsuura(real *x, std::vector<real>& obj) {

		size_t i, j;

		real tmp, arr, prod = 1., tmp2;

		/* BOUNDARY HANDLING*/
		real fPen = penalize(x), fAdd = m_bias;
		fAdd += fPen;

		std::vector<real> trasX(m_variable_size), tmpvect(m_variable_size);
		/* TRANSFORMATION IN SEARCH SPACE*/
		/* write rotated difference std::vector into tmx*/
		for (j = 0; j < m_variable_size; ++j)  /* store difference std::vector*/
			tmpvect[j] = x[j] - m_optima.variable(0)[j];
		real *ptmx, *plinTF, *ptmp;
		for (i = 0; i < m_variable_size; ++i) {
			trasX[i] = 0.;
			ptmx = &trasX[i];
			plinTF = m_linearTF[i].vect().data();
			ptmp = tmpvect.data();
			for (j = 0; j < m_variable_size; ++j) {
				*ptmx += *plinTF++ * *ptmp++;
			}
		}

		/* COMPUTATION core*/
		for (i = 0; i < m_variable_size; ++i)
		{
			tmp = 0.;
			for (j = 1; j < 33; ++j)
			{
				tmp2 = pow(2., (real)j);
				arr = trasX[i] * tmp2;
				tmp += fabs(arr - round(arr)) / tmp2;
			}
			tmp = 1. + tmp * (real)(i + 1);
			prod *= tmp;
		}
		obj[0] = 10. / (real)m_variable_size / (real)m_variable_size * (-1. + pow(prod, 10. / pow((real)m_variable_size, 1.2)));
		obj[0] += fAdd;
	}


	void BBOB::f24_lunacekbi_rastrigin(real *x, std::vector<real>& obj) {
		size_t i, j;

		real tmp, tmp2 = 0., tmp3 = 0., tmp4 = 0.;
		real s = 1. - 0.5 / (sqrt((real)(m_variable_size + 20)) - 4.1);
		real d = 1.;
		real mu2 = -sqrt((m_mu * m_mu - d) / s);


		/* BOUNDARY HANDLING*/
		real fPen = penalize(x);
		real fAdd = m_optima.objective()[0] + 1e4 * fPen;
		std::vector<real> trasX(m_variable_size);

		/* TRANSFORMATION IN SEARCH SPACE*/
		for (i = 0; i < m_variable_size; ++i)
		{
			trasX[i] = 2. * x[i];
			if (m_optima.variable(0)[i] < 0.)
				trasX[i] *= -1.;
		}

		/* COMPUTATION core*/
		tmp = 0.;
		for (i = 0; i < m_variable_size; ++i)
		{
			tmp2 += (trasX[i] - m_mu) * (trasX[i] - m_mu);
			tmp3 += (trasX[i] - mu2) * (trasX[i] - mu2);
			tmp4 = 0.;
			for (j = 0; j < m_variable_size; ++j)
			{
				tmp4 += m_linearTF[i][j] * (trasX[j] - m_mu);
			}
			tmp += cos(2 * OFEC_PI * tmp4);
		}
		obj[0] = fmin(tmp2, d * (real)m_variable_size + s * tmp3) + 10. * ((real)m_variable_size - tmp);
		obj[0] += fAdd;
	}
}

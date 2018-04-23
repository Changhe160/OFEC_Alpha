#include "MOEA_F.h"
#include <fstream>

namespace OFEC {
	MOEA_FBase::MOEA_FBase(const std::string & name, size_t size_var, size_t size_obj) : problem(name, size_var, size_obj), continuous(name, size_var, size_obj) {
		set_init_range(0., 1.);
		set_range(0., 1.);
		for (size_t idx = 0; idx < m_opt_mode.size(); ++idx)
			m_opt_mode[idx] = optimization_mode::Minimization;
	}
	void MOEA_FBase::alphafunction(double alpha[], double const * x, int dim, int type) {
		if (dim == 2)
		{
			if (type == 21) {
				alpha[0] = x[0];
				alpha[1] = 1 - sqrt(x[0]);
			}

			if (type == 22) {
				alpha[0] = x[0];
				alpha[1] = 1 - x[0] * x[0];
			}

			if (type == 23) {
				alpha[0] = x[0];
				alpha[1] = 1 - sqrt(alpha[0]) - alpha[0] * sin(10 * alpha[0] * alpha[0] * OFEC_PI);
			}

			if (type == 24) {
				alpha[0] = x[0];
				alpha[1] = 1 - x[0] - 0.05*sin(4 * OFEC_PI*x[0]);
			}

		}
		else
		{

			if (type == 31) {
				alpha[0] = cos(x[0] * OFEC_PI / 2)*cos(x[1] * OFEC_PI / 2);
				alpha[1] = cos(x[0] * OFEC_PI / 2)*sin(x[1] * OFEC_PI / 2);
				alpha[2] = sin(x[0] * OFEC_PI / 2);
			}

			if (type == 32) {
				alpha[0] = 1 - cos(x[0] * OFEC_PI / 2)*cos(x[1] * OFEC_PI / 2);
				alpha[1] = 1 - cos(x[0] * OFEC_PI / 2)*sin(x[1] * OFEC_PI / 2);
				alpha[2] = 1 - sin(x[0] * OFEC_PI / 2);
			}

			if (type == 33) {
				alpha[0] = x[0];
				alpha[1] = x[1];
				alpha[2] = 3 - (sin(3 * OFEC_PI*x[0]) + sin(3 * OFEC_PI*x[1])) - 2 * (x[0] + x[1]);
			}

			if (type == 34) {
				alpha[0] = x[0] * x[1];
				alpha[1] = x[0] * (1 - x[1]);
				alpha[2] = (1 - x[0]);
			}
		}
	}
	double MOEA_FBase::betafunction(const std::vector<double>& x, int type) {
		double beta;
		size_t dim = x.size();

		if (dim == 0) {
			// a bug here when dim=0
			//beta = 0;
			return 0;
		}

		if (type == 1) {
			beta = 0;
			for (size_t i = 0; i<dim; i++) {
				beta += x[i] * x[i];
			}
			beta = 2.0*beta / dim;
		}

		if (type == 2) {
			beta = 0;
			for (size_t i = 0; i<dim; i++) {
				beta += sqrt(i + 1)*x[i] * x[i];
			}
			beta = 2.0*beta / dim;
		}

		if (type == 3) {
			double sum = 0, xx;
			for (size_t i = 0; i<dim; i++) {
				xx = 2 * x[i];
				sum += (xx*xx - cos(4 * OFEC_PI*xx) + 1);
			}
			beta = 2.0*sum / dim;
		}

		if (type == 4) {
			double sum = 0, prod = 1, xx;
			for (size_t i = 0; i<dim; i++) {
				xx = 2 * x[i];
				sum += xx*xx;
				prod *= cos(10 * OFEC_PI*xx / sqrt(i + 1));
			}
			beta = 2.0*(sum - 2 * prod + 2) / dim;
		}

		return beta;
	}
	double MOEA_FBase::psfunc2(const double & x, const double & t1, size_t dim, int type, int css) {
		// type:  the type of curve 
		// css:   the class of index
		double beta;
		size_t numDim = global::ms_global->m_problem->variable_size();

		dim++;

		if (type == 21) {
			double xy = 2 * (x - 0.5);
			// a bug here when numDim=2
			if (numDim == 2) beta = xy - pow(t1, 2.0);
			else	beta = xy - pow(t1, 0.5*(numDim + 3 * dim - 8) / (numDim - 2));

		}

		if (type == 22) {
			double theta = 6 * OFEC_PI*t1 + dim*OFEC_PI / numDim;
			double xy = 2 * (x - 0.5);
			beta = xy - sin(theta);
		}

		if (type == 23) {
			double theta = 6 * OFEC_PI*t1 + dim*OFEC_PI / numDim;
			double ra = 0.8*t1;
			double xy = 2 * (x - 0.5);
			if (css == 1)
				beta = xy - ra*cos(theta);
			else {
				beta = xy - ra*sin(theta);
			}
		}

		if (type == 24) {
			double theta = 6 * OFEC_PI*t1 + dim*OFEC_PI / numDim;
			double xy = 2 * (x - 0.5);
			double ra = 0.8*t1;
			if (css == 1)
				beta = xy - ra*cos(theta / 3);
			else {
				beta = xy - ra*sin(theta);
			}
		}

		if (type == 25) {
			double rho = 0.8;
			double phi = OFEC_PI*t1;
			double theta = 6 * OFEC_PI*t1 + dim*OFEC_PI / numDim;
			double xy = 2 * (x - 0.5);
			if (css == 1)
				beta = xy - rho*sin(phi)*sin(theta);
			else if (css == 2)
				beta = xy - rho*sin(phi)*cos(theta);
			else
				beta = xy - rho*cos(phi);
		}

		if (type == 26) {
			double theta = 6 * OFEC_PI*t1 + dim*OFEC_PI / numDim;
			double ra = 0.3*t1*(t1*cos(4 * theta) + 2);
			double xy = 2 * (x - 0.5);
			if (css == 1)
				beta = xy - ra*cos(theta);
			else {
				beta = xy - ra*sin(theta);
			}
		}

		return beta;
	}
	double MOEA_FBase::psfunc3(const double & x, const double & t1, const double & t2, int dim, int type) {
		// type:  the type of curve 
		// css:   the class of index
		double beta;
		size_t numDim = global::ms_global->m_problem->variable_size();
		dim++;

		if (type == 31) {
			double xy = 4 * (x - 0.5);
			double rate = 1.0*dim / numDim;
			beta = xy - 4 * (t1*t1*rate + t2*(1.0 - rate)) + 2;
		}

		if (type == 32) {
			double theta = 2 * OFEC_PI*t1 + dim*OFEC_PI / numDim;
			double xy = 4 * (x - 0.5);
			beta = xy - 2 * t2*sin(theta);
		}

		return beta;
	}
	void MOEA_FBase::calObjective(double * x, std::vector<double>& y_obj) {
		// 2-objective case
		double *x_var = x;
		size_t nobj = global::ms_global->m_problem->objective_size();
		size_t nDim = global::ms_global->m_problem->variable_size();
		if (nobj == 2)
		{
			if (m_ltype == 21 || m_ltype == 22 || m_ltype == 23 || m_ltype == 24 || m_ltype == 26)
			{
				double g = 0, h = 0, a, b;
				std::vector <double> aa;
				std::vector <double> bb;
				for (size_t n = 1; n<nDim; n++)
				{

					if (n % 2 == 0) {
						a = psfunc2(x_var[n], x_var[0], n, m_ltype, 1);  // linkage
						aa.push_back(a);
					}
					else
					{
						b = psfunc2(x_var[n], x_var[0], n, m_ltype, 2);
						bb.push_back(b);
					}

				}

				g = betafunction(aa, m_dtype);
				h = betafunction(bb, m_dtype);

				double alpha[2];
				alphafunction(alpha, x_var, 2, m_ptype);  // shape function
				y_obj[0] = alpha[0] + h;
				y_obj[1] = alpha[1] + g;
				aa.clear();
				bb.clear();
			}

			if (m_ltype == 25)
			{
				double g = 0, h = 0, a, b;
				double e = 0, c;
				std::vector <double> aa;
				std::vector <double> bb;
				for (size_t n = 1; n<nDim; n++) {
					if (n % 3 == 0) {
						a = psfunc2(x_var[n], x_var[0], n, m_ltype, 1);
						aa.push_back(a);
					}
					else if (n % 3 == 1)
					{
						b = psfunc2(x_var[n], x_var[0], n, m_ltype, 2);
						bb.push_back(b);
					}
					else {
						c = psfunc2(x_var[n], x_var[0], n, m_ltype, 3);
						if (n % 2 == 0)    aa.push_back(c);
						else          bb.push_back(c);
					}
				}
				g = betafunction(aa, m_dtype);          // distance function
				h = betafunction(bb, m_dtype);
				double alpha[2];
				alphafunction(alpha, x_var, 2, m_ptype);  // shape function
				y_obj[0] = alpha[0] + h;
				y_obj[1] = alpha[1] + g;
				aa.clear();
				bb.clear();
			}
		}


		// 3-objective case
		if (nobj == 3)
		{
			if (m_ltype == 31 || m_ltype == 32)
			{
				double g = 0, h = 0, e = 0, a;
				std::vector <double> aa;
				std::vector <double> bb;
				std::vector <double> cc;
				for (int n = 2; n<nDim; n++)
				{
					a = psfunc3(x_var[n], x_var[0], x_var[1], n, m_ltype);
					if (n % 3 == 0)	    aa.push_back(a);
					else if (n % 3 == 1)	bb.push_back(a);
					else            cc.push_back(a);
				}

				g = betafunction(aa, m_dtype);
				h = betafunction(bb, m_dtype);
				e = betafunction(cc, m_dtype);

				double alpha[3];
				alphafunction(alpha, x_var, 3, m_ptype);  // shape function
				y_obj[0] = alpha[0] + h;
				y_obj[1] = alpha[1] + g;
				y_obj[2] = alpha[2] + e;
				aa.clear();
				bb.clear();
				cc.clear();
			}
		}
	}
	void MOEA_FBase::LoadPF() {
		std::ifstream infile;
		std::stringstream os;
		os << "./instance/problem/continuous/multi_objective/MOEA_F/data/pf_P" << m_ptype << "D" << m_dtype << "L" << m_ltype << ".dat";
		infile.open(os.str());
		if (!infile)
			return;
		int lines = 0;
		std::string str;
		while (getline(infile, str))
			++lines;
		m_optima.resize_objective_set(lines);
		infile.close();
		infile.clear();
		infile.open(os.str());
		std::vector<real> temp_obj(m_objective_size);
		for (size_t i = 0; i < lines; i++) {
			for (size_t j = 0; j < m_objective_size; j++)
				infile >> temp_obj[j];
			m_optima.set_objective(temp_obj, i);
		}
		infile.close();
	}
}
// ------------------------------------------------------------------------ //
// This source file is part of the 'ESA Advanced Concepts Team's			//
// Space Mechanics Toolbox' software.                                       //
//                                                                          //
// The source files are for research use only,                              //
// and are distributed WITHOUT ANY WARRANTY. Use them on your own risk.     //
//                                                                          //
// Copyright (c) 2004-2007 European Space Agency                            //
// ------------------------------------------------------------------------ //


#include <iostream>
#include <iomanip>
#include "Astro_Functions.h"

namespace gtop_toolbox {

	class CZF : public ZeroFinder::Function1D
	{
	public:
		double operator()(double x)
		{
			return p1 * tan(x) - log(tan(0.5 * x + 0.25 * M_PI)) - p2;
		}
	};


	double Mean2Eccentric(const double M, const double e)
	{

		double tolerance = 1e-13;
		int n_of_it = 0; // Number of iterations
		double Eccentric, Ecc_New;
		double err = 1.0;



		if (e < 1.0)
		{
			Eccentric = M + e * cos(M); // Initial guess
			while ((err > tolerance) && (n_of_it < 100))
			{
				Ecc_New = Eccentric - (Eccentric - e * sin(Eccentric) - M) / (1.0 - e * cos(Eccentric));
				err = fabs(Eccentric - Ecc_New);
				Eccentric = Ecc_New;
				n_of_it++;
			}
		}
		else
		{
			CZF FF;  // function to find its zero point
			ZeroFinder::FZero fz(-0.5 * 3.14159265358979 + 1e-8, 0.5 * 3.14159265358979 - 1e-8);
			FF.SetParameters(e, M);
			Ecc_New = fz.FindZero(FF);
			Eccentric = Ecc_New;
		}
		return Eccentric;
	}



	void Conversion(const double *E,
		double *pos,
		double *vel,
		const double mu)
	{
		double a, e, i, omg, omp, theta;
		double b, n;
		double X_per[3], X_dotper[3];
		double R[3][3];

		a = E[0];
		e = E[1];
		i = E[2];
		omg = E[3];
		omp = E[4];
		theta = E[5];

		b = a * sqrt(1 - e * e);
		n = sqrt(mu / pow(a, 3));

		const double sin_theta = sin(theta);
		const double cos_theta = cos(theta);

		X_per[0] = a * (cos_theta - e);
		X_per[1] = b * sin_theta;

		X_dotper[0] = -(a * n * sin_theta) / (1 - e * cos_theta);
		X_dotper[1] = (b * n * cos_theta) / (1 - e * cos_theta);

		const double cosomg = cos(omg);
		const double cosomp = cos(omp);
		const double sinomg = sin(omg);
		const double sinomp = sin(omp);
		const double cosi = cos(i);
		const double sini = sin(i);

		R[0][0] = cosomg * cosomp - sinomg * sinomp * cosi;
		R[0][1] = -cosomg * sinomp - sinomg * cosomp * cosi;

		R[1][0] = sinomg * cosomp + cosomg * sinomp * cosi;
		R[1][1] = -sinomg * sinomp + cosomg * cosomp * cosi;

		R[2][0] = sinomp * sini;
		R[2][1] = cosomp * sini;

		// evaluate position and velocity
		for (int i = 0; i < 3; i++)
		{
			pos[i] = 0;
			vel[i] = 0;
			for (int j = 0; j < 2; j++)
			{
				pos[i] += R[i][j] * X_per[j];
				vel[i] += R[i][j] * X_dotper[j];
			}
		}
		return;
	}


	double norm(const double *vet1, const double *vet2)
	{
		double Vin = 0;
		for (int i = 0; i < 3; i++)
		{
			Vin += (vet1[i] - vet2[i]) * (vet1[i] - vet2[i]);
		}
		return sqrt(Vin);
	}


	double norm2(const double *vet1)
	{
		double temp = 0.0;
		for (int i = 0; i < 3; i++) {
			temp += vet1[i] * vet1[i];
		}
		return sqrt(temp);
	}


	//subfunction that evaluates vector product
	void vett(const double *vet1, const double *vet2, double *prod)
	{
		prod[0] = (vet1[1] * vet2[2] - vet1[2] * vet2[1]);
		prod[1] = (vet1[2] * vet2[0] - vet1[0] * vet2[2]);
		prod[2] = (vet1[0] * vet2[1] - vet1[1] * vet2[0]);
	}

	double asinh(double x) { return log(x + sqrt(x * x + 1)); };

	double acosh(double x) { return log(x + sqrt(x * x - 1)); };

	double x2tof(const double &x, const double &s, const double &c, const int lw)
	{
		double am, a, alfa, beta;

		am = s / 2;
		a = am / (1 - x * x);
		if (x < 1)//ellpise
		{
			beta = 2 * asin(sqrt((s - c) / (2 * a)));
			if (lw) beta = -beta;
			alfa = 2 * acos(x);
		}
		else
		{
			alfa = 2 * acosh(x);
			beta = 2 * asinh(sqrt((s - c) / (-2 * a)));
			if (lw) beta = -beta;
		}

		if (a > 0)
		{
			return (a * sqrt(a) * ((alfa - sin(alfa)) - (beta - sin(beta))));
		}
		else
		{
			return (-a * sqrt(-a) * ((sinh(alfa) - alfa) - (sinh(beta) - beta)));
		}

	}


	// Subfunction that evaluates the time of flight as a function of x
	double tofabn(const double &sigma, const double &alfa, const double &beta)
	{
		if (sigma > 0)
		{
			return (sigma * sqrt(sigma) * ((alfa - sin(alfa)) - (beta - sin(beta))));
		}
		else
		{
			return (-sigma * sqrt(-sigma) * ((sinh(alfa) - alfa) - (sinh(beta) - beta)));
		}
	}

	// subfunction that evaluates unit vectors
	void vers(const double *V_in, double *Ver_out)
	{
		double v_mod = 0;
		int i;

		for (i = 0; i < 3; i++)
		{
			v_mod += V_in[i] * V_in[i];
		}

		double sqrtv_mod = sqrt(v_mod);

		for (i = 0; i < 3; i++)
		{
			Ver_out[i] = V_in[i] / sqrtv_mod;
		}
	}

}



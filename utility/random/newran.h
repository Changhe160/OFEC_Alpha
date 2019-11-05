/******************************************************************************
* Project:Open Frameworks for Evolutionary Computation (OFEC)
*******************************************************************************
* Author: Changhe Li & Hai Xia
* Email: changhe.lw@gmail.com
* Language: C++
* Last-modified date: 2019.08.16
*-------------------------------------------------------------------------------
*  This file is part of OFEC. This library is free software;
*  you can redistribute it and/or modify it under the terms of the
*  GNU General Public License as published by the Free Software
*  Foundation; either version 2, or (at your option) any later version.
*
*  see https://github.com/Changhe160/OFEC for more information
*
*-------------------------------------------------------------------------------
* Generators of different types of random numbers derived from NewRan Lib at
* http ://www.robertnz.net/nr02doc.htm
*
*********************************************************************************/


#ifndef NEWRAN_LIB_H
#define NEWRAN_LIB_H 0



#include "../myexcept.h"
#include "extreal.h"

#include <utility>

namespace OFEC {

	typedef double(*PDF)(double);                // probability density function
	double ln_gamma(double);                      // log gamma function

	//**************** uniform random number generator **********************

	class random                              // uniform random number generator
	{
	protected:
		double m_seed;                    // seed
		double m_motherseed;            // must be (0,1)
		double m_buffer[128];               // for mixing random numbers
		double raw();                     // unmixed random numbers
		const char * m_name;
	public:
		random(double s);             // set seed (0 < seed < 1)
		double get();                   // get seed
		virtual double next();                   // get new value
		const char* name() { return m_name; }                  // identification
		virtual double density(double) const;      // used by positive & Asymgen
		random() { m_name = "random"; }                            // do nothing
		virtual ~random() {}                   // make destructors virtual
		virtual ext_real mean() const { return 0.5; }
		// mean of distribution
		virtual ext_real variance() const { return 1.0 / 12.0; }
		// variance of distribution
		virtual void tDelete() {}              // delete components of sum
		virtual int nelems() const { return 1; }

		random& operator=(const random&) = delete;
	};


	//****************** uniform random number generator *********************

	class uniform : public random {
	public:
		uniform(double s) :random(s) { m_name = "uniform"; }                           // set value
		double next() { return random::next(); }
		ext_real mean() const { return 0.5; }
		ext_real variance() const { return 1.0 / 12.0; }
		double density(double x) const { return (x < 0.0 || x > 1.0) ? 0 : 1.0; }
		template<typename T>
		T next_non_standard(T min, T max) { // get a value within [min,max)
			return min + static_cast<T>((max - min)*next());
		}
		template<typename T>
		void shuffle(T first, T last) { //iterator
			for (auto i = last - first - 1; i > 0; --i) {
				auto t = decltype(i) (i + 1)*next();
				std::swap(first[i], first[t]);
			}
		}
	};


	//************************* return constant ******************************

	class constant : public random {
		double m_value;                            // value to be returned
	public:
		constant(double v) {
			m_value = v;
			m_name = "constant";
		}          // set value
		double next() { return m_value; }
		ext_real mean() const { return m_value; }
		ext_real variance() const { return 0.0; }
	};

	//**************** positive random number generator **********************

	class positive : public random              // generate positive rv
	{
		void operator=(const positive&) {}       // private so can't access

	protected:
		double m_xi, *m_sx, *m_sfx;
		bool m_not_ready;
		void build(bool);                      // called on first call to next

	public:
		positive(double s);           // constructor
		~positive();                             // destructor
		double next();                           // to get a single new value
		ext_real mean() const { return (ext_real)EXT_REAL_CODE::Missing; }
		ext_real variance() const { return (ext_real)EXT_REAL_CODE::Missing; }
	};

	//**************** symmetric random number generator **********************

	class symmetric : public positive              // generate symmetric rv
	{
	public:
		symmetric(double s) :positive(s) { m_name = "symmetric"; };
		double next();                           // to get a single new value
	};

	//**************** normal random number generator **********************

	class normal : public symmetric              // generate standard normal rv
	{
		double Nxi, *Nsx, *Nsfx;          // so we need initialise only once
		long count;                     // assume initialised to 0

	public:
		normal(double s);
		~normal();
		double density(double) const;              // normal density function
		ext_real mean() const { return 0.0; }
		ext_real variance() const { return 1.0; }
		double  next_non_standard(double rmean, double rvariance);
	};

	//**************** cauchy random number generator **********************

	class cauchy : public symmetric              // generate standard cauchy rv
	{
	public:
		cauchy(double s) :symmetric(s) { m_name = "cauchy"; };
		double density(double) const;              // cauchy density function
		ext_real mean() const { return EXT_REAL_CODE::Indefinite; }
		ext_real variance() const { return EXT_REAL_CODE::PlusInfinity; }
		double next_non_standard(double rmean, double rvariance);
	};

	//**************** exponential random number generator **********************

	class exponential : public positive         // generate standard exponential rv
	{
	public:
		exponential(double s) :positive(s) { m_name = "exponential"; };
		double density(double) const;              // exponential density function
		ext_real mean() const { return 1.0; }
		ext_real variance() const { return 1.0; }
	};

	//**************** asymmetric random number generator **********************

	class asymmetric : public random             // generate asymmetric rv
	{
		double m_xi, *m_sx, *m_sfx; int ic;
		bool m_not_ready;
		void build();                          // called on first call to next

	protected:
		double m_modex;

	public:
		asymmetric(double, double s);                         // constructor (double=mode)
		~asymmetric();                            // destructor
		double next();                           // to get a single new value
		ext_real mean() const { return (ext_real)EXT_REAL_CODE::Missing; }
		ext_real variance() const { return (ext_real)EXT_REAL_CODE::Missing; }
	};

	class  levy : public asymmetric {
	public:
		double m_sc;
		levy(double, double s);
		double density(double) const;              // levy density function
		ext_real mean() const { return EXT_REAL_CODE::Indefinite; }
		ext_real variance() const { return EXT_REAL_CODE::Indefinite; }
	};

	//**************** gamma random number generator **********************

	class gamma : public random               // generate gamma rv
	{
		random* m_method;

		//********** gamma random number generator, alpha <= 1 *****************

		class gamma1 : public positive              // generate gamma rv
												  // shape parameter <= 1
		{
			double ln_gam, ralpha, alpha;
		public:
			gamma1(double, double s);                          // constructor (double=shape)
			double density(double) const;              // gamma density function
			double next();                           // carries out power transform
			ext_real mean() const { return alpha; }
			ext_real variance() const { return alpha; }
		};

		//********** gamma random number generator, alpha > 1 ******************

		class gamma2 : public asymmetric             // generate gamma rv
												  // shape parameter > 1
		{
			double alpha, ln_gam;
		public:
			gamma2(double, double s);                          // constructor (double=shape)
			double density(double) const;              // gamma density function
			ext_real mean() const { return alpha; }
			ext_real variance() const { return alpha; }
		};

	public:
		gamma(double, double s);                           // constructor (double=shape)
		~gamma() { delete m_method; }
		double next() { return m_method->next(); }
		ext_real mean() const { return m_method->mean(); }
		ext_real variance() const { return m_method->variance(); }
		void shape(double);
	};

}

#endif

// body file: newran.cpp




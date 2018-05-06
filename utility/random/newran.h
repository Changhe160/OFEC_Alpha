/******************************************************************************
* Project:Open Frameworks for Evolutionary Computation (OFEC)
*******************************************************************************
* Author: Changhe Li
* Email: changhe.lw@gmail.com
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

typedef real (*PDF)(real);                // probability density function
real ln_gamma(real);                      // log gamma function

//**************** uniform random number generator **********************

class random                              // uniform random number generator
{
protected:
   double m_seed;                    // seed
   double m_motherseed;            // must be (0,1)
   real m_buffer[128];               // for mixing random numbers
   real raw();                     // unmixed random numbers
   const char * m_name;
public:
   random(double s);             // set seed (0 < seed < 1)
   double get();                   // get seed
   virtual real next();                   // get new value
	const char* name() { return m_name; }                  // identification
   virtual real density(real) const;      // used by positive & Asymgen
   random() { m_name = "random"; }                            // do nothing
   virtual ~random() {}                   // make destructors virtual
   virtual ext_real mean() const { return 0.5; }
                                          // mean of distribution
   virtual ext_real variance() const { return 1.0/12.0; }
					  // variance of distribution
   virtual void tDelete() {}              // delete components of sum
   virtual int nelems() const { return 1; }

   random& operator=(const random&) = delete;
};


//****************** uniform random number generator *********************

class uniform : public random
{
public:
   uniform(double s):random(s){ 	   m_name = "uniform";    }                           // set value
   real next() {  		return random::next();    }
   ext_real mean() const { return 0.5; }
   ext_real variance() const { return 1.0/12.0; }
   real density(real x) const { return (x < 0.0 || x > 1.0) ? 0 : 1.0; }
   template<typename T>
   T next_non_standard(T min, T max) { // get a value within [min,max)
	   return min + static_cast<T>((max - min)*next());
   }
   template<typename T>
   void shuffle(T first, T last) { //iterator
	   for (auto i = last - first - 1; i >0; --i) {		 
		  auto t =  decltype(i) (i+1)*next();		 
		  std::swap(first[i], first[t]);
	   }
   }
};


//************************* return constant ******************************

class constant : public random
{
	real m_value;                            // value to be returned
public:
	constant(real v) { 
		m_value=v; 
		m_name = "constant";
	}          // set value
	real next() {  return m_value; }
	ext_real mean() const { return m_value; }
	ext_real variance() const { return 0.0; }
};

//**************** positive random number generator **********************

class positive : public random              // generate positive rv
{
   void operator=(const positive&) {}       // private so can't access

protected:
   real m_xi, *m_sx, *m_sfx;
   bool m_not_ready;
   void build(bool);                      // called on first call to next

public:
   positive(double s);           // constructor
   ~positive();                             // destructor
   real next();                           // to get a single new value
   ext_real mean() const { return (ext_real)EXT_REAL_CODE::Missing; }
   ext_real variance() const { return (ext_real)EXT_REAL_CODE::Missing; }
};

//**************** symmetric random number generator **********************

class symmetric : public positive              // generate symmetric rv
{
public:
	symmetric(double s) :positive(s) { m_name = "symmetric"; };
   real next();                           // to get a single new value
};

//**************** normal random number generator **********************

class normal : public symmetric              // generate standard normal rv
{
   real Nxi, *Nsx, *Nsfx;          // so we need initialise only once
   long count;                     // assume initialised to 0

public:
   normal(double s);
   ~normal();
   real density(real) const;              // normal density function
   ext_real mean() const { return 0.0; }
   ext_real variance() const { return 1.0; }
   real  next_non_standard(real rmean,real rvariance);
};

//**************** cauchy random number generator **********************

class cauchy : public symmetric              // generate standard cauchy rv
{
public:
	cauchy(double s) :symmetric(s) { m_name = "cauchy"; };
   real density(real) const;              // cauchy density function
   ext_real mean() const { return EXT_REAL_CODE::Indefinite; }
   ext_real variance() const { return EXT_REAL_CODE::PlusInfinity; }
   real next_non_standard(real rmean,real rvariance);
};

//**************** exponential random number generator **********************

class exponential : public positive         // generate standard exponential rv
{
public:
	exponential(double s):positive(s){ m_name = "exponential"; };
   real density(real) const;              // exponential density function
   ext_real mean() const { return 1.0; }
   ext_real variance() const { return 1.0; }
};

//**************** asymmetric random number generator **********************

class asymmetric : public random             // generate asymmetric rv
{
   real m_xi, *m_sx, *m_sfx; int ic;
   bool m_not_ready;
   void build();                          // called on first call to next

protected:
   real mode;

public:
   asymmetric(real,double s);                         // constructor (real=mode)
   ~asymmetric();                            // destructor
   real next();                           // to get a single new value
   ext_real mean() const { return (ext_real)EXT_REAL_CODE::Missing; }
   ext_real variance() const { return (ext_real)EXT_REAL_CODE::Missing; }
};

class  levy : public asymmetric{	
public:
	real m_sc;
   levy(real,double s);
   real density(real) const;              // levy density function
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
		real ln_gam, ralpha, alpha;
	public:
		gamma1(real, double s);                          // constructor (real=shape)
		real density(real) const;              // gamma density function
		real next();                           // carries out power transform
		ext_real mean() const { return alpha; }
		ext_real variance() const { return alpha; }
	};

	//********** gamma random number generator, alpha > 1 ******************

	class gamma2 : public asymmetric             // generate gamma rv
											  // shape parameter > 1
	{
		real alpha, ln_gam;
	public:
		gamma2(real, double s);                          // constructor (real=shape)
		real density(real) const;              // gamma density function
		ext_real mean() const { return alpha; }
		ext_real variance() const { return alpha; }
	};

public:
	gamma(real, double s);                           // constructor (real=shape)
	~gamma() { delete m_method; }
	real next() { return m_method->next(); }
	ext_real mean() const { return m_method->mean(); }
	ext_real variance() const { return m_method->variance(); }
	void shape(real);
};

}

#endif

// body file: newran.cpp




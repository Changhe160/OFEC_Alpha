#ifndef NEWRAN_LIB_H
#define NEWRAN_LIB_H 

//generators of random numbers derived from http://www.robertnz.net/nr02doc.htm

#include "../exception.h"
#include "extreal.h"

namespace OFEC {

typedef real (*PDF)(real);                // probability density function

real ln_gamma(real);                      // log gamma function

//**************** uniform random number generator **********************

class RepeatedRandom;
class SelectedRandom;

class random                              // uniform random number generator
{
   double m_seed;                    // seed  
   real m_buffer[128];               // for mixing random numbers
   real raw();                     // unmixed random numbers
   void operator=(const random&) {}       // private so can't access
protected:
   double m_mother_seed;            // must be (0,1)
   const char * m_name;
public:
	random(double s);
   double get();                   // get seed
   virtual real next();                   // get new value
   const char* name() { return m_name; }                  // identification
   virtual real density(real) const;     // used by PosGen & Asymgen
   random():m_name("Random") {}                            // do nothing
   virtual ~random() {}                   // make destructors virtual
   SelectedRandom& operator()(double);    // used by MixedRandom
   RepeatedRandom& operator()(int);       // for making combinations
   virtual ext_real Mean() const { return 0.5; }
                                          // mean of distribution
   virtual ext_real Variance() const { return 1.0/12.0; }
					  // variance of distribution
   virtual void tDelete() {}              // delete components of sum
   virtual int nelems() const { return 1; }
                                          // used by MixedRandom
   virtual void load(int*,real*,random**);
   friend class RandomPermutation;
};


//****************** uniform random number generator *********************

class Uniform : public random
{
public:
	Uniform() { m_name = "Uniform"; }                           // set value
   real next() { return random::next(); }
   ext_real Mean() const { return 0.5; }
   ext_real Variance() const { return 1.0/12.0; }
   real density(real x) const { return (x < 0.0 || x > 1.0) ? 0 : 1.0; }
};


//************************* return constant ******************************

class Constant : public random
{
   real m_value;                            // value to be returned

public:
   Constant(real v) :m_value(v) { m_name = "Constant"; }          // set value
//   real next();
   real next() { return m_value; }
   ext_real Mean() const { return m_value; }
   ext_real Variance() const { return 0.0; }
};

//**************** positive random number generator **********************

class PosGen : public random              // generate positive rv
{
protected:
   real xi, *sx, *sfx;
   bool NotReady;
   void Build(bool);                      // called on first call to next

public:
   PosGen();                              // constructor
   ~PosGen();                             // destructor
   real next();                           // to get a single new value
   ext_real Mean() const { return (ext_real)EXT_REAL_CODE::Missing; }
   ext_real Variance() const { return (ext_real)EXT_REAL_CODE::Missing; }
};

//**************** symmetric random number generator **********************

class SymGen : public PosGen              // generate symmetric rv
{
   void operator=(const SymGen&) {}       // private so can't access
public:
	SymGen() { m_name = "SymGen"; }
   real next();                           // to get a single new value
};

//**************** normal random number generator **********************

class Normal : public SymGen              // generate standard normal rv
{
   void operator=(const Normal&) {}       // private so can't access
   static real Nxi, *Nsx, *Nsfx;          // so we need initialise only once
   static long count;                     // assume initialised to 0

public:
   Normal();
   ~Normal();
   real density(real) const;              // normal density function
   ext_real Mean() const { return 0.0; }
   ext_real Variance() const { return 1.0; }
};

//*********** chi-square random number generator **********************

class ChiSq : public random               // generate non-central chi-sq rv
{
   void operator=(const ChiSq&) {}        // private so can't access
   random* c1;                            // pointers to generators
   random* c2;                            // pointers to generators
   int version;                           // indicates method of generation
   real mean, var;

public:
   ChiSq(int, real=0.0);                  // df and non-centrality parameter
   ~ChiSq();
   ext_real Mean() const { return mean; }
   ext_real Variance() const { return var; }
   real next();
};

//**************** Cauchy random number generator **********************

class Cauchy : public SymGen              // generate standard cauchy rv
{
   void operator=(const Cauchy&) {}       // private so can't access

public:
	Cauchy(){ m_name = "Cauchy"; }
   real density(real) const;              // Cauchy density function
   ext_real Mean() const { return (ext_real)EXT_REAL_CODE::Indefinite; }
   ext_real Variance() const { return (ext_real)EXT_REAL_CODE::PlusInfinity; }
};

//**************** Exponential random number generator **********************

class Exponential : public PosGen         // generate standard exponential rv
{
   void operator=(const Exponential&) {}  // private so can't access

public:
	Exponential() { m_name = "Exponential"; }
   real density(real) const;              // Exponential density function
   ext_real Mean() const { return 1.0; }
   ext_real Variance() const { return 1.0; }
};

//**************** asymmetric random number generator **********************

class AsymGen : public random             // generate asymmetric rv
{
   void operator=(const AsymGen&) {}      // private so can't access
   real xi, *sx, *sfx; int ic;
   bool NotReady;
   void Build();                          // called on first call to next

protected:
   real mode;

public:
  
   AsymGen(real);                         // constructor (real=mode)
   ~AsymGen();                            // destructor
   real next();                           // to get a single new value
   ext_real Mean() const { return (ext_real)EXT_REAL_CODE::Missing; }
   ext_real Variance() const { return (ext_real)EXT_REAL_CODE::Missing; }
};

//**************** Gamma random number generator **********************

class Gamma : public random               // generate gamma rv
{
   random* method;
public:
   Gamma(real);                           // constructor (real=shape)
   ~Gamma() { delete method; }
   real next() { return method->next(); }
   ext_real Mean() const { return method->Mean(); }
   ext_real Variance() const { return method->Variance(); }
};

//**************** Generators with pointers to pdf ***********************

class PosGenX : public PosGen
{
   PDF f;
public:
   PosGenX(PDF fx);
   real density(real x) const { return (*f)(x); }
};

class SymGenX : public SymGen
{
   PDF f;
public:
   SymGenX(PDF fx);
   real density(real x) const { return (*f)(x); }
};

class AsymGenX : public AsymGen
{
   PDF f;

public:
   AsymGenX(PDF fx, real mx);
   real density(real x) const { return (*f)(x); }
};

//***************** Pareto random number generator ************************

class Pareto : public random
// Use definition of Kotz and Johnson: "Continuous univariate distributions 1",
// chapter 19 with k = 1.
{
   real Shape, RS;
public:
   Pareto(real shape);                    // constructor (real=shape)
   ~Pareto() {}
   real next();
   ext_real Mean() const;
   ext_real Variance() const;
};


//**************** discrete random number generator **********************

class DiscreteGen : public random         // discrete random generator
{
   real *p; int *ialt; int n; real *val;
   void Gen(int, real*);                  // called by constructors
   real mean, var;                        // calculated by the constructor

public:
   DiscreteGen(int,real*);                // constructor
   DiscreteGen(int,real*,real*);          // constructor
   ~DiscreteGen();                        // destructor
   real next();                           // new single value
   ext_real Mean() const { return mean; }
   ext_real Variance() const { return var; }
};

//***************** Poisson random number generator *******************

class Poisson : public random             // generate poisson rv
{
   random* method;
public:
   Poisson(real);                         // constructor (real=mean)
   ~Poisson() { delete method; }
   real next() { return method->next(); }
   ext_real Mean() const { return method->Mean(); }
   ext_real Variance() const { return method->Variance(); }
};

//***************** binomial random number generator *******************

class Binomial : public random            // generate binomial rv
{
   random* method;

public:
   Binomial(int p, real n);               // constructor (int=n, real=p)
   ~Binomial() { delete method; }
   real next() { return method->next(); }
   ext_real Mean() const { return method->Mean(); }
   ext_real Variance() const { return method->Variance(); }
};

//************** negative binomial random number generator *****************

class NegativeBinomial : public AsymGen   // generate negative binomial rv
{
   real N, P, Q, p, ln_q, c;

public:
   NegativeBinomial(real NX, real PX);    // constructor
   real density(real) const;              // Negative binomial density function
   real next();
   ext_real Mean() const { return N * P; }
   ext_real Variance() const { return N * P * Q; }
};



//************************ sum of random numbers ************************

class NegatedRandom : public random
{
protected:
   random* rv;
   NegatedRandom(random& rvx) : rv(&rvx) {}
   void tDelete() { rv->tDelete(); delete this; }

public:
   real next();
   ext_real Mean() const;
   ext_real Variance() const;
   friend NegatedRandom& operator-(random&);
};

class ScaledRandom : public random
{
protected:
   random* rv; real s;
   ScaledRandom(random& rvx, real sx) : rv(&rvx), s(sx) {}
   void tDelete() { rv->tDelete(); delete this; }

public:
   real next();
   ext_real Mean() const;
   ext_real Variance() const;
   friend ScaledRandom& operator*(real, random&);
   friend ScaledRandom& operator*(random&, real);
   friend ScaledRandom& operator/(random&, real);
};

class ReciprocalRandom : public random
{
protected:
   random* rv; real s;
   ReciprocalRandom(random& rvx, real sx) : rv(&rvx), s(sx) {}
   void tDelete() { rv->tDelete(); delete this; }

public:
   real next();
   ext_real Mean() const { return EXT_REAL_CODE::Missing; }
   ext_real Variance() const { return EXT_REAL_CODE::Missing; }
   friend ReciprocalRandom& operator/(real, random&);
};

class ShiftedRandom : public ScaledRandom
{
   ShiftedRandom(random& rvx, real sx) : ScaledRandom(rvx, sx) {}

public:
   real next();
   ext_real Mean() const;
   ext_real Variance() const;
   friend ShiftedRandom& operator+(real, random&);
   friend ShiftedRandom& operator+(random&, real);
   friend ShiftedRandom& operator-(random&, real);
};

class ReverseShiftedRandom : public ScaledRandom
{
   ReverseShiftedRandom(random& rvx, real sx) : ScaledRandom(rvx, sx) {}

public:
   real next();
   ext_real Mean() const;
   ext_real Variance() const;
   friend ReverseShiftedRandom& operator-(real, random&);
};

class RepeatedRandom : public random
{
   random* rv; int n;
   void tDelete() { rv->tDelete(); delete this; }
   RepeatedRandom(random& rvx, int nx)  : rv(&rvx), n(nx) {}

public:
   real next();
   ext_real Mean() const;
   ext_real Variance() const;
   friend class random;
};

class MultipliedRandom : public random
{
protected:
   random* rv1; random* rv2;
   void tDelete() { rv1->tDelete(); rv2->tDelete(); delete this; }
   MultipliedRandom(random& rv1x, random& rv2x) : rv1(&rv1x), rv2(&rv2x) {}

public:
   real next();
   ext_real Mean() const;
   ext_real Variance() const;
   friend MultipliedRandom& operator*(random&, random&);
};

class AddedRandom : public MultipliedRandom
{
   AddedRandom(random& rv1x, random& rv2x) : MultipliedRandom(rv1x, rv2x) {}

public:
   real next();
   ext_real Mean() const;
   ext_real Variance() const;
   friend AddedRandom& operator+(random&, random&);
};

class SubtractedRandom : public MultipliedRandom
{
   SubtractedRandom(random& rv1x, random& rv2x)
      : MultipliedRandom(rv1x, rv2x) {}

public:
   real next();
   ext_real Mean() const;
   ext_real Variance() const;
   friend SubtractedRandom& operator-(random&, random&);
};

class DividedRandom : public MultipliedRandom
{
   DividedRandom(random& rv1x, random& rv2x)
      : MultipliedRandom(rv1x, rv2x) {}

public:
   real next();
   ext_real Mean() const { return EXT_REAL_CODE::Missing; }
   ext_real Variance() const { return EXT_REAL_CODE::Missing; }
   friend DividedRandom& operator/(random&, random&);
};

class SumRandom : public random           // sum of random variables
{
   void operator=(const SumRandom&) {}    // private so can't access
   random* rv;

public:
   SumRandom(NegatedRandom& rvx) : rv(&rvx) { m_name = "SumRandom"; }
   SumRandom(AddedRandom& rvx) : rv(&rvx) { m_name = "SumRandom"; }
   SumRandom(MultipliedRandom& rvx) : rv(&rvx) { m_name = "SumRandom"; }
   SumRandom(SubtractedRandom& rvx) : rv(&rvx) { m_name = "SumRandom"; }
   SumRandom(DividedRandom& rvx) : rv(&rvx) { m_name = "SumRandom"; }
   SumRandom(ShiftedRandom& rvx) : rv(&rvx) { m_name = "SumRandom"; }
   SumRandom(ReverseShiftedRandom& rvx) : rv(&rvx) { m_name = "SumRandom"; }
   SumRandom(ScaledRandom& rvx) : rv(&rvx) { m_name = "SumRandom"; }
   SumRandom(ReciprocalRandom& rvx) : rv(&rvx) { m_name = "SumRandom"; }
   SumRandom(RepeatedRandom& rvx) : rv(&rvx) { m_name = "SumRandom"; }
   real next() { return rv->next(); }
   ext_real Mean() const { return rv->Mean(); }
   ext_real Variance() const { return rv->Variance(); }
   ~SumRandom() { rv->tDelete(); }
};

//******************** mixtures of random numbers ************************


class SelectedRandom : public random
{
   friend class random;
   random* rv; real p;
   SelectedRandom(random& rvx, real px) : rv(&rvx), p(px) {}
   void tDelete() { rv->tDelete(); delete this; }

public:
   void load(int*, real*, random**);
   real next();
};

class AddedSelectedRandom : public random
{
   friend class random;

protected:
   random* rv1; random* rv2;
   AddedSelectedRandom(random& rv1x, random& rv2x)
      : rv1(&rv1x), rv2(&rv2x) {}
   void tDelete() { rv1->tDelete(); rv2->tDelete(); delete this; }

public:
   int nelems() const;
   void load(int*, real*, random**);
   real next();
   friend AddedSelectedRandom& operator+(SelectedRandom&,
      SelectedRandom&);
   friend AddedSelectedRandom& operator+(AddedSelectedRandom&,
      SelectedRandom&);
   friend AddedSelectedRandom& operator+(SelectedRandom&,
      AddedSelectedRandom&);
   friend AddedSelectedRandom& operator+(AddedSelectedRandom&,
      AddedSelectedRandom&);
};

class MixedRandom : public random         // mixtures of random numbers
{
   void operator=(const MixedRandom&) {}  // private so can't access
   int n;                                 // number of components
   DiscreteGen* dg;                       // discrete mixing distribution
   random** rv;                           // array of pointers to rvs
   ext_real mean, var;
   void Build(real*);                     // used by constructors

public:
   MixedRandom(int, real*, random**);
   MixedRandom(AddedSelectedRandom&);
   ~MixedRandom();
   real next();
   ext_real Mean() const { return mean; }
   ext_real Variance() const { return var; }
};

//******************* Permutation generator *******************************

class RandomPermutation                   // generate permutation of integers
{
   random U;

public:
   void next(int N, int M, int p[], int start = 0);
                                          // select permutation of M numbers
                                          // from start, ..., start+N-1
                                          // results to p
   void next(int N, int p[], int start = 0) { next(N, N, p, start); }
};

class RandomCombination : public RandomPermutation
                                          // generate combination of integers
                                          // sorted - ascending
{
   void SortAscending(int n, int gm[]);

public:
   void next(int N, int M, int p[], int start = 0)
      { RandomPermutation::next(N, M, p, start); SortAscending(M, p); }
   void next(int N, int p[], int start = 0) { next(N, N, p, start); }
};

//***************** Generators with variable parameters ********************

class VariPoisson
{
   Uniform U;
   Normal N;
   Poisson P100;
   Poisson P200;
   int iNext_very_small(real mu);
   int iNext_small(real mu);
   int iNext_large(real mu);
public:
   VariPoisson();
   int iNext(real mu);
};

class VariBinomial
{
   Uniform U;
   Normal N;
   int iNext_very_small(int n, real p);
   int iNext_small(int n, real p);
   int iNext_large(int n, real p);
public:
   VariBinomial();
   int iNext(int n, real p);
};

class VariLogNormal
{
   Normal N;
public:
   VariLogNormal() {}
   real next(real mean, real sd);
};

// friend functions declared again outside class definitions
NegatedRandom& operator-(random&);
ScaledRandom& operator*(real, random&);
ScaledRandom& operator*(random&, real);
ScaledRandom& operator/(random&, real);
ReciprocalRandom& operator/(real, random&);
ShiftedRandom& operator+(real, random&);
ShiftedRandom& operator+(random&, real);
ShiftedRandom& operator-(random&, real);
ReverseShiftedRandom& operator-(real, random&);
MultipliedRandom& operator*(random&, random&);
AddedRandom& operator+(random&, random&);
SubtractedRandom& operator-(random&, random&);
DividedRandom& operator/(random&, random&);
AddedSelectedRandom& operator+(SelectedRandom&, SelectedRandom&);
AddedSelectedRandom& operator+(AddedSelectedRandom&, SelectedRandom&);
AddedSelectedRandom& operator+(SelectedRandom&, AddedSelectedRandom&);
AddedSelectedRandom& operator+(AddedSelectedRandom&, AddedSelectedRandom&);



}

#endif

// body file: newran.cpp




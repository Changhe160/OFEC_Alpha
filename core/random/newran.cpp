#include <cmath>
#include "newran.h"

namespace ofec {


	//**************************** utilities ******************************
	inline double square(double x) { return x * x; }
	inline ExtReal square(const ExtReal& x) { return x * x; }

	static void ErrorNoSpace() {
		throw Exception("Newran: out of space@ ErrorNoSpace()");
	}

	//************************* end of definitions ************************


	double RandBase::raw()                           // get new uniform random number
	{
		// m = 2147483647 = 2^31 - 1; a = 16807;
		// 127773 = m div a; 2836 = m mod a
		long iseed = (long)m_seed;
		long hi = iseed / 127773L;                 // integer division
		long lo = iseed - hi * 127773L;            // modulo
		iseed = 16807 * lo - 2836 * hi;
		if (iseed <= 0)
			iseed += 2147483647L;
		m_seed = (double)iseed;
		return m_seed * 4.656612875e-10;
	}

	double RandBase::density(double) const {

		throw Exception("density function not defined@RandBase::density");

		return 0.0;
	}

#ifdef _MSC_VER
	static void DoNothing(int) {}
#endif

	double RandBase::next()                          // get new mixed random number
	{
		if (!m_seed)
			throw Exception("RandBase number generator not initialised@RandBase::next");
		++m_index;
		int i = (int)(raw() * 128);               // 0 <= i < 128
#ifdef _MSC_VER
		DoNothing(i); DoNothing(i);
#endif
		double f = m_buffer[i];
		m_buffer[i] = raw();  // Microsoft release gets this wrong
		return f;

		// return Mother(&iseed);
	}

	double RandBase::get()                  // get random number seed
	{
		return m_seed / 2147483648UL;
	}

	RandBase::RandBase(double s)            // set random number seed
										  // s must be between 0 and 1
	{

		if (s >= 1.0 || s <= 0.0)
			throw Exception("Newran: seed out of range@RandBase::RandBase");
		m_index = 0;
		//iseed = 2147483648L * s;         // for Mother
		m_motherseed = s;
		m_name = "random";
		initialize();
	}

	void RandBase::initialize() {
		m_seed = (long)(m_motherseed * 2147483648L);
		for (int i = 0; i < 128; i++)
			m_buffer[i] = raw();
	}


	Positive::Positive(double s) :RandBase(s)                             // Constructor
	{
#ifdef MONITOR
		cout << "constructing positive\n";
#endif
		m_not_ready = true;
		m_name = "positive";
	}

	Positive::~Positive() {
		if (!m_not_ready) {
#ifdef MONITOR
			cout << "freeing positive arrays\n";
#endif
			//delete[] m_sx; delete[] m_sfx;
			//m_sx = m_sfx = 0;
		}
#ifdef MONITOR
		cout << "destructing positive\n";
#endif
	}

	void Positive::build(bool sym)                 // set up arrays
	{
#ifdef MONITOR
		cout << "building positive arrays\n";
#endif
		int i;
		m_not_ready = false;
		m_sx.resize(60); 
		m_sfx.resize(60);

	//	if (!m_sx || !m_sfx) ErrorNoSpace();

		double sxi = 0.0; double inc = sym ? 0.01 : 0.02;
		for (i = 0; i < 60; i++) {
			m_sx[i] = sxi; double f1 = density(sxi); m_sfx[i] = f1;
			if (f1 <= 0.0) goto L20;
			sxi += inc / f1;
		}

		throw Exception("Newran: area too large@positive::build");

	L20:
		if (i < 50) throw Exception("Newran: area too small@positive::build");

		m_xi = sym ? 2 * i : i;
		return;
	}

	double Positive::next() {
		double ak, y; int ir;
		if (m_not_ready) build(false);
		do {
			double r1 = RandBase::next();
			ir = (int)(r1 * m_xi); double sxi = m_sx[ir];
			ak = sxi + (m_sx[ir + 1] - sxi) * RandBase::next();
			y = m_sfx[ir] * RandBase::next();
		} while (y >= m_sfx[ir + 1] && y >= density(ak));
		return ak;
	}

	double Symmetric::next() {
		double s, ak, y; int ir;
		if (m_not_ready) build(true);
		do {
			s = 1.0;
			double r1 = RandBase::next();
			if (r1 > 0.5) { s = -1.0; r1 = 1.0 - r1; }
			ir = (int)(r1 * m_xi); double sxi = m_sx[ir];
			ak = sxi + (m_sx[ir + 1] - sxi) * RandBase::next();
			y = m_sfx[ir] * RandBase::next();
		} while (y >= m_sfx[ir + 1] && y >= density(ak));
		return s * ak;
	}

	Asymmetric::Asymmetric(double modex, double s) :RandBase(s)                 // Constructor
	{
#ifdef MONITOR
		cout << "constructing asymmetric\n";
#endif
		m_modex = modex; m_not_ready = true;
		m_name = "asymmetric";
	}

	void Asymmetric::build()                        // set up arrays
	{
#ifdef MONITOR
		cout << "building asymmetric arrays\n";
#endif
		int i;
		m_not_ready = false;
		m_sx.resize(121); 
		m_sfx.resize(121);

	//	if (!m_sx || !m_sfx)  ErrorNoSpace();

		double sxi = m_modex;
		for (i = 0; i < 120; i++) {
			m_sx[i] = sxi; double f1 = density(sxi); m_sfx[i] = f1;
			if (f1 <= 0.0) goto L20;
			sxi += 0.01 / f1;
		}

		throw Exception("Newran: area too large (a)@asymmetric::build");

	L20:
		ic = i - 1; m_sx[120] = sxi; m_sfx[120] = 0.0;
		sxi = m_modex;
		for (; i < 120; i++) {
			m_sx[i] = sxi; double f1 = density(sxi); m_sfx[i] = f1;
			if (f1 <= 0.0) goto L30;
			sxi -= 0.01 / f1;
		}

		throw Exception("Newran: area too large (b)@asymmetric::build");
	L30:
		if (i < 100)  throw Exception("Newran: area too small@asymmetric::build");

		m_xi = i;
		return;
	}

	double Asymmetric::next() {
		double ak, y; int ir1;
		if (m_not_ready) build();
		do {
			double r1 = RandBase::next();
			int ir = (int)(r1 * m_xi); double sxi = m_sx[ir];
			ir1 = (ir == ic) ? 120 : ir + 1;
			ak = sxi + (m_sx[ir1] - sxi) * RandBase::next();
			y = m_sfx[ir] * RandBase::next();
		} while (y >= m_sfx[ir1] && y >= density(ak));
		return ak;
	}

	Asymmetric::~Asymmetric() {
		if (!m_not_ready) {
#ifdef MONITOR
			cout << "freeing asymmetric arrays\n";
#endif
		//	delete[] m_sx; delete[] m_sfx;
		}
#ifdef MONITOR
		cout << "destructing asymmetric\n";
#endif
	}

	Normal::Normal(double s) :Symmetric(s), count(0) {
		if (count) { m_not_ready = false; m_xi = Nxi; m_sx = Nsx; m_sfx = Nsfx; }
		else { build(true); Nxi = m_xi; Nsx = m_sx; Nsfx = m_sfx; }
		count++;
		m_name = "normal";
	}

	Normal::~Normal() {
		count--;
		if (count) m_not_ready = true;                     // disable freeing arrays
	}

	double Normal::density(double x) const               // normal density
	{
		return (std::fabs(x) > 8.0) ? 0 : 0.398942280 * exp(-x * x / 2);
	}

	double  Normal::nextNonStd(double rmean, double rvariance) {

		double X = next();
		double stddev = std::sqrt(rvariance);

		return rmean + stddev * X;
	}

	double Cauchy::density(double x) const               // cauchy density function
	{
		return (std::fabs(x) > 1.0e15) ? 0 : 0.31830988618 / (1.0 + x * x);
	}

	double  Cauchy::nextNonStd(double rmean, double rvariance) {

		double X = next();
		double stddev = std::sqrt(rvariance);

		return rmean + stddev * X;
	}

	double Exponential::density(double x) const          // Negative exponential
	{
		return  (x > 40.0 || x < 0.0) ? 0.0 : exp(-x);
	}


	Levy::Levy(double c, double s) :Asymmetric(c / 3, s) {
		m_sc = c;
		m_name = "levy";
	}
	double Levy::density(double x) const {
		if (x <= 0.0) return 0.0;
		double y;
		y = std::sqrt(0.5 * m_sc / 3.14159265358979323846) * exp(-0.5 * m_sc / x) / pow(x, 1.5);
		return (std::fabs(x) > 1.0e15) ? 0 : y;
	}


	Gamma::Gamma1::Gamma1(double alphax, double s) :Positive(s)                      // constructor (double=shape)
	{
		ralpha = 1.0 / alphax; ln_gam = ln_gamma(alphax + 1.0); alpha = alphax;
	}

	double Gamma::Gamma1::density(double x) const               // density function for
	{                                                // transformed gamma
		double l = -pow(x, ralpha) - ln_gam;
		return  (l < -40.0) ? 0.0 : exp(l);
	}

	double Gamma::Gamma1::next()                               // transform variable
	{
		return pow(Positive::next(), ralpha);
	}

	Gamma::Gamma2::Gamma2(double alphax, double s) : Asymmetric(alphax - 1.0, s) // constructor (double=shape)
	{
		alpha = alphax; ln_gam = ln_gamma(alpha);
	}

	double Gamma::Gamma2::density(double x) const                // gamma density function
	{
		if (x <= 0.0) return 0.0;
		double l = (alpha - 1.0) * log(x) - x - ln_gam;
		return  (l < -40.0) ? 0.0 : exp(l);
	}

	void Gamma::shape(double alpha) {

		//if (m_method!) delete m_method;

		if (alpha < 1.0) m_method.reset(new Gamma1(alpha, m_motherseed));
		else if (alpha == 1.0) m_method.reset(new Exponential(m_motherseed));
		else m_method.reset(new Gamma2(alpha, m_motherseed));

		if (m_method==nullptr)  ErrorNoSpace();

	}

	Gamma::Gamma(double alpha, double s)                         // general gamma generator
	{
		if (alpha < 1.0) m_method.reset(new Gamma1(alpha, s));
		else if (alpha == 1.0) m_method.reset(new Exponential(s));
		else m_method.reset(new Gamma2(alpha, s));

		if (m_method==nullptr)  ErrorNoSpace();

		m_motherseed = s;
		m_name = "gamma";
	}

	double ln_gamma(double xx) {
		// log gamma function adapted from numerical recipes in C

		if (xx < 1.0)                           // Use reflection formula
		{
			double piz = 3.14159265359 * (1.0 - xx);
			return log(piz / sin(piz)) - ln_gamma(2.0 - xx);
		}
		else {
			static double cof[6] = { 76.18009173,-86.50532033,24.01409822,
				-1.231739516,0.120858003e-2,-0.536382e-5 };

			double x = xx - 1.0; double tmp = x + 5.5;
			tmp -= (x + 0.5) * log(tmp); double ser = 1.0;
			for (int j = 0; j <= 5; j++) { x += 1.0; ser += cof[j] / x; }
			return -tmp + log(2.50662827465 * ser);
		}
	}

	Random::Random(Real seed) :
		initial_seed(seed),
		seed(seed),
		uniform(seed),
		normal(seed),
		cauchy(seed),
		levy(1.4, seed),
		gamma(0.5, seed) {}

	void Random::initialize() {
		uniform.initialize();
		normal.initialize();
		cauchy.initialize();
		levy.initialize();
		gamma.initialize();
	}
}




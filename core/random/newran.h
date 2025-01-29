/******************************************************************************
* Project:Open Frameworks for Evolutionary Computation (OFEC)
*******************************************************************************
* Author: Yiya Diao & Junchen Wang & Changhe Li & Hai Xia
* Email: diaoyiyacug@gmail.com & wangjunchen.chris@gmail.com & changhe.lw@gmail.com
* Language: C++
* Last-modified date: 2019.08.16
* 
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

#ifndef OFEC_NEWRAN_LIB_H
#define OFEC_NEWRAN_LIB_H

#include "../exception.h"
#include "extreal.h"

#include <utility>
#include <functional>
#include <vector>
#include <mutex>
#include "../enable_shared_ptr.h"

namespace ofec {
	typedef double(*PDF)(double);                // probability density function
	double ln_gamma(double);                      // log gamma function

	//**************** uniform random number generator **********************

	class RandBase : public EnableSharedPtr<RandBase> {                             // uniform random number generator
	protected:
		unsigned long long m_index = 0;
		std::mutex m_mutex;// for multithread
		double m_seed;                    // seed
		double m_motherseed;            // must be (0,1)
		double m_buffer[128];               // for mixing random numbers
		double raw();                     // unmixed random numbers
		std::string m_name;
	public:
		auto index() {
			return m_index;
		}
		double getSeed() {
			return m_seed;
		}
		double getMotherSeed() {
			return m_motherseed;
		}
		RandBase(double s);             // set seed (0 < seed < 1)
		void initialize();
		double get();                   // get seed
		virtual double next();                   // get new value
		double nextWithMutex() {
			std::unique_lock lcok(m_mutex);
			return next();
		}
		const char* name() { return m_name.c_str(); }                  // identification
		virtual double density(double) const;      // used by positive & Asymgen
		RandBase() { m_name = "random"; }                            // do nothing
		virtual ~RandBase() {}                   // make destructors virtual
		virtual ExtReal mean() const { return 0.5; }
		// mean of distribution
		virtual ExtReal variance() const { return 1.0 / 12.0; }
		// variance of distribution
		virtual void tDelete() {}              // delete components of sum
		virtual int nelems() const { return 1; }

		RandBase& operator=(const RandBase&) = delete;
	};


	//****************** uniform random number generator *********************

	class Uniform : public RandBase {
	public:
		Uniform(double s) :RandBase(s) { m_name = "uniform"; }                           // set value
		double next() { return RandBase::next(); }
		ExtReal mean() const { return 0.5; }
		ExtReal variance() const { return 1.0 / 12.0; }
		double density(double x) const { return (x < 0.0 || x > 1.0) ? 0 : 1.0; }

		template<typename T>
		T nextNonStd(T min, T max) { // get a value within [min,max)
			return min + static_cast<T>((max - min)*next());
		}

		template<typename T>
		void shuffle(T first, T last) { //iterator
			for (auto i = last - first - 1; i > 0; --i) {
				auto t = decltype(i) (i + 1)*next();
				std::swap(first[i], first[t]);
			}
		}

		/*
		* 
		*/
		template<typename T>
		void shuffle(T first, T last, size_t num) { //iterator
			for (auto i = 0; i < num; ++i) {
				auto t = decltype(i) (last - first) * next();
				std::swap(first[i], first[t]);
			}
		}

		template<typename T>
		T nextElem(T first, T last) {
			size_t num_elems = 0, seq_rand, seq_elem = 0;
			T iter = first;
			do {
				num_elems++;
			} while (++first != last);
			seq_rand = floor(num_elems * next());
			while (seq_elem++ != seq_rand)
				iter++;
			return iter;
		}

		template<typename T>
		T spinWheel(T first, T last) {
			Real sum = 0;
			for (T iter = first; iter != last; ++iter) {
				sum += *iter;
			}
			if (sum == 0)return last;
			double rand_pos = sum * next();
			double accum = 0;
			T iter(first);
			for (; iter != last; ++iter) {
				accum += *iter;
				if (rand_pos <= accum)
					break;
			}
			return iter;
		}

		template<typename T, typename K>
		T spinWheel(T first, T last, const std::function<K( const T& cur_iter)>& pro_fun) {
			double sum = 0;
			for (T iter = first; iter != last; ++iter) {
				sum += pro_fun(iter);
			}
			if (sum == 0)return last;
			double rand_pos = sum * next();
			double accum = 0;
			T iter(first);
			for (; iter != last; ++iter) {
				accum += pro_fun(iter);
				if (rand_pos < accum)
					break;
			}
			return iter;
		}

		template<typename T, typename K>
		T greedyRandom(T first, T last, const std::function<K(int idx)>& pro_fun) {
			double max_val =-std::numeric_limits<double>::max();
			std::vector<int> idxs;
			int cur_idx(0);
			double cur_val(0);
			for (T iter = first; iter != last; ++iter) {
				cur_val = pro_fun(*iter);
				if (cur_val > max_val) {
					max_val = cur_val;
					idxs.clear();
					idxs.push_back(cur_idx);
				}
				else if (cur_val == max_val) {
					idxs.push_back(cur_idx);
				}
				++cur_idx;
			}
			if (idxs.empty())return first;
			else return idxs[nextNonStd<int>(0, idxs.size())] + first;
		}

		template<typename T,typename K>
		T greedyRandom(T first, T last, const std::function<K(const T& iter)>& pro_fun) {
			double max_val = -std::numeric_limits<double>::max();
			std::vector<int> idxs;
			int cur_idx(0);
			double cur_val(0);
			for (T iter = first; iter != last; ++iter) {
				cur_val = pro_fun(iter);
				if (cur_val > max_val) {
					max_val = cur_val;
					idxs.clear();
					idxs.push_back(cur_idx);
				}
				else if (cur_val == max_val) {
					idxs.push_back(cur_idx);
				}
				++cur_idx;
			}
			if (idxs.empty())return last;
			else return idxs[nextNonStd<int>(0, idxs.size())] + first;
		}

		template<typename T, typename K>
		T spinWheelIdx(T first, T last, const std::function<K(int idx)>& pro_fun) {
			double sum = 0;
			for (T iter = first; iter != last; ++iter) {
				sum += pro_fun(*iter);
			}
			if (sum == 0)return last;
			double rand_pos = sum * next();
			double accum = 0;
			T iter(first);
			for (; iter != last; ++iter) {
				accum += pro_fun(*iter);
				if (rand_pos < accum)
					break;
			}
			return iter;
		}

		template<typename T>
		void sequencePro(T first, T last,std::vector<int> & seq) {
			seq.clear();
			std::vector<double> pro;
			double sum(0);
			T cur(first);
			while (cur != last) {
				pro += *cur;
				sum += *cur;
			}
			while (sum > 0) {
				double rand_num = sum * next();
				for (int idx(0); idx < pro.size(); ++idx) {
					rand_num -= pro[idx];
					if (rand_num <= 0) {
						seq.push_back(idx);
						sum -= pro[idx];
						pro[idx] = 0;
						break;
					}
				}
			}
		}

		template<typename T>
		void sequencePro(T first, T last, const std::function<double(int idx)>& pro_fun, std::vector<int>& seq) {
			seq.clear();
			std::vector<double> pro;
			double sum(0);
			T cur(first);
			double cur_val(0);
			while (cur != last) {
				cur_val = pro_fun(*cur);
				pro.push_back(cur_val);
				sum += cur_val;
			}
			while (sum > 0) {
				double rand_num = sum * next();
				for (int idx(0); idx < pro.size(); ++idx) {
					rand_num -= pro[idx];
					if (rand_num <= 0) {
						seq.push_back(idx);
						sum -= pro[idx];
						pro[idx] = 0;
						break;
					}
				}
			}
		}

		template<typename T>
		void sequenceProIdx(T first, T last, const std::function<double(T cur_iter)>& pro_fun, std::vector<int>& seq) {
			seq.clear();
			std::vector<double> pro;
			double sum(0);
			T cur(first);
			double cur_val(0);
			while (cur != last) {
				cur_val = pro_fun(cur);
				pro.push_back(cur_val);
				sum += cur_val;
			}
			while (sum > 0) {
				double rand_num = sum * next();
				for (int idx(0); idx < pro.size(); ++idx) {
					rand_num -= pro[idx];
					if (rand_num <= 0) {
						seq.push_back(idx);
						sum -= pro[idx];
						pro[idx] = 0;
						break;
					}
				}
			}
		}

	};


	//************************* return constant ******************************

	class Constant : public RandBase {
		double m_value;                            // value to be returned
	public:
		Constant(double v) {
			m_value = v;
			m_name = "constant";
		}          // set value
		double next() { return m_value; }
		ExtReal mean() const { return m_value; }
		ExtReal variance() const { return 0.0; }
	};

	//**************** positive random number generator **********************

	class Positive : public RandBase {            // generate positive r
		void operator=(const Positive&) {}       // private so can't access
	protected:
		double m_xi;
		std::vector<double> m_sx , m_sfx;
		bool m_not_ready;
		void build(bool);                      // called on first call to next

	public:
		Positive(double s);           // constructor
		~Positive();                             // destructor
		double next();                           // to get a single new value
		ExtReal mean() const { return (ExtReal)ExtRealCode::Missing; }
		ExtReal variance() const { return (ExtReal)ExtRealCode::Missing; }
	};

	//**************** symmetric random number generator **********************

	class Symmetric : public Positive {             // generate symmetric rv
	public:
		Symmetric(double s) : Positive(s) { m_name = "symmetric"; };
		double next();                           // to get a single new value
	};

	//**************** normal random number generator **********************

	class Normal : public Symmetric {           // generate standard normal rv
		double Nxi;
		std::vector<double> Nsx, Nsfx;          // so we need initialise only once
		long count;                     // assume initialised to 0

	public:
		Normal(double s);
		~Normal();
		double density(double) const;              // normal density function
		ExtReal mean() const { return 0.0; }
		ExtReal variance() const { return 1.0; }
		double  nextNonStd(double rmean, double rvariance);
	};

	//**************** cauchy random number generator **********************

	class Cauchy : public Symmetric {              // generate standard cauchy rv
	public:
		Cauchy(double s) : Symmetric(s) { m_name = "cauchy"; };
		double density(double) const;              // cauchy density function
		ExtReal mean() const { return ExtRealCode::Indefinite; }
		ExtReal variance() const { return ExtRealCode::PlusInfinity; }
		double nextNonStd(double rmean, double rvariance);
	};

	//**************** exponential random number generator **********************

	class Exponential : public Positive {        // generate standard exponential rv
	public:
		Exponential(double s) :Positive(s) { m_name = "exponential"; };
		double density(double) const;              // exponential density function
		ExtReal mean() const { return 1.0; }
		ExtReal variance() const { return 1.0; }
	};

	//**************** asymmetric random number generator **********************

	class Asymmetric : public RandBase {            // generate asymmetric rv
		double m_xi;
		std::vector<double> m_sx,  m_sfx; 
		int ic;
		bool m_not_ready;
		void build();                          // called on first call to next

	protected:
		double m_modex;

	public:
		Asymmetric(double, double s);                         // constructor (double=mode)
		~Asymmetric();                            // destructor
		double next();                           // to get a single new value
		ExtReal mean() const { return (ExtReal)ExtRealCode::Missing; }
		ExtReal variance() const { return (ExtReal)ExtRealCode::Missing; }
	};

	class Levy : public Asymmetric {
	public:
		double m_sc;
		Levy(double, double s);
		double density(double) const;              // levy density function
		ExtReal mean() const { return ExtRealCode::Indefinite; }
		ExtReal variance() const { return ExtRealCode::Indefinite; }
	};

	//**************** gamma random number generator **********************

	class Gamma : public RandBase {              // generate gamma rv
		std::shared_ptr<RandBase> m_method;

		//********** gamma random number generator, alpha <= 1 *****************

		class Gamma1 : public Positive {          // generate gamma rv								  
			double ln_gam, ralpha, alpha;		  // shape parameter <= 1
		public:
			Gamma1(double, double s);                          // constructor (double=shape)
			double density(double) const;              // gamma density function
			double next();                           // carries out power transform
			ExtReal mean() const { return alpha; }
			ExtReal variance() const { return alpha; }
		};

		//********** gamma random number generator, alpha > 1 ******************

		class Gamma2 : public Asymmetric {          // generate gamma rv		 
			double alpha, ln_gam;                      // shape parameter > 1
		public:
			Gamma2(double, double s);                          // constructor (double=shape)
			double density(double) const;              // gamma density function
			ExtReal mean() const { return alpha; }
			ExtReal variance() const { return alpha; }
		};

	public:
		Gamma(double, double s);                           // constructor (double=shape)
		~Gamma() { m_method.reset(); }
		double next() { return m_method->next(); }
		ExtReal mean() const { return m_method->mean(); }
		ExtReal variance() const { return m_method->variance(); }
		void shape(double);
	};

	struct Random : public EnableSharedPtr<Random> {
		enum RandomType {
			kUniform = 0,
			kNormal,
			kCauchy,
			kLevy,
			kGamma
		};
		Random(Real seed);
		void initialize();
		double next(RandomType type) {
			switch (type)
			{
			case kUniform:
				return uniform.next();
				break;
			case kNormal:
				return normal.next();
				break;
			case kCauchy:
				return cauchy.next();
				break;
			case kLevy:
				return levy.next();
				break;
			case kGamma:
				return gamma.next();
				break;
			default:
				return 0;
				break;
			}
		}
		const Real initial_seed;
		Real seed;
		Uniform uniform;
		Normal normal;
		Cauchy cauchy;
		Levy levy;
		Gamma gamma;
	};
}

#endif //!OFEC_NEWRAN_LIB_H

// body file: newran.cpp




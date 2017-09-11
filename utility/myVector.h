#pragma once
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

#ifndef OFEC_myVector_H
#define OFEC_myVector_H

#include "../core/definition.h"
#include "../utility/typevar/typevar.h"
#include <random>
using namespace std;
namespace OFEC {

	class matrix;
	class myVector {
		vector<double> m_data;
		double m_length;
		bool m_write;
	public:
		class myVectorProxy {
		private:
			myVector & m_vec;
			int m_idx;
		public:
			myVectorProxy(myVector& v, int idx);
			myVectorProxy& operator=(const myVectorProxy& rhs);
			operator double() const;
			myVectorProxy&  operator=(double val);
			myVectorProxy& operator+=(double);
			myVectorProxy& operator-=(double);
			myVectorProxy& operator/=(double);
			myVectorProxy& operator*=(double);
		};
		friend class myVectorProxy;
		myVector(int s = 0);
		myVector(int s, double val);
		myVector(int s, double *val);
		myVector(const vector<double> & v);
		myVector(const vector<typevar> &v);
		myVector(const myVector&) = default;
		~myVector();
		myVector &operator =(const myVector & v);
		myVector &operator =(const vector<double> & v);
		myVector & operator +=(const myVector & v);
		myVector & operator -=(const myVector & v);
		myVector & operator *=(double val);
		myVector & operator /=(double val);
		myVector & operator -=(double val);
		myVector & operator +=(double val);
		myVector  operator *(double val) const;
		myVector  operator *(double val);
		myVector  operator /(double val);
		myVector  operator -(double val);
		myVector  operator +(double val);
		myVector getPointBetween(const myVector & v, double ratio);
		myVectorProxy operator [](int);
		const double &operator [](int)const;
		double operator *(const myVector & v);
		void projectionToV(const myVector &v);
		void normalize();
		void absoluteSum();
		int size()const;
		friend class matrix;
		void randWithinRadi(double radius, caller mode = caller::Algorithm);
		void randOnRadi(double radius, caller mode = caller::Algorithm);
		void randOnRadi(double radius, uniform_real_distribution<double> &unif, default_random_engine &gen);
		void randomize(double min = 0, double max = 1, caller mode = caller::Algorithm);
		void randomize(uniform_real_distribution<double> &unif, default_random_engine &gen, double min, double max);
		void norRandomize(caller mode = caller::Algorithm);
		void norRandWithinRadi(double radius, caller mode = caller::Algorithm);
		vector<double>::iterator  begin();
		vector<double>::iterator  end();
		double getAngle(myVector & v);
		double length();
		double getDis(const myVector&v);
		double getDis(const vector<double>&v);
		void push_back(double);
		myVector(myVector&& rhs);
		myVector& operator=(myVector&& rhs);
		vector<double> & data();
		const vector<double> & data()const;
		void zero();
	protected:
		void length_();
		friend ifstream &operator>>(ifstream &, myVector&);
	};

	inline void myVector::length_() {
		m_length = 0;
		for (auto &i : m_data) m_length += i*i;
		m_length = sqrt(m_length);
	}
	inline vector<double> & myVector::data() {
		return m_data;
	}
	inline const vector<double> & myVector::data()const {
		return m_data;
	}
}
#endif // !OFEC_myVector_H

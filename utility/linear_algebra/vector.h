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
* class vector defines basic operations for Euclidean vector operations
*
*********************************************************************************/
#ifndef OFEC_VECTOR_H
#define OFEC_VECTOR_H

#include <vector>
#include <fstream>
#include <random>
#include <iostream>

#include "../../core/definition.h"
#include "../../core/random/newran.h"

namespace ofec {
	class Vector;
	Real norm(const Vector &vec, Real p = 2);
	Vector power(const Vector &vec, Real exp = 2);
	std::vector<int> sort(const Vector &vec, bool ascend = true);

	class Vector final {
		friend Vector operator-(const Vector &vec);
	
		friend Vector operator*(Real val, const Vector &vec);
		friend Vector operator/(Real val, const Vector &vec);
		friend Vector operator+(Real val, const Vector &vec);
		friend Vector operator-(Real val, const Vector &vec);
		friend Vector operator>(Real val, const Vector &vec);
		friend Vector operator<(Real val, const Vector &vec);
		friend Vector operator>=(Real val, const Vector &vec);
		friend Vector operator<=(Real val, const Vector &vec);
		friend Vector operator==(Real val, const Vector &vec);
		friend Vector operator!=(Real val, const Vector &vec);

		friend Vector operator*(const Vector &vec, Real val);
		friend Vector operator/(const Vector &vec, Real val);
		friend Vector operator+(const Vector &vec, Real val);
		friend Vector operator-(const Vector &vec, Real val);
		friend Vector operator>(const Vector &left, Real val);
		friend Vector operator<(const Vector &left, Real val);
		friend Vector operator>=(const Vector &left, Real val);
		friend Vector operator<=(const Vector &left, Real val);
		friend Vector operator==(const Vector &left, Real val);
		friend Vector operator!=(const Vector &left, Real val);

		friend Real operator*(const Vector &left_vec, const Vector &right_vec);
		friend Vector operator+(const Vector &left_vec, const Vector &right_vec);
		friend Vector operator-(const Vector &left_vec, const Vector &right_vec);
		friend Vector operator>(const Vector &left_vec, const Vector &right_vec);
		friend Vector operator<(const Vector &left_vec, const Vector &right_vec);
		friend Vector operator>=(const Vector &left_vec, const Vector &right_vec);
		friend Vector operator<=(const Vector &left_vec, const Vector &right_vec);
		friend Vector operator==(const Vector &left_vec, const Vector &right_vec);
		friend Vector operator!=(const Vector &left_vec, const Vector &right_vec);

		friend Real norm(const Vector &vec, Real p);
		friend Real sum(const Vector &vec);
		friend Real prod(const Vector &vec);
		friend Real min(const Vector &vec);
		friend Real max(const Vector &vec);
		friend Real mean(const Vector &vec);
		friend Real prctile(const Vector &vec, size_t p);
		friend Real median(const Vector &vec);
		friend Real maxDiff(const Vector &vec);
		friend Vector power(const Vector &vec, Real exp);
		friend Vector squareRoot(const Vector &vec);
		friend Vector logarithm(const Vector &vec);
		friend Vector absolute(const Vector &vec);
		friend Vector normcdf(const Vector &vec);
		friend std::vector<int> find(const Vector &vec);
		friend std::vector<int> sort(const Vector &vec, bool ascend);
		friend Vector min(const Vector &left_vec, const Vector &right_vec);
		friend Vector max(const Vector &left_vec, const Vector &right_vec);

	private:
		std::vector<Real> m_data;
		Real m_norm = 0;
		bool m_wrote = true;

	public:
		class vector_ {
		private:
			Vector &m_vec;
			size_t m_idx;
		public:
			vector_(Vector &v, size_t idx);
			vector_& operator=(const vector_ &rhs);
			operator Real() const;
			vector_& operator=(Real);
			vector_& operator+=(Real);
			vector_& operator-=(Real);
			vector_& operator/=(Real);
			vector_& operator*=(Real);
		};
		friend class vector_;
		Vector(size_t n = 0);
		Vector(size_t n, Real val);
		Vector(const std::vector<Real> &v);
		Vector(std::vector<Real> &&v);
		Vector(const Vector &) = default;
		Vector(Vector &&rhs) = default;
		Real operator[](size_t) const;
		Real& operator[](size_t);

		std::vector<Real>& vect() { return m_data; }
		const std::vector<Real>& vect() const { return m_data; }

		Vector& operator=(const Vector &v);
		Vector& operator=(Vector &&rhs) noexcept;
		Vector& operator=(const std::vector<Real> &v);
		Vector& operator+=(const Vector &v);
		Vector& operator-=(const Vector &v);
		Vector& operator*=(Real val);
		Vector& operator/=(Real val);
		Vector& operator-=(Real val);
		Vector& operator+=(Real val);

		Vector pointBetween(const Vector &v, Real ratio) const;

		void pushBack(Real);
		void clear();

		Vector projection(const Vector &v) const;
		void normalize();

		size_t size()const;
		std::vector<Real>::iterator  begin();
		std::vector<Real>::iterator  end();

		void randomizeInSphere(Real radius, Uniform *rand);
		void randomizeOnSphere(Real radius, Uniform *rand);
		void randomizeOnSphere(Real radius, std::uniform_real_distribution<Real> &unif, std::default_random_engine &gen);
		void randomize(Uniform *uni, Real min = 0, Real max = 1);
		void randomize(std::uniform_real_distribution<Real> &unif, std::default_random_engine &gen, Real min, Real max);
		void randomize(Normal *nor);
		void randomizeInSphere(Real radius, Normal *nor, Uniform *uni);

		Real angle(Vector &v);
		Real norm();
		Real distance(const Vector &point) const;
		Real distance(const std::vector<Real> &point)const;
		void zeroize();
		void resize(size_t n) noexcept;
		Real perpendicularDistance(const Vector &point);

		Vector times(const Vector &vec) const;
		Vector rdivide(const Vector &vec) const;
		Vector concat(const Vector &vec) const;
		Vector concat(Real val) const;
		Vector slice(const std::vector<int> &ids) const;

		Vector power(Real exp = 2) const;

	protected:
		void norm_();

	public:
		static Vector zeros(size_t n);
		static Vector rand(size_t n, Uniform *unif);
		static Vector randn(size_t n, Normal *norm);

		friend std::ifstream& operator>>(std::ifstream &, Vector &);
		friend std::ostream& operator<<(std::ostream &, Vector &);
	};

}

#endif

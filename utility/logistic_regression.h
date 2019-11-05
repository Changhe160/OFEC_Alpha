/******************************************************************************
* Project:Open Frameworks for Evolutionary Computation (OFEC)
*******************************************************************************
* Author: Li Zhou
* Email: 441837060@qq.com
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
* This logistic regression model can be used to make predictions.
*
*********************************************************************************/

#ifndef OFEC_LOGISTIC_REGRESSION_H
#define OFEC_LOGISTIC_REGRESSION_H

#include <string>
#include <memory>
#include <vector>
#include <numeric>
#include <fstream>
#include <cmath>

namespace OFEC {

	void check_it(bool flag);

	template<typename T>
	class logistic_regression { 
	public:
		logistic_regression() = default;
		int init(const T* data, const T* labels, int train_num, int feature_length,
			int reg_kinds = -1, T learning_rate = 0.00001, int iterations = 10000, int train_method = 0, int mini_batch_size = 1);
		int train(/*const std::string& model*/);
		int load_model(const std::string& model);
		T predict(const T* data, int feature_length) const; 

		// Regularization kinds
		enum RegKinds {
			REG_DISABLE = -1, // Regularization disabled
			REG_L1 = 0 // L1 norm
		};

		// Training methods
		enum Methods {
			BATCH = 0,
			MINI_BATCH = 1
		};

	private:
		int store_model(const std::string& model) const;
		T calc_sigmoid(T x) const; // y = 1/(1+exp(-x))
		T norm(const std::vector<T>& v1, const std::vector<T>& v2) const;
		void batch_gradient_descent();
		void mini_batch_gradient_descent();
		void gradient_descent(const std::vector<std::vector<T>>& data_batch, const std::vector<T>& labels_batch, int length_batch);

		std::vector<std::vector<T>> data;
		std::vector<T> labels;
		int iterations = 1000;
		int train_num = 0; // train samples num
		int feature_length = 0;
		T learning_rate = 0.00001;
		std::vector<T> thetas; // regression coefficients
							   
		T lambda = (T)0.; // regularization method
		int train_method = 0;
		int mini_batch_size = 1;
	};

	template<typename T>
	int logistic_regression<T>::init(const T* data, const T* labels, int train_num, int feature_length,
		int reg_kinds, T learning_rate, int iterations, int train_method, int mini_batch_size)
	{
		if (train_num < 2) {
			fprintf(stderr, "logistic regression train samples num is too little: %d\n", train_num);
			return -1;
		}
		if (learning_rate <= 0) {
			fprintf(stderr, "learning rate must be greater 0: %f\n", learning_rate);
			return -1;
		}
		if (iterations <= 0) {
			fprintf(stderr, "number of iterations cannot be zero or a negative number: %d\n", iterations);
			return -1;
		}

		check_it(reg_kinds == -1 || reg_kinds == 0);
		check_it(train_method == 0 || train_method == 1);
		check_it(mini_batch_size >= 1 && mini_batch_size < train_num);

		if (reg_kinds == REG_L1) this->lambda = (T)1.;
		if (train_method == MINI_BATCH) this->train_method = 1;
		this->mini_batch_size = mini_batch_size;

		this->learning_rate = learning_rate;
		this->iterations = iterations;

		this->train_num = train_num;
		this->feature_length = feature_length;

		this->data.resize(train_num);
		this->labels.resize(train_num);

		for (int i = 0; i < train_num; ++i) {
			const T* p = data + i * feature_length;
			this->data[i].resize(feature_length + 1);
			this->data[i][0] = (T)1; // bias

			for (int j = 0; j < feature_length; ++j) {
				this->data[i][j + 1] = p[j];
			}

			this->labels[i] = labels[i];
		}

		this->thetas.resize(feature_length + 1, (T)0.); // bias + feature_length

		return 0;
	}

	template<typename T>
	int logistic_regression<T>::train(/*const std::string& model*/)
	{
		check_it(data.size() == labels.size());

		if (train_method == BATCH) batch_gradient_descent();
		else mini_batch_gradient_descent();

		//check_it(store_model(model) == 0);

		return 0;
	}

	template<typename T>
	void logistic_regression<T>::batch_gradient_descent()
	{
		for (int i = 0; i < iterations; ++i) {
			gradient_descent(data, labels, train_num);
		}
	}

	template<typename T>
	void logistic_regression<T>::mini_batch_gradient_descent()
	{
		const int step = mini_batch_size;
		const int iter_batch = (train_num + step - 1) / step;

		for (int i = 0; i < iterations; ++i) {
			int pos{ 0 };

			for (int j = 0; j < iter_batch; ++j) {
				std::vector<std::vector<T>> data_batch;
				std::vector<T> labels_batch;
				int remainder{ 0 };

				if (pos + step < train_num) remainder = step;
				else remainder = train_num - pos;

				data_batch.resize(remainder);
				labels_batch.resize(remainder, (T)0.);

				for (int t = 0; t < remainder; ++t) {
					data_batch[t].resize(thetas.size(), (T)0.);
					for (int m = 0; m < thetas.size(); ++m) {
						data_batch[t][m] = data[pos + t][m];
					}

					labels_batch[t] = labels[pos + t];
				}

				gradient_descent(data_batch, labels_batch, remainder);

				pos += step;
			}
		}
	}

	template<typename T>
	void logistic_regression<T>::gradient_descent(const std::vector<std::vector<T>>& data_batch, const std::vector<T>& labels_batch, int length_batch)
	{
		std::unique_ptr<T[]> z(new T[length_batch]), gradient(new T[this->thetas.size()]);
		for (int j = 0; j < length_batch; ++j) {
			z.get()[j] = (T)0.;
			for (int t = 0; t < this->thetas.size(); ++t) {
				z.get()[j] += data_batch[j][t] * this->thetas[t];
			}
		}

		std::unique_ptr<T[]> pcal_a(new T[length_batch]), pcal_b(new T[length_batch]), pcal_ab(new T[length_batch]);
		for (int j = 0; j < length_batch; ++j) {
			pcal_a.get()[j] = calc_sigmoid(z.get()[j]) - labels_batch[j];
			pcal_b.get()[j] = data_batch[j][0]; // bias
			pcal_ab.get()[j] = pcal_a.get()[j] * pcal_b.get()[j];
		}

		gradient.get()[0] = ((T)1. / length_batch) * std::accumulate(pcal_ab.get(), pcal_ab.get() + length_batch, (T)0.); // bias

		for (int j = 1; j < this->thetas.size(); ++j) {
			for (int t = 0; t < length_batch; ++t) {
				pcal_b.get()[t] = data_batch[t][j];
				pcal_ab.get()[t] = pcal_a.get()[t] * pcal_b.get()[t];
			}

			gradient.get()[j] = ((T)1. / length_batch) * std::accumulate(pcal_ab.get(), pcal_ab.get() + length_batch, (T)0.) +
				(this->lambda / length_batch) * this->thetas[j];
		}

		for (int i = 0; i < thetas.size(); ++i) {
			this->thetas[i] = this->thetas[i] - this->learning_rate / length_batch * gradient.get()[i];
		}
	}

	template<typename T>
	int logistic_regression<T>::load_model(const std::string& model)
	{
		std::ifstream file;
		file.open(model.c_str(), std::ios::binary);
		if (!file.is_open()) {
			fprintf(stderr, "open file fail: %s\n", model.c_str());
			return -1;
		}

		int length{ 0 };
		file.read((char*)&length, sizeof(length));
		thetas.resize(length);
		file.read((char*)thetas.data(), sizeof(T)*thetas.size());

		file.close();

		return 0;
	}

	template<typename T>
	T logistic_regression<T>::predict(const T* data, int feature_length) const
	{
		check_it(feature_length + 1 == thetas.size());

		T value{ (T)0. };
		for (int t = 1; t < thetas.size(); ++t) {
			value += data[t - 1] * thetas[t];
		}
		return (calc_sigmoid(value + thetas[0]));
	}

	template<typename T>
	int logistic_regression<T>::store_model(const std::string& model) const
	{
		std::ofstream file;
		file.open(model.c_str(), std::ios::binary);
		if (!file.is_open()) {
			fprintf(stderr, "open file fail: %s\n", model.c_str());
			return -1;
		}

		int length = thetas.size();
		file.write((char*)&length, sizeof(length));
		file.write((char*)thetas.data(), sizeof(T) * thetas.size());

		file.close();

		return 0;
	}

	template<typename T>
	T logistic_regression<T>::calc_sigmoid(T x) const
	{
		return ((T)1 / ((T)1 + exp(-x)));
	}

	template<typename T>
	T logistic_regression<T>::norm(const std::vector<T>& v1, const std::vector<T>& v2) const
	{
		check_it(v1.size() == v2.size());

		T sum{ 0 };

		for (int i = 0; i < v1.size(); ++i) {
			T minus = v1[i] - v2[i];
			sum += (minus * minus);
		}

		return std::sqrt(sum);
	}

}

#endif // OFEC_LOGISTIC_REGRESSION_H


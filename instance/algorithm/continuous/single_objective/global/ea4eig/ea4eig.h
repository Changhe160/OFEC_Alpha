/********* Begin Register Information **********
{   
    "dependency on libraries": [ "Eigen" ],
	"name": "EA4eig",
	"identifier": "EA4eig",
	"tags": [ "continuous", "single-objective" ]
}
*********** End Register Information **********/

/******************************************************************************
* Project:Open Frameworks for Evolutionary Computation (OFEC)
*******************************************************************************
* Author: Yiya Diao
* Email: diaoyiyacug@gmail.com
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
*@INPROCEEDINGS{9870433,
  author={Bujok, Petr and Kolenovsky, Patrik},
  booktitle={2022 IEEE Congress on Evolutionary Computation (CEC)}, 
  title={Eigen Crossover in Cooperative Model of Evolutionary Algorithms Applied to CEC 2022 Single Objective Numerical Optimisation}, 
  year={2022},
  volume={},
  number={},
  pages={1-8},
  keywords={Adaptation models;Computational modeling;Evolutionary computation;Benchmark testing;Numerical models;Optimization;Tuning;Differential Evolution;Evolution Strategy;co-operative model;competition;experiments;Eigen crossover},
  doi={10.1109/CEC55065.2022.9870433}}
*
*********************************************************************************/

#ifndef OFEC_EA4EIG_H
#define OFEC_EA4EIG_H

#include "../../../../../../core/algorithm/algorithm.h"
#include "../../../../../../core/problem/continuous/continuous.h"

#include <Eigen/Dense>


namespace ofec {
	class EA4eig : virtual public Algorithm {
		OFEC_CONCRETE_INSTANCE(EA4eig)
	protected:

        struct InitParameters {
            int N_init = 100;
            int N = N_init;
            int Nmin = 10;
            int n0 = 2;
            std::vector<int> cni;
            std::vector<int> ni;
            int nrst = 0;
            std::vector<int> success;

            void initialize(int h) {
                N_init = 100;
                N = N_init;
                Nmin = 10;
                n0 = 2;
                cni.assign(h, 0);
                ni.assign(h, n0);
                nrst = 0;
                success.assign(h, 0);
            }
        };


        struct IDEparameters {

            int gmax = 0;
            std::vector<double> SRg;
            int T = 0;
            int GT = 0;
            int gt = GT;
            int g = 0;
            int Tcurr = 0;

            void initialize(int maxFES, int N) {
                gmax = round(maxFES / N);
                SRg.assign(gmax, 2);
                T = gmax / 10;
                GT = std::floor(gmax / 2);
                gt = GT;
                g = 0;
                Tcurr = 0;
            }
        };


        struct CobideParameter {
            // cobide 参数
            double CBps = 0.5; // such part of P is selscetd fo eigenvestors...
            double peig = 0.4;//probability to use an eigenvector crossover
            int suceig = 0;
            std::vector<double> CBF, CBCR;

            void setCB(ofec::Random* rnd);
            void initialize(int N, ofec::Random* rnd) {
                // cobide 参数
                CBps = 0.5;
                peig = 0.4;
                suceig = 0;
                CBF.assign(N, 0);
                CBCR.assign(N, 0);

                setCB(rnd);
            }
        };


        struct CMEASparam {

            // CMAES 参数
            double sigma = 0;
            Eigen::MatrixXd oldPop;
            double myeps = 1e-6;
            void initialize(int N, int DIM, const ofec::Domain<ofec::Real>& curDomain) {
                sigma = (curDomain[0].limit.second - curDomain[0].limit.first) / 2;
                myeps = 1e-6;
                oldPop = Eigen::MatrixXd(DIM, N);
            }
        };


        struct SelectionParam {
            int mu = 0;
            Eigen::VectorXd weights;
            double mueff = 0;

            void initalize(int N) {
                mu = N / 2;
                weights = Eigen::VectorXd(mu);
                for (int i = 0; i < mu; ++i) {
                    weights[i] = std::log(mu + 0.5) - std::log(i);
                }
                weights = weights / weights.sum();
                double sumWeights = weights.sum();
                double sumSquaredWeights = weights.array().square().sum();
                mueff = sumWeights * sumWeights / sumSquaredWeights;
            }
        };


        struct AdaptationPa {
            // 计算参数
            double cc = 0;
            double cs = 0;
            double c1 = 0;
            double cmu = 0;
            double damps = 0;

            // 初始化变量
            Eigen::VectorXd pc;
            Eigen::VectorXd ps;

            Eigen::MatrixXd B;
            Eigen::VectorXd D;
            Eigen::MatrixXd CC;
            Eigen::MatrixXd invsqrtC;
            int eigeneval = 0;
            double chiN = 0;

            void initialize(int DIM, double mueff) {
                cc = (4 + mueff / DIM) / (DIM + 4 + 2 * mueff / DIM);
                cs = (mueff + 2) / (DIM + mueff + 5);
                c1 = 2 / ((DIM + 1.3) * (DIM + 1.3) + mueff);
                cmu = std::min(1 - c1, 2 * (mueff - 2 + 1 / mueff) / ((DIM + 2) * (DIM + 2) + mueff));
                damps = 1 + 2 * std::max(0.0, std::sqrt((mueff - 1) / (DIM + 1)) - 1) + cs;

                // 初始化变量
                pc = Eigen::VectorXd(DIM);
                pc.setZero();
                ps = Eigen::VectorXd(DIM);
                ps.setZero();

                B = Eigen::MatrixXd::Identity(DIM, DIM);
                D = Eigen::VectorXd::Ones(DIM);
                CC = B * Eigen::VectorXd(D.array().square()).asDiagonal() * B.transpose();
                invsqrtC = B * D.cwiseInverse().asDiagonal() * B.transpose();
                eigeneval = 0;
                chiN = std::sqrt(DIM) * (1 - 1 / (4 * DIM) + 1 / (21 * DIM * DIM));
            }
        };

        struct JSOparam {

            int Asize_max;
            int H = 5;
            std::vector<double> MF;
            std::vector<double> MCR;
            int k = 1;
            std::vector<std::vector<double>> A;
            double pmax = 0.25;
            double pmin = pmax / 2;

            void initialize(int N) {
                Asize_max = std::round(N * 2.6);
                H = 5;
                MF.assign(H, 0.3);
                MCR.assign(H, 0.8);
                MF[H - 1] = 0.9;
                MCR[H - 1] = 0.9;
                k = 1;
                A.clear();
                pmax = 0.25;
                pmin = pmax / 2;
            }
        };


	protected:

        template<typename T>
        void sortVector(std::vector<T>& values, std::vector<int>& sortedIds) {
            auto copyValues = values;
            for (int idx(0); idx < sortedIds.size(); ++idx) {
                copyValues[idx] = values[sortedIds[idx]];
            }
            swap(copyValues, values);
        }

        void calculateEigVect(const std::vector<std::shared_ptr<Continuous::SolutionType>>& P,
            Eigen::MatrixXcd& EigVect, double CBps);

        std::vector<double> crossoverWithEigVect(
            const ofec::Continuous::SolutionType& p,
            std::vector<double>& v,
            double CBCR,
            Eigen::MatrixXcd& EigVect,
            const ofec::Domain<ofec::Real>& curDomain,
            ofec::Random* rnd);

        std::vector<double> crossover(
            const ofec::Continuous::SolutionType& p,
            std::vector<double>& v,
            double CBCR,
            const ofec::Domain<ofec::Real>& curDomain,
            ofec::Random* rnd);


        void updateBestSoFar(double& bsf_fit_var,
            const std::vector<std::shared_ptr<Continuous::SolutionType>>& P) {
            for (auto& it : P) {
                if (bsf_fit_var < it->fitness()) {
                    bsf_fit_var = it->fitness();
                }
            }
        }


        void generation_cobide(
            int hh,
            InitParameters& initpar,
            CobideParameter& codipar,
           std::vector<std::shared_ptr<Continuous::SolutionType>>& P,
            const std::function<void(ofec::SolutionBase& sol, ofec::Environment* env)>& eval_fun,
            ofec::Environment* env,
            ofec::Random* rnd);

        void generation_IDE(
            int hh,
            InitParameters& initpar,
            IDEparameters& idepar,
            CobideParameter& codipar,
           std::vector<std::shared_ptr<Continuous::SolutionType>>& P,
            const std::function<void(ofec::SolutionBase& sol, ofec::Environment* env)>& eval_fun,
            ofec::Environment* env,
            ofec::Random* rnd
        );

        void generation_CMAES(
            int hh,
            InitParameters& initPar,
            SelectionParam& selePar,
            AdaptationPa& adaPar,
            CobideParameter& codiPar,
            CMEASparam& cmaesPar,
            std::vector<std::shared_ptr<Continuous::SolutionType>>& P,
            const std::function<void(ofec::SolutionBase& sol, ofec::Environment* env)>& eval_fun,
            ofec::Environment* env,
            ofec::Random* rnd
        );

        void generation_jSO(
            int hh,
            InitParameters& initPar,
            CobideParameter& codiPar,
            JSOparam& jsoPar,
            std::vector<std::shared_ptr<Continuous::SolutionType>>& P,
            const std::function<void(ofec::SolutionBase& sol, ofec::Environment* env)>& eval_fun,
            ofec::Environment* env,
            ofec::Random* rnd
        );

        void updateDatum(Environment* env);


		void addInputParameters();
		void initialize_(Environment* env) override;
		void run_(Environment* env) override;
		void initPop(Environment* env);

	protected:
		const double h = 4;
        double delta = 1.0 / double(5 * h);

        InitParameters initPar;
        IDEparameters idePar;
        CobideParameter codiPar;
        CMEASparam cmeasPar;
        SelectionParam selectPar;
        AdaptationPa adaPar;
        JSOparam jsoPar;


        std::function<void(ofec::SolutionBase& sol, ofec::Environment* env)> m_eval_fun;
        std::vector<std::shared_ptr<Continuous::SolutionType>> m_pop;
        
        double bsf_fit_var = 0;

	};

}

#endif // OFEC_SPSO07_H

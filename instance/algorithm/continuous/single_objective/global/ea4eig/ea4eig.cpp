#include "ea4eig.h"

#include "../../../../../../core/problem/continuous/continuous.h"
#include "../../../../../../utility/linear_algebra/vector.h"
#include "../../../../../../utility/functional.h"

#include "../../../../../../core/environment/environment.h"


#ifdef OFEC_PLAYBACK
#include <buffer/datum_inclusion.h>
#endif // OFEC_PLAYBACK

#ifdef OFEC_STATISTICS
#include <record/datum_inclusion.h>
#endif // OFEC_STATISTICS

#include "../../../../../../datum/datum_inclusion.h"



namespace ofec {

    

    // 模拟 roulete 函数
    std::pair<int, double> roulete(const std::vector<int>& cutpoints, ofec::Random* rnd) {
        int h = cutpoints.size();
        int ss = 0;
        for (int value : cutpoints) {
            ss += value;
        }
        double p_min = static_cast<double>(*std::min_element(cutpoints.begin(), cutpoints.end())) / ss;
        std::vector<double> cp;
        cp.push_back(static_cast<double>(cutpoints[0]));
        for (size_t i = 1; i < cutpoints.size(); ++i) {
            cp.push_back(cp[i - 1] + static_cast<double>(cutpoints[i]));
        }
        for (double& value : cp) {
            value /= ss;
        }
        double sumValue(0);
        for (auto& it : cp) {
            if (it < rnd->uniform.next()) {
                ++sumValue;
            }
        }


        int res = std::floor(sumValue);
        return std::make_pair(res, p_min);
    }
    double cauchy_rnd(double x0, double gamma, double randomValue) {
        return x0 + gamma * std::tan(OFEC_PI * (randomValue - 0.5));
    }


    std::vector<int> nahvyb_expt(int N, int k, int expt, ofec::Random* rnd) {
        std::vector<int> opora;
        for (int i = 0; i < N; ++i) {
            if (i != expt) {
                opora.push_back(i);
            }
        }
        rnd->uniform.shuffle(opora.begin(), opora.end());
        opora.resize(k);
        return opora;
        //std::vector<int> vyb;
        //for (int i = 0; i < k; ++i) {
        //    int index = rnd->uniform.nextNonStd<int>(0, opora.size());
        //    vyb.push_back(opora[index]);
        //    opora.erase(opora.begin() + index);
        //}
        //return vyb;
    }

    std::vector<int> nahvyb_expt(int N, int k, std::set<int> expt, ofec::Random* rnd) {
        std::vector<int> opora;
        for (int i = 0; i < N; ++i) {
            if (expt.find(i) == expt.end()) {
                opora.push_back(i);
            }
        }
        rnd->uniform.shuffle(opora.begin(), opora.end());
        opora.resize(k);
        return opora;
        //std::vector<int> vyb;
        //for (int i = 0; i < k; ++i) {
        //    int index = rnd->uniform.nextNonStd<int>(0, opora.size());
        //    vyb.push_back(opora[index]);
        //    opora.erase(opora.begin() + index);
        //}
        //return vyb;
    }


    std::vector<int> nahvyb(int N, int k, ofec::Random* rnd) {
        std::vector<int> opora(N);
        for (int i = 0; i < N; ++i) {
            opora[i] = i;
        }
        rnd->uniform.shuffle(opora.begin(), opora.end());
        opora.resize(k);
        return opora;
    }



    std::vector<double> zrcad(const std::vector<double>& y,
        const ofec::Domain<ofec::Real>& curDomain) {
        std::vector<double> result = y;
        for (size_t i = 0; i < y.size(); ++i) {
            if (y[i] < curDomain[i].limit.first || y[i] > curDomain[i].limit.second) {
                while (result[i] < curDomain[i].limit.first || result[i] > curDomain[i].limit.second) {
                    if (result[i] > curDomain[i].limit.second) {
                        result[i] = 2 * curDomain[i].limit.second - result[i];
                    }
                    else if (result[i] < curDomain[i].limit.first) {
                        result[i] = 2 * curDomain[i].limit.first - result[i];
                    }
                }
            }
        }
        return result;
    }

    std::vector<double> convertDoublePointerToVector(const double* arr, int size) {
        std::vector<double> vec;
        for (int i = 0; i < size; ++i) {
            vec.push_back(arr[i]);
        }
        return vec;
    }


    std::vector<double> convertToStdVector(const Eigen::VectorXcd& vec) {
        std::vector<double> result(vec.size());
        for (int i = 0; i < vec.size(); ++i) {
            result[i] = vec[i].real();
        }
        return result;
    }


    Eigen::MatrixXd calculateCovarianceMatrix(Eigen::MatrixXd& data) {
        using namespace Eigen;
        // Calculate the mean of each column
        VectorXd mean = data.colwise().mean();
        // Center the data by subtracting the mean
        MatrixXd centered = data.rowwise() - mean.transpose();
        // Calculate the covariance matrix
        MatrixXd covarianceMatrix = (centered.adjoint() * centered) / (data.rows() - 1);
        return covarianceMatrix;
        // Print the covariance matrix
        //std::cout << "Covariance Matrix:\n" << covarianceMatrix << std::endl;
    }

    void EA4eig::CobideParameter::setCB(ofec::Random* rnd) {
        {

            for (int i = 0; i < CBF.size(); ++i) {
                if (rnd->uniform.next() < 0.5) {
                    CBF[i] = cauchy_rnd(0.65, 0.1, rnd->uniform.next());
                }
                else {
                    CBF[i] = cauchy_rnd(1, 0.1, rnd->uniform.next());
                }
                while (CBF[i] < 0) {
                    if (rnd->uniform.next() < 0.5) {
                        CBF[i] = cauchy_rnd(0.65, 0.1, rnd->uniform.next());
                    }
                    else {
                        CBF[i] = cauchy_rnd(1, 0.1, rnd->uniform.next());
                    }
                }
                if (CBF[i] > 1) CBF[i] = 1;
                if (rnd->uniform.next() < 0.5) {
                    CBCR[i] = cauchy_rnd(0.1, 0.1, rnd->uniform.next());
                }
                else {
                    CBCR[i] = cauchy_rnd(0.95, 0.1, rnd->uniform.next());
                }
                if (CBCR[i] > 1) CBCR[i] = 1;
                else if (CBCR[i] < 0) CBCR[i] = 0;
            }

        }
    }


    void EA4eig::calculateEigVect(const std::vector<std::shared_ptr<Continuous::SolutionType>>& P,
        Eigen::MatrixXcd& EigVect, double CBps) {
        auto N = P.size();
        auto DIM = P.front()->variable().size();


        std::vector<int> pidx(P.size());
        for (int idx(0); idx < pidx.size(); ++idx) {
            pidx[idx] = idx;
        }

        std::sort(pidx.begin(), pidx.end(), [&](int a, int b) {
            return P[a]->fitness() > P[b]->fitness();
            });
        // 对 P 按适应度排序并取部分用于计算特征向量
        std::vector<std::vector<double>> Popeig;
        for (int idx = std::round(CBps * N); idx < N; ++idx) {
            Popeig.push_back(P[pidx[idx]]->variable().vector());
        }
        Eigen::MatrixXd covMatrix(Popeig.size(), DIM);
        for (int i = 0; i < Popeig.size(); ++i) {
            for (int j = 0; j < DIM; ++j) {
                covMatrix(i, j) = Popeig[i][j];
            }
        }

        covMatrix = calculateCovarianceMatrix(covMatrix);
        // Compute eigenvalues and eigenvectors
        Eigen::EigenSolver<Eigen::MatrixXd> solver(covMatrix);
        // 提取特征向量矩阵 V 和特征值矩阵 D
        EigVect = solver.eigenvectors();
    }

    std::vector<double> EA4eig::crossoverWithEigVect(
        const ofec::Continuous::SolutionType& p,
        std::vector<double>& v,
        double CBCR,
        Eigen::MatrixXcd& EigVect,
        const ofec::Domain<ofec::Real>& curDomain,
        ofec::Random* rnd) {

        auto DIM = p.variable().size();

        std::vector<double> y = p.variable().vector();

        std::vector<int> change;
        for (int j = 0; j < DIM; ++j) {
            if (rnd->uniform.next() < CBCR) {
                change.push_back(j);
            }
        }

        if (change.empty()) {
            change.push_back(rnd->uniform.nextNonStd<int>(0, DIM));
        }
        // Transpose and multiply
        Eigen::VectorXcd yeig = EigVect.transpose() * Eigen::Map<Eigen::VectorXd>(y.data(), DIM);
        Eigen::VectorXcd veig = EigVect.transpose() * Eigen::Map<Eigen::VectorXd>(v.data(), DIM);
        // Update yeig based on change
        for (auto j : change) {
            {
                yeig(j) = veig(j);
            }
        }
        auto result = EigVect * yeig;
        y = convertToStdVector(result);

        zrcad(y, curDomain);
        return y;
    }


    std::vector<double> EA4eig::crossover(
        const ofec::Continuous::SolutionType& p,
        std::vector<double>& v,
        double CBCR,
        const ofec::Domain<ofec::Real>& curDomain,
        ofec::Random* rnd) {

        int DIM = p.variable().size();

        std::vector<int> change;
        for (int j = 0; j < DIM; ++j) {
            if (rnd->uniform.next() < CBCR) {
                change.push_back(j);
            }
        }
        if (change.empty()) {
            change.push_back(rnd->uniform.nextNonStd<int>(0, DIM));
        }
        auto y = p.variable().vector();
        for (auto j : change) {
            {
                y[j] = v[j];
            }
        }
        zrcad(y, curDomain);
        return y;
        //  q.variable() = y;

    }


    void EA4eig::generation_cobide(
        int hh,
        InitParameters& initpar,
        CobideParameter& codipar,
        std::vector<std::shared_ptr<Continuous::SolutionType>>& P,
        const std::function<void(ofec::SolutionBase& sol, ofec::Environment* env)>& eval_fun,
        ofec::Environment* env,
        ofec::Random* rnd) {


        auto& success = initpar.success;
        auto& ni = initpar.ni;

        auto CBps = codipar.CBps;
        auto peig = codipar.peig;
        auto& suceig = codipar.suceig;
        auto& CBF = codipar.CBF;
        auto& CBCR = codipar.CBCR;

        ofec::Continuous* con_pro = dynamic_cast<ofec::Continuous*>(env->problem());
        int N = P.size();
        int DIM = con_pro->numberVariables();
        const auto& curDomain = con_pro->domain();


        std::vector<ofec::Continuous::SolutionType> Q(N,
            { con_pro->numberObjectives(), con_pro->numberConstraints(),con_pro->numberVariables() });
        int ceig = 0;


        {
            Eigen::MatrixXcd EigVect;
            if (rnd->uniform.next() < peig) {
                ceig = 1;
            }

            if (ceig == 1) {

                //VectorXcd D = solver.eigenvalues();
                calculateEigVect(P, EigVect, CBps);
            }


            for (int i = 0; i < N; ++i) {
                std::vector<int> vyb = nahvyb_expt(N, 3, i, rnd);
                const auto& r1 = P[vyb[0]]->variable().vector();
                const auto& r2 = P[vyb[1]]->variable().vector();
                const auto& r3 = P[vyb[2]]->variable().vector();
                std::vector<double> v(DIM);
                for (int j = 0; j < DIM; ++j) {
                    v[j] = r1[j] + CBF[i] * (r2[j] - r3[j]);
                }

                v = zrcad(v, curDomain);
                if (ceig == 1) {
                    Q[i].variable() = crossoverWithEigVect(*P[i], v, CBCR[i], EigVect, curDomain, rnd);
                }

                else {
                    Q[i].variable() = crossover(*P[i], v, CBCR[i], curDomain, rnd);
                }


                eval_fun(Q[i], env);

            }

            {
                for (int i = 0; i < N; ++i) {
                    if (Q[i].fitness() >= P[i]->fitness()) {
                        *P[i] = Q[i];
                        if (ceig == 1) {
                            suceig++;
                        }
                        success[hh]++;
                        ni[hh]++;
                    }
                    else {

                        codipar.setCB(rnd);
                    }
                }
                ceig = 0;
            }

        }

    }

    void EA4eig::generation_IDE(
        int hh,
        InitParameters& initpar,
        IDEparameters& idepar,
        CobideParameter& codipar,
        std::vector<std::shared_ptr<Continuous::SolutionType>>& P,
        const std::function<void(ofec::SolutionBase& sol, ofec::Environment* env)>& eval_fun,
        ofec::Environment* env,
        ofec::Random* rnd
    ) {


        using namespace std;
        using namespace ofec;
        using namespace Eigen;

        auto& success = initpar.success;
        auto& ni = initpar.ni;

        auto CBps = codipar.CBps;
        auto peig = codipar.peig;
        auto& CBF = codipar.CBF;
        auto& CBCR = codipar.CBCR;

        auto g = idepar.g;
        auto gmax = idepar.gmax;
        auto gt = idepar.gt;
        auto& Tcurr = idepar.Tcurr;
        auto T = idepar.T;





        ofec::Continuous* con_pro = dynamic_cast<ofec::Continuous*>(env->problem());
        int N = P.size();
        int DIM = con_pro->numberVariables();
        const auto& curDomain = con_pro->domain();


        std::vector<int> sortedIds(CBF.size(), 0);
        for (int idx(0); idx < sortedIds.size(); ++idx) {
            sortedIds[idx] = idx;
        }

        std::sort(sortedIds.begin(), sortedIds.end(), [&](int a, int b) {
            return P[a]->fitness() > P[b]->fitness();
            });

        sortVector(P, sortedIds);
        sortVector(CBF, sortedIds);
        sortVector(CBCR, sortedIds);

        std::vector<ofec::Continuous::SolutionType> Q(N,
            { con_pro->numberObjectives(), con_pro->numberConstraints(),con_pro->numberVariables() });

        for (int idx(0); idx < Q.size(); ++idx) {
            Q[idx] = *P[idx];
        }


        double IDEps = 0.1 + 0.9 * pow(10, (5 * (g / gmax - 1)));
        double pd = 0.1 * IDEps; // Probability of perturbation
        double SRT = (g < gt) ? 0 : 0.1;



        for (int i = 0; i < N; i++) {
            // Replace `nahvyb_expt` with an appropriate selection method
            std::vector<int> vyb = nahvyb_expt(N, 4, i, rnd);
            int o = vyb[0];
            if (g <= gt) o = i;

            int r1 = vyb[1], r2 = vyb[2], r3 = vyb[3];

            ofec::Vector xo = P[o]->variable().vector();
            ofec::Vector xr1 = P[r1]->variable().vector();
            ofec::Vector xr2 = P[r2]->variable().vector();
            ofec::Vector xr3 = P[r3]->variable().vector();


            for (int idim(0); idim < DIM; ++idim) {
                if (rnd->uniform.next() < pd) {
                    xr3[idim] = rnd->uniform.nextNonStd<double>(curDomain[idim].limit.first, curDomain[idim].limit.second);
                }
            }

            double Fo = static_cast<double>(o) / N + 0.1 * rnd->normal.next();
            while (Fo <= 0 || Fo > 1) {
                Fo = static_cast<double>(o) / N + 0.1 * rnd->normal.next();
            }

            int high_ind_S = static_cast<int>(IDEps * N);
            if (o > high_ind_S) {
                if (r1 > high_ind_S) {
                    vector<int> candidates;

                    // %  find "better" from Superior
                    for (int j = 0; j < high_ind_S; j++) {
                        if (find(vyb.begin(), vyb.end(), j) == vyb.end() && j != i) {
                            candidates.push_back(j);
                        }
                    }
                    r1 = rnd->uniform.nextNonStd<int>(0, candidates.size());
                    //r1 = candidates[static_cast<int>(rand() % candidates.size())];
                    xr1 = P[r1]->variable().vector();
                }
            }

            if ((g > gt) && (rnd->uniform.next() < 0.5)) {
                auto tmp = ofec::Vector(P[i]->variable().vector()) + Fo * (xr1 - xo) + Fo * (xr2 - xr3);
                Q[i].variable().vector() = tmp.vect();
                //  Q[i].variable().vector() = P.row(i).head(DIM) + Fo * (xr1 - xo) + Fo * (xr2 - xr3);
            }
            else {
                auto tmp = xo + Fo * (xr1 - xo) + Fo * (xr2 - xr3);
                Q[i].variable().vector() = tmp.vect();
            }
        }

        if (rnd->uniform.next() < peig) {
            Eigen::MatrixXcd EigVect;
            calculateEigVect(P, EigVect, CBps);

            for (int i = 0; i < N; i++) {
                auto v = Q[i].variable().vector();
                auto y = crossoverWithEigVect(*P[i], v, CBCR[i], EigVect, curDomain, rnd);
                // zrcad(y, curDomain);

                Q[i].variable() = y;
                //     crossover()
            }
        }
        else {
            for (int i = 0; i < N; i++) {
                double CR = static_cast<double>(i) / N + 0.1 * rnd->normal.next();
                while (CR < 0 || CR > 1) {
                    CR = static_cast<double>(i) / N + 0.1 * rnd->normal.next();
                }
                int jrand = rnd->uniform.nextNonStd<int>(0, DIM);
                for (int j = 0; j < DIM; j++) {
                    if (!(rnd->uniform.next() <= CR || j == jrand)) {
                        Q[i].variable()[j] = P[i]->variable()[j];
                    }
                    if (Q[i].variable()[j] < curDomain[j].limit.first || Q[i].variable()[j] > curDomain[j].limit.second) {
                        Q[i].variable()[j] = rnd->uniform.nextNonStd<double>(curDomain[j].limit.first, curDomain[j].limit.second);
                    }
                    //con_pro->validateSolution(Q[i], ofec::Validation::kReinitialize, rnd);
                }
            }
        }

        for (int idx(0); idx < N; ++idx) {
            eval_fun(Q[idx], env);
        }

        std::vector<int> indsucc;
        for (int idx(0); idx < P.size(); ++idx) {
            if (Q[idx].fitness() > P[idx]->fitness()) {
                indsucc.push_back(idx);
            }
        }
        success[hh] += indsucc.size();
        ni[hh] += indsucc.size();
        double SR = static_cast<double>(indsucc.size()) / N;

        if (g < gt) {
            if (SR <= SRT) {
                Tcurr++;
            }
            else {
                Tcurr = 0;
            }
            if (Tcurr >= T) {
                gt = g;
            }
        }


        for (auto& it : indsucc) {
            {
                *P[it] = Q[it];
            }
        }

        std::sort(sortedIds.begin(), sortedIds.end(), [&](int a, int b) {
            return P[a]->fitness() > P[b]->fitness();
            });
        sortVector(P, sortedIds);
        sortVector(CBF, sortedIds);
        sortVector(CBCR, sortedIds);


        g++;
    }

    void EA4eig::generation_CMAES(
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
    ) {

        auto& success = initPar.success;
        auto& ni = initPar.ni;

        auto& CBF = codiPar.CBF;
        auto& CBCR = codiPar.CBCR;

        auto mu = selePar.mu;
        const auto& weights = selePar.weights;
        auto mueff = selePar.mueff;

        auto& oldPop = cmaesPar.oldPop;
        auto& sigma = cmaesPar.sigma;




        auto& B = adaPar.B;
        auto& D = adaPar.D;
        auto& CC = adaPar.CC;
        auto& invsqrtC = adaPar.invsqrtC;
        auto& eigeneval = adaPar.eigeneval;
        auto& chiN = adaPar.chiN;

        auto& ps = adaPar.ps;
        auto& pc = adaPar.pc;

        auto& cc = adaPar.cc;
        auto& cs = adaPar.cs;
        auto& c1 = adaPar.c1;
        auto& cmu = adaPar.cmu;
        auto& damps = adaPar.damps;


        ofec::Continuous* con_pro = dynamic_cast<ofec::Continuous*>(env->problem());
        int N = P.size();
        int DIM = con_pro->numberVariables();
        const auto& curDomain = con_pro->domain();



        std::vector<int> sortedIds(P.size(), 0);
        for (int idx(0); idx < sortedIds.size(); ++idx) {
            sortedIds[idx] = idx;
        }

        std::sort(sortedIds.begin(), sortedIds.end(), [&](int a, int b) {
            return P[a]->fitness() > P[b]->fitness();
            });

        sortVector(P, sortedIds);
        sortVector(CBF, sortedIds);
        sortVector(CBCR, sortedIds);



        std::vector<ofec::Continuous::SolutionType> Q(N,
            { con_pro->numberObjectives(), con_pro->numberConstraints(),con_pro->numberVariables() });

        // 设置 weights 的值
        Eigen::MatrixXd PopMat(mu, DIM);
        for (int i = 0; i < mu; ++i) {
            for (int j = 0; j < DIM; ++j) {
                PopMat(i, j) = P[i]->variable()[j];
            }
        }
        // recombination, new mean value
        Eigen::VectorXd xmean = PopMat.transpose() * weights;
        Eigen::MatrixXd Pop(DIM, N);
        for (int kk = 0; kk < N; ++kk) {
            Eigen::VectorXd randnVec = Eigen::VectorXd::NullaryExpr(DIM, [&](auto) { return rnd->normal.next(); });
            Eigen::VectorXd temp = xmean + sigma * (B * (D.cwiseProduct(randnVec)));
            Pop.col(kk) = temp;

            // 边界处理
            for (int j = 0; j < DIM; ++j) {
                if (Pop(j, kk) < curDomain[j].limit.first) {
                    Pop(j, kk) = (oldPop(j, kk) + curDomain[j].limit.first) / 2;
                }
                else if (Pop(j, kk) > curDomain[j].limit.second) {
                    Pop(j, kk) = (oldPop(j, kk) + curDomain[j].limit.second) / 2;
                }
            }

            Q[kk].variable() = convertDoublePointerToVector(Pop.col(kk).data(), DIM);
            eval_fun(Q[kk], env);
            //if the solution is better than any of P - replace  and increase success

            int worstSolId(0);
            double worstSolFit(std::numeric_limits<double>::max());
            // if the solution is better than any of P - replace and increase success
            for (int pid(0); pid < P.size(); ++pid) {
                if (worstSolFit < P[pid]->fitness()) {
                    worstSolFit = P[pid]->fitness();
                    worstSolId = pid;
                }
            }
            if (Q[kk].fitness() > worstSolFit) {
                *P[worstSolId] = Q[kk];

                success[hh]++;
                ni[hh]++;
            }
        }


        auto& FES = env->algorithm()->evaluations();
        //Sort by fitness and compute weighted mean into xmean
        //// 排序并计算新的加权均值
        std::sort(sortedIds.begin(), sortedIds.end(), [&](int a, int b) {
            return Q[a].fitness() > Q[b].fitness();
            });
        auto xold = xmean;

        Eigen::MatrixXd sortedPop(DIM, mu);
        for (int i = 0; i < mu; ++i) {
            sortedPop.col(i) = Pop.col(sortedIds[i]);
        }
        xmean = sortedPop * weights;
        oldPop = Pop;
        // Cumulation: Update evolution paths

        ps = (1 - cs) * ps + sqrt(cs * (2 - cs) * mueff) * invsqrtC * (xmean - xold) / sigma;
        double sumSquaredPs = ps.array().square().sum();
        double numerator = sumSquaredPs / (1 - std::pow(1 - cs, 2.0 * FES / N)) / DIM;
        int hsig = numerator < 2 + 4.0 / (DIM + 1);
        pc = (1 - cc) * pc + hsig * sqrt(cc * (2 - cc) * mueff) * (xmean - xold) / sigma;

        //// 更新协方差矩阵 C
        // 
        Eigen::MatrixXd artmp = (1 / sigma) * (sortedPop - xold.replicate(1, mu));
        CC = (1 - c1 - cmu) * CC + c1 * (pc * pc.transpose() + (1 - hsig) * cc * (2 - cc) * CC) + cmu * artmp * weights.asDiagonal() * artmp.transpose();

        //// 更新步长 sigma
        sigma *= std::exp((cs / damps) * (ps.norm() / chiN - 1));
        if (sigma > 1e+300 || sigma < 1e-300 || std::isnan(sigma)) {
            sigma = (curDomain[0].limit.second - curDomain[0].limit.first) / 2;
        }

        //// 更新 B 和 D
        if (FES - eigeneval > N / (c1 + cmu) / DIM / 10) {
            eigeneval = FES;
            // 先取上三角部分 enforce symmetry
            Eigen::MatrixXd upperTriangular = CC.triangularView<Eigen::Upper>();
            Eigen::MatrixXd upperTriangularT = CC.triangularView<Eigen::StrictlyUpper>().transpose();
            // 加上上三角部分的转置（对角线除外）
            CC = upperTriangular + upperTriangularT;
            Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> eigSolver(CC);
            B = eigSolver.eigenvectors();
            D = eigSolver.eigenvalues();
            D = D.asDiagonal();
            D = D.array().sqrt();
            invsqrtC = B * D.cwiseInverse().asDiagonal() * B.transpose();
        }



    }


    void EA4eig::generation_jSO(
        int hh,
        InitParameters& initPar,
        CobideParameter& codiPar,
        JSOparam& jsoPar,
        std::vector<std::shared_ptr<Continuous::SolutionType>>& P,
        const std::function<void(ofec::SolutionBase& sol, ofec::Environment* env)>& eval_fun,
        ofec::Environment* env,
        ofec::Random* rnd
    ) {

        auto& success = initPar.success;
        auto& ni = initPar.ni;

        auto& peig = codiPar.peig;
        auto& CBps = codiPar.CBps;
        auto& CBCR = codiPar.CBCR;


        auto& pmax = jsoPar.pmax;
        auto& pmin = jsoPar.pmin;
        auto& MCR = jsoPar.MCR;
        auto& MF = jsoPar.MF;
        auto& H = jsoPar.H;
        auto& k = jsoPar.k;
        auto& A = jsoPar.A;
        auto Asize_max = jsoPar.Asize_max;


        ofec::Continuous* con_pro = dynamic_cast<ofec::Continuous*>(env->problem());
        int N = P.size();
        int DIM = con_pro->numberVariables();
        const auto& curDomain = con_pro->domain();
        auto maxFES = env->algorithm()->maximumEvaluations();


        std::vector<int> sortedIds(P.size(), 0);
        for (int idx(0); idx < sortedIds.size(); ++idx) {
            sortedIds[idx] = idx;
        }
        std::sort(sortedIds.begin(), sortedIds.end(), [&](int a, int b) {
            return P[a]->fitness() > P[b]->fitness();
            });

        int ceig = 0;
        {

            std::vector<double> Fpole(N, -1);
            std::vector<double> CRpole(N, -1);
            std::vector<double> SCR;
            std::vector<double> SF;
            int suc = 0;



            std::vector<ofec::Continuous::SolutionType> Q(N,
                { con_pro->numberObjectives(), con_pro->numberConstraints(),con_pro->numberVariables() });

            //for (int idx(0); idx < Q.size(); ++idx) {
            //    Q[idx] = *P[idx];
            //}

            Eigen::MatrixXcd EigVect;
            if (rnd->uniform.next() < peig) {
                ceig = 1;
            }
            if (ceig == 1) {
                calculateEigVect(P, EigVect, CBps);
            }



            auto& FES = env->algorithm()->evaluations();
            double pp = pmax - ((pmax - pmin) * (static_cast<double>(FES) / maxFES));
            int p = std::max<int>(2, std::ceil(pp * N));

            for (int i = 0; i < N; ++i) {
                int rr = rnd->uniform.nextNonStd<int>(0, H);
                double CR = MCR[rr] + std::sqrt(0.1) * rnd->normal.next();;
                if (CR > 1) CR = 1;
                else if (CR < 0) CR = 0;
                // jSO CR:
                if (FES < (0.25 * maxFES)) CR = std::max(CR, 0.7);
                else if (FES < (0.5 * maxFES)) CR = std::max(CR, 0.6);

                double F = -1;
                while (F <= 0) {
                    F = rnd->uniform.next() * OFEC_PI - OFEC_PI / 2;
                    F = 0.1 * std::tan(F) + MF[rr];
                }
                if (F > 1) F = 1;
                // jSO F:
                if ((FES < (0.6 * maxFES)) && (F > 0.7)) F = 0.7;

                Fpole[i] = F;
                CRpole[i] = CR;

                int expt = i;

                int ktery = sortedIds[rnd->uniform.nextNonStd<int>(0, p)];

                Eigen::VectorXd xpbest = Eigen::Map<const Eigen::VectorXd>(P[ktery]->variable().vector().data(), P[ktery]->variable().size());
                // (P[ktery]->variable().vector());

                Eigen::VectorXd xi = Eigen::Map<const Eigen::VectorXd>(P[expt]->variable().vector().data(), P[expt]->variable().size());

                //Eigen::VectorXd xi(P[expt]->variable().vector());

                std::vector<int> vyb = nahvyb_expt(N, 1, expt, rnd);
                //   Eigen::VectorXd r1(P[vyb.front()]->variable().vector());

                Eigen::VectorXd r1 = Eigen::Map<const Eigen::VectorXd>(P[vyb.front()]->variable().vector().data(), P[vyb.front()]->variable().size());

                std::set<int> exptSet = { expt,vyb.front() };

                vyb = nahvyb_expt(N + A.size(), 1, exptSet, rnd);
                Eigen::VectorXd r2;
                if (vyb.front() < N) {
                    r2 = Eigen::Map<const Eigen::VectorXd>(P[vyb.front()]->variable().vector().data(), P[vyb.front()]->variable().size());
                }
                else {
                    int idx = vyb.front() - N;

                    r2 = Eigen::Map<const Eigen::VectorXd>(A[idx].data(), A[idx].size());

                    //  r2 = Eigen::VectorXd(A[idx]);
                }

                double Fw;
                if (FES < 0.2 * maxFES) Fw = 0.7 * F;
                else if (FES < 0.4 * maxFES) Fw = 0.8 * F;
                else Fw = 1.2 * F;

                Eigen::VectorXd v = xi + Fw * (xpbest - xi) + F * (r1 - r2);
                auto vec_v = convertDoublePointerToVector(v.data(), v.size());

                if (ceig == 1) {
                    Q[i].variable() = crossoverWithEigVect(*P[i], vec_v, CBCR[i], EigVect, curDomain, rnd);
                }
                else {
                    Q[i].variable() = crossover(*P[i], vec_v, CR, curDomain, rnd);
                }
            }

            for (auto& it : Q) {
                eval_fun(it, env);
            }
            std::vector<double> deltafce;
            for (int idx(0); idx < N; ++idx) {
                if (Q[idx].fitness() > P[idx]->fitness()) {
                    deltafce.push_back(Q[idx].fitness() - P[idx]->fitness());
                    suc++;
                    if (A.size() < Asize_max) {
                        A.push_back(P[idx]->variable().vector());
                    }
                    else {
                        A[rnd->uniform.nextNonStd<int>(0, A.size())] = P[idx]->variable().vector();
                    }
                    SCR.push_back(CRpole[idx]);
                    SF.push_back(Fpole[idx]);
                }
                if (Q[idx].fitness() >= P[idx]->fitness()) {
                    *P[idx] = Q[idx];
                }
            }

            if (suc > 0) {
                auto MCR_old = MCR[k];
                auto MF_old = MF[k];
                double sum_delta(0);
                ofec::calSum(deltafce, sum_delta);
                auto vahyw = deltafce;
                for (auto& it : vahyw) {
                    it /= sum_delta;
                }
                double mSCR(0);
                ofec::calMax(SCR, mSCR);


                if (MCR[k] == -1 || mSCR == 0) {
                    MCR[k] = -1;
                }
                else {
                    std::vector<double> meanSCRpomjm(vahyw.size());
                    std::transform(vahyw.begin(), vahyw.end(), SCR.begin(), meanSCRpomjm.begin(), std::multiplies<double>());
                    std::vector<double> meanSCRpomci(vahyw.size());
                    std::transform(meanSCRpomjm.begin(), meanSCRpomjm.end(), SCR.begin(), meanSCRpomci.begin(), std::multiplies<double>());
                    double sumjm(0);
                    ofec::calSum(meanSCRpomjm, sumjm);
                    double sumci(0);
                    ofec::calSum(meanSCRpomci, sumci);
                    MCR[k] = sumci / sumjm;
                }

                std::vector<double> meanSCRpomjm(vahyw.size());
                std::transform(vahyw.begin(), vahyw.end(), SF.begin(), meanSCRpomjm.begin(), std::multiplies<double>());
                std::vector<double> meanSCRpomci(vahyw.size());
                std::transform(meanSCRpomjm.begin(), meanSCRpomjm.end(), SF.begin(), meanSCRpomci.begin(), std::multiplies<double>());
                double sumjm(0);
                ofec::calSum(meanSCRpomjm, sumjm);
                double sumci(0);
                ofec::calSum(meanSCRpomci, sumci);

                MF[k] = sumci / sumjm;
                MCR[k] = (MCR[k] + MCR_old) / 2;
                MF[k] = (MF[k] + MF_old) / 2;
                k++;
                if (k + 1 >= H) {
                    k = 0;
                }

                success[hh] = success[hh] + suc;
                ni[hh] = ni[hh] + suc;
            }

        }
    }



	void EA4eig::addInputParameters() {
	}

	void EA4eig::initialize_(Environment* env) {
		Algorithm::initialize_(env);

        ofec::Continuous* con_pro = dynamic_cast<ofec::Continuous*>(env->problem());
        int DIM = con_pro->numberVariables();
        const auto& curDomain = con_pro->domain();
        auto maxFES = env->algorithm()->maximumEvaluations();

        
        initPar.initialize(h);
        auto& N = initPar.N;
        idePar.initialize(maximumEvaluations(), initPar.N);
        codiPar.initialize(N, m_random.get());
        cmeasPar.initialize(N, DIM, curDomain);
        selectPar.initalize(N);
        adaPar.initialize(DIM, selectPar.mueff);
        jsoPar.initialize(N);


        m_eval_fun=[](ofec::SolutionBase& sol, ofec::Environment* env) {
            using namespace ofec;
            sol.evaluate(env, true);
            ofec::Real pos = env->problem()->optimizeMode(0) == ofec::OptimizeMode::kMaximize ? 1 : -1;
            sol.setFitness(pos * sol.objective(0));
            };

        bsf_fit_var=(std::numeric_limits<double>::lowest());
	}

	void EA4eig::run_(Environment* env) {
		initPop(env);


        //std::vector<std::shared_ptr<Continuous::SolutionType>> P;
        //for (auto& it : m_pop) {
        //    P.push_back(dynamic_cast<Continuous::SolutionType*>(it.get()));
        //}
        updateBestSoFar(bsf_fit_var, m_pop);


        int maxFES = maximumEvaluations();
        auto& ni = initPar.ni;
        auto& n0 = initPar.n0;
        auto& cni = initPar.cni;
        auto& nrst = initPar.nrst;
        auto& Nmin = initPar.Nmin;
        auto& N_init = initPar.N_init;
        auto& N = initPar.N;

        auto& Asize_max = jsoPar.Asize_max;
        auto& A = jsoPar.A;

        auto rnd = m_random.get();
        auto& eval_fun = m_eval_fun;

        while (!terminating()) {
            int hh(0);
            double p_min(0);
            // 需要实现 roulete 函数来确定 hh 和 p_min
            {
                auto ret = roulete(ni, rnd);
                hh = ret.first;
                p_min = ret.second;
            }
            //[hh, p_min] = roulete(ni);

            if (p_min < delta) {
                for (int i = 0; i < h; ++i) {
                    cni[i] += ni[i] - n0;
                }
                for (int i = 0; i < h; ++i) {
                    ni[i] = n0;
                }
                nrst++;
            }

            if (hh == 0) {
                generation_cobide(hh, initPar, codiPar, m_pop, eval_fun, env, rnd);
            }
            else if (hh == 1) {
                generation_IDE(hh, initPar, idePar, codiPar, m_pop, eval_fun, env, rnd);
            }
            else if (hh == 2) {
                generation_CMAES(hh, initPar, selectPar, adaPar, codiPar, cmeasPar, m_pop, eval_fun, env, rnd);
            }
            else if (hh == 3) {
                generation_jSO(hh, initPar, codiPar, jsoPar, m_pop, eval_fun, env, rnd);
            }


            updateBestSoFar(bsf_fit_var, m_pop);

            updateDatum(env);
            int optN = std::round((((Nmin - N_init) / static_cast<double>(maxFES)) * evaluations()) + N_init);
            if (N > optN) {
                int diffPop = N - optN;
                if ((N - diffPop) < Nmin)
                    diffPop = N - Nmin;
                N = N - diffPop;


                std::vector<int> sortedIds(m_pop.size(), 0);
                for (int idx(0); idx < sortedIds.size(); ++idx) {
                    sortedIds[idx] = idx;
                }

                std::sort(sortedIds.begin(), sortedIds.end(), [&](int a, int b) {
                    return m_pop[a]->fitness() > m_pop[b]->fitness();
                    });
                auto& CBF = codiPar.CBF;
                auto& CBCR = codiPar.CBCR;
                sortVector(m_pop, sortedIds);
                sortVector(CBF, sortedIds);
                sortVector(CBCR, sortedIds);

                m_pop.resize(N);
                CBF.resize(N);
                CBCR.resize(N);
                selectPar.initalize(N);

                Asize_max = round(N * 2.6);
                if (A.size() > Asize_max) {
                    rnd->uniform.shuffle(A.begin(), A.end());
                    A.resize(Asize_max);
                }

                selectPar.initalize(N);
                //cni = cni + ni - n0;
            }
        }

	}

    void EA4eig::updateDatum(Environment* env) {

#ifdef OFEC_DATUM_MULTI_POP_H
        g_multi_pop.pops.clear();
        g_multi_pop.pops.resize(1);
        for (size_t i = 0; i < m_pop.size(); ++i) {
            g_multi_pop.pops[0].push_back(m_pop[i].get());
        }
        datumUpdated(env, g_multi_pop);
#endif
    }

	void EA4eig::initPop(Environment* env) {

        ofec::Continuous* con_pro = dynamic_cast<ofec::Continuous*>(env->problem());
        m_pop.resize(initPar.N);
        for (int idx(0); idx < m_pop.size(); ++idx) {
            m_pop[idx].reset(new Continuous::SolutionType(con_pro->numberObjectives(),con_pro->numberConstraints(),con_pro->numberVariables()));
            m_pop[idx]->type() = idx;
            m_pop[idx]->initialize(env, m_random.get());
            m_eval_fun(*m_pop[idx], env);
        }
       // auto con_pro = CAST_CONOP(env->problem());
        for (auto& it : m_pop) {
            if (con_pro->boundaryViolated(*it)) {
                std::cout << "initialize error" << std::endl;
                break;
            }
        }
        

        updateDatum(env);
	}
}
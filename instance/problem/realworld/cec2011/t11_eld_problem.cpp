#include "t11_eld_problem.h"
#include <algorithm>

namespace ofec::cec2011 {
    void ELDproblem::initialize_(Environment* env) {
        Continuous::initialize_(env);
        resizeVariable(120);

        {
            Eigen::Matrix<double, 5, 1> Pmin;
            Eigen::Matrix<double, 5, 1> Pmax;

            Pmin << 10, 20, 30, 40, 50;


            Pmax << 75, 125, 175, 250, 300;

            for (int i = 0; i < 24; ++i) {
                Lower_Limit.col(i) = Pmin;
                Upper_Limit.col(i) = Pmax;
            }


            for (int i = 0; i < No_of_Load_Hours; ++i) {
                for (int j = 0; j < No_of_Units; ++j) {
                    m_domain.setRange(Lower_Limit(j, i), Upper_Limit(j, i), j * No_of_Load_Hours + i);
                    //Input_Generations(i, j) = input_vector(j * No_of_Load_Hours + i);
                }
            }
        }

        resizeObjective(1);
        m_optimize_mode[0] = OptimizeMode::kMinimize;





        Power_Demand = Eigen::VectorXd(No_of_Load_Hours);
        Data1 = Eigen::MatrixXd(No_of_Units, 7);
        Data2 = Eigen::MatrixXd(No_of_Units, 7);

        Power_Demand << 410, 435, 475, 530, 558, 608, 626, 654, 690, 704, 720, 740, 704, 690, 654, 580, 558, 608, 654, 704, 680, 605, 527, 463;
        Data1 << 10, 75, 0.0080, 2.0, 25, 100, 0.042,
            20, 125, 0.0030, 1.8, 60, 140, 0.040,
            30, 175, 0.0012, 2.1, 100, 160, 0.038,
            40, 250, 0.0010, 2.0, 120, 180, 0.037,
            50, 300, 0.0015, 1.8, 40, 200, 0.035;


        Data2 << std::numeric_limits<double>::quiet_NaN(), 30, 30, 10, 10, 10, 10,
            std::numeric_limits<double>::quiet_NaN(), 30, 30, 20, 20, 20, 20,
            std::numeric_limits<double>::quiet_NaN(), 40, 40, 30, 30, 30, 30,
            std::numeric_limits<double>::quiet_NaN(), 50, 50, 40, 40, 40, 40,
            std::numeric_limits<double>::quiet_NaN(), 50, 50, 50, 50, 50, 50;

        B1 = Eigen::MatrixXd(No_of_Units, No_of_Units);
        B1 << 0.000049, 0.000014, 0.000015, 0.000015, 0.000020,
            0.000014, 0.000045, 0.000016, 0.000020, 0.000018,
            0.000015, 0.000016, 0.000039, 0.000010, 0.000012,
            0.000015, 0.000020, 0.000010, 0.000040, 0.000014,
            0.000020, 0.000018, 0.000012, 0.000014, 0.000035;


        B2 = Eigen::VectorXd(No_of_Units);
        B2.setZero();
        B3 = 0;







    }

    void ELDproblem::evaluateObjective(Real* tmp_x, std::vector<Real>& obj)const {



        Eigen::VectorXd Pmin;
        Eigen::VectorXd Pmax;
        Eigen::VectorXd a;
        Eigen::VectorXd b;
        Eigen::VectorXd c;
        Eigen::VectorXd e;
        Eigen::VectorXd f;
        Eigen::VectorXd Previous_Generations;
        Eigen::VectorXd Up_Ramp;
        Eigen::VectorXd Down_Ramp;
        Eigen::MatrixXd Prohibited_Operating_Zones_POZ;
        int No_of_POZ_Limits;
        Eigen::MatrixXd POZ_Lower_Limits;
        Eigen::MatrixXd POZ_Upper_Limits;



        Eigen::VectorXd Power_Balance_Penalty;
        Eigen::VectorXd Capacity_Limits_Penalty;
        Eigen::MatrixXd Up_Ramp_Limit;
        Eigen::MatrixXd Down_Ramp_Limit;
        Eigen::VectorXd Ramp_Limits_Penalty;
        Eigen::VectorXd POZ_Penalty;
        Eigen::VectorXd All_Penalty;
        Eigen::VectorXd Current_Cost;
        Eigen::VectorXd Power_Loss;



        Pmin = Data1.col(0);
        Pmax = Data1.col(1);
        a = Data1.col(2);
        b = Data1.col(3);
        c = Data1.col(4);
        e = Data1.col(5);
        f = Data1.col(6);
        Previous_Generations = Data2.col(0);
        Up_Ramp = Data2.col(1);
        Down_Ramp = Data2.col(2);
        Prohibited_Operating_Zones_POZ = Data2.block(0, 3, No_of_Units, Data2.cols() - 3);
        No_of_POZ_Limits = Prohibited_Operating_Zones_POZ.cols();
        POZ_Lower_Limits = Eigen::MatrixXd(No_of_POZ_Limits / 2, No_of_Units);
        POZ_Upper_Limits = Eigen::MatrixXd(No_of_POZ_Limits / 2, No_of_Units);
        for (int i = 0; i < No_of_POZ_Limits / 2; ++i) {
            POZ_Lower_Limits.row(i) = Prohibited_Operating_Zones_POZ.col(2 * i);
            POZ_Upper_Limits.row(i) = Prohibited_Operating_Zones_POZ.col(2 * i + 1);
        }

        Power_Balance_Penalty = Eigen::VectorXd(No_of_Load_Hours);

        Capacity_Limits_Penalty = Eigen::VectorXd(No_of_Load_Hours);

        Up_Ramp_Limit = Eigen::MatrixXd(No_of_Load_Hours, No_of_Units);

        Down_Ramp_Limit = Eigen::MatrixXd(No_of_Load_Hours, No_of_Units);

        Ramp_Limits_Penalty = Eigen::VectorXd(No_of_Load_Hours);

        Capacity_Limits_Penalty = Eigen::VectorXd(No_of_Load_Hours);

        POZ_Penalty = Eigen::VectorXd(No_of_Load_Hours);

        All_Penalty = Eigen::VectorXd(No_of_Load_Hours);

        Current_Cost = Eigen::VectorXd(No_of_Load_Hours);

        Power_Loss = Eigen::VectorXd(No_of_Load_Hours);

        std::vector<double> x(m_number_variables);
        for (int idx(0); idx < m_number_variables; ++idx) {
            //	x1[idx] = tmp_x[idx];
            x[idx] = round(tmp_x[idx] * 10000) / 10000;// For fixing the 4 digit precision
        }

        auto x_ = x.data();

        Power_Balance_Penalty.setZero();
        Capacity_Limits_Penalty.setZero();
        Up_Ramp_Limit.setZero();
        Down_Ramp_Limit.setZero();

        Ramp_Limits_Penalty.setZero();
        POZ_Penalty.setZero();
        All_Penalty.setZero();
        Current_Cost.setZero();
        Power_Loss.setZero();


        Eigen::MatrixXd Input_Generations(No_of_Load_Hours, No_of_Units);

        for (int i = 0; i < No_of_Load_Hours; ++i) {
            for (int j = 0; j < No_of_Units; ++j) {
                Input_Generations(i, j) = x_[j * No_of_Load_Hours + i];
            }
        }

        // Input_Generations = Input_Generations.transpose();
        Eigen::Vector3d result;


        for (int j = 0; j < No_of_Load_Hours; ++j) {
            Eigen::VectorXd x = Input_Generations.row(j).transpose();
            double powerLoss = x.transpose() * B1 * x + B2.dot(x) + B3;
            Power_Loss(j) = std::round(powerLoss * 10000) / 10000;

            // Power Balance Penalty Calculation
            Power_Balance_Penalty(j) = std::abs(Power_Demand(j) + Power_Loss(j) - x.sum());


            //auto a1 = (x - Pmin).cwiseAbs();
            //auto b1 = a1.cwiseMax(x - Pmin).sum();;
            //// Capacity Limits Penalty Calculation

            for (int idx(0); idx < No_of_Units; ++idx) {
                Capacity_Limits_Penalty(j) += abs(x(idx) - Pmin(idx)) - (x(idx) - Pmin(idx));
                Capacity_Limits_Penalty(j) += abs(Pmax(idx) - x(idx)) - (Pmax(idx) - x(idx));

            }

            //   Capacity_Limits_Penalty(j) = ((x - Pmin).cwiseAbs()-(x - Pmin)).sum() + ((Pmax - x).cwiseAbs()-(Pmax - x)).sum();

               // Ramp Rate Limits Penalty Calculation
            if (j > 0) {
                Up_Ramp_Limit.row(j) = Pmax.cwiseMin(Previous_Generations + Up_Ramp);
                Down_Ramp_Limit.row(j) = Pmin.cwiseMax(Previous_Generations - Down_Ramp);
                Eigen::VectorXd temp = x.transpose() - Down_Ramp_Limit.row(j);
                Eigen::VectorXd temp2 = Up_Ramp_Limit.row(j) - x.transpose();

                //for (int idx(0); idx < No_of_Units; ++idx) {
                //    Ramp_Limits_Penalty(j) += abs(temp(idx)) - (temp(idx));
                //    Ramp_Limits_Penalty(j) += abs(temp2(idx)) - (temp2(idx));
                //    //Ramp_Limits_Penalty(j) += abs(Pmax(idx) - x(idx)) - (Pmax(idx) - x(idx));

                //}


                Ramp_Limits_Penalty(j) = (temp.cwiseAbs() - temp).sum() + (temp2.cwiseAbs() - temp2).sum();
            }
            Previous_Generations = x;

            // Prohibited Operating Zones Penalty Calculation
            Eigen::MatrixXd temp_x(No_of_POZ_Limits / 2, No_of_Units);
            temp_x = x.transpose().replicate(No_of_POZ_Limits / 2, 1);

            Eigen::MatrixXd tmpValue = (temp_x - POZ_Lower_Limits).cwiseMin(POZ_Upper_Limits - temp_x);
            Eigen::MatrixXd penalty_matrix = ((POZ_Lower_Limits.array() < temp_x.array()) && (temp_x.array() < POZ_Upper_Limits.array())).cast<double>();


            penalty_matrix = penalty_matrix.cwiseProduct(tmpValue);
            POZ_Penalty(j) = penalty_matrix.sum();


            // Eigen::VectorXd a = f.array()* (Pmin.array() - x.array());
             // Cost Calculation
            Current_Cost(j) = (a.array() * x.array().square() + b.array() * x.array() + c.array() + (e.array() * (f.array() * (Pmin.array() - x.array())).sin()).abs()).sum();
        }

        // All & Total Penalty Calculation
        All_Penalty = 1e3 * Power_Balance_Penalty + 1e3 * Capacity_Limits_Penalty + 1e5 * Ramp_Limits_Penalty + 1e5 * POZ_Penalty;
        double Total_Penalty = All_Penalty.sum();
        double Total_Cost = Current_Cost.sum();
        double Total_Value = Total_Cost + Total_Penalty;

        result << Total_Value, Total_Cost, Total_Penalty;

        obj[0] = Total_Value;
    }
}
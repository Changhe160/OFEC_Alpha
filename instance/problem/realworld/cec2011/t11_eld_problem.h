/********* Begin Register Information **********
{
	"dependency on libraries": [ "Eigen" ],
	"name": "RW-CEC2011-T11",
	"identifier": "RW_CEC2011_T11",
	"tags": [ "continuous", "single-objective" ]
}
*********** End Register Information **********/


// only for function idx=  1

#ifndef OFEC_RW_CEC2011_T11_H
#define OFEC_RW_CEC2011_T11_H

#include "../../../../core/problem/continuous/continuous.h"
#include <Eigen/Dense>


namespace ofec {
	namespace cec2011 {
		class ELDproblem : virtual public Continuous {

			OFEC_CONCRETE_INSTANCE(ELDproblem)
		protected:
			void addInputParameters() {}
			virtual void initialize_(Environment* env) override;
			void evaluateObjective(Real* x, std::vector<Real>& obj) const override;

		//	void evaluateObjective(Real* x, std::vector<Real>& obj) const;
		private:
			Eigen::Matrix<double, 5, 24> Lower_Limit = Eigen::Matrix<double, 5, 24>::Constant(0);
			Eigen::Matrix<double, 5, 24> Upper_Limit = Eigen::Matrix<double, 5, 24>::Constant(0);

			const int No_of_Load_Hours = 24;
			const int No_of_Units = 5;




			Eigen::VectorXd Power_Demand;
			Eigen::MatrixXd Data1;
			Eigen::MatrixXd Data2;

			Eigen::MatrixXd B1;
			Eigen::VectorXd B2;
			double B3 = 0;

		};
	}
	using RW_CEC2011_T11 = cec2011::ELDproblem;
}

#endif // !OFEC_RW_CEC2011_T10_H

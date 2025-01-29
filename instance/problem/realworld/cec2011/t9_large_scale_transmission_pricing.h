/********* Begin Register Information **********
{
	"name": "RW-CEC2011-T09",
	"identifier": "RW_CEC2011_T9",
	"tags": [ "continuous", "single-objective" ],
	"dependency on libraries": [ "Eigen" ]
}
*********** End Register Information **********/

#ifndef OFEC_RW_CEC2011_T9_H
#define OFEC_RW_CEC2011_T9_H

#include "../../../../core/problem/continuous/continuous.h"
#include <complex>

namespace ofec {
	namespace cec2011 {
		class LargeScaleTransmissionPricing : virtual public Continuous {
			OFEC_CONCRETE_INSTANCE(LargeScaleTransmissionPricing)

		protected:
			struct LineData {
				int firstNumber;
				int secondNumber;
				std::complex<double> complexValue;
				double fourthValue;
				int fifthValue;
				double sixthValue;
			};

		protected:
			virtual void initialize_(Environment* env) override;
			void addInputParameters();
			void evaluateObjective(Real* x, std::vector<Real>& obj)const override;

			void readData();
			void readEBEdata(std::vector<std::vector<double>>& ebeData);
			void readLineData(std::vector<LineData>& linedata);

			void formybus(const std::vector<LineData>& linedata, int n, std::vector<std::vector<double>>& YIbus);

			void calculateGD(Real* x, std::vector<std::vector<double>>& GD)const;

			double calculateObjective(const std::vector<std::vector<double>>& GD)const;
			double calculateViolations(const std::vector<std::vector<double>>& GD)const;
			

		protected:

			double Kp = 100;
			std::vector<std::vector<double>> m_GD_max;
			std::vector<std::vector<double>> BT;

			std::vector<int> g;
			std::vector<int> d;

			std::vector<double> Pg;
			std::vector<double> Pd;

			std::vector<double> Pg2;
			std::vector<double> Pd2;


			std::vector<int> na;
			std::vector<int> nb;
			std::vector<double> xij;
			std::vector<std::vector<std::vector<double>>> PTDF;

		};
	}
	using RW_CEC2011_T9 = cec2011::LargeScaleTransmissionPricing;
}

#endif // !OFEC_RW_CEC2011_T2_H

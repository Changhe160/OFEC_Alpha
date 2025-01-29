#include "cassini2.h"
#include "gtoptoolbox/trajobjfuns.h"
#include <fstream>

namespace ofec::trajectory {
	void Cassini2::initialize_(Environment* env) {
		Continuous::initialize_(env);
		resizeVariable(22);
		m_domain.setRange(-1000, 0, 0);
		m_domain.setRange(3, 5, 1);
		m_domain.setRange(0, 1, 2);
		m_domain.setRange(0, 1, 3);
		m_domain.setRange(100, 400, 4);
		m_domain.setRange(100, 500, 5);
		m_domain.setRange(30, 300, 6);
		m_domain.setRange(400, 1600, 7);
		m_domain.setRange(800, 2200, 8);
		for (size_t i = 9; i < 14; ++i)
			m_domain.setRange(0.01, 0.9, i);
		for (size_t i = 14; i < 18; ++i)
			m_domain.setRange(1.05, 6, i);
		m_domain.setRange(1.15, 6.5, 16);
		m_domain.setRange(1.7, 291, 17);
		for (size_t i = 18; i < 22; ++i)
			m_domain.setRange(-OFEC_PI, OFEC_PI, i);
		resizeObjective(1);
		m_optimize_mode[0] = OptimizeMode::kMinimize;







	}
	void Cassini2::updateOptima(Environment* env)  {
		Continuous::updateOptima(env);
		std::vector<Real> test_x = { -779.046753814506, 3.25911446832345, 0.525976214695235, 0.38086496458657, 167.378952534645, 424.028254165204, 53.2897409769205, 589.766954923325, 2200, 0.769483451363201, 0.513289529822621, 0.0274175362264024, 0.263985256705873, 0.599984695281461, 1.34877968657176, 1.05, 1.30730278372017, 69.8090142993495, -1.5937371121191, -1.95952512232447, -1.55498859283059, -1.5134625299674 };
		std::vector<Real> test_y(1);
		evaluateObjective(test_x.data(), test_y);
		auto obj = test_y[0];
		Solution<> temp_sol(m_number_objectives, m_number_constraints, m_number_variables);
		temp_sol.variable().vector() = test_x;
		evaluateObjective(temp_sol.variable().vector().data(), temp_sol.objective());
		dynamic_cast<Optima<>&>(*m_optima).appendSolution(temp_sol);

	}

	void Cassini2::evaluateObjective(Real *x, std::vector<Real> &obj)const {
		std::vector<double> vx(x, x + m_number_variables);
		obj[0] = gtop_toolbox::cassini2(vx);
		//bool errorFlag = false;
		//try {
		//	obj[0] = gtop_toolbox::cassini2(vx);
		//}
		//catch (std::exception e) {
		//	errorFlag = true;
		//	std::ofstream out("Cassini2_error.txt");
		//	for (int idx(0); idx < vx.size(); ++idx) {
		//		out << vx[idx] << "\t";
		//	}
		//	out << std::endl;
		//	out << e.what() << std::endl;
		//	out.close();
		//}

		//if (errorFlag) {
		//	throw Exception("error at Cassini2: wrong evaluate\n");
		//}
	}
}
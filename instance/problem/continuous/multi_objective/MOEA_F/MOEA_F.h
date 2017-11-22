#ifndef MOEA_FBASE_H
#define MOEA_FBASE_H

#include "../../../../../core/problem/continuous/continuous.h"

namespace OFEC {
	class MOEA_FBase : public continuous {
	protected:
		MOEA_FBase(const std::string &name, size_t size_var, size_t size_obj);
		int getDtype() const { return m_dtype; }
		int getPtype() const { return m_ptype; }
		int getLtype() const { return m_ltype; }
		// control the PF shape
		void alphafunction(double alpha[], double const *x, int dim, int type);
		// control the distance
		double betafunction(const std::vector<double> & x, int type);
		// control the PS shape of 2-d instances
		double psfunc2(const double &x, const double &t1, size_t dim, int type, int css);
		// control the PS shapes of 3-D instances
		double psfunc3(const double &x, const double &t1, const double &t2, int dim, int type);
		void calObjective(double *x, std::vector <double> &y_obj);
		void LoadPF();
		void evaluate__(double *x, std::vector<double>& obj) { calObjective(x, obj); }
		int m_dtype, m_ptype, m_ltype;
	};
}
#endif //MOEA_FBASE_H
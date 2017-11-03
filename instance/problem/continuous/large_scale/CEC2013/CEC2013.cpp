#include "CEC2013.h"

namespace OFEC {
	CEC2013::CEC2013(const std::string &name, size_t size_var, size_t size_obj) : problem(name, size_var, size_obj), continuous(name, size_var, size_obj) \
	{
		m_variable_accuracy=1.0e-6;
	}
	
	CEC2013::~CEC2013() {
		// delete[] anotherz;
		// delete[] anotherz1;
		// delete[] anotherz2;
	}

	evaluation_tag CEC2013::evaluate_(base &s, caller call, bool effective_fes, bool constructed) {
		variable<real> &x = dynamic_cast< solution<variable<real>, real> &>(s).get_variable();
		auto & obj = dynamic_cast< solution<variable<real>, real> &>(s).get_objective();

		std::vector<real> x_(x.begin(), x.end()); //for parallel running

		evaluate__(x_.data(), obj);
		
		if (constructed) {
			if (effective_fes)		m_effective_eval++;

			if (m_variable_monitor) {
				m_optima.is_optimal_variable(dynamic_cast<solution<variable<real>, real> &>(s), m_variable_accuracy);
				if (m_optima.is_variable_found())
					m_solved = true;
			}
			if (m_objective_monitor) {
				m_optima.is_optimal_objective(obj, m_objective_accuracy);
				if (m_optima.is_objective_found())
					m_solved = true;
			}
			if (call == caller::Algorithm&& global::ms_global->m_algorithm&&global::ms_global->m_algorithm->terminating())
				return evaluation_tag::Terminate;

			//if (mode == Program_Algorithm&&Global::msp_global->mp_problem && !Global::msp_global->mp_problem->isProTag(MOP)) m_globalOpt.isFound(s, m_disAccuracy, m_accuracy);
			//if (Global::msp_global != nullptr&&Global::msp_global->mp_algorithm != nullptr&&Global::msp_global->mp_algorithm->ifTerminating()) { return Return_Terminate; }
		}
		return evaluation_tag::Normal;
	}

	void CEC2013::set_original_global_opt(real *opt) {
		if (m_objective_size > 1) throw myexcept("CEC2013::set_original_global_opt only for problems with a single obj");
		variable<real> temp_var(m_variable_size);
		if (opt == 0)		for (auto&i : temp_var) i = 0.;
		else	for (int i = 0; i < m_variable_size; i++)  temp_var[i] = opt[i];
		m_original_global_opt.append(std::move(temp_var));

		objective<real> temp_obj(m_objective_size);
		solution<variable<real>, real> temp(m_original_global_opt.variable(0), std::move(temp_obj));

		evaluate_(temp, caller::Problem,false,false);
		m_original_global_opt.append(std::move(temp.get_objective()));
	}
	void CEC2013::set_global_opt(real *tran) {
		if (m_objective_size > 1) throw myexcept("CEC2013::set_global_opt only for problems with a single obj");
		variable<real> temp_var(m_variable_size);
		for (int i = 0; i < m_variable_size; ++i) 
			temp_var[i] = tran[i];
		m_optima.append(std::move(temp_var));

		objective<real> temp_obj(m_objective_size);
		solution<variable<real>, real> temp(m_optima.variable(0), std::move(temp_obj));
		
		evaluate_(temp, caller::Problem,false,false);
		m_optima.append(std::move(temp.get_objective()));
	}

	real* CEC2013::readOvector()
	{
		// read O vector from file in csv format
		real* d = new real[m_variable_size];
		std::stringstream ss;
		ss << "C:/Users/lenovo/Documents/GitHub/OFEC_Alpha/instance/problem/continuous/global/CEC2013/data/" << "F" << ID << "-xopt.txt";
		std::ifstream file(ss.str());
		std::string value;
		std::string line;
		int c = 0;

		if (file.is_open())
		{
			std::stringstream iss;
			while (getline(file, line))
			{
				iss << line;
				while (getline(iss, value, ','))
				{
					d[c++] = stod(value);
				}
				iss.clear();
				//iss.str("");
				// if (c==dimension)
				//   {
				//     break;
				//   }
				// printf("%d\n",c);
			}
			file.close();
			//file.clear();
		}
		else
		{
			std::cout << "readOvector: Cannot open data" << std::endl;
		}
		return d;
	}

	real** CEC2013::readOvectorVec()
	{
		// read O vector from file in csv format, seperated by s_size groups
		real** d = (real**)malloc(m_nonSeparableGroupNumber*sizeof(real*));
		std::stringstream ss;
		ss << "C:/Users/lenovo/Documents/GitHub/OFEC_Alpha/instance/problem/continuous/global/CEC2013/data/" << "F" << ID << "-xopt.txt";
		std::ifstream file(ss.str());
		std::string value;
		std::string line;
		size_t c = 0;                      // index over 1 to dim
		int i = -1;                      // index over 1 to s_size
		size_t up = 0;                   // current upper bound for one group

		if (file.is_open())
		{
			std::stringstream iss;
			while (getline(file, line))
			{
				if (c == up)             // out (start) of one group
				{
					// printf("=\n");
					++i;
					d[i] = (real*)malloc(mp_s[i] * sizeof(real));
					up += mp_s[i];
				}
				iss << line;
				while (getline(iss, value, ','))
				{
					// printf("c=%d\ts=%d\ti=%d\tup=%d\tindex=%d\n",c,s[i],i,up,c-(up-s[i]));
					d[i][c - (up - mp_s[i])] = stod(value);
					// printf("1\n");
					c++;
				}
				iss.clear();
				//iss.str("");
				// printf("2\n");
			}
			file.close();
			//file.clear();
		}
		else
		{
			std::cout << "readOvectorVec: Cannot open data" << std::endl;
		}
		return d;
	}

	void CEC2013::create_shifted_vector(std::vector<real> &vec) {

		double hw, middle;
		real s, max, min;
		for (int i = 0; i < m_variable_size; i++) {
			max = m_domain[i].limit.second;
			min = m_domain[i].limit.first;
			hw = (0.5 * (max - min));
			middle = (min + hw);
			do {
				s = (middle + (global::ms_global->m_normal[caller::Problem]->next() * hw));
			} while ((s < min) || (s > max));
			vec[i] = s;
		}
	}

	void CEC2013::transform_osz(real* z, size_t dim)
	{
		// apply osz transformation to z
		for (int i = 0; i < dim; ++i)
		{
			z[i] = sign(z[i]) * exp(hat(z[i]) + 0.049 * (sin(c1(z[i]) * hat(z[i])) + sin(c2(z[i])* hat(z[i]))));
		}
	}

	void CEC2013::transform_asy(real* z, double beta, size_t dim)
	{
		for (int i = 0; i < dim; ++i)
		{
			if (z[i] > 0)
			{
				z[i] = pow(z[i], 1 + beta * i / ((double)(dim - 1)) * sqrt(z[i]));
			}
		}
	}

	void CEC2013::lambda(real* z, double alpha, size_t dim)
	{
		for (int i = 0; i < dim; ++i)
		{
			z[i] = z[i] * pow(alpha, 0.5 * i / ((double)(dim - 1)));
		}
	}

	size_t* CEC2013::readPermVector() {
		size_t* d;

		d = new size_t[m_variable_size];

		std::stringstream ss;
		ss << "C:/Users/lenovo/Documents/GitHub/OFEC_Alpha/instance/problem/continuous/global/CEC2013/data/" << "F" << ID << "-p.txt";
		std::ifstream file(ss.str());
		size_t c = 0;
		std::string value;

		if (file.is_open())
		{
			while (getline(file, value, ','))
			{
				d[c++] = stoi(value) - 1;
			}
			file.close();
			//file.clear();
		}

		// for (int i = 0; i < m_variable_size; ++i)
		//  {
		//    printf("%d\n", d[i]);
		// }

		return(d);
	}

	void CEC2013::create_random_permutation(std::vector<size_t> &vec) {
		size_t i, t;
		int k, j;
		size_t val = 0;
		for (auto &m : vec) {
			m = val++;
		}
		
		for (i = (m_variable_size << 3); i >= 0; --i) {
			j = global::ms_global->m_uniform[caller::Problem]->next_non_standard(0, (int)(m_variable_size-1));
			do {
				k = global::ms_global->m_uniform[caller::Problem]->next_non_standard(0, (int)(m_variable_size - 1));
			} while (k == j);
			t = vec[j];
			vec[j] = vec[k];
			vec[k] = t;
		}
	}

	//Create a random rotation matrix
	void CEC2013::create_rotated_matrix(size_t dim, matrix & mat) {
		double sum = 0;
		double temp = 0;
		matrix M(dim);
		// initialize
		for (size_t i = 0; i < dim; ++i) {
			for (size_t j = 0; j < dim; ++j) {
				M[i][j] = global::ms_global->m_normal[caller::Problem]->next();
			}
		}

		// main loop of gram/schmidt
		for (size_t i = (dim - 1); i >= 0; --i) {

			//
			for (size_t j = (dim - 1); j > i; --j) {

				// dot product
				sum = 0;
				for (size_t k = (dim - 1); k >= 0; --k) {
					sum += (M[i][k] * M[j][k]);
				}

				// subtract
				for (size_t k = (dim - 1); k >= 0; --k) {
					M[i][k] -= (sum * M[j][k]);
				}
			}

			// normalize
			sum = 0;
			for (size_t k = (dim - 1); k >= 0; --k) {
				temp = M[i][k];
				sum += (temp * temp);
			}

			// linear dependency -> restart
			if (sum <= 0) {
				continue;
			}
			sum = (1. / sqrt(sum));

			for (size_t k = (dim - 1); k >= 0; --k) {
				M[i][k] *= sum;
			}
		}
		mat = M;
	}


	/**
	* Create a random rotation matrix
	*/
	void CEC2013::create_rotated_matrix_1D(matrix & mat, std::vector<real> & vec) {
		int k = 0;
		size_t row = mat.data().size();
		size_t col = mat.data()[0].size();
		for (size_t i = 0; i < row; ++i) {
			for (size_t j = 0; j < col; ++j) {
				vec[k++] = mat[i][j];
			}
		}
	}

	/*
	* create several 1-D rotation matrix randomly
	*/
	void CEC2013::create_multi_rotated_matrix_1D(size_t dim, size_t num, std::vector<std::vector<real>> &vvec) {
		size_t i;
		vvec.resize(num, std::vector<real>(dim));
		matrix mat(dim);
		std::vector<real> vec(dim*dim);
		/*  allocate storage for an array of pointers */
		//a =(double **) malloc(num * sizeof(double *));

		/* for each pointer, allocate storage for an array of ints */
		for (i = 0; i < num; i++) {
			create_rotated_matrix(dim, mat);
			create_rotated_matrix_1D(mat, vec);
			vvec[i] = vec;
		}
	}

	real** CEC2013::readR(size_t sub_dim)
	{
		real** m;

		m = new real*[sub_dim];
		for (int i = 0; i < sub_dim; i++)
		{
			m[i] = new real[sub_dim];
		}

		std::stringstream ss;
		ss << "C:/Users/lenovo/Documents/GitHub/OFEC_Alpha/instance/problem/continuous/global/CEC2013/data/" << "F" << ID << "-R" << sub_dim << ".txt";
		// cout<<ss.str()<<endl;

		std::ifstream file(ss.str());
		std::string value;
		std::string line;
		int i = 0;
		int j;

		if (file.is_open())
		{
			std::stringstream iss;
			while (getline(file, line))
			{
				j = 0;
				iss << line;
				while (getline(iss, value, ','))
				{
					// printf("%d,%d\t%f\n", i,j, stod(value));
					m[i][j] = stod(value);
					// printf("done\n");
					j++;
				}
				iss.clear();
				//iss.str("");
				i++;
			}
			file.close();
			//file.clear();
		}
		else
		{
			std::cout << "Cannot open datafiles" << std::endl;
		}
		return m;
	}

	size_t* CEC2013::getS() {
		return mp_s;
	}

	size_t* CEC2013::readS(size_t num)
	{
		mp_s = new size_t[num];

		std::stringstream ss;
		ss << "C:/Users/lenovo/Documents/GitHub/OFEC_Alpha/instance/problem/continuous/global/CEC2013/data/" << "F" << ID << "-s.txt";
		std::ifstream file(ss.str());
		int c = 0;
		std::string value;
		if (file.is_open())
		{
			while (getline(file, value))
			{
				// cout<<stod(value)<<endl;
				mp_s[c++] = stoi(value);
			}
			file.close();
			//file.clear();
		}
		return mp_s;
	}

	double* CEC2013::readW(size_t num)
	{
		mp_w = new double[num];

		std::stringstream ss;
		ss << "C:/Users/lenovo/Documents/GitHub/OFEC_Alpha/instance/problem/continuous/global/CEC2013/data/" << "F" << ID << "-w.txt";
		std::ifstream file(ss.str());
		int c = 0;
		std::string value;
		if (file.is_open())
		{
			while (getline(file, value))
			{
				// cout<<stod(value)<<endl;
				mp_w[c++] = stod(value);
			}
			file.close();
			//file.clear();
		}

		return mp_w;
	}

	real* CEC2013::rotate_vector(size_t i, size_t &c)
	{
		real* z = new real[mp_s[i]];
		// cout<<"s "<<s[i]<<endl;

		// copy values into the new vector
		for (size_t j = c; j < c + mp_s[i]; ++j)
		{
			// cout<<"j-c "<<j-c<<" p "<<mp_Pvector[j]<<endl;
			z[j - c] = mp_anotherz[mp_Pvector[j]];
		}
		// cout<<"copy done"<<endl;

		if (mp_s[i] == 25)
		{
			mp_anotherz1 = multiply(z, mpp_r25, mp_s[i]);
		}
		else if (mp_s[i] == 50)
		{
			mp_anotherz1 = multiply(z, mpp_r50, mp_s[i]);
		}
		else if (mp_s[i] == 100)
		{
			mp_anotherz1 = multiply(z, mpp_r100, mp_s[i]);
		}
		else
		{
			std::cout << "size of rotation matrix out of range" << std::endl;
		}
		delete[]z;
		z = nullptr;
		c = c + mp_s[i];
		return mp_anotherz1;
	}

	real* CEC2013::rotate_vector_conform(size_t i, size_t &c)
	{
		real* z = new real[mp_s[i]];
		// printf("i=%d\tc=%d\tl=%d\tu=%d\n", i, c, c - (i)*overlap, c +s[i] - (i)*overlap);

		// copy values into the new vector
		for (size_t j = c - i*m_overlap; j < c + mp_s[i] - i*m_overlap; ++j)
		{
			// cout<<"j-c "<<j-c<<" p "<<Pvector[j]<<endl;
			z[j - (c - i*m_overlap)] = mp_anotherz[mp_Pvector[j]];
		}
		// cout<<"copy done"<<endl;
		if (mp_s[i] == 25)
		{
			mp_anotherz1 = multiply(z, mpp_r25, mp_s[i]);
		}
		else if (mp_s[i] == 50)
		{
			mp_anotherz1 = multiply(z, mpp_r50, mp_s[i]);
		}
		else if (mp_s[i] == 100)
		{
			mp_anotherz1 = multiply(z, mpp_r100, mp_s[i]);
		}
		else
		{
			std::cout << "size of rotation matrix out of range" << std::endl;
		}
		delete[]z;
		z = nullptr;
		c = c + mp_s[i];
		return mp_anotherz1;
	}

	real* CEC2013::rotate_vector_conflict(size_t i, size_t &c, real* x)
	{
		real* z = new real[mp_s[i]];
		// printf("i=%d\tc=%d\tl=%d\tu=%d\n", i, c, c - (i)*overlap, c +s[i] - (i)*overlap);

		// copy values into the new vector
		for (size_t j = c - i*m_overlap; j < c + mp_s[i] - i*m_overlap; ++j)
		{
			// cout<<"j-c "<<j-c<<" p "<<Pvector[j]<<endl;
			z[j - (c - i*m_overlap)] = x[mp_Pvector[j]] - mpp_OvectorVec[i][j - (c - i*m_overlap)];
		}

		// cout<<"copy done"<<endl;
		if (mp_s[i] == 25)
		{
			mp_anotherz1 = multiply(z, mpp_r25, mp_s[i]);
		}
		else if (mp_s[i] == 50)
		{
			mp_anotherz1 = multiply(z, mpp_r50, mp_s[i]);
		}
		else if (mp_s[i] == 100)
		{
			mp_anotherz1 = multiply(z, mpp_r100, mp_s[i]);
		}
		else
		{
			std::cout << "size of rotation matrix out of range" << std::endl;
		}
		delete[]z;
		z = nullptr;
		c = c + mp_s[i];
		return mp_anotherz1;
	}


	// double* CEC2013::lookupprepare(int dim) {
	//   double pownum;
	//   int    i;
	//   double* lookup;
	//   i         = (dim - 1);
	//   pownum    = (1.0 / i);
	//   //lookup    = (double*)malloc(dim * sizeof(double));
	//   lookup    = new double[dim];
	//   lookup[i] = 1.0e6;
	//   lookup[0] = 1.0;

	//   for (--i; i > 0; i--) {
	//     lookup[i] = pow(1.0e6, i * pownum);
	//   }
	//   return lookup;
	// }



	/*
	* Basic Mathematical Functions' Implementation
	*/
	// // elliptic function for F1 ~ F8
	// double CEC2013::elliptic(double*x,int dim) {
	//   double result = 0.0;
	//   int    i;

	//   for(i = dim - 1; i >= 0; i--) {
	//     if (dim == nonSeparableGroupSize){
	//       result += lookup2[i] * x[i] * x[i];
	//     }else{
	//       result += lookup[i] * x[i] * x[i];
	//     }
	//   }
	//   return(result);
	// }

	real CEC2013::elliptic(real *x, size_t dim) {
		real result = 0.0;
		size_t i;

		transform_osz(x, dim);

		// for(i = dim - 1; i >= 0; i--) {
		for (i = 0; i < dim; ++i)
		{
			// printf("%f\n", pow(1.0e6,  i/((double)(dim - 1)) ));
			result += pow(1.0e6, i / ((double)(dim - 1))) * x[i] * x[i];
		}

		return result;
	}



	// // elliptic function for F9 ~ 
	// double CEC2013::elliptic(double*x, int dim, int k) {
	//   double result = 0.0;
	//   int    i;

	//   for(i=dim/k-1;i>=0;i--)
	//     {
	//       result+=lookup2[i]*x[Pvector[dim/k+i]]*x[Pvector[dim/k+i]];
	//     }

	//   return(result);
	// }

	// rastrigin function for F1~F8
	real CEC2013::rastrigin(real *x, size_t dim) {
		real sum = 0;
		size_t i;

		// T_{osz}
		transform_osz(x, dim);

		// T_{asy}^{0.2}
		transform_asy(x, 0.2, dim);

		// lambda
		lambda(x, 10, dim);

		for (i = dim - 1; i >= 0; --i) {
			sum += x[i] * x[i] - 10.0 * cos(2 * OFEC_PI * x[i]) + 10.0;
		}

		return sum;
	}

	// rastrigin function for F9 ~
	real CEC2013::rastrigin(real *x, size_t dim, size_t k)
	{
		real result = 0.0;
		size_t i;
		for (i = dim / k - 1; i >= 0; --i)
		{
			result += x[mp_Pvector[dim / k + i]] * x[mp_Pvector[dim / k + i]] - 10.0*cos(2 * OFEC_PI*x[mp_Pvector[dim / k + i]]) + 10.0;
		}
		return result;
	}

	// ackley function for single group non-separable 
	real CEC2013::ackley(real *x, size_t dim) {
		real sum1 = 0.0;
		real sum2 = 0.0;
		real sum;
		size_t    i;

		// T_{osz}
		transform_osz(x, dim);

		// T_{asy}^{0.2}
		transform_asy(x, 0.2, dim);

		// lambda
		lambda(x, 10, dim);

		for (i = dim - 1; i >= 0; --i) {
			sum1 += (x[i] * x[i]);
			sum2 += cos(2.0 * OFEC_PI * x[i]);
		}

		sum = -20.0 * exp(-0.2 * sqrt(sum1 / dim)) - exp(sum2 / dim) + 20.0 + OFEC_E;
		return sum;
	}

	// ackley function for m-group non-separable 
	real CEC2013::ackley(real *x, size_t dim, size_t k)
	{
		real sum1 = 0.0;
		real sum2 = 0.0;
		real result = 0.0;
		size_t i;

		for (i = dim / k - 1; i >= 0; --i)
		{
			sum1 += x[mp_Pvector[dim / k + i]] * x[mp_Pvector[dim / k + i]];
			sum2 += cos(2.0*OFEC_PI*x[mp_Pvector[dim / k + i]]);
		}

		result = -20.0*exp(-0.2*sqrt(sum1 / (dim / k))) - exp(sum2 / (dim / k)) + 20.0 + OFEC_E;

		return result;
	}

	real* CEC2013::multiply(real *vector, real *matrix, size_t dim) {
		size_t i, j;
		//double*result = (double*)malloc(sizeof(double) * dim);
		real *result = new real[dim];

		for (i = dim - 1; i >= 0; --i) {
			result[i] = 0;

			for (j = dim - 1; j >= 0; j--) {
				result[i] += vector[j] * matrix[dim * j + i];
			}
		}

		return result;
	}

	real* CEC2013::multiply(real *vector, real **matrix, size_t dim) {
		size_t i, j;
		//double*result = (double*)malloc(sizeof(double) * dim);
		real *result = new real[dim];

		for (i = dim - 1; i >= 0; --i) {
			result[i] = 0;

			for (j = dim - 1; j >= 0; --j) {
				result[i] += vector[j] * matrix[i][j];
			}
		}

		return result;
	}


	// // Rotated Elliptic Function for F1 & F4
	// double CEC2013::rot_elliptic(double*x,int dim){
	//   double result = 0.0;
	//   double *z = multiply(x,RotMatrix,dim);

	//   result = elliptic(z,dim);

	//   delete[] z;
	//   return(result);
	// }

	// // Rotated Elliptic Function for F9 & F14
	// double CEC2013::rot_elliptic(double*x,int dim, int k){
	//   double result=0.0;

	//   int i,j;
	//   for(i=dim-1;i>=0;i--)
	//     {
	//       anotherz1[i]=0;
	//       for(j=dim-1;j>=0;j--)
	//         {
	//           anotherz1[i]+=x[Pvector[(k-1)*dim+j]]*RotMatrix[dim*j+i];
	//         }
	//     }
	//   for(i=dim-1;i>=0;i--)
	//     {
	//       result+=lookup[i]*anotherz1[i]*anotherz1[i];
	//     }
	//   return(result);
	// }

	// Rotated Rastrigin Function for F1~F8
	real CEC2013::rot_rastrigin(real *x, size_t dim) {
		real result = 0.0;
		real*z = multiply(x, mp_rot_matrix, dim);
		result = rastrigin(z, dim);

		delete[] z;
		return result;
	}

	// Rotated Rastrigin Function for F9 ~
	real CEC2013::rot_rastrigin(real *x, size_t dim, size_t k)
	{
		real result = 0.0;

		size_t i, j;
		for (i = dim - 1; i >= 0; i--)
		{
			mp_anotherz1[i] = 0;
			for (j = dim - 1; j >= 0; j--)
			{
				mp_anotherz1[i] += x[mp_Pvector[(k - 1)*dim + j]] * mp_rot_matrix[dim*j + i];
			}
		}

		for (i = dim - 1; i >= 0; i--)
		{
			result += mp_anotherz1[i] * mp_anotherz1[i] - 10.0*cos(2 * OFEC_PI*mp_anotherz1[i]) + 10.0;
		}
		return result;
	}

	// rotated ackley function for single group non-separable
	real CEC2013::rot_ackley(real *x, size_t dim) {
		real result = 0.0;
		real *z = multiply(x, mp_rot_matrix, dim);
		result = ackley(z, dim);
		delete[] z;
		return result;
	}

	// rotated ackley function for m group non-separable
	real CEC2013::rot_ackley(real *x, size_t dim, size_t k)
	{
		real result = 0.0;
		real sum1 = 0.0;
		real sum2 = 0.0;

		size_t i, j;
		for (i = dim - 1; i >= 0; i--)
		{
			mp_anotherz1[i] = 0;
			for (j = dim - 1; j >= 0; j--)
			{
				mp_anotherz1[i] += x[mp_Pvector[(k - 1)*dim + j]] * mp_rot_matrix[dim*j + i];
			}
		}
		for (i = dim - 1; i >= 0; i--)
		{
			sum1 += mp_anotherz1[i] * mp_anotherz1[i];
			sum2 += cos(2.0*OFEC_PI*mp_anotherz1[i]);
		}
		result = -20.0*exp(-0.2*sqrt(sum1 / dim)) - exp(sum2 / dim) + 20.0 + OFEC_E;
		return result;
	}

	// for single group non-separable function
	real CEC2013::sphere(real *x, size_t dim) {
		real sum = 0;
		size_t    i;

		for (i = dim - 1; i >= 0; i--) {
			sum += pow(x[i], 2);
		}

		return sum;
	}

	// for m groups non-separable function
	real CEC2013::sphere(real *x, size_t dim, size_t k) {
		real result = 0.0;
		size_t i;

		//	cout<<"partial sum of sphere function ="<<endl;
		for (i = dim / k - 1; i >= 0; --i)
		{
			result += x[mp_Pvector[dim / k + i]] * x[mp_Pvector[dim / k + i]];
			//		printf("x = %1.16E\t", x[Pvector[dim/k+i]]);
			//		printf("partial sum = %1.16E\t", result);
		}

		return result;
	}

	// for single group non-separable function
	real CEC2013::schwefel(real *x, size_t dim) {
		size_t    j;
		real s1 = 0;
		real s2 = 0;

		// T_{osz}
		transform_osz(x, dim);

		// T_{asy}^{0.2}
		transform_asy(x, 0.2, dim);

		for (j = 0; j < dim; j++) {
			s1 += x[j];
			s2 += (s1 * s1);
		}

		return s2;
	}

	// for m groups non-separable function
	real CEC2013::schwefel(real *x, size_t dim, size_t k) {
		real sum1 = 0.0;
		real sum2 = 0.0;
		size_t i;
		for (i = 0; i < dim; ++i)
		{
			sum1 += x[mp_Pvector[(k - 1)*dim + i]];
			sum2 += sum1*sum1;
		}
		return sum2;
	}

	// single group non-separable function
	real CEC2013::rosenbrock(real *x, size_t dim) {
		size_t    j;
		real oz, t;
		real s = 0.0;
		j = dim - 1;

		for (--j; j >= 0; j--) {
			oz = x[j + 1];
			t = ((x[j] * x[j]) - oz);
			s += (100.0 * t * t);
			t = (x[j] - 1.0);
			s += (t * t);
		}
		return s;
	}

	// m groups non-separable function
	real CEC2013::rosenbrock(real *x, size_t dim, size_t k) {
		size_t j;
		real oz, t;
		real result = 0.0;
		j = dim - 1;
		for (--j; j >= 0; j--) {
			oz = x[mp_Pvector[(k - 1)*dim + j + 1]];
			t = ((x[mp_Pvector[(k - 1)*dim + j]] * x[mp_Pvector[(k - 1)*dim + j]]) - oz);
			result += (100.0 * t * t);
			t = (x[mp_Pvector[(k - 1)*dim + j]] - 1.0);
			result += (t * t);
		}
		return result;
	}


	/*
	* ===  FUNCTION  ======================================================================
	*         Name:  getInterArray
	*  Description:  get the variable interaction information in the representation of one
	*  				dimensional array
	* =====================================================================================
	*/
	std::vector<bool>
		CEC2013::getInterArray()
	{
		return mbv_interArray;
	}		/* -----  end of function getInterArray  ----- */


			/*
			* ===  FUNCTION  ======================================================================
			*         Name:  convertMatrixToArrayIndex
			*  Description:
			* =====================================================================================
			*/
	unsigned
		CEC2013::convertMatrixToArrayIndex(unsigned i, unsigned j)
	{
		return (unsigned)(i* (2 * m_variable_size - i - 3) / 2 + j - 1);
	}		/* -----  end of function convertMatrixToArrayIndex  ----- */


			/*
			* ===  FUNCTION  ======================================================================
			*         Name:  createIndexMapping
			*  Description:
			* =====================================================================================
			*/
	void
		CEC2013::create_index_mapping()
	{
		unsigned N = (unsigned)m_variable_size, indexCounter = 0;

		mp_index_map = new struct index_map[m_arrSize];

		for (unsigned i = 0; i < N; i++) {
			for (unsigned j = i + 1; j < N; j++) {
				mp_index_map[indexCounter].arr_index1 = i;
				mp_index_map[indexCounter].arr_index2 = j;
				indexCounter++;
			}
		}
	}		/* -----  end of function CCVIL::createIndexMapping  ----- */


			/*
			* ===  FUNCTION  ======================================================================
			*         Name:  ArrToMat
			*  Description:
			* =====================================================================================
			*/
	void CEC2013::ArrToMat(unsigned I1, unsigned I2, unsigned &matIndex)
	{
		for (unsigned i = 0; i < m_arrSize; i++) {
			if (mp_index_map[i].arr_index1 == I1 && mp_index_map[i].arr_index2 == I2) {
				matIndex = i;
				return;
			}
		}

		printf("Cannot locate the matrix index from given array indices\n");
		exit(EXIT_FAILURE);
	}		/* -----  end of function CEC2013::ArrToMat  ----- */


			/*
			* ===  FUNCTION  ======================================================================
			*         Name:  CEC2013::MatToArr
			*  Description:
			* =====================================================================================
			*/
	void CEC2013::MatToArr(unsigned &I1, unsigned &I2, unsigned matIndex)
	{
		I1 = mp_index_map[matIndex].arr_index1;
		I2 = mp_index_map[matIndex].arr_index2;
	}		/* -----  end of function CEC2013::MatToArr  ----- */

	int CEC2013::sign(double x)
	{
		if (x > 0) return 1;
		if (x < 0) return -1;
		return 0;
	}

	double CEC2013::hat(double x)
	{
		if (x == 0)
		{
			return 0;
		}
		else
		{
			return log(abs(x));
		}
	}

	double CEC2013::c1(double x)
	{
		if (x > 0)
		{
			return 10;
		}
		else
		{
			return 5.5;
		}
	}

	double CEC2013::c2(double x)
	{
		if (x > 0)
		{
			return 7.9;
		}
		else
		{
			return 3.1;
		}
	}
}



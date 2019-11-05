#include "moving_peak.h"
#include "../../../../../core/global.h"
#include <algorithm>
namespace OFEC {
	static real basis_peak[5][7] = {
	  {8.0,  64.0,  67.0,  55.0,   4.0, 0.1, 50.0},
	  {50.0,  13.0,  76.0,  15.0,   7.0, 0.1, 50.0},
	  {9.0,  19.0,  27.0,  67.0, 24.0, 0.1, 50.0},
	  {66.0,  87.0,  65.0,  19.0,  43.0, 0.1, 50.0},
	  {76.0,  32.0,  43.0,  54.0,  65.0, 0.1, 50.0}
	};

	static real twin_peak[7] = /* difference to first peak */
	{
	  1.0,  1.0,  1.0,  1.0,   1.0, 0.0, 0.0,
	};

	void moving_peak::set_severity() {
		for (int i = 0; i < m_peak.size(); i++) {
			m_height_severity[i] = global::ms_global->m_uniform[caller::Problem]->next_non_standard(1.0,10.);//1.+9.*i/m_peak.size();//severity of height changes, larger numbers  mean larger severity. in the contex of ROOT, peaks have different values
			m_width_severity[i] = global::ms_global->m_uniform[caller::Problem]->next_non_standard(0.1,1.0);// 0.1+0.9*i/m_peak.size();severity of width changes, larger numbers mean larger severity
		}

	}

	moving_peak::moving_peak(param_map &v) :problem(v.at("proName"),v.at("numDim"), 1), \
		dynamic_continuous(v.at("proName"), v.at("numDim"), v.at("numPeak"), 1) {
		set_dimension_change(v.at("flagNumDimChange"));
		set_num_peak_change_mode(v.at("peakNumChangeMode"));
		set_flag_num_peaks_change(v.at("flagNumPeakChange"));
		set_flag_noise(v.at("flagNoise"));
		set_flag_time_linkage(v.at("flagTimeLinkage"));
		set_frequency((v.at("changeFre")));
		set_type(CT_Random);

		m_vlength = ((real)v.at("shiftLength"));
		if ((v.at("flagNoise")) == true)		set_noise_severity(v.at("noiseSeverity"));
		if ((v.at("flagTimeLinkage")) == true)	set_time_linkage_severity(v.at("timelinkageSeverity"));

		m_num_peak_tracked = 0;
		
		initialize();
		set_num_change((real)v.at("changeRatio"));
		m_optima.set_flag_variable(true);
		
		update_parameters();
	}
	moving_peak::moving_peak(const std::string &name, int rDimNumber,  int rNumPeaks, real  rChangingRatio,  int fre, real vlength, bool rFlagDimChange, \
		 bool rFlagNumPeakChange,  int peakNumChangeMode,  bool flagNoise,  bool flagTimelinkage) :\
		problem(name,rDimNumber, 1), dynamic_continuous(name, rDimNumber, rNumPeaks, 1) {

		set_frequency(fre);
		set_dimension_change(rFlagDimChange);
		set_num_peak_change_mode(peakNumChangeMode);
		set_flag_num_peaks_change(rFlagNumPeakChange);
		set_flag_noise(flagNoise);
		set_flag_time_linkage(flagTimelinkage);
		set_vlength(vlength);
		set_num_change(rChangingRatio);
		m_optima.set_flag_variable(true);
		m_num_peak_tracked = 0;
		
		initialize();
		update_parameters();
	}
	void moving_peak::initialize() {
		int i = 0;
		m_variable_accuracy = 0.1;
		m_objective_accuracy=0.2;
		set_range(0, 100);
		set_init_range(0, 100);
		set_opt_mode(optimization_mode::Maximization,0);
		add_tag(problem_tag::MMOP);
		/***************************************
		//		m_F		Evaluation Function
		//		1		constant_basis_func()
		//		2		five_peak_basis_func()
		//		3		peak_function1()
		//		4		peak_function1()
		//		5		peak_function_hilly()
		//		6		peak_function_twin()
		**************************************
		in>>temp>>m_vlength; // distance by which the peaks are moved, severity
		lambda determines whether there is a direction of the movement, or whether
		they are totally random. For lambda = 1.0 each move has the same direction,
		while for lambda = 0.0, each move has a random direction
		//in>>temp>>lambda;
		//in>>temp>>m_useBasisFunction;  if set to 1, a static landscape (basis_function) is included in the fitness evaluation
		}*/

		m_F = 4;
		m_lambda = 0;
		m_vlength = 1.0;
		m_min_height = 30.0;
		m_max_height = 70.0;
		m_standardHeight = 50.0;
		m_min_width = 1.0;
		m_max_width = 12.0;
		m_standardWidth = 0.0;

		m_shift.resize(m_variable_size);
		m_pre_movement.resize(m_num_peaks, std::vector<real>(m_variable_size));

		set_severity();

		update_time_linkage();
		for (i = 0; i < m_peak.size(); i++)
			for (int j = 0; j < m_variable_size; j++) {
				m_peak[i][j] = 100.0*global::ms_global->m_uniform[caller::Problem]->next(); 
				m_pre_movement[i][j] = global::ms_global->m_uniform[caller::Problem]->next() - 0.5;
			}

		if (m_standardHeight <= 0.0) {
			for (i = 0; i < m_peak.size(); i++) m_height[i] = (m_max_height - m_min_height)*global::ms_global->m_uniform[caller::Problem]->next() + m_min_height;
		}
		else {
			for (i = 0; i < m_peak.size(); i++) m_height[i] = m_standardHeight;
		}

		if (m_standardWidth <= 0.0) {
			for (i = 0; i < m_peak.size(); i++)
				m_width[i] = (m_max_width - m_min_width)*global::ms_global->m_uniform[caller::Problem]->next() + m_min_width;
		}
		else {
			for (i = 0; i < m_peak.size(); i++)
				m_width[i] = m_standardWidth;
		}

		calculate_global_optima();

		for (i = 0; i < m_peak.size(); i++) m_num_tracking[i] = 0;
		for (i = 0; i < m_peak.size(); i++)
			std::copy(m_peak[i].begin(), m_peak[i].end(),m_pre_peak[i].begin());

		std::copy(m_height.begin(), m_height.end(), m_pre_height.begin());
		std::copy(m_width.begin(), m_width.end(), m_pre_width.begin());
		m_initialized = true;
	}

	/* current_peak_calc determines the peak of the current best individual */
	void moving_peak::calculate_current_peak(const real *gen)
	{
		int i;
		real maximum = -100000.0, dummy;

		m_current_peak = 0;
		maximum = select_function(gen, 0);
		for (i = 1; i < m_peak.size(); i++) {
			dummy = select_function(gen, i);
			if (dummy > maximum) {
				maximum = dummy;
				m_current_peak = i;
			}
		}
	}

	void moving_peak::evaluate_objective(real* x_, std::vector<real>& obj_) {
		real *x = new real[m_variable_size];
		for (size_t i = 0; i < m_variable_size; i++)
			x[i] = x_[i];
		if (this->m_flag_noise)	add_noise(x);

		real maximum = -std::numeric_limits<real>::max(), dummy;

		for (int i = 0; i < m_peak.size(); i++) {
			dummy = select_function(x, i);
			if (dummy > maximum)      maximum = dummy;
		}
		obj_[0] = maximum;

		if (m_eval_monitor&&m_effective_eval%m_frequency == 0) {
			ms_minmax_objective.clear();
		}
		bool flag_stop;

		if (global::ms_global->m_algorithm != nullptr)	flag_stop = global::ms_global->m_algorithm->terminating();
		else flag_stop = false;

		if (m_effective_eval%m_frequency == 0 && !flag_stop) {
			dynamic::change();
		}
		delete[] x;
		x = 0;
		//evaluation_tag rf = evaluation_tag::Normal;
		//if (effective) {
		//	if (global::ms_global->m_algorithm != nullptr&&flag_stop) {
		//		rf = evaluation_tag::Terminate; 
		//	}
		//		
		//	if ((m_effective_eval + 1) % (m_frequency) == 0) {
		//		rf = evaluation_tag::Change_next_eval;
		//	}
		//	else if (m_effective_eval % m_frequency == 0) {
		//		if (CAST_DYN->get_flag_dimension_change()) {
		//			rf = evaluation_tag::Change_dimension;
		//		}else if (CAST_DYN->get_flag_time_linkage() && CAST_DYN->get_trigger_flag_time_linkage()) {
		//			rf = evaluation_tag::Change_timelinkage;
		//		}
		//		else	rf = evaluation_tag::Change;
		//	}						
		//}
		//return rf;
	}


	/* dummy evaluation function allows to evaluate without being counted */
	real moving_peak::dummy_eval(const real *gen)
	{
		int i;
		real maximum = -100000.0, dummy;

		for (i = 0; i < m_peak.size(); i++) {
			dummy = select_function(gen, i);
			if (dummy > maximum)    maximum = dummy;
		}
		return(maximum);
	}

	/* whenever this function is called, the peaks are changed */
	void moving_peak::change_random() {
		int i = 0, j = 0;
		real sum, sum2, offset;

		
		for (i = 0; i < m_peak.size(); i++) 	m_pre_peak[i] = m_peak[i];
		m_pre_height = m_height;
		m_pre_width = m_width;

		for (i = 0; i < m_peak.size(); i++) {
			if (m_flag_change[i] == false) continue;
			/* shift peak locations */
			sum = 0.0;
			for (j = 0; j < m_variable_size; j++) {
				m_shift[j] = global::ms_global->m_uniform[caller::Problem]->next() - 0.5;
				sum += m_shift[j] * m_shift[j];
			}
			if (sum > 0.0)		sum = m_vlength / sqrt(sum);
			else  sum = 0.0;                        /* only in case of rounding errors */

			sum2 = 0.0;
			for (j = 0; j < m_variable_size; j++) {
				m_shift[j] = sum*(1.0 - m_lambda)*m_shift[j] + m_lambda*m_pre_movement[i][j];
				sum2 += m_shift[j] * m_shift[j];
			}
			if (sum2 > 0.0)sum2 = m_vlength / sqrt(sum2);
			else     sum2 = 0.0;                      /* only in case of rounding errors */

			for (j = 0; j < m_variable_size; j++) {
				m_shift[j] *= sum2;
				m_pre_movement[i][j] = m_shift[j];
				if (m_domain[j].limit.first > (m_peak[i][j] + m_pre_movement[i][j])) {
					m_peak[i][j] = 2.0*m_domain[j].limit.first - m_peak[i][j] - m_pre_movement[i][j];
					m_pre_movement[i][j] *= -1.0;
				}
				else if (m_domain[j].limit.second < (m_peak[i][j] + m_pre_movement[i][j])) {
					m_peak[i][j] = 2.0*m_domain[j].limit.second - m_peak[i][j] - m_pre_movement[i][j];
					m_pre_movement[i][j] *= -1.0;
				}
				else
					m_peak[i][j] += m_pre_movement[i][j];
			}

			/* change peak width */

			offset = global::ms_global->m_normal[caller::Problem]->next()*m_width_severity[i];
			if ((m_width[i] + offset) < m_min_width)		m_width[i] = 2.0*m_min_width - m_width[i] - offset;
			else if ((m_width[i] + offset) > m_max_width)	m_width[i] = 2.0*m_max_width - m_width[i] - offset;
			else	m_width[i] += offset;

			if (m_counter > 1 && m_ratio_changing_peak < 1.0&&m_flag_global_optima[i]) continue;
			/* change peak height */

			offset = m_height_severity[i] * global::ms_global->m_normal[caller::Problem]->next();

			if ((m_height[i] + offset) < m_min_height)	m_height[i] = 2.0*m_min_height - m_height[i] - offset;
			else if ((m_height[i] + offset) > m_max_height)	m_height[i] = 2.0*m_max_height - m_height[i] - offset;
			else	m_height[i] += offset;
		}

		calculate_global_optima();
		update_num_change();

	}


	/* Basis Functions */

	/* This gives a constant value back to the eval-function that chooses the max of them */
	real moving_peak::constant_basis_func(const real *gen)
	{
		return 0.0;
	}

	real moving_peak::five_peak_basis_func(const real *gen) {
		real maximum = -100000.0, dummy = 0;
		for (int i = 0; i < 5; i++) {
			dummy = (gen[0] - basis_peak[i][0])*(gen[0] - basis_peak[i][0]);
			for (int j = 1; j < m_variable_size; j++)  dummy += (gen[j] - basis_peak[i][j])*(gen[j] - basis_peak[i][j]);
			dummy = basis_peak[i][m_variable_size + 1] - (basis_peak[i][m_variable_size] * dummy);
			if (dummy > maximum)       maximum = dummy;
		}
		return maximum;
	}

	/* Peak Functions */

	/* sharp peaks */
	real moving_peak::peak_function1(const real *gen, int peak_number) {

		real dummy = (gen[0] - m_peak[peak_number][0])*(gen[0] - m_peak[peak_number][0]);
		for (int j = 1; j < m_variable_size; j++)
			dummy += (gen[j] - m_peak[peak_number][j])*(gen[j] - m_peak[peak_number][j]);

		return m_height[peak_number] / (1 + m_width[peak_number] * dummy);
	}

	real moving_peak::peak_function_hilly(const real *gen, int peak_number) {
		int j = 0;
		real dummy = (gen[0] - m_peak[peak_number][0])*(gen[0] - m_peak[peak_number][0]);
		for (j = 1; j < m_variable_size; j++)
			dummy += (gen[j] - m_peak[peak_number][j])*(gen[j] - m_peak[peak_number][j]);

		return m_height[peak_number] - m_width[peak_number] * dummy - 0.01*sin(20.0*dummy);
	}

	real moving_peak::peak_function_twin(const real  *gen, int peak_number) /* two twin peaks moving together */
	{
		int j;
		real maximum = -100000.0, dummy = pow(gen[0] - m_peak[peak_number][0], 2);
		for (j = 1; j < m_variable_size; j++)
			dummy += pow(gen[j] - m_peak[peak_number][j], 2);

		dummy = m_height[peak_number] - m_width[peak_number] * dummy;

		maximum = dummy;
		dummy = pow(gen[0] - (m_peak[peak_number][0] + twin_peak[0]), 2);
		for (j = 1; j < m_variable_size; j++)
			dummy += pow(gen[j] - (m_peak[peak_number][j] + twin_peak[0]), 2);

		dummy = m_height[peak_number] + twin_peak[m_variable_size + 1] - ((m_width[peak_number] + twin_peak[m_variable_size])*dummy);
		if (dummy > maximum)
			maximum = dummy;

		return maximum;
	}


	real moving_peak::peakFunctionCone(const real *gen, const int &peak_number) {

		real val, dummy = 0;
		for (int j = 0; j < m_variable_size; j++) {
			val = gen[j] - m_peak[peak_number][j];
			dummy += val*val;
		}
		if (dummy != 0)  dummy = m_height[peak_number] - m_width[peak_number] * sqrt(dummy);
		return dummy;
	}
	real moving_peak::select_function(const real  *gen, const int &peak_number) {
		real dummy = 0;
		switch (m_F) {
		case 1: {
			dummy = constant_basis_func(gen);
			break;
		}
		case 2: {
			dummy = five_peak_basis_func(gen);
			break;
		}
		case 3: {
			dummy = peak_function1(gen, peak_number);
			break;
		}
		case 4: {
			dummy = peakFunctionCone(gen, peak_number);
			break;
		}
		case 5: {
			dummy = peak_function_hilly(gen, peak_number);
			break;
		}
		case 6: {
			dummy = peak_function_twin(gen, peak_number);
			break;
		}
		}
		return dummy;
	}
	/* The following procedures may be used to change the step size over time */


	void moving_peak::change_stepsize_random() /* assigns vlength a value from a normal distribution */
	{
		m_vlength = global::ms_global->m_normal[caller::Problem]->next();
	}

	void moving_peak::change_stepsize_linear() /* sinusoidal change of the stepsize, */
	{
		static	thread_local std::unique_ptr< int> counter;
		if (!counter.get()) counter.reset(new int(1));
		thread_local std::unique_ptr<real> frequency;
		if (!frequency.get()) frequency.reset(new real(3.14159 / 20.0));

		m_vlength = 1 + sin((real)(*counter)*(*frequency));
		(*counter)++;
	}

	int moving_peak::get_right_peak()  /* returns 1 if current best individual is on highest m_peak, 0 otherwise */
	{
		bool flag = false;

		for (int i = 0; i < m_peak.size(); i++) {
			if (m_flag_global_optima[i] == true && m_current_peak == i) {
				flag = true;
				break;
			}
		}

		return flag;
	}
	void moving_peak::set_vlength(real s) {
		m_vlength = s;
		m_parameters["vlength"] = s;
	}

	void moving_peak::change_num_peak() {

		moving_peak mpb(m_name, m_variable_size, m_temp_num_peak, m_ratio_changing_peak, m_flag_dimension_change
			, m_flag_num_peak_change, m_mode, m_flag_noise, m_flag_time_linkage);

		mpb.copy(*this);
		mpb.calculate_global_optima();

		*this = std::move(mpb);

	}

	void moving_peak::copy(const problem &rP) {
		dynamic_continuous::copy(rP);

		auto& mpb = dynamic_cast<const moving_peak &>(rP);
		
		int peaks = m_peak.size() < mpb.get_num_peak() ? m_peak.size() : mpb.get_num_peak();

		m_F = mpb.m_F;
		m_vlength = mpb.m_vlength;
		m_lambda = mpb.m_lambda;
		m_standardHeight = mpb.m_standardHeight;
		m_standardWidth = mpb.m_standardWidth;
		m_shift = mpb.m_shift;
				
		for (int i = 0; i < peaks; i++) {
			m_pre_movement[i] = mpb.m_pre_movement[i];
		}

	}

	real moving_peak::get_vlength() {
		return m_vlength;
	}

	void moving_peak::update_parameters() {
		dynamic_continuous::update_parameters();

		m_parameters["vlength"]= m_vlength; 
		m_parameters["lambda"] =m_lambda;
	}
}

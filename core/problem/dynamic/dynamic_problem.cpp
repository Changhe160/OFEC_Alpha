#include"dynamic_problem.h"



namespace OFEC {

	thread_local unique_ptr<int> dynamic_problem::ms_init_num_peaks, dynamic_problem::ms_init_num_dim, dynamic_problem::ms_num_instance;

#ifdef OFEC_DEMON
#include "../../../ui/Buffer/Scene.h"
	extern unique_ptr<Scene> msp_buffer;
#endif


	void dynamic_problem::set_dimension_change(const bool flag)
	{
		m_flag_dimension_change = flag;

		size_t start, end;
		start = m_parameters.str().find("dimensionalchange:");
		for (auto i = start; i<m_parameters.str().size(); i++) {
			if (m_parameters.str()[i] == ';') {
				end = i;
				break;
			}
		}
		stringstream ss;
		ss << "dimensionalchange:" << m_flag_dimension_change << "; ";
		string result = m_parameters.str();
		result.replace(start, end - start + 1, ss.str());
		m_parameters.str(result);
	}

	void dynamic_problem::set_change_dirction(const bool flag)
	{
		m_dir_dimension_change = flag;
	}

	dynamic_problem::dynamic_problem(const int size_var, const int num_peaks, const unsigned size_obj):problem(string(), size_var, size_obj), m_change_counter(0)
		, m_dim_number_temp(size_var), m_num_peaks(num_peaks), m_num_peaks_temp(num_peaks), m_noise_flag(false), m_time_linkage_flag(false), m_flag_trigger_time_linkage(false) {

		m_change_interval = 5000;
		m_change_type.type = change_type::CT_random;
		m_change_type.counter = 0;
		m_period = 0;
		m_flag_dimension_change = false;
		m_dir_dimension_change = true;
		m_synchronize = true;
		m_noisy_severity = 0.8;

		m_alpha = 0.04;
		m_max_alpha = 0.1;
		m_chaotic_constant = 3.67; //in [3.57,4]

		m_flag_num_peaks_change = false;
		m_flag_num_peaks_change = true;
		m_num_peaks_change_mode = 1;
		m_noise_severity_ = 0.01;
		m_time_linkage_severity = 0.1;

		m_parameters << "Change frequency:" << m_change_interval << "; " << "TotalEvals:" << global::ms_arg[param_maxEvals] << "; " << "Peaks:" << m_num_peaks << "; " << "NumPeaksChange:" << m_flag_num_peaks_change << "-" << m_num_peaks_change_mode << "; " <<
			"NoisyEnvioronments:" << m_noise_flag << "; NoiseSeverity:" << m_noise_severity_ << "; TimeLinkageEnvironments:" << m_time_linkage_flag << "; TimeLinkageSeverity:" << m_time_linkage_severity << "; DimensionalChange:" << m_flag_dimension_change << "; ";

		if (!ms_num_instance.get()) ms_num_instance.reset(new int(0));
		if (!ms_init_num_peaks.get()) ms_init_num_peaks.reset(new int);
		if (!ms_init_num_dim.get())ms_init_num_dim.reset(new int);
		(*ms_num_instance)++;
		if (*ms_num_instance == 1) {
			*ms_init_num_peaks = m_num_peaks;
			*ms_init_num_dim = m_variable_size;
		}
		add_tag(problem_tag::DOP);
	}

	dynamic_problem::~dynamic_problem()
	{
		//dtor
	}

	dynamic_problem & dynamic_problem::operator=(const dynamic_problem & dynamic_problem)
	{
		if (this == &dynamic_problem) return *this;

		if (m_variable_size != dynamic_problem.m_variable_size) {
			throw myexcept("The number of dimensions must be same!@dynamic_problem::operator=");
		}
		if (m_change_type.type != dynamic_problem.m_change_type.type) {
			throw myexcept("The change type must be same!@dynamic_problem::operator=");
		}
		if (m_num_peaks != dynamic_problem.m_num_peaks) {
			throw myexcept("The number of peaks must be same!@dynamic_problem::operator=");
		}
		problem::operator=(dynamic_problem);


		m_change_type.counter = dynamic_problem.m_change_type.counter;
		m_change_interval = dynamic_problem.m_change_interval;
		m_period = dynamic_problem.m_period;
		m_flag_dimension_change = dynamic_problem.m_flag_dimension_change;
		m_dir_dimension_change = dynamic_problem.m_dir_dimension_change;
		m_synchronize = dynamic_problem.m_synchronize;
		m_noisy_severity = dynamic_problem.m_noisy_severity;

		m_alpha = dynamic_problem.m_alpha;
		m_max_alpha = dynamic_problem.m_max_alpha;
		m_chaotic_constant = dynamic_problem.m_chaotic_constant;

		m_flag_num_peaks_change = dynamic_problem.m_flag_num_peaks_change;
		m_flag_num_peaks_change = dynamic_problem.m_flag_num_peaks_change;
		m_num_peaks_change_mode = dynamic_problem.m_num_peaks_change_mode;

		m_noise_flag = dynamic_problem.m_noise_flag;
		m_time_linkage_flag = dynamic_problem.m_time_linkage_flag;

		m_noise_severity_ = dynamic_problem.m_noise_severity_;
		m_time_linkage_severity = dynamic_problem.m_time_linkage_severity;
		m_flag_trigger_time_linkage = dynamic_problem.m_flag_trigger_time_linkage;
		return *this;
	}

	void dynamic_problem::set_num_peak_change_mode(const int mode)
	{
		m_num_peaks_change_mode = mode;
	}

	int dynamic_problem::get_num_peak_change_mode()
	{
		return m_num_peaks_change_mode;
	}

	void dynamic_problem::set_noise_flag(const bool flag)
	{
		m_noise_flag = flag;

		size_t start, end;
		start = m_parameters.str().find("NoisyEnvioronments:");
		for (auto i = start; i<m_parameters.str().size(); i++) {
			if (m_parameters.str()[i] == ';') {
				end = i;
				break;
			}
		}

		stringstream ss;
		ss << "NoisyEnvioronments:" << m_noise_flag << "; ";
		string result = m_parameters.str();
		result.replace(start, end - start + 1, ss.str());
		m_parameters.str(result);
	}

	int dynamic_problem::get_number_of_peak() const
	{
		return m_num_peaks;
	}

	void dynamic_problem::set_time_linkage_flag(const bool flag)
	{
		m_time_linkage_flag = flag;
		size_t start, end;
		start = m_parameters.str().find("TimeLinkageEnvironments:");
		for (auto i = start; i<m_parameters.str().size(); i++) {
			if (m_parameters.str()[i] == ';') {
				end = i;
				break;
			}
		}

		stringstream ss;
		ss << "TimeLinkageEnvironments:" << m_time_linkage_flag << "; ";
		string result = m_parameters.str();
		result.replace(start, end - start + 1, ss.str());
		m_parameters.str(result);
	}

	void dynamic_problem::change()
	{
		m_change_counter++;
		switch (get_change_type()) {
		case change_type::CT_random:
			random_change();
			break;
		case change_type::CT_recurrent:
			recurrent_change();
			break;
		case change_type::CT_recurrent_noisy:
			recurrent_noisy_change();
			break;
		case change_type::CT_small_step:
			small_step_change();
			break;
		case change_type::CT_large_step:
			large_step_change();
			break;
		case change_type::CT_chaotic:
			chaotic_change();
			break;
		default:
			break;
		}

		if (m_flag_dimension_change) {

			if (m_variable_size == msc_min_dimension_number)
				m_dir_dimension_change = true;
			if (m_variable_size == msc_max_dimension_number)
				m_dir_dimension_change = false;

			if (m_dir_dimension_change == true) {
				m_dim_number_temp += 1;
			}
			else {
				m_dim_number_temp -= 1;
			}
			change_dimension();
		}

		if (m_flag_num_peaks_change) {
			if (m_num_peaks_change_mode == 1 || m_num_peaks_change_mode == 2) {
				if ((unsigned int)m_num_peaks >= msc_max_num_peaks - 1) m_dir_num_peaks_change = false;
				if ((unsigned int)m_num_peaks <= msc_min_num_peaks + 1) m_dir_num_peaks_change = true;
				int step = 0;

				if (m_name == "DYN_CONT_CompositionDBG") step = 2;
				else if (m_name == "DYN_CONT_RotationDBG") step = 2;
				else if (m_name == "DYN_CONT_MovingPeak") step = 2;
				else step = 2;

				if (m_num_peaks_change_mode == 2) {
					// TODO: too long
					step = static_cast<int>(step / 2 + (5 * step / 2 - step / 2)*global::ms_global->m_uniform.at(caller::Problem)->next());
				}

				if (m_dir_num_peaks_change == true) {
					if (m_num_peaks + step <= msc_max_num_peaks)		m_num_peaks_temp = m_num_peaks + step;
					else m_num_peaks_temp = msc_max_num_peaks;
				}
				else {
					if (m_num_peaks - step >= msc_min_num_peaks)		m_num_peaks_temp = m_num_peaks - step;
					else m_num_peaks_temp = msc_min_num_peaks;
				}
			}
			else {
				//random change
				//TODO: too long
				m_num_peaks_temp = static_cast<int>(msc_min_num_peaks + (msc_max_num_peaks - msc_min_num_peaks)*global::ms_global->m_uniform.at(caller::Problem)->next());
			}
			change_num_peaks();
		}

		//TODO: need measure module and a get_optima function in class continuous, and the run_id in class global
		//if (mSingleObj::getSingleObj() != nullptr) {
		//	vector<double> gOpt;
		//	if (global::ms_global->m_problem->getObjglobalOpt(gOpt)) {
		//		mSingleObj::getSingleObj()->addGOpt(global::ms_global->m_runId, gOpt[0]);
		//	}
		//	else {
		//		std::cout << "err" << endl;
		//	}
		//}

#ifdef OFEC_DEMON
		msp_buffer->updateFitnessLandsacpe_();
#endif

	}

	double dynamic_problem::sin_value_noisy(const int x, const double min, const double max, const double amplitude, const double angle, const double noisy_severity)
	{
		double y;
		double noisy, t;
		y = min + amplitude*(sin(2 * OFEC_PI*(x + angle) / m_period) + 1) / 2.;
		noisy = noisy_severity*global::ms_global->m_normal.at(caller::Problem)->next();
		t = y + noisy;
		if (t>min&&t<max) y = t;
		else y = t - noisy;
		return y;
	}

	double dynamic_problem::chaotic_step(const double x, const double min, const double max, const double scale)
	{
		if (min>max) return -1;
		double chaotic_value;
		chaotic_value = (x - min) / (max - min);
		chaotic_value = m_chaotic_constant*chaotic_value*(1 - chaotic_value);
		//return fabs((min+chaotic_value*(max-min)-x)* global::scale);
		return chaotic_value*scale;
	}

	bool dynamic_problem::predict_change(const int evals_more)
	{
		int fre = get_change_fre();
		int evals = evaluations() % fre;
		if (evals + evals_more >= fre) return true;
		else return false;
	}

	void dynamic_problem::set_noise_severity_(double value)
	{
		m_noise_severity_ = value;
		size_t start, end;
		start = m_parameters.str().find("NoiseSeverity:");
		for (auto i = start; i<m_parameters.str().size(); i++) {
			if (m_parameters.str()[i] == ';') {
				end = i;
				break;
			}
		}
		stringstream ss;
		ss << "NoiseSeverity:" << m_noise_severity_ << "; ";
		string result = m_parameters.str();
		result.replace(start, end - start + 1, ss.str());
		m_parameters.str(result);

	}





	void dynamic_problem::set_change_interval(const int change_interval)
	{
		if (change_interval>0) m_change_interval = change_interval;
		else {
			throw myexcept("Change frequncy must be greater than 0 @dynamic_problem::set_change_fre");
		}

		size_t start, end;
		start = m_parameters.str().find("Change frequency:");
		for (auto i = start; i<m_parameters.str().size(); i++) {
			if (m_parameters.str()[i] == ';') {
				end = i;
				break;
			}
		}
		stringstream ss;
		ss << "Change frequency:" << m_change_interval << "; ";
		string result = m_parameters.str();
		result.replace(start, end - start + 1, ss.str());
		m_parameters.str(result);
	}

	bool dynamic_problem::set_period(const int period)
	{
		if (period >= 0) m_period = period;
		else {
			throw myexcept("period must be positive@ dynamic_problem::set_period");
		}
		return true;
	}

	void dynamic_problem::set_change_type(const s_change_type & change_type)
	{
		m_change_type = change_type;
	}

	void dynamic_problem::set_change_type(const change_type type)
	{
		m_change_type.type = type;
	}

	void dynamic_problem::set_num_peaks_change(const bool peaks_change)
	{
		m_flag_num_peaks_change = peaks_change;
		size_t start, end;
		start = m_parameters.str().find("NumPeaksChange:");
		for (auto i = start; i < m_parameters.str().size(); i++) {
			if (m_parameters.str()[i] == ';') {
				end = i;
				break;
			}
		}
	}

	void dynamic_problem::set_synchronize(const bool flag)
	{
		m_dir_dimension_change = flag;
	}

	void dynamic_problem::set_noisy_severity(const double severity)
	{
		m_noisy_severity = severity;
	}

	void dynamic_problem::set_timelinkage_severity(double value)
	{
		m_time_linkage_severity = value;

		size_t start, end;
		start = m_parameters.str().find("TimeLinkageSeverity:");
		for (auto i = start; i<m_parameters.str().size(); i++) {
			if (m_parameters.str()[i] == ';') {
				end = i;
				break;
			}
		}
		stringstream ss;
		ss << "TimeLinkageSeverity:" << m_time_linkage_severity << "; ";
		string result = m_parameters.str();
		result.replace(start, end - start + 1, ss.str());
		m_parameters.str(result);
	}

	int dynamic_problem::get_initial_num_peaks()
	{
		return *ms_init_num_peaks;
	}

	void dynamic_problem::parameter_setting(problem * rp)
	{

	}

}

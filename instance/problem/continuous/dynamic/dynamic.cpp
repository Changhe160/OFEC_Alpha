#include "../../../../core/global.h"
#include "dynamic.h"


namespace OFEC {
	thread_local std::unique_ptr<int> dynamic::ms_init_peaks, dynamic::ms_init_dimensions, dynamic::ms_instances;
	std::vector<std::string> dynamic::ms_type = { "SmallStep", "LargeStep", "Random", "Recurrent", "Chaotic", "RecurrentNoisy" };

	dynamic::dynamic(const std::string &name, size_t rDimNumber, size_t rNumPeaks, size_t numObj) :problem(name, rDimNumber, numObj), m_counter(0)
		, m_temp_dimension(rDimNumber), m_num_peaks(rNumPeaks), m_temp_num_peak(rNumPeaks), m_flag_noise(false), m_flag_time_linkage(false), m_flag_trigger_time_linkage(false) {
		//ctor

		m_frequency = 5000;
		m_change.type = CT_Random;
		m_change.counter = 0;
		m_period = 0;
		m_flag_dimension_change = false;
		m_direction_dimension_change = true;
		m_synchronize = true;
		m_recurrent_noisy_severity = 0.8;

		m_alpha = 0.04;
		m_max_alpha = 0.1;
		m_chaotic_constant = 3.67; //in [3.57,4]

		m_flag_num_peak_change = false;
		m_dir_num_peak_change = true;
		m_mode = 1;
		m_noise_severity = 0.01;
		m_time_linkage_severity = 0.1;

		if (!ms_instances.get()) ms_instances.reset(new int(0));
		if (!ms_init_peaks.get()) ms_init_peaks.reset(new int);
		if (!ms_init_dimensions.get())ms_init_dimensions.reset(new int);
		(*ms_instances)++;
		if (*ms_instances == 1) {
			*ms_init_peaks = m_num_peaks;
			*ms_init_dimensions = m_variable_size;
		}
		add_tag(problem_tag::DOP);
		update_parameters();
	}
	void dynamic::update_parameters() {
		problem::update_parameters();

		m_parameters["Change frequency"] = m_frequency;
		m_parameters["Initial peaks"] = m_num_peaks;
		m_parameters["Flag number of peaks change"] = m_flag_num_peak_change;
		if (m_flag_num_peak_change)
			m_parameters["change mode"] = m_mode;
		m_parameters["Flag of noise"] = m_flag_noise;
		if (m_flag_noise)
			m_parameters["Noise severity"] = m_noise_severity;
		m_parameters["Change frequency:"] = m_frequency;
		m_parameters["Flag time-linkage"] = m_flag_time_linkage;
		if (m_flag_time_linkage)
			m_parameters["Time-linkage severity"] = m_time_linkage_severity;
		m_parameters["Flag dimensional change"] = m_flag_dimension_change;
		m_parameters["Period"] = m_period;
		if (m_change.type == CT_RecurrentNoisy)
			m_parameters["Recurrent noisy severity"] = m_recurrent_noisy_severity;
		m_parameters["Change type"] = ms_type[static_cast<int>(m_change.type)];
	}
	dynamic::~dynamic()
	{
		//dtor
	}
	int dynamic::get_num_peak()const {
		return m_num_peaks;
	}

	void dynamic::set_flag_num_peaks_change(bool rPC) {
		m_flag_num_peak_change = rPC;
		m_parameters["Flag number of peaks change"] = m_flag_num_peak_change;
	}

	void dynamic::set_frequency(int rChangeFre) {
		m_frequency = rChangeFre;
		m_parameters["Change frequency:"] = m_frequency;
	}
	void dynamic::set_period( int rPeriod) {
		m_period = rPeriod;
		m_parameters["Period"] = m_period;
	}

	void dynamic::set_type( ChangeType rT) {
		m_change.type = rT;
		m_parameters["Change type"] = ms_type[static_cast<int>(m_change.type)];
	}

	void dynamic::set_dimension_change( bool rFlag) {
		m_flag_dimension_change = rFlag;
		m_parameters["Flag dimensional change"] = m_flag_dimension_change;
	}
	void dynamic::set_noise_severity(real value) {
		m_noise_severity = value;
		m_parameters["Noise severity"] = m_noise_severity;
	}
	void dynamic::set_time_linkage_severity(real value) {
		m_time_linkage_severity = value;
		m_parameters["Time-linkage severity"] = m_time_linkage_severity;
	}

	void dynamic::set_change_dirction(bool rFlag) {
		m_direction_dimension_change = rFlag;
	}
	void dynamic::set_flag_synchronize(bool rFlag) {
		m_synchronize = rFlag;
	}

	void dynamic::set_recurrent_noisy_severity(real rSeverity) {
		m_recurrent_noisy_severity = rSeverity;
		m_parameters["Recurrent noisy severity"] = m_recurrent_noisy_severity;
	}

	void dynamic::change() {
		m_counter++;
		switch (m_change.type) {
		case CT_Random:
			change_random();
			break;
		case CT_Recurrent:
			change_recurrent();
			break;
		case CT_RecurrentNoisy:
			change_recurrent_noisy();
			break;
		case CT_SmallStep:
			change_small_step();
			break;
		case CT_LargeStep:
			change_large_step();
			break;
		case CT_Chaotic:
			change_chaotic();
			break;
		default:
			break;
		}

		if (m_flag_dimension_change) {
			if (m_variable_size == ms_min_dimension)
				m_direction_dimension_change = true;
			if (m_variable_size == ms_max_dimension)
				m_direction_dimension_change = false;

			if (m_direction_dimension_change == true) {
				m_temp_dimension += 1;
			}
			else {
				m_temp_dimension -= 1;
			}
			change_dimension();
		}

		if (m_flag_num_peak_change) {
			if (m_mode == 1 || m_mode == 2) {
				if ((unsigned int)m_num_peaks >= ms_max_peaks - 1) m_dir_num_peak_change = false;
				if ((unsigned int)m_num_peaks <= ms_min_peaks + 1) m_dir_num_peak_change = true;
				int step = 2;

				if (m_mode == 2) {
					step = global::ms_global->m_uniform[caller::Problem]->next_non_standard(step / 2, 5 * step / 2);
				}

				if (m_dir_num_peak_change == true) {
					if (m_num_peaks + step <= ms_max_peaks)		m_temp_num_peak = m_num_peaks + step;
					else m_temp_num_peak = ms_max_peaks;
				}
				else {
					if (m_num_peaks - step >= ms_min_peaks)		m_temp_num_peak = m_num_peaks - step;
					else m_temp_num_peak = ms_min_peaks;
				}
			}
			else {
				//random change
				m_temp_num_peak = global::ms_global->m_uniform[caller::Problem]->next_non_standard(ms_min_peaks, ms_max_peaks);
			}
			change_num_peak();
		}

	}

	void  dynamic::copy(const problem &rdp) {
		problem::copy(rdp);

		const dynamic &rDP = dynamic_cast<const dynamic&>(rdp);
		m_change = rDP.m_change;
		m_frequency = rDP.m_frequency;
		m_period = rDP.m_period;
		m_flag_dimension_change = rDP.m_flag_dimension_change;
		m_direction_dimension_change = rDP.m_direction_dimension_change;
		m_synchronize = rDP.m_synchronize;
		m_recurrent_noisy_severity = rDP.m_recurrent_noisy_severity;
		m_alpha = rDP.m_alpha;
		m_max_alpha = rDP.m_max_alpha;
		m_chaotic_constant = rDP.m_chaotic_constant;

		m_flag_num_peak_change = rDP.m_flag_num_peak_change;
		m_dir_num_peak_change = rDP.m_dir_num_peak_change;
		m_mode = rDP.m_mode;
		m_flag_noise = rDP.m_flag_noise;
		m_flag_time_linkage = rDP.m_flag_time_linkage;
		m_noise_severity = rDP.m_noise_severity;
		m_time_linkage_severity = rDP.m_time_linkage_severity;
		m_flag_trigger_time_linkage = rDP.m_flag_trigger_time_linkage;
	}

	real dynamic::get_recurrent_noise(int x, real min, real max, real amplitude, real angle, real noisy_severity) {
		// return a value in recurrent with noisy dynamism environment
		real y;
		real noisy, t;
		y = min + amplitude*(sin(2 * OFEC_PI*(x + angle) / m_period) + 1) / 2.;
		noisy = noisy_severity*global::ms_global->m_normal[caller::Problem]->next();
		t = y + noisy;
		if (t > min&&t < max) y = t;
		else y = t - noisy;
		return y;
	}

	real dynamic::chaotic_step(const real x, const real min, const real max, const real scale) {
		if (min > max) return -1.;
		real chaotic_value;
		chaotic_value = (x - min) / (max - min);
		chaotic_value = m_chaotic_constant*chaotic_value*(1 - chaotic_value);
		return chaotic_value*scale;
	}

	bool dynamic::predict_change(const int evalsMore) {
		int fre = get_frequency();
		int evals = evaluations() % fre;
		if (evals + evalsMore >= fre) return true;
		else return false;
	}
	void dynamic::set_num_peak_change_mode(const int mode) {
		m_mode = mode;
	}
	int dynamic::get_num_peak_change_mode() {
		return m_mode;
	}
	void dynamic::set_flag_noise(const bool flag) {
		m_flag_noise = flag;
		m_parameters["Flag of noise"] = flag;
	}
	void dynamic::set_flag_time_linkage(const bool flag) {
		m_flag_time_linkage = flag;
		m_parameters["Flag time-linkage"] = m_flag_time_linkage;
	}
}
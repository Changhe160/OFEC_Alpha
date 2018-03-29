

#include "par_est_FM_sound_waves.h"
namespace OFEC {
	par_est_FM_sound_waves::par_est_FM_sound_waves(param_map &v) :problem((v[param_proName]), 6, 1), \
		function((v[param_proName]), 6, 1) {
		set_range(-6.4, 6.35);
		set_init_range(-6.4, 6.35);
		initialize();

	}
	par_est_FM_sound_waves::par_est_FM_sound_waves(const std::string &name, size_t size_var, size_t size_obj) :problem(name, 6, 1), \
		function(name, 6, 1) {
		set_range(-6.4, 6.35);
		set_init_range(-6.4, 6.35);
		initialize();
	}


	void par_est_FM_sound_waves::initialize() {
		std::vector<real> v(m_variable_size);
		v[0] = 1.0; v[1] = 5.0; v[2] = 1.5; v[3] = 4.8; v[4] = 2.0; v[5] = 4.9;

		set_original_global_opt(v.data());
		m_optima = m_original_optima;
	}

	void par_est_FM_sound_waves::evaluate__(real *x, std::vector<real>& obj) {

		double theta = 2 * OFEC_PI / 100.;
		real s = 0, t;

		for (int i = 0; i < 100; i++) {
			t = x[0] * sin(x[1] * i*theta + x[2] * sin(x[3] * i*theta + x[4] * sin(x[5] * i*theta))) - sin(5.*i*theta + 1.5*sin(4.8*i*theta + 2.0*sin(4.9*i*theta)));
			s += t*t;
		}
		obj[0] = s;

	}
}
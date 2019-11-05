#include "dynamic_continuous.h"
#include "../../../../core/global.h"
#include "../../../../utility/vector.h"
#include <algorithm>

namespace OFEC {
	
	dynamic_continuous::dynamic_continuous(const std::string &name, size_t rDimNumber, size_t rNumPeaks,  size_t numObj):
		dynamic(name,rDimNumber, rNumPeaks, numObj), continuous(name,rDimNumber, numObj){
		m_objective_accuracy = 0.01;
		m_peak.resize(rNumPeaks);		
		m_pre_peak.resize(rNumPeaks);
		m_ini_peak.resize(rNumPeaks);
		for (int i = 0; i < rNumPeaks; i++) {
			m_peak[i].resize(rDimNumber);
			m_pre_peak[i].resize(rDimNumber);
			m_ini_peak[i].resize(rDimNumber);
		}

		m_width.resize(rNumPeaks);
		m_height.resize(rNumPeaks);
		m_pre_height.resize(rNumPeaks);
		m_pre_width.resize(rNumPeaks);
		m_fitness.resize(rNumPeaks);
		m_width_severity.resize(rNumPeaks);
		m_height_severity.resize(rNumPeaks);

		m_flag_change.resize(rNumPeaks);
		m_flag_global_optima.resize(rNumPeaks);
		m_num_tracking.resize(rNumPeaks);
		m_height_order.resize(rNumPeaks);
		m_tracked.resize(rNumPeaks);

		m_time_linkage.resize(rNumPeaks);
		for (int i = 0; i < rNumPeaks; ++i) {
			m_tracked[i] = false;
			m_flag_global_optima[i] = false;
			m_flag_change[i] = true;
			m_height_order[i] = -1;
		}
		m_num_changing_peak = rNumPeaks;
	}

	void dynamic_continuous::set_num_change(real rRatio) {
		if (rRatio < 0 || rRatio>1) {
			throw myexcept("the ratio of changing peaks is invalid@dynamic_continuous::set_num_change");
		}

		m_ratio_changing_peak = rRatio;
		m_num_changing_peak = (int)(m_peak.size()*m_ratio_changing_peak) > 1 ? (int)(m_peak.size()*m_ratio_changing_peak) : 1;
		update_num_change();

		m_parameters["Changing peaks ratio"] = m_ratio_changing_peak;
	}

	void dynamic_continuous::update_num_change() {
		if (m_num_changing_peak == m_peak.size()) {
			for (int i = 0; i < m_peak.size(); i++) m_flag_change[i] = true;
			return;
		}
		std::vector<int> a(m_peak.size());
		for (int i = 0; i < m_peak.size(); ++i) a[i] = i;
		global::ms_global->m_uniform[caller::Problem]->shuffle(a.begin(), a.end());
		// make sure the global optimum changes always
		int gopt = 0;
		for (int i = 0; i < m_peak.size(); i++) {
			if (m_flag_global_optima[i]) {
				gopt = i;
				break;
			}
		}
		int gidx;
		for (int i = 0; i < m_peak.size(); i++) {
			if (a[i] == gopt) {
				gidx = i;
				break;
			}
		}
		int t = a[0];
		a[0] = a[gidx];
		a[gidx] = t;

		for (int i = 0; i < m_peak.size(); i++) m_flag_change[i] = false;
		for (int i = 0; i < m_num_changing_peak; i++) m_flag_change[a[i]] = true;
		
	}
	void dynamic_continuous::set_height_severity(const real rS) {
		for (int i = 0; i < m_peak.size(); i++) 	m_height_severity[i] = rS;
	}
	void dynamic_continuous::set_width_severity(const real rS) {
		for (int i = 0; i < m_peak.size(); i++) 	m_width_severity[i] = rS;
	}

	void dynamic_continuous::copy(const problem& rP) {
		dynamic::copy(rP);
		continuous::copy(rP);

		auto& dcp = dynamic_cast<const dynamic_continuous &>(rP);

		int dim = m_temp_dimension < rP.variable_size() ? m_temp_dimension : rP.variable_size();
		int peaks = m_peak.size() < dcp.m_peak.size() ? m_peak.size() : dcp.m_peak.size();

		for (int i = 0; i < peaks; i++) {
			std::copy(dcp.m_peak[i].begin(), dcp.m_peak[i].begin()+ dim, m_peak[i].begin());
			std::copy(dcp.m_pre_peak[i].begin(), dcp.m_pre_peak[i].begin() + dim, m_pre_peak[i].begin());
			std::copy(dcp.m_ini_peak[i].begin(), dcp.m_ini_peak[i].begin() + dim, m_ini_peak[i].begin());
		}
		std::copy(dcp.m_height.begin(), dcp.m_height.begin()+ peaks, m_height.begin());
		std::copy(dcp.m_width.begin(), dcp.m_width.begin() + peaks, m_width.begin());
		std::copy(dcp.m_pre_height.begin(), dcp.m_pre_height.begin() + peaks, m_pre_height.begin());
		std::copy(dcp.m_pre_width.begin(), dcp.m_pre_width.begin() + peaks, m_pre_width.begin());
		std::copy(dcp.m_fitness.begin(), dcp.m_fitness.begin() + peaks, m_fitness.begin());
		std::copy(dcp.m_flag_change.begin(), dcp.m_flag_change.begin() + peaks, m_flag_change.begin());

		m_min_height = dcp.m_min_height;
		m_max_height = dcp.m_max_height;

		m_min_width = dcp.m_min_width;
		m_max_width = dcp.m_max_width;

		std::copy(dcp.m_height_severity.begin(), dcp.m_height_severity.begin() + peaks, m_height_severity.begin());
		std::copy(dcp.m_width_severity.begin(), dcp.m_width_severity.begin() + peaks, m_width_severity.begin());
		std::copy(dcp.m_flag_global_optima.begin(), dcp.m_flag_global_optima.begin() + peaks, m_flag_global_optima.begin());

		m_current_peak = dcp.m_current_peak;

		m_ratio_changing_peak = dcp.m_ratio_changing_peak;
		m_num_changing_peak = (int)(m_ratio_changing_peak*peaks) > 1 ? (int)(m_ratio_changing_peak*peaks) : 1;//dcp.m_num_changing_peak;

		std::copy(dcp.m_num_tracking.begin(), dcp.m_num_tracking.begin() + peaks, m_num_tracking.begin());
		std::copy(dcp.m_height_order.begin(), dcp.m_height_order.begin() + peaks, m_height_order.begin());
		std::copy(dcp.m_tracked.begin(), dcp.m_tracked.begin() + peaks, m_tracked.begin());
		m_num_peak_tracked = dcp.m_num_peak_tracked;
		std::copy(dcp.m_time_linkage.begin(), dcp.m_time_linkage.begin() + peaks, m_time_linkage.begin());

	}

	void dynamic_continuous::calculate_global_optima() {

		real gobj;
		if (m_opt_mode[0] == optimization_mode::Maximization) gobj = *std::max_element(m_height.begin(), m_height.end());
		else gobj = *min_element(m_height.begin(), m_height.end());

		m_optima.clear();
		real mindis = std::numeric_limits<real>::max();
		for (int i = 0; i < m_peak.size(); i++) {
			m_flag_global_optima[i] = false;
			if (m_height[i] == gobj) {
				for (int j = 0; j < m_peak.size(); ++j) {
					if (j == i) continue;
					Vector s1(m_peak[i].vect()), s2(m_peak[j].vect());
					real dis = s1.distance(s2);
					if (mindis > dis) {
						mindis = dis;
					}
				}
				m_flag_global_optima[i] = true;
				m_optima.append(variable_vector<real>(m_peak[i]));
				m_optima.append(m_height[i]);
			}
		}

		if (m_name=="DYN_CONT_RotationDBG" || m_name == "moving_peak") {
			if (mindis / 2 < m_variable_accuracy)		m_variable_accuracy=(mindis / 2);
		}
		update_num_visable_peak();
		m_num_peak_tracked = 0;
		if (m_flag_time_linkage) update_time_linkage();
		for (int i = 0; i < m_peak.size(); i++) {
			m_height_order[i] = i;
			m_tracked[i] = false;
		}
		merge_sort(m_height, m_peak.size(), m_height_order);		
		m_height_order=amend_order(m_height, m_height_order);
	}

	void dynamic_continuous::set_height(const real *h) {
		std::copy(h, h + m_peak.size(), m_height.begin());
	}


	//const real **p

	void dynamic_continuous::set_location(const std::vector<std::vector<real>> &p) {
		for (int i = 0; i < m_peak.size(); i++) {
			m_peak[i].vect() = p[i];
		}
	}

	void dynamic_continuous::set_initial_location(const std::vector<std::vector<real>> &p) {
		for (int i = 0; i < m_peak.size(); i++) {
			m_ini_peak[i].vect() = m_peak[i].vect() = p[i];
		}
	}

	void dynamic_continuous::set_width(const real w) {
		for (int i = 0; i < m_peak.size(); i++)
			m_width[i] = w;
	}

	int dynamic_continuous::get_num_visible_peak() {
		return m_num_visable_peak;

	}
	void dynamic_continuous::update_num_visable_peak() {
		m_num_visable_peak = m_peak.size();
		for (int i = 0; i < m_peak.size(); i++) {
			if (!is_visible(i)) --m_num_visable_peak;
		}

	}
	bool dynamic_continuous::is_visible(const int rIdx) {
        solution<variable_vector<real>, real> s(m_objective_size, num_constraints(), m_variable_size);
        s.variable() = m_peak[rIdx];
        s.evaluate(false,caller::Problem);
		real height = s.objective(0);
		switch (m_opt_mode[0]) {
		case optimization_mode::Minimization:
			if (height < m_height[rIdx]) return false;
			break;
		case optimization_mode::Maximization:
			if (height > m_height[rIdx]) return false;
			break;
		}
		return true;

	}
	void dynamic_continuous::add_noise(real *x_) {
		for (int d = 0; d < m_variable_size; d++) {
			real x = x_[d];
			x += m_noise_severity*global::ms_global->m_normal[caller::Problem]->next();
			if (m_domain[d].limit.second < x) x = m_domain[d].limit.second;
			if (m_domain[d].limit.first > x)  x = m_domain[d].limit.first;
			x_[d] = x;
		}
	}

	bool dynamic_continuous::is_tracked(real *gen, real obj) {
		bool flag = false, movepeaks = false;
		for (int i = 0; i < m_peak.size(); i++) {
			real dis = 0, dis1 = fabs(obj - m_height[i]);
			for (int j = 0; j < m_variable_size; j++) dis += (gen[j] - m_peak[i][j])*(gen[j] - m_peak[i][j]);
			dis = sqrt(dis);
			if (dis <= m_variable_accuracy&&dis1 <= m_objective_accuracy) {
				// peak[i] assumed to be found
				int j = 0;
				while (m_height_order[j++] != i&&j < m_peak.size());
				if (!m_tracked[i]) {
					m_num_tracking[j - 1]++;
					m_tracked[i] = true;
					m_num_peak_tracked++;
					flag = true;
				}
			}
			if (dis < m_time_linkage_severity) {
				// move peak[i] to a near random position when it was tracked
				if (m_flag_time_linkage) {
					move_peak(i);
					update_time_linkage();
					movepeaks = true;
					m_flag_trigger_time_linkage = true;
				}
			}
		}
		if (movepeaks) {
#ifdef DEMON_OFEC
			calculateSamplePoints();
#endif
		}
		return flag;
	}
	int dynamic_continuous::get_num_peak_found() {
		return m_num_peak_tracked;
	}
	void dynamic_continuous::update_time_linkage() {
		if (!m_flag_time_linkage) return;
		real range;
		for (int j = 0; j < m_variable_size; j++) {
			range = fabs(real(m_domain[j].limit.second - m_domain[j].limit.first));
			m_time_linkage[j] = 0.2*range*(global::ms_global->m_uniform[caller::Problem]->next() - 0.5);
		}
	}
	void dynamic_continuous::move_peak( int idx) {
		if (idx < 0 || idx >= m_peak.size()) throw myexcept("index out of boundary @ dynamic_continuous::move_peak(const int idx)");
		for (int d = 0; d < m_variable_size; d++) {
			real x = m_peak[idx][d];
			x += m_time_linkage[d];
			if (m_domain[d].limit.second < x) x = m_domain[d].limit.second;
			if (m_domain[d].limit.first > x)  x = m_domain[d].limit.first;
			m_peak[idx][d] = x;
		}
	}

	bool dynamic_continuous::is_GO_tracked() {
		// the global optimum is assumed to be tracked if any one of the global optima is tracked	
		for (int i = 0; i < m_peak.size(); i++) {
			if (m_flag_global_optima[i] && m_tracked[i]) return true;
		}
		return false;
	}

	const std::vector<real>& dynamic_continuous::get_nearest_peak(const std::vector<real>& p) {
		int nearest = 0;
		Vector peak(m_peak[0].vect());
		real dis = peak.distance(p);
		for (int i = 1; i < m_peak.size(); i++) {
			std::copy(m_peak[i].begin(), m_peak[i].end(), peak.begin());
			real d = peak.distance(p);
			if (d < dis) {
				dis = d;
				nearest = i;
			}
		}
		return m_peak[nearest].vect();
	}


	void dynamic_continuous::update_parameters() {
		dynamic::update_parameters();
		continuous::update_parameters();

		m_parameters["Changing peaks ratio"]=m_ratio_changing_peak;
	}

	void dynamic_continuous::update_pre_peak() {
		m_pre_peak = m_peak;
		m_pre_height = m_height;
		m_pre_width = m_width;
	}
}
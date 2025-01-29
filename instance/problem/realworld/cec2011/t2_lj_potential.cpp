#include "t2_lj_potential.h"

namespace ofec::cec2011 {
	void LennardJonesPotential::addInputParameters() {
		m_input_parameters.add("number of variables", new RangedSizeT(m_number_variables, 3, 444, 30));
	}

	void LennardJonesPotential::initialize_(Environment* env) {
		Continuous::initialize_(env);
		if (m_number_variables < 3 || m_number_variables % 3 != 0) {
			throw Exception("The number of varialbes must be a multiple of 3.");
		}
		resizeVariable(m_number_variables);
		m_domain.setRange(0, 4, 0);
		m_domain.setRange(0, 4, 1);
		m_domain.setRange(0, OFEC_PI, 2);
		for (size_t i = 3; i < m_number_variables; ++i) {
			m_domain.setRange(-4 - floor((i - 3) / 3.0) / 4, 4 + floor((i - 3) / 3.0), i);
		}
		resizeObjective(1);
		m_optimize_mode[0] = OptimizeMode::kMinimize;
	}

	/*********************************************************************
	* Optimal Objective Value according to the Cambridge Cluster Database
	* http://doye.chem.ox.ac.uk/jon/structures/LJ/tables.150.html
	**********************************************************************/
	void LennardJonesPotential::updateOptima(Environment* env) {
		m_optima.reset(new Optima<VariableType>());
		size_t num_atoms = m_number_variables / 3 + 2;
		switch (num_atoms) {
			case   3: m_optima->appendObjective(  -3.000000); break;
			case   4: m_optima->appendObjective(  -6.000000); break;
			case   5: m_optima->appendObjective(  -9.103852); break;
			case   6: m_optima->appendObjective( -12.712062); break;
			case   7: m_optima->appendObjective( -16.505384); break;
			case   8: m_optima->appendObjective( -19.821489); break;
			case   9: m_optima->appendObjective( -24.113360); break;
			case  10: m_optima->appendObjective( -28.422532); break;
			case  11: m_optima->appendObjective( -32.765970); break;
			case  12: m_optima->appendObjective( -37.967600); break;
			case  13: m_optima->appendObjective( -44.326801); break;
			case  14: m_optima->appendObjective( -47.845157); break;
			case  15: m_optima->appendObjective( -52.322627); break;
			case  16: m_optima->appendObjective( -56.815742); break;
			case  17: m_optima->appendObjective( -61.317995); break;
			case  18: m_optima->appendObjective( -66.530949); break;
			case  19: m_optima->appendObjective( -72.659782); break;
			case  20: m_optima->appendObjective( -77.177043); break;
			case  21: m_optima->appendObjective( -81.684571); break;
			case  22: m_optima->appendObjective( -86.809782); break;
			case  23: m_optima->appendObjective( -92.844472); break;
			case  24: m_optima->appendObjective( -97.348815); break;
			case  25: m_optima->appendObjective(-102.372663); break;
			case  26: m_optima->appendObjective(-108.315616); break;
			case  27: m_optima->appendObjective(-112.873584); break;
			case  28: m_optima->appendObjective(-117.822402); break;
			case  29: m_optima->appendObjective(-123.587371); break;
			case  30: m_optima->appendObjective(-128.286571); break;
			case  31: m_optima->appendObjective(-133.586422); break;
			case  32: m_optima->appendObjective(-139.635524); break;
			case  33: m_optima->appendObjective(-144.842719); break;
			case  34: m_optima->appendObjective(-150.044528); break;
			case  35: m_optima->appendObjective(-155.756643); break;
			case  36: m_optima->appendObjective(-161.825363); break;
			case  37: m_optima->appendObjective(-167.033672); break;
			case  38: m_optima->appendObjective(-173.928427); break;
			case  39: m_optima->appendObjective(-180.033185); break;
			case  40: m_optima->appendObjective(-185.249839); break;
			case  41: m_optima->appendObjective(-190.536277); break;
			case  42: m_optima->appendObjective(-196.277534); break;
			case  43: m_optima->appendObjective(-202.364664); break;
			case  44: m_optima->appendObjective(-207.688728); break;
			case  45: m_optima->appendObjective(-213.784862); break;
			case  46: m_optima->appendObjective(-220.680330); break;
			case  47: m_optima->appendObjective(-226.012256); break;
			case  48: m_optima->appendObjective(-232.199529); break;
			case  49: m_optima->appendObjective(-239.091864); break;
			case  50: m_optima->appendObjective(-244.549926); break;
			case  51: m_optima->appendObjective(-251.253964); break;
			case  52: m_optima->appendObjective(-258.229991); break;
			case  53: m_optima->appendObjective(-265.203016); break;
			case  54: m_optima->appendObjective(-272.208631); break;
			case  55: m_optima->appendObjective(-279.248470); break;
			case  56: m_optima->appendObjective(-283.643105); break;
			case  57: m_optima->appendObjective(-288.342625); break;
			case  58: m_optima->appendObjective(-294.378148); break;
			case  59: m_optima->appendObjective(-299.738070); break;
			case  60: m_optima->appendObjective(-305.875476); break;
			case  61: m_optima->appendObjective(-312.008896); break;
			case  62: m_optima->appendObjective(-317.353901); break;
			case  63: m_optima->appendObjective(-323.489734); break;
			case  64: m_optima->appendObjective(-329.620147); break;
			case  65: m_optima->appendObjective(-334.971532); break;
			case  66: m_optima->appendObjective(-341.110599); break;
			case  67: m_optima->appendObjective(-347.252007); break;
			case  68: m_optima->appendObjective(-353.394542); break;
			case  69: m_optima->appendObjective(-359.882566); break;
			case  70: m_optima->appendObjective(-366.892251); break;
			case  71: m_optima->appendObjective(-373.349661); break;
			case  72: m_optima->appendObjective(-378.637253); break;
			case  73: m_optima->appendObjective(-384.789377); break;
			case  74: m_optima->appendObjective(-390.908500); break;
			case  75: m_optima->appendObjective(-397.492331); break;
			case  76: m_optima->appendObjective(-402.894866); break;
			case  77: m_optima->appendObjective(-409.083517); break;
			case  78: m_optima->appendObjective(-414.794401); break;
			case  79: m_optima->appendObjective(-421.810897); break;
			case  80: m_optima->appendObjective(-428.083564); break;
			case  81: m_optima->appendObjective(-434.343643); break;
			case  82: m_optima->appendObjective(-440.550425); break;
			case  83: m_optima->appendObjective(-446.924094); break;
			case  84: m_optima->appendObjective(-452.657214); break;
			case  85: m_optima->appendObjective(-459.055799); break;
			case  86: m_optima->appendObjective(-465.384493); break;
			case  87: m_optima->appendObjective(-472.098165); break;
			case  88: m_optima->appendObjective(-479.032630); break;
			case  89: m_optima->appendObjective(-486.053911); break;
			case  90: m_optima->appendObjective(-492.433908); break;
			case  91: m_optima->appendObjective(-498.811060); break;
			case  92: m_optima->appendObjective(-505.185309); break;
			case  93: m_optima->appendObjective(-510.877688); break;
			case  94: m_optima->appendObjective(-517.264131); break;
			case  95: m_optima->appendObjective(-523.640211); break;
			case  96: m_optima->appendObjective(-529.879146); break;
			case  97: m_optima->appendObjective(-536.681383); break;
			case  98: m_optima->appendObjective(-543.665361); break;
			case  99: m_optima->appendObjective(-550.666526); break;
			case 100: m_optima->appendObjective(-557.039820); break;
			case 101: m_optima->appendObjective(-563.411308); break;
			case 102: m_optima->appendObjective(-569.363652); break;
			case 103: m_optima->appendObjective(-575.766131); break;
			case 104: m_optima->appendObjective(-582.086642); break;
			case 105: m_optima->appendObjective(-588.266501); break;
			case 106: m_optima->appendObjective(-595.061072); break;
			case 107: m_optima->appendObjective(-602.007110); break;
			case 108: m_optima->appendObjective(-609.033011); break;
			case 109: m_optima->appendObjective(-615.411166); break;
			case 110: m_optima->appendObjective(-621.788224); break;
			case 111: m_optima->appendObjective(-628.068416); break;
			case 112: m_optima->appendObjective(-634.874626); break;
			case 113: m_optima->appendObjective(-641.794704); break;
			case 114: m_optima->appendObjective(-648.833100); break;
			case 115: m_optima->appendObjective(-655.756307); break;
			case 116: m_optima->appendObjective(-662.809353); break;
			case 117: m_optima->appendObjective(-668.282701); break;
			case 118: m_optima->appendObjective(-674.769635); break;
			case 119: m_optima->appendObjective(-681.419158); break;
			case 120: m_optima->appendObjective(-687.021982); break;
			case 121: m_optima->appendObjective(-693.819577); break;
			case 122: m_optima->appendObjective(-700.939379); break;
			case 123: m_optima->appendObjective(-707.802109); break;
			case 124: m_optima->appendObjective(-714.920896); break;
			case 125: m_optima->appendObjective(-721.303235); break;
			case 126: m_optima->appendObjective(-727.349853); break;
			case 127: m_optima->appendObjective(-734.479629); break;
			case 128: m_optima->appendObjective(-741.332100); break;
			case 129: m_optima->appendObjective(-748.460647); break;
			case 130: m_optima->appendObjective(-755.271073); break;
			case 131: m_optima->appendObjective(-762.441558); break;
			case 132: m_optima->appendObjective(-768.042203); break;
			case 133: m_optima->appendObjective(-775.023203); break;
			case 134: m_optima->appendObjective(-782.206157); break;
			case 135: m_optima->appendObjective(-790.278120); break;
			case 136: m_optima->appendObjective(-797.453259); break;
			case 137: m_optima->appendObjective(-804.631473); break;
			case 138: m_optima->appendObjective(-811.812780); break;
			case 139: m_optima->appendObjective(-818.993848); break;
			case 140: m_optima->appendObjective(-826.174676); break;
			case 141: m_optima->appendObjective(-833.358586); break;
			case 142: m_optima->appendObjective(-840.538610); break;
			case 143: m_optima->appendObjective(-847.721698); break;
			case 144: m_optima->appendObjective(-854.904499); break;
			case 145: m_optima->appendObjective(-862.087012); break;
			case 146: m_optima->appendObjective(-869.272573); break;
			case 147: m_optima->appendObjective(-876.461207); break;
			case 148: m_optima->appendObjective(-881.072971); break;
			case 149: m_optima->appendObjective(-886.693405); break;
			case 150: m_optima->appendObjective(-893.310258); break;
			default: break;
		}
	}

	void LennardJonesPotential::evaluateObjective(Real *x, std::vector<Real> &obj) const {
		Real v = 0;
		size_t num_atoms = m_number_variables / 3 + 2;
		std::vector<std::array<Real, 3>> pos(num_atoms);
		pos[0] = { 0,0,0 };
		pos[1] = { x[0], 0, 0 };
		pos[2] = { x[1], x[2], 0 };
		for (size_t i = 3; i < num_atoms; ++i) {
			pos[i] = { x[(i - 2) * 3], x[(i - 2) * 3 + 1] , x[(i - 2) * 3 + 2] };
		}
		for (size_t i = 0; i < num_atoms - 1; ++i) {
			for (size_t j = i + 1; j < num_atoms; ++j) {
				Real r = 0;
				for (size_t k = 0; k < 3; ++k) {
					r += pow(pos[i][k] - pos[j][k], 2);
				}
				r = sqrt(r);
				v += pow(r, -12) - 2 * pow(r, -6);
			}
		}
		obj[0] = v;
	}
}